//hc-sr501 sensor:
//detection lockout/cooldown after turning low: about 4 seconds (customizable during manufacture)
//   delay  distance/sensitivity
//      (+) (+)
// -----------------
// |               * L (single trigger)
// |               *
// |               * H (repeatable trigger)
// |    + out -    |
// -----------------
//  <== more sensible for motions along this axis ==>

//hc-sr501  nodemcu
//+         5V
//out
//-         GND

const int pin=34;//ADC6

int potValue=0;//0~4095

void setup() {
  // put your setup code here, to run once:

  pinMode(LED_BUILTIN,OUTPUT);
  
  Serial.begin(115200);   //设置串口波特率
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  potValue=analogRead(pin);
  Serial.println(potValue);

  if(potValue>2000)
    digitalWrite(LED_BUILTIN, HIGH);
  else
    digitalWrite(LED_BUILTIN, LOW);
  
  delay(100);
}
