#include <string>

#define NOTE_Cm1   0 
#define NOTE_CSm1  1
#define NOTE_Dm1   2
#define NOTE_DSm1  3
#define NOTE_Em1   4
#define NOTE_Fm1   5
#define NOTE_FSm1  6
#define NOTE_Gm1   7
#define NOTE_GSm1  8
#define NOTE_Am1   9
#define NOTE_ASm1 10
#define NOTE_Bm1 11
#define NOTE_C0  12
#define NOTE_CS0 13
#define NOTE_D0  14
#define NOTE_DS0 15
#define NOTE_E0  16
#define NOTE_F0  17
#define NOTE_FS0 18
#define NOTE_G0  19
#define NOTE_GS0 20
#define NOTE_A0  21
#define NOTE_AS0 22
#define NOTE_B0  23
#define NOTE_C1  24
#define NOTE_CS1 25
#define NOTE_D1  26
#define NOTE_DS1 27
#define NOTE_E1  28
#define NOTE_F1  29
#define NOTE_FS1 30
#define NOTE_G1  31
#define NOTE_GS1 32
#define NOTE_A1  33
#define NOTE_AS1 34
#define NOTE_B1  35
#define NOTE_C2  36
#define NOTE_CS2 37
#define NOTE_D2  38
#define NOTE_DS2 39
#define NOTE_E2  40
#define NOTE_F2  41
#define NOTE_FS2 42
#define NOTE_G2  43
#define NOTE_GS2 44
#define NOTE_A2  45
#define NOTE_AS2 46
#define NOTE_B2  47
#define NOTE_C3  48
#define NOTE_CS3 49
#define NOTE_D3  50
#define NOTE_DS3 51
#define NOTE_E3  52
#define NOTE_F3  53
#define NOTE_FS3 54
#define NOTE_G3  55
#define NOTE_GS3 56
#define NOTE_A3  57
#define NOTE_AS3 58
#define NOTE_B3  59
#define NOTE_C4  60
#define NOTE_CS4 61
#define NOTE_D4  62
#define NOTE_DS4 63
#define NOTE_E4  64
#define NOTE_F4  65
#define NOTE_FS4 66
#define NOTE_G4  67
#define NOTE_GS4 68
#define NOTE_A4  69
#define NOTE_AS4 70
#define NOTE_B4  71
#define NOTE_C5  72
#define NOTE_CS5 73
#define NOTE_D5  74
#define NOTE_DS5 75
#define NOTE_E5  76
#define NOTE_F5  77
#define NOTE_FS5 78
#define NOTE_G5  79
#define NOTE_GS5 80
#define NOTE_A5  81
#define NOTE_AS5 82
#define NOTE_B5  83
#define NOTE_C6  84
#define NOTE_CS6 85
#define NOTE_D6  86
#define NOTE_DS6 87
#define NOTE_E6  88
#define NOTE_F6  89
#define NOTE_FS6 90
#define NOTE_G6  91
#define NOTE_GS6 92
#define NOTE_A6  93
#define NOTE_AS6 94
#define NOTE_B6  95
#define NOTE_C7  96
#define NOTE_CS7 97
#define NOTE_D7  98
#define NOTE_DS7 99
#define NOTE_E7  100
#define NOTE_F7  101
#define NOTE_FS7 102
#define NOTE_G7  103
#define NOTE_GS7 104
#define NOTE_A7  105
#define NOTE_AS7 106
#define NOTE_B7  107
#define NOTE_C8  108
#define NOTE_CS8 109
#define NOTE_D8  110
#define NOTE_DS8 111
#define NOTE_E8  112
#define NOTE_F8  113
#define NOTE_FS8 114
#define NOTE_G8  115
#define NOTE_GS8 116
#define NOTE_A8  117
#define NOTE_AS8 118
#define NOTE_B8  119
#define NOTE_C9  120
#define NOTE_CS9 121
#define NOTE_D9  122
#define NOTE_DS9 123
#define NOTE_E9  124
#define NOTE_F9  125
#define NOTE_FS9 126
#define NOTE_G9  127

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Song Data
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MAX_SONG_SIZE 200

struct songData {
  int note;
  int duration;
  int volume;
}; 

struct Song {
  int SongLength;
  char *name;
  songData *data;
  
  Song( char* songName, songData *newSongData, int songLength);
  Song();
};

