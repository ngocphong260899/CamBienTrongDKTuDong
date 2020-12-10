#include <Wire.h> 
#include <TimerOne.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include<EEPROM.h>

LiquidCrystal_I2C lcd(0x27,16,2);

#define LM35_PIN A0
#define TRIAC_PIN 7
#define Kp 3
#define Kd 0.001    
#define Ki 0.01

int   minTempC   = 0 ;
int   maxTempC   = 60;
int   nhietdodat = 40;
float nhietdo;
float t;
float E,E1,E2,alpha,gamma,beta;
float Output=0;
float LastOutput=0;
float thoigian=0;
int   flag=0;
float T=1.5; //thoi gian lay mau 1.5s
float timerloop;
float tam=0;
byte degree[8] =
{
  0B01110,
  0B01010,
  0B01110,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
};

RTC_DS1307 RTC;

#define relay 6

int temp,inc,hours1,minut,add=11;
int HOUR,MINUT,SECOND;

int tang   =13;
int giam   =12;
int set_mo =11;
int set_tat=10; 
int INC    =9 ;
int next   =8 ;
unsigned long time_now = 0;

void Temperature()
{
  float read_ADC;
  read_ADC=(analogRead(LM35_PIN)*5.0/1024.0);
  nhietdo=read_ADC*100.0;  //1000/10 10mv = 1 *C
  //return nhietdo;
}
/*HAM DIEU KHIEN TRIAC*/
void TriacControl()
{
   delayMicroseconds(thoigian*1000); 
   digitalWrite(TRIAC_PIN,HIGH);
   delay(1);
   digitalWrite(TRIAC_PIN,LOW);
}
void PID()
{
  for (int i=0; i<10;i++)
  {
	  Temperature();
  tam+=nhietdo;}
  nhietdo=tam/10.0;
  tam=0;
  E=nhietdodat-nhietdo;
  alpha = 2*T*Kp + Ki*T*T + 2*Kd;
  beta = T*T*Ki - 4*Kd -2*T*Kp;
  gamma = 2*Kd;
  Output = (alpha*E + beta*E1 + gamma*E2 + 2*T*LastOutput)/(2*T);
  LastOutput=Output;
  E2=E1;
  E1=E;
  if (Output>9)
    Output=9;
    else if (Output<=1)
      Output=1;
  thoigian = 10-Output; /***************/
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(1, degree);
  Wire.begin();
  pinMode(relay,OUTPUT);
  pinMode(INC, INPUT_PULLUP);
  pinMode(next, INPUT_PULLUP);
  pinMode(set_mo, INPUT_PULLUP);
  pinMode(set_tat, INPUT_PULLUP);
  pinMode(tang, INPUT_PULLUP);
  pinMode(giam, INPUT_PULLUP);
  pinMode(TRIAC_PIN,OUTPUT);
  pinMode(LM35_PIN ,INPUT);
  digitalWrite(relay, HIGH);
  E=0;E1=0;E2=0;
  lcd.setCursor(3,0);
  lcd.print("LO AP TRUNG ");
  lcd.setCursor(5,1);
  lcd.print("TU DONG ");
  delay(2000);
   
  if (! RTC.begin()) {
    while (1);
  }
  if (! RTC.isrunning()) {
    RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {

//nut_tang
    if (digitalRead(tang)==LOW){ // Trường hợp nút nhấn được nhấn
        nhietdodat= nhietdodat+1 ;  //  tăng giá trị cài lên 1 đơn vị
      if(nhietdodat < minTempC) nhietdodat = minTempC;
    }
//nut_giam    
    if (digitalRead(giam)==LOW){
        nhietdodat= nhietdodat-1 ; //  giảm giá trị cài lên 1 đơn vị
      if(nhietdodat > maxTempC) nhietdodat = maxTempC;
    
    }
//nut_mo
   int temp=0,val=1,temp4;
   DateTime now = RTC.now();
   if(digitalRead(set_mo) == LOW)      //set Alarm time
   { 
     lcd.setCursor(0,0);
     lcd.print("  Set ON  ");
    delay(2000);
    defualt();
    time();
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Alarm time ");
    lcd.setCursor(0,1);
    lcd.print(" has been set  ");
    delay(2000);
   }    
//nut_tat
    if(digitalRead(set_tat) == LOW)      //set Alarm time
   { 
    lcd.setCursor(0,0);
    lcd.print("  Set OFF  ");
    delay(2000);
    defualt();
    time();
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("  Alarm time ");
    lcd.setCursor(0,1);
    lcd.print(" has been set  ");
    delay(2000);  
 }
  for (int i=0; i<10;i++)
  {Temperature();
  tam+=nhietdo;}
  nhietdo=tam/10.0;
  tam=0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("NDT:");
  lcd.setCursor(4,0);
  lcd.print(nhietdo);
  lcd.setCursor(0,1);
  lcd.print("SET:");
  lcd.print(nhietdodat);
  lcd.setCursor(8, 1);
  lcd.print(HOUR=now.hour(),DEC); 
  lcd.print(":");
  lcd.print(MINUT=now.minute(),DEC);
  lcd.print(":");
  lcd.print(SECOND=now.second(),DEC);
  lcd.setCursor(10, 0);
  lcd.print(now.day(),DEC);
  lcd.print("/");
  lcd.print(now.month(),DEC);
  match();
  match1();
  delay(500);
}  
void defualt()
{
  lcd.setCursor(0,1);
  lcd.print(HOUR);
  lcd.print(":");
  lcd.print(MINUT);
  lcd.print(":");
  lcd.print(SECOND);
}
void time()                             
{
  int temp=1,minuts=0,hours=0,seconds=0;
    while(temp==1)
    {
     if(digitalRead(INC)==LOW)
     {
      HOUR++;
      if(HOUR==24)
      {
       HOUR=0;
      }
      while(digitalRead(INC)==LOW);
     }
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Set Alarm Time ");
   //lcd.print(x); 
    lcd.setCursor(0,1);
    lcd.print(HOUR);
    lcd.print(":");
    lcd.print(MINUT);
    lcd.print(":");
    lcd.print(SECOND);
    delay(500);
    if(digitalRead(next)==LOW)
    {
     hours1=HOUR;
     EEPROM.write(add++,hours1);
     temp=2;
     while(digitalRead(next)==LOW);
    }
    }
    
    while(temp==2)
    {
     if(digitalRead(INC)==LOW)
     {
      MINUT++;
      if(MINUT==60)
      {MINUT=0;}
      while(digitalRead(INC)==LOW);
     }
     // lcd.clear();
    lcd.setCursor(0,1);
    lcd.print(HOUR);
    lcd.print(":");
    lcd.print(MINUT);
    lcd.print(":");
    lcd.print(SECOND);
    delay(500);
      if(digitalRead(next)==LOW)
      {
       minut=MINUT;
       EEPROM.write(add++, minut);
       temp=0;
       while(digitalRead(next)==LOW);
      }
    }
    delay(1000);
}
void match()
{
  int tem[23];
  for(int i=11;i<23;i++)
  {
    tem[i]=EEPROM.read(i);
  }
  if(HOUR == tem[11] && MINUT == tem[12]) 
  {
  digitalWrite(relay, LOW) ;
  attachInterrupt(0, TriacControl, RISING);
  Timer1.initialize(1500000); //don vi us
  Timer1.attachInterrupt(PID);
  }
}
void match1()
{
  int tem1[23];
  for(int j=11;j<23;j++)
  {
    tem1[j]=EEPROM.read(j);
  }
  if(HOUR == tem1[13] && MINUT == tem1[14]) 
  {
  digitalWrite(relay, HIGH);
  digitalWrite(TRIAC_PIN, LOW);
    lcd.clear();
    lcd.setCursor(0,5);
    lcd.print("FINISH");
    delay(1000);
  }
}
