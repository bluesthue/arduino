#include <AFMotor.h>
#include <NewPing.h>
#include <NewTone.h>

#include "pitches.h"
#include "jokes.h"

#define SENSORTRIG  10
#define SENSORECHO  9
#define SENSORMAX   200

int OGBot_mode=0;//0=debug;1=patrol;2=songmode;3=follow;4=theramin;5=alarm;
String ogbot_name="GBot";

String serialin;
String serialout;

AF_DCMotor motor1(3);
AF_DCMotor motor2(4);
int motor1dir=FORWARD;
int motor1spd=0;
int motor2dir=FORWARD;
int motor2spd=0;
bool motoron=false;

unsigned int sensorval=200;
int sensormaxval=40;
NewPing sonar(SENSORTRIG, SENSORECHO, SENSORMAX);

int buzzerpin=A0;
int musicspd=2000;
//int melody[] = {
  //NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
//};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
//int noteDurations[] = {
  //4, 8, 8, 4, 4, 4, 4, 4
//};
int melody[] = {
  NOTE_D4, 0, NOTE_F4, NOTE_D4, 0, NOTE_D4, NOTE_G4, NOTE_D4, NOTE_C4,
  NOTE_D4, 0, NOTE_A4, NOTE_D4, 0, NOTE_D4, NOTE_AS4, NOTE_A4, NOTE_F4,
  NOTE_D4, NOTE_A4, NOTE_D5, NOTE_D4, NOTE_C4, 0, NOTE_C4, NOTE_A3, NOTE_E4, NOTE_D4,
  0};

int noteDurations[] = {
  8, 8, 6, 16, 16, 16, 8, 8, 8, 
  8, 8, 6, 16, 16, 16, 8, 8, 8,
  8, 8, 8, 16, 16, 16, 16, 8, 8, 2,
  2};


int theraminpitch=0;

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  //ogc_serialconnect();
  ogc_msg("Ready");
  switch (OGBot_mode){//0=debug;1=patrol;2=songmode;3=follow;4=theramin;5=alarm;
    case 0:
      ogc_msg("Drive Mode");
      
    break; 
    case 1:
      ogc_msg("Patrol");
      //[TODO: move to motorinit()]
      motor1.setSpeed(motor1spd);
      motor1.run(RELEASE);
      motor2.setSpeed(motor2spd);
      motor2.run(RELEASE);
    break;
    case 2:
      ogc_msg("Songmode");
    break;
    case 3:
      ogc_msg("follow");
    break;
    case 4:
      ogc_msg("theremin");
    break;
    case 5:
      ogc_msg("alarm");
      motor1spd=0;
      motor2spd=0;
      
    break;
  }
}

void loop() {
  ogc_serialcheck();
  switch (OGBot_mode) {
    case 0://DEBUG  
       motorstop();
    break;
 
    case 1://PATROL
      sensormaxval=25;
      sensorloop();
      if((sensorval>sensormaxval)||(sensorval==0)){
       ogbotforward(); 
      }
      else{
       ogbotdetectobj();
      }
    break;
 
    case 2://SONGMODE
      motorstop();
      buzzersong();
    break;
    
    case 3://FOLLOW
     sensormaxval=35;
     sensorloop();
     if((sensorval<=sensormaxval)&&(sensorval>=5)){
       ogbotforward(); 
      }
      else{
       motorstop();
      }
    break;
 
    case 4://THERAMIN
      motorstop();
      sensorloop();
      theraminpitch = map(sensorval, 1, 100, 31, 65535);
      if(sensorval!=0){
        NewTone(buzzerpin, theraminpitch, 200);//noteDuration=10;
      }
    break;
    case 5:
     //ALARM  
     motorstop();
     sensormaxval=25; 
      sensorloop();
      if((sensorval<sensormaxval)&&(sensorval!=0)){
       ogbotalarm(); 
      }
    break;
  }
  if(motoron){
    
  }
}
void ogbotdetectobj(){
  motorstop();
  buzzersong();
  ogbotbackward();
  delay(750);
  ogbotright();
  delay(500);
}
void ogbotforward(){
   motor1dir=FORWARD;
   motor2dir=FORWARD;
   motor1spd=240;
   motor2spd=240;
   motorloop(); 
}
void ogbotbackward(){
   motor1dir=BACKWARD;
   motor2dir=BACKWARD;
   motor1spd=255;
   motor2spd=255;
   motorloop();
}
void ogbotright(){
   motor1dir=FORWARD;
   motor2dir=BACKWARD;
   motor1spd=255;
   motor2spd=255;
   motorloop();
}
void ogbotleft(){
   motor1dir=BACKWARD;
   motor2dir=FORWARD;
   motor1spd=127;
   motor2spd=127;
   motorloop();
}
void ogbotalarm(){
   ogc_msg("ALARM!!!");
   buzzersong(); 
}

