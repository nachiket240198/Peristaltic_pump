//This code is to be run with a Secondary Arduino running as Slave. Connect the second arduino with i2c connection. Connect trigger pins to arduino 2.
//To this arduino connect LCD and Rotary Encoder. 
//YOU HAVE TO MAKE CHANGES ON LINES: 100 ; 164 ; 230 ; 264 ; 333

#define CLK 4  // clock pin of Rotary Encoder
#define DT 5   // Data pin of Rotary Encoder
#define SW 6   // push button of Rotary Rncoder. Acts as the "Enter Key"
#define EX 3   // External button. Acts as Force End or exit key (ONLY WHEN A FUNCTION IS RUNNING). 
#define AR 7   // Trigger interrupt for Slave 

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Stepper.h>

//Defining some variables that will be used all along. 
int i = 0;        //Being used to relay back the time from TIME loop
int j = 1;
int k = 0;
int h = 0;        //Hour Variable
int m = 0;        //Minute Variable
int s = 0;        //Second Variable
int z = 0;        //Being used as the switchcase variable
int x = 0;
int y = 0;
int n = 0; 
float o = 0.0;     //Flow rate variable
int p = 0;        
float rpm = 0.0;
int rpm1 = 0;
int st = 0;

int timer = 0; 
int timer1 = 0;   //Timer countdown for the loops
int timer2 = 0;   //Timer countdown for the loops
int t1 = 0;       //For those loops with multiple time settings.
int t2 = 0;       //For those loops with multiple time settings.
int t3 = 0;        //For those loops with multiple time settings.
int currentCLK;     //To store the clock state of rotatry encoder.  
int lastCLK;        //To store the previous clock state of rotary encoder.
unsigned long lastButtonPress = 0;
static uint32_t oldtime=millis();

LiquidCrystal_I2C lcd(0x3f, 16, 2);

