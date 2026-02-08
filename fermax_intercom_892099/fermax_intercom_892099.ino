/*
 * JQC-3FF-S-Z    NodeMcu-32S
 * IN             GPIO22
 * DC+            5V
 * DC-            GND
 */

const int RELAY_OUT = 22;  //relay signal
const int CALL_IN = 15;    //call-in signal, originally 5~7.5v, divided to 1/3
const int TONE = 25;       //mic out tone, Use GPIO 25 (DAC Channel 1)

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(RELAY_OUT, OUTPUT);
  digitalWrite(RELAY_OUT, LOW);

  pinMode(CALL_IN, INPUT);
}

void loop() {
  // playTone(350, 300);
  // playTone(400, 300);
  // delay(3000);
  // return;

  // sleep(1);
  delay(20);

  // Serial.println(digitalRead(CALL_IN));
  // Serial.println(analogRead(CALL_IN));
  // Serial.println();

  int val = analogRead(CALL_IN);  //1st read
  Serial.println(val);

  if (val > 1500) {  //call-in status ON

    if (val > 3500) {  //call-in btn being pressed
      Serial.println("btn press detected. waiting a bit...");
      sleep(4);
      Serial.println("closing relay...");

      // digitalWrite(RELAY_OUT, HIGH);
      // sleep(1);
      // digitalWrite(RELAY_OUT, LOW);
      // sleep(1);

      // tone(TONE,200,50);
      // delay(100);
      // tone(TONE,250,50);
      // delay(100);

      digitalWrite(RELAY_OUT, HIGH);
      // delay(1000);
      delay(400);
      //3 rising tones
      // playTone(800, 100);
      // playTone(960, 100);
      // playTone(1152, 100);
      //2 repeat tones
      playTone(800, 200);
      delay(150);
      playTone(800, 200);

      digitalWrite(RELAY_OUT, LOW);
      delay(500);
    } else {  //btn not pressed but status ON
    }

    // // sleep(1);
    // val = analogRead(CALL_IN);  //2nd read
    // Serial.println(val);
    // if (val > 1500) {
    //   sleep(1);
    //   val = analogRead(CALL_IN);  //3rd read
    //   Serial.println(val);
    //   if (val > 1500) {
    //     // sleep(1);

    //   }
    // }
  }
}

void playTone(int f, int periodMs) {
  unsigned long half_t_us = 1000000 / (2 * f);
  int cycles = periodMs * 1000 / half_t_us / 2;
  for (int i = 0; i < cycles; i++) {
    // Generate a square wave with ~50% duty cycle
    dacWrite(TONE, 50);            //0~255 as volume            // High voltage (around 3.3V)
    delayMicroseconds(half_t_us);  // Half period delay

    dacWrite(TONE, 0);             // Low voltage (around 0V)
    delayMicroseconds(half_t_us);  // Other half period delay
  }
}