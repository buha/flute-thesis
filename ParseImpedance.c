/*
ParseImpedance.c
By Andrew Botros, 2001-2004
Modified by Paul Dickens, 2005, 2006
ParseImpedance.c reads an acoustic impedance spectra file and
returns a Vector of Vectors of Points. Refer to ParseImpedance.h for
interface details.
*/
#include "ParseImpedance.h"
#include "Point.h"
#include "Vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int parseImpedanceFile(Vector filev, char *filename) {
  FILE *fp;
  char *line = (char *)malloc(BUFSIZ * sizeof(char));
  char *delimiters = " \t";
  char *token;
  int *midi;
  double x;
  Point p;
  Vector seriesVector;
  int series;
  /* open data file and indicate any error */
  if ((fp = fopen(filename, "r")) == NULL) {
    fprintf(stderr, "Cannot open file %s\n", filename);
    return 0;
  }
  /* parse midi line and add midi numbers to Vector */
  fgets(line, BUFSIZ, fp);
  token = strtok(line, delimiters);
  while ((token != NULL) && (token != "\n")) {
    seriesVector = createVector();
    midi = (int *)malloc(sizeof(char));
    *midi = atoi(token);
    addElement(seriesVector, midi);
    addElement(filev, seriesVector);
    token = strtok(NULL, delimiters);
  }
  /* parse each line and add points to Vector */
  while (fgets(line, BUFSIZ, fp) != NULL) {
    /* Parse x value */
    token = strtok(line, delimiters);
    if (token != NULL)
      x = atof(token);
    else {
      fprintf(stderr, "File %s is invalid\n", filename);
      return 0;
    }
    series = 0;
    while (((token = strtok(NULL, delimiters)) != NULL) && (token != "\n")) {
      p = (Point)malloc(sizeof(*p));
      p->x = x;
      p->y = atof(token);
      seriesVector = (Vector)elementAt(filev, series);
      addElement(seriesVector, p);
      series++;
    }
  }
  /* close data file and return Vector of Vectors of Points */
  fclose(fp);
  return 1;
}
