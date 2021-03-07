
#define __PROG__ "jm_LCM2004A_I2C_demo2"

////////////////////////////////////////////////////////////////////////////////

#ifndef LED_BUILTIN
#define LED_BUILTIN 2 // ESP32 DEVKIT V1
//#define LED_BUILTIN 25 // ESP32 TTGO, HELTEC
#endif

////////////////////////////////////////////////////////////////////////////////

#include <jm_Scheduler.h>

#include <Wire.h>

#include <jm_LCM2004A_I2C.h>

#ifndef sgn
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}
#endif

#ifndef sign
template <typename T> int sign(T val) {
    return (val >= T(0));
}
#endif

////////////////////////////////////////////////////////////////////////////////

class jm_LCM2004A_I2C_Plus : public jm_LCM2004A_I2C
{
private:
protected:

  char _buf[12]; // +0.123456789
  int _len;

  size_t _buf_rj(int nprint); // right justify

  size_t _buf_u32(uint32_t u32, int nprint=0);
  size_t _buf_s32(int32_t s32, int nprint=0);
  size_t _buf_u32f(uint32_t u32, int nfrac, int nprint=0);
  size_t _buf_s32f(int32_t value, int nfrac, int nprint=0);

public:

  jm_LCM2004A_I2C_Plus();
  jm_LCM2004A_I2C_Plus(uint8_t i2c_address);

  size_t print_space(int num=1);

  size_t print_sgn(int sgn);
  size_t print_sign(bool sign);

  size_t print_h4(uint8_t value);
  size_t print_h8(uint8_t value);
  size_t print_h16(uint16_t value);
  size_t print_h24(uint32_t value);
  size_t print_h32(uint32_t value);

  size_t print_u8(uint8_t value, int nprint=3);
  size_t print_s8(int8_t value, int nprint=3+1);

  size_t print_u16(uint16_t value, int nprint=5);
  size_t print_s16(int16_t value, int nprint=5+1);

  size_t print_u32(uint32_t value, int nprint=10);
  size_t print_s32(int32_t value, int nprint=10+1);

  size_t print_u32f(uint32_t value, int nfrac, int nprint=10+1);
  size_t print_s32f(int32_t value, int nfrac, int nprint=10+1+1);
};

// -----------------------------------------------------------------------------

size_t jm_LCM2004A_I2C_Plus::_buf_rj(int nprint)
{
  if (nprint > _len) {
    memmove(&_buf[nprint - _len], &_buf[0], _len);
    memset(&_buf[0], ' ', nprint - _len);
  }
  _len = nprint;
  return _len;
}

size_t jm_LCM2004A_I2C_Plus::_buf_u32(uint32_t u32, int nprint)
{
//  _len = snprintf(_buf, 12, "%lu", u32);
  _len = snprintf(_buf, 12, "%" PRIu32, u32);
  if (nprint) _buf_rj(nprint);
  return _len;
}

size_t jm_LCM2004A_I2C_Plus::_buf_s32(int32_t s32, int nprint)
{
  if (s32)
//    _len = snprintf(_buf, 12, "%+li", s32);
    _len = snprintf(_buf, 12, "%+" PRIi32, s32);
  else
    _len = snprintf(_buf, 12, "0");
  if (nprint) _buf_rj(nprint);
  return _len;
}

size_t jm_LCM2004A_I2C_Plus::_buf_u32f(uint32_t u32, int nfrac, int nprint)
{
  _buf_u32(u32);
  if (_len > nfrac) {
    memmove(&_buf[_len-nfrac+1], &_buf[_len-nfrac], nfrac);
    _len++;
  } else {
    memmove(&_buf[nfrac+1+1-_len], &_buf[0], _len);
    memset(&_buf[0], '0', nfrac+1+1-_len);
    _len = nfrac+1+1;
  }
  _buf[_len-nfrac-1] = '.';
  if (nprint) _buf_rj(nprint);
  return _len;
}

size_t jm_LCM2004A_I2C_Plus::_buf_s32f(int32_t value, int nfrac, int nprint)
{
  _buf_u32f((value<0?-value:+value), nfrac);
  if (value) {
    memmove(&_buf[1], &_buf[0], _len);
    _buf[0] = sign(value)?'+':'-';
    _len++;
  }
  if (nprint) _buf_rj(nprint);
  return _len;
}

// -----------------------------------------------------------------------------

jm_LCM2004A_I2C_Plus::jm_LCM2004A_I2C_Plus() :
  jm_LCM2004A_I2C::jm_LCM2004A_I2C(),
  _len(0)
{
}

jm_LCM2004A_I2C_Plus::jm_LCM2004A_I2C_Plus(uint8_t i2c_address) :
  jm_LCM2004A_I2C::jm_LCM2004A_I2C(i2c_address),
  _len(0)
{
}

// -----------------------------------------------------------------------------

size_t jm_LCM2004A_I2C_Plus::print_space(int num)
{
  for (int i=0; i<num; i++) print(' ');
  return num;
}

// -----------------------------------------------------------------------------

size_t jm_LCM2004A_I2C_Plus::print_sgn(int sgn)
{
  return print((sgn==+1) ? '+' : (sgn==-1) ? '-' : ' ');
}

