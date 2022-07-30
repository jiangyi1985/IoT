/* 
 * 连接方式
 * SHTC3    NodeMcu-32S   NodeMcu-8266  BW16kit-v1.0  TM1637
 * SDA      GPIO21        GPIO04        PA26
 * SCL      GPIO22        GPIO05        PA25
 * +        3.3V          3.3V          3.3V
 *                                      2(PA27)       DIO
 *                                      9(PA15)       CLK
 */

#include <SparkFun_SHTC3.h>
//#include <jm_LCM2004A_I2C.h>
#include "TM1637.h"

SHTC3 mySHTC3;
const int CLK = 9;//bw16 PA15
const int DIO = 2;//bw16 PA27
TM1637 tm1637(CLK, DIO);

void setup(){ //初始化函数，只在程序开始时运行一次
  Serial.begin(115200);   //设置串口波特率
  while(Serial == false){};   //等待串行连接启动
  
  Wire.begin();           //初始化Wire（IIC）库
  
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  
  errorDecoder(mySHTC3.begin());// To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)
}

bool isT=true;

void loop() {
  float RH,T;
  delay(1000);                  //延时1000毫秒
  
  SHTC3_Status_TypeDef result = mySHTC3.update();
  if(mySHTC3.lastStatus == SHTC3_Status_Nominal)   //判断SHTC3状态是否正常
  {
    RH = mySHTC3.toPercent();   //读取湿度数据
    T = mySHTC3.toDegC();       //读取温度数据

    Serial.print("Humidity:");  //向串口打印 Humidity:
    Serial.print(RH);           //向串口打印湿度数据
    Serial.print("%");
    Serial.print("  Temperature:");
    Serial.print(T);            //向串口打印温度数据
    Serial.println("C"); 
  }else{
    Serial.print("Update failed, error: ");
    errorDecoder(mySHTC3.lastStatus); //输出错误原因
    Serial.println();
  }

  if(isT)
    tm1637.displayNum(T,2);
  else
    tm1637.displayNum(RH,2);
  isT=!isT;
}

void errorDecoder(SHTC3_Status_TypeDef message) // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
{
  switch(message)
  {
    case SHTC3_Status_Nominal : Serial.print("Nominal"); break;
    case SHTC3_Status_Error : Serial.print("Error"); break;
    case SHTC3_Status_CRC_Fail : Serial.print("CRC Fail"); break;
    default : Serial.print("Unknown return code"); break;
  }
}
