/*************************************************** 
  This is an example for the SHT4x Humidity & Temp Sensor
  Designed specifically to work with the SHT4x sensor from Adafruit
  ----> https://www.adafruit.com/products/4885
  These sensors use I2C to communicate, 2 pins are required to  
  interface
 ****************************************************/

#include "Adafruit_SHT4x.h"//https://github.com/adafruit/Adafruit_SHT4X

Adafruit_SHT4x sht41 = Adafruit_SHT4x();
Adafruit_SHT4x sht40 = Adafruit_SHT4x();

void setup() {
  Serial.begin(115200);

  while (!Serial)
    delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit SHT4x test");
  
  if (! sht41.begin(&Wire)) {
    Serial.println("Couldn't find SHT41");
    while (1) delay(1);
  }
  Serial.println("Found SHT41 sensor");
  Serial.print("Serial number 0x");
  Serial.println(sht41.readSerial(), HEX);

  // You can have 3 different precisions, higher precision takes longer
  sht41.setPrecision(SHT4X_HIGH_PRECISION);
  switch (sht41.getPrecision()) {
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

  // You can have 6 different heater settings
  // higher heat and longer times uses more power
  // and reads will take longer too!
  sht41.setHeater(SHT4X_NO_HEATER);
  switch (sht41.getHeater()) {
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
  
  /////////////////////////////////////////////////////////////////////////////////
  Wire1.begin(16,17,100000);
  if (! sht40.begin(&Wire1)) {
    Serial.println("Couldn't find SHT40");
    while (1) delay(1);
  }
  Serial.println("Found SHT40 sensor");
  Serial.print("Serial number 0x");
  Serial.println(sht40.readSerial(), HEX);

  // You can have 3 different precisions, higher precision takes longer
  sht40.setPrecision(SHT4X_HIGH_PRECISION);
  switch (sht40.getPrecision()) {
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

  // You can have 6 different heater settings
  // higher heat and longer times uses more power
  // and reads will take longer too!
  sht40.setHeater(SHT4X_NO_HEATER);
  switch (sht40.getHeater()) {
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


void loop() {
  sensors_event_t humidity, temp;
  
  uint32_t timestamp = millis();
  sht41.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  timestamp = millis() - timestamp;

  Serial.print("SHT41:");
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.print(" degrees C\t");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.print("% rH\t");
  Serial.print("Read duration (ms): ");  Serial.println(timestamp);

  ////////////////////////////////////////////////////////////////////////
  
  sensors_event_t humidity2, temp2;
  
  uint32_t timestamp2 = millis();
  sht40.getEvent(&humidity2, &temp2);// populate temp and humidity objects with fresh data
  timestamp2 = millis() - timestamp2;

  Serial.print("SHT40:");
  Serial.print("Temperature: "); Serial.print(temp2.temperature); Serial.print(" degrees C\t");
  Serial.print("Humidity: "); Serial.print(humidity2.relative_humidity); Serial.print("% rH\t");
  Serial.print("Read duration (ms): ");  Serial.println(timestamp2);


  Serial.println();
  delay(1000);
}
