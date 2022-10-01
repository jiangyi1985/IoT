#define TX_PIN 32 //transmit
#define S_PIN 35 //switch

void setup()
{
    Serial.begin(115200);	  // Debugging only
    pinMode(TX_PIN,OUTPUT);
    pinMode(S_PIN,INPUT);//during power up this could be high
    pinMode(LED_BUILTIN,OUTPUT);
}

int s=0;
int signalA[29]={0,0,0,0,0,1,0,1,
0,0,1,0,1,1,0,0,
1,1,1,0,1,1,1,0,
0,1,0,1,0};

int signalB[14]={0,0,0,0,0,1,0,1,//probably not needed, same as signalA
0,0,1,0,1,1};

int fosc_us=133;//microseconds time of 1/3 bit symbol

void loop()
{
    s= digitalRead(S_PIN);
    if(s==1)
    {
      digitalWrite(LED_BUILTIN,HIGH);

      for(int i=0; i<sizeof(signalA); i++)
      {
        if(signalA[i]==0)
        {
          delayMicroseconds(fosc_us*2);
          digitalWrite(TX_PIN,HIGH);
          delayMicroseconds(fosc_us);
          digitalWrite(TX_PIN,LOW);
        }
        else
        {
          delayMicroseconds(fosc_us);
          digitalWrite(TX_PIN,HIGH);
          delayMicroseconds(fosc_us*2);
          digitalWrite(TX_PIN,LOW);
        }
      }
      
      delayMicroseconds(fosc_us*3*7);//gap between 2 parts of data
      
      for(int i=0; i<sizeof(signalB); i++)
      {
        if(signalB[i]==0)
        {
          delayMicroseconds(fosc_us*2);
          digitalWrite(TX_PIN,HIGH);
          delayMicroseconds(fosc_us);
          digitalWrite(TX_PIN,LOW);
        }
        else
        {
          delayMicroseconds(fosc_us);
          digitalWrite(TX_PIN,HIGH);
          delayMicroseconds(fosc_us*2);
          digitalWrite(TX_PIN,LOW);
        }
      }

      delay(1000);
    }
    else{
      digitalWrite(LED_BUILTIN,LOW);
    }
}