void ogc_serialcheck(){
    if (Serial.available()) {
      serialin="";
      delay(100);
      while (Serial.available() > 0) {
        serialin+=(char)Serial.read();
      }
      if(serialin!=""){
        ogc_serialprocess();
      }
    }
}
void ogc_serialprocess(){
  //Serial.println(serialin);
   if(serialin== "JOKE"){
     chatjoke();
   } 
   if(serialin=="HELLO"){
    chathello(); 
   }
   if(serialin=="HELP"){
    chathelp(); 
   }
   
   if(serialin== "CM0"){
     ogc_msg("Drive me crazy!");
     ogc_changemode(0);
   } 
   if(serialin== "CM1"){
     ogc_msg("Patrolling!");
     ogc_changemode(1);
   }
   if(serialin== "CM2"){
     ogc_msg("La La La La La!");
     ogc_changemode(2);
   }  
   if(serialin=="CM3"){
    ogc_msg("I'll follow you.");
    ogc_changemode(3); 
   }
   if(serialin== "CM4"){
     ogc_msg("Strange sounds it is!");
     ogc_changemode(4);
   }  
   if(serialin=="CM5"){
    ogc_msg("On ALERT!");    
    ogc_changemode(5); 
   }
}
void ogc_changemode(int m){
  OGBot_mode=m;
}
void ogc_msg(String msg){
  Serial.print(ogbot_name);
  Serial.print(":");
  Serial.println(msg);
}

void motorloop(){
  motor1.run(motor1dir);
  motor2.run(motor2dir);
  motor1.setSpeed(motor1spd);
  motor2.setSpeed(motor1spd); 
}
void motortest(){
   uint8_t i;
   buzzersong();
   motor1spd=255;
   motor2spd=255;
   i=0;
   while(i<200){ 
     motorloop();
     i++;
     Serial.println(i);
     delay(10);
   }
   motor1spd=0;
   motor2spd=0;
   motorloop();
}
void motorrelease(){
 motor1.run(RELEASE);
 motor2.run(RELEASE);
} 
 void motorstop(){
  motor1spd=0;
  motor2spd=0;
  motorloop();
}
void sensorloop(){
  delay(50);
 unsigned int uS=sonar.ping();
 sensorval=uS/US_ROUNDTRIP_CM;
}
void buzzersong(){
   ogc_msg("Playing song.");
   for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = musicspd / noteDurations[thisNote];
    NewTone(buzzerpin, melody[thisNote], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noNewTone(buzzerpin);
  }
}

void chatjoke(){
// Serial.write("ogbot:What did the muffin say to the other muffin in the oven?\nOH MY GOD A TALKING MUFFIN!"); 
randomSeed(analogRead(0));
int rand=random(0,INTJOKES);
char* joke=arrjokes[rand];
ogc_msg(joke);
}
void chathello(){
 ogc_msg("Hello!  My name is"+ogbot_name);
 ogc_msg("type HELP for list of commands."); 
}
void chathelp(){
 ogc_msg("What can I help you do?");
 Serial.println("CM1-5 - Change Mode");
 Serial.println("        CM0=debug;CM1=patrol;CM2=songmode;CM3=follow;CM4=theramin;CM5=alarm;)");
 Serial.println("JOKE - Tell you a joke");
 Serial.println("HELP - List Commands"); 
}
