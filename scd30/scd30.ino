/* 
 * SCD30    NodeMcu-32S   NodeMcu-8266
 * SDA      GPIO21        GPIO04
 * SCL      GPIO22        GPIO05
 * +        5V            5V
 */
 
/*
  Reading CO2, humidity and temperature from the SCD30
  By: Nathan Seidle
  SparkFun Electronics
  Date: May 22nd, 2018
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.
  Feel like supporting open source hardware?
  Buy a board from SparkFun! https://www.sparkfun.com/products/15112
  This example gets the SCD30's settings using the new getSettingValue function (thank you paulvha)
  Hardware Connections:
  Attach RedBoard to computer using a USB cable.
  Connect SCD30 to RedBoard using Qwiic cable.
  Open Serial Monitor at 115200 baud.
*/

#include <Wire.h>

#include "SparkFun_SCD30_Arduino_Library.h" //Click here to get the library: http://librarymanager/All#SparkFun_SCD30
SCD30 airSensor;

void setup()
{
    Serial.begin(115200);
    Serial.println("SCD30 Example");
    Wire.begin();

    //Start sensor using the Wire port, but 
    bool autoSelfCalibration = false;//enable auto-calibration?
    bool measureBegin = false;//start measuring immediately?
    while (airSensor.begin(Wire, autoSelfCalibration, measureBegin) == false)
    {
        Serial.println("Air sensor not detected. Retrying...");
        delay(500) ;
    }

    airSensor.StopMeasurement();
    
    //re-power device
    //airSensor.reset();
    
    //Ambient pressure compensation. Available range: 0 & [700 ... 1400]. Pressure in mBar.
    //0 deactivates pressure compensation (default ambient pressure = 1013.25 mBar)
    int presureOffset = 0;
    airSensor.beginMeasuring(presureOffset);

    //airSensor.setAmbientPressure(presureOffset)
    

    uint16_t settingVal; // The settings will be returned in settingVal
    
    if (airSensor.getForcedRecalibration(&settingVal) == true) // Get the setting
    {
      Serial.print("Forced recalibration factor (ppm) is ");
      Serial.println(settingVal);
    }
    else
    {
      Serial.print("getForcedRecalibration failed! Freezing...");
      while (1)
        ; // Do nothing more
    }

    //airSensor.setMeasurementInterval(2);//Interval in seconds. Available range: [2 ... 1800]
    if (airSensor.getMeasurementInterval(&settingVal) == true) // Get the setting
    {
      Serial.print("Measurement interval (s) is ");
      Serial.println(settingVal);
    }
    else
    {
      Serial.print("getMeasurementInterval failed! Freezing...");
      while (1)
        ; // Do nothing more
    }

    //Temperature offset, unit [°C x 100], i.e. one tick corresponds to 0.01°C
    //EACH TIME setTemperatureOffset is called, the offset value is added up ACCUMULATIVELY
    //So to actually update the offset, either re-power the device or soft reset
    //airSensor.setTemperatureOffset(0);
    if (airSensor.getTemperatureOffset(&settingVal) == true) // Get the setting
    {
      Serial.print("Temperature offset (C) is ");
      Serial.println(((float)settingVal) / 100.0, 2);
    }
    else
    {
      Serial.print("getTemperatureOffset failed! Freezing...");
      while (1)
        ; // Do nothing more
    }
    
    //airSensor.setAltitudeCompensation(0);//Height over sea level in [m]
    if (airSensor.getAltitudeCompensation(&settingVal) == true) // Get the setting
    {
      Serial.print("Altitude offset (m) is ");
      Serial.println(settingVal);
    }
    else
    {
      Serial.print("getAltitudeCompensation failed! Freezing...");
      while (1)
        ; // Do nothing more
    }

    if (airSensor.getFirmwareVersion(&settingVal) == true) // Get the setting
    {
      Serial.print("Firmware version is 0x");
      Serial.println(settingVal, HEX);
    }
    else
    {
      Serial.print("getFirmwareVersion! Freezing...");
      while (1)
        ; // Do nothing more
    }

    Serial.print("Auto calibration set to ");
    if (airSensor.getAutoSelfCalibration() == true)
        Serial.println("true");
    else
        Serial.println("false");

    //The SCD30 has data ready every two seconds
}

void loop()
{
    if (airSensor.dataAvailable())
    {
        Serial.print("co2(ppm):");
        Serial.print(airSensor.getCO2());

        Serial.print(" temp(C):");
        Serial.print(airSensor.getTemperature());

        Serial.print(" humidity(%):");
        Serial.print(airSensor.getHumidity());

        Serial.println();
    }
    else
        Serial.print(".");

    delay(500);
}
