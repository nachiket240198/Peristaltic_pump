//Use this code with the Master Config code. 
//To this arduino connect the stepper driver. Connect the pins (A4 AND A5) of both arduinos. Establish common ground!
//ANIKET YOU NEED TO MAKE CHANGES TO LINES: 8 ; 12 ; 26 ; 39.  

#include <Wire.h>
#include <Stepper.h>

const int SPR = 200;                           //ANIKET CHANGE THE STEPS PER REVOLUTION ACCORDING TO PUMP STEPPER.
int x = 0;
float rpm = 0.0;
long int currenttime = 0;

Stepper mystepper(SPR, 8, 10, 9, 11);           //ANIKET CHANGE THE PINNUMBERS OF THE STEPPER ACCORDING TO YOUR CONNECTIONS.

void setup() 
{
  Serial.begin(9600);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  Wire.begin(8);               
  Wire.onReceive(receiveEvent);
  attachInterrupt(digitalPinToInterrupt(2), RESET, RISING);     //  Defined Interrupt pin.
}

void loop()
{
  if(rpm > 0)
  {
     // Serial.println(rpm);
      if(millis()-currenttime > (6021/rpm))
      {
        //Serial.println("step");
        currenttime = millis();
        mystepper.step(1);
      } 
                                                            
  }
  if(rpm == 0)
  {
   digitalWrite(8, LOW);
   digitalWrite(9, LOW);
   digitalWrite(10, LOW);
   digitalWrite(11, LOW);
  }
}

void receiveEvent(int howMany)
{
  while (1 < Wire.available())
  {
    char c = Wire.read();  
  }
  int x = Wire.read();
  rpm = x+1;
  //Serial.println(rpm);
}

void RESET()
{
  rpm = 0.0;
  x = 0;
}
