// ESP32    MAX98357   MicroSD Module
// 5V       VCC  	     VCC               #use seperate power source for Max98357 to reduce NOISE
// GND      GND  	     GND               #make sure SD has a common GND with ESP when using seperate power source
// GPIO26   BCK  	
// GPIO27   DIN  	
// GPIO25   LCK  	
// GPIO23              MOSI
// GPIO19              MISO
// GPIO18              SCK
// GPIO 5              CS

#include <SD.h>
#include <SPI.h>
#include "AudioTools.h"

const int chipSelect = 5;
AudioInfo info(44100, 1, 16);
I2SStream i2s;
VolumeStream volume(i2s); 
WAVDecoder wav;
EncodedAudioOutput decoder(&volume, &wav);  // Decoding stream
StreamCopy copier;
File audioFile;

void setup() {
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  // setup file
  SD.begin(chipSelect);
  audioFile = SD.open("/test.wav");

  // setup i2s
  auto config = i2s.defaultConfig(TX_MODE);
  config.copyFrom(info);
  // config1.port_no = 0;  // 0 is default port  
  // Ensure these pins match your hardware and don't conflict with SD (Pin 5)
  config.pin_ws = 25; 
  config.pin_bck = 26;
  config.pin_data = 27;
  i2s.begin(config);
  
  volume.begin(config);
  volume.setVolume(0.3);

  // setup decoder
  decoder.begin();

  // begin copy
  copier.begin(decoder, audioFile);
}

void loop() {
  if (!copier.copy()) {
    stop();
  }
}
