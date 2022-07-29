//https://github.com/RoboticsBrno/ServoESP32
#include <Servo.h>
static const int servoPin = 4;
Servo servo1;
void setup() {
    Serial.begin(115200);
//    servo1.attach(servoPin);    
    servo1.attach(servoPin,1,0,180,450,2350);//attach(int pin, int channel, int minAngle, int maxAngle, int minPulseWidth, int maxPulseWidth) 
}
void loop() {
    for(int posDegrees = 0; posDegrees <= 180; posDegrees++) {
        servo1.write(posDegrees);
        Serial.println(posDegrees);
        delay(20);
    }
    for(int posDegrees = 180; posDegrees >= 0; posDegrees--) {
        servo1.write(posDegrees);
        Serial.println(posDegrees);
        delay(20);
    }
}



////https://github.com/madhephaestus/ESP32Servo
//#include <ESP32Servo.h>
//Servo myservo;  // create servo object to control a servo// 16 servo objects can be created on the ESP32
//int pos = 0;    // variable to store the servo position// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 // Possible PWM GPIO pins on the ESP32-S2: 0(used by on-board button),1-17,18(used by on-board LED),19-21,26,33-42
//int servoPin = 4;
//void setup() {
//  ESP32PWM::allocateTimer(0);// Allow allocation of all timers
//  ESP32PWM::allocateTimer(1);
//  ESP32PWM::allocateTimer(2);
//  ESP32PWM::allocateTimer(3);
//  myservo.setPeriodHertz(50);    // standard 50 hz servo
//  myservo.attach(servoPin, 450, 2350); // different servos may require different min/max settings  // for an accurate 0 to 180 sweep
//}
//void loop() {
//  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//    myservo.write(pos);    // tell servo to go to position in variable 'pos'
//    Serial.println(pos);
//    delay(20);             // waits 15ms for the servo to reach the position
//  }
//  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//    myservo.write(pos);    // tell servo to go to position in variable 'pos'
//    Serial.println(pos);
//    delay(20);             // waits 15ms for the servo to reach the position
//  }
//}
