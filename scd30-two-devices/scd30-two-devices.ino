/* 
 * Dual SCD30 Wiring for NodeMcu-32S (ESP32)
 * Sensor 1: SDA -> GPIO21, SCL -> GPIO22
 * Sensor 2: SDA -> GPIO19, SCL -> GPIO18 (Custom Wire1 configuration)
 * Power   : Both to 5V and GND
 */

#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h" // https://github.com/sparkfun/SparkFun_SCD30_Arduino_Library

// Create two independent sensor instances
SCD30 sensor1;
SCD30 sensor2;

// Define custom pins for the second I2C bus
#define I2C2_SDA 19
#define I2C2_SCL 18

void setup()
{
    Serial.begin(115200);
    Serial.println("Dual SCD30 Example Starting...");

    // 1. Initialize the primary I2C bus (uses default GPIO21 and GPIO22)
    Wire.begin(); 
    
    // 2. Initialize the secondary hardware I2C bus with custom pins
    Wire1.begin(I2C2_SDA, I2C2_SCL); 

    bool autoSelfCalibration = false;
    bool measureBegin = false;

    // --- Initialize Sensor 1 ---
    Serial.println("Initializing Sensor 1...");
    while (sensor1.begin(Wire, autoSelfCalibration, measureBegin) == false)
    {
        Serial.println("Sensor 1 not detected. Retrying...");
        delay(500);
    }
    sensor1.StopMeasurement();
    sensor1.beginMeasuring(0); // Start with default pressure compensation

    // --- Initialize Sensor 2 ---
    Serial.println("Initializing Sensor 2...");
    while (sensor2.begin(Wire1, autoSelfCalibration, measureBegin) == false)
    {
        Serial.println("Sensor 2 not detected. Retrying...");
        delay(500);
    }
    sensor2.StopMeasurement();
    sensor2.beginMeasuring(0); // Start with default pressure compensation

    Serial.println("Both sensors successfully initialized!");
}

void loop()
{
    // Check and print Sensor 1 Data
    if (sensor1.dataAvailable())
    {
        Serial.print("[S1] CO2(ppm): ");     Serial.print(sensor1.getCO2());
        Serial.print(" | Temp(C): ");        Serial.print(sensor1.getTemperature(), 1);
        Serial.print(" | Humidity(%): ");    Serial.print(sensor1.getHumidity(), 1);
        Serial.println();
    }

    // Check and print Sensor 2 Data
    if (sensor2.dataAvailable())
    {
        Serial.print("[S2] CO2(ppm): ");     Serial.print(sensor2.getCO2());
        Serial.print(" | Temp(C): ");        Serial.print(sensor2.getTemperature(), 1);
        Serial.print(" | Humidity(%): ");    Serial.print(sensor2.getHumidity(), 1);
        Serial.println();
    }

    Serial.println("--------------------------------------------------");
    delay(2000); // The SCD30 updates every 2 seconds by default
}
