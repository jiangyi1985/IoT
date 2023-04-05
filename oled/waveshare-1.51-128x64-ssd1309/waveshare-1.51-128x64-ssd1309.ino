#include "U8g2lib.h"//https://github.com/olikraus/u8g2

//The name of the Arduino C++ constructor has the following parts. These parts are connect with _.
//No   Description   Example
//1   Prefix  U8G2
//2   Display Controller  UC1701
//3   Display Name  DOGS102
//4   Buffer Size   1, 2 or F (full frame buffer)
//5   Communication   4W_SW_SPI

//Buffer Size
//1   Only one page of the display memory is stored in the microcontroller RAM. Use a firstPage()/nextPage() loop for drawing on the display.
//2   Same as 1, but maintains two pages in the microcontroller RAM. This will be up to two times faster than 1.
//F   Keep a copy of the full display frame buffer in the microcontroller RAM. Use clearBuffer() to clear the RAM and sendBuffer() to transfer the microcontroller RAM to the display.
//The full buffer F option can be used only, if there is sufficient RAM available in the microcontroller. Use option 1 or 2 on a microcontroller with a small amount of RAM. The u8x8 API can be used if there is not even RAM for one page.

//Communication
//The communication protocol is part of the constructor name (see above). The following communication protocols are supported:
//4W_SW_SPI   4-wire (clock, data, cs and dc) software emulated SPI
//4W_HW_SPI   4-wire (clock, data, cs and dc) hardware SPI (based on Arduino SPI library)
//2ND_4W_HW_SPI   If supported, second 4-wire hardware SPI (based on Arduino SPI library)
//3W_SW_SPI   3-wire (clock, data and cs) software emulated SPI
//SW_I2C  Software emulated I2C/TWI
//HW_I2C  Hardware I2C based on the Arduino Wire library
//2ND_HW_I2C  If supported, use second hardware I2C (Arduino Wire lib)
//6800  8-bit parallel interface, 6800 protocol
//8080  8-bit parallel interface, 8080 protocol//
//The hardware I2C allows pin remapping for some controller types. The optional pin numbers are listed after the reset pin: ..._HW_I2C([reset [, clock, data]]). Use U8X8_PIN_NONE if the reset input of the display is not connected.
//This might be confusing, because the software emulated I2C constructor lists the required clock and data line before the reset pin: ..._SW_I2C(clock, data [, reset])

//Rotation
//The software emulated display rotation is the first argument of the constructor.
//U8G2_R0   No rotation, landscape
//U8G2_R1   90 degree clockwise rotation
//U8G2_R2   180 degree clockwise rotation
//U8G2_R3   270 degree clockwise rotation
//U8G2_MIRROR   No rotation, landscape, display content is mirrored (v2.6.x)
//If supported U8G2_R2 is identical to u8g2::setFlipMode(1).

//U8g2 full buffer, page buffer and u8x8 mode
//U8g2 supports three different drawing modes:
//    Full screen buffer mode
//    Page mode (This is the U8glib picture loop)
//    U8x8, character only mode

//----------------------------------NodeMCU SPI PINs-----------------------------
// SPI	MOSI	  MISO	  SCLK	  CS
// VSPI	GPIO 23	GPIO 19	GPIO 18	GPIO 5        esp32 default to VSPI (usually, when not specified, the board will use the VSPI pins when initializing an SPI communication with the default settings)
// HSPI	GPIO 13	GPIO 12	GPIO 14	GPIO 15
//U8G2_SSD1309_128X64_NONAME0_1_4W_HW_SPI(const u8g2_cb_t *rotation, uint8_t cs, uint8_t dc, uint8_t reset = U8X8_PIN_NONE)

//U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);//full buffer mode
U8G2_SSD1309_128X64_NONAME0_1_4W_HW_SPI u8g2(U8G2_R0,5,17,16);//page buffer mode  //U8G2_SSD1306_128X64_NONAME_1_HW_I2C(rotation, [reset [, clock, data]]) [page buffer, size = 128 bytes]
//U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE,SCL,SDA); //u8x8 mode  //U8X8_SSD1306_128X64_NONAME_HW_I2C([reset [, clock, data]])

void setup() {
  Serial.begin(115200);

  u8g2.begin();
//  u8x8.begin();
}

int i=0;

void loop() {
//  //full buffer mode
//  u8g2.clearBuffer();
//  u8g2.setFont(u8g2_font_ncenB14_tr);
//  u8g2.setCursor(0, 15);
//  u8g2.print("Hello World!");
//  u8g2.setCursor(0, 30);
//  u8g2.print(i);  
//  u8g2.sendBuffer();

  //page buffer mode
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_unifont_t_chinese1); //完整中文字库参见https://github.com/larryli/u8g2_wqy  //由于太大，需要自行生成字库数据 https://github.com/larryli/u8g2_wqy/wiki/CustomFont

    u8g2.setDrawColor(1);//is_transparent: Enable (1) or disable (0) transparent mode. Default mode is 0 (background color of the characters is overwritten)
//    u8g2.setDrawColor(2);//0 (clear pixel value in the display RAM), 1 (set pixel value) or 2 (XOR mode)


    u8g2.drawUTF8(0,20,"你好啊,u8g2!");
    
    String str = String(i);
    char buffer[20];
    str.toCharArray(buffer,20);
    u8g2.drawStr(0,40,buffer);
    
    u8g2.drawLine(127, 0, 0, 63);
    u8g2.drawCircle(108, 20, 15, U8G2_DRAW_ALL);
    u8g2.drawBox(64,32,64,32);

    u8g2.drawStr(30,64,"abc");
    u8g2.setDrawColor(0);
    u8g2.drawStr(65,64,"abc");

    //mark screen corners
    // Serial.println(millis()/1000%2);
    if(millis()/1000%2==0)
      u8g2.setDrawColor(1);
    else
      u8g2.setDrawColor(0);
    u8g2.drawPixel(0, 0);
    u8g2.drawPixel(127, 0);
    u8g2.drawPixel(0, 63);
    u8g2.drawPixel(127, 63);
    
  } while ( u8g2.nextPage() );

//  //u8x8 mode
//  //line1
//  u8x8.setFont(u8x8_font_chroma48medium8_r);  
//  u8x8.drawString(0,0,"drawStr123ABC");
//  //line2
//  String str = String(i);
//  char buffer[20];
//  str.toCharArray(buffer,20);
//  u8x8.drawString(0,1,buffer);
//  //line3
//  u8x8.setFont(u8x8_font_courB18_2x3_r);
//  u8x8.drawString(0,2,"ab12XY");
//  //line4
//  u8x8.setFont(u8x8_font_lucasarts_scumm_subtitle_o_2x2_r);
//  u8x8.drawString(0,5,"Seq:");
//  u8x8.drawString(8,5,buffer);
//  //line5
//  u8x8.setFont(u8x8_font_amstrad_cpc_extended_f);
//  u8x8.drawUTF8(0, 7, "Umlaut äöü");   
//  delay(1000);
  
  i++;
}
