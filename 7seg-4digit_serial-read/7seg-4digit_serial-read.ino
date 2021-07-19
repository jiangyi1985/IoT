//digit pins (left to right)    --> nodeMCU32s.GPIO 15 12 13 14
//segment pins (A to G to dot)  --> nodeMCU32s.GPIO 21 22 23 25 26 27 32 33
//4 x 330Ω resistors

#include "SevSeg.h"

SevSeg sevseg; //Instantiate a seven segment object

void setup() {
  Serial.begin(115200);
    
  byte numDigits = 4;
  byte digitPins[] = {15, 12, 13, 14};
  byte segmentPins[] = {21, 22, 23, 25, 26, 27, 32, 33};
  bool resistorsOnSegments = false; // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  bool updateWithDelays = false; // Default 'false' is Recommended
  bool leadingZeros = false; // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);
  //sevseg.setBrightness(150);//a value between -200 and 200. 0 to 100 is the standard range. Numbers greater than 100 and less than -100 may cause noticeable flickering
}

//int digit=0;
//unsigned long t,t1,diff;
char c;
char cBuffer[4];
String input;

void loop() {
//  t1 = micros();
//  diff = t1-t;

//  if (diff>1000000)
//  {
//    t=t1;
//  
//    int number=digit*1000+digit*100+digit*10+digit;
//  
//    Serial.println(number);
//    sevseg.setNumber(number,3);
//  
//    digit=digit+1;
//    if(digit>9)
//      digit=0;
//  }

  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    char c = Serial.read();

    if (c!=10)//not line-feed
    {
      input+=c;
    }
    else{
      // say what you got:
      Serial.println(input);  

      //show float
      float f = input.toFloat();
      Serial.println(f);
      sevseg.setNumberF(f,2);

      //show string
      //input.toCharArray(cBuffer,5);//why 5?
      //Serial.println(cBuffer);      
      //sevseg.setChars(cBuffer);
      
      input="";
    }
  }
  
  sevseg.refreshDisplay();
}
