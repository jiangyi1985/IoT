//TFT80x160 UNO NodeMCU-32S
//SCL       13  18
//SDA       11  23
//RST       9   16
//DC        8   17
//CS        10  SS(5)
//BLK

#include <Adafruit_GFX.h>  // Core graphics library //https://github.com/adafruit/Adafruit-GFX-Library
// #include "Adafruit_ST7735.h" // Hardware-specific library for ST7735 //https://github.com/adafruit/Adafruit-ST7735-Library //MODIFIED for 金逸晨 0.96-8pin13p_ips
#include "Adafruit_ST7789.h"  // Hardware-specific library for ST7789
#include <SPI.h>
#include "arduinoFFT.h"  //https://github.com/kosme/arduinoFFT

#define NODEMCU32S
#if defined(NODEMCU32S)
#define TFT_CS SS
#define TFT_RST 16
#define TFT_DC 17
#elif defined(ESP8266)
#define TFT_CS 4
#define TFT_RST 16
#define TFT_DC 5
#else
// For the breakout board, you can use any 2 or 3 pins.
// These pins will also work for the 1.8" TFT shield.
#define TFT_CS 10
#define TFT_RST 9  // Or set to -1 and connect to Arduino RESET pin
#define TFT_DC 8
#endif

// For 1.14", 1.3", 1.54", 1.69", and 2.0" TFT with ST7789:
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// OR for the ST7789-based displays, we will use this call
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

#define CHANNEL 32
const uint16_t samples = 512;            //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 10000;  //Hz, must be less than 10000 due to ADC
unsigned int sampling_period_us;
unsigned long microseconds;
double vReal[samples];
double vImag[samples];
arduinoFFT FFT = arduinoFFT();

void setup(void) {
  Serial.begin(115200);
  Serial.print("TFT_CS:");
  Serial.println(TFT_CS);
  Serial.print("TFT_DC:");
  Serial.println(TFT_DC);
  Serial.print("TFT_RST:");
  Serial.println(TFT_RST);
  Serial.println(F("Hello! ST77xx TFT Test"));

  //for GMT130-v1.0:
  tft.init(240, 320);  //SPI data mode; one of SPI_MODE0, SPI_MODE1, SPI_MODE2 or SPI_MODE3 (do NOT pass the numbers 0,1,2 or 3 -- use the defines only, the values are NOT the same!)
  tft.setRotation(3);

  Serial.print("width:");
  Serial.println(tft.width());
  Serial.print("height:");
  Serial.println(tft.height());
  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
  Serial.print("fft sampling_period_us:");
  Serial.println(sampling_period_us);
}

unsigned long t = 0, t1, diff;
int sum = 0, sum_diff = 0, count = 0, range = 0;
double maxV = 0, minV = 9999, avg = 0, avg_diff, variance, std_dev, square_sum_diff;

int x = 0;
short lastMaxV[320];
short lastMinV[320];
short lastMax = 0, lastMin = 0;

void loop() {
  int tStart=millis();

  /*SAMPLING*/
  microseconds = micros();//Overflows after around 70 minutes!
  for (int i = 0; i < samples; i++) {
    vReal[i] = analogRead(CHANNEL);
    vImag[i] = 0;
    while (micros() - microseconds < sampling_period_us) {
      //empty loop
    }
    microseconds += sampling_period_us;
  }

  int tEnd=millis();
  Serial.println(tEnd-tStart);

  FFT = arduinoFFT(vReal, vImag, samples, samplingFrequency); /* Create FFT object */
  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);            /* Weigh data */
  FFT.Compute(FFT_FORWARD);                                   /* Compute FFT */
  FFT.ComplexToMagnitude();                                   /* Compute magnitudes */

  //----print to screen-----
  tft.drawLine(x, 0, x, 239, ST77XX_BLACK);  //clear vertical line

  //there are samples/2 magnitude data
  for (int y = 239; y >= 0; y--) {
    tft.drawPixel(x, y, vReal[239 - y]);
  }

  x++;
  if (x >= 320) x = 0;
}
