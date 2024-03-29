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

#define BLINKER_WIFI
#include <Blinker.h>

//shtc3.sda, PCF8574.sda --> nodeMCU32s.P21(GPIO21)
//shtc3.scl, PCF8574.scl --> nodeMCU32s.P22(GPIO22)
//lightSensor.VCC --> 3.3v
//lightSensor.AO --> nodeMCU32s.ADC6(GPIO34)
//
//AO output 0~4095
//3.3v office light 500 shadow 1600
//5v office light 700 shadow 2500

const int pin=34;//ADC6
SHTC3 mySHTC3;

char auth[] = "xxxxxxxxxxxx"; //上一步中在app中获取到的Secret Key
char ssid[] = "xxxxxxxx"; //您的WiFi热点名称
char pswd[] = "xxxxxxxx"; //您的WiFi密码

BlinkerNumber NumberTemp("num-temp");//defines a blinker number
BlinkerNumber NumberRh("num-rh");

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
BigFont bigfont(4); // declaration, 2 for 3colx2rows, 3 for 3colx3rows or 4 4colx4rows digits 
int passSeconds=0;  
bool isTemp=true;
bool isBacklightRequired=true;

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

  Blinker.begin(auth, ssid, pswd);//start blinker wifi mode
  Blinker.attachHeartbeat(heartbeat);//app定时向设备发送心跳包, 设备收到心跳包后会返回设备当前状态，如果用户有自定义状态需要在收到心跳包时返回, 可调用该函数
  Blinker.attachDataStorage(dataStorage);//关联回调函数，开启历史数据存储功能
}

  float RH,T;
  
void loop() {
  delay(1000);                  //延时1000毫秒

  int potValue=analogRead(pin);
  Serial.print("light sensor: ");
  Serial.println(potValue);  
  isBacklightRequired = potValue < 3000; //turn off backlight if room is too dark
  if(isBacklightRequired)
    lcd.backlight();
  else
    lcd.noBacklight();

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

    Blinker.run();//此函数需要频繁调用以保持设备间连接及处理收到的数据, 建议放在 loop() 函数中
}

void dataStorage(){//在回调函数中，设定要存储的键名和值
   Blinker.dataStorage("num-temp", T);//键值可以跟blinker number一样或者不一样
   Blinker.dataStorage("num-rh", RH);
}

void heartbeat(){
  //Serial.println("Blinker heartbeat received.");
  //Blinker.print("temp", T, "°C");
    NumberTemp.print(T);//send blinker number data to app
    NumberRh.print(RH);
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
