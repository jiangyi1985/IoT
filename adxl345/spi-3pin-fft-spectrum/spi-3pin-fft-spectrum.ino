// =================== 3200 Hz 3线硬件 SPI + 30频段 范围显示均衡器 ===================
// SCL                 GPIO18               SPI Clock (SCLK)
// SDA                 GPIO19               Connects to MISO directly
// SD0                 悬空 (LEAVE OPEN)     Do not connect (Internally grounded via R4)
// CS                  GPIO5                Chip Select (CS)
// [Resistor Bridge]   GPIO23 -> GPIO19     Bridge a 1k~10k resistor between 23 and 19
// ========================================================================

#include <SPI.h>
#include <arduinoFFT.h>               // 引入 v2.x 核心数学库

#define CS_PIN   5
#define SCL_PIN  18
#define MISO_PIN 19
#define MOSI_PIN 23

#define SAMPLES 512                   
#define SAMPLING_FREQUENCY 3200       

double vReal[SAMPLES];                
double vImag[SAMPLES];                
int sampleCounter = 0;

// Instantiate v2.x object
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

void writeRegHw(byte reg, byte value);
void readXYZBurst(int16_t *x, int16_t *y, int16_t *z);

void setup(){
  Serial.begin(921600);                 
  while(!Serial); 
  
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  SPI.begin(SCL_PIN, MISO_PIN, MOSI_PIN, CS_PIN); 
  SPI.setDataMode(SPI_MODE3);
  SPI.setFrequency(4000000);            

  writeRegHw(0x31, 0x48); // 3-Wire Mode, 2g, Full Res
  delay(5);
  writeRegHw(0x2C, 0x0E); // 3200 Hz ODR
  writeRegHw(0x2D, 0x08); // Measure On
  delay(10);
  
  Serial.println("ADXL345 30-Band Equalizer Initialized.");
  delay(1000);
}

void loop(){  
  int16_t x, y, z;
  readXYZBurst(&x, &y, &z);

  // 用 总加速度
  double total_accel = sqrt((double)x * x + (double)y * y + (double)z * z);
  vReal[sampleCounter] = total_accel;
  // 用 x,y,z 轴时间序列数据填充数组
  // vReal[sampleCounter] = (double)x;     
     
  vImag[sampleCounter] = 0.0;           
  sampleCounter++;

  if (sampleCounter >= SAMPLES) {
    FFT.dcRemoval(); 
    FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
    FFT.compute(FFTDirection::Forward);
    FFT.complexToMagnitude();

    // 刷新率控制器：强制降低打印速率到 5 FPS (每 200 毫秒刷新一次屏幕)
    static unsigned long last_print_time = 0;
    if (millis() - last_print_time >= 200) {
      last_print_time = millis();

      // 发送 35 行空行，实现顺畅的原地刷新动画
      for(int clear = 0; clear < 35; clear++) Serial.println();

      Serial.println("======= ADXL345 HIGH-SPEED 30-CHANNEL SPECTRUM VISUALIZER =======");

      int start_index = 2;    // 从第 2 个 bin (12.5Hz) 开始，避开最低端缓慢温漂
      int bins_per_band = 8;  // 每个通道合并 8 个数据桶 (8 * 6.25Hz = 50Hz 跨度)
      const int TOTAL_BANDS = 30;

      for (int band = 0; band < TOTAL_BANDS; band++) {
        int current_start = start_index + (band * bins_per_band);
        
        // 限制超出 1600Hz (256 bins) 的越界保护
        if (current_start >= (SAMPLES / 2)) break;

        // 计算当前 50Hz 范围通道的起始和终止频率
        int start_freq = (current_start * SAMPLING_FREQUENCY) / SAMPLES;
        int end_freq = ((current_start + bins_per_band) * SAMPLING_FREQUENCY) / SAMPLES;

        // 字符格式排版自适应强制左对齐
        if (start_freq < 10)       Serial.print("   ");
        else if (start_freq < 100)  Serial.print("  ");
        else if (start_freq < 1000) Serial.print(" ");
        Serial.print(start_freq);
        Serial.print("-");
        Serial.print(end_freq);
        if (end_freq < 100)         Serial.print(" Hz  : ");
        else if (end_freq < 1000)   Serial.print(" Hz : ");
        else                        Serial.print(" Hz: ");

        // 提取当前 50Hz 通道窗体内的最大波形能量
        double max_energy = 0;
        for (int b = 0; b < bins_per_band; b++) {
          if ((current_start + b) < (SAMPLES / 2)) {
            if (vReal[current_start + b] > max_energy) {
              max_energy = vReal[current_start + b];
            }
          }
        }

        //at 2g range, raw value -512~512
        //max total 0~886
        //fft multiply signal amp by sample/2 (256 for 512 sample)
        //max_energy_physical=886×256≈226816 实际很难达到

        // 噪底防御过滤：若特征幅值小于 50.0 噪底，判定为静止状态不触发显示
        if (max_energy < 50.0) max_energy = 0.0;

        // 将振幅幅度值缩放映射为等离子字符条长度
        // max:60/0.015=4000
        // max:60/0.005=12000
        // max:60/0.0015=40000
        int bar_length = (int)(max_energy * 0.0015); 

        if (bar_length > 60) bar_length = 60;  //max 60 blocks of graph

        // 渲染打印特征直方图条块
        for (int b = 0; b < bar_length; b++) {
          Serial.print("█");
        }
        Serial.println(); 
      }
    }

    // 重置 512 点大计数器
    sampleCounter = 0;
  }

  delayMicroseconds(260); 
}

void writeRegHw(byte reg, byte value) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(reg & 0x3F); 
  SPI.transfer(value);      
  digitalWrite(CS_PIN, HIGH);
}

void readXYZBurst(int16_t *x, int16_t *y, int16_t *z) {
  byte buffer[6]; // Standard fixed array size
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0xF2); 
  for(int i = 0; i < 6; i++) {
    buffer[i] = SPI.transfer(0x00);
  }
  digitalWrite(CS_PIN, HIGH);
  *x = (int16_t)((buffer[1] << 8) | buffer[0]);
  *y = (int16_t)((buffer[3] << 8) | buffer[2]);
  *z = (int16_t)((buffer[5] << 8) | buffer[4]);
}
