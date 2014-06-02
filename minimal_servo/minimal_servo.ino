int servoPin = 9;

void setup() 
 {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(servoPin, OUTPUT);  // attaches the servo on pin 9 to the servo object 
 }

void loop() 
 {
  // put your main code here, to run repeatedly: 
  for(int pulse = 500; pulse < 2500; pulse+=14)
    {
     digitalWrite(servoPin, HIGH); 
     delayMicroseconds(pulse);
     digitalWrite(servoPin, LOW);
     delay(20);
    }
  
 }
