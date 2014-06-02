int servoPin = 9;    // variable to store the servo position 
 
void setup() 
{ 
  digitalWrite(servoPin, LOW);
} 
 
void loop() 
{ 
  // send 1.5 mS pulse
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(150); 
  digitalWrite(servoPin, LOW);
  delay(3000);
 
  // send .5 mS pulse
  digitalWrite(servoPin, HIGH);
  delayMicroseconds(50); 
  digitalWrite(servoPin, LOW);
  delay(3000);
} 

