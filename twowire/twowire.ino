#include <SparkFun_SHTC3.h>

//To use the two I2C bus interfaces of the ESP32, you need to create two TwoWire instances.
//TwoWire I2Cone = TwoWire(0);
//TwoWire I2Ctwo = TwoWire(1);

//A simpler alternative is using the predefined Wire() and Wire1() objects.
//Wire().begin() creates an I2C communication on the first I2C bus using the default pins and default frequency.
//For the Wire1.begin() you should pass your desired SDA and SCL pins as well as the frequency.

SHTC3 shtc3_1;
SHTC3 shtc3_2;

void setup(){ //初始化函数，只在程序开始时运行一次
  Serial.begin(115200);   //设置串口波特率

  Serial.println("starting wire 1");
//  I2Cone.begin(21,22,100000); // SDA pin 21, SCL pin 22, 100kHz frequency
  Wire.begin();
  Serial.println("starting wire 2");
//  I2Ctwo.begin(16,17,400000); // SDA pin 16, SCL pin 17, 400kHz frequency
  Wire1.begin(16,17,100000);

  Serial.println("starting shtc3_1");
//  errorDecoder(shtc3_1.begin(I2Cone));
  errorDecoder(shtc3_1.begin(Wire));
  Serial.println("starting shtc3_2");
//  errorDecoder(shtc3_2.begin(I2Ctwo));
  errorDecoder(shtc3_2.begin(Wire1));
}

void loop() {
  float RH,T,RH2,T2;
  delay(1000);                  //延时1000毫秒
  
  SHTC3_Status_TypeDef result = shtc3_1.update();
  if(shtc3_1.lastStatus == SHTC3_Status_Nominal)   //判断SHTC3状态是否正常
  {
    RH = shtc3_1.toPercent();   //读取湿度数据
    T = shtc3_1.toDegC();       //读取温度数据

    Serial.print("shtc3_1:");
    Serial.print("Humidity:");  //向串口打印 Humidity:
    Serial.print(RH);           //向串口打印湿度数据
    Serial.print("%");
    Serial.print("  Temperature:");
    Serial.print(T);            //向串口打印温度数据
    Serial.println("C"); 
  }else{
    Serial.print("Update failed, error: ");
    errorDecoder(shtc3_1.lastStatus); //输出错误原因
  }
  
  SHTC3_Status_TypeDef result2 = shtc3_2.update();
  if(shtc3_2.lastStatus == SHTC3_Status_Nominal)   //判断SHTC3状态是否正常
  {
    RH2 = shtc3_2.toPercent();   //读取湿度数据
    T2 = shtc3_2.toDegC();       //读取温度数据

    Serial.print("shtc3_2:");
    Serial.print("Humidity:");  //向串口打印 Humidity:
    Serial.print(RH2);           //向串口打印湿度数据
    Serial.print("%");
    Serial.print("  Temperature:");
    Serial.print(T2);            //向串口打印温度数据
    Serial.println("C"); 
  }else{
    Serial.print("Update failed, error: ");
    errorDecoder(shtc3_2.lastStatus); //输出错误原因
  }

  Serial.println();
}

void errorDecoder(SHTC3_Status_TypeDef message) // The errorDecoder function prints "SHTC3_Status_TypeDef" results in a human-friendly way
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
