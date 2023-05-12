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

#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
#define SCL_PLOT 0x03

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
  // int tStart=millis();

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

  // int tEnd=millis();
  // Serial.println(tEnd-tStart);

  FFT = arduinoFFT(vReal, vImag, samples, samplingFrequency); /* Create FFT object */
  // /* Print the results of the sampling according to time */
  // Serial.println("Data:");
  // PrintVector(vReal, samples, SCL_TIME);

  FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);            /* Weigh data */
  // Serial.println("Weighed data:");
  // PrintVector(vReal, samples, SCL_TIME);

  FFT.Compute(FFT_FORWARD);                                   /* Compute FFT */
  // Serial.println("Computed Real values:");
  // PrintVector(vReal, samples, SCL_INDEX);
  // Serial.println("Computed Imaginary values:");
  // PrintVector(vImag, samples, SCL_INDEX);

  FFT.ComplexToMagnitude();                                   /* Compute magnitudes */
  // Serial.println("Computed magnitudes:");
  // PrintVector(vReal, (samples >> 1), SCL_FREQUENCY);

  // int peak=FFT.MajorPeak();
  // Serial.print(peak);
  // Serial.print(" ");
  // int vM=0;
  // for(int p=0;p<samples >> 1;p++)
  // {
  //   if(vReal[p]>vM) vM=vReal[p];
  // }
  // Serial.print(vM);
  // Serial.println();

  // while(true);


  int xMultiplier=1;//set this to something > 1 to print same data to multiple x so that the chart timeline looks faster
  for(int k=0;k<xMultiplier;k++)
  {
    //----print to screen-----
    tft.drawLine(x, 0, x, 239, ST77XX_BLACK);  //clear vertical line

    //there are samples/2 magnitude data
    //ranged from 0 hz to samplingFrequency / 2 hz

    //magnitude range from 0 to 370k (max) or 100k (typical high) at adc0~4096   WHY??


    if(samples==512)
    {
      //sample 512, mag 256, y240

      // //default
      // for (int y = 239; y >= 0; y--) {
      //   tft.drawPixel(x, y, vReal[239 - y]);
      // }

      //monochrome
      for (int y = 239; y >= 0; y--) {
        int g=vReal[239 - y]/100000*32;
        if(g>32)g=32;
        int color=g*32*64+g*32*2+g;
        tft.drawPixel(x, y, color);
      }
    }
    else if (samples==256)
    {
      //sample 256, mag 128, y120
      for (int y = 239; y >= 0; y=y-2) {
        tft.drawPixel(x, y, vReal[119 - (y-1)/2]);
        tft.drawPixel(x, y-1, vReal[119 - (y-1)/2]);
      }
    }
    else if (samples==128)
    {
      //sample 128, mag 64, y60
      for (int y = 239; y >= 0; y=y-4) {
        tft.drawLine(x, y,x,y+3, vReal[60 - (y+1)/4]);
      }
    }

    if(k!=xMultiplier-1) x++;
  }

  x++;
  if (x >= 320) x = 0;
}

void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType)
{
  for (uint16_t i = 0; i < bufferSize; i++)
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType)
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
	break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
	break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
	break;
    }
    Serial.print(abscissa, 6);
    if(scaleType==SCL_FREQUENCY)
      Serial.print("Hz");
    Serial.print(" ");
    Serial.println(vData[i], 4);
  }
  Serial.println();
}
