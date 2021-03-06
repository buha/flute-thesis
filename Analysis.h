/*
Analysis.h
By Andrew Botros, 2001-2004
Modified by Paul Dickens, 2006
Analyses an acoustic impedance spectrum
for playable notes and multiphonics.
*/
#ifndef ANALYSIS_H_PROTECTOR
#define ANALYSIS_H_PROTECTOR
#include "Minima.h"
#include "Vector.h"
typedef enum { NOTES, MULTIPHONICS2, MULTIPHONICS3 } AnalysisType;
void Analysis(char *filename, int applypitchcorrection, int displayharmonicity,
              AnalysisType at);
/*
Performs an analysis of the given impedance data file, sending
the required analysis results to stdout.
Parameters:
filename: the filename of the impedance data file
applypitchcorrection: boolean to flag the use of pitch correction
displayharmonicity: boolean to flag output of harmonicity data
at: the type of analysis (notes, two note multiphonics or
three note multiphonics)
*/
int analyseNote(Minimum m, int applypitchcorrection, int displayharmonicity,
                int output);
/*
Determines the note and playability details of a Minimum if
playable, after correcting its pitch (output optional).
Parameters:
m: the Minimum to be analysed
applypitchcorrection: boolean to flag the use of pitch correction
displayharmonicity: boolean to flag output of harmonicity data
output: boolean to flag output
Returns:
1 if the Minimum is playable and within defined note range, 0 if
not.
*/
int playable(Minimum m);
/*
Determines whether a given Minimum is playable. Playability
is defined by the decision tree generated by C5.0. The decision
tree is implemented in this function.
Parameters:
m: the Minimum to be analysed
Returns:
1 if playable, 0 if not.
*/
double playabilityLevel(Minimum m);
/*
Determines the playability level of a playable Minimum. The
playability level is determined by the rules generated by M5'.
The M5' rules are implemented in this function. In general,
3.0 is easily playable while 1.0 is difficult to play.
Parameters:
m: the Minimum to be analysed
Returns:
The playability level as a double.
*/
double strengthLevel(Minimum m);
/*
Determines the strength level of a playable Minimum. The
playability level is determined by the rules generated by M5'.
The M5' rules are implemented in this function. In general,
4.0 is strong and bright while 1.0 is weak and dark.
Parameters:
m: the Minimum to be analysed
Returns:
The strength level as a double.
*/
Minimum getCompletedMinimum(Minimum m);
/*
Creates a copy of a given Minimum with all empty fields initialised
to the average values of the expert set (cf M5').
Parameters:
m: the Minimum to be analysed
Returns:
A new Minimum with NaN fields replaced with expert set averages.
*/
void pitchCorrection(Minimum m);
/*
Corrects the pitch of a playable Minimum according to experimental
results from a number of flutists.
Parameters:
m: the Minimum to be analysed
*/
void analyseMultiphonics2(Vector playableminv);
/*
Determines if any two note multiphonics are playable in a set of
playable notes for a fingering. Multiphonics are defined as notes
which are "not harmonically related" (see code for details).
Ouputs the multiphonic if so.
Parameters:
playableminv: the set of playable minima for a fingering
*/
void analyseMultiphonics3(Vector playableminv);
/*
Determines if any three note multiphonics are playable in a set of
playable notes for a fingering. Multiphonics are defined as notes
which are "not harmonically related" (see code for details).
Ouputs the multiphonic if so.
Parameters:
playableminv: the set of playable minima for a fingering
*/
int noteDistance(Minimum m1, Minimum m2, Vector playableminv);
/*
Calculates the distance in playable notes between two given minima.
Parameters:
m1: the first minimum
m2: the second minimum
playableminv: the set of playable minima for a fingering
Returns:
The number of playable minima between the given two minima, plus
1.
-1 if error.
*/
int pitchIndex(Vector minv);
/*
Calculates the sum of squares of cents of the given minima.
Parameters:
minv: the set of minima
Returns:
The result of the calculation (0 if no minima).
*/
char *allNotes(Vector playableminv);
/*
Concatenates the note strings of the playable notes for a
fingering, delimited by ';'
Parameters:
playableminv: the set of playable minima for a fingering
Returns:
The concatenated null terminated string
*/
int harmonic(Minimum m1, Minimum m2, Vector playableminv);
/*
Determines whether two minima are harmonically related.
See code for definition of harmonic.
Parameters:
m1: the first minimum
m2: the second minimum
playableminv: the set of playable minima for a fingering
Returns:
1 if m1 and m2 are harmonic, 0 if not.
*/
int inArray(int *array, int size, int num);
/*
Determines whether an integer is present in a given sized
array of integers.
Parameters:
array: the array of integers
size: the number of elements in the array
num: the integer to find in the array
Returns:
1 if the integer is present, 0 if not.
*/
#endif
