/*
 * Copyright CogniPilot Foundation 2023
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef CEREBRI_ACTUATE_SOUND_ACTUATOR_SOUND_H
#define CEREBRI_ACTUATE_SOUND_ACTUATOR_SOUND_H

#define REST 1
#define thirtysecond 19
#define sixteenth 38
#define eighth 75
#define quarter 150
#define half 300
#define whole 600

#define C1 33
#define Db1 35
#define D1 37
#define Eb1 39
#define E1 41
#define F1 44
#define Gb1 46
#define G1 49
#define Ab1 52
#define A1 55
#define Bb1 58
#define B1 62

#define C2 65
#define Db2 69
#define D2 73
#define Eb2 78
#define E2 82
#define F2 87
#define Gb2 92
#define G2 98
#define Ab2 104
#define A2 110
#define Bb2 117
#define B2 123

#define C3 131
#define Db3 139
#define D3 147
#define Eb3 156
#define E3 165
#define F3 175
#define Gb3 185
#define G3 196
#define Ab3 208
#define A3 220
#define Bb3 233
#define B3 247

#define C4 262
#define Db4 277
#define D4 294
#define Eb4 311
#define E4 330
#define F4 349
#define Gb4 370
#define G4 392
#define Ab4 415
#define A4 440
#define Bb4 466
#define B4 494

#define C5 523
#define Db5 554
#define D5 587
#define Eb5 622
#define E5 659
#define F5 698
#define Gb5 740
#define G5 784
#define Ab5 831
#define A5 880
#define Bb5 932
#define B5 988

#define C6 1046
#define Db6 1109
#define D6 1175
#define Eb6 1245
#define E6 1319
#define F6 1397
#define Gb6 1480
#define G6 1568
#define Ab6 1661
#define A6 1760
#define Bb6 1865
#define B6 1976

struct tones_t {
    int note;
    int duration;
};

struct tones_t imperial_start_tone[] = {
    { .note = G5, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = G5, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = G5, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = Eb5, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = Bb5, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = G5, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = Eb5, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = Bb5, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = G5, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = D6, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = D6, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = D6, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = Eb6, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = Bb5, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = G5, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = Eb5, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = Bb5, .duration = quarter },
    { .note = REST, .duration = sixteenth },
    { .note = G5, .duration = quarter },
    { .note = REST, .duration = whole },
    { .note = REST, .duration = whole },
};

struct tones_t brave_start_tone[] = {
    { .note = G5, .duration = quarter },
    { .note = REST, .duration = eighth },
    { .note = G5, .duration = quarter },
    { .note = REST, .duration = eighth },
    { .note = Gb5, .duration = quarter },
    { .note = REST, .duration = eighth },
    { .note = A5, .duration = quarter },
    { .note = REST, .duration = eighth },
    { .note = G5, .duration = quarter },
    { .note = REST, .duration = eighth },
    { .note = E5, .duration = quarter },
    { .note = REST, .duration = whole },
    { .note = G5, .duration = quarter },
    { .note = REST, .duration = eighth },
    { .note = G5, .duration = quarter },
    { .note = REST, .duration = eighth },
    { .note = Gb5, .duration = quarter },
    { .note = REST, .duration = eighth },
    { .note = A5, .duration = quarter },
    { .note = REST, .duration = eighth },
    { .note = G5, .duration = quarter },
    { .note = REST, .duration = eighth },
    { .note = E5, .duration = quarter },
    { .note = REST, .duration = eighth },
    { .note = D5, .duration = quarter },
    { .note = REST, .duration = whole },
    { .note = REST, .duration = whole },
};

struct tones_t airy_start_tone[] = {
    { .note = B4, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = B4, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = B4, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
    { .note = B4, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = Gb5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
    { .note = Gb5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = Gb5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = Gb5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = A5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
    { .note = A4, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = Gb5, .duration = quarter },
    { .note = REST, .duration = whole },
    { .note = REST, .duration = whole },
};

/* Manual Mode Morse code 1*/
struct tones_t manual_mode_tone[] = {
    { .note = B5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
};

/* Auto Mode Morse code 2*/
struct tones_t auto_mode_tone[] = {
    { .note = B5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = B5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
};

/* CMD_VEL Mode Morse code 3*/
struct tones_t cmd_vel_mode_tone[] = {
    { .note = B5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = B5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = B5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
};

/* CAL Mode Morse code 4*/
struct tones_t cal_mode_tone[] = {
    { .note = B5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = B5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = B5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = B5, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = quarter },
    { .note = REST, .duration = thirtysecond },
};

/* Armed Morse code A all high*/
struct tones_t armed_tone[] = {
    { .note = B5, .duration = half },
    { .note = REST, .duration = thirtysecond },
    { .note = D5, .duration = whole },
    { .note = REST, .duration = thirtysecond },
};

/* Disarmed Morse code A high low*/
struct tones_t disarmed_tone[] = {
    { .note = B5, .duration = half },
    { .note = REST, .duration = thirtysecond },
    { .note = D4, .duration = whole },
    { .note = REST, .duration = thirtysecond },
};

/* Safety OFF Morse code S low mid high */
struct tones_t safety_off_tone[] = {
    { .note = B3, .duration = whole },
    { .note = REST, .duration = thirtysecond },
    { .note = B4, .duration = whole },
    { .note = REST, .duration = thirtysecond },
    { .note = B5, .duration = whole },
    { .note = REST, .duration = thirtysecond },
};

/* Safety ON Morse code S HIGH LOW MID */
struct tones_t safety_on_tone[] = {
    { .note = B5, .duration = whole },
    { .note = REST, .duration = thirtysecond },
    { .note = B4, .duration = whole },
    { .note = REST, .duration = thirtysecond },
    { .note = B3, .duration = whole },
    { .note = REST, .duration = thirtysecond },
};

/* Fuel Tone Morse code C high low high low*/
struct tones_t fuel_tone[] = {
    { .note = C5, .duration = half },
    { .note = REST, .duration = thirtysecond },
    { .note = B6, .duration = whole },
    { .note = REST, .duration = thirtysecond },
    { .note = C5, .duration = half },
    { .note = REST, .duration = thirtysecond },
    { .note = B6, .duration = whole },
    { .note = REST, .duration = thirtysecond },
};

/* No Input Received*/
struct tones_t input_loss_tone[] = {
    { .note = REST, .duration = thirtysecond },
    { .note = C6, .duration = eighth },
    { .note = REST, .duration = thirtysecond },
};

/* Reject Tone */
struct tones_t reject_tone[] = {
    { .note = 110, .duration = half },
    { .note = REST, .duration = thirtysecond },
    { .note = 1325, .duration = 12 },
    { .note = 1480, .duration = 12 },
    { .note = 1634, .duration = 12 },
    { .note = 1781, .duration = 12 },
    { .note = 1921, .duration = 12 },
    { .note = 2050, .duration = 12 },
    { .note = 2166, .duration = 12 },
    { .note = 2268, .duration = 12 },
    { .note = 2352, .duration = 12 },
    { .note = 2419, .duration = 12 },
    { .note = 2466, .duration = 12 },
    { .note = 2493, .duration = 12 },
    { .note = 2499, .duration = 12 },
    { .note = 2485, .duration = 12 },
    { .note = 2449, .duration = 12 },
    { .note = 2394, .duration = 12 },
    { .note = 2321, .duration = 12 },
    { .note = 2229, .duration = 12 },
    { .note = 2122, .duration = 12 },
    { .note = 2000, .duration = 12 },
    { .note = 1866, .duration = 12 },
    { .note = 1723, .duration = 12 },
    { .note = 1573, .duration = 12 },
    { .note = 1418, .duration = 12 },
    { .note = 1262, .duration = 12 },
    { .note = 1107, .duration = 12 },
    { .note = 955, .duration = 12 },
    { .note = 811, .duration = 12 },
    { .note = 675, .duration = 12 },
    { .note = 551, .duration = 12 },
    { .note = 440, .duration = 12 },
    { .note = 345, .duration = 12 },
    { .note = 268, .duration = 12 },
    { .note = 209, .duration = 12 },
    { .note = 170, .duration = 12 },
    { .note = 151, .duration = 12 },
    { .note = 153, .duration = 12 },
    { .note = 176, .duration = 12 },
    { .note = 219, .duration = 12 },
    { .note = 282, .duration = 12 },
    { .note = 363, .duration = 12 },
    { .note = 461, .duration = 12 },
    { .note = 575, .duration = 12 },
    { .note = 701, .duration = 12 },
    { .note = 839, .duration = 12 },
    { .note = 985, .duration = 12 },
    { .note = 1138, .duration = 12 },
    { .note = 1293, .duration = 12 },
    { .note = 1449, .duration = 12 },
    { .note = 1603, .duration = 12 },
    { .note = 1752, .duration = 12 },
    { .note = 1894, .duration = 12 },
    { .note = 2025, .duration = 12 },
    { .note = 2144, .duration = 12 },
    { .note = 2249, .duration = 12 },
    { .note = 2337, .duration = 12 },
    { .note = 2407, .duration = 12 },
    { .note = 2458, .duration = 12 },
    { .note = 2489, .duration = 12 },
    { .note = 2500, .duration = 12 },
    { .note = REST, .duration = whole },
};

#endif // CEREBRI_ACTUATE_SOUND_ACTUATOR_SOUND_H
/* vi: ts=4 sw=4 et */
