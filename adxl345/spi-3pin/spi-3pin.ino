// =================== 3200 Hz 电阻桥超频硬件 SPI 矩阵 ===================
// SCL              GPIO18                SPI Clock (SCLK)
// SDA              GPIO19                接 MISO 引脚
// SD0              悬空 (LEAVE OPEN)      不用接 (板载 R4 已将其接地)
// CS               GPIO5                 Chip Select (CS)
// [电阻]            GPIO23 -> GPIO19      在面包板上跨接一根 1k~10k 电阻
// ========================================================================

#include <SPI.h>

#define CS_PIN   5
#define SCL_PIN  18
#define MISO_PIN 19
#define MOSI_PIN 23

void writeRegHw(byte reg, byte value);
void readXYZBurst(int16_t *x, int16_t *y, int16_t *z);

unsigned long lastTime = 0;
int frameCount = 0;

void setup(){
  // 保持 921600 极速波特率
  Serial.begin(921600);                 
  Serial.println();
  
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  // 初始化硬件 SPI 接口
  SPI.begin(SCL_PIN, MISO_PIN, MOSI_PIN, CS_PIN); 
  SPI.setDataMode(SPI_MODE3);
  SPI.setFrequency(4000000);            // 4 MHz 硬件频率

  // 1. 切换为 3线模式 (Bit 6 = 1), 2g量程, FULL_RES -> 0x48
  writeRegHw(0x31, 0x48); 
  delay(5);
  
  // 2. 核心突破：将内部采样率直接拉满到最高 3200 Hz -> 0x0E
  writeRegHw(0x2C, 0x0E);               
  
  // 3. 开启测量模式
  writeRegHw(0x2D, 0x08);               
  delay(10);
  
  lastTime = millis();
}

void loop(){  
  int16_t x, y, z;
  
  // 使用多字节突发读取（Burst Read），单次总线开启直接收齐 6 个字节
  readXYZBurst(&x, &y, &z);

  // 极为紧凑的 CSV 打印输出，绝不掺杂任何多余字符
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.print(z); 
  // print min/max caps
  Serial.print(",");
  Serial.print(-1000);
  Serial.print(",");
  Serial.println(1000);

  // 每过 1 秒，在后台默默帮你监控一次实际输出的行数（帧率检测）
  frameCount++;
  if (millis() - lastTime >= 1000) {
    // 临时在串口打印当前实际赫兹数，看是否逼近 3200
    // Serial.print("--- Actual Hz: "); Serial.println(frameCount); 
    frameCount = 0;
    lastTime = millis();
  }

  // 3200 Hz 对应的周期是 312.5 微秒。
  // 扣除 SPI 读取与串口发送所消耗的硬件时间，此处大约补齐 260 微秒即可。
  delayMicroseconds(260); 
}

// 硬件快速写入
void writeRegHw(byte reg, byte value) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(reg & 0x3F); 
  SPI.transfer(value);      
  digitalWrite(CS_PIN, HIGH);
}

// 硬件多字节突发读取（Burst Read 六轴）
void readXYZBurst(int16_t *x, int16_t *y, int16_t *z) {
  byte buffer[6];
  
  digitalWrite(CS_PIN, LOW);
  // 0x32 是 X 轴低字节起始地址
  // 0x80 代表 Read 操作
  // 0x40 代表 Multi-byte (突发多字节连续读取)
  // 0x32 | 0x80 | 0x40 = 0xF2
  SPI.transfer(0xF2); 
  
  // 一口气连续硬件吞吐 6 个字节
  for(int i = 0; i < 6; i++) {
    buffer[i] = SPI.transfer(0x00);
  }
  digitalWrite(CS_PIN, HIGH);

  // 高低字节拼接
  *x = (int16_t)((buffer[1] << 8) | buffer[0]);
  *y = (int16_t)((buffer[3] << 8) | buffer[2]);
  *z = (int16_t)((buffer[5] << 8) | buffer[4]);
}