void setup()
{
  //Initialise the LCD
  lcd.init();
  lcd.backlight();
  lcd.print("PERISTALTIC");
  lcd.setCursor(0,1);
  lcd.print("PUMP v2.1");
  delay(1000);

  //Initialise I2C bus
  Wire.begin(8);
    
  //Initialise all Pins(Input and Outputs)
  pinMode(DT, INPUT);
  pinMode(CLK, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(EX, INPUT_PULLUP);
  pinMode(AR, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
 // Serial.begin(9600);
  //Declare pin 3 as interrupt pin so that it can break the function during its run.
  attachInterrupt(digitalPinToInterrupt(3), RESET, FALLING);
  lastCLK = digitalRead(CLK);
}

void loop()     //All cases defined in the switch case loop
{
  lcd.clear();
  switch(z)
  {
    lcd.clear();
    
    case 0:           // This is the default off state(Select Mode)
    {
      digitalWrite(AR, HIGH);
      delay(200);
      digitalWrite(AR, LOW);
      lcd.setCursor(0,0);
      lcd.print("Select Mode: ");
      function();
      break;
    }
    
    case 1:           //This is CONTINOUS with time setting
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("1: CONTINOUS");
      delay(1000);     
      TIME();
      FLOW();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("CONTINOUS");
      rpm1 = rpm*10;                        //ANIKET CHANGE THE MULTIPLIER HERE ACCORDING TO WISH. KNOW THAT WE CAN TRANSMIT ONLY INTEGERS UPTO 255! 
                                            //SO IF YOU WANT TO TRANSMIT 68.5rpm THEN YOU CANNOT MULTIPLY WITH 10 AND TRANSMIT. THINK!
                                            //ALSO THIS MULTIPLIER SHOULD WORK FOR OUR WHOLE SCALE OF rpms> THANKS
      while(z == 1)
      {
        oldtime = millis();
        while(timer<i)
        {
          while((millis()-oldtime) < 600)
          {
             Wire.beginTransmission(8);
             Wire.write("X is ");
             Wire.write(rpm1);
             Wire.endTransmission();
             lcd.setCursor(0,1);
             lcd.print(timer);
             lcd.print(" Sec done");
          }
          if((millis()-oldtime) >= 1000)
          {
            //Serial.println(millis());
             oldtime = millis();
             timer++;
          }
        }
        digitalWrite(AR, HIGH);
        delay(500);
        digitalWrite(AR, LOW);
       RESET();
      }
     break;
    }
    
    case 2:           //This is PULSED with time setting
    { 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("2: PULSED");
      lcd.setCursor(0,1);
      lcd.print("Set ON time"); 
      delay(1000);    
      TIME();
      t1 = i;
      i = 0;
      lcd.clear();
      lcd.print("PULSED");
      lcd.setCursor(0,1);
      lcd.print("Set OFF time");
      delay(1000);
      TIME();
      t2 = i;
      i = 0;
      lcd.clear();
      lcd.print("PULSED");
      lcd.setCursor(0,1);
      lcd.print("Set RUN time");
      delay(1000);
      TIME();
      t3 = i;
      i = 0;
      FLOW();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("PULSED");
      rpm1 = rpm*10;                          //ANIKET INSTRUCTIONS FOLLOW HERE!!
      while(z == 2)
      {
        oldtime = millis();
        while(timer < t3)
        {
          digitalWrite(AR, LOW);
          timer1 = 0;
          lcd.clear();
          while(timer1 < t1)
          {
            while((millis()-oldtime) < 600)
            {
              Wire.beginTransmission(8);
              Wire.write("X is ");
              Wire.write(rpm1);
              Wire.endTransmission();
            
              lcd.setCursor(0,0);
              lcd.print("TOTAL: ");
              lcd.print(timer);
              lcd.setCursor(0,1);
              lcd.print("ON:");
              lcd.print(timer1);
            }
            if((millis()-oldtime) >= 1000)
            {
              //Serial.println(millis());
              oldtime = millis();
              timer++;
              timer1++;
            }
          }
          timer2 = 0;
          lcd.clear();
          while(timer2 < t2)
          {
            while((millis()-oldtime) < 600)
            {
              digitalWrite(AR, HIGH);
              lcd.setCursor(0,0);
              lcd.print("TOTAL: ");
              lcd.print(timer);
              lcd.setCursor(0,1);
              lcd.print("OFF:");
              lcd.print(timer2);
            }
            if((millis()-oldtime) >= 1000)
            {
              oldtime = millis();
              timer++;
              timer2++;
            }
          }
        }
       RESET();
      }
     break;
    }

    case 3:           //This is PRIME with time setting
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("3: PRIME");
      delay(1000);     
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("PRIME with LS");
      rpm1 = 10;                            //ANIKET CHANGE THIS VALUE TO THE SMALLEST VALUE CORRESPONDING TO THE MULTIPLIER YOU PUT IN SLAVE CODE
                                          //REMEMBER IT HAS TO BE AN INTEGER!! >THANKS
      while(z == 3)
      {
        while(z == 3)
        {
          while((millis()-oldtime) < 600)
          {
             Wire.beginTransmission(8);
             Wire.write("X is ");
             Wire.write(rpm1);
             Wire.endTransmission();
             lcd.setCursor(0,1);
             lcd.print(timer);
             lcd.print(" Sec done");
          }
          if((millis()-oldtime) >= 1000)
          {
             oldtime = millis();
             timer++;
          }
        }
       RESET();
      }
     break; 
    }

    case 4:           //This is PURGE with time setting
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("4: PURGE");
      delay(1000);     
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("PURGE with HS");
      rpm1 = 50;                     //ANIKET CHANGE THIS VALUE TO THE LARGEST VALUE CORRESPONDING TO THE CONVERSION FACTOR THAT YOU PUT IN SLAVE CODE.  
                                    //REMEMBER THIS VALUE CANNOT BE LARGER THAN 255!! >THANKS
      while(z == 4)
      {
        while(z == 4)
        {
          while((millis()-oldtime) < 600)
          {
             Wire.beginTransmission(8);
             Wire.write("X is ");
             Wire.write(rpm1);
             Wire.endTransmission();
             lcd.setCursor(0,1);
             lcd.print(timer);
             lcd.print(" Sec done");
          }
          if((millis()-oldtime) >= 1000)
          {
             oldtime = millis();
             timer++;
          }
        }
       RESET();
      }
     break;
    }
  }
}

