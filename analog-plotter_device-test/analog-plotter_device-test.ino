
unsigned long t,t1,diff;
int maxV=0,minV=9999,sum=0,sum_diff=0,count=0,range=0;
double avg=0,avg_diff,variance,std_dev,square_sum_diff;
//unsigned long square_sum;
int c0_500,c500_1000,c1000_avg,cavg_1800,c1800_2300,c2300_2800;

void setup() {
  Serial.begin(115200);
//    Serial.print(minV);
//    Serial.print(",");//seperator
//    Serial.println(maxV);
}

void loop() {
  t1 = micros();
  diff = t1-t;
  
//  int v = analogRead(A0); //arduino
  int v = analogRead(32); //esp32
//  Serial.println(v);

  if(maxV<v){maxV=v;}
  if(v<minV){minV=v;}
  sum+=v;
//  square_sum+=v*v;

  if(avg!=0)//上次的平均数
  {
    double diff=abs(v-avg);
    sum_diff+=diff;
    square_sum_diff+=diff*diff;
    
  if(v<500) c0_500++;
  else if(v<1000) c500_1000++;
  else if(v<avg) c1000_avg++;
  else if(v<1800) cavg_1800++;
  else if(v<2300) c1800_2300++;
  else if(v<2800) c2300_2800++;
  }
  
  count++;
  
  if (diff>100000)//square_sum_diff is about 1.2E9 at 50ms sample range, usigned long overflows at 4.3E9
  {
    t=t1;
    range=maxV-minV;//极差
    avg=(double)sum/count;//平均数
    avg_diff=sum_diff/count;//平均差
    variance=square_sum_diff/count;//方差
    std_dev=sqrt(variance);//标准差
    
    Serial.print("count:");    Serial.print(count);    Serial.print(",");
    Serial.print("min:");    Serial.print(minV);
    Serial.print(",max:");    Serial.print(maxV);
    Serial.print(",avg:");    Serial.print(avg);
    Serial.print(",<500:");    Serial.print(c0_500);
    Serial.print(",<1000:");    Serial.print(c500_1000);
    Serial.print(",<avg:");    Serial.print(c1000_avg);
    Serial.print(",<1800:");    Serial.print(cavg_1800);
    Serial.print(",<2300:");    Serial.print(c1800_2300);
    Serial.print(",<2800:");    Serial.print(c2300_2800);

    Serial.print(",range:");
    Serial.print(range);
//    Serial.print(",square_sum_diff:");
//    Serial.print(square_sum_diff);
//    Serial.print(",variance:");
//    Serial.print(variance);
    Serial.print(",std_dev:");
    Serial.print(std_dev);
    Serial.println();
    
    minV=9999;
    maxV=0;
    range=0;
    sum=0;
//    square_sum=0;
    square_sum_diff=0;
    count=0;

    c0_500=0;
    c500_1000=0;
    c1000_avg=0;
    cavg_1800=0;
    c1800_2300=0;
    c2300_2800=0;
  }
}
