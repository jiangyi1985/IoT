/* 
 * ====================================================================
 *                 NODE MCU-32S SYSTEM WIRING GUIDE
 * ====================================================================
 * 
 * 1. GMT1.3 240x240 TFT DISPLAY (SPI Interface)
 * --------------------------------------------------------------------
 * SCREEN PIN   ->  ESP32 PIN    | NOTES
 * SCL (CLK)    ->  GPIO18       | Hardware VSPI Clock (Fixed)
 * SDA (MOSI)   ->  GPIO23       | Hardware VSPI MOSI (Fixed)
 * RST (RESET)  ->  GPIO16       | Configurable (TFT_RST)
 * DC (RS)      ->  GPIO17       | Configurable (TFT_DC)
 * VCC          ->  3.3V         | 
 * GND          ->  GND          | 
 * 
 * 2. SENSORS (Shared I2C Bus Interface)
 * --------------------------------------------------------------------
 * SENSOR PIN   ->  ESP32 PIN    | NOTES
 * SCD30 SDA    ->  GPIO21       | Default I2C Data Pin
 * SCD30 SCL    ->  GPIO22       | Default I2C Clock Pin
 * SCD30 VIN(+) ->  5V           | IMPORTANT: Needs 5V!
 * SCD30 GND(-) ->  GND          | Connect to common system ground
 * 
 * PMSA003I PIN 9 (SDA) -> GPIO21| Connects to same I2C data line
 * PMSA003I PIN 7 (SCL) -> GPIO22| Connects to same I2C clock line
 * PMSA003I VCC         -> 5V    | Connect either Pin 1 or Pin 2
 * PMSA003I GND         -> GND   | Connect either Pin 3 or Pin 4
 * ====================================================================
 */

#include "Adafruit_PM25AQI.h"
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"

// Screen libraries
#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_ST7789.h" // Hardware-specific library for ST7789
#include <SPI.h>

#define BLINKER_WIFI
#include <Blinker.h>

// Sensor instances
SCD30 airSensor;
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

// Blinker Credentials
char auth[] = "xxxxxxxxxxxx"; // Secret Key
char ssid[] = "xxxxxxxxxxxx"; // WiFi SSID
char pswd[] = "xxxxxxxxxxxx"; // WiFi Password

// Blinker Numbers Mapping
BlinkerNumber NumberTemp("num-temp");
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

// Screen Pin Mapping
#define NODEMCU32S
#if defined(NODEMCU32S)
  #define TFT_CS         SS
  #define TFT_RST        16
  #define TFT_DC         17
#endif

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// Global Variables
float RH = 0, T = 0; 
int CO2 = 0, PART03 = 0, PART05 = 0, PART10 = 0, PART25 = 0, PART50 = 0, PART100 = 0;
int PM25CONCenv = 0, PM100CONCenv = 0, PM25AQIus = 0, PM100AQIus = 0, PM25AQIchina = 0, PM100AQIchina = 0;

// Variables for display timing state machine
unsigned long lastDisplayToggle = 0;
bool showFullScreenPM25 = true; // Toggle flag

void setup() {
  Serial.begin(115200);
  while(Serial == false){};
  
  // Power stabilization delay for cold boots
  delay(1000); 
  
  Wire.begin();

  // Screen Initialization
  tft.init(240, 240, SPI_MODE2);
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);

  // SCD30 Setup
  delay(500);
  while (airSensor.begin() == false)
  {
      Serial.println("CO2 sensor not detected. retrying...");
      delay(500);
  }
  
  // PMSA003I Setup
  delay(500);
  while (! aqi.begin_I2C()) {      
    Serial.println("PM 2.5 sensor not detected. retrying...");
    delay(500);
  }

  // Blinker Setup
  Blinker.begin(auth, ssid, pswd);
  Blinker.attachHeartbeat(heartbeat);
  Blinker.attachDataStorage(dataStorage);
}