void FLOW()     //Function to set the flow rate and convert to rpm
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set flow rate");
  lcd.setCursor(0,1);
  lcd.print("in ml/min");
  while(n == 0)
 {
   currentCLK = digitalRead(CLK);
   if (currentCLK != lastCLK && currentCLK == 1)
   {
     if(digitalRead(DT) != currentCLK)
     {
        o = o+0.01;
     }
     if(digitalRead(DT) == currentCLK)
     {
        o = o-0.01;
        if(o <= 0)
          {o = 0;} 
     }
    
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Set flowrate");
     lcd.setCursor(0,1);
     lcd.print(o);
     lcd.print(" ml/min");
  }

 lastCLK = currentCLK;
 int btnState = digitalRead(SW);
 if (btnState == LOW) 
 {
    if (millis() - lastButtonPress > 100) 
    {  
      lastButtonPress = millis();
      rpm = 10*o;                  //ANIKET EDIT THIS MULTIPLIER TO CONVERT THE FLOW RATE TO RPM.
      n = 1;
      delay(500);
    }
  }
    delay(1);
  }
}

void function() //Used to read from the rotary encoder which function you want to choose
{
  while(z == 0) //Will break when value of z changes (i.e. when you press the encoder button)
  {
  currentCLK = digitalRead(CLK);

  if (currentCLK != lastCLK && currentCLK == 1)
  {
    if(digitalRead(DT) != currentCLK)
    {
      x ++;
      if(x>=4)
        x = 4; //as we dont have more than 4 functions yet. Can be increased later.
    }
    if(digitalRead(DT) == currentCLK)
    {
      x --;
      if(x <=1)
        x = 1; 
    }
    
  if(x == 1)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Mode selected:");
    lcd.setCursor(0,1);
    lcd.print("1: CONTINOUS");
  }
  if(x == 2)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Mode selected:");
    lcd.setCursor(0,1);
    lcd.print("2: PULSED");
  }
  if(x == 3)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Mode selected:");
    lcd.setCursor(0,1);
    lcd.print("3: PRIME");   
  }
  if(x == 4)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Mode selected:");
    lcd.setCursor(0,1);
    lcd.print("4: PURGE");
  }
  }

 lastCLK = currentCLK;
 int btnState = digitalRead(SW);
 if (btnState == LOW) 
 {
    if (millis() - lastButtonPress > 50) 
    {  
    lastButtonPress = millis();
    z = x;
    delay(500);   //This delay is to prevent multiple button presses detection. Hence release the button within 500ms
    }
  }
  delay(1);
  }
}

void TIME()     //Function to set time of cycle
{
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Set hh:mm:ss");
 j = 1;
 k = 0;
 lcd.setCursor(0,1);
 lcd.print("Set ss");
 mult();
 s = i;
 i = 0; 
 j = 2;
 k = 0;
 lcd.setCursor(0,1);
 lcd.print("Set mm");
 mult();
 m = i;
 i = 0; 
 j = 3;
 k = 0;
 lcd.setCursor(0,1);
 lcd.print("Set hh");
 mult();
 h = i;
 i = (h*3600)+(m*60)+s;
}

void mult()     //Used to read from the rotary encoder the timing.
{
 while(k == 0)
 {
   currentCLK = digitalRead(CLK);
   if (currentCLK != lastCLK && currentCLK == 1)
   {
     if(digitalRead(DT) != currentCLK)
     {
        i ++;
        if(i>=59)
          {i = 59;}
     }
     if(digitalRead(DT) == currentCLK)
     {
        i --;
        if(i <= 0)
          {i = 0;} 
     }
    
    if(j == 1)
   {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Set hh:mm:ss");
     lcd.setCursor(0,1);
     lcd.print("ss: ");
     lcd.print(i);
    }
    if(j == 2)
    {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Set hh:mm:ss");
     lcd.setCursor(0,1);
     lcd.print("mm: ");
     lcd.print(i);
    }
    if(j == 3)
    {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Set hh:mm:ss");
     lcd.setCursor(0,1);
     lcd.print("hh: ");
     lcd.print(i);
    }
  }

lastCLK = currentCLK;
 int btnState = digitalRead(SW);
 if (btnState == LOW) 
 {
    if (millis() - lastButtonPress > 100) 
    {  
      lastButtonPress = millis();
      k = 1;
      delay(500);
    }
  }
    delay(1);
  }
}

void RESET()    //Used as interrupt: to end the ongoing cycle prematurely
{
 //Returns all variables to default state.
  z = 0;
  x = 0;
  i = 0;
  j = 1;
  k = 0;
  h = 0;
  m = 0; 
  n = 0;
  o = 0;
  s = 0;
  t1 = 0;
  t2 = 0; 
  t3 = 0;
  y = 0;
  timer = 0;
  timer1 = 0;
  timer2 = 0;
 
}
