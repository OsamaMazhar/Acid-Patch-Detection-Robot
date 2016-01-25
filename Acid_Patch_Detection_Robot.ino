#include <LiquidCrystal.h>
#include <Servo.h>
#include <EEPROM.h>

LiquidCrystal lcd(12, 7, 3, 4, 5, 6);         // initializing the library with the numbers of the interface pins

Servo leftservo;                               // creating servo object to control leftservo
Servo rightservo;                               // creating servo object to control rightservo

const int buttonPin = 2;                        // the number of the pushbutton pin
int buttonState = 0;                            // variable for reading the pushbutton status

//************************************************************************/
//*********Initializing the variables used in the program*****************/
//************************************************************************/

int s[6], i;                
int sixthsensor, sixthsensorval;
int left, right, difference;
int rightmost, leftmost, turn;
int condition, inters, cnt;
int block, patch, patchval, gridval, p[6];
int EEPROMadd = 0;

//************************************************************************/
//****************************setup function******************************/
//************************************************************************/

void setup() {
   
  Serial.begin(9600);           // initializing serial port with baud rate = 9600
  
  pinMode(buttonPin, INPUT);    // initializing the button pin as input
  
  lcd.begin(16, 2);             // initializing the 16X2 LCD 
  
  pinMode(A0, INPUT);           // initializing pins A0 to A4 as digital input pins
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT); 
  
  lcd.setCursor(0,0);           // positioning the cursor of LCD to first row, first column
  
  cnt = 0;                      // initializing the values for certain variables
  patchval = 0;
  gridval = 0;
  block = 1;
  turn = 1; 
  rightmost = 0;        
  leftmost = 0;
  condition = 1;
 
}

//***************************************************************************/
//*******************************setup loop ends*****************************/
//***************************************************************************/

//***************************************************************************/
//******************************start of main loop()*************************/
//***************************************************************************/

