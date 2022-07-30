/* Test sketch for Adafruit PM2.5 sensor with UART or I2C */

#include "Adafruit_PM25AQI.h"//https://github.com/adafruit/Adafruit_PM25AQI

// If your PM2.5 is UART only, for UNO and others (without hardware serial) 
// we must use software serial...
// pin #2 is IN from sensor (TX pin on sensor), leave pin #3 disconnected
// comment these two lines if using hardware serial
//#include <SoftwareSerial.h>
//SoftwareSerial pmSerial(2, 3);

Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

void setup() {
  // Wait for serial monitor to open
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println("Adafruit PMSA003I Air Quality Sensor");

  // Wait one second for sensor to boot up!
  delay(1000);

  // If using serial, initialize it and set baudrate before starting!
  // Uncomment one of the following
  //Serial1.begin(9600);
  //pmSerial.begin(9600);

  // There are 3 options for connectivity!
  if (! aqi.begin_I2C()) {      // connect to the sensor over I2C
  //if (! aqi.begin_UART(&Serial1)) { // connect to the sensor over hardware serial
  //if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial 
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }

  Serial.println("PM25 found!");
}

void loop() {
  PM25_AQI_Data data;
  
  if (! aqi.read(&data)) {
    //Serial.println("Could not read from AQI");
    Serial.println();
    Serial.println("FAIL:");
    printInfo(data);
  
    delay(500);  // try again in a bit!
    return;
  }
  //Serial.println("AQI reading success"); 
  
  Serial.println();
  Serial.println(F("---------------------------------------"));
  Serial.println(F("Concentration Units (standard)"));//标准颗粒物质量浓度值是指用工业金属颗粒物作为等效颗粒进行密度换算得到的质量浓度值，适用于工业生产车间等环境。
  Serial.print(F("PM1.0: ")); Serial.print(data.pm10_standard); Serial.print("μg/m3");//PM1.0 浓度（CF=1，标准颗粒物）单位μg/m3
  Serial.print(F("\t\tPM2.5: ")); Serial.print(data.pm25_standard); Serial.print("μg/m3");//PM2.5 浓度（CF=1，标准颗粒物）单位μg/m3
  Serial.print(F("\t\tPM10: ")); Serial.print(data.pm100_standard); Serial.println("μg/m3");//PM10 浓度（CF=1，标准颗粒物）单位μg/m3
  Serial.println(F("---------------------------------------"));
  Serial.println(F("Concentration Units (environmental)"));//大气环境颗粒物质量浓度值以空气中主要污染物为等效颗粒进行密度换算，适用于普通室内外大气环境。
  Serial.print(F("PM1.0: ")); Serial.print(data.pm10_env); Serial.print("μg/m3");//PM1.0浓度（大气环境下）单位μg/m3
  Serial.print(F("\t\tPM2.5: ")); Serial.print(data.pm25_env); Serial.print("μg/m3");//PM2.5浓度（大气环境下）单位μg/m3
  Serial.print(F("\t\tPM10: ")); Serial.print(data.pm100_env); Serial.println("μg/m3");//PM10浓度 （大气环境下）单位μg/m3
  Serial.println(F("---------------------------------------"));
  Serial.println(F("AQI"));
  Serial.print(F("PM2.5 AQI US: ")); Serial.print(data.aqi_pm25_us);
  Serial.print(F("\t\tPM2.5 AQI China: ")); Serial.println(data.aqi_pm25_china);
  Serial.print(F("PM10  AQI US: ")); Serial.print(data.aqi_pm100_us);
  Serial.print(F("\t\tPM10  AQI China: ")); Serial.println(data.aqi_pm100_china);
  Serial.println(F("---------------------------------------"));
  Serial.print(F("Particles>0.3um / 0.1L air: ")); Serial.println(data.particles_03um);//0.1升空气中直径在0.3um以上颗粒物个数
  Serial.print(F("Particles>0.5um / 0.1L air: ")); Serial.println(data.particles_05um);//0.1升空气中直径在0.5um以上颗粒物个数
  Serial.print(F("Particles>1.0um / 0.1L air: ")); Serial.println(data.particles_10um);//0.1升空气中直径在1.0um以上颗粒物个数
  Serial.print(F("Particles>2.5um / 0.1L air: ")); Serial.println(data.particles_25um);//0.1升空气中直径在2.5um以上颗粒物个数
  Serial.print(F("Particles>5.0um / 0.1L air: ")); Serial.println(data.particles_50um);//0.1升空气中直径在5.0um以上颗粒物个数
  Serial.print(F("Particles>10 um / 0.1L air: ")); Serial.println(data.particles_100um);//0.1升空气中直径在10um以上颗粒物个数
  Serial.println(F("---------------------------------------"));
  //Serial.println();
  //printInfo(data);

  delay(1000);
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
