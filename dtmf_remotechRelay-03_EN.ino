#include <FlexiTimer2.h>

int OT1;
int OT2;
int OT3;
int OT4;

int STD = A0;
int Q1 = A1;
int Q2 = A2;
int Q3 = A3;
int Q4 = A4;

int CH1 = 4;
int CH2 = 5;
int CH3 = 6;
int CH4 = 7;

unsigned char Q_dat = 0x00;
unsigned char STDat_cnt = 0x00;
unsigned char Decod_finished = 0x00;

/*The following four pins is matching to the four buttons on the remotecontroller 315MHz(SKU:FIT0355))*/
/*以下四个管脚定义，对应遥控器上的四个按键(遥控器为推荐的无线遥控器315MHz(SKU:FIT0355))*/
int D1 = 8;    //Decode pin 1 解码芯片数字输出管脚1  
int D2 = 9;    //Decode pin 2 解码芯片数字输出管脚2 
int D3 = 10;   //Decode pin 3 解码芯片数字输出管脚3  
int D4 = 11;   //Decode pin 4 解码芯片数字输出管脚4  
int ledPin = 13;   //接收指示灯

volatile int state = LOW; 
//Check once every 5 ms

int delay_time = 800;//500 ms

void dtmf_decode(void)
{
   
   int STD_State = 0;
   int IO_State = 0;
   
    STD_State = digitalRead(STD);
    
    //STD  Rising edge
    if(STD_State) {if(STDat_cnt < 100) STDat_cnt ++;}  else STDat_cnt = 0;

    if(STDat_cnt==3)//Pulse width greater than 15MS
   {
    Q_dat = 0x00;
    
    IO_State = digitalRead(Q1);
    if(IO_State) Q_dat = 0x01;
    
    IO_State = digitalRead(Q2);
    if(IO_State) Q_dat |= 0x02;
    
    IO_State = digitalRead(Q3);
    if(IO_State) Q_dat |= 0x04;
    
    IO_State = digitalRead(Q4);
    if(IO_State) Q_dat |= 0x08;
    
    Decod_finished = 0xff;//Complete the decoding
   
   }

}

void setup()
{
   // initialize the DTMF Decoding module as an output.
    pinMode(STD, INPUT); 
    pinMode(Q1, INPUT);
    pinMode(Q2, INPUT);
    pinMode(Q3, INPUT);
    pinMode(Q4, INPUT);
    
    pinMode(CH1, OUTPUT);
    pinMode(CH2, OUTPUT);
    pinMode(CH3, OUTPUT);
    pinMode(CH4, OUTPUT);
    
    digitalWrite(CH1, LOW);
    digitalWrite(CH2, LOW);
    digitalWrite(CH3, LOW);
    digitalWrite(CH4, LOW);
    
    Serial.begin(9600);
    
      pinMode(D4, INPUT);  //Initialize input pin, for decoder output 分别初始化为输入端口，读取解码芯片输出管脚的电平
      pinMode(D2, INPUT);  
      pinMode(D1, INPUT);   
      pinMode(D3, INPUT);  
      pinMode(ledPin, OUTPUT);  
      attachInterrupt(1,blink,RISING); //Brake out 数字口3，中断1，对应解码芯片的接收中断管脚  
      digitalWrite(ledPin, LOW);

    
    
   //Enable Timer2
   FlexiTimer2::set(5, 1.0/1000, dtmf_decode); // call every 5 1ms "ticks"
   FlexiTimer2::start();
}

void loop()
{

if(state!=LOW)    //If receive command from remote controller 如果接收到遥控器的命令，则进入该语句
     {
       state=LOW;
       delay(1);   //Wait tail the level is stable 适当延时，等待管脚电平稳定
       digitalWrite(ledPin, HIGH);

       
       Serial.print(digitalRead(D4));  //Read the decoded level and print them out 分别读取解码芯片输出管脚的电平，并打印出来    
       Serial.print(digitalRead(D2));
       Serial.print(digitalRead(D1));
       Serial.println(digitalRead(D3) );
      
       OT1=digitalRead(D4);
       OT2=digitalRead(D2);
       OT3=digitalRead(D1);
       OT4=digitalRead(D3);
       
       if (OT1==1)
{
  digitalWrite(CH1, HIGH);
 }

if (OT1==0 && OT2==1 && OT3==0 && OT4==0)
{
  digitalWrite(CH2, HIGH);
 }


if (OT1==0 && OT2==0 && OT3==1 && OT4==0)
{
  digitalWrite(CH3, HIGH);
 }

       if (OT1==0 && OT2==0 && OT3==0 && OT4==1)
{
  digitalWrite(CH1, LOW);
  digitalWrite(CH2, LOW);
  digitalWrite(CH3, LOW);
  digitalWrite(CH4, LOW);
 }
  delay(300);
       digitalWrite(ledPin, LOW);
 Serial.println(digitalRead(OT1));
  Serial.println(digitalRead(OT2));
  Serial.println(digitalRead(OT3));
  Serial.println(digitalRead(OT4));
     } 
  //Complete the decoding
 if(Decod_finished)
 {
  Decod_finished = 0;
   
   switch(Q_dat){
    case 1 : Serial.print(" Activate : Channel 1 ON\n" ); digitalWrite(CH1, HIGH); OT1=1;break;
    case 2 : Serial.print(" Activate : Channel 2 ON\n" ); digitalWrite(CH2, HIGH); OT2=1;break;
    case 3 : Serial.print(" Activate : Channel 3 ON\n" ); digitalWrite(CH3, HIGH); OT3=1;break;
    case 4 : Serial.print(" Activate : Channel 4 ON\n" ); digitalWrite(CH4, HIGH); OT4=1;break;
    case 5 : Serial.print(" Activate : Channel 1 OFF\n" ); digitalWrite(CH1, LOW); OT1=0; break;
    case 6 : Serial.print(" Activate : Channel 2 OFF\n" ); digitalWrite(CH2, LOW); OT2=0; break;
    case 7 : Serial.print(" Activate : Channel 3 OFF\n" ); digitalWrite(CH3, LOW); OT3=0; break;
    case 8 : Serial.print(" Activate : Channel 4 OFF\n" ); digitalWrite(CH4, LOW); OT4=0; break;
    default : Serial.print("Invalid channel \n" );  break;
  } 


 }
  
}

void blink()
{
 state =! state;
}
