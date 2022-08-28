/* 
 * 连接方式
 * SHTC3    NodeMcu-32S   NodeMcu-8266  BW16kit-v1.0
 * SDA      GPIO21        GPIO04        PA26
 * SCL      GPIO22        GPIO05        PA25
 * +        3.3V          3.3V          3.3V
 */

#include <SparkFun_SHTC3.h>//https://github.com/sparkfun/SparkFun_SHTC3_Arduino_Library
//#include <jm_LCM2004A_I2C.h>

SHTC3 mySHTC3;

void setup(){ //初始化函数，只在程序开始时运行一次
  Serial.begin(115200);   //设置串口波特率
  while(Serial == false){};   //等待串行连接启动
  Wire.begin();           //初始化Wire（IIC）库
  unsigned char i=0;
  errorDecoder(mySHTC3.begin());// To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)
  Serial.print("mySHTC3.passIDcrc:");Serial.println(mySHTC3.passIDcrc);

  //some cheap shtc3 from taobao cannot work (wrong rh output) in RH first mode (with clock stretching, for both NPM/LPM), and they have wrong ID register: 8432 (0b0010000011110000)
//  mySHTC3.setMode(SHTC3_CMD_CSE_TF_NPM);
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
    Serial.print("C"); 
    Serial.print(" passRHcrc:");    Serial.print(mySHTC3.passRHcrc);
    Serial.print(" passTcrc:");    Serial.print(mySHTC3.passTcrc);
    Serial.println();
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
