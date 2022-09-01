/*************************************************** 
  This is an example for the SHT4x Humidity & Temp Sensor
  Designed specifically to work with the SHT4x sensor from Adafruit
  ----> https://www.adafruit.com/products/4885
  These sensors use I2C to communicate, 2 pins are required to  
  interface
 ****************************************************/

#include "Adafruit_SHT4x.h"//https://github.com/adafruit/Adafruit_SHT4X

Adafruit_SHT4x sht41 = Adafruit_SHT4x();
Adafruit_SHT4x sht40_1 = Adafruit_SHT4x();
Adafruit_SHT4x sht40_2 = Adafruit_SHT4x();
bool sht41_started=false;
bool sht40_1_started=false;
bool sht40_2_started=false;

#define TCA_ADDRESS 0x70

void setup() {
  Serial.begin(115200);
  while (!Serial)    delay(10);
  Wire.begin();

  Serial.println();
  
  TCA9548A(0);
  Serial.print("starting sht41  ...");
  sht41_started=InitSht4x(sht41);
  Serial.println();
  
  TCA9548A(4);
  Serial.print("starting sht40_1...");
  sht40_1_started=InitSht4x(sht40_1);
  Serial.println();
  
  TCA9548A(5);
  Serial.print("starting sht40_2...");
  sht40_2_started=InitSht4x(sht40_2);
  Serial.println();
}

void loop() {
  TCA9548A(0);
  if(sht41_started){
    Serial.print("sht41  :");
    GetData(sht41);
  }

  TCA9548A(4);
  if(sht40_1_started){
    Serial.print("sht40_1:");
    GetData(sht40_1);
  }
  
  TCA9548A(5);
  if(sht40_2_started){
    Serial.print("sht40_2:");
    GetData(sht40_2);
  }

  Serial.println();
  delay(1000);
}

bool InitSht4x(Adafruit_SHT4x &s)
{
  if (!s.begin(&Wire)) {
    Serial.println("Couldn't find SHT4x");
    return false;
  }
  else{
    Serial.println("Found SHT4x sensor");
    Serial.print("Serial number 0x");
    Serial.println(s.readSerial(), HEX);
    SetPrecision(s);
    SetHeater(s);
    return true;
  }
}

void GetData(Adafruit_SHT4x &s){
  sensors_event_t humidity, temp;
  
  uint32_t timestamp = millis();
  bool result = s.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  timestamp = millis() - timestamp;

  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.print(" degrees C\t");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.print("% rH\t");
  Serial.print("Read duration (ms): ");  Serial.println(timestamp);
}

void SetPrecision(Adafruit_SHT4x &s){
  // You can have 3 different precisions, higher precision takes longer
  s.setPrecision(SHT4X_HIGH_PRECISION);
  switch (s.getPrecision()) {
     case SHT4X_HIGH_PRECISION: 
       Serial.println("High precision");
       break;
     case SHT4X_MED_PRECISION: 
       Serial.println("Med precision");
       break;
     case SHT4X_LOW_PRECISION: 
       Serial.println("Low precision");
       break;
  }
}

void SetHeater(Adafruit_SHT4x &s){
  // You can have 6 different heater settings
  // higher heat and longer times uses more power
  // and reads will take longer too!
  s.setHeater(SHT4X_NO_HEATER);
  switch (s.getHeater()) {
     case SHT4X_NO_HEATER: 
       Serial.println("No heater");
       break;
     case SHT4X_HIGH_HEATER_1S: 
       Serial.println("High heat for 1 second");
       break;
     case SHT4X_HIGH_HEATER_100MS: 
       Serial.println("High heat for 0.1 second");
       break;
     case SHT4X_MED_HEATER_1S: 
       Serial.println("Medium heat for 1 second");
       break;
     case SHT4X_MED_HEATER_100MS: 
       Serial.println("Medium heat for 0.1 second");
       break;
     case SHT4X_LOW_HEATER_1S: 
       Serial.println("Low heat for 1 second");
       break;
     case SHT4X_LOW_HEATER_100MS: 
       Serial.println("Low heat for 0.1 second");
       break;
  }
}

// Select I2C BUS
void TCA9548A(uint8_t bus){
  Wire.beginTransmission(TCA_ADDRESS);  // TCA9548A address
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
}
