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

  /* Default scale is +/-2g (full-scale range)
     Set scale using SCALE_2G, SCALE_4G, SCALE_8G
     Sets scale to +/-2g, 4g, or 8g respectively
  */
  // accel.init(SCALE_8G);

  /* Default output data rate (ODR) is 800 Hz (fastest)
     Set data rate using ODR_800, ODR_400, ODR_200, 
     ODR_100, ODR_50, ODR_12, ODR_6, ODR_1
     Sets data rate to 800, 400, 200, 100, 50, 12.5, 
     6.25, or 1.56 Hz respectively 
     See data sheet for relationship between voltage
     and ODR (pg. 7)
     https://cdn.sparkfun.com/datasheets/Sensors/Accelerometers/MMA8452Q-rev8.1.pdf */
  // accel.setDataRate(ODR_1);
}

void loop() {
  if (accel.available()) {      // Wait for new data from accelerometer
    // Acceleration of x, y, and z directions in g units
    // Inverting X and Y ensures that pointing the arrow DOWN gives a positive value
    float gX = -accel.getCalculatedX(); 
    float gY = -accel.getCalculatedY(); 
    float gZ = accel.getCalculatedZ(); // Z is already directionally correct

    // //raw data
    // float gRawX = -accel.getX(); 
    // float gRawY = -accel.getY(); 
    // float gRawZ = accel.getZ();

    // Net G-force vector formula: √(X² + Y² + Z²)
    float totalForce = sqrt((gX * gX) + (gY * gY) + (gZ * gZ));

    Serial.print("X_Axis:");
    Serial.print(gX, 3);

    Serial.print("\t");
    Serial.print("Y_Axis:");
    Serial.print(gY, 3);

    Serial.print("\t");
    Serial.print("Z_Axis:");
    Serial.print(gZ, 3);

    Serial.print("\t");
    Serial.print("Total:");
    Serial.print(totalForce, 3);
    
    // Serial.print("\t");
    // Serial.print("X_raw:");
    // Serial.print(gRawX, 3);

    // Serial.print("\t");
    // Serial.print("Y_Raw:");
    // Serial.print(gRawY, 3);

    // Serial.print("\t");
    // Serial.print("Z_Raw:");
    // Serial.print(gRawZ, 3);

    Serial.println();
  }
}