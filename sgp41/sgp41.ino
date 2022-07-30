/* 
 * SGP41    NodeMcu-32S   NodeMcu-8266
 * SDA      GPIO21        GPIO04
 * SCL      GPIO22        GPIO05
 * +        3.3V          3.3V
 */
 
#include <Arduino.h>
#include <SensirionI2CSgp41.h>//https://github.com/Sensirion/arduino-i2c-sgp41
#include <Wire.h>
#include <SensirionGasIndexAlgorithm.h>//https://github.com/Sensirion/arduino-gas-index-algorithm

SensirionI2CSgp41 sgp41;
GasIndexAlgorithmParams paramsVoc;
GasIndexAlgorithmParams paramsNox;

//The conditioning heats the hotplate of the NOx pixel to a different temperature compared to the measurement mode enabling a faster switch-on thereafter.
//It is recommended to execute the conditioning for 10 s, but 10 s must not be exceeded to avoid damage to the sensing material.
uint16_t conditioning_s = 10; ///< time in seconds needed for NOx conditioning

void setup() {
    GasIndexAlgorithm_init(&paramsVoc, GasIndexAlgorithm_ALGORITHM_TYPE_VOC);
    GasIndexAlgorithm_init(&paramsNox, GasIndexAlgorithm_ALGORITHM_TYPE_NOX);

    Serial.begin(115200);
    while (!Serial) {
        delay(100);
    }

    Wire.begin();

    uint16_t error;
    char errorMessage[256];

    sgp41.begin(Wire);

    uint16_t serialNumber[3];
    uint8_t serialNumberSize = 3;

    error = sgp41.getSerialNumber(serialNumber, serialNumberSize);
    if (error) {
        Serial.print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("SerialNumber:");
        Serial.print("0x");
        for (size_t i = 0; i < serialNumberSize; i++) {
            uint16_t value = serialNumber[i];
            Serial.print(value < 4096 ? "0" : "");
            Serial.print(value < 256 ? "0" : "");
            Serial.print(value < 16 ? "0" : "");
            Serial.print(value, HEX);
        }
        Serial.println();
    }

    uint16_t testResult;
    error = sgp41.executeSelfTest(testResult);
    if (error) {
        Serial.print("Error trying to execute executeSelfTest(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else if (testResult != 0xD400) {
        Serial.print("executeSelfTest failed with error: ");
        Serial.println(testResult);
    }
}

void loop() {
    uint16_t error;
    char errorMessage[256];

    //RH/ticks=RH/%×65'535/100
    uint16_t defaultRh = 0x8000;//---50%
    //T/ticks=(T/°C+45)×65'535/175
    uint16_t defaultT = 0x6666;//---25°C
    
    uint16_t srawVoc = 0;
    uint16_t srawNox = 0;

    delay(1000);

    if (conditioning_s > 0) {
        // During NOx conditioning (10s) SRAW NOx will remain 0
        error = sgp41.executeConditioning(defaultRh, defaultT, srawVoc);
        conditioning_s--;
    } else {
        // Read Measurement
        error = sgp41.measureRawSignals(defaultRh, defaultT, srawVoc, srawNox);
    }

    if (error) {
        Serial.print("Error trying to execute measureRawSignals(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        //In case of the VOC Index, a value of 100 refers to the average indoor gas composition over the past 24 h.
        //While values between 100 and 500 indicate a deterioration, values between 1 and 100 inform about improvement of the VOC based air quality.
        int32_t voc_index_value;
        //In case of the NOx Index, the average condition is mapped to a value of 1 and therefore, the NOx Index displays values between 2 and 500 when NOx events are present.
        int32_t nox_index_value; 
        
        GasIndexAlgorithm_process(&paramsVoc, srawVoc, &voc_index_value);
        if(srawNox!=0)
          GasIndexAlgorithm_process(&paramsNox, srawNox, &nox_index_value);
      
        Serial.print("SRAW_VOC:");
        Serial.print(srawVoc);
        Serial.print("\t");
        Serial.print("SRAW_NOx:");
        Serial.print(srawNox);
        
        Serial.print("\t");
        Serial.print("VOC:");
        Serial.print(voc_index_value);
        if(srawNox!=0)
        {
            Serial.print("\t");
            Serial.print("NOX:");
            Serial.print(nox_index_value);
        }
        
        Serial.println();
    }
}
