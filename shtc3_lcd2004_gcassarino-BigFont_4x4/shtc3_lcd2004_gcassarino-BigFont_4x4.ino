/*
 * 连接方式
 * SHTC3    开发板
 * SDA      SDA（NodeMcu-32S开发板是P21）
 * SCL      SCL（NodeMcu-32S开发板是P22）
 * +        3.3V
 */

#include <SparkFun_SHTC3.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "BigFont.h"

SHTC3 mySHTC3;

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
BigFont bigfont(4); // declaration, 2 for 3colx2rows, 3 for 3colx3rows or 4 4colx4rows digits 
int passSeconds=0;  
bool isTemp=true;

void setup(){ //初始化函数，只在程序开始时运行一次
  Serial.begin(115200);   //设置串口波特率
  while(Serial == false){};   //等待串行连接启动
  Wire.begin();           //初始化Wire（IIC）库
  unsigned char i=0;
  errorDecoder(mySHTC3.begin());// To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)

  lcd.init(); 
  lcd.backlight();
//lcd.noBlink();
  bigfont.loadchars(lcd);
}

void loop() {
  float RH,T;
  delay(1000);                  //延时1000毫秒
  passSeconds++;
  if(passSeconds>=2)
  {
    isTemp=!isTemp;
    passSeconds=0;
  }
  
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


  char outstr[15];
  
  if(isTemp)
  dtostrf(T,2,2,outstr);
  else
  dtostrf(RH,2,2,outstr);
  
  int width=4;
  int minusWidth=2;
  int dotWidth=2;
  int degreePreSpace=1;
  int digits=3;
  
  if(isTemp and T<0)
  {
  //print minus
  }
  
  for(int i=0;i<2;i++)
  {
    byte b=outstr[i]-'0';
    bigfont.printbigchar(lcd, b, minusWidth+i*width, 0);
  }
  
  //dot
  lcd.setCursor(10,3);
  lcd.write(7);
  //lcd.setCursor(11,3);
  //lcd.write(255);
  
  for(int i=3;i<=digits;i++)
  {
    byte b=outstr[i]-'0';
    bigfont.printbigchar(lcd, b, minusWidth-(width-dotWidth)+i*width, 0);
  }
  
  int tempNumbersWidth=minusWidth+width*digits+dotWidth+degreePreSpace;
  
  if(isTemp)
  {
    //clear
    lcd.setCursor(tempNumbersWidth+1,2);
    lcd.write(254);
    lcd.setCursor(tempNumbersWidth+0,3);
    lcd.write(254);
    lcd.setCursor(tempNumbersWidth+2,3);
    lcd.write(254);
    //degree
    lcd.setCursor(tempNumbersWidth,0);
    lcd.write(1);
    lcd.setCursor(tempNumbersWidth+1,0);
    lcd.write(2);
    lcd.setCursor(tempNumbersWidth+2,0);
    lcd.write(3);
    lcd.setCursor(tempNumbersWidth,1);
    lcd.write(4);
    lcd.setCursor(tempNumbersWidth+1,1);
    lcd.write(5);
    lcd.setCursor(tempNumbersWidth+2,1);
    lcd.write(6);
  }
  else
  {
    //clear
    lcd.setCursor(tempNumbersWidth,0);
    lcd.write(254);
    lcd.setCursor(tempNumbersWidth+1,0);
    lcd.write(254);
    lcd.setCursor(tempNumbersWidth+2,0);
    lcd.write(254);
    lcd.setCursor(tempNumbersWidth+1,1);
    lcd.write(254);
    //%
    lcd.setCursor(tempNumbersWidth,1);
    lcd.write(7);
    lcd.setCursor(tempNumbersWidth+2,1);
    lcd.write(0);
    lcd.setCursor(tempNumbersWidth+1,2);
    lcd.write(0);
    lcd.setCursor(tempNumbersWidth,3);
    lcd.write(0);
    lcd.setCursor(tempNumbersWidth+2,3);
    lcd.write(7);
  }
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
