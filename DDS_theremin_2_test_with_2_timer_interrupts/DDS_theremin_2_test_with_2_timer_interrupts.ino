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
// 656Khz/44.1Khz = 14.88 // use for servo control 13120 / 65600 = .02 ( update servos every 20 milliseconds )

// 84Mhz/44.1Khz = 1904 instructions per tick

// include the library code:
#include <LiquidCrystal.h>
#include <Servo.h>
#include <songdata.h>

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

// we are using a servo as a potentiometer to control the volume. It reports values from  90 - 640, so there are 550 possible volume increments.
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Song Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct songData_midi {
  int note;
  int duration;
  int volume;
};

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

char* songNames[] = { "Star Trek Theme", "SWOTR"};
// array of song Data
// actual song data, should prob put this in a header file
/*songData_midi STT[] = { {NOTE_AS4, 150, 10},     // e.g. 150 *.020sec ~= 3 seconds
                        {NOTE_A7,  300, 10},     // volume = 10
                        {NOTE_AS4, 150, 10},
                        {NOTE_A7,  300, 5},     
                        {NOTE_AS4, 150, 5},     
                        {NOTE_A7,  300, 5},
                        {NOTE_AS4, 150, 10},   
                        {NOTE_A7,  300, 10}, 
                        {NOTE_AS4, 150, 10}};*/
int tempo = 2;
songData_midi STT[] = {{NOTE_FS3,   138/tempo,     7+3},
                       {NOTE_FS4,   135/tempo,     6+3},
                       {NOTE_F4,    51/tempo,      7+3},
                       {NOTE_CS4,   29/tempo,      5+3},
                       {NOTE_DS4,   27/tempo,      7+3},
                       {NOTE_F4,    62/tempo,      7+3},
                       {NOTE_FS4,   63/tempo,      5+3},
                       {NOTE_FS3,   122/tempo,     7+3},
                       {NOTE_DS4,   123/tempo,     6+3},
                       {NOTE_CS4,   166/tempo,     6+3},
                       {NOTE_CS4,   78/tempo,      0},
                       {NOTE_FS3,   138/tempo,     7+3}};
                       
char *song = "MissionImp:d=16,o=6,b=95:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,a#,g,2d,32p,a#,g,2c#,32p,a#,g,2c,a#5,8c,2p,32p,a#5,g5,2f#,32p,a#5,g5,2f,32p,a#5,g5,2e,d#,8d";
                      
songData_midi *songs[] = {STT};
songData_midi *currSong; // = songs[0]; // currSong points to the song that is playing
int iCurrSong = 0; // index for 2 arrays, songs[] and songNames[]
int currSongLength = sizeof(STT) / sizeof(STT[0]); // number of notes to play for a particular song

void PlaySong( songData_midi* song ); 
songData_midi* newSong;

