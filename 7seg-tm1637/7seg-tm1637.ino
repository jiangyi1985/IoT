
//tm1637.DIO --> nodeMCU32s.P16(GPIO16)
//tm1637.CLK --> nodeMCU32s.P17(GPIO17)

#include "TM1637.h"

// Pins definitions for TM1637 and can be changed to other ports
const int CLK = 17;//NodeMCU GPIO17
const int DIO = 16;//NodeMCU GPIO16
TM1637 tm1637(CLK, DIO);

float number = -1234.5;
int mult = 1;

void setup() {
//  Serial.begin(115200);   //设置串口波特率
  
    tm1637.init();
    tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

    float num = -123.45;

    // Without specifying decimal pointt it displays int
    tm1637.displayNum(num);    // -123
    delay(2000);

    // When numbers doesn't fit biggest digits are truncated
    tm1637.displayNum(num, 1); // -234
    delay(2000);

    // Colon light up as decimal point
    tm1637.displayNum(num, 2); // -3:45
    delay(2000);

    // Note that adding 3 digits (-123450) makes number out of int range
    tm1637.displayNum(num, 3); // -450
    delay(2000);

    // Display negative numbers without sign
    tm1637.displayNum(num, 0, false); // _123
    delay(2000);
//    return;
}

void loop() {
    tm1637.displayNum(number);

    number += 17.42 * mult;
    if (abs(number) > 1500) {
        mult *= -1;
    }

    delay(50);
}
