// =================== I2C WIRING MATRIX ===================
// ADXL345 Module   NodeMCU-32S (ESP32)   Notes
// 3.3V             3V3                   Main 3.3V power rail input
// GND              GND                   Common Ground reference
// SCL              GPIO22 / D22          Hardware I2C Clock Pin (SCL)
// SDA              GPIO21 / D21          Hardware I2C Data Pin (SDA)
// SD0              -                     Leave DISCONNECTED -> Default address 0x53 via on-board pull-down
// 5V               -                     Leave Disconnected (Only use if powering via 5V rail)
// VS               -                     Leave Disconnected (Internal core supply, handled on-board)
// CS               3V3                   Tied High -> Safely locks chip into I2C mode
// INT1             -                     Optional: Leave empty if using polling mode
// INT2             -                     Optional: Leave empty if using polling mode
// =========================================================

#include <SparkFun_ADXL345.h>         // SparkFun ADXL345 Library

/*********** COMMUNICATION SELECTION ***********/
/*    Comment Out The One You Are Not Using    */
// ADXL345 adxl = ADXL345(10);           // USE FOR SPI COMMUNICATION, ADXL345(CS_PIN);
ADXL345 adxl = ADXL345();             // USE FOR I2C COMMUNICATION

/****************** INTERRUPT ******************/
/*      Uncomment If Attaching Interrupt       */
//int interruptPin = 2;                 // Setup pin 2 to be the interrupt pin (for most Arduino Boards)

/******************** SETUP ********************/
/*          Configure ADXL345 Settings         */
void setup(){
  
  Serial.begin(115200);                 // Start the serial terminal
  Serial.println("SparkFun ADXL345 Accelerometer Hook Up Guide Example");
  Serial.println();
  
  adxl.powerOn();                     // Power on the ADXL345

  adxl.setRangeSetting(2);           // Give the range settings
                                      // Accepted values are 2g, 4g, 8g or 16g
                                      // Higher Values = Wider Measurement Range
                                      // Lower Values = Greater Sensitivity

  // chip is always 4mg/LSB
  // If Set (1) Device is in Full Resolution Mode: Output Raw Value Range Increase with G Range
  //    when range is 2g, raw value -512~512, 1G = 256
  //    when range is 4g, raw value -1024~1024, 1G = 256
  //    when range is 16g, raw value -4096~4096, 1G = 256
  // If Set (0) Device is in 10-bit Mode: Output Raw Value Stays
  //    when range is 2g, raw value -512~512, 1G = 256
  //    when range is 16g, raw value -512~512, 1G = 32
  adxl.setFullResBit(1); // Forces 4mg/LSB full resolution accuracy across all ranges

  adxl.setSpiBit(0);                  // If Set (1) Puts Device in 3-wire Mode
                                      // If Set (0) Puts Device in 4-wire SPI Mode
   
  adxl.setActivityXYZ(1, 0, 0);       // Set to activate movement detection in the axes "adxl.setActivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setActivityThreshold(75);      // 62.5mg per increment   // Set activity   // Inactivity thresholds (0-255)
 
  adxl.setInactivityXYZ(1, 0, 0);     // Set to detect inactivity in all the axes "adxl.setInactivityXYZ(X, Y, Z);" (1 == ON, 0 == OFF)
  adxl.setInactivityThreshold(75);    // 62.5mg per increment   // Set inactivity // Inactivity thresholds (0-255)
  adxl.setTimeInactivity(10);         // How many seconds of no activity is inactive?

  adxl.setTapDetectionOnXYZ(0, 0, 1); // Detect taps in the directions turned ON "adxl.setTapDetectionOnX(X, Y, Z);" (1 == ON, 0 == OFF)
 
  // Set values for what is considered a TAP and what is a DOUBLE TAP (0-255)
  adxl.setTapThreshold(50);           // 62.5 mg per increment
  adxl.setTapDuration(15);            // 625 μs per increment
  adxl.setDoubleTapLatency(80);       // 1.25 ms per increment
  adxl.setDoubleTapWindow(200);       // 1.25 ms per increment
 
  // Set values for what is considered FREE FALL (0-255)
  adxl.setFreeFallThreshold(7);       // (5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(30);       // (20 - 70) recommended - 5ms per increment
 
  // Setting all interupts to take place on INT1 pin
  //adxl.setImportantInterruptMapping(1, 1, 1, 1, 1);     // Sets "adxl.setEveryInterruptMapping(single tap, double tap, free fall, activity, inactivity);" 
                                                        // Accepts only 1 or 2 values for pins INT1 and INT2. This chooses the pin on the ADXL345 to use for Interrupts.
                                                        // This library may have a problem using INT2 pin. Default to INT1 pin.
  
  // Turn on Interrupts for each mode (1 == ON, 0 == OFF)
  adxl.InactivityINT(1);
  adxl.ActivityINT(1);
  adxl.FreeFallINT(1);
  adxl.doubleTapINT(1);
  adxl.singleTapINT(1);
  
//attachInterrupt(digitalPinToInterrupt(interruptPin), ADXL_ISR, RISING);   // Attach Interrupt

}

