

const int PIN_TONE = 16;

void setup() {

  Serial.begin(115200);
  Serial.println("starting...");
  
}

void loop() {
  Serial.println("playing tone...");
  tone(PIN_TONE, 240, 100);
  
  Serial.println("sleeping...");
  sleep(1);
}
