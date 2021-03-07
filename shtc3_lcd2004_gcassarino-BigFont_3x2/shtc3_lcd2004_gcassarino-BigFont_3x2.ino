/* https://blog.zeruns.tech
 * 连接方式
 * SHTC3    开发板
 * SCL      SCL（NodeMcu开发板是D1）
 * SDA      SDA（NodeMcu开发板是D2）
 */

#include <SparkFun_SHTC3.h>
//#include <jm_LCM2004A_I2C.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
//#include <BigFont02_I2C.h>

//#include <LiquidCrystal.h>
#include "BigFont.h"

SHTC3 mySHTC3;

//jm_LCM2004A_I2C lcd;

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//BigFont02_I2C     big(&lcd); // construct large font object, passing to it the name of our lcd object

//LiquidCrystal lcd(0x27,20,4)
BigFont bigfont(2); // declaration, 2 for 3colx2rows, 3 for 3colx3rows or 4 4colx4rows digits   



//byte c8[]={ // 2
//          B00000,
//          B00000,
//          B00000,
//          B00000,
//          B01111,
//          B01111,
//          B01111,
//          B01111
//        };

 

void setup(){ //初始化函数，只在程序开始时运行一次
  Serial.begin(115200);   //设置串口波特率
  while(Serial == false){};   //等待串行连接启动
  Wire.begin();           //初始化Wire（IIC）库
  unsigned char i=0;
  errorDecoder(mySHTC3.begin());// To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)

//  lcd.begin();
//  lcd.backlight();
//  lcd.init();

//lcd.init();                      // initialize the lcd 
//big.begin();

lcd.init(); 
//bigfont.setFontSize(size); // can be used to change display font size 2,3 or 4 at runtime
bigfont.loadchars(lcd);


//    lcd.createChar(0, c8);
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
//  Serial.println("https://blog.zeruns.tech");

//lcd.setCursor(0,0);
//  lcd.print(T);
//lcd.set_cursor(0,1);
//  lcd.print(RH);
//lcd.setCursor(0,2);
//  lcd.print("abc 1 2.34 56.789");
//lcd.setCursor(0,3);

////lcd.clear();
//big.writechar(0,0,'1');
//big.writechar(0,3,'2');
//big.writechar(0,6,'3');
//big.writechar(0,9,'4');
//big.writechar(0,12,'5');
//big.writechar(2,0,'a');
//big.writechar(2,3,'b');


//if(_bigFontSize == 2){

//    bigfont.printbigchar(lcd, 0, 0, 0);
//    bigfont.printbigchar(lcd, 1, 3, 0);
//    bigfont.printbigchar(lcd, 2, 6, 0);
//    bigfont.printbigchar(lcd, 3, 9, 0);
//    bigfont.printbigchar(lcd, 4, 12, 0);
//    bigfont.printbigchar(lcd, 5, 15, 0);
//    bigfont.printbigchar(lcd, 6, 0, 2);
//    bigfont.printbigchar(lcd, 7, 3, 2);
//    bigfont.printbigchar(lcd, 8, 6, 2);
//    bigfont.printbigchar(lcd, 9, 9, 2);
//    bigfont.printbigchar(lcd, 0, 12, 2);
//    bigfont.printbigchar(lcd, 0, 15, 2);

//for (int i = 0; i < 20; i++)
//{
//  lcd.setCursor(i,0);
//  lcd.write(i);
//}
//for (int i = 0; i < 20; i++)
//{
//  lcd.setCursor(i,1);
//  lcd.write(i+20);
//}
//for (int i = 0; i < 20; i++)
//{
//  lcd.setCursor(i,2);
//  lcd.write(i+40);
//}
//for (int i = 0; i < 20; i++)
//{
//  lcd.setCursor(i,3);
//  lcd.write(i+60);
//  //lcd.write(i+240);
//}

//    lcd.setCursor(0,0);
//    lcd.write('a');
//    lcd.setCursor(1,0);
//    lcd.write(254);
//    lcd.setCursor(2,0);
//    lcd.write(1);
//    lcd.setCursor(3,0);
//    lcd.write(2);
//    lcd.setCursor(4,0);
//    lcd.write(3);
//    lcd.setCursor(5,0);
//    lcd.write(4);
//    lcd.setCursor(6,0);
//    lcd.write(5);
//    lcd.setCursor(7,0);
//    lcd.write(6);


char outstr[15];
dtostrf(T,2,2,outstr);

//Serial.println(outstr);
////int number=atoi(outstr[0]);
////Serial.println(number);
//Serial.println(outstr[4]-'0');

int width=3;
int minusWidth=2;
int dotWidth=2;
int degreePreSpace=1;

if(T<0)
{
//print minus
}

for(int i=0;i<2;i++)
{
  byte b=outstr[i]-'0';
  bigfont.printbigchar(lcd, b, minusWidth+i*3, 0);
}

//dot
lcd.setCursor(8,1);
lcd.write(8);

for(int i=3;i<5;i++)
{
  byte b=outstr[i]-'0';
  bigfont.printbigchar(lcd, b, minusWidth-(width-dotWidth)+i*3, 0);
}

int tempNumbersWidth=minusWidth+width*4+dotWidth+degreePreSpace;
//degree
lcd.setCursor(tempNumbersWidth,0);
lcd.write('o');

//C
lcd.setCursor(tempNumbersWidth+1,0);
lcd.write(1);
lcd.setCursor(tempNumbersWidth+2,0);
lcd.write(2);
lcd.setCursor(tempNumbersWidth+1,1);
lcd.write(4);
lcd.setCursor(tempNumbersWidth+2,1);
lcd.write(8);

//for (int i = 0; i < 20; i++)
//{
//  lcd.setCursor(i,2);
//  lcd.write(i);
//}
//for (int i = 0; i < 20; i++)
//{
//  lcd.setCursor(i,3);
//  lcd.write(i+20);
//}
//
//return;

dtostrf(RH,2,2,outstr);

for(int i=0;i<2;i++)
{
  byte b=outstr[i]-'0';
  bigfont.printbigchar(lcd, b, minusWidth+i*3, 2);
}

//dot
lcd.setCursor(8,3);
lcd.write(8);

for(int i=3;i<5;i++)
{
  byte b=outstr[i]-'0';
  bigfont.printbigchar(lcd, b, minusWidth-(width-dotWidth)+i*3, 2);
}

//%
lcd.setCursor(tempNumbersWidth+1,2);
lcd.write('o');
lcd.setCursor(tempNumbersWidth+2,2);
lcd.write('/');
lcd.setCursor(tempNumbersWidth+1,3);
lcd.write('/');
lcd.setCursor(tempNumbersWidth+2,3);
lcd.write('o');


//    bigfont.printbigchar(lcd, 1, 3, 0);
//  } else if(_bigFontSize == 3){
//    bigfont.printbigchar(lcd, 0, 0, 0);
//    bigfont.printbigchar(lcd, this->_dt.hour, 3, 0);
//  } else {
//    bigfont.printbigchar(lcd, 0, 0, 0);
//    bigfont.printbigchar(lcd, this->_dt.hour, 4, 0);
//  }
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
