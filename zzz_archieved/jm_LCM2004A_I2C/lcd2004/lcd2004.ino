
#define __PROG__ "jm_LCM2004A_I2C_demo1"

////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>

////////////////////////////////////////////////////////////////////////////////

#include <jm_LCM2004A_I2C.h>

////////////////////////////////////////////////////////////////////////////////

//jm_LCM2004A_I2C lcd(0x3F);
//jm_LCM2004A_I2C lcd(0x27);
jm_LCM2004A_I2C lcd;

const PROGMEM uint8_t custom_font5x8_P[8][8] = {
   { 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00 },
   { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F }
};

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis()<3000) {} // wait for USB Serial ready
  Serial.print(F(__PROG__));
  Serial.print(F("..."));
  Serial.println();

  Wire.begin();

  lcd.begin();

  delay(500);

  lcd.print(F("qwertz"));

  lcd.write('1');
  lcd.write('2');
  lcd.write('3');
  lcd.write('4');

  lcd.cursor_display_shift();

  lcd.write("xyz");

  lcd.set_ddram_addr(20);
  lcd.write("xyz");

  lcd.set_cursor(0,0);
  lcd.write('0');
  lcd.set_cursor(1,1);
  lcd.write('1');
  lcd.set_cursor(12,2);
  lcd.write('2');
  lcd.set_cursor(13,3);
  lcd.write('3');

  Serial.println(lcd.rd_instreg(), HEX);

  delay(10000);

  lcd.display_control(false, false, false);

  lcd.clear_display();

  lcd.set_cursor(0,0);
  lcd.write('0');
  lcd.set_cursor(1,1);
  lcd.write('1');
  lcd.set_cursor(12,2);
  lcd.write('2');
  lcd.set_cursor(13,3);
  lcd.write('3');

  Serial.println(lcd.rd_instreg(), HEX);

  delay(1000);

  lcd.display_control(true, true, true);

  lcd.write_cgram_P(0, 8, custom_font5x8_P[0]);

  lcd.set_cursor(10,0);
  for (uint8_t c=0; c<8; c++) lcd.write(c);
}

void loop()
{
}
