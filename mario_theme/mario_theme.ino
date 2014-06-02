

// 2 voices on 1 Arduino
// demo by John Harrison
// Nov 28, 2011
// public domain
// Modified by Dipto Pratyaksa
// Added Mario tunes by Dipto Pratyaksa
// Apr 3, 2013
// v 1.1

// for the demo to work unmodified, hook up a 1Kish resistor to pin 11 and a 1Kish resistor to pin 12.
// tie the other ends of the resistors together and hook those up to an amp and speaker.
// or you could skip the resistors and hook up pin 11 to one side of a stereo amp and pin 12 to the other side

// You have to included pitches.h. Below shows the path on my computer. Yours will be similar
#include <pitches.h>

// as stated above,
// connect amp/speaker to FIRST_TONE_PIN and subsequent.
// for example, if FIRST_TONE_PIN is 8 and NUM_OF_VOICE is 9,
// connect amp/speaker to arduino  pin 8 and pin 9.
// if using the same amp/speaker for both pins, have each
// pin go through a 1Kish resistor before being summed together
#define FIRST_TONE_PIN 8
#define NUM_OF_VOICES 2

// a future version will support more than 2 simultaneous voices

// notes in the melody:
int melody1[] = {
  NOTE_E7, NOTE_E7,0,NOTE_E7, 
  0, NOTE_C7, NOTE_E7,0,
  NOTE_G7,0,0,0,
  NOTE_G6,0,0,0,

  NOTE_C7, 0, 0,NOTE_G6, 
  0, 0,NOTE_E6,0,
  0, NOTE_A6, 0, NOTE_B6, 
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G7, NOTE_E7, NOTE_G7, 
  NOTE_A7, 0, NOTE_F7, NOTE_G7, 
  0, NOTE_E7, 0,NOTE_C7, 
  NOTE_D7, NOTE_B6, 0,0,

  NOTE_C7, 0, 0,NOTE_G6, 
  0, 0,NOTE_E6,0,
  0, NOTE_A6, 0, NOTE_B6, 
  0, NOTE_AS6, NOTE_A6, 0,

  NOTE_G7, NOTE_E7, NOTE_G7, 
  NOTE_A7, 0, NOTE_F7, NOTE_G7, 
  0, NOTE_E7, 0,NOTE_C7, 
  NOTE_D7, NOTE_B6, 0,0,
};

int harmony1[] = {
  NOTE_FS6, NOTE_FS6,0,NOTE_FS6, 
  0, NOTE_FS6, NOTE_FS6,0,
  NOTE_B6,0,0,0,
  NOTE_G6,0,0,0,

  NOTE_E6, 0, 0,NOTE_C6, 
  0, 0,NOTE_G5,0,
  0, NOTE_C6, 0, NOTE_D6, 
  0, NOTE_CS6, NOTE_C6, 0,

  NOTE_C6, NOTE_G6, NOTE_B6, 
  NOTE_C7, 0, NOTE_A6, NOTE_B6, 
  0, NOTE_A6, 0,NOTE_E6, 
  NOTE_F6, NOTE_D6, 0,0,

  NOTE_E6, 0, 0,NOTE_C6, 
  0, 0,NOTE_G5,0,
  0, NOTE_C6, 0, NOTE_D6, 
  0, NOTE_CS6, NOTE_C6, 0, 

  NOTE_C6, NOTE_G6, NOTE_B6, 
  NOTE_C7, 0, NOTE_A6, NOTE_B6, 
  0, NOTE_A6, 0,NOTE_E6, 
  NOTE_F6, NOTE_D6, 0,0,
};

int underworld_melody[] = {
  NOTE_C5, NOTE_C6, NOTE_A4, NOTE_A5, 
  NOTE_AS4, NOTE_AS5, 0,
  0,
  NOTE_C5, NOTE_C6, NOTE_A4, NOTE_A5, 
  NOTE_AS4, NOTE_AS5, 0,
  0,
  NOTE_F4, NOTE_F5, NOTE_D4, NOTE_D5,
  NOTE_DS4, NOTE_DS5, 0,
  0,
  NOTE_F4, NOTE_F5, NOTE_D4, NOTE_D5,
  NOTE_DS4, NOTE_DS5, 0,
  0, NOTE_DS5, NOTE_D5, NOTE_CS5,
  NOTE_C5, NOTE_DS5, 
  NOTE_DS5, NOTE_GS4,
  NOTE_G4, NOTE_CS5,
  NOTE_C5, NOTE_FS5,NOTE_F5, NOTE_E4, NOTE_AS5, NOTE_A5,
  NOTE_GS5, NOTE_DS5, NOTE_B4,
  NOTE_AS4, NOTE_A4, NOTE_GS4,
  0, 0, 0
};

int underworld_melody1[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4, 
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4, 
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_D4, NOTE_CS4,
  NOTE_C4, NOTE_DS4, 
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4,NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  0, 0, 0
};

