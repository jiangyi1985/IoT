/**
 * Simplified ESP32 I2S Microphone Volume Plotter
 * Uses your exact same 44.1kHz setup with a static 10.0x gain multiplier!
 */

#include "AudioTools.h"

// Matching your exact hardware pin definitions
#define I2S_SCK        26
#define I2S_WS         25
#define I2S_MIC_SD     33

// Your exact 44.1kHz Studio Quality Profile
AudioInfo info(44100, 1, 16); 

I2SStream i2s_mic;             
StreamCopy copier;             

// Your exact gain value from the playback script
const float AUDIO_GAIN = 10.0; 

void setup() {
  Serial.begin(115200);
  while(!Serial); 
  
  Serial.println("Configuring I2S Microphone for Serial Plotter...");

  auto mic_config = i2s_mic.defaultConfig(RX_MODE);
  mic_config.copyFrom(info);
  mic_config.pin_bck = I2S_SCK;
  mic_config.pin_ws = I2S_WS;
  mic_config.pin_data = I2S_MIC_SD; 
  mic_config.i2s_format = I2S_STD_FORMAT; 
  i2s_mic.begin(mic_config);

  Serial.println("Plotter Ready! Open Tools -> Serial Plotter.");
}

void loop() {
  // 1. 将块大小（BLOCK_SIZE）缩小到 64。
  // 这样 ESP32 处理速度极快，不会漏掉任何一个快速、短暂的字。
  const int BLOCK_SIZE = 64;
  int16_t samples[BLOCK_SIZE]; 
  
  // 从麦克风队列中读取数据
  size_t bytes_read = i2s_mic.readBytes((uint8_t *)samples, BLOCK_SIZE * sizeof(int16_t));
  int samples_read = bytes_read / sizeof(int16_t);

  if (samples_read > 0) {
    float total_volume = 0;

    for (int i = 0; i < samples_read; ++i) {
      // 累加绝对值（剥离正负波形，避免它们互相抵消）
      total_volume += abs(samples[i]);
    }

    float average_volume = total_volume / samples_read;

    // 乘以你录音里完全相同的 10.0 倍增益
    float sensitive_volume = average_volume * AUDIO_GAIN;

    // 2. 降低Y轴天花板：将原来的 100000 压缩到 25000 
    // 这相当于把放大镜聚焦在安静区域，让细微的小声音在屏幕上看起来也很大、很明显！
    Serial.print(0);
    Serial.print(",");
    Serial.print(25000); 
    Serial.print(",");
    Serial.println(sensitive_volume);
  }

  // 3. 彻底删除了 delay(30)！
  // 因为 44.1kHz 的音频流自带完美的硬件速度限制，不加 delay 才能保证 100% 抓住你说的每个字。
}
