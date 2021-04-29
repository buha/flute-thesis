/*
Note.c
By Andrew Botros, 2001-2004
Modified by Paul Dickens, 2007
Note.c is a frequency to musical note converter.
Refer to Note.h for interface details.
*/
#include "Note.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define A4 440.0 /* A4 set at 440Hz reference point */
#define A4_INDEX 57
#define A4_MIDI_INDEX 69
/* array of notes */
char *notes[120] = {
    "C0",  "C#0", "D0",  "D#0", "E0",  "F0",  "F#0", "G0",  "G#0", "A0",  "A#0",
    "B0",  "C1",  "C#1", "D1",  "D#1", "E1",  "F1",  "F#1", "G1",  "G#1", "A1",
    "A#1", "B1",  "C2",  "C#2", "D2",  "D#2", "E2",  "F2",  "F#2", "G2",  "G#2",
    "A2",  "A#2", "B2",  "C3",  "C#3", "D3",  "D#3", "E3",  "F3",  "F#3", "G3",
    "G#3", "A3",  "A#3", "B3",  "C4",  "C#4", "D4",  "D#4", "E4",  "F4",  "F#4",
    "G4",  "G#4", "A4",  "A#4", "B4",  "C5",  "C#5", "D5",  "D#5", "E5",  "F5",
    "F#5", "G5",  "G#5", "A5",  "A#5", "B5",  "C6",  "C#6", "D6",  "D#6", "E6",
    "F6",  "F#6", "G6",  "G#6", "A6",  "A#6", "B6",  "C7",  "C#7", "D7",  "D#7",
    "E7",  "F7",  "F#7", "G7",  "G#7", "A7",  "A#7", "B7",  "C8",  "C#8", "D8",
    "D#8", "E8",  "F8",  "F#8", "G8",  "G#8", "A8",  "A#8", "B8",  "C9",  "C#9",
    "D9",  "D#9", "E9",  "F9",  "F#9", "G9",  "G#9", "A9",  "A#9", "B9"};
#define MINUS 0
#define PLUS 1
Note note(double input, int round) {
  double frequency;
  int r_index = 0;
  int cent_index = 0;
  int side;
  Note n = (Note)malloc(sizeof(Note *));
  /* a semitone higher than a given frequency is 2^(1/12) times the
  frequency
  a cent higher than a given frequency is 2^(1/1200) times the
  frequency */
  double r = pow(2, 1.0 / 12.0);
  double cent = pow(2, 1.0 / 1200.0);
  /* input frequency must be between A0 and A9 */
  if ((input < 26.73) || (input > 14496.0)) {
    return NULL;
  }
  /* set A4 (440Hz) as reference point */
  frequency = A4;
  /* search for input ratio against A4 to the nearest cent
  in range -49 to +50 cents around closest note */
  if (input >= frequency) {
    while (input >= r * frequency) {
      frequency = r * frequency;
      r_index++;
    }
  } else {
    while (input < frequency) {
      frequency = frequency / r;
      r_index--;
    }
  }
  while (input >= cent * frequency) {
    frequency = cent * frequency;
    cent_index++;
  }
  if ((cent * frequency - input) < (input - frequency))
    cent_index++;
  if (cent_index > 50) {
    r_index++;
    cent_index = 100 - cent_index;
    if (cent_index != 0)
      side = MINUS;
    else
      side = PLUS;
  } else
    side = PLUS;
  if (round) {
    /* round cents to nearest 5 cents */
    switch (cent_index % 5) {
    case 1:
      cent_index = cent_index - 1;
      break;
    case 2:
      cent_index = cent_index - 2;
      break;
    case 3:
      cent_index = cent_index + 2;
      break;
    case 4:
      cent_index = cent_index + 1;
      break;
    default:
      break;
    }
    if (cent_index == 0)
      side = PLUS;
  }
  /* fill string buffer with note information and return */
  n->name = (char *)malloc(BUFSIZ * sizeof(char));
  sprintf(n->name, "%s", notes[A4_INDEX + r_index]);
  if (side == PLUS)
    n->cents = cent_index;
  else
    n->cents = (-1) * cent_index;
  n->midi = A4_MIDI_INDEX + r_index;
  return n;
}
char *noteString(Note n) {
  char *note_string = (char *)malloc(BUFSIZ * sizeof(char));
  if (n->cents >= 0)
    sprintf(note_string, "%s plus %d cents", n->name, abs(n->cents));
  else
    sprintf(note_string, "%s minus %d cents", n->name, abs(n->cents));
  return note_string;
}
