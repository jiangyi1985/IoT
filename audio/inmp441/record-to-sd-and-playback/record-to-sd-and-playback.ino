/**
 * INMP441 Studio Quality Recorder & Playback with Digital Gain
 * Requirements: Pair with isolated power layout to prevent brownouts.
 */

/* 
 * 连接方式 (Record and Playback Setup with Power Isolation)
 * INMP441      MAX98357A      HW-125 (SD)    Main ESP32     Second ESP32
 * SCK          BCLK           -              GPIO26         -
 * WS           LRC            -              GPIO25         -
 * SD           -              -              GPIO33         -
 * -            DIN            -              GPIO22         -
 * L/R          -              -              GND            -
 * GND          GND            GND            GND            GND (Connect all GNDs!)
 * VDD          -              -              3.3V           -
 * -            -              CS             GPIO5          -
 * -            -              SCK            GPIO18         -
 * -            -              MISO           GPIO19         -
 * -            -              MOSI           GPIO23         -
 * -            -              VCC            5V / VIN       -
 * -            VIN            -              -              5V / VIN (Power Source)
 */

#include "AudioTools.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define SD_CS          5
#define I2S_SCK        26
#define I2S_WS         25
#define I2S_MIC_SD     33
#define I2S_AMP_SD     22

// Studio Quality Audio Profile (44.1kHz, Mono, 16-bit)
AudioInfo info(44100, 1, 16); 

I2SStream i2s_mic;             // Hardware input from Microphone
I2SStream i2s_amp;             // Hardware output to Speaker Amplifier

WAVEncoder wav_encoder;        // WAV Formatting engine
WAVDecoder wav_decoder;        // WAV Decoding engine
File audio_file;         

// Audio Processing Pipeline
EncodedAudioStream encoderStream(&audio_file, &wav_encoder); // Formats WAV and sends to file
VolumeStream volume_booster(encoderStream);                  // Applies digital volume gain
EncodedAudioStream decoderStream(&i2s_amp, &wav_decoder);     // Decodes WAV and sends to speaker

StreamCopy copier;             

void setup() {
  Serial.begin(115200);
  while(!Serial);
  
  Serial.println("Initializing SD Card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card Mount Failed!");
    while (true);
  }

  // --- PHASE 1: HIGH-GAIN STUDIO RECORDING ---
  Serial.println("\n--- RECORDING PHASE ---");
  
  // 1. FIX: Manually pull the speaker data pin LOW and set it as an output.
  // This forces the MAX98357A to enter silent standby mode during recording!
  pinMode(I2S_AMP_SD, OUTPUT);
  digitalWrite(I2S_AMP_SD, LOW);

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

  encoderStream.begin(info);
  
  auto volume_config = volume_booster.defaultConfig();
  volume_config.copyFrom(info);
  volume_booster.begin(volume_config);
  volume_booster.setVolume(10.0); 

  copier.begin(volume_booster, i2s_mic);

  Serial.println(">>> RECORDING STARTED! Speaker is muted. Speak clearly...");
  unsigned long start_time = millis();
  while (millis() - start_time < 5000) {
    copier.copy(); 
  }

  // Finalize recording
  volume_booster.end();
  encoderStream.end();
  audio_file.flush(); 
  audio_file.close();
  i2s_mic.end();
  copier.end();
  Serial.println(">>> Recording Saved. Transitioning to speaker lines...");

  // --- PHASE 2: AUDIO PLAYBACK ---
  Serial.println("\n--- PLAYBACK PHASE ---");
  auto amp_config = i2s_amp.defaultConfig(TX_MODE);
  amp_config.copyFrom(info);
  amp_config.pin_bck = I2S_SCK;
  amp_config.pin_ws = I2S_WS;
  amp_config.pin_data = I2S_AMP_SD; // 2. This hands control of the pin back to the I2S engine!
  i2s_amp.begin(amp_config);

  audio_file = SD.open("/test.wav", FILE_READ);
  if (!audio_file) {
    Serial.println("Failed to open file for reading!");
    while(true);
  }

  decoderStream.begin(info);
  copier.begin(decoderStream, audio_file);
  Serial.println(">>> Looping recording back through the speaker...");
}

void loop() {
  // Push decoding streams continuously until the track hits the end
  if (!copier.copy()) {
    Serial.println("End of file track. Re-spooling loop...");
    copier.end();
    audio_file.close();
    delay(1000); // 1-second pause between playbacks
    
    audio_file = SD.open("/test.wav", FILE_READ);
    copier.begin(decoderStream, audio_file);
  }
}
