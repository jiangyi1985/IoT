/* 
 * 连接方式
 * SHTC3    NodeMcu-32S   NodeMcu-8266  BW16kit-v1.0  TM1637
 * SDA      GPIO21        GPIO04        PA26
 * SCL      GPIO22        GPIO05        PA25
 * +        3.3V          3.3V          3.3V
 *                                      2(PA27)       DIO
 *                                      9(PA15)       CLK
 */

// #include <SparkFun_SHTC3.h>
#include "Adafruit_SHTC3.h"
//#include <jm_LCM2004A_I2C.h>
#include "TM1637.h"
#define BLINKER_PRINT Serial
#define BLINKER_WIFI
#include <Blinker.h>

// SHTC3 mySHTC3;
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();
const int CLK = 17;  //bw16 PA15
const int DIO = 16;  //bw16 PA27
TM1637 tm1637(CLK, DIO);
const int PIN_BUZZ = 32;  //low v trigger buzzer

char auth[] = "xxxxxxxxxxxx";  //上一步中在app中获取到的Secret Key
char ssid[] = "xxxxxxxx";      //您的WiFi热点名称
char pswd[] = "xxxxxxxx";      //您的WiFi密码

BlinkerNumber NumberTemp("num-temp");  //defines a blinker number
BlinkerNumber NumberRh("num-rh");

void setup() {                 //初始化函数，只在程序开始时运行一次
  Serial.begin(115200);        //设置串口波特率
  while (Serial == false) {};  //等待串行连接启动

#if defined(BLINKER_PRINT)
  BLINKER_DEBUG.stream(BLINKER_PRINT);
#endif

  // Wire.begin();           //初始化Wire（IIC）库

  pinMode(PIN_BUZZ, OUTPUT);
  digitalWrite(PIN_BUZZ, HIGH);

  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);  //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

  // errorDecoder(mySHTC3.begin());// To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)
  if (!shtc3.begin()) {
    Serial.println("Couldn't find SHTC3");
    while (1) delay(1);
  } else Serial.println("Found SHTC3 sensor");

  Blinker.begin(auth, ssid, pswd);         // start blinker wifi mode
  Blinker.attachHeartbeat(heartbeat);      // app定时向设备发送心跳包, 设备收到心跳包后会返回设备当前状态，如果用户有自定义状态需要在收到心跳包时返回, 可调用该函数
  Blinker.attachDataStorage(dataStorage);  //关联回调函数，开启历史数据存储功能
}

float RH, T;
bool isT = true;
bool isNotify = false;

void loop() {
  sensors_event_t humidity, temp;
  delay(1000);  //延时1000毫秒

  // SHTC3_Status_TypeDef result = mySHTC3.update();
  shtc3.getEvent(&humidity, &temp);  // populate temp and humidity objects with fresh data
                                     // if(mySHTC3.lastStatus == SHTC3_Status_Nominal)   //判断SHTC3状态是否正常
                                     // {
  // RH = mySHTC3.toPercent();   //读取湿度数据
  // T = mySHTC3.toDegC();       //读取温度数据
  T = temp.temperature;
  RH = humidity.relative_humidity;

  if (T >= 26) isNotify = true;
  else isNotify = false;

  Serial.print("Humidity:");  //向串口打印 Humidity:
  Serial.print(RH);           //向串口打印湿度数据
  Serial.print("%");
  Serial.print("  Temperature:");
  Serial.print(T);  //向串口打印温度数据
  Serial.println("C");
  // }else{
  //   Serial.print("Update failed, error: ");
  //   errorDecoder(mySHTC3.lastStatus); //输出错误原因
  //   Serial.println();
  // }

  if (isT)
    tm1637.displayNum(T, 2);
  else
    tm1637.displayNum(RH, 2);
  isT = !isT;

  if (isNotify) {
    digitalWrite(PIN_BUZZ, LOW);
    delay(100);
    digitalWrite(PIN_BUZZ, HIGH);
  }

  Blinker.run();  //此函数需要频繁调用以保持设备间连接及处理收到的数据, 建议放在 loop() 函数中
}

// void errorDecoder(SHTC3_Status_TypeDef message) // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
// {
//   switch(message)
//   {
//     case SHTC3_Status_Nominal : Serial.print("Nominal"); break;
//     case SHTC3_Status_Error : Serial.print("Error"); break;
//     case SHTC3_Status_CRC_Fail : Serial.print("CRC Fail"); break;
//     case SHTC3_Status_ID_Fail : Serial.print("Check ID Fail"); break;
//     default : Serial.print("Unknown return code"); break;
//   }
//   Serial.println();
// }

void dataStorage() {                   //在回调函数中，设定要存储的键名和值
  Blinker.dataStorage("num-temp", T);  //键值可以跟blinker number一样或者不一样
  Blinker.dataStorage("num-rh", RH);
  // if (srawVoc != 0) Blinker.dataStorage("num-voc-raw", srawVoc);
  // if (srawNox != 0) Blinker.dataStorage("num-nox-raw", srawNox);
  // Blinker.dataStorage("num-voc", voc_index_value);
  // Blinker.dataStorage("num-nox", nox_index_value);
}

void heartbeat() {
  // Serial.println("Blinker heartbeat received.");
  // Blinker.print("temp", T, "°C");
  NumberTemp.print(T);  // send blinker number data to app
  NumberRh.print(RH);
  // NumberVocRaw.print(srawVoc);
  // NumberNoxRaw.print(srawNox);
  // NumberVoc.print(voc_index_value);
  // NumberNox.print(nox_index_value);
}
