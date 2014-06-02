// code to read a servo position, requires an additional wire to be connected to the 
// servo potentiometer
// 
// adapted from
// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int pos = 0;    // variable to store the servo position 
int readPin = 0;      // use anaglog input 0 to read servo
int positionRead = 0.0;

void setup() 
{ 
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  analogReference(INTERNAL2V56); 
  Serial.begin(9600);
} 
  
void loop() 
{ 
  for(pos = 0; pos < 180; pos += 1)     // goes from 0 degrees to 180 degrees 
  {                                     // in steps of 1 degree
    positionRead = analogRead(readPin); // read position and report 
    //Serial.println("positionRead = ");
    Serial.println(positionRead);
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    delay(45);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos >= 1; pos -=1 ) // goes from 180 degrees to 0 degrees 
  {
    positionRead = analogRead(readPin);
    //Serial.println("positionRead = ");
    //Serial.println(positionRead);    
    myservo.write(pos);             // tell servo to go to position in variable 'pos' 
    
    delay(45);                      // waits 15ms for the servo to reach the position 
  } 
} 
