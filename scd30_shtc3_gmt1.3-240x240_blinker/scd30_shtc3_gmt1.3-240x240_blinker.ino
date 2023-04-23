//TFT80x160 NodeMCU-32S
//SCL       18
//SDA       23
//RST       16
//DC        17
//CS
//BLK       

// scd30    nodemcu
// SDA      GPIO21
// SCL      GPIO22
// +        5V (3v时读取数据很慢)

/* 
 * SHTC3    NodeMcu-32S
 * SDA      GPIO21
 * SCL      GPIO22
 * +        3.3V
 */

#include <Adafruit_GFX.h>    // Core graphics library //https://github.com/adafruit/Adafruit-GFX-Library
#include "Adafruit_ST7789.h" // Hardware-specific library for ST7789
#include <SPI.h>


#include <SparkFun_SHTC3.h>//https://github.com/sparkfun/SparkFun_SHTC3_Arduino_Library


#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"

#define BLINKER_WIFI
#include <Blinker.h>

SCD30 airSensor;
SHTC3 mySHTC3;

char auth[] = "xxxxxxxxx"; //上一步中在app中获取到的Secret Key
char ssid[] = "xxxxxxxxx"; //您的WiFi热点名称
char pswd[] = "xxxxxxxxx"; //您的WiFi密码

BlinkerNumber NumberTemp("num-temp"); // defines a blinker number
BlinkerNumber NumberRh("num-rh");
BlinkerNumber NumberCo2("num-co2");


#define NODEMCU32S
#if defined(NODEMCU32S)
  #define TFT_CS         SS
  #define TFT_RST        16
  #define TFT_DC         17
#elif defined(ESP8266)
  #define TFT_CS         4
  #define TFT_RST        16                                            
  #define TFT_DC         5
#else
  // For the breakout board, you can use any 2 or 3 pins.
  // These pins will also work for the 1.8" TFT shield.
  #define TFT_CS        10
  #define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
  #define TFT_DC         8
#endif

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


void setup(void) {
  Serial.begin(115200);
  while (Serial == false)
  {
  };


  Wire.begin();


  tft.init(240, 240 ,SPI_MODE2);//SPI data mode; one of SPI_MODE0, SPI_MODE1, SPI_MODE2 or SPI_MODE3 (do NOT pass the numbers 0,1,2 or 3 -- use the defines only, the values are NOT the same!)
  tft.setRotation(2);

  tft.fillScreen(ST77XX_BLACK);


  errorDecoder(mySHTC3.begin());// To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)
  Serial.print("mySHTC3.passIDcrc:");Serial.println(mySHTC3.passIDcrc);


  Serial.print("TFT_CS:");Serial.println(TFT_CS);
  Serial.print("TFT_DC:");Serial.println(TFT_DC);
  Serial.print("TFT_RST:");Serial.println(TFT_RST);
  Serial.println();


  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }


  Blinker.begin(auth, ssid, pswd);        // start blinker wifi mode
  Blinker.attachHeartbeat(heartbeat);     // app定时向设备发送心跳包, 设备收到心跳包后会返回设备当前状态，如果用户有自定义状态需要在收到心跳包时返回, 可调用该函数
  //Blinker.attachDataStorage(dataStorage); //关联回调函数，开启历史数据存储功能
}

float RH_scd30=0, T_scd30=0, RH=0,T=0;
int CO2=0;

void loop() {
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
      T_scd30 = t;
      RH_scd30 = rh;

      Serial.print("co2(ppm):");
      Serial.print(CO2);

      Serial.print(" temp(C):");
      Serial.print(T_scd30);

      Serial.print(" humidity(%):");
      Serial.print(RH_scd30);

      Serial.println();
    }
  }
  else
    Serial.println(".");


  SHTC3_Status_TypeDef result = mySHTC3.update();
  if(mySHTC3.lastStatus == SHTC3_Status_Nominal)   //判断SHTC3状态是否正常
  {
    RH = mySHTC3.toPercent();   //读取湿度数据
    T = mySHTC3.toDegC();       //读取温度数据

    Serial.print("Humidity:");  //向串口打印 Humidity:
    Serial.print(RH);           //向串口打印湿度数据
    Serial.print("%");
    Serial.print("  Temperature:");
    Serial.print(T);            //向串口打印温度数据
    Serial.print("C"); 
    Serial.print(" passRHcrc:");    Serial.print(mySHTC3.passRHcrc);
    Serial.print(" passTcrc:");    Serial.print(mySHTC3.passTcrc);
    Serial.println();
  }
  else{
    Serial.print("Update failed, error: ");
    errorDecoder(mySHTC3.lastStatus); //输出错误原因
    Serial.println();
  }
  
  
  tft.setCursor(0, 0);
  tft.setTextWrap(true);
  tft.fillScreen(ST77XX_BLACK);


  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("Temp (`C):");
  tft.println();
  tft.setTextSize(1);
  tft.println();
  tft.setTextSize(7);
  tft.setTextColor(0xFE1A);
  tft.println(T);

  // tft.setTextSize(1);
  // tft.println();
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("RH (%): ");
  tft.println();
  tft.setTextSize(1);
  tft.println();
  tft.setTextSize(7);
  tft.setTextColor(0xBF1F);
  tft.println(RH);

  // tft.setTextSize(1);
  // tft.println();
  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);
  tft.print("CO2 (ppm): ");
  tft.println();
  tft.setTextSize(1);
  tft.println();
  tft.setTextSize(7);
  tft.setTextColor(0xE6D2);
  tft.println(CO2);


  Blinker.delay(1000);

  Blinker.run();//此函数需要频繁调用以保持设备间连接及处理收到的数据, 建议放在 loop() 函数中
}

void dataStorage()
{                                     //在回调函数中，设定要存储的键名和值
  //if(T>0) Blinker.dataStorage("num-temp", T); //键值可以跟blinker number一样或者不一样
  //Blinker.dataStorage("num-rh", RH);
  //if(CO2>0) Blinker.dataStorage("num-co2", CO2);
}

void heartbeat()
{
  Serial.println("Blinker heartbeat received.");
  Blinker.print("temp", T, "°C");
  if(T>0) NumberTemp.print(T); // send blinker number data to app
  if(RH>0) NumberRh.print(RH);
  if(CO2>0) NumberCo2.print(CO2);
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