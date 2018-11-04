#define Start_Pin A5
#define Green A1
#define Yellow A2
#define Red1 A3
#define Red2 A4
#define Down_Contact    12
#define Up_Contact 11

#define trigger 4
#define echo 5

#define Lift_PWM 6
#define Liftp 8
#define Liftn 7

#define Broom_Pin 9
#define Bucket_Pin 10

#define Sweep_in_Angle 50
#define Sweep_out_Angle 120

#define Dump_Angle  150
#define Collect_Angle 50

#include<Servo.h>
#include<SoftwareSerial.h>

 Servo Broom;
 Servo Bucket;
 
 SoftwareSerial Node_MCU(3, 2); // RX, TX

 int LIFT_UP_SPEED = 150;
 int LIFT_DOWN_SPEED = 60;

 int data=0;
 int duration=0,space=0;
 int bin_height = 33;
 int percent = 0;
 
 unsigned long Last_Timer =0;
 
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Node_MCU.begin(9600);
  
  pinMode(Liftp,OUTPUT);
  pinMode(Liftn,OUTPUT);
  pinMode(Lift_PWM,OUTPUT);

  pinMode(trigger,OUTPUT);
  pinMode(echo,INPUT);
  
  Broom.attach(Broom_Pin);
  Bucket.attach(Bucket_Pin);

  Broom.write(Sweep_out_Angle);
  delay(500);
  Bucket.write(Collect_Angle);
  delay(1000);
  
}

void loop() {
//while(!flag)
//{
// flag = digitalRead(start_pin);
// Serial.print("Idle");
//}

if(digitalRead(Start_Pin))
{
  Sequence();
}
 scan();

  if(space > bin_height) { space = bin_height;}
  
  percent = (float(bin_height-space)/bin_height)*100;
  if(percent >100){ percent = 100;}
  
  if( percent >70)
  {
    digitalWrite(Red2,HIGH);
    delay(200);
    digitalWrite(Red2,LOW);
    delay(200);
    digitalWrite(Red1,HIGH);
    digitalWrite(Yellow,HIGH);
    digitalWrite(Green,HIGH);
    Serial.print("  >70   ");
  }
  else if( percent >= 40 && percent < 70 )
  {
    Serial.print("  >40   ");
    digitalWrite(Red2,LOW);
    digitalWrite(Red1,HIGH);
    digitalWrite(Yellow,HIGH);
    digitalWrite(Green,HIGH);
    }
  else if( percent >= 20 && percent < 40 )
  {
    Serial.print("  >20   ");
    digitalWrite(Red2,LOW);
    digitalWrite(Red1,LOW);
    digitalWrite(Yellow,HIGH);
    digitalWrite(Green,HIGH);
    }

  else{
    Serial.print("  <20   ");
    digitalWrite(Red2,LOW);
    digitalWrite(Red1,LOW);
    digitalWrite(Yellow,LOW);
    digitalWrite(Green,HIGH);
  }

Send_data();

if(Node_MCU.available())
{
 //Serial.print("Received from Node MCU :   ");  Serial.println(Node_MCU.read());
}
Serial.print(digitalRead(Start_Pin)); Serial.print("    ");  Serial.print(digitalRead(Down_Contact)); Serial.print("    ");  Serial.print(digitalRead(Up_Contact)); Serial.print("    ");
Serial.print("Space = : "); Serial.print(space); Serial.print("    ");Serial.print("Percent = : "); Serial.print(percent);
//Serial.print("      ");
//Serial.print(digitalRead(Up_Contact));
//Serial.print("      ");
//Serial.print(digitalRead(Down_Contact));
//Serial.print("      ");
//Serial.print(data);

data=0;
Serial.println("  ");

delay(1000);
}

int scan()
{
 digitalWrite(trigger,LOW);
 delayMicroseconds(50);
 digitalWrite(trigger,HIGH);
 delayMicroseconds(100);
 digitalWrite(trigger,LOW);
 
 duration=pulseIn(echo,HIGH);
 space=duration/29/2;
 //Serial.println(space);
 return space;
}

void Send_data()
{
  Node_MCU.write(percent);
  Serial.print("Written to Node MCU:    "); Serial.print(percent); Serial.print("    ");
}
void brake()
{
  Serial.print("Brake");
    digitalWrite(Liftp,HIGH);
    digitalWrite(Liftn,HIGH);
}
void stops()
{
   Serial.print("Stopped");
    digitalWrite(Liftp,LOW);
    digitalWrite(Liftn,LOW);
}

int Lift_up(int i)
{

  Last_Timer = millis();
  while(!digitalRead(Up_Contact))
  {
    Serial.println("Lift Up");
    digitalWrite(Liftp,HIGH);
    digitalWrite(Liftn,LOW);
    analogWrite(Lift_PWM,i);
  unsigned long Timer =millis();
  if(Timer - Last_Timer > 2000)
  Lift_up(200);
  }
  delay(100);
  
  
}

int Lift_down(int i)
{
   while(!digitalRead(Down_Contact))
   {
  Serial.println("Lifting Down");
    digitalWrite(Liftp,LOW);
    digitalWrite(Liftn,HIGH);
    analogWrite(Lift_PWM,i);
   }
  stops();
  brake();
  delay(25);
    digitalWrite(Liftp,HIGH);
    digitalWrite(Liftn,LOW);
    analogWrite(Lift_PWM,60);
    delay(50);
  
  
}

 void Sweep_in()
 {
  
  for(int i=Sweep_out_Angle; i>=Sweep_in_Angle; i--)
  {
    Broom.write(i);
    Serial.println("sweeping in");
    delay(25);
  }
 }

  void Sweep_out()
 {
  for(int i=Sweep_in_Angle; i<=Sweep_out_Angle; i++)
  {
    Broom.write(i);
    Serial.println("Sweeping out");
    delay(25);
  }
 }

void Collect()
 {
  
  int i;
  for(i=Dump_Angle; i>=Collect_Angle; i--)
  {
    Bucket.write(i);
    Serial.println("Collecting");
    delay(25);
  }
 }

 void Dump()
 {
  
  for(int i=Collect_Angle; i<=Dump_Angle; i++)
  {
    Bucket.write(i);
    Serial.println("Dumping");
    delay(25);
 }
 }

void Sequence()
{
  Serial.println(" SEQUENCE INITIATED  ");
  
  Sweep_in();
  Sweep_out();
  Sweep_in();
  Sweep_out();

  Lift_up(LIFT_UP_SPEED);
  delay(100);
  
  Dump();
  Collect();
  delay(100);

  Lift_down(LIFT_DOWN_SPEED);
  
}

