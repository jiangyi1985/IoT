//TFT80x160 UNO NodeMCU-32S
//SCL       13  18
//SDA       11  23
//RST       9   16
//DC        8   17
//CS        10  SS(5)
//BLK

#include <Adafruit_GFX.h>    // Core graphics library //https://github.com/adafruit/Adafruit-GFX-Library
// #include "Adafruit_ST7735.h" // Hardware-specific library for ST7735 //https://github.com/adafruit/Adafruit-ST7735-Library //MODIFIED for 金逸晨 0.96-8pin13p_ips 
#include "Adafruit_ST7789.h" // Hardware-specific library for ST7789
#include <SPI.h>

#define NODEMCU32S
#if defined(NODEMCU32S)
  #define TFT_CS         SS
  #define TFT_RST        16
  #define TFT_DC         17
#elif defined(ESP8266)
  #define TFT_CS         4
  #define TFT_RST        16                                            
  #define TFT_DC         5
#else
  // For the breakout board, you can use any 2 or 3 pins.
  // These pins will also work for the 1.8" TFT shield.
  #define TFT_CS        10
  #define TFT_RST        9 // Or set to -1 and connect to Arduino RESET pin
  #define TFT_DC         8
#endif

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

// For 1.44" and 1.8" TFT with ST7735 use:
// Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
// For 1.14", 1.3", 1.54", 1.69", and 2.0" TFT with ST7789:
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);


// OPTION 2 lets you interface the display using ANY TWO or THREE PINS,
// tradeoff being that performance is not as fast as hardware SPI above.
//#define TFT_MOSI 11  // Data out
//#define TFT_SCLK 13  // Clock out

// For ST7735-based displays, we will use this call
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
// OR for the ST7789-based displays, we will use this call
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

float p = 3.1415926;

void setup(void) {
  Serial.begin(115200);
  Serial.print("TFT_CS:");Serial.println(TFT_CS);
  Serial.print("TFT_DC:");Serial.println(TFT_DC);
  Serial.print("TFT_RST:");Serial.println(TFT_RST);
  Serial.println(F("Hello! ST77xx TFT Test"));

//for GMT130-v1.0:
  tft.init(240, 320);//SPI data mode; one of SPI_MODE0, SPI_MODE1, SPI_MODE2 or SPI_MODE3 (do NOT pass the numbers 0,1,2 or 3 -- use the defines only, the values are NOT the same!)
  tft.setRotation(3);

  // OR use this initializer (uncomment) if using a 2.0" 320x240 TFT:
  //tft.init(240, 320);           // Init ST7789 320x240

  // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
  // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
  // may end up with a black screen some times, or all the time.
  //tft.setSPISpeed(40000000);
  
  Serial.print("width:");Serial.println(tft.width());
  Serial.print("height:");Serial.println(tft.height());
  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);
}

unsigned long t=0,t1,diff;
int sum=0,sum_diff=0,count=0,range=0;
double maxV=0,minV=9999,avg=0,avg_diff,variance,std_dev,square_sum_diff;

int x=0;
short lastMaxV[320];
short lastMinV[320];
short lastMax=0,lastMin=0;

void loop() {
  t1 = micros();
  diff = t1-t;

  int v = analogRead(32); //esp32

  if(maxV<v){maxV=v;}
  if(v<minV){minV=v;}
  sum+=v;

  if(avg!=0)//上次的平均数
  {
    double diff=abs(v-avg);
    sum_diff+=diff;
    square_sum_diff+=diff*diff;
    
  // if(v<500) c0_500++;
  // else if(v<1000) c500_1000++;
  // else if(v<avg) c1000_avg++;
  // else if(v<1800) cavg_1800++;
  // else if(v<2300) c1800_2300++;
  // else if(v<2800) c2300_2800++;
  }
  
  count++;

  // if (diff>100000)//square_sum_diff is about 1.2E9 at 50ms sample range, usigned long overflows at 4.3E9
  if (diff>10000)
  {
    t=t1;
    range=maxV-minV;//极差
    avg=(double)sum/count;//平均数
    avg_diff=sum_diff/count;//平均差
    variance=square_sum_diff/count;//方差
    std_dev=sqrt(variance);//标准差
    
    // Serial.print("count:");    Serial.print(count);
    // Serial.print(",min:");    Serial.print(minV);
    // Serial.print(",max:");    Serial.print(maxV);
    // Serial.print(",avg:");    Serial.print(avg);
    // Serial.print(",<500:");    Serial.print(c0_500);
    // Serial.print(",<1000:");    Serial.print(c500_1000);
    // Serial.print(",<avg:");    Serial.print(c1000_avg);
    // Serial.print(",<1800:");    Serial.print(cavg_1800);
    // Serial.print(",<2300:");    Serial.print(c1800_2300);
    // Serial.print(",<2800:");    Serial.print(c2300_2800);

    // Serial.print(",range:");    Serial.print(range);
//    Serial.print(",square_sum_diff:");
//    Serial.print(square_sum_diff);
//    Serial.print(",variance:");
//    Serial.print(variance);
    // Serial.print(",std_dev:");    Serial.print(std_dev);
    // Serial.println();


    //------------print to display
    // Serial.print(maxV/10);Serial.print(" ");Serial.print(minV/10);
    int maxY=maxV/4096*240;
    int minY=minV/4096*240;

    //different kinds of chart:

    // //-----dots-----
    // tft.drawPixel(x,lastMaxV[x],ST77XX_BLACK);
    // tft.drawPixel(x,lastMinV[x],ST77XX_BLACK);
    // tft.drawPixel(x,maxY,ST77XX_WHITE);
    // tft.drawPixel(x,minY,ST77XX_WHITE);

    // //-----areas-----
    // tft.drawLine(x,lastMinV[x],x,lastMaxV[x],ST77XX_BLACK);
    // tft.drawLine(x,minY,x,maxY,ST77XX_WHITE);

    //-----lines-----
    tft.drawLine(x,0,x,239,ST77XX_BLACK);
    tft.drawLine(x+1,0,x+1,239,ST77XX_BLACK);//better indication of where the scan line is
    if(x>0)
    {
      tft.drawLine(x-1,239-lastMax,x,239-maxY,ST77XX_WHITE);
      tft.drawLine(x-1,239-lastMin,x,239-minY,ST77XX_WHITE);
    }
    else
    {
      tft.drawPixel(x,maxY,ST77XX_WHITE);
      tft.drawPixel(x,minY,ST77XX_WHITE);
    }

    //save values here so that in next scan we can instantly reset each data (to avoid filling the whole screen)
    lastMaxV[x]=maxY;
    lastMinV[x]=minY;
    lastMax=maxY;
    lastMin=minY;
    x++;
    if(x>=320) x=0;
    
    //------------reset
    minV=9999;
    maxV=0;
    range=0;
    sum=0;
//    square_sum=0;
    square_sum_diff=0;
    count=0;
  }
}


