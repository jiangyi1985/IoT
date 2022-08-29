/* 
 * 连接方式
 * SHTC3    NodeMcu-32S   NodeMcu-8266  BW16kit-v1.0
 * SDA      GPIO21        GPIO04        PA26
 * SCL      GPIO22        GPIO05        PA25
 * +        3.3V          3.3V          3.3V
 */

#include <SparkFun_SHTC3.h>//https://github.com/sparkfun/SparkFun_SHTC3_Arduino_Library
#include "U8g2lib.h"

SHTC3 mySHTC3;
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE,SCL,SDA);

void setup(){
  Serial.begin(115200);
  while(Serial == false){};
  Wire.begin();
  errorDecoder(mySHTC3.begin());// To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)
  u8x8.begin();
}

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

    u8x8.setFont(u8x8_font_courB18_2x3_r);
    u8x8.drawString(0,1," T:");
    String str = String(T);
    char buffer[20];
    str.toCharArray(buffer,20);
    u8x8.drawString(6,1,buffer);      
      
    u8x8.drawString(0,4,"RH:");
    str = String(RH);
    str.toCharArray(buffer,20);
    u8x8.drawString(6,4,buffer);    
  }else{
    Serial.print("Update failed, error: ");
    errorDecoder(mySHTC3.lastStatus); //输出错误原因
    Serial.println();
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