void loop() {
  buttonState = digitalRead(buttonPin);         // reading button state
  if(buttonState == HIGH && turn == 1)          // entering loop while button is released
    {
      while(buttonState == HIGH)                // wait until the button is pressed
        {
          buttonState = digitalRead(buttonPin);
          lcd.print("Press Button");            //displaying the message for user help
          lcd.setCursor(0,0);
        }
       turn = 2;                                // changing the value of turn variable such
                                                // that the program never enters the previous
                                                // loop during execution
      
       lcd.clear();                             // clearing the LCD       
       lcd.print("Starting Plz wait");          // printing another message for user help
       delay(2000);
       lcd.clear();
       buttonState = digitalRead(buttonPin);
       if(buttonState == LOW && turn == 2)      //enter if the button is pressed
         {
           lcd.print("1 s");
           delay(1000);                         //wait for 1 second
           for(i=0; i<6; i++)
             p[i] = EEPROM.read(i);             // read values from EEPROM and save in an 
                                                // array p[i] (previous values)
           patch = EEPROM.read(6);              // read patch count from EEPROM
             buttonState = digitalRead(buttonPin);
             while(buttonState == HIGH)         // enter if button is released, wait until the 
                                                // button is pressed again
               {
                 buttonState = digitalRead(buttonPin);
                 lcd.setCursor(0,0);
                 for(i=0; i<6; i++)
                   {lcd.print(p[i]);            // display the values on LCD
                    Serial.print(p[i]);         // send patch values to serial monitor
                   }
                 lcd.print(' ');
                 Serial.print(' ');
                 
                 lcd.print(patch);
                 Serial.print(patch);
                 Serial.println(' ');           //go to new line
                   
               }
            turn = 3;                           // change the value of turn variable so the 
                                                // program never enters this loop during 
                                                // execution
         }
       lcd.clear();
       leftservo.attach(9);                     // initialize both servos and assign pins
       rightservo.attach(10);
       for(i = 0; i < 6; i++)
       EEPROM.write(i, 0);                      // clear EEPROM before running the routine
              // program

       patch = 0;                               // clear patch variable
    }
  
//**********************routine program starts for grid scanning**********************/
     
  sixthsensorval = sixthsensorvalue();         // check for black line/patch detection
  while(sixthsensorval == 1)                   // stay here until ir senses black      
    
{
      sixthsensorval = sixthsensorvalue();    // keep scanning ir-sensor
      linefollow();                           // keep following the line  
      patchval++;                             // increment this value until present in this 
                                              // loop
    }
  
  if(patchval < 200 && patchval > 30)         // if the count is within this band, increment 
                                              // the variable for grid-line
    {
      gridval++;
      delay(500);                             // wait until ir continues to scan
    }

  if(patchval > 200)                          // if the count is greater than 200, it is a
                                              // patch
    {
      patch++;                                // increment the patch count
      delay(50);
      
      if(gridval == 5)                        // if grid variable is 5 (upper right block), do
                                              // not sense the patch twice because of 
                                              // rotation
        patch--;                              // therefore decrease by one count
      if(gridval == 8)                        // same for upper left block, (again rotation)
        patch--;
       
      if(gridval == 1)                        // if patch was in block 1, write in address 0
       {
         EEPROM.write(0, 1);
       }
      if(gridval == 2)                        // and so on
       {
         EEPROM.write(1, 1);
       }
      if(gridval == 3)
       {
         EEPROM.write(2, 1);
       }
      if(gridval == 6)
       {
         EEPROM.write(3, 1);
       }
      if(gridval == 9)
       {
         EEPROM.write(4, 1);
       }
      if(gridval == 10)
       {
         EEPROM.write(5, 1);
       }
    }
  patchval = 0;                           // refresh the count variable
  lcd.setCursor(5,5);                     // move cursor to 5,5 position
 // lcd.print(' ');
  lcd.print(gridval);                     // display grid count
  lcd.print(' ');
  lcd.print(patch);                       // display patch count

       if(gridval == 4)                   // if grid count is 4 (upper right block), take
                                          // a turn until
        {
         s[0] = sensorrd(A0);       
         while(s[0] == 0){              
          leftturn();
          s[0] = sensorrd(A0);
         }
        s[0] = sensorrd(A0);
        s[1] = sensorrd(A1);
        while(gridval == 4 && s[0] != 0 && s[1] !=0)  // the left most line following sensor
                                                      // senses the black line

        linefollow();
        delay(500);                       // wait until it start sensing again for 0.5 sec
        gridval = 5;                      // increment grid value so it doesn’t rotate 
// again
        
        }
      if(gridval == 7)                    // same as above
        {
         s[0] = sensorrd(A0);
         while(s[0] == 0){      
          leftturn();
          s[0] = sensorrd(A0);
         }
         gridval = 8;                     // same as above
         linefollow();
        }
  sixthsensorval = sixthsensorvalue();      
  if(sixthsensorval == 0)                 // if ir-sensor doesn’t detect black line
    {
   //   sixthsensorval = sixthsensorvalue();
      linefollow();                       // keep following the line you’re following
    } 
  if(gridval == 11)                       // when it is finished with all blocks
    {
     gostraight();
     delay(1000);
     leftservo.attach(11);                // detaching servos from 9, 10 so they stop
     rightservo.attach(11);
     lcd.clear();
     EEPROM.write(6, patch);              // write the value of patch count to EEPROM
     for(i=0; i<6; i++)
         p[i] = EEPROM.read(i);           // write the values from EEPROM to p[i]
       
     while(digitalRead(buttonPin) == HIGH && turn == 3) // enter the loop
       {
         lcd.print("Prs Btn 4 s-data");
         lcd.setCursor(0,0);
       }
     lcd.clear();
     while(1)                             // enter the infinite loop
     {
       lcd.setCursor(0,0);
      for(i=0; i<6; i++)
        {
          lcd.print(p[i]);                // keep displaying data on LCD
          Serial.print(p[i]);             // keep sending it to serial port
        }
        lcd.print(' ');
        Serial.print(' ');
        lcd.print(patch);
        Serial.print(patch);
        Serial.println(' ');
        
      
     }
    }
}  