// int maxX = 0;
// int maxY = 0;
// int maxZ = 0;

/*     Accelerometer Readings and Interrupt    */
void loop(){

  // -------G Value-------
  double g_values[3];          // Create an array to hold X, Y, Z
  adxl.get_Gxyz(g_values);     // Pass the array to the function
  Serial.print("X:"); Serial.print(g_values[0]);
  Serial.print(",Y:"); Serial.print(g_values[1]);
  Serial.print(",Z:"); Serial.println(g_values[2]);
  
  // ------Raw Value------
  // int raw_values[3];           // Create a 3-element integer array
  // adxl.readAccel(raw_values);  // The library populates [0]=X, [1]=Y, [2]=Z
  // Serial.print("RawX:"); Serial.print(raw_values[0]);
  // Serial.print(",RawY:"); Serial.print(raw_values[1]);
  // Serial.print(",RawZ:"); Serial.println(raw_values[2]);
  // -------print max raw value-------
  // if (abs(raw_values[0]) > abs(maxX)) { maxX = raw_values[0]; }
  // if (abs(raw_values[1]) > abs(maxY)) { maxY = raw_values[1]; }
  // if (abs(raw_values[2]) > abs(maxZ)) { maxZ = raw_values[2]; }
  // Serial.print("MaxX:"); Serial.print(maxX);
  // Serial.print(",MaxY:"); Serial.print(maxY);
  // Serial.print(",MaxZ:"); Serial.println(maxZ);
  
  // ------------print motions-------------
  // ADXL_ISR();
  // You may also choose to avoid using interrupts and simply run the functions within ADXL_ISR(); 
  //  and place it within the loop instead.  
  // This may come in handy when it doesn't matter when the action occurs. 

}

/********************* ISR *********************/
/* Look for Interrupts and Triggered Action    */
void ADXL_ISR() {
  
  // getInterruptSource clears all triggered actions after returning value
  // Do not call again until you need to recheck for triggered actions
  byte interrupts = adxl.getInterruptSource();
  
  // Free Fall Detection
  if(adxl.triggered(interrupts, ADXL345_FREE_FALL)){
    Serial.println("*** FREE FALL ***");
    //add code here to do when free fall is sensed
  } 
  
  // Inactivity
  if(adxl.triggered(interrupts, ADXL345_INACTIVITY)){
    Serial.println("*** INACTIVITY ***");
     //add code here to do when inactivity is sensed
  }
  
  // Activity
  if(adxl.triggered(interrupts, ADXL345_ACTIVITY)){
    Serial.println("*** ACTIVITY ***"); 
     //add code here to do when activity is sensed
  }
  
  // Double Tap Detection
  if(adxl.triggered(interrupts, ADXL345_DOUBLE_TAP)){
    Serial.println("*** DOUBLE TAP ***");
     //add code here to do when a 2X tap is sensed
  }
  
  // Tap Detection
  if(adxl.triggered(interrupts, ADXL345_SINGLE_TAP)){
    Serial.println("*** TAP ***");
     //add code here to do when a tap is sensed
  } 
}