// somewhere over the rainbow
songData SWOTR[] = {{NOTE_GS3,    67,      8},
                    {NOTE_GS4,    74,      8},
                    {NOTE_G4,     40,      9},
                    {NOTE_DS4,    16,      8},
                    {NOTE_F4,     20,      9},
                    {NOTE_G4,     38,      8},
                    {NOTE_GS4,    47,      7},
                    {NOTE_GS3,    37,      8},
                    {NOTE_F4,     75,      8},
                    {NOTE_DS4,   130,      6},
                    {NOTE_F3,     77,      8},
                    {NOTE_CS4,    73,      6},
                    {NOTE_C4,     41,      8},
                    {NOTE_GS3,    16,      7},
                    {NOTE_AS3,    19,      9},
                    {NOTE_C4,     37,      9},
                    {NOTE_CS4,    39,      6},
                    {NOTE_AS3,    42,      8},
                    {NOTE_G3,      9,      9},
                    {NOTE_GS3,    13,      7},
                    {NOTE_AS3,    23,      8},
                    {NOTE_C4,     27,      7},
                    {NOTE_GS3,   114,      7},
                    {NOTE_GS3,    72,      8},
                    {NOTE_GS4,    77,      6},
                    {NOTE_G4,     41,      7},
                    {NOTE_DS4,    19,      8},
                    {NOTE_F4,     20,      9},
                    {NOTE_G4,     42,      9},
                    {NOTE_GS4,    53,      7},
                    {NOTE_GS3,    43,      9},
                    {NOTE_F4,     79,      8},
                    {NOTE_DS4,   128,      8},
                    {NOTE_F3,     38,      9},
                    {NOTE_CS4,    78,      7},
                    {NOTE_C4,     47,      8},
                    {NOTE_GS3,    16,      8},
                    {NOTE_AS3,    18,      8},
                    {NOTE_C4,     58,      7},
                    {NOTE_CS4,    40,      7},
                    {NOTE_AS3,    43,      9},
                    {NOTE_G3,     13,      9},
                    {NOTE_GS3,    13,      8},
                    {NOTE_AS3,    51,      8},
                    {NOTE_C4,     63,      8},
                    {NOTE_GS3,    93,      5},
                    {NOTE_DS4,    97,      8},
                    {NOTE_C4,     32,      6},
                    {NOTE_DS4,    23,      5},
                    {NOTE_C4,     19,      8},
                    {NOTE_DS4,    20,      7},
                    {NOTE_C4,     17,      8},
                    {NOTE_DS4,    19,      8},
                    {NOTE_C4,     20,      8},
                    {NOTE_DS4,    20,      9},
                    {NOTE_CS4,    18,      7},
                    {NOTE_DS4,    17,      8},
                    {NOTE_CS4,    16,      7},
                    {NOTE_DS4,    18,      8},
                    {NOTE_CS4,    16,      7},
                    {NOTE_DS4,    19,      8},
                    {NOTE_CS4,    21,      8},
                    {NOTE_DS4,    26,      8},
                    {NOTE_F4,     81,      9},
                    {NOTE_F4,    148,      8},
                    {NOTE_DS4,    19,      7},
                    {NOTE_C4,     16,      7},
                    {NOTE_DS4,    17,      7},
                    {NOTE_C4,     15,      8},
                    {NOTE_DS4,    17,      7},
                    {NOTE_C4,     15,      9},
                    {NOTE_DS4,    17,      8},
                    {NOTE_C4,     19,      8},
                    {NOTE_D4,     20,      8},
                    {NOTE_D4,     17,      9},
                    {NOTE_F4,     16,      9},
                    {NOTE_D4,     15,      9},
                    {NOTE_F4,     15,      9},
                    {NOTE_D4,     16,      8},
                    {NOTE_F4,     18,      9},
                    {NOTE_D4,     21,      9},
                    {NOTE_F4,     23,      9},
                    {NOTE_G4,     82,      10},
                    {NOTE_G4,     94,      8},
                    {NOTE_AS4,   108,      9},
                    {NOTE_F4,    161,      6},
                    {NOTE_GS3,    75,      8},
                    {NOTE_GS4,    74,      8},
                    {NOTE_G4,     38,      9},
                    {NOTE_DS4,    20,      8},
                    {NOTE_F4,     19,      9},
                    {NOTE_G4,     41,      8},
                    {NOTE_GS4,    53,      8},
                    {NOTE_GS3,    44,      9},
                    {NOTE_F4,     74,      8},
                    {NOTE_DS4,   119,      7},
                    {NOTE_F3,     36,      8},
                    {NOTE_CS4,    76,      9},
                    {NOTE_C4,     43,      8},
                    {NOTE_GS3,    19,      8},
                    {NOTE_AS3,    21,      9},
                    {NOTE_C4,     49,      8},
                    {NOTE_CS4,    61,      7},
                    {NOTE_AS3,    56,      8},
                    {NOTE_G3,     53,      7},
                    {NOTE_G3,     31,      8},
                    {NOTE_AS3,    66,      8},
                    {NOTE_C4,     98,      8},
                    {NOTE_GS3,   139,      8},
                    {NOTE_DS4,    22,      8},
                    {NOTE_C4,     18,      7},
                    {NOTE_DS4,    20,      7},
                    {NOTE_C4,     17,      7},
                    {NOTE_DS4,    20,      8},
                    {NOTE_C4,     19,      8},
                    {NOTE_DS4,    23,      7},
                    {NOTE_C4,     30,      6},
                    {NOTE_DS4,    24,      7},
                    {NOTE_CS4,    23,      7},
                    {NOTE_DS4,    22,      6},
                    {NOTE_CS4,    26,      7},
                    {NOTE_DS4,    38,      7},
                    {NOTE_CS4,    51,      7},
                    {NOTE_DS4,    80,      8},
                    {NOTE_F4,    101,      9},
                    {NOTE_G4,    130,      9},
                    {NOTE_GS4,   261,      8}};
       
    
