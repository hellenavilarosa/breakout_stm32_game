
#ifndef MUSIC_H_
#define MUSIC_H_

// PSC = CLOCK / (FREQ * PWM_RESOLUTION)
// PSC = (CLOCK / PWM_RESOLUTION) / FREQ
#define stp 0
#define B0  562500 / 31
#define C1  562500 / 33
#define CS1 562500 / 35
#define D1  562500 / 37
#define DS1 562500 / 39
#define E1  562500 / 41
#define F1  562500 / 44
#define FS1 562500 / 46
#define G1  562500 / 49
#define GS1 562500 / 52
#define A1  562500 / 55
#define AS1 562500 / 58
#define B1  562500 / 62
#define C2  562500 / 65
#define CS2 562500 / 69
#define D2  562500 / 73
#define DS2 562500 / 78
#define E2  562500 / 82
#define F2  562500 / 87
#define FS2 562500 / 93
#define G2  562500 / 98
#define GS2 562500 / 104
#define A2  562500 / 110
#define AS2 562500 / 117
#define B2  562500 / 123
#define C3  562500 / 131
#define CS3 562500 / 139
#define D3  562500 / 147
#define DS3 562500 / 156
#define E3  562500 / 165
#define F3  562500 / 175
#define FS3 562500 / 185
#define G3  562500 / 196
#define GS3 562500 / 208
#define A3  562500 / 220
#define AS3 562500 / 233
#define B3  562500 / 247
#define C4  562500 / 262
#define CS4 562500 / 277
#define D4  562500 / 294
#define DS4 562500 / 311
#define E4  562500 / 330
#define F4  562500 / 349
#define FS4 562500 / 370
#define G4  562500 / 392
#define GS4 562500 / 415
#define A4  562500 / 440
#define AS4 562500 / 466
#define B4  562500 / 494
#define C5  562500 / 523
#define CS5 562500 / 554
#define D5  562500 / 587
#define DS5 562500 / 622
#define E5  562500 / 659
#define F5  562500 / 698
#define FS5 562500 / 740
#define G5  562500 / 784
#define GS5 562500 / 831
#define A5  562500 / 880
#define AS5 562500 / 932
#define B5  562500 / 988
#define C6  562500 / 1047
#define CS6 562500 / 1109
#define D6  562500 / 1175
#define DS6 562500 / 1245
#define E6  562500 / 1319
#define F6  562500 / 1397
#define FS6 562500 / 1480
#define G6  562500 / 1568
#define GS6 562500 / 1661
#define A6  562500 / 1760
#define AS6 562500 / 1865
#define B6  562500 / 1976
#define C7  562500 / 2093
#define CS7 562500 / 2217
#define D7  562500 / 2349
#define DS7 562500 / 2489
#define E7  562500 / 2637
#define F7  562500 / 2794
#define FS7 562500 / 2960
#define G7  562500 / 3136
#define GS7 562500 / 3322
#define A7  562500 / 3520
#define AS7 562500 / 3729
#define B7  562500 / 3951
#define C8  562500 / 4186
#define CS8 562500 / 4435
#define D8  562500 / 4699
#define DS8 562500 / 4978

#define DELAY_BASE 1000

#define W DELAY_BASE
#define H DELAY_BASE/2
#define Q DELAY_BASE/4
#define E DELAY_BASE/8
#define S DELAY_BASE/16

const uint32_t music[2][132]= {	{A3, stp, A3, stp, A3, stp,  F3,   stp, C4, stp, A3, stp,  F3,   stp, C4, stp, A3, stp, E4, stp, E4, stp, E4, stp,  F4,   stp, C4, stp, GS3, stp,  F3,   stp, C4, stp, A3, stp, A4, stp,  A3,   stp, A3, stp, A4, stp, GS4,   stp, G4, stp, FS4, stp, E4, stp, F4,     stp, AS3, stp, DS4, stp,  D4,   stp, CS4, stp, C4, stp, B3, stp, C4,     stp, F3, stp, GS3, stp,  F3,   stp, A3, stp, C4, stp,  A3,   stp, C4, stp, E4, stp, A4, stp,  A3,   stp, A3, stp, A4, stp, GS4,   stp, G4, stp, FS4, stp, E4, stp, F4,     stp, AS3, stp, DS4, stp,  D4,   stp, CS4, stp, C4, stp, B3, stp, C4,     stp, F3, stp, GS3, stp,  F3,   stp, C4, stp, A3, stp,  F3,   stp, C4, stp, A3, stp},
								{ Q, 1+Q,  Q, 1+Q,  Q, 1+Q, E+S, 1+E+S,  S, 1+S,  Q, 1+Q, E+S, 1+E+S,  S, 1+S,  H, 1+H,  Q, 1+Q,  Q, 1+Q,  Q, 1+Q, E+S, 1+E+S,  S, 1+S,   Q, 1+Q, E+S, 1+E+S,  S, 1+S,  H, 1+H,  Q, 1+Q, E+S, 1+E+S,  S, 1+S,  Q, 1+Q, E+S, 1+E+S,  S, 1+S,   S, 1+S,  S, 1+S,  E, 2*(E+1),   E, 1+E,   Q, 1+Q, E+S, 1+E+S,   S, 1+S,  S, 1+S,  S, 1+S,  E, 2*(1+E),  E, 1+E,   Q, 1+Q, E+S, 1+E+S,  S, 1+S,  Q, 1+Q, E+S, 1+E+S,  S, 1+S,  H, 1+H,  Q, 1+Q, E+S, 1+E+S,  S, 1+S,  Q, 1+Q, E+S, 1+E+S,  S, 1+S,   S, 1+S,  S, 1+S,  E, 2*(1+E),   E, 1+E,   Q, 1+Q, E+S, 1+E+S,   S, 1+S,  S, 1+S,  S, 1+S,  E, 2*(1+E),  E, 1+E,   Q, 1+Q, E+S, 1+E+S,  S, 1+S,  Q, 1+Q, E+S, 1+E+S,  S, 1+S,  H, 3*H}};

#endif /* MUSIC_H_ */
