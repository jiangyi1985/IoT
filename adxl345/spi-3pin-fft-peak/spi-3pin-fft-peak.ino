// =================== 3200 Hz 3线硬件 SPI + 512点 高精度 FFT ===================
// ADXL345 Board Pin   NodeMCU-32S Pin      Wiring Role
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

// 512 points @ 3200Hz gives an ultra-sharp 6.25 Hz frequency resolution step size
#define SAMPLES 512                   
#define SAMPLING_FREQUENCY 3200       

double vReal[SAMPLES];                
double vImag[SAMPLES];                
int sampleCounter = 0;

// Instantiate v2.x object passing memory array pointers
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

// Direct hardware forward declarations
void writeRegHw(byte reg, byte value);
void readXYZBurst(int16_t *x, int16_t *y, int16_t *z);

void setup(){
  Serial.begin(921600);                 // Stable maximum baud rate
  Serial.println();
  
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  // Initialize pure 4-wire hardware maps
  SPI.begin(SCL_PIN, MISO_PIN, MOSI_PIN, CS_PIN); 
  SPI.setDataMode(SPI_MODE3);
  SPI.setFrequency(4000000);            // 4 MHz high speed hardware clock

  // Force sensor into 3-Wire SPI Mode (Bit 6 = 1), 2g Range, Full Res -> 0x48
  writeRegHw(0x31, 0x48); 
  delay(5);
  
  writeRegHw(0x2C, 0x0E);               // Ramp internal sampling up to maximum 3200 Hz
  writeRegHw(0x2D, 0x08);               // Enable measurement stream
  delay(10);
  
  Serial.println("Vibration FFT Analyzer Fully Initialized.");
}

void loop(){  
  int16_t x, y, z;
  
  // 1. Gather the synchronous multi-axis frame from hardware
  readXYZBurst(&x, &y, &z);

  // 2. Continuous time-series streaming collection step
  vReal[sampleCounter] = (double)x;     // Track X-axis changes over time
  // vReal[sampleCounter] = (double)z;     // Track Z-axis changes over time
  vImag[sampleCounter] = 0.0;           // Keep imaginary plane completely clear
  sampleCounter++;

  // 3. Process the window ONLY when 512 actual consecutive samples are cached (every ~160ms)
  if (sampleCounter >= SAMPLES) {
    
    // Core Fix: Erase static gravity DC baseline components to clean up the 0Hz block
    FFT.dcRemoval(); 
    
    // Standard Fourier transforms using v2.x macros
    FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
    FFT.compute(FFTDirection::Forward);
    FFT.complexToMagnitude();
    
    // 4. Advanced Amplitude Noise-Floor Masking
    double majorPeak = 0.0;
    double maxMagnitude = 0.0;
    
    // Scan frequencies starting from Bin 6 (6 * 6.25Hz = 37.5 Hz)
    // This fully sweeps away any human cable handling sway or slow drift loops
    for (int i = 6; i < (SAMPLES / 2); i++) { 
      if (vReal[i] > maxMagnitude) {
        maxMagnitude = vReal[i];
      }
    }

    // Dynamic amplitude guard check:
    // If the dominant peak energy does not exceed 35.0, force a flat 0.0 Hz line
    if (maxMagnitude > 35.0) {
      majorPeak = FFT.majorPeak();
    } else {
      majorPeak = 0.0; 
    }
    
    // Fast logging vector line output
    Serial.print("Peak_Freq_Hz:");
    Serial.println(majorPeak);

    // Wipe index loop pointer block clean for the next data batch accumulation run
    sampleCounter = 0;
  }

  // Pure 3200Hz loop balancing constraint metric delay block (312us target window)
  delayMicroseconds(260); 
}

// Low-level hardware block transaction methods
void writeRegHw(byte reg, byte value) {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(reg & 0x3F); 
  SPI.transfer(value);      
  digitalWrite(CS_PIN, HIGH);
}

void readXYZBurst(int16_t *x, int16_t *y, int16_t *z) {
  byte buffer[6];
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0xF2); // Multi-byte sequential read command address tag
  for(int i = 0; i < 6; i++) {
    buffer[i] = SPI.transfer(0x00);
  }
  digitalWrite(CS_PIN, HIGH);
  
  // Reconstruct physical registers cleanly into true signed 16-bit bounds
  *x = (int16_t)((buffer[1] << 8) | buffer[0]);
  *y = (int16_t)((buffer[3] << 8) | buffer[2]);
  *z = (int16_t)((buffer[5] << 8) | buffer[4]);
}
