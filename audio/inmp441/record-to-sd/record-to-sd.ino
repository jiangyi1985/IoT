/**
 * INMP441 High-Gain & Studio Quality Recording Test
 * Boosts voice volume and audio clarity directly on the SD card!
 * Fixed constructor chaining syntax for arduino-audio-tools 1.2.6+
 */

/* 
 * 连接方式 (Recording Only Setup)
 * INMP441      HW-125 (SD)    NodeMcu-32S
 * SCK          -              GPIO26
 * WS           -              GPIO25
 * SD           -              GPIO33
 * L/R          -              GND
 * GND          GND            GND
 * VDD          -              3.3V
 * -            CS             GPIO5
 * -            SCK            GPIO18
 * -            MISO           GPIO19
 * -            MOSI           GPIO23
 * -            VCC            5V / VIN
 */

#include "AudioTools.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_CS          5
#define I2S_SCK        26
#define I2S_WS         25
#define I2S_MIC_SD     33

// 1. Upgraded Sample Rate to 44.1kHz (Studio/CD Quality)
AudioInfo info(44100, 1, 16); 

I2SStream i2s_mic;             
WAVEncoder wav_encoder;        
File audio_file;         

// 2. Chain the Encoder Stream to write data straight into the file pointer
EncodedAudioStream encoderStream(&audio_file, &wav_encoder); 

// 3. CORRECT CHAINING: Pass the downstream target directly into the Volume constructor
VolumeStream volume_booster(encoderStream); 

StreamCopy copier;             

void setup() {
  Serial.begin(115200);
  while(!Serial);
  
  Serial.println("Initializing SD Card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card Mount Failed!");
    while (true);
  }

  Serial.println("\n--- HIGH-GAIN RECORDING ---");
  auto mic_config = i2s_mic.defaultConfig(RX_MODE);
  mic_config.copyFrom(info);
  mic_config.pin_bck = I2S_SCK;
  mic_config.pin_ws = I2S_WS;
  mic_config.pin_data = I2S_MIC_SD; 
  mic_config.i2s_format = I2S_STD_FORMAT; 
  i2s_mic.begin(mic_config);

  SD.remove("/test.wav");
  audio_file = SD.open("/test.wav", FILE_WRITE);
  if (!audio_file) {
    Serial.println("Failed to open file for writing!");
    while(true);
  }

  // Initialize encoder pipeline
  encoderStream.begin(info);

  // 4. Configure and activate the volume processing stream configuration layout
  auto volume_config = volume_booster.defaultConfig();
  volume_config.copyFrom(info);
  volume_booster.begin(volume_config);
  
  // Set gain level (4.0 means the sound amplitude is multiplied by 4x)
  // If the recording clips or distorts on your PC, change this value down to 2.5 or 3.0
  volume_booster.setVolume(10.0); 

  // Point the copier tool to pull from mic and push directly into the booster pipeline
  copier.begin(volume_booster, i2s_mic);

  Serial.println(">>> RECORDING STARTED! Speak normally near the microphone...");
  
  unsigned long start_time = millis();
  while (millis() - start_time < 5000) {
    copier.copy(); 
  }

  // Force close and flush pipelines to save file securely
  volume_booster.end();
  encoderStream.end();
  audio_file.flush(); 
  audio_file.close();
  i2s_mic.end();
  copier.end();
  
  Serial.println(">>> SUCCESS! The boosted file is permanently saved.");
}

void loop() {
  // Do nothing
}
