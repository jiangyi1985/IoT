/* 
 * 连接方式
 * PCA9548A SHTC3_1 SHTC3_2 SHTC3_3 NodeMcu-32S
 * SDA                              GPIO21
 * SCL                              GPIO22
 * +        +       +       +       3.3V
 * -        -       -       -       -
 * A0                               -
 * A1                               3.3v  (A0 LOW, A1 HIGH, A2 LOW --> TCA address is 0x72)
 * A2                               -
 * SD0      SDL
 * SC0      SCL
 * SD4              SDL
 * SC4              SCL
 * SD5                      SDL
 * SC5                      SCL
 */
#include "SparkFun_SHTC3.h"

#define TCA_ADDRESS 0x72

SHTC3 s1;
SHTC3 s2;
SHTC3 s3;

void setup(){
  Serial.begin(115200);
  while(Serial == false){};
  Wire.begin();
  
  Serial.println();
  
  TCA9548A(0);
  Serial.print("starting s1...");
  errorDecoder(s1.begin());
  
  TCA9548A(4);
  Serial.print("starting s2...");
  errorDecoder(s2.begin());
  
  TCA9548A(5);
  Serial.print("starting s3...");
  errorDecoder(s3.begin());
  
  Serial.println();
}

// Select I2C BUS
void TCA9548A(uint8_t bus){
  Wire.beginTransmission(TCA_ADDRESS);  // TCA9548A address
  Wire.write(1 << bus);          // send byte to select bus
  Wire.endTransmission();
}

void loop() {
  TCA9548A(0);
  Serial.print("reading s1...");
  ReadShtc3Data(s1);
  
  TCA9548A(4);
  Serial.print("reading s2...");
  ReadShtc3Data(s1);
  
  TCA9548A(5);
  Serial.print("reading s3...");
  ReadShtc3Data(s1);

  Serial.println();
  delay(1000);
}

void ReadShtc3Data(SHTC3 mySHTC3)
{
  float RH,T;
  SHTC3_Status_TypeDef result = mySHTC3.update();
  if(mySHTC3.lastStatus == SHTC3_Status_Nominal)
  {
    RH = mySHTC3.toPercent();
    T = mySHTC3.toDegC();
    Serial.print("Humidity:");    Serial.print(RH);    Serial.print("%");
    Serial.print("  Temperature:");    Serial.print(T);    Serial.print("C"); 
    Serial.println();
  }else{
    Serial.print("Update failed, error: ");
    errorDecoder(mySHTC3.lastStatus);
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
