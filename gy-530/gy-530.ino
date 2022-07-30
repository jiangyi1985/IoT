/* 
 * SGP41    NodeMcu-32S   NodeMcu-8266
 * SDA      GPIO21        GPIO04
 * SCL      GPIO22        GPIO05
 * +        3.3V          3.3V
 */
#include "Adafruit_VL53L0X.h"//https://github.com/adafruit/Adafruit_VL53L0X

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  

//VL53L0X_SENSE_DEFAULT
//VL53L0X_SENSE_LONG_RANGE
//VL53L0X_SENSE_HIGH_SPEED
//VL53L0X_SENSE_HIGH_ACCURACY
  Adafruit_VL53L0X::VL53L0X_Sense_config_t vl_config = Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE;
    
//  if (!lox.begin()) {
  if (!lox.begin(0x29,false,&Wire,vl_config)) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
}

unsigned long time1;
unsigned long time2;

void loop() {
  VL53L0X_RangingMeasurementData_t measure;
    
  Serial.print("Reading a measurement... ");
  time1=millis();
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  time2=millis();

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print(time2-time1); Serial.print("ms, Distance (mm): "); Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.print(time2-time1); Serial.println("ms, out of range ");
  }
    
  delay(100);
}