//melody notes in miliseconds
int underworld_tempo[] = {
  200, 200, 200, 200, 
  200, 200, 400,
  800,
  200, 200, 200, 200, 
  200, 200, 400,
  800,
  200, 200, 200, 200, 
  200, 200, 400,
  800,
  200, 200, 200, 200, 
  200, 200, 400,
  400, 133, 133, 133,
  400, 400,
  400, 400,
  400, 400,
  133, 133, 133,133, 133, 133,
  267, 267, 267,
  267, 267, 267,
  800, 800, 800
};

int mel1Durations[] = {
  150, 150, 150,150, 
  150, 150, 150,150,
  150, 150,150,150,
  150, 150,150,150,

  150, 150,150,150,
  150, 150,150,150,
  150, 150, 150,150, 
  150, 150, 150,150, 

  200, 200, 200,
  150, 150,150,150,
  150, 150, 150,150, 
  150, 150, 150,150,

  150, 150, 150,150, 
  150, 150, 150,150,   
  150, 150, 150,150, 
  150, 150, 150,150, 

  200, 200, 200, 
  150, 150, 150,150,   
  150, 150, 150,150, 
  150, 150, 150,150,
};

int harm1Durations[] = {
  150, 150, 150,150, 
  150, 150, 150,150,
  150, 150, 150,150, 
  150, 150, 150,150, 

  150, 150, 150,150, 
  150, 150, 150,150,   
  150, 150, 150,150, 
  150, 150, 150,150, 

  150, 150, 150,150, 
  150, 150, 150,150,   
  150, 150, 150,150, 
  150, 150, 150,150,

  150, 150, 150,150, 
  150, 150, 150,150,   
  150, 150, 150,150, 
  150, 150, 150,150, 

  150, 150, 150,150, 
  150, 150, 150,150,   
  150, 150, 150,150, 
  150, 150, 150,150,
};

void setup() {
  Serial.println("play");
  Serial.begin(9600);
  for (unsigned char i = 0; i < NUM_OF_VOICES; i++) 
   {
    pinMode(i + FIRST_TONE_PIN, OUTPUT);
    Serial.println(i);
   }

  //led indicator
  pinMode(13,OUTPUT);

  // in this test example, the harmony and the melody are initially the same
  // so we raise the harmony up an octave and slow it down to 1/2 speed
  for (int j = 0; j < (sizeof(harmony1) / sizeof(int)); j++) 
   {
     harmony1[j] = harmony1[j] *1;
     harm1Durations[j] = harm1Durations[j] * 1;
     Serial.println(harmony1[j]);
   }

  // call magic function to play both voices
  Serial.println("play");
  playTune(melody1, sizeof(melody1)/sizeof(int), mel1Durations, harmony1, sizeof(harmony1)/sizeof(int), mel1Durations);
  playTune(underworld_melody, sizeof(underworld_melody)/sizeof(int), underworld_tempo, underworld_melody1, sizeof(underworld_melody1)/sizeof(int), underworld_tempo);
  Serial.println("play");
}

void loop() 
  {
   // if we get here, we played the melody
   // all we can do is pee in corner of circular room
   //while (1)
   playTune(melody1, sizeof(melody1)/sizeof(int), mel1Durations, harmony1, sizeof(harmony1)/sizeof(int), mel1Durations);
   playTune(underworld_melody, sizeof(underworld_melody)/sizeof(int), underworld_tempo, underworld_melody1, sizeof(underworld_melody1)/sizeof(int), underworld_tempo);
   Serial.println("play");
  }

// no need to modify anything below for basic use
void playTune(int melody[], int melLength, int melDurations[], int harmony[], int harmLength, int harmDurations[])
{
  unsigned int Counts[] = { 0, 0};
  unsigned int Periods[] = { 0, 0};
  unsigned char States[] = { 0, 0};

  unsigned long stopTimeNote1 = 0;
  unsigned long stopTimeNote2 = 0;
  unsigned char TimedPin = 0;
  unsigned int CurrentCount = 0;
  unsigned char i;
  unsigned char indexMel = 0, indexHarm = 0;

  while (1) {
    CurrentCount = Counts[0];
    TimedPin = 0;
    for (i=1;i<NUM_OF_VOICES;i++) 
     {
      if (Counts[i] < CurrentCount) 
       {
        TimedPin = i;
        CurrentCount = Counts[i];
       } // for
     }
    if (CurrentCount > 3)
      delayMicroseconds(CurrentCount);
    if (Periods[TimedPin] < 65535)
      States[TimedPin] = !States[TimedPin];

    digitalWrite(13, HIGH);
    digitalWrite(FIRST_TONE_PIN+TimedPin,States[TimedPin]);
    digitalWrite(13, LOW);

    if (millis() >= stopTimeNote1) 
     {
      if (indexMel >= melLength)
        break;
      stopTimeNote1 = millis() + melDurations[indexMel];
      Periods[0] = 1000000 / melody[indexMel++];
     }
    if (millis() >= stopTimeNote2) 
     {
      if (indexHarm >= harmLength)
        break;
      stopTimeNote2 = millis() + harmDurations[indexHarm];
      Periods[1] = 1000000 / harmony[indexHarm++];
     }

    for (i = 0; i < NUM_OF_VOICES; i++) 
     {
      Counts[i] = Counts[i] - CurrentCount;
     }
    Counts[TimedPin] = Periods[TimedPin];
  }
}
