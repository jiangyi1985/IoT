
unsigned long t,t1,diff;
int max=0,min=9999;

void setup() {
  Serial.begin(115200);
    Serial.print(min);
    Serial.print(",");//seperator
    Serial.println(max);
}

void loop() {
  t1 = micros();
  diff = t1-t;

  int v = analogRead(A2);
  if(max<v){max=v;}
  if(v<min){min=v;}
  
  if (diff>20000)
  {
    t=t1;
    Serial.print(min);
    Serial.print(",");//seperator
    Serial.println(max);
    min=9999;
    max=0;
  }
}
