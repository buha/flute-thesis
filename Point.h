/*
Point.h
By Andrew Botros, 2001-2004
A data point ADT.
*/
#ifndef POINT_H_PROTECTOR
#define POINT_H_PROTECTOR
/* Point: { x, y } */
typedef struct point_str {
  double x;
  double y;
} * Point;
void printPoint(Point p);
/*
Prints the contents of a Point struct to stdout.
Parameters:
p: the Point struct.
*/
void printImpedancePoint(Point p);
/*
Prints the contents of a Point struct to stdout in the Impedance
format.
Parameters:
p: the Point struct.
*/
Point parseImpedancePoint(char *line, int series);
/*
Parses a line of text into its impedance point data.
Parameters:
line: the line of data.
Returns:
A Point struct representing the data.
NULL if line does not contain two numbers.
*/
#endif
