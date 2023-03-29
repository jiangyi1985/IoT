/*
 * SGP41    NodeMcu-32S   NodeMcu-8266
 * SDA      GPIO21        GPIO04 //ai-thinker nodemcu8266 onboard rgb-R led will be lit
 * SCL      GPIO22        GPIO05 //ai-thinker nodemcu8266 onboard rgb-B led will be lit
 * +        3.3V          3.3V
 */

#include <Arduino.h>
#include <SensirionI2CSgp41.h>
#include <Wire.h>
#include <SensirionGasIndexAlgorithm.h>
#include <SparkFun_SHTC3.h>
#define BLINKER_WIFI
#include <Blinker.h>

SensirionI2CSgp41 sgp41;
SHTC3 mySHTC3;
GasIndexAlgorithmParams paramsVoc;
GasIndexAlgorithmParams paramsNox;

char auth[] = "xxxxxxxxxxxx"; //上一步中在app中获取到的Secret Key
char ssid[] = "xxxxxxxx"; //您的WiFi热点名称
char pswd[] = "xxxxxxxx"; //您的WiFi密码

BlinkerNumber NumberTemp("num-temp"); // defines a blinker number
BlinkerNumber NumberRh("num-rh");
BlinkerNumber NumberVoc("num-voc");
BlinkerNumber NumberNox("num-nox");
BlinkerNumber NumberVocRaw("num-voc-raw");
BlinkerNumber NumberNoxRaw("num-nox-raw");

// time in seconds needed for NOx conditioning
uint16_t conditioning_s = 6; // should be 10 but we are doing some other things

