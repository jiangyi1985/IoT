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

    // Orientation of board (Right, Left, Down, Up);
    //the embedded Portrait-to-Landscape orientation threshold angle is actually permanently fixed at 45° with a ±14° hysteresis window

    if (accel.isRight() == true) {
      Serial.print("Right");
    }
    else if (accel.isLeft() == true) {
      Serial.print("Left");
    }
    else if (accel.isUp() == true) {
      Serial.print("Up");
    }
    else if (accel.isDown() == true) {
      Serial.print("Down");
    }
    else if (accel.isFlat() == true) {
      Serial.print("Flat");
    }

    Serial.println(); // Complete the printed log line
  }
}