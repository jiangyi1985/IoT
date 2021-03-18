//sensor.AO --> nodeMCU32s.ADC6(GPIO34)
//sensor.VCC --> 3.3v
//AO output 0~4095
//
//3.3v office light 500 shadow 1600
//5v office light 700 shadow 2500

const int pin=34;//ADC6

int potValue=0;//0~4095

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);   //设置串口波特率
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  potValue=analogRead(pin);
  Serial.println(potValue);
  delay(100);
}
