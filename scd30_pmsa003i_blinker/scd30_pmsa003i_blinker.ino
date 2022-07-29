#include "Adafruit_PM25AQI.h"
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"
#define BLINKER_WIFI
#include <Blinker.h>

SCD30 airSensor;
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

char auth[] = "xxxxxxxxxxxx"; //上一步中在app中获取到的Secret Key
char ssid[] = "xxxxxxxx"; //您的WiFi热点名称
char pswd[] = "xxxxxxxx"; //您的WiFi密码

BlinkerNumber NumberTemp("num-temp");//defines a blinker number
BlinkerNumber NumberRh("num-rh");
BlinkerNumber NumberCo2("num-co2");
BlinkerNumber NumberPart03("num-part03");
BlinkerNumber NumberPart05("num-part05");
BlinkerNumber NumberPart10("num-part10");
BlinkerNumber NumberPart25("num-part25");
BlinkerNumber NumberPart50("num-part50");
BlinkerNumber NumberPart100("num-part100");
BlinkerNumber NumberPm25ConcEnv("num-pm25conc-env");
BlinkerNumber NumberPm100ConcEnv("num-pm100conc-env");
BlinkerNumber NumberPm25aqiUS("num-pm25aqi-us");
BlinkerNumber NumberPm100aqiUS("num-pm100aqi-us");
BlinkerNumber NumberPm25aqiCN("num-pm25aqi-cn");
BlinkerNumber NumberPm100aqiCN("num-pm100aqi-cn");

void setup() {
  Serial.begin(115200);
  while(Serial == false){};
  Wire.begin();

      delay(500);
  while (airSensor.begin() == false)
  {
      Serial.println("CO2 sensor not detected. retrying...");
      delay(500);
  }
  
  delay(500);
  while (! aqi.begin_I2C()) {      // connect to the sensor over I2C
    Serial.println("PM 2.5 sensor not detected. retrying...");
    delay(500);
  }

    Blinker.begin(auth, ssid, pswd);//start blinker wifi mode
    Blinker.attachHeartbeat(heartbeat);//app定时向设备发送心跳包, 设备收到心跳包后会返回设备当前状态，如果用户有自定义状态需要在收到心跳包时返回, 可调用该函数

    //Blinker.attachDataStorage(func, time, times); func, 对应的回调函数
//    time, 对应数据采集时间-默认60 s, 最少60 s
//    times, 对应采集次数后上传到云端-默认2次, 最多不超过4次 
    Blinker.attachDataStorage(dataStorage);//关联回调函数，开启历史数据存储功能
}

float RH,T;
int CO2,PART03,PART05,PART10,PART25,PART50,PART100,PM25CONCenv,PM100CONCenv,PM25AQIus,PM100AQIus,PM25AQIchina,PM100AQIchina;
int seq=0;

void loop() {
  if (airSensor.dataAvailable())
    {
        int co2 = airSensor.getCO2();
        float t = airSensor.getTemperature();
        float rh = airSensor.getHumidity();
        
        if(co2==0 || t==0 || rh==0){
          Serial.println("scd30 0 data...");
        } else {
          CO2 = co2;
          T = t;
          RH = rh;
          
          Serial.println(F("***************************************"));
          Serial.print("co2(ppm):");        Serial.print(CO2);
          Serial.print("\ttemp(C):");        Serial.print(T);
          Serial.print("\thumidity(%):");        Serial.println(RH);
          Serial.println(F("***************************************"));
        }
    }
    else
        Serial.println("scd30 no data...");
  
  PM25_AQI_Data data;  
  if (aqi.read(&data)) {
    PART03=data.particles_03um;
    PART05=data.particles_05um;
    PART10=data.particles_10um;
    PART25=data.particles_25um;
    PART50=data.particles_50um;
    PART100=data.particles_100um;
    PM25CONCenv=data.pm25_env;
    PM100CONCenv=data.pm100_env;
    PM25AQIus=data.aqi_pm25_us;
    PM100AQIus=data.aqi_pm100_us;
    PM25AQIchina=data.aqi_pm25_china;
    PM100AQIchina=data.aqi_pm100_china;
    Serial.println(F("---------------------------------------"));
    Serial.print(F("PM1.0: ")); Serial.print(data.pm10_env); Serial.print("μg/m3");//PM1.0浓度（大气环境下）单位μg/m3
    Serial.print(F("\t\tPM2.5: ")); Serial.print(data.pm25_env); Serial.print("μg/m3");//PM2.5浓度（大气环境下）单位μg/m3
    Serial.print(F("\t\tPM10: ")); Serial.print(data.pm100_env); Serial.println("μg/m3");//PM10浓度 （大气环境下）单位μg/m3
    Serial.print(F("PM2.5 AQI US: ")); Serial.print(data.aqi_pm25_us);
    Serial.print(F("\tPM2.5 AQI China: ")); Serial.print(data.aqi_pm25_china);
    Serial.print(F("\tPM10  AQI US: ")); Serial.print(data.aqi_pm100_us);
    Serial.print(F("\tPM10  AQI China: ")); Serial.println(data.aqi_pm100_china);
    Serial.print(F("Particles/0.1L:\t>0.3um:")); Serial.print(data.particles_03um);    Serial.print(F("\t>0.5um:")); Serial.print(data.particles_05um);
    Serial.print(F("\t>1.0um:")); Serial.print(data.particles_10um);    Serial.print(F("\t>2.5um:")); Serial.print(data.particles_25um);
    Serial.print(F("\t>5.0um:")); Serial.print(data.particles_50um);    Serial.print(F("\t>10um:")); Serial.println(data.particles_100um);
    Serial.print(F("0.3~0.5μm:")); Serial.println(data.particles_03um-data.particles_05um);
    Serial.println(F("---------------------------------------"));
  } else {
    Serial.println("pmsa003i no data...");
    //printInfo(data);
  }

    Serial.println();
  delay(1000);

  Blinker.run();//此函数需要频繁调用以保持设备间连接及处理收到的数据, 建议放在 loop() 函数中
}

