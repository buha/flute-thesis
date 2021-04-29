/*
Note.h
By Andrew Botros, 2001-2004
Modified by Paul Dickens, 2007
Note.c is a frequency to musical note converter.
*/
#ifndef NOTE_H_PROTECTOR
#define NOTE_H_PROTECTOR
/* Note: { note struct including name, cents, midi number ) */
typedef struct note_str {
  char *name;
  int cents;
  int midi;
} * Note;
Note note(double input, int round);
/*
Converts the given frequency to a musical note.
Parameters:
input: must be a double between 27.5Hz (A0) and 14080Hz (A9)
round: round note to nearest 5 cents if true
Returns:
A note struct which includes:
- name: A string representing the note, such as "C4"
- cents: Number of cents as an int, optionally rounded to the
nearest 5 cents
- midi: a midi number corresponding to the closest semitone for
the frequency
... OR NULL if frequency out of range.
*/
char *noteString(Note n);
/*
Returns a string representation of the note.
Parameters:
n: The note struct pointer
Returns:
A string representation, such as "C4 plus 20 cents" or "A#4 minus
15 cents"
*/
#endif
