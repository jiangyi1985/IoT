/*
 * 连接方式
 * SHTC3    开发板
 * SDA      SDA（NodeMcu-32S开发板是P21）
 * SCL      SCL（NodeMcu-32S开发板是P22）
 * +        3.3V
 */

#include <SparkFun_SHTC3.h>
#include <jm_LCM2004A_I2C.h>
#include <Wire.h> 
//#include "LiquidCrystal_I2C.h"

SHTC3 mySHTC3;
//LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
jm_LCM2004A_I2C lcd;

void setup(){ //初始化函数，只在程序开始时运行一次
  Serial.begin(115200);   //设置串口波特率
  while(Serial == false){};   //等待串行连接启动
  Wire.begin();           //初始化Wire（IIC）库
  unsigned char i=0;
  errorDecoder(mySHTC3.begin());// To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)

  lcd.begin();
//lcd.init();                      // initialize the lcd 
//  lcd.backlight();
//  lcd.init();
}

void loop() {
  float RH,T;
  delay(1000);                  //延时1000毫秒
  SHTC3_Status_TypeDef result = mySHTC3.update();
  if(mySHTC3.lastStatus == SHTC3_Status_Nominal)   //判断SHTC3状态是否正常
  {
    RH = mySHTC3.toPercent();   //读取湿度数据                       
    T = mySHTC3.toDegC();       //读取温度数据                    
  }else{
    Serial.print("Update failed, error: ");
    errorDecoder(mySHTC3.lastStatus); //输出错误原因
    Serial.println();
  }
  Serial.print("Humidity:");  //向串口打印 Humidity:
  Serial.print(RH);           //向串口打印湿度数据
  Serial.print("%");
  Serial.print("  Temperature:");
  Serial.print(T);            //向串口打印温度数据
  Serial.println("C"); 

lcd.setCursor(0,0);
  lcd.print(T);
lcd.set_cursor(0,1);
  lcd.print(RH);
lcd.setCursor(0,2);
  lcd.print("abc 1 2.34 56.789");
lcd.setCursor(0,3);
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