//****************************************************************************/
//****************************main loop ends**********************************/
//****************************************************************************/

/****************************functions definitions start****************************/

int sixthsensorvalue(void)    // function for ir-sensor
  {
    s[5] = analogRead(A5);    // reading analog data from channel 6
//    lcd.print(s[5]);
//    lcd.print(' ');
    
    if(s[5] > 800)            // if the value is greater than 800 infer black line detection
      sixthsensor = 1;
    else if(s[5] < 700)       // no black line
      sixthsensor = 0;
    
//    lcd.print(sixthsensor);  
//    lcd.setCursor(0,0);
    return sixthsensor;       // return the value to the main loop
  }
  
void linefollow(void)         // function for line follow
  {
    s[0] = sensorrd(A0);      // read the sensor values in binary form
    s[1] = sensorrd(A1);
    s[2] = sensorrd(A2);
    s[3] = sensorrd(A3);
    s[4] = sensorrd(A4);
    sixthsensorval = sixthsensorvalue();  // read ir-sensor
              
    lcd.setCursor(0,0);
    
    for(i = 0; i<5; i++) {
      lcd.print(s[i]);                 // print status of LSS05 sensors
      lcd.print(' ');
    }
    
    difference = differencefunc();    // use difference function to get the difference value
        
    switch (difference) {             // use switch case for different values of difference
    case 0:
      rightcurve();
      break;
    case 1:
      rightlittlecurve();
      break;
    case 2:
      rightlittlecurve();
      break;
    case 3:
      gostraight();
      break;
    case 4:
      leftlittlecurve();
      break;
    case 5:
      leftlittlecurve();
      if(sixthsensorval == 1)
        gostraight();
      break;
    case 6:
      leftcurve();
      break;
    }
  }

int sensorrd(char channel){             // for LSS05 sensor
  int dummysensor;
  if(digitalRead(channel) == HIGH)      // if HIGH replace it with decimal 1
    dummysensor = 1;
  else if(digitalRead(channel) == LOW)  // if LOW replace it with decimal 0
    dummysensor = 0;
  return dummysensor;
}

int differencefunc(void)                // difference function
  {
//   lcd.setCursor(0,1);
   
   left = s[0] + s[1] + s[2] + 3;       // 3 is added to avoid negative results
   right = s[2] + s[3] + s[4];
   
   difference = left - right;
      
   if(difference == 2)                  // differentiating value = 2 resulting from extreme sensors
     {
      if(s[4] == 1 && s[3] == 0)
         rightmost = 1;
     }
   else
     rightmost = 0;
   
   if(difference == 4)                  // differentiating value = 4 resulting from extreme sensors
       {
        if(s[0] == 1 && s[1] == 0)
          leftmost = 1;
       }
   else
     leftmost = 0;  
  
   if(difference < 3)
     {
       if(rightmost == 1)
         difference = 0;
     }
     
   if(difference > 3)
     {
       if(leftmost == 1)
         difference = 6;
     }
     
   return difference; 
  }
  

void rightslightestcurve(void)            // functions for servo motors
  {
    leftservo.write(100);
    rightservo.write(86);
  }
  
void rightlittlecurve(void)
  {
    leftservo.write(102);
    rightservo.write(87);
  }
  
void rightcurve(void)
  {
    leftservo.write(103);
    rightservo.write(88);
  }

void leftslightestcurve(void)
  {
    leftservo.write(99);
    rightservo.write(85);
  }
  
void leftlittlecurve(void)
  {
    leftservo.write(96);
    rightservo.write(83);
  }
  
void leftcurve(void)
  {
    leftservo.write(95);
    rightservo.write(85);
  }

void gostraight(void)
  {
    leftservo.write(100);
    rightservo.write(84);
  }
  
void leftturn(void)
  {
    leftservo.write(92);
    rightservo.write(70);
  }
  
 

