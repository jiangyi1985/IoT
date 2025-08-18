/*
 * JQC-3FF-S-Z    NodeMcu-32S
 * IN             GPIO22
 * DC+            5V
 * DC-            GND
 */

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(22, OUTPUT);
  digitalWrite(22, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  sleep(3);
  digitalWrite(22, HIGH);
  sleep(3);
  digitalWrite(22, LOW);
}
