#include <Servo.h>
//#include <midi_notes.h>

Servo freqServo;
Servo volServo;
volatile int angle = 175;

std::map<string, int> noteMap;
//  this code reuses code from several places, for the DDS part, look here 
//
// http://rcarduino.blogspot.com/2012/12/arduino-due-dds-part-1-sinewaves-and.html
// http://interface.khm.de/index.php/lab/experiments/arduino-dds-sinewave-generator/
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
// 656Khz/44.1Khz = 14.88

// 84Mhz/44.1Khz = 1904 instructions per tick

// include the library code:
#include <LiquidCrystal.h>

#define btnNext 0
#define btnSelect 1
#define btnNextPin 50
#define btnSelectPin 51
#define freqServoPin 9
#define volServoPin 8

// fill the note table with the phase increment values we require to generate the note
uint32_t lowMidiNote = 45; // 45 to 110 will be 110Hz to 3520 Hz
uint32_t highMidiNote = 110;  

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

// to represent 600 we need 10 bits
// Our fixed point format will be 10P22 = 32 bits


// We have 521K flash and 96K ram to play with

// Create a table to hold the phase increments we need to generate midi note frequencies at our 44.1Khz sample rate
#define MIDI_NOTES 128
uint32_t nMidiPhaseIncrement[MIDI_NOTES];

// Create a table to hold pre computed sinewave, the table has a resolution of 600 samples
#define WAVE_SAMPLES 600
// default int is 32 bit, in most cases its best to use uint32_t but for large arrays its better to use smaller
// data types if possible, here we are storing 12 bit samples in 16 bit ints
uint16_t nSineTable[WAVE_SAMPLES];

// we are using a servo as a potentiometer to control the volume. It reports values from  90 - 640, so there are 550 possible volume increments.
#define MAX_VOLUME 10 // we'll have 10 volume levels that can be set in software
uint32_t volume = MAX_VOLUME; // var we'll actually use to control the volume

struct SongData {
  int note;
  int duration;
  int volume;
};

int lastInterrupt = 0; // time since last interrupt, for switch debouncing
void BtnNext();
void BtnSelect();
void displayLCD( char* line1, char* line2);

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// define some values used by the panel and buttons
int lcd_key    = 0;
int adc_key_in = 0;
int playingSong = true; // state variable to indicate a preloaded song should be playing
int PWM_period = 770; // period used to send PWM signal to servos ( send a pulse to servos every 20 MS = 770 * .000026
int iNote = 0; // index to indicate what note of particular song is playing
uint32_t interruptCtr = 0; // ctr used for timing servo PWM 
uint32_t volInterruptCtr = 0; // ctr used for timing servo PWM 
uint32_t noteDurationCtr = 0; // ctr used to control how long a given song note should play

// array of song Data
// actual song data, should prob put this in a header file
// setting the servo to 0 degrees requires a 1 millisecond pulse, 90 degrees a 1.5 mS pulse, and 180, a 2.0 mS pulse
// the interrupt fires @ 44.1 kHz, which is once every 26 microseconds, to set the servo to a position of 0 degrees, we need a pulse
// of 1 millisecond, so .001 / .000026 = 38.4 interrupts should occur to produce a 1 millisecond pulse
// 90 degrees .0015 / .000026 = 57.69 interrupts to produce a 1.5 millisecond pulse
// 180 degrees .002 / .000026 = 76.92 interrupts to produce a 2.0 millisecond pulse
SongData STT[] = { {59, 200000, 10},     // 200000*.000026 ~= 5 seconds
                   {97, 200000, 10},     // volume = 5
                   {59, 200000, 10},
                   {97, 200000, 5},     
                   {59, 200000, 5},     
                   {97, 200000, 5},
                   {59, 200000, 10},   
                   {97, 200000, 10}, 
                   {59, 200000, 10}};
    
char* songNames[] = { "Star Trek Theme", "SWOT"};
SongData *songs[] = {STT};
SongData *currSong = songs[0]; // currSong points to the song that is playing
int iCurrSong = 0; // index for 2 arrays, songs[] and songNames[]
int currSongLength = sizeof(STT) / sizeof(STT[0]); // number of notes to play for a particular song
void PlaySong( SongData* song ); 

void setup()
{
  freqServo.attach(9);
  volServo.attach(8);
   
  Serial.begin(9600);
  
  pinMode(btnNextPin, INPUT);
  pinMode(btnSelectPin, INPUT);
   
  attachInterrupt(btnNextPin, BtnNext, CHANGE);
  attachInterrupt(btnSelectPin, BtnSelect, CHANGE);
   
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  createNoteTable(SAMPLE_RATE);
  createSineTable();
 
  /* turn on the timer clock in the power management controller */
  pmc_set_writeprotect(false);		 // disable write protection for pmc registers
  pmc_enable_periph_clk(ID_TC6);	 // enable peripheral clock TC6
  pmc_enable_periph_clk(ID_TC7);	 // enable peripheral clock TC7

  /* we want wavesel 01 with RC */
  TC_Configure(/* clock */TC2,/* channel */0, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK2);
  TC_SetRC(TC2, 0, 238); // sets <> 44.1 Khz interrupt rate
  TC_SetRC(TC2, 1, 210000); 
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
  // read analog input 0 drop the range from 0-1024 to 0-127 with a right shift 3 places,
  // then look up the phaseIncrement required to generate the note in our nMidiPhaseIncrement table
  uint32_t ulInput = analogRead(0);
  volume = min(10, (analogRead(1) - 65) / 25);
  ulPhaseIncrement = nMidiPhaseIncrement[ulInput>>3];
}

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

void TC7_Handler()
{
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC2, 1);
  
   // handle servo output if we are in song playing mode
  if( playingSong )
    {
     // play the current note
     
     
     // test if we need to increment to the next note
     if( noteDurationCtr >= currSong[iNote].duration )
       {
        noteDurationCtr = 0;
        // increment note and test if we have finished the song
        if( iNote++ >= currSongLength )
          {
           // song is over
           playingSong = false;
          }   
      //else
      //{ 
         // set volume for next note
         //volume = currSong[iNote].volume;
        //}
    }
  
  // handle servo output if we are in song playing mode
  if( playingSong )
    {
      if(interruptCtr == 1)
        {
         freqServo.write(160);
         volServo.write(45);
        } 
      if(interruptCtr == 10)
        {
         freqServo.write(110);  
         volServo.write(90);   
        }
      if(interruptCtr == 20)
        {
         interruptCtr = 0;
        }    
      interruptCtr++;
    }
}
  
  
// create the notes we want to be abale to play, i.e. a list of frequencies
void createNoteTable(float fSampleRate)
{
  for(uint32_t unMidiNote = lowMidiNote; unMidiNote < highMidiNote; unMidiNote++)
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
    nSineTable[nIndex] = (uint32_t)  (((1+sin(((2.0*PI)/WAVE_SAMPLES)*nIndex))*4095.0)/2) / MAX_VOLUME;
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
    int numSongs = sizeof(songNames) / sizeof(songNames[0]);
    displayLCD("Press sel 4Demo", songNames[++iCurrSong % numSongs] );  
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
     PlaySong(STT);
   }
}

// function to call when a song is to be played
void PlaySong(struct SongData* song)
{
  displayLCD("Playing", "Star Trek Theme");
  playingSong = true;
  // set currSong to song, and name to songNames
}

