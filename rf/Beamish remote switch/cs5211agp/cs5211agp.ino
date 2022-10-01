#define TX_PIN 32

void setup()
{
  Serial.begin(115200);

  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  
  pinMode(TX_PIN,OUTPUT);  
  digitalWrite(TX_PIN,LOW);

  pinMode(33,INPUT);
  pinMode(34,INPUT);
  pinMode(35,INPUT);
  pinMode(36,INPUT);
  delay(1000);
}

//long Tf=69;//us // 1/14.49khz=0.069ms
long Tf=69;

int s33,s34,s35,s36;

void loop()
{
  s33=digitalRead(33);
  s34=digitalRead(34);
  s35=digitalRead(35)==1?0:1;
  s36=digitalRead(36);

  if(s33==1||s34==1||s35==1||s36==1)
  {
    Serial.print("1-33:");Serial.print(s33);Serial.print(" 2-34:");Serial.print(s34);Serial.print(" 3-35:");Serial.print(s35);Serial.print(" 4-36:");Serial.print(s36);
    Serial.println();
    
  //  Serial.print("Tf=");Serial.println(Tf);
  //  Serial.println("sending frame...");
  //  sendFrame();
  //  Serial.println("sent");
    if(s33==1)
    {
      digitalWrite(LED_BUILTIN,HIGH);
      sendByteBlue();sendByteBlue();sendByteBlue();sendByteBlue();
    }

    if(s34==1)
    {
      digitalWrite(LED_BUILTIN,HIGH);
      sendByteWhite();sendByteWhite();sendByteWhite();sendByteWhite();
    }

    if(s35==1)
    {
      digitalWrite(LED_BUILTIN,HIGH);
      sendByteYellow();sendByteYellow();sendByteYellow();sendByteYellow();
    }

    if(s36==1)
    {
      digitalWrite(LED_BUILTIN,HIGH);
      sendByteAll();sendByteAll();sendByteAll();sendByteAll();
    }
  
  //  Tf+=1;
      digitalWrite(LED_BUILTIN,LOW);
    delay(1000);
  }
}

//OSC:Tf

//bit:
//0:4T1/12T0/4T1/12T0 (1=HIGH,0=LOW)(32T)
//1:12T1/4T0/12T1/4T0
//F:4T1/12T0/12T1/4T0
//Sync:4T1/124T0(128T)

//byte:
//12bits+1Sync = 12*32T+128T = 512T

//frame:
//4 bytes (4x512T)

//byte data:
//全开：0F0F011F 0001
//黄开：0F0F011F 0010
//白开：0F0F011F 0100
//暗开：0F0F011F 1000

//void sendFrame()
//{
//  sendByte();sendByte();sendByte();sendByte();
//}
void sendByteAll()
{
  send0();sendF();send0();sendF();  send0();send1();send1();sendF();
  send0();send0();send0();send1();
  sendSync();
}
void sendByteYellow()
{
  send0();sendF();send0();sendF();  send0();send1();send1();sendF();
  send0();send0();send1();send0();
  sendSync();
}
void sendByteWhite()
{
  send0();sendF();send0();sendF();  send0();send1();send1();sendF();
  send0();send1();send0();send0();
  sendSync();
}
void sendByteBlue()
{
  send0();sendF();send0();sendF();  send0();send1();send1();sendF();
  send1();send0();send0();send0();
  sendSync();
}
void send0()
{
  digitalWrite(TX_PIN,HIGH);  delayMicroseconds(4*Tf);
  digitalWrite(TX_PIN,LOW);  delayMicroseconds(12*Tf);
  digitalWrite(TX_PIN,HIGH);  delayMicroseconds(4*Tf);
  digitalWrite(TX_PIN,LOW);  delayMicroseconds(12*Tf);
}
void send1()
{
  digitalWrite(TX_PIN,HIGH);  delayMicroseconds(12*Tf);
  digitalWrite(TX_PIN,LOW);  delayMicroseconds(4*Tf);
  digitalWrite(TX_PIN,HIGH);  delayMicroseconds(12*Tf);
  digitalWrite(TX_PIN,LOW);  delayMicroseconds(4*Tf);
}
void sendF()
{
  digitalWrite(TX_PIN,HIGH);  delayMicroseconds(4*Tf);
  digitalWrite(TX_PIN,LOW);  delayMicroseconds(12*Tf);
  digitalWrite(TX_PIN,HIGH);  delayMicroseconds(12*Tf);
  digitalWrite(TX_PIN,LOW);  delayMicroseconds(4*Tf);
}
void sendSync()
{
  digitalWrite(TX_PIN,HIGH);  delayMicroseconds(4*Tf);
  digitalWrite(TX_PIN,LOW);  delayMicroseconds(124*Tf);
}
