//export music with Audacity/Audition
//convert to c data array https://tomeko.net/online_tools/file_to_hex.php?lang=en

// ESP32  | MAX98357  	
// -------|---------
// 5V     | VCC  	
// GND    | GND  	
// GPIO14 | BCK  	
// GPIO22 | DIN  	
// GPIO15 | LCK  	
// GPIO16 | SD(Shutdown)

//Compile with Partition Scheme Hughe APP if not enough space

#include "AudioTools.h"

// #include "door-open-1-click-low_22050-1-16.h"
// #include "door-open-1-click-high_22050-1-16.h"
#include "door-open-2-beep_22050-1-16.h"

const int SD_PIN = 16;
AudioInfo info(22050, 1, 16);  //sample rate, channels, bits per sample
// AudioInfo info(8000, 1, 8);

I2SStream i2s;             // Output to I2S
VolumeStream volume(i2s);  // Wrap I2S with volume control
MemoryStream music(sample, sample_length);
StreamCopy copier(volume, music);  // Copy music -> volume -> i2s

void setup() {
  Serial.begin(115200);
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

  pinMode(SD_PIN, OUTPUT);
  digitalWrite(SD_PIN, LOW);  // Start with the amp OFF to avoid startup pops

  auto config = i2s.defaultConfig(TX_MODE);
  config.copyFrom(info);
  // config.pin_ws = 15;    // LRC
  // config.pin_bck = 14;   // BCLK
  // config.pin_data = 22;  // DIN
  i2s.begin(config);

  // Setup Volume
  auto vcfg = volume.defaultConfig();
  vcfg.copyFrom(config);
  volume.begin(vcfg);

  volume.setVolume(0.4);  // Set to 30% volume
}

void loop() {
  delay(3000);

  digitalWrite(SD_PIN, HIGH);
  while (copier.copy()) {
    yield();  // Keeps the ESP32 background tasks (WiFi/Watchdog) alive
  }
  digitalWrite(SD_PIN, LOW);
  // copier.end();  //if calls, it won't loop
  // i2s.flush();// Add this to clear the I2S internal buffers and prevent static //not working

  delay(7000);

  music.begin();
  copier.begin();
}