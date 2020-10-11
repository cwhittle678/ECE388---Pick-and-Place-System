# Arduino-Proof-of-Concept-Design-Code
This was the full prototype code for our project in Arduino IDE. This served as a stepping stone and proof of concept design before heading over to Atmel Studio and basic c code.

/*
 * ---------------------------------------------------------------
 * Cameron Whittle ECE388
 * This program allows for the full directed control of the arm 
 * using the display controller as specified. This is a "proof of 
 * concept" prototype design written for arduino that was used as 
 * a starting point for the final .c Atmel Studio code 
 * ---------------------------------------------------------------
 */
#include <Servo.h>
#include "SevSeg.h"
#define trigPin 37
#define echoPin 35
Servo mA;  // servo object for Servo Motor A
Servo mB;  // servo object for Servo Motor B
Servo mC;  // servo object for Servo Motor C
Servo mD;  // servo object for Servo Motor D
Servo mE;  // servo object for Servo Motor E
SevSeg dis;
int DEF[6] = {0, 90, 80, 0, 180, 90}; //Default location of arm
int mALoc[20] = {163, 163, 147, 147, 130, 130, 115, 115, 90, 90, 75, 75, 60, 60, 43, 43, 25, 25, 5, 5}; //Motor A locations
int mBLoc[20] = {80, 117, 80, 117, 80, 117, 80, 117, 80, 117, 80, 117, 80, 117, 80, 117, 80, 117, 80, 117}; //Motor B locations
int mCLoc[20] = {0, 39, 0, 39, 0, 39, 0, 39, 0, 39, 0, 39, 0, 39, 0, 39, 0, 39, 0, 39}; //Motor C locations
int GmCLoc[20] = {27, 70, 27, 70, 27, 70, 27, 70, 27, 70, 27, 70, 27, 70, 27, 70, 27, 70, 27, 70}; //Motor C grab position
int mDLoc = 180; //Motor D locations
int mELoc = 90; //Motor E locations
int GmELoc = 0; //Motor E grab locations
int BPIN1 = 53; //Button pin locations
int BPIN2 = 51;
int BPIN3 = 49;
int BPIN4 = 47;
int LEDPIN = 43; //LED pin location
int STARTPIN = 45;
int count1 = 0; //Initialize all counts
int count2 = 0; 
int count3 = 0;
int count4 = 0;
int TOTALstart = 0;
int TOTALdest = 0;
int TOTALtotal = 0000;
  
//***************************************************************
void setup() //Robot moves to its start location
{ 
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW); //Start with LED off
  byte numDigits = 4;
  byte digitalPins[] = {2, 3, 4, 5}; //initialize display
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  dis.begin(COMMON_CATHODE, numDigits, digitalPins, segmentPins);
  dis.setBrightness(90);
  dis.setNumber(0000);
  pinMode(BPIN1, INPUT_PULLUP);
  pinMode(BPIN2, INPUT_PULLUP);
  pinMode(BPIN3, INPUT_PULLUP);
  pinMode(BPIN4, INPUT_PULLUP);
  pinMode(STARTPIN, INPUT_PULLUP);
  GoDefault(); //Robot moves to default position
  delay(1000);
  //Sensor setup
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  }
//***************************************************************
void loop() 
{
  
  long duration, distance;
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;

  digitalWrite(LEDPIN, HIGH); //Controller in use
  while(digitalRead(STARTPIN) == HIGH) //Start Button is not pressed
  {
    if(digitalRead(BPIN1) == LOW) //digit 1
    {
      count1++; 
      delay(200); 
    }
    if(digitalRead(BPIN2) == LOW) //digit 2
    {
      count2++; 
      delay(200); 
    }
    if(digitalRead(BPIN3) == LOW) //digit 3
    {
      count3++; 
      delay(200); 
    }
    if(digitalRead(BPIN4) == LOW) //digit 4
    {
      count4++; 
      delay(200); 
    }
    if(count1>1) //If count1 exceeds max value 1
    {
      count1 = 0;
    }
    if(count2>9) //If count2 exceeds max value 9
    {
      count2 = 0; 
    }
    if(count3>1) //If count3 exceeds max value 1
    {
      count3 = 0;
    }
    if(count4>9) //If count4 exceeds max value 9
    {
      count4 = 0;
    }
    
    TOTALstart = count2 + 10*count1; //Count reset if exceeding combined limit
    if(TOTALstart > 19)
    {
      TOTALstart = 0;
      count1 = 0;
      count2 = 0;
    }
    TOTALdest = count4 + 10*count3;
    if(TOTALdest > 19)
    {
      TOTALdest = 0;
      count3 = 0;
      count4 = 0;
    }
    
    TOTALtotal = 1000*count1 + 100*count2 + 10*count3 + count4;
    dis.setNumber(TOTALtotal,2); //Sets output to TOTALtotal value
    dis.refreshDisplay(); 
  }
  
  if(digitalRead(STARTPIN) == LOW) //Start button is pressed
  {
    digitalWrite(LEDPIN, LOW); //LED off, robot excecuting move
    mA.write(mALoc[TOTALstart]); //Move to start location
    delay(500);
    mC.write(mCLoc[TOTALstart]);
    delay(500);
    mB.write(mBLoc[TOTALstart]);
    delay(500);   
    mD.write(mDLoc);
    delay(500);
    mE.write(mELoc);
    delay(500); 
    //delay(1500);

  //Go to and Grab the Object
    if(distance <= 7)
    {
      //grab the object if there is an object
      mC.write(GmCLoc[TOTALstart]-10); 
      delay(500);
      mE.write(GmELoc);
      delay(500);
    } 
    else{
      GoDefault();
      loop(); //back to controller loop
    }

      if((TOTALdest % 2) == 0)
        {
        //Go to second location
        mA.write(mALoc[TOTALdest]); //Move to dest location
        delay(500);
        mB.write(mBLoc[TOTALdest]);
        delay(500);
        mC.write(GmCLoc[TOTALdest]);
        delay(500);   
        mD.write(mDLoc);
        delay(500);
        mE.write(GmELoc);
        delay(500);
        //let go of the grabbed object
        mC.write(mCLoc[TOTALdest]);
        delay(500);
        mE.write(mELoc);
        delay(500);        
      }
      else
        {
        //Go to second location
        mA.write(mALoc[TOTALdest]); //Move to dest location
        delay(500);
        mC.write(GmCLoc[TOTALdest]- 10);
        delay(500);   
        mB.write(mBLoc[TOTALdest]-5);
        delay(500);
        mD.write(mDLoc);
        delay(500);
        mE.write(GmELoc);
        delay(500);
        //let go of the grabbed object
        mE.write(mELoc);
        delay(500);  
        mC.write(mCLoc[TOTALdest]);
        delay(500);      
      }
      //go back to default position
      GoDefault();
      loop(); //back to controller loop
    }
  else
    loop; //back to controller loop
}
//***************************************************************
void GoDefault() //Arm moves to default location
{
  mB.attach(25); 
  mB.write(DEF[2]);
  delay(500);
  mC.attach(27);  
  mC.write(DEF[3]);
  delay(500);  
  mD.attach(29);  
  mD.write(DEF[4]);
  delay(500);
  mE.attach(31); 
  mE.write(DEF[5]);
  delay(500);
  mA.attach(23);
  mA.write(DEF[1]);
  delay(500); 
  mB.write(DEF[2]);
  delay(500);
}
//***************************************************************