void dataStorage(){//在回调函数中，设定要存储的键名和值 //键值可以跟blinker number一样或者不一样
  //缓存数据最大为 256 字节
  //最多可以同时缓存 6 个不同的key 
  
  //免费用户只有5个存储key，用完就不能再存了，可以通过菜单"开发者"进去，删除现有的key

   Blinker.dataStorage("num-co2", CO2);
//   Blinker.dataStorage("num-temp", T);
//   Blinker.dataStorage("num-rh", RH);

//   Blinker.dataStorage("num-pm25conc-env", PM25CONCenv);
//   Blinker.dataStorage("num-pm100conc-env", PM100CONCenv);
   Blinker.dataStorage("num-pm25aqi-us", PM25AQIus);
//   Blinker.dataStorage("num-pm100aqi-us", PM100AQIus);
//   Blinker.dataStorage("num-pm25aqi-cn", PM25AQIchina);
//   Blinker.dataStorage("num-pm100aqi-cn", PM100AQIchina);
   
//   Blinker.dataStorage("num-part03", PART03);
//   Blinker.dataStorage("num-part05", PART05);
//   Blinker.dataStorage("num-part10", PART10);
//   Blinker.dataStorage("num-part25", PART25);
//   Blinker.dataStorage("num-part50", PART50);
//   Blinker.dataStorage("num-part100", PART100);

//   Blinker.dataStorage("num-part03-25", PART03-PART25);
//   Blinker.dataStorage("num-part03-05", PART03-PART05);
//   Blinker.dataStorage("num-part05-25", PART05-PART25);
//   Blinker.dataStorage("num-part25-100", PART25-PART100);
//   Blinker.dataStorage("num-part100", PART100);
}

void heartbeat(){
    NumberTemp.print(T);//send blinker number data to app
    NumberRh.print(RH);
    NumberCo2.print(CO2);
    NumberPart03.print(PART03);
    NumberPart05.print(PART05);
    NumberPart10.print(PART10);
    NumberPart25.print(PART25);
    NumberPart50.print(PART50);
    NumberPart100.print(PART100);
    NumberPm25ConcEnv.print(PM25CONCenv);
    NumberPm100ConcEnv.print(PM100CONCenv);
    NumberPm25aqiUS.print(PM25AQIus);
    NumberPm100aqiUS.print(PM100AQIus);
    NumberPm25aqiCN.print(PM25AQIchina);
    NumberPm100aqiCN.print(PM100AQIchina);
}

void printInfo(PM25_AQI_Data data)
{
  Serial.print(F("startbyte_fail: ")); Serial.println(data.startbyte_fail);
  Serial.print(F("checksum_fail: ")); Serial.println(data.checksum_fail);
  Serial.print(F("framelen: ")); Serial.println(data.framelen);
  Serial.print(F("version: ")); Serial.println(data.version);
  Serial.print(F("error_code: ")); Serial.println(data.error_code);
  Serial.print(F("checksum: ")); Serial.println(data.checksum);
  Serial.print(F("datasum: ")); Serial.println(data.datasum);
  Serial.println(F("raw data: "));
  for(int i = 0; i < sizeof(data.raw); i++)
  {
    Serial.print(data.raw[i]);
    Serial.print("\t");
    if((i+1)%2==0)
      Serial.println();
  }
}
