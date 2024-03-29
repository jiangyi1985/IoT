/* 
 * 连接方式
 * SHTC3    NodeMcu-32S   NodeMcu-8266  BW16kit-v1.0
 * SDA      GPIO21        GPIO04        PA26
 * SCL      GPIO22        GPIO05        PA25
 * +        3.3V          3.3V          3.3V
 */

#include <SparkFun_SHTC3.h>
//#include <jm_LCM2004A_I2C.h>

#define BLINKER_WIFI
#include <Blinker.h>

SHTC3 mySHTC3;

char auth[] = "xxxxxxxxxxxx"; //上一步中在app中获取到的Secret Key
char ssid[] = "xxxxxxxx"; //您的WiFi热点名称
char pswd[] = "xxxxxxxx"; //您的WiFi密码

BlinkerNumber NumberTemp("num-temp");//defines a blinker number
BlinkerNumber NumberRh("num-rh");

void setup(){ //初始化函数，只在程序开始时运行一次
  Serial.begin(115200);   //设置串口波特率
  while(Serial == false){};   //等待串行连接启动
  Wire.begin();           //初始化Wire（IIC）库
  errorDecoder(mySHTC3.begin());// To start the sensor you must call "begin()", the default settings use Wire (default Arduino I2C port)

  Blinker.begin(auth, ssid, pswd);//start blinker wifi mode
  Blinker.attachHeartbeat(heartbeat);//app定时向设备发送心跳包, 设备收到心跳包后会返回设备当前状态，如果用户有自定义状态需要在收到心跳包时返回, 可调用该函数
//  Blinker.attachDataStorage(dataStorage);//关联回调函数，开启历史数据存储功能
}

float RH,T;

void loop() {
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