void setup()
{
  Serial.begin(9600);
  
  pinMode(btnNextPin, INPUT);
  pinMode(btnSelectPin, INPUT);
   
  attachInterrupt(btnNextPin, BtnNext, CHANGE);
  attachInterrupt(btnSelectPin, BtnSelect, CHANGE);
   
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  // Print a message to the LCD.
  displayLCD("Press Sel 4Demo", songNames[iCurrSong] );

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
      fAngle = ((float)(currSong[iNote].note - minNote) / (float)(maxNote - minNote)) * 90.0 + fMinAngle;
      freqServo.write(fAngle);
      
         
      // test if we need to increment to the next note
      if( noteDurationCtr >= currSong[iNote].duration )
        {
         noteDurationCtr = 0;
         // increment note and test if we have finished the song
         if( ++iNote >= currSongLength )
           {
            // song is over
            playingSong = false;
            freqServo.detach();
            volServo.detach();
            displayLCD("Press sel 4Demo", songNames[iCurrSong] );
           }   
         else
          { 
           // set volume for next note
           volume = currSong[iNote].volume;
           vAngle = vMinAngle + ((float)(vMaxAngle - vMinAngle) / (float) MAX_VOL) * volume;
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
    Serial.println(unMidiNote);
    Serial.println(fFrequency);
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
void PlaySong(struct songData_midi* song)
{
  displayLCD("Playing", songNames[iCurrSong]);
  currSong = song;
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
     int numSongs = sizeof(songNames) / sizeof(songNames[0]);
     iCurrSong = ((iCurrSong+1) % numSongs);
     displayLCD("Press sel 4Demo", songNames[iCurrSong] );  
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

/*
NOTE_Cm1   0  8.18
NOTE_CSm1  1  8.66
NOTE_Dm1   2  9.18
NOTE_DSm1  3  9.72
NOTE_Em1   4  10.30
NOTE_Fm1   5  10.91
NOTE_FSm1  6  11.56
NOTE_Gm1   7  12.25
NOTE_GSm1  8  12.98
NOTE_Am1   9  13.75
NOTE_ASm1 10  14.57
NOTE_Bm1  11  15.43
NOTE_C0   12  16.35
NOTE_CS0  13  17.32
NOTE_D0   14  18.35
NOTE_DS0  15  19.45
NOTE_E0   16  20.60
NOTE_F0   17  21.83
NOTE_FS0  18  23.12
NOTE_G0   19  24.50
NOTE_GS0  20  25.96
NOTE_A0   21  27.50
NOTE_AS0  22  29.14
NOTE_B0   23  30.87
NOTE_C1   24  32.70
NOTE_CS1  25  34.65
NOTE_D1   26  36.71
NOTE_DS1  27  38.89
NOTE_E1   28  41.20
NOTE_F1   29  43.65
NOTE_FS1  30  46.25
NOTE_G1   31  49.00
NOTE_GS1  32  51.91
NOTE_A1   33  55.00
NOTE_AS1  34  58.27
NOTE_B1   35  61.74
NOTE_C2   36  65.41
NOTE_CS2  37  69.30
NOTE_D2   38  73.42
NOTE_DS2  39  77.78
NOTE_E2   40  82.41
NOTE_F2   41  87.31
NOTE_FS2  42  92.50
NOTE_G2   43  98.00
NOTE_GS2  44  103.83
NOTE_A2   45  110.00
NOTE_AS2  46  116.54
NOTE_B2   47  123.47
NOTE_C3   48  130.81
NOTE_CS3  49  138.59
NOTE_D3   50  146.83
NOTE_DS3  51  155.56
NOTE_E3   52  164.81
NOTE_F3   53  174.61
NOTE_FS3  54  185.00
NOTE_G3   55  196.00
NOTE_GS3  56  207.65
NOTE_A3   57  220.00
NOTE_AS3  58  233.08
NOTE_B3   59  246.94
NOTE_C4   60  261.63
NOTE_CS4  61  277.18
NOTE_D4   62  293.66
NOTE_DS4  63  311.13
NOTE_E4   64  329.63
NOTE_F4   65  349.23
NOTE_FS4  66  369.99
NOTE_G4   67  392.00
NOTE_GS4  68  415.30
NOTE_A4   69  440.00
NOTE_AS4  70  466.16
NOTE_B4   71  493.88
NOTE_C5   72  523.25
NOTE_CS5  73  554.37
NOTE_D5   74  587.33
NOTE_DS5  75  622.25
NOTE_E5   76  659.26
NOTE_F5   77  698.46
NOTE_FS5  78  739.99
NOTE_G5   79  783.99
NOTE_GS5  80  830.61
NOTE_A5   81  880.00
NOTE_AS5  82  932.33
NOTE_B5   83  987.77
NOTE_C6   84  1046.50
NOTE_CS6  85  1108.73
NOTE_D6   86  1174.66
NOTE_DS6  87  1244.51
NOTE_E6   88  1318.51
NOTE_F6   89  1396.91
NOTE_FS6  90  1479.98
NOTE_G6  91   1567.98
NOTE_GS6  92  1661.22
NOTE_A6   93  1760.00
NOTE_AS6  94  1864.66
NOTE_B6   95  1975.53
NOTE_C7   96  2093.00
NOTE_CS7  97  2217.46
NOTE_D7   98  2349.32
NOTE_DS7  99  2489.02
NOTE_E7   100 2637.02
NOTE_F7   101  2793.83
NOTE_FS7  102  2959.96
NOTE_G7   103  3135.96
NOTE_GS7  104  3322.44
NOTE_A7   105  3520.00
NOTE_AS7  106  3729.31
NOTE_B7   107  3951.07
NOTE_C8   108  4186.01
NOTE_CS8  109  4434.92
NOTE_D8   110  4698.64
NOTE_DS8  111  4978.03
NOTE_E8   112  5174.04
NOTE_F8   113  5587.65
NOTE_FS8  114  5919.91
NOTE_G8   115  6271.93
NOTE_GS8  116  6644.88
NOTE_A8   117  7040.00
NOTE_AS8  118  7458.62
NOTE_B8   119  7902.13
NOTE_C9   120  8372.02
NOTE_CS9  121  8869.84
NOTE_D9   122  9397.27
NOTE_DS9  123  9956.06
NOTE_E9   124  10548.08
NOTE_F9   125  11175.30
NOTE_FS9  126  11839.82
NOTE_G9   127  12543.85
*/