char *RTTLSongs[] = {"Indiana Jones:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6",
                     "TheSimpsons:d=4,o=5,b=160:c.6,e6,f#6,8a6,g.6,e6,c6,8a,8f#,8f#,8f#,2g,8p,8p,8f#,8f#,8f#,8g,a#.,8c6,8c6,8c6,c6",
                     "Entertainer:d=4,o=5,b=140:8d,8d#,8e,c6,8e,c6,8e,2c.6,8c6,8d6,8d#6,8e6,8c6,8d6,e6,8b,d6,2c6,p,8d,8d#,8e,c6,8e,c6,8e,2c.6,8p,8a,8g,8f#,8a,8c6,e6,8d6,8c6,8a,2d6",
                     "Muppets:d=4,o=5,b=250:c6,c6,a,b,8a,b,g,p,c6,c6,a,8b,8a,8p,g.,p,e,e,g,f,8e,f,8c6,8c,8d,e,8e,8e,8p,8e,g,2p,c6,c6,a,b,8a,b,g,p,c6,c6,a,8b,a,g.,p,e,e,g,f,8e,f,8c6,8c,8d,e,8e,d,8d,c",
                     "Bond:d=4,o=5,b=80:32p,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d#6,16d#6,16c#6,32d#6,32d#6,16d#6,8d#6,16c#6,16c#6,16c#6,16c#6,32e6,32e6,16e6,8e6,16d#6,16d6,16c#6,16c#7,c.7,16g#6,16f#6,g#.6",
                     "StarWars:d=4,o=5,b=45:32p,32f#,32f#,32f#,8b.,8f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32e6,8c#.6,32f#,32f#,32f#,8b.,8f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32c#6,8b.6,16f#.6,32e6,32d#6,32e6,8c#6",
                     "GoodBad:d=4,o=5,b=56:32p,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,d#,32a#,32d#6,32a#,32d#6,8a#.,16f#.,16g#.,c#6,32a#,32d#6,32a#,32d#6,8a#.,16f#.,32f.,32d#.,c#,32a#,32d#6,32a#,32d#6,8a#.,16g#.,d#",
                     "A-Team:d=8,o=5,b=120:4d#6,a#,2d#6,16p,g#,4a#,4d#.,p,16g,16a#,d#6,a#,f6,2d#6,16p,c#.6,16c6,16a#,g#.,2a#,d#6,a#,2d#6,16p,g#,4a#,4d#.,p,16g,16a#,d#6,a#,f6,2d#6,16p,c#.6,16c6,16a#,g#.,2a#",
                     "Flinstones:d=4,o=5,b=40:32p,16f6,16a#,16a#6,32g6,16f6,16a#.,16f6,32d#6,32d6,32d6,32d#6,32f6,16a#,16c6,d6,16f6,16a#.,16a#6,32g6,16f6,16a#.,32f6,32f6,32d#6,32d6,32d6,32d#6,32f6,16a#,16c6,a#,16a6,16d.6,16a#6,32a6,32a6,32g6,32f#6,32a6,8g6,16g6,16c.6,32a6,32a6,32g6,32g6,32f6,32e6,32g6,8f6,16f6,16a#.,16a#6,32g6,16f6,16a#.,16f6,32d#6,32d6,32d6,32d#6,32f6,16a#,16c.6,32d6,32d#6,32f6,16a#,16c.6,32d6,32d#6,32f6,16a#6,16c7,8a#.6",
                     "Jeopardy:d=4,o=6,b=125:c,f,c,f5,c,f,2c,c,f,c,f,a.,8g,8f,8e,8d,8c#,c,f,c,f5,c,f,2c,f.,8d,c,a#5,a5,g5,f5,p,d#,g#,d#,g#5,d#,g#,2d#,d#,g#,d#,g#,c.7,8a#,8g#,8g,8f,8e,d#,g#,d#,g#5,d#,g#,2d#,g#.,8f,d#,c#,c,p,a#5,p,g#.5,d#,g#",
                     "Gadget:d=16,o=5,b=40:32d#,32f,32f#,32g#,a#,f#,a,f,g#,f#,32d#,32f,32f#,32g#,a#,d#6,4d6,32d#,32f,32f#,32g#,a#,f#,a,f,g#,f#,8d#",
                     "MissionImp:d=16,o=6,b=75:32d,32d#,32d,32d#,32d,32d#,32d,32d#,32d,32d,32d#,32e,32f,32f#,32g,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,g,8p,g,8p,a#,p,c7,p,g,8p,g,8p,f,p,f#,p,a#,g,2d,32p,a#,g,2c#,32p,a#,g,2c,a#5,8c,2p,32p,a#5,g5,2f#,32p,a#5,g5,2f,32p,a#5,g5,2e,d#,8d",
                     "smb:d=4,o=5,b=95:16e6,16e6,32p,8e6,16c6,8e6,8g6,8p,8g,8p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,16p,8c6,16p,8g,16p,8e,16p,8a,8b,16a#,8a,16g.,16e6,16g6,8a6,16f6,8g6,8e6,16c6,16d6,8b,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16c7,16p,16c7,16c7,p,16g6,16f#6,16f6,16d#6,16p,16e6,16p,16g#,16a,16c6,16p,16a,16c6,16d6,8p,16d#6,8p,16d6,8p,16c6",
                     "Airwolf:d=4,o=5,b=100:e,16a,16b,16d6,e6,16g6,16f#6,16d6,e6,16g6,16f#6,16d6,e6,8d6,16f#6,b,a,8g,16a,8f#,16d,g,16c6,16d6,16f6,g,16c6,16b6,16f6,g6,16c6,16b6,16f6,g6,8f6,16a,d6,c6,8b,16d6,8a,16f,g6,16c6,16d6,16f6,g6,16c6,16b6,16f6",
                     "ThemeV:d=4,o=5,b=100:16c#7,16c#7,16c#7,16c#7,16c#7,16c#7,16c#7,16c#7,8c#6,16p,16f#6,8g#6,16p,16p,16c#7,8c#7,8c#6,16a#6,16a#6,16a#6,16g#6,16p,16e#7,8p,8p,16c#7,8c#7,8c#6,8p,16f#6,8a#6,8g#6,8p",
                     "Macgyver:d=4,o=4,b=160:8c5,8c5,8c5,8c5,2b,8f#,a,2g,8c5,c5,b,8a,8b,8a,g,e5,5,a,b.,8p,8c5,8b,8a,c5,8b,8a,d5,8c5,8b,d5,8c5,8b,e5,8d5,8e5,f#5,b,1g5,8p,8g5,8e5,8c5,8f#5,8d5,8b,8e5,8c5,8a,8d5,8b,8g,c5,b,8c5,8b,8a,8g,a#,a,8g.",  
                     "StarWars:d=4,o=5,b=112:8d.,16p,8d.,16p,8d.,16p,8a#4,16p,16f,8d.,16p,8a#4,16p,16f,d.,8p,8a.,16p,8a.,16p,8a.,16p,8a#,16p,16f,8c#.,16p,8a#4,16p,16f,d.,8p,8d.6,16p,8d,16p,16d,8d6,8p,8c#6,16p,16c6,16b,16a#,8b,8p,16d#,16p,8g#,8p,8g,16p,16f#,16f,16e,8f,8p,16a#4,16p,8c#,8p,8a#4,16p,16c#,8f.,16p,8d,16p,16f,a.,8p,8d.6,16p,8d,16p,16d,8d6,8p,8c#6,16p,16c6,16b,16a#,8b,8p,16d#,16p,8g#,8p,8g,16p,16f#,16f,16e,8f,8p,16a#4,16p,8c#"};
                

