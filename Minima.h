/*
Minima.h
By Andrew Botros, 2001-2004.
Modified by Paul Dickens, 2006-2007.
Characterises the minima of acoustic impedance spectra.
IMPORTANT: Calculations are optimised for a spectrum resolution of
2Hz.
NOTE: The weighting of the frequency of an impedance extremum fit to
favour the absolute emtremum has been changed to an option in the
function 'parabolaExt' with a default of 0 (no weighting). A block
has also been added to the function 'extrema' to handle the case
when the next point to be considered is equal to the previous one
(this fixed a bug in the code).
*/
#ifndef MINIMA_H_PROTECTOR
#define MINIMA_H_PROTECTOR
#include "Note.h"
#include "Vector.h"
/* minmax: a minimum/maximum flag type */
typedef enum { MINIMUM, MAXIMUM } minmax;
/*
Minimum: {
musical note struct, frequency, impedance, bandwidth,
number of harmonics, weighted average harmonic impedance,
distance to left minimum in Hz,
difference in impedance of left minimum in dB,
distance to right minimum in Hz,
difference in impedance of right minimum in dB,
distance to left maximum in Hz,
difference in impedance of left maximum in dB,
distance to right maximum in Hz,
difference in impedance of right maximum in dB
}
*/
typedef struct minimum_str {
  Note note;
  double f;
  double Z;
  double B;
  double numharm;
  double meanharmZ;
  double L_min_df;
  double L_min_dZ;
  double R_min_df;
  double R_min_dZ;
  double L_max_df;
  double L_max_dZ;
  double R_max_df;
  double R_max_dZ;
} * Minimum;
/* Extremum: { max/min, frequency, impedance, bandwidth } */
typedef struct extremum_str {
  minmax type;
  double f;
  double Z;
  double B;
} * Extremum;
/* Harmonic:
{ harmonic number, weighted average harmonic impedance } */
typedef struct harmonic_str {
  int n;
  double Z;
} * Harmonic;
Vector minima(Vector allpoints);
/*
Evaluates the minima in an impedance spectra data file.
Parameters:
allpoints: the vector of data points.
Returns:
A vector of Minimum structs.
*/
Vector extrema(Vector allpoints);
/*
Evaluates the extrema in an impedance spectra data file.
Parameters:
allpoints: the vector of data points.
Returns:
A vector of Extremum structs, or NULL if bad data file.
*/
Extremum parabolaExt(Vector points, minmax type, int weight);
/*
Calculates the extremum of a given vector of data points. It
performs a least squares parabolic fit on the data and evaluates the
frequency, impedance and bandwidth from this fit. Note that the
frequency of extrema are (optionally) averaged with the absolute
extrema present in the data set.
Parameters:
points: a Vector of Points about the extremum.
type: a minmax type indicating minimum/maximum.
weight: if true weights the calculation of the extremum frequency
in favour of the absolute extremum.
Returns:
An Extremum struct containing evaluated minimum.
*/
int progressPoints(Vector points, Vector allpoints, int index);
/*
Reads the next data point present in the impedance spectra file, and
places this within a window of data points - also shifting the
window to the right by one point and popping the leftmost data
point.
Parameters:
points: the window of data points that the function updates.
allpoints: the vector of all data points.
index: the next point in the allpoints vector to read.
Returns:
1 if successful, 0 otherwise (such as EOF reached).
*/
int numExtrema(Vector points, minmax type);
/*
Calculates the number of local extrema in the given Vector window.
Parameters:
points: the window of data points that the function processes.
type: a minmax type indicating minimum/maximum.
Returns:
The number of local extrema.
*/
double lowest(Vector points);
/*
Evaluates the frequency of the lowest y value among a set of data
points.
Parameters:
points: a Vector of Points
Returns:
A double of the frequency of the lowest y value among points
*/
double highest(Vector points);
/*
Evaluates the frequency of the highest y value among a set of data
points.
Parameters:
points: a Vector of Points
Returns:
A double of the frequency of the highest y value among points
*/
void harmonicity(Vector minv);
/*
Sets the harmonicity variables numharm and meanharmZ
for each Minimum struct in a given minima vector.
Parameters:
minv: the vector of Minimum structs to be updated
*/
Vector harmonics(Vector minv, int pos);
/*
Evaluates the harmonics of a particular Minimum struct in a
given minima vector.
Parameters:
minv: the vector of Minimum structs for a particular data file
pos: the index of the required Mimimum struct in the vector
(starting at 0)
Returns:
A vector of Harmonic structs, one for each existent harmonic for
the particular Minimum in question.
*/
double harmAvgZ(Vector harmv);
/*
Calculates the weighted average of the impedance levels for the
first HARMONICS_AVERAGED number of harmonics in a given set. Each
harmonic in the vector contains its integer frequency ratio
(relative to an anonymous minimum), and its impedance level. An
integer ratio of 2 (the second harmonic) has a relative weight of
1/2. An integer ratio of 3 (the third harmonic) has a relative
weight of 1/3, and so on.
Parameters:
harmv: the vector of Harmonic structs for a particular minimum
Returns:
The weighted average of their impedance levels as a double
*/
double *harmonicsArray(Vector minv, int pos);
/*
Calculates the frequency ratios of minima following a particular
Minimum.
Parameters:
minv: the vector of Minimum structs for a particular data file
pos: the index of the required Mimimum struct in the vector
(starting at 0)
Returns:
An array of doubles, one for each minimum following the Minimum
in question. Each double is the frequency of the higher minima
divided by the frequency of the Minimum in question.
*/
double invalidNum(void);
/*
Gives the result of atof("+NAN").
Returns:
A double representing NaN.
*/
int isValidNum(double d);
/*
Evaluates whether a number is valid. NaN and inf values are invalid.
Parameters:
d: the double to evaluate
Returns:
1 if valid, 0 otherwise
*/
double det(double **M);
/*
Calculates the determinant of a 3x3 matrix.
Parameters:
M: a 2 dimensional double array representing the matrix.
Returns:
The determinant of M as a double.
*/
double **A(Vector points);
double **B(Vector points);
double **C(Vector points);
double **D(Vector points);
/*
A, B, C, D return matrices necessary in calculating the least
squares parabolic fit to a set of data points. Each function is
passed a Vector of Points about the minimum to be fitted.
Refer to Bevington (1969), "Data Reduction and Error Analysis
for the Physical Sciences".
*/
#endif