void loop() {
  // 1. Gather SCD30 data
  if (airSensor.dataAvailable())
  {
      int co2 = airSensor.getCO2();
      float t = airSensor.getTemperature();
      float rh = airSensor.getHumidity();
      
      if(co2 == 0 || t == 0 || rh == 0){
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
  
  // 2. Gather PMSA003I data
  PM25_AQI_Data data;  
  if (aqi.read(&data)) {
    PART03 = data.particles_03um;
    PART05 = data.particles_05um;
    PART10 = data.particles_10um;
    PART25 = data.particles_25um;
    PART50 = data.particles_50um;
    PART100 = data.particles_100um;
    PM25CONCenv = data.pm25_env;
    PM100CONCenv = data.pm100_env;
    PM25AQIus = data.aqi_pm25_us;
    PM100AQIus = data.aqi_pm100_us;
    PM25AQIchina = data.aqi_pm25_china;
    PM100AQIchina = data.aqi_pm100_china;

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
  }else {
    Serial.println("pmsa003i no data...");
    //printInfo(data);
  }

  // 3. Alternate display view exactly every 2000ms (2 second)
  if (millis() - lastDisplayToggle >= 2000) {
    lastDisplayToggle = millis();
    
    tft.setCursor(0, 0);
    tft.setTextWrap(true);
    tft.fillScreen(ST77XX_BLACK);

    if (showFullScreenPM25) {
      // --- VIEW A: GIANT FULL SCREEN PM2.5 US ---
      tft.setTextSize(3);
      tft.setTextColor(ST77XX_WHITE);
      tft.println(); // Leave a small top gap
      tft.println("PM2.5 AQI");
      tft.println("(US Standard)");
      tft.println();
      
      tft.setTextSize(13);        // Maximize font scale for full screen display
      tft.setTextColor(getAQIColor(PM25AQIus));  // color based on value
      tft.println(PM25AQIus);
    } 
    else {
      // --- VIEW B: THE 3-VALUE SPLIT LAYOUT ---
      // Render PM2.5 US AQI
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE);
      tft.print("PM2.5 AQI (US):");
      tft.println();
      tft.setTextSize(1); tft.println();
      tft.setTextSize(6); 
      tft.setTextColor(getAQIColor(PM25AQIus)); // color based on value
      tft.println(PM25AQIus); 

      // Render PM2.5 China AQI
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE);
      tft.print("PM2.5 AQI (CN): ");
      tft.println();
      tft.setTextSize(1); tft.println();
      tft.setTextSize(6);
      tft.setTextColor(getAQIColor(PM25AQIchina)); // color based on value
      tft.println(PM25AQIchina);

      // Render CO2
      tft.setTextSize(2);
      tft.setTextColor(ST77XX_WHITE);
      tft.print("CO2 (ppm): ");
      tft.println();
      tft.setTextSize(1); tft.println();
      tft.setTextSize(6);
      tft.setTextColor(0xE6D2); // Yellow-Green
      tft.println(CO2);
    }
    
    showFullScreenPM25 = !showFullScreenPM25; // Flip flag for the next second
  }

  // Blinker system polling tasks
  Blinker.delay(2000);
  Blinker.run();
}

void dataStorage() {
   Blinker.dataStorage("num-co2", CO2);
   Blinker.dataStorage("num-pm25aqi-us", PM25AQIus);
}

void heartbeat() {
    NumberTemp.print(T);
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

// Helper function to return RGB565 code based on the chart image
uint16_t getAQIColor(int aqiValue) {
  if (aqiValue <= 50)  return 0x4426; // Good (Green)
  if (aqiValue <= 100) return 0xF64A; // Moderate (Yellow)
  if (aqiValue <= 150) return 0xDD06; // Unhealthy for Sensitive Groups (Orange)
  if (aqiValue <= 200) return 0xB104; // Unhealthy (Red/Crimson)
  if (aqiValue <= 300) return 0x6012; // Very Unhealthy (Purple)
  return 0x5802;                      // 300+ Hazardous (Dark Maroon)
}
