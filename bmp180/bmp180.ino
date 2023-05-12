// bmp180(GY-68)  NodeMcu-32S   NodeMcu-8266
// SDA            GPIO21        GPIO04 //ai-thinker nodemcu8266 onboard rgb-R led will be lit
// SCL            GPIO22        GPIO05 //ai-thinker nodemcu8266 onboard rgb-B led will be lit
// vcc            5V            5v
// 3.3v           3.3

#include <Adafruit_BMP085.h> //https://github.com/adafruit/Adafruit-BMP085-Library

/*************************************************** 
  This is an example for the BMP085 Barometric Pressure & Temp Sensor
  Designed specifically to work with the Adafruit BMP085 Breakout 
  ----> https://www.adafruit.com/products/391
  These pressure and temperature sensors use I2C to communicate, 2 pins
  are required to interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// Connect VCC of the BMP085 sensor to 3.3V (NOT 5.0V!)
// Connect GND to Ground
// Connect SCL to i2c clock - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 5
// Connect SDA to i2c data - on '168/'328 Arduino Uno/Duemilanove/etc thats Analog 4
// EOC is not used, it signifies an end of conversion
// XCLR is a reset pin, also not used here

Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(115200);
  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
}
  
void loop() {
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.print(" *C");
    Serial.print("  ");
    
    Serial.print("Pressure = ");
    Serial.print(bmp.readPressure());
    Serial.print(" Pa");
    Serial.print("  ");
    
    //---- calculate altitude from default sealevel pressure
    // // Calculate altitude assuming 'standard' barometric
    // // pressure of 1013.25 millibar = 101325 Pascal
    // Serial.print("Altitude = ");
    // Serial.print(bmp.readAltitude());
    // Serial.print(" meters");
    // Serial.print("  ");

    //---- calculate altitude from known sealevel pressure
    // you can get a more precise measurement of altitude
    // if you know the current sea level pressure which will
    // vary with weather and such. If it is 1015 millibars
    // that is equal to 101500 Pascals.
    Serial.print("Real altitude = ");
    Serial.print(bmp.readAltitude(101600));
    Serial.print(" meters");

    //---- calculate sealevel pressure from known altitude
    // Serial.print("Pressure at sealevel (calculated) = ");
    // Serial.print(bmp.readSealevelPressure(86));
    // Serial.print(" Pa");
    // Serial.print("  ");
    
    Serial.println();
    delay(1000);
}
