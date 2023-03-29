// scd30    bmp180(GY-68) NodeMcu-32S   NodeMcu-8266
// SDA      sda           GPIO21        GPIO04 //ai-thinker nodemcu8266 onboard rgb-R led will be lit
// SCL      scl           GPIO22        GPIO05 //ai-thinker nodemcu8266 onboard rgb-B led will be lit
// +(5/3.3) vcc           5V            5v
//          3.3v

#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"
#include <Adafruit_BMP085.h>
#define BLINKER_WIFI
#include <Blinker.h>

SCD30 airSensor;
Adafruit_BMP085 bmp;

char auth[] = "xxxxxxxxxxxx"; //上一步中在app中获取到的Secret Key
char ssid[] = "xxxxxxxx"; //您的WiFi热点名称
char pswd[] = "xxxxxxxx"; //您的WiFi密码

BlinkerNumber NumberTemp("num-temp"); // defines a blinker number
BlinkerNumber NumberRh("num-rh");
BlinkerNumber NumberCo2("num-co2");
BlinkerNumber NumberBmpP("num-bmpP");
BlinkerNumber NumberBmpT("num-bmpT");
BlinkerNumber NumberAtm("num-atm");

void setup()
{
    Serial.begin(115200);
    while (Serial == false)
    {
    };
    Wire.begin();
//    Wire.begin(12, 13); // on nodemcu8266 use gpio 12-sda 13-scl so that the rbg-R,B won't lit

    if (airSensor.begin() == false)
    {
        Serial.println("Air sensor not detected. Please check wiring. Freezing...");
        while (1)
            ;
    }
    // The SCD30 has data ready every two seconds

    if (!bmp.begin())
    {
        Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    }

    Blinker.begin(auth, ssid, pswd);        // start blinker wifi mode
    Blinker.attachHeartbeat(heartbeat);     // app定时向设备发送心跳包, 设备收到心跳包后会返回设备当前状态，如果用户有自定义状态需要在收到心跳包时返回, 可调用该函数
//    Blinker.attachDataStorage(dataStorage); //关联回调函数，开启历史数据存储功能
}

float RH, T;
int CO2;
float bmpT, bmpP, ATM;

void loop()
{
    if (airSensor.dataAvailable())
    {
        int co2 = airSensor.getCO2();
        float t = airSensor.getTemperature();
        float rh = airSensor.getHumidity();

        if (co2 == 0 || t == 0 || rh == 0)
        {
            Serial.println("scd30 got some 0 data...");
        }
        else
        {
            CO2 = co2;
            T = t;
            RH = rh;

            Serial.print("co2(ppm):");
            Serial.print(CO2);
            Serial.print(" temp(C):");
            Serial.print(T);
            Serial.print(" humidity(%):");
            Serial.print(RH);
            Serial.println();
        }
    }
    else
        Serial.println("no co2 data");

    bmpT = bmp.readTemperature();
    bmpP = bmp.readPressure();
    ATM = bmpP / 101325;
    Serial.print("bmpT:");
    Serial.print(bmpT);
    Serial.print("C");
    Serial.print(" bmpP:");
    Serial.print(bmpP);
    Serial.print("Pa");
    Serial.print(" ATM:");
    Serial.println(ATM);

    delay(1000);

    Blinker.run(); //此函数需要频繁调用以保持设备间连接及处理收到的数据, 建议放在 loop() 函数中
}

void dataStorage()
{ //在回调函数中，设定要存储的键名和值
    // Blinker.dataStorage("num-temp", T); //键值可以跟blinker number一样或者不一样
    // Blinker.dataStorage("num-rh", RH);
    Blinker.dataStorage("num-co2", CO2);
    Blinker.dataStorage("num-bmpP", bmpP);
    // Blinker.dataStorage("num-bmpT", bmpT);
}

void heartbeat()
{
    // Serial.println("Blinker heartbeat received.");
    // Blinker.print("temp", T, "°C");
    NumberTemp.print(T); // send blinker number data to app
    NumberRh.print(RH);
    NumberCo2.print(CO2);
    NumberBmpP.print(bmpP);
    NumberBmpT.print(bmpT);
    NumberAtm.print(ATM);
}
