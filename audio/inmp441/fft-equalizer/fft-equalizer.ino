/**
 * Ultra-Stable 36-Band Full-Range Equalizer (0 ~ 10,000 Hz)
 * Renders 36 distinct frequency channels at a stable 5 frames per second.
 */

#include "AudioTools.h"
#include "arduinoFFT.h"

#define I2S_SCK        26
#define I2S_WS         25
#define I2S_MIC_SD     33

AudioInfo info(44100, 1, 16); 
I2SStream i2s_mic;             

const uint16_t SAMPLES = 2048; 
const double SAMPLING_FREQUENCY = 44100.0;

double vReal[SAMPLES];
double vImag[SAMPLES];

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

void setup() {
  Serial.begin(921600);
  while(!Serial); 
  
  auto mic_config = i2s_mic.defaultConfig(RX_MODE);
  mic_config.copyFrom(info);
  mic_config.pin_bck = I2S_SCK;
  mic_config.pin_ws = I2S_WS;
  mic_config.pin_data = I2S_MIC_SD; 
  mic_config.i2s_format = I2S_STD_FORMAT; 
  i2s_mic.begin(mic_config);

  Serial.println("36-Band 10kHz Equalizer Ready! Open Serial Monitor (Set to 921600 Baud).");
  delay(1000);
}

void loop() {
  int16_t raw_samples[SAMPLES];
  size_t bytes_read = i2s_mic.readBytes((uint8_t *)raw_samples, SAMPLES * sizeof(int16_t));
  int samples_read = bytes_read / sizeof(int16_t);

  if (samples_read < SAMPLES) return; 

  // Fixed array mapping syntax
  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = (double)raw_samples[i]; 
    vImag[i] = 0.0;
  }

  FFT.dcRemoval();
  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(FFT_FORWARD);
  FFT.complexToMagnitude();

  // --- REFRESH RATE CONTROLLER (5 FPS) ---
  static unsigned long last_print_time = 0;
  if (millis() - last_print_time >= 200) {
    last_print_time = millis();

    // Clear 42 blank rows to completely wipe the monitor for our 36-band chart
    for(int clear = 0; clear < 42; clear++) Serial.println();

    Serial.println("====== ROCK-SOLID 36-BAND FULL SPECTRA EQUALIZER ======");

    int start_index = 2;   // Start at 43Hz to skip the lowest DC offset row
    int bins_per_band = 13; // 13 bins * 21.53Hz = ~280Hz bandwidth per row channel block

    for (int band = 0; band < 36; band++) {
      int current_start = start_index + (band * bins_per_band);
      
      // Calculate approximate center frequency for this specific row label block
      int center_freq = ((current_start + (bins_per_band / 2)) * SAMPLING_FREQUENCY) / SAMPLES;

      // Text formatting layout alignment blocks
      if (center_freq < 100) Serial.print("    ");
      else if (center_freq < 1000) Serial.print("   ");
      else if (center_freq < 10000) Serial.print("  ");
      else Serial.print(" ");
      
      Serial.print(center_freq);
      Serial.print(" Hz: ");

      // Pull the highest energy signature hidden inside this 280Hz band window
      double max_energy = 0;
      for (int b = 0; b < bins_per_band; b++) {
        if ((current_start + b) < SAMPLES) {
          if (vReal[current_start + b] > max_energy) {
            max_energy = vReal[current_start + b];
          }
        }
      }

      // Convert the extracted magnitude level to clean graphic display bars
      int bar_length = (max_energy * 0.0006); 
      if (bar_length > 40) bar_length = 40;

      // Print the custom bar block elements
      for (int b = 0; b < bar_length; b++) {
        Serial.print("█");
      }
      Serial.println(); 
    }
  }
}
