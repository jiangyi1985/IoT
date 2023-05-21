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
#include <Adafruit_AS7341.h>

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

Adafruit_AS7341 as7341;

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


  if (!as7341.begin()) {
    Serial.println("Could not find AS7341");
    while (1) { delay(10); }
  }

  as7341.setATIME(100);
  as7341.setASTEP(100);
  as7341.setGain(AS7341_GAIN_256X);
}

int seq = 1;

void loop() {
  uint16_t readings[12];
  uint16_t normDisplay[12];

  int channelColor[10];
  //as7341 datasheet characteristics color scheme
  channelColor[0] = rgbToColor16(112, 48, 160);
  channelColor[1] = rgbToColor16(0, 32, 96);
  channelColor[2] = rgbToColor16(0, 176, 240);
  channelColor[3] = rgbToColor16(0, 255, 255);
  channelColor[4] = rgbToColor16(0, 176, 80);
  channelColor[5] = rgbToColor16(255, 255, 0);
  channelColor[6] = rgbToColor16(255, 192, 0);
  channelColor[7] = rgbToColor16(255, 0, 0);
  channelColor[8] = rgbToColor16(99, 99, 99);  //clear
  channelColor[9] = rgbToColor16(192, 0, 0);   //NIR

  String channelDiscription[10];
  channelDiscription[0] = "415";
  channelDiscription[1] = "445";
  channelDiscription[2] = "480";
  channelDiscription[3] = "515";
  channelDiscription[4] = "555";
  channelDiscription[5] = "590";
  channelDiscription[6] = "630";
  channelDiscription[7] = "680";
  channelDiscription[8] = "CLR";
  channelDiscription[9] = "NIR";

  if (!as7341.readAllChannels(readings)) {
    Serial.println("Error reading all channels!");
    return;
  }

  // Serial.print(readings[0]);
  // Serial.print("\t");
  // Serial.print(readings[1]);
  // Serial.print("\t");
  // Serial.print(readings[2]);
  // Serial.print("\t");
  // Serial.print(readings[3]);
  // Serial.print("\t");
  // Serial.print(readings[4]);
  // Serial.print("\t");
  // Serial.print(readings[5]);
  // Serial.print("\t");
  // Serial.print(readings[6]);
  // Serial.print("\t");
  // Serial.print(readings[7]);
  // Serial.print("\t");
  // Serial.print(readings[8]);
  // Serial.print("\t");
  // Serial.print(readings[9]);
  // Serial.print("\t");
  // Serial.print(readings[10]);
  // Serial.print("\t");
  // Serial.print(readings[11]);
  // Serial.println();

  // tft.fillScreen(ST77XX_BLACK);

  double max = 10201.0;  //max value at 10201, WHY?

  int idxBarHeight = 199;

  for (int i = 0; i < 12; i++) {
    normDisplay[i] = readings[i] / max * idxBarHeight;
  }

  int idx = 0;
  int widthTotal = 30;
  int widthBar = 25;
  for (int i = 0; i < 12; i++) {
    if (i == 4 || i == 5) continue;  //skip clear/NIR duplicates
    int offset = i >= 10 ? 25 : 10;  //x offset
    int textToBarOffset = 4;

    //draw bar
    tft.fillRect(idx * widthTotal + offset, 0, widthBar, idxBarHeight - normDisplay[i], ST77XX_BLACK);
    tft.fillRect(idx * widthTotal + offset, idxBarHeight - normDisplay[i], widthBar, normDisplay[i], channelColor[idx]);

    //draw description
    if (seq == 1) {
      tft.setCursor(idx * widthTotal + offset + textToBarOffset, idxBarHeight + 10);
      // tft.setTextSize(1);
      tft.setTextColor(channelColor[idx]);
      // tft.setTextWrap(false);
      tft.print(channelDiscription[idx]);
    }

    //draw text values
    tft.fillRect(idx * widthTotal + offset + textToBarOffset, idxBarHeight + 10 + 15, widthBar, 15, ST77XX_BLACK);
    tft.setCursor(idx * widthTotal + offset + textToBarOffset, idxBarHeight + 10 + 15);
    // tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    // tft.setTextWrap(false);
    tft.print(readings[i]);

    idx++;
  }

  seq++;

  // //rgb from https://405nm.com/wavelength-to-color/
  // tft.fillRect(0,239-readings[0]/max*239,20,239,rgbToColor16(118,0,237));
  // tft.fillRect(20,239-readings[1]/max*239,20,239,rgbToColor16(0,40,255));
  // tft.fillRect(40,239-readings[2]/max*239,20,239,rgbToColor16(0,213,255));
  // tft.fillRect(60,239-readings[3]/max*239,20,239,rgbToColor16(31,255,0));
  // tft.fillRect(80,239-readings[6]/max*239,20,239,rgbToColor16(179,255,0));
  // tft.fillRect(100,239-readings[7]/max*239,20,239,rgbToColor16(255,223,0));
  // tft.fillRect(120,239-readings[8]/max*239,20,239,rgbToColor16(255,79,0));
  // tft.fillRect(140,239-readings[9]/max*239,20,239,rgbToColor16(223,0,0));
  // tft.fillRect(200,239-readings[10]/10000.0*239,20,239,rgbToColor16(255,255,255));//clear
  // tft.fillRect(180,239-readings[11]/10000.0*239,20,239,rgbToColor16(107,0,0));//NIR

  // //rgb from as7341 datasheet sensor array diagram
  // tft.fillRect(0,239-readings[0]/max*239,20,239,rgbToColor16(139,61,197));
  // tft.fillRect(20,239-readings[1]/max*239,20,239,rgbToColor16(0,82,142));
  // tft.fillRect(40,239-readings[2]/max*239,20,239,rgbToColor16(0,176,240));
  // tft.fillRect(60,239-readings[3]/max*239,20,239,rgbToColor16(0,255,204));
  // tft.fillRect(80,239-readings[6]/max*239,20,239,rgbToColor16(0,176,80));
  // tft.fillRect(100,239-readings[7]/max*239,20,239,rgbToColor16(255,255,0));
  // tft.fillRect(120,239-readings[8]/max*239,20,239,rgbToColor16(255,192,0));
  // tft.fillRect(140,239-readings[9]/max*239,20,239,rgbToColor16(192,0,0));
  // tft.fillRect(200,239-readings[10]/10000.0*239,20,239,rgbToColor16(255,255,255));//clear
  // tft.fillRect(180,239-readings[11]/10000.0*239,20,239,rgbToColor16(107,0,0));//NIR
}

int rgbToColor16(int r, int g, int b) {
  int r16 = (r + 1) / 8 - 1;
  int g16 = (g + 1) / 4 - 1;
  int b16 = (b + 1) / 8 - 1;
  if (r16 < 0) r16 = 0;
  if (g16 < 0) g16 = 0;
  if (b16 < 0) b16 = 0;

  // Serial.print(r);
  // Serial.print("\t");
  // Serial.print(g);
  // Serial.print("\t");
  // Serial.print(b);
  // Serial.print("\t");
  // Serial.print(r16);
  // Serial.print("\t");
  // Serial.print(g16);
  // Serial.print("\t");
  // Serial.print(b16);
  // Serial.println();

  return r16 * 32 * 64 + g16 * 32 + b16;
}