void setup(){
 
  Serial.begin(115200);
}

unsigned long t,t1,diff,sum,count;

void loop()
{  
  t1 = micros();
  diff = t1-t;
  
  int v=analogRead(32);
  sum+=v;
  count++;

  if (diff>50000)
  {
    t=t1;

    double voltage=(double)sum/count/4096*3.3;
//    double voltage=(double)sum/count/1024*5;//arduino
    
//    Serial.print("A read:");Serial.print(v);
    Serial.print(" voltage:");
    Serial.print(voltage);
    Serial.print("V");
    Serial.print(" UVindex:");
    Serial.print(voltage*10);
    Serial.println();

    sum=0;
    count=0;
  }
}
