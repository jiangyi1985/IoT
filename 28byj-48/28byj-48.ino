//Includes the Arduino Stepper Library
#include <Stepper.h>

// Defines the number of steps per rotation
const int stepsPerRevolution = 2038;

// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
// Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);//arduino IN1,2,3,4 <=> 8,9,10,11
Stepper myStepper = Stepper(stepsPerRevolution, 25, 27, 26, 14);//nodemcu IN1,2,3,4 <=> 25,26,27,14

void setup() {
  Serial.begin(115200);
  
    // Nothing to do (Stepper Library sets pins as outputs)
}

void loop() {
	// Rotate CW slowly at 5 RPM
	myStepper.setSpeed(5);
  Serial.println("clockwise");
	myStepper.step(stepsPerRevolution);
	delay(1000);
	
	// Rotate CCW quickly at 10 RPM
	myStepper.setSpeed(10);
  Serial.println("counterclockwise");
	myStepper.step(-stepsPerRevolution);
	delay(1000);
}