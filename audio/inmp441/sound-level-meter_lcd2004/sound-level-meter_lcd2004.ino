/*
 * ====================================================================
 *              ESP32 HARDWARE CONNECTIONS WITH LCD2004
 * ====================================================================
 * I2S Microphone (e.g., INMP441) Connections:
 *   ESP32 Pin     -->   Microphone Pin
 *   GPIO 26       -->   SCK / BCLK
 *   GPIO 25       -->   WS  / LRCLK
 *   GPIO 33       -->   SD  / DATA
 *   3V3           -->   VDD / VCC
 *   GND           -->   GND
 *   GND           -->   L/R (Lock to Mono Left Channel)
 * 
 * LCD2004 I2C Display Connections:
 *   ESP32 Pin     -->   LCD I2C Module Pin
 *   GPIO 21       -->   SDA
 *   GPIO 22       -->   SCL
 *   5V            -->   VCC (Ensures good LCD contrast)
 *   GND           -->   GND
 * ====================================================================
 */

#include "AudioTools.h"
#include "arduinoFFT.h"
#include <math.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "BigFont.h"

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

const double DB_CALIBRATION = 7.0; 

// Initialize LCD display at I2C address 0x27 with 20 columns and 4 rows
LiquidCrystal_I2C lcd(0x27, 20, 4);
BigFont font3x3(3);
BigFont font4x4(4);

// Tracking state variables for the 10-second rolling peak window
double current_second_max = 0.0;
double history_10s[10] = {30.0, 30.0, 30.0, 30.0, 30.0, 30.0, 30.0, 30.0, 30.0, 30.0};
uint8_t history_index = 0;
unsigned long last_second_timestamp = 0;

// Render storage values to prevent display flickering
int last_rendered_rt = -1;
int last_rendered_max = -1;

double getAWeighting(double frequency) {
  if (frequency <= 0) return -100.0;
  double f2 = frequency * frequency;
  double f4 = f2 * f2;
  double num = 12194.0 * 12194.0 * f4;
  double den = (f2 + 20.6 * 20.6) * sqrt((f2 + 107.7 * 107.7) * (f2 + 737.9 * 737.9)) * (f2 + 12194.0 * 12194.0);
  double RA = num / den;
  return (20.0 * log10(RA) + 2.0);
}

void setup() {
  Serial.begin(115200);
  while(!Serial); 
  
  // Initialize LCD display and turn on backlight
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Assign custom font graphics generation to the LCD 
  font4x4.loadchars(lcd);
  
  auto mic_config = i2s_mic.defaultConfig(RX_MODE);
  mic_config.copyFrom(info);
  mic_config.pin_bck = I2S_SCK;
  mic_config.pin_ws = I2S_WS;
  mic_config.pin_data = I2S_MIC_SD; 
  mic_config.i2s_format = I2S_STD_FORMAT; 
  i2s_mic.begin(mic_config);

  //static label

  // Left Label above the 3x3 Block
  lcd.setCursor(0, 0);
  lcd.print("10s max");

  // Stacked Middle Text Labels (Centered beautifully in the 6-column gap)
  lcd.setCursor(8, 1);
  lcd.print("real");  

  lcd.setCursor(8, 2);
  lcd.print("time");

  lcd.setCursor(9, 3); // Indented 1 space for visual balance
  lcd.print("db");

  last_second_timestamp = millis();
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

  double total_weighted_energy = 0.0;

  for (int i = 2; i < (SAMPLES / 2); i++) {
    double frequency_hz = (i * SAMPLING_FREQUENCY) / SAMPLES;
    if (frequency_hz > 20000.0) break;

    double raw_magnitude = vReal[i];
    double a_weight = getAWeighting(frequency_hz);
    double weight_multiplier = pow(10.0, a_weight / 20.0);
    double weighted_magnitude = raw_magnitude * weight_multiplier;
    
    total_weighted_energy += (weighted_magnitude * weighted_magnitude);
  }

  double rms_pressure = sqrt(total_weighted_energy / SAMPLES);
  double dBA = 30.0;
  if (rms_pressure > 0.0) {
    dBA = 20.0 * log10(rms_pressure) - DB_CALIBRATION;
  }
  if (dBA < 30.0) dBA = 30.0;
  if (dBA > 99.0) dBA = 99.0; // Enforce 2-digit limit for clean 4x4 fitting boundaries

  // Output plain numeric value to Serial
  Serial.println(dBA, 1);

  // Update current temporary maximum tracker
  if (dBA > current_second_max) {
    current_second_max = dBA;
  }

  unsigned long current_time = millis();
  
  // Every 1 second (1000ms), rotate and update the rolling history windows
  if (current_time - last_second_timestamp >= 1000) {
    last_second_timestamp = current_time;

    // 1. Capture the 1-second max for real-time display before resetting it
    int display_rt = (int)round(current_second_max);

    // 2. Shift history and push current second to the array
    history_10s[history_index] = current_second_max;
    history_index = (history_index + 1) % 10;

    // 3. Derive the global absolute peak over the 10-second history window arrays
    double max_10s = 30.0;
    for (int i = 0; i < 10; i++) {
      if (history_10s[i] > max_10s) {
        max_10s = history_10s[i];
      }
    }
    int display_max = (int)round(max_10s);
    
    // 4. Render Max 10s Window (Columns 0 and 3) if changed
    if (display_max != last_rendered_max) {
      last_rendered_max = display_max;
      int max_tens = display_max / 10;
      int max_ones = display_max % 10;
      
      font3x3.loadchars(lcd); // Dynamic character swap for 3x3 font map
      font3x3.printbigchar(lcd, max_tens, 0, 1);
      font3x3.printbigchar(lcd, max_ones, 3, 1);
    }

    // 5. Render Realtime Level on the RIGHT side (Columns 12 and 16) if changed
    if (display_rt != last_rendered_rt) {
      last_rendered_rt = display_rt;
      int rt_tens = display_rt / 10;
      int rt_ones = display_rt % 10;
      
      font4x4.loadchars(lcd); // Dynamic character swap for 4x4 font map
      font4x4.printbigchar(lcd, rt_tens, 12, 0);
      font4x4.printbigchar(lcd, rt_ones, 16, 0);
    }

    // 6. Reset maximum accumulator with current baseline for the next 1-second cycle
    current_second_max = dBA;
  }
}
