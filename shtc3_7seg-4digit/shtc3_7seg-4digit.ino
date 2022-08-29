//digit pins (left to right)    --> nodeMCU32s.GPIO 15 12 13 14
//segment pins (A to G to dot)  --> nodeMCU32s.GPIO 18 19 23 25 26 27 32 33
//4 x 330Ω resistors on digit pins
/* 
 * 连接方式
 * SHTC3    开发板
 * SDA      SDA（NodeMcu-32S开发板是P21）
 * SCL      SCL（NodeMcu-32S开发板是P22）
 * +        3.3V
 */

#include "SevSeg.h"
#include <SparkFun_SHTC3.h>

SevSeg sevseg; //Instantiate a seven segment object
SHTC3 mySHTC3;

void setup()
{
  Serial.begin(115200);
  while (Serial == false)
  {
  };                             //等待串行连接启动
  Wire.begin();                  //初始化Wire（IIC）库
  errorDecoder(mySHTC3.begin()); // To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)

  byte numDigits = 4;
  byte digitPins[] = {15, 12, 13, 14};
  byte segmentPins[] = {18, 19, 23, 25, 26, 27, 32, 33};
  bool resistorsOnSegments = false;   // 'false' means resistors are on digit pins
  byte hardwareConfig = COMMON_ANODE; // See README.md for options
  bool updateWithDelays = false;      // Default 'false' is Recommended
  bool leadingZeros = false;          // Use 'true' if you'd like to keep the leading zeros
  bool disableDecPoint = false;       // Use 'true' if your decimal point doesn't exist or isn't connected. Then, you only need to specify 7 segmentPins[]

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
               updateWithDelays, leadingZeros, disableDecPoint);
  //sevseg.setBrightness(150);//a value between -200 and 200. 0 to 100 is the standard range. Numbers greater than 100 and less than -100 may cause noticeable flickering
}

//int digit=0;
unsigned long t, t1, diff;
bool isTemp = true;
float RH, T;

void loop()
{
  t1 = micros();
  diff = t1 - t;

  if (diff > 1500000)
  {
    t = t1;

    //    int number=digit*1000+digit*100+digit*10+digit;
    //    Serial.println(number);
    //    sevseg.setNumber(number,3);
    //    digit=digit+1;
    //    if(digit>9)
    //      digit=0;

    SHTC3_Status_TypeDef result = mySHTC3.update();
    if (mySHTC3.lastStatus == SHTC3_Status_Nominal) //判断SHTC3状态是否正常
    {
      RH = mySHTC3.toPercent(); //读取湿度数据
      T = mySHTC3.toDegC();     //读取温度数据
    }
    else
    {
      Serial.print("Update failed, error: ");
      errorDecoder(mySHTC3.lastStatus); //输出错误原因
      Serial.println();
    }
    // Serial.print("Humidity:"); //向串口打印 Humidity:
    // Serial.print(RH);          //向串口打印湿度数据
    // Serial.print("%");
    // Serial.print("  Temperature:");
    // Serial.print(T); //向串口打印温度数据
    // Serial.println("C");

    if (isTemp)
      sevseg.setNumberF(T, 2);
    else
      sevseg.setNumberF(RH, 2);

    isTemp = !isTemp;
  }

  sevseg.refreshDisplay();
}

void errorDecoder(SHTC3_Status_TypeDef message) // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
{
  switch(message)
  {
    case SHTC3_Status_Nominal : Serial.print("Nominal"); break;
    case SHTC3_Status_Error : Serial.print("Error"); break;
    case SHTC3_Status_CRC_Fail : Serial.print("CRC Fail"); break;
    case SHTC3_Status_ID_Fail : Serial.print("Check ID Fail"); break;
    default : Serial.print("Unknown return code"); break;
  }
  Serial.println();
}
