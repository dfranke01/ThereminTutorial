//  this code reuses code from several places, for the DDS part, look here 
//
// http://rcarduino.blogspot.com/2012/12/arduino-due-dds-part-1-sinewaves-and.html
// http://interface.khm.de/index.php/lab/experiments/arduino-dds-sinewave-generator/
// 
// for timer interrupts see
// http://arduino.cc/forum/index.php?action=post;topic=130423.15;num_replies=20
//
// for controlling the lcd, most was taken from the examples included with the Arduino install
//    File->Examples->LiquidCrystal->HelloWorld
//
// These are the clock frequencies available to the timers /2,/8,/32,/128
// 84Mhz/2 = 42.000 MHz
// 84Mhz/8 = 10.500 MHz
// 84Mhz/32 = 2.625 MHz
// 84Mhz/128 = 656.250 KHz
//
// 44.1Khz = CD Sample Rate
// Lets aim for as close to the CD Sample Rate as we can get -
//
// 42Mhz/44.1Khz = 952.38
// 10.5Mhz/44.1Khz = 238.09 // best fit divide by 8 = TIMER_CLOCK2 and 238 ticks per sample
// 2.625Hmz/44.1Khz = 59.5
// 656Khz/44.1Khz = 14.88 // use for servo control 13120 / 65600 = .02 ( update servos every 20 milliseconds )

// 84Mhz/44.1Khz = 1904 instructions per tick

// include the library code:
#include <LiquidCrystal.h>
#include <Servo.h>
#include "songdata.h"
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INPUT PARAMS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define btnNext 0
#define btnSelect 1
#define btnNextPin 50
#define btnSelectPin 51
#define freqServoPin 9
#define volServoPin 8

int fMinInput = 214;  // corresponds to 26 degrees
int fMaxInput = 594;  // corresponds approx to 154 degrees
int fMinAngle = 26;
int fMaxAngle = 154;
int minNote = 50;
int maxNote = 100;
int vMinInput = 350;
int vMaxInput = 435;
int vMinAngle = 75;
int vMaxAngle = 105;
int scaleFactor = (fMaxInput - fMinInput) / (maxNote - minNote);

float fAngle = 0.0;
float vAngle = 0.0;

Servo freqServo;
Servo volServo;

int lastInterrupt = 0; // time since last interrupt, for switch debouncing
void BtnNext();
void BtnSelect();

// we are using a servo as a potentiometer to control the volume. 
#define MAX_VOL 10 // we'll have 10 volume levels that can be set in software
uint32_t volume = MAX_VOL; // var we'll actually use to control the volume

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DDS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// the phase accumulator points to the current sample in our wavetable
uint32_t ulPhaseAccumulator = 0;
// the phase increment controls the rate at which we move through the wave table
// higher values = higher frequencies
volatile uint32_t ulPhaseIncrement = 0;   // 32 bit phase increment, see below

// full waveform = 0 to SAMPLES_PER_CYCLE
// Phase Increment for 1 Hz =(SAMPLES_PER_CYCLE_FIXEDPOINT/SAMPLE_RATE) = 1Hz
// Phase Increment for frequency F = (SAMPLES_PER_CYCLE/SAMPLE_RATE)*F
#define SAMPLE_RATE 44100.0
#define SAMPLES_PER_CYCLE 600
#define SAMPLES_PER_CYCLE_FIXEDPOINT (SAMPLES_PER_CYCLE<<20)
#define TICKS_PER_CYCLE (float)((float)SAMPLES_PER_CYCLE_FIXEDPOINT/(float)SAMPLE_RATE)

// We have 521K flash and 96K ram to play with
// Create a table to hold the phase increments we need to generate midi note frequencies at our 44.1Khz sample rate
#define MIDI_NOTES 128
uint32_t nMidiPhaseIncrement[MIDI_NOTES];

