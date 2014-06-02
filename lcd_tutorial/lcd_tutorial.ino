// include the library code:
#include <LiquidCrystal.h>

void displayLCD( char* line1, char* line2);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
                    
void setup()
{
   // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  // Print a message to the LCD.
  displayLCD("Hello", "World!" );
}

void loop()
{
  // do nothing 
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

