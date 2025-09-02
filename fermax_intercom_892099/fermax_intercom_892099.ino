/*
 * JQC-3FF-S-Z    NodeMcu-32S
 * IN             GPIO22
 * DC+            5V
 * DC-            GND
 */

const int RELAY_OUT = 22;  //relay signal
const int CALL_IN = 15;    //call-in signal, originally 5~7.5v, divided to 1/3

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(RELAY_OUT, OUTPUT);
  digitalWrite(RELAY_OUT, LOW);

  pinMode(CALL_IN, INPUT);
}

void loop() {
  // sleep(1);
  delay(20);

  // Serial.println(digitalRead(CALL_IN));
  // Serial.println(analogRead(CALL_IN));
  // Serial.println();

  int val = analogRead(CALL_IN); //1st read
  Serial.println(val);

  if (val > 1500) {//call-in status ON

    if(val>3500){//call-in btn being pressed
        Serial.println("btn press detected. waiting a bit...");
        sleep(3);
        Serial.println("closing relay...");
        digitalWrite(RELAY_OUT, HIGH);
        sleep(1);
        digitalWrite(RELAY_OUT, LOW);
        sleep(1);
    }
    else    {//btn not pressed but status ON

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
