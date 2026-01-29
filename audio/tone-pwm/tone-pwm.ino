

const int PIN_TONE = 16;

void setup() {

  Serial.begin(115200);
  pinMode(PIN_TONE, OUTPUT);
}

void loop() {
  Serial.println("toning 1...");
  analogWrite(PIN_TONE,1);
  delay(500);
  analogWrite(PIN_TONE,2);
  delay(500);
  analogWrite(PIN_TONE,3);
  delay(500);
  analogWrite(PIN_TONE,0);
  
  Serial.println("sleeping...");
  sleep(1);

  Serial.println("toning 2...");
  analogWrite(PIN_TONE,64);
  delay(500);
  analogWrite(PIN_TONE,127);
  delay(500);
  analogWrite(PIN_TONE,191);
  delay(500);
  analogWrite(PIN_TONE,255);
  
  Serial.println("sleeping...");
  sleep(1);
}