void setup()
{
    // turn off onboard rgb led
    // pinMode(2,OUTPUT); //when setting gpio2 to output, rgb-G is off but the blue led near the antenna is lit...
    // digitalWrite(2,LOW);

    GasIndexAlgorithm_init(&paramsVoc, GasIndexAlgorithm_ALGORITHM_TYPE_VOC);
    GasIndexAlgorithm_init(&paramsNox, GasIndexAlgorithm_ALGORITHM_TYPE_NOX);

    Serial.begin(115200);
    while (!Serial)
    {
        delay(100);
    }

    Wire.begin();
    //Wire.begin(12, 13); // on nodemcu8266 use gpio 12-sda 13-scl so that the rbg-R,B won't lit

    errorDecoder(mySHTC3.begin());

    uint16_t error;
    char errorMessage[256];
    sgp41.begin(Wire);

    uint16_t serialNumber[3];
    uint8_t serialNumberSize = 3;
    error = sgp41.getSerialNumber(serialNumber, serialNumberSize);
    if (error)
    {
        Serial.print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
    else
    {
        Serial.print("SerialNumber:");
        Serial.print("0x");
        for (size_t i = 0; i < serialNumberSize; i++)
        {
            uint16_t value = serialNumber[i];
            Serial.print(value < 4096 ? "0" : "");
            Serial.print(value < 256 ? "0" : "");
            Serial.print(value < 16 ? "0" : "");
            Serial.print(value, HEX);
        }
        Serial.println();
    }

    uint16_t testResult;
    error = sgp41.executeSelfTest(testResult);
    if (error)
    {
        Serial.print("Error trying to execute executeSelfTest(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
    else if (testResult != 0xD400)
    {
        Serial.print("executeSelfTest failed with error: ");
        Serial.println(testResult);
    }

    Blinker.begin(auth, ssid, pswd);        // start blinker wifi mode
    Blinker.attachHeartbeat(heartbeat);     // app定时向设备发送心跳包, 设备收到心跳包后会返回设备当前状态，如果用户有自定义状态需要在收到心跳包时返回, 可调用该函数
    Blinker.attachDataStorage(dataStorage); //关联回调函数，开启历史数据存储功能
}

// RH/ticks=RH/%×65'535/100
uint16_t defaultRh = 0x8000; //---50%
// T/ticks=(T/°C+45)×65'535/175
uint16_t defaultT = 0x6666; //---25°C

float RH, T;
uint16_t srawVoc = 0;
uint16_t srawNox = 0;
int32_t voc_index_value;
int32_t nox_index_value;

void loop()
{

    delay(1000);

    SHTC3_Status_TypeDef result = mySHTC3.update();
    if (mySHTC3.lastStatus == SHTC3_Status_Nominal) //判断SHTC3状态是否正常
    {
        RH = mySHTC3.toPercent(); //读取湿度数据
        T = mySHTC3.toDegC();     //读取温度数据

        if (RH != 0 && T != 0)
        {
            defaultRh = RH * 65535 / 100;
            defaultT = (T + 45) * 65535 / 175;
        }

        Serial.print("Humidity:");
        Serial.print(RH);
        Serial.print("%");
        Serial.print("  Temperature:");
        Serial.print(T);
        Serial.println("C");
        Serial.print("RH tick:");
        Serial.print(defaultRh);
        Serial.print("  T tick:");
        Serial.println(defaultT);
    }
    else
    {
        Serial.print("Update failed, error: ");
        errorDecoder(mySHTC3.lastStatus); //输出错误原因
        Serial.println();
    }

    uint16_t error;
    char errorMessage[256];

    if (conditioning_s > 0)
    {
        // During NOx conditioning (10s) SRAW NOx will remain 0
        error = sgp41.executeConditioning(defaultRh, defaultT, srawVoc);
        conditioning_s--;
    }
    else
    {
        // Read Measurement
        error = sgp41.measureRawSignals(defaultRh, defaultT, srawVoc, srawNox);
    }

    if (error)
    {
        Serial.print("Error trying to execute measureRawSignals(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    }
    else
    {
        GasIndexAlgorithm_process(&paramsVoc, srawVoc, &voc_index_value);
        if (srawNox != 0)
            GasIndexAlgorithm_process(&paramsNox, srawNox, &nox_index_value);

        Serial.print("SRAW_VOC:");
        Serial.print(srawVoc);
        Serial.print("\t");
        Serial.print("SRAW_NOx:");
        Serial.print(srawNox);

        Serial.print("\t");
        Serial.print("VOC:");
        Serial.print(voc_index_value);
        if (srawNox != 0)
        {
            Serial.print("\t");
            Serial.print("NOX:");
            Serial.print(nox_index_value);
        }

        Serial.println();
    }

    Serial.println();
    Blinker.run(); //此函数需要频繁调用以保持设备间连接及处理收到的数据, 建议放在 loop() 函数中
}

void dataStorage()
{                                       //在回调函数中，设定要存储的键名和值
    Blinker.dataStorage("num-temp", T); //键值可以跟blinker number一样或者不一样
    Blinker.dataStorage("num-rh", RH);
    if (srawVoc != 0) Blinker.dataStorage("num-voc-raw", srawVoc);
    if (srawNox != 0) Blinker.dataStorage("num-nox-raw", srawNox);
    Blinker.dataStorage("num-voc", voc_index_value);
    Blinker.dataStorage("num-nox", nox_index_value);
}

void heartbeat()
{
    // Serial.println("Blinker heartbeat received.");
    // Blinker.print("temp", T, "°C");
    NumberTemp.print(T); // send blinker number data to app
    NumberRh.print(RH);
    NumberVocRaw.print(srawVoc);
    NumberNoxRaw.print(srawNox);
    NumberVoc.print(voc_index_value);
    NumberNox.print(nox_index_value);
}

void errorDecoder(SHTC3_Status_TypeDef message) // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
{
  switch(message)
  {
    case SHTC3_Status_Nominal : Serial.print("Nominal"); break;
    case SHTC3_Status_Error : Serial.print("Error"); break;
    case SHTC3_Status_CRC_Fail : Serial.print("CRC Fail"); break;
    case SHTC3_Status_ID_Fail : Serial.print("Check ID Fail"); break;
    default : Serial.print("Unknown return code"); break;
  }
  Serial.println();
}