size_t jm_LCM2004A_I2C_Plus::print_sign(bool sign)
{
  return print(sign ? '+' : '-');
}

// -----------------------------------------------------------------------------

size_t jm_LCM2004A_I2C_Plus::print_h4(uint8_t value)
{
  value &= 0x0F;
  return print(value>=10 ? (char)('A'+value-10) : (char)('0'+value));
}

size_t jm_LCM2004A_I2C_Plus::print_h8(uint8_t value)
{
  return print_h4(value>>4) + print_h4(value>>0);
}

size_t jm_LCM2004A_I2C_Plus::print_h16(uint16_t value)
{
  return print_h8((uint8_t)(value>>8)) + print_h8((uint8_t)(value>>0));
}

size_t jm_LCM2004A_I2C_Plus::print_h24(uint32_t value)
{
  return print_h8((uint8_t)(value>>16)) + print_h16((uint16_t)(value>>0));
}

size_t jm_LCM2004A_I2C_Plus::print_h32(uint32_t value)
{
  return print_h16((uint16_t)(value>>16)) + print_h16((uint16_t)(value>>0));
}

// -----------------------------------------------------------------------------

size_t jm_LCM2004A_I2C_Plus::print_u8(uint8_t value, int nprint)
{
  return print_u32((uint32_t)value, nprint);
}

size_t jm_LCM2004A_I2C_Plus::print_s8(int8_t value, int nprint)
{
  return print_s32((int32_t)value, nprint);
}

size_t jm_LCM2004A_I2C_Plus::print_u16(uint16_t value, int nprint)
{
  return print_u32((uint32_t)value, nprint);
}

size_t jm_LCM2004A_I2C_Plus::print_s16(int16_t value, int nprint)
{
  return print_s32((int32_t)value, nprint);
}

size_t jm_LCM2004A_I2C_Plus::print_u32(uint32_t value, int nprint)
{
  _buf_u32(value, nprint);
  return write(_buf, _len);
}

size_t jm_LCM2004A_I2C_Plus::print_s32(int32_t value, int nprint)
{
  _buf_s32(value, nprint);
  return write(_buf, _len);
}

size_t jm_LCM2004A_I2C_Plus::print_u32f(uint32_t value, int nfrac, int nprint)
{
  _buf_u32f(value, nfrac, nprint);
  return write(_buf, _len);
}

size_t jm_LCM2004A_I2C_Plus::print_s32f(int32_t value, int nfrac, int nprint)
{
  _buf_s32f(value, nfrac, nprint);
  return write(_buf, _len);
}

////////////////////////////////////////////////////////////////////////////////

//jm_LCM2004A_I2C_Plus lcd(0x3F);
//jm_LCM2004A_I2C_Plus lcd(0x27);
jm_LCM2004A_I2C_Plus lcd;

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

bool lcd_init() // return OK
{
  Wire.begin();
#ifdef ESP32
//  Wire.reset();
#endif

  Serial.print(__LINE__);
  Serial.print(' ');
  Serial.print(lcd.i2c_address(), HEX);
  Serial.println();

  if (!lcd.begin()) return false;

  Serial.print(__LINE__);
  Serial.print(' ');
  Serial.print(lcd.i2c_address(), HEX);
  Serial.println();

  if (!lcd.write_cgram_P(0, 8, custom_font5x8_P[0])) return false;

  return true;
}

////////////////////////////////////////////////////////////////////////////////

jm_Scheduler lcd_scheduler;

void lcd_coroutine()
{
  digitalWrite( LED_BUILTIN, !digitalRead(LED_BUILTIN) );

//  Wire.begin();
//#ifdef ESP32
//  Wire.reset();
//#endif

  static char c = (0x00 - 0x10);
  c += 0x10;

  static long count = (0 - 1);
  Serial.println(++count);

  int32_t rand32 = -2147483648 + random(1073741824)*4 + random(4); // -2147483648..+2147483647
  uint32_t time32 = micros();

  if (!lcd) if (!lcd_init()) return;

  lcd.set_cursor(0, 0);
  lcd.print_space();
  lcd.print_s32(rand32); // 10+1 chars

  lcd.set_cursor(0, 1);
  lcd.print_s32f(rand32, 9); // 10+1+1 chars

  lcd.set_cursor(0, 2);
  lcd.print_u32f(time32, 6); // 10+1 chars

  lcd.set_cursor(0, 3);
  lcd.print_h8(c);
  lcd.print_space();
  for (int i=0; i<0x10; i++) lcd.print((char)(c + i));
}

////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis()<3000) {} // wait for USB Serial ready
  Serial.print(F(__PROG__));
  Serial.print(F("..."));
  Serial.println();

//  Wire.begin();
//#ifdef ESP32
//  Wire.reset();
//#endif

//  lcd_init();

  pinMode(LED_BUILTIN, OUTPUT);

  lcd_scheduler.start(
    lcd_coroutine,
    jm_Scheduler_time_read() + 1*TIMESTAMP_1SEC,
    1*TIMESTAMP_1SEC
  );
}

void loop()
{
  jm_Scheduler::yield();
}
