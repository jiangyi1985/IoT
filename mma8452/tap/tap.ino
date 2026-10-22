// HW-616 Module   NodeMCU-32S (ESP32)   Notes
// 3.3V            3V3                   Main 3.3V power rail input
// GND             GND                   Common Ground reference
// SCL             GPIO22                Hardware I2C Clock Pin
// SDA             GPIO21                Hardware I2C Data Pin
// SA0 / ADR       3V3                   Tied High -> Locks address permanently to 0x1D
// 5V              -                     Leave Disconnected

#include <Wire.h>                 // Must include Wire library for I2C
#include "SparkFun_MMA8452Q.h"    // https://github.com/sparkfun/SparkFun_MMA8452Q_Arduino_Library

MMA8452Q accel;                   // create instance of the MMA8452 class

void setup() {
  Serial.begin(115200);
  Serial.println("MMA8452Q Basic Reading Code!");
  Wire.begin();

  if (accel.begin() == false) {
    Serial.println("Not Connected. Please check connections and read the hookup guide.");
    while (1);
  }
}

void loop() {
  if (accel.available()) {      // Wait for new data from accelerometer
    
    // Read the raw 7-bit status register from the chip backend
    byte tapRegister = accel.readTap();    

    if (tapRegister > 0) {
      Serial.print("Raw Register: ");
      Serial.print(tapRegister);
      Serial.print("\t-> ");

      // 1. Decode Single vs Double Tap Event using Bit 3 (0x08 / DPE)
      if ((tapRegister & 0x08) == 0x08) {    
        Serial.print("⚡ DOUBLE TAP [");
      } 
      else { 
        Serial.print("☝️ SINGLE TAP [");
      }

      // 2. Decode the Intercepted Axis and Directional Polarity
      // Bit 6 (0x40) = Z-Axis, Bit 2 (0x04) = Z-Polarity
      if ((tapRegister & 0x40) == 0x40) {
        Serial.print("Axis: Z | Direction: ");
        // Z-axis uses standard direct tracking values
        if ((tapRegister & 0x04) == 0x04) Serial.print("+Z (Into Board)");
        else                              Serial.print("-Z (Out of Board)");
      }
      // Bit 5 (0x20) = Y-Axis, Bit 1 (0x02) = Y-Polarity
      else if ((tapRegister & 0x20) == 0x20) {
        Serial.print("Axis: Y | Direction: ");
        // Applying your custom coordinate sign correction (Inverting Y-polarity)
        if ((tapRegister & 0x02) == 0x02) Serial.print("-Y (Opposite Arrow)");
        else                              Serial.print("+Y (Arrow Direction)");
      }
      // Bit 4 (0x10) = X-Axis, Bit 0 (0x01) = X-Polarity
      else if ((tapRegister & 0x10) == 0x10) {
        Serial.print("Axis: X | Direction: ");
        // Applying your custom coordinate sign correction (Inverting X-polarity)
        if ((tapRegister & 0x01) == 0x01) Serial.print("-X (Opposite Arrow)");
        else                              Serial.print("+X (Arrow Direction)");
      }

      //x y axis tap is turned off by sparkFun in the library:
      //setupTap(0x80, 0x80, 0x08); // Disable x, y, set z to 0.5g

      Serial.print("]");

      Serial.println(); // Complete the printed log line
    }
  }
}