// Create a table to hold pre computed sinewave, the table has a resolution of 600 samples
#define WAVE_SAMPLES 600
// default int is 32 bit, in most cases its best to use uint32_t but for large arrays its better to use smaller
// data types if possible, here we are storing 12 bit samples in 16 bit ints
uint16_t nSineTable[WAVE_SAMPLES];

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Song Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void displayLCD( char* line1, char* line2);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int playingSong = false; // state variable to indicate a preloaded song should be playing
int iNote = 0; // index to indicate what note of particular song is playing
uint32_t interruptCtr = 0; // ctr used for timing servo PWM 
uint32_t volInterruptCtr = 0; // ctr used for timing servo PWM 
uint32_t noteDurationCtr = 0; // ctr used to control how long a given song note should play
     
Song::Song( char *songName, songData *newSongData, int songLength)
  {
    this->name = songName; 
    this->data = newSongData;
    this->SongLength = songLength;
  };
  
// declare our song  
Song *SomewhereOTR = new Song::Song("SWOTR", SWOTR, sizeof(SWOTR)/sizeof(SWOTR[0]));

// declare vars for the number of songs
int numSongs = 1;
Song *Songs = (Song*) calloc( numSongs, sizeof(Song));

int iCurrSong = 0; 

// function called to play a demo
void PlaySong( songData* song ); 

void setup()
{
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);  
  
   // load songs array 
   Songs = SomewhereOTR;
    
  // Print a message to the LCD.
  displayLCD("Press Sel 4Demo", Songs[0].name );
  
  pinMode(btnNextPin, INPUT);
  pinMode(btnSelectPin, INPUT);
   
  attachInterrupt(btnNextPin, BtnNext, CHANGE);
  attachInterrupt(btnSelectPin, BtnSelect, CHANGE);
   
  createNoteTable(SAMPLE_RATE);
  createSineTable();
 
  /* turn on the timer clock in the power management controller */
  pmc_set_writeprotect(false);		 // disable write protection for pmc registers
  pmc_enable_periph_clk(ID_TC6);	 // enable peripheral clock TC6
  pmc_enable_periph_clk(ID_TC7);	 // enable peripheral clock TC7

  /* we want wavesel 01 with RC */
  TC_Configure(/* clock */TC2,/* channel */0, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK2);
  TC_Configure(/* clock */TC2,/* channel */1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
  TC_SetRC(TC2, 0, 238); // sets <> 44.1 Khz interrupt rate 
  TC_SetRC(TC2, 1, 13120);
  TC_Start(TC2, 0);
  TC_Start(TC2, 1);
 
  // enable timer interrupts on the timer
  TC2->TC_CHANNEL[0].TC_IER=TC_IER_CPCS;   // IER = interrupt enable register
  TC2->TC_CHANNEL[0].TC_IDR=~TC_IER_CPCS;  // IDR = interrupt disable register
  TC2->TC_CHANNEL[1].TC_IER=TC_IER_CPCS;   // IER = interrupt enable register
  TC2->TC_CHANNEL[1].TC_IDR=~TC_IER_CPCS;  // IDR = interrupt disable register
 
  /* Enable the interrupt in the nested vector interrupt controller */
  /* TC4_IRQn where 4 is the timer number * timer channels (3) + the channel number (=(1*3)+1) for timer1 channel1 */
  NVIC_EnableIRQ(TC6_IRQn);
  NVIC_EnableIRQ(TC7_IRQn);

  // this is a cheat - enable the DAC
  analogWrite(DAC1,0);
}

