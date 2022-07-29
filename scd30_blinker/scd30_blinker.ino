// scd30    nodemcu
// SDA      GPIO21
// SCL      GPIO22
// +        5V

#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"

#define BLINKER_WIFI
#include <Blinker.h>

SCD30 airSensor;

char auth[] = "xxxxxxxxxxxx"; //上一步中在app中获取到的Secret Key
char ssid[] = "xxxxxxxx"; //您的WiFi热点名称
char pswd[] = "xxxxxxxx"; //您的WiFi密码

BlinkerNumber NumberTemp("num-temp"); // defines a blinker number
BlinkerNumber NumberRh("num-rh");
BlinkerNumber NumberCo2("num-co2");

void setup()
{
  Serial.begin(115200);
  while (Serial == false)
  {
  };
  Wire.begin();

  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }
  // The SCD30 has data ready every two seconds

  Blinker.begin(auth, ssid, pswd);        // start blinker wifi mode
  Blinker.attachHeartbeat(heartbeat);     // app定时向设备发送心跳包, 设备收到心跳包后会返回设备当前状态，如果用户有自定义状态需要在收到心跳包时返回, 可调用该函数
  Blinker.attachDataStorage(dataStorage); //关联回调函数，开启历史数据存储功能
}

float RH, T;
int CO2;

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
    Serial.print(".");

    delay(500);
    
    Blinker.run();//此函数需要频繁调用以保持设备间连接及处理收到的数据, 建议放在 loop() 函数中
}

void dataStorage()
{                                     //在回调函数中，设定要存储的键名和值
  //Blinker.dataStorage("num-temp", T); //键值可以跟blinker number一样或者不一样
  //Blinker.dataStorage("num-rh", RH);
  //Blinker.dataStorage("num-co2", CO2);
}

void heartbeat()
{
  // Serial.println("Blinker heartbeat received.");
  // Blinker.print("temp", T, "°C");
  NumberTemp.print(T); // send blinker number data to app
  NumberRh.print(RH);
  NumberCo2.print(CO2);
}
