// include the library code:
#include <LiquidCrystal.h>

#define btnNextPin 10

int lastInterrupt = 0; // time since last interrupt, for switch debouncing
void BtnNext();

void displayLCD( char* line1, char* line2);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
             
void setup()
{ 
  pinMode(btnNextPin, INPUT);  
  attachInterrupt(btnNextPin, BtnNext, CHANGE);
   
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  // Print a message to the LCD.
  displayLCD("Press to roll", "" );
}

void loop()
{
 // do nothing, interrupts will handle it
}

// interrupt handler for next button
void BtnNext()
{
  if( millis() - lastInterrupt < 500 )
   {
     return;
   } 
  else
   {
     lastInterrupt = millis();
     int r1 = random(1,7);
     int r2 = random(1,7);
     char dice[16];
     sprintf(dice, "%d %d", r1, r2);
     displayLCD("Press to roll", dice);
   } 
}

// routine to display text to the LCD
void displayLCD( char* line1, char* line2)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}
