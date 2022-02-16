
unsigned long t,t1,diff;
int maxV=0,minV=9999;

void setup() {
  Serial.begin(115200);
    Serial.print(minV);
    Serial.print(",");//seperator
    Serial.println(maxV);
}

void loop() {
  t1 = micros();
  diff = t1-t;
  
  int v = analogRead(A0); //arduino
//  int v = analogRead(32); //esp32
//  Serial.println(v);
  if(maxV<v){maxV=v;}
  if(v<minV){minV=v;}
  
  if (diff>20000)
  {
    t=t1;
    Serial.print(minV);
    Serial.print(",");//seperator
    Serial.println(maxV);
    minV=9999;
    maxV=0;
  }
}
