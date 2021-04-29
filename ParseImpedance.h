/*
ParseImpedance.h
By Andrew Botros, 2001-2004
Modified by Paul Dickens Apr 2005, 2006
ParseImpedance.c reads an acoustic impedance spectra file and
returns a Vector of Vectors of Points.
*/
#ifndef PARSEIMPEDANCE_H_PROTECTOR
#define PARSEIMPEDANCE_H_PROTECTOR
#include "Vector.h"
int parseImpedanceFile(Vector filev, char *filename);
/*
Reads each line of a given acoustic impedance spectra file
and constructs a Vector of Vectors of Points
representing the file.
Parameters:
filev: the return Vector
filename: the name of the file to read
Returns:
1 if the operatioon was successful
0 otherwise
*/
#endif
