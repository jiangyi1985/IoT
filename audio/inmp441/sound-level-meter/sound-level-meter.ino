/*
 * ====================================================================
 *              ESP32 I2S MICROPHONE PIN CONNECTIONS
 * ====================================================================
 * Connect your I2S Microphone (e.g., INMP441, SPH0645) to the ESP32
 * using the following hardware layout:
 * 
 *   ESP32 Pin     -->   Microphone Pin    [Function Name in Code]
 *   ------------------------------------------------------------
 *   GPIO 26       -->   SCK / BCLK        [I2S_SCK]
 *   GPIO 25       -->   WS  / LRCLK       [I2S_WS]
 *   GPIO 33       -->   SD  / DATA        [I2S_MIC_SD]
 *   3V3           -->   VDD / VCC         [Power Supply]
 *   GND           -->   GND               [Ground]
 *   GND           -->   L/R               [Left Channel Select]*
 * 
 * * CRITICAL NOTE: The L/R (Left/Right) pin must be physically wired to 
 *   GND. This locks the microphone to the Left channel to match the 
 *   Mono (1 channel) setting configured in your audio setup.
 * ====================================================================
 */

/**
 * ESP32 I2S Microphone Real-Time A-Weighted Sound Level Meter (dBA)
 * Formatted as a rock-solid, frozen terminal display running at 5 FPS.
 */

#include "AudioTools.h"
#include "arduinoFFT.h"
#include <math.h>

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

// Calibration constant to align the digital mic range to real-world decibels (~35-40dB room noise floor)
const double DB_CALIBRATION = 5.0; 

// Official IEC 61672-1 standard A-weighting correction formula function
double getAWeighting(double frequency) {
  if (frequency <= 0) return -100.0; // Avoid log10 errors at 0Hz
  
  double f2 = frequency * frequency;
  double f4 = f2 * f2;
  
  // IEC Standard equations modeling the physical acoustics of the human ear canal
  double num = 12194.0 * 12194.0 * f4;
  double den = (f2 + 20.6 * 20.6) * sqrt((f2 + 107.7 * 107.7) * (f2 + 737.9 * 737.9)) * (f2 + 12194.0 * 12194.0);
  
  double RA = num / den;
  double dBA_offset = 20.0 * log10(RA) + 2.0; // The standard shifts the relative scale to 0dB at 1kHz
  
  return dBA_offset;
}

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

  Serial.println("A-Weighted Sound Level Meter Ready! Open Serial Monitor (Set to 921600 Baud).");
  delay(1000);
}

void loop() {
  int16_t raw_samples[SAMPLES];
  size_t bytes_read = i2s_mic.readBytes((uint8_t *)raw_samples, SAMPLES * sizeof(int16_t));
  int samples_read = bytes_read / sizeof(int16_t);

  if (samples_read < SAMPLES) return; 

  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = (double)raw_samples[i]; 
    vImag[i] = 0.0;
  }

  FFT.dcRemoval();
  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(FFT_FORWARD);
  FFT.complexToMagnitude();

  // Accumulate the A-weighted sound energy across all human hearing frequency buckets
  double total_weighted_energy = 0.0;

  // We loop from index 2 up to the Nyquist limit (SAMPLES / 2)
  for (int i = 2; i < (SAMPLES / 2); i++) {
    double frequency_hz = (i * SAMPLING_FREQUENCY) / SAMPLES;
    
    // Ignore everything past the human hearing range limit (20,000 Hz)
    if (frequency_hz > 20000.0) break;

    double raw_magnitude = vReal[i];
    
    // Fetch the structural decibel weighting offset for this specific frequency pitch (e.g., -20dB at 100Hz)
    double a_weight = getAWeighting(frequency_hz);
    
    // Convert the weighting dB offset back into a raw scalar multiplier fraction
    double weight_multiplier = pow(10.0, a_weight / 20.0);
    
    // Apply the multiplier filter directly to the frequency energy bundle
    double weighted_magnitude = raw_magnitude * weight_multiplier;
    
    // Square the amplitude values to sum total power correctly (Root-Mean-Square acoustic law)
    total_weighted_energy += (weighted_magnitude * weighted_magnitude);
  }

  // Calculate the Root Mean Square across the calculated block frame window
  double rms_pressure = sqrt(total_weighted_energy / SAMPLES);

  // Convert the acoustic pressure value into log-scale Decibels (dBA)
  double dBA = 0.0;
  if (rms_pressure > 0.0) {
    dBA = 20.0 * log10(rms_pressure) - DB_CALIBRATION;
  }
  
  // Floor the value at standard quiet room ambient thresholds to prevent negative noise scales
  if (dBA < 30.0) dBA = 30.0;

  // --- REFRESH RATE CONTROLLER (5 FPS) ---
  static unsigned long last_print_time = 0;
  if (millis() - last_print_time >= 200) {
    last_print_time = millis();

    // Clear 15 lines to cleanly overwrite the previous reading frame window
    for(int clear = 0; clear < 15; clear++) Serial.println();

    Serial.println("==================================================");
    Serial.println("       REAL-TIME A-WEIGHTED SOUND LEVEL METER     ");
    Serial.println("==================================================");
    Serial.println();
    
    // Print the numeric dBA value
    Serial.print("  SOUND LEVEL: ");
    Serial.print(dBA, 1); // 1 decimal point precision
    Serial.println(" dBA");
    Serial.println();

    // Generate a visual sound volume progress level indicator bar
    Serial.print("  [");
    // Map decibel ranges 30-90dB smoothly to 40 graphical column blocks
    int bar_segments = map((int)dBA, 30, 90, 0, 40);
    if (bar_segments < 0) bar_segments = 0;
    if (bar_segments > 40) bar_segments = 40;

    for (int b = 0; b < bar_segments; b++) {
      // Transition from solid blocks to warning dashes if sound gets dangerously loud
      if (b > 30) Serial.print("▒"); // Danger zone (>75dBA)
      else Serial.print("█");        // Safe / normal zone
    }
    for (int space = bar_segments; space < 40; space++) {
      Serial.print(" ");
    }
    Serial.println("]");
    Serial.println();
    Serial.println("==================================================");
  }
}