void loop()
{
  // read analog input 0, drop the range from 0-1024 to minNote-maxNote
  // then look up the phaseIncrement required to generate the note in our nMidiPhaseIncrement table
  uint32_t ulInput = min( max( analogRead(0), fMinInput ), fMaxInput);
  ulInput = ((ulInput - fMinInput) / scaleFactor) + minNote;  // scale to minNote to maxNote
 
  // get volume from analog input, then scale it to range vMinInput to vMaxInput
  volume = min( max( analogRead(1), vMinInput ), vMaxInput);
  volume = ((volume - vMinInput) / 8);
  
  ulPhaseIncrement = nMidiPhaseIncrement[ulInput];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INTERRUPT HANDLERS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// this is the interrupt handler that is called at our sample rate of 44100 times per second, its primary job is to update 
// the value writtento the DAC port
void TC6_Handler()
{
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC2, 0);
 
  ulPhaseAccumulator += ulPhaseIncrement;   // 32 bit phase increment, see below

  // if the phase accumulator over flows - we have been through one cycle at the current pitch,
  // now we need to reset the grains ready for our next cycle
  if(ulPhaseAccumulator > SAMPLES_PER_CYCLE_FIXEDPOINT)
  {
   // DB 02/Jan/2012 - carry the remainder of the phase accumulator
   ulPhaseAccumulator -= SAMPLES_PER_CYCLE_FIXEDPOINT;
  }

  // get the current sample  
  uint32_t ulOutput = volume * nSineTable[ulPhaseAccumulator>>20]; 
 
  // we cheated and user analogWrite to enable the dac, but here we want to be fast so
  // write directly 
  dacc_write_conversion_data(DACC_INTERFACE, ulOutput);
}

// this is the interrupt handler that is called every 20 mS (i.e. time between pulses sent to a servo), its job is to play a demo of a song
void TC7_Handler()
{
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC2, 1);
  
  // handle servo output if we are in song playing mode
  if( playingSong )
    {
      fAngle = ((float)(Songs[iCurrSong].data[iNote].note - minNote) / (float)(maxNote - minNote)) * (fMaxAngle - fMinAngle) + fMinAngle;
      freqServo.write(fAngle);
               
      // test if we need to increment to the next note
      if( noteDurationCtr >= Songs[iCurrSong].data[iNote].duration )
        {
         noteDurationCtr = 0;
         // increment note and test if we have finished the song
         if( ++iNote >= Songs[iCurrSong].SongLength )
           {
            // song is over
            playingSong = false;
            iNote = 0;
            freqServo.detach();
            volServo.detach();
            displayLCD("Press sel 4Demo", Songs[iCurrSong].name );
           }   
         else
           { 
            // set volume for next note
            volume = Songs[iCurrSong].data[iNote].volume;
            vAngle = vMinAngle + (volume / (float) MAX_VOL * (float)(vMaxAngle - vMinAngle));
            volServo.write(vAngle);
           }
        }
        noteDurationCtr++;
    }
}
  
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SONG RELATED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// create the notes we want to be abale to play, i.e. a list of frequencies
void createNoteTable(float fSampleRate)
{
  for(uint32_t unMidiNote = 0; unMidiNote < 128; unMidiNote++)
  {
    float fFrequency = ((pow(2.0,(unMidiNote-69.0)/12.0)) * 440.0);
    nMidiPhaseIncrement[unMidiNote] = fFrequency*TICKS_PER_CYCLE;
  }
}

// create the individual samples for our sinewave table
void createSineTable()
{
  for(uint32_t nIndex = 0;nIndex < WAVE_SAMPLES;nIndex++)
  {
    // normalised to 12 bit range 0-4095
    nSineTable[nIndex] = (uint32_t)  (((1+sin(((2.0*PI)/WAVE_SAMPLES)*nIndex))*4095.0)/2) / MAX_VOL;
  }
}

// function to call when a song is to be played
void PlaySong(struct songData* song)
{
  displayLCD("Playing", Songs[iCurrSong].name);

  playingSong = true;
  // enable servos for output
  freqServo.attach(9);
  volServo.attach(8);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// I/O FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// routine to display text to the LCD
void displayLCD( char* line1, char* line2)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
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
     // increment song num
     iCurrSong = ((iCurrSong+1) % numSongs); 
     displayLCD("Press sel 4Demo", Songs[iCurrSong].name );
   }
}

// interrupt handler for select button
void BtnSelect()
{
 if( millis() - lastInterrupt < 500 )
   {
     return;
   } 
 else
   {
     lastInterrupt = millis();
     // Play Current song
     PlaySong(Songs[iCurrSong].data);
     
   }
}
