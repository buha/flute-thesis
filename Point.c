/*
Point.h
By Andrew Botros, 2001-2004
A data point ADT.
*/
#include "Point.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void printPoint(Point p) {
  printf("(%.2f,%.2f)", p->x, p->y);
  return;
}
void printImpedancePoint(Point p) {
  printf("%.2f\t%.2f\n", p->x, p->y);
  return;
}
Point parseImpedancePoint(char *line, int series) {
  Point p = (Point)malloc(sizeof(*p));
  char *delimiters = " \t";
  char *token;
  int n;
  /* Parse x value */
  token = strtok(line, delimiters);
  if (token != NULL)
    p->x = atof(token);
  else
    return NULL;
  /* Parse y value */
  for (n = 0; n < series; n++)
    token = strtok(NULL, delimiters);
  if (token != NULL && token != "\n") {
    p->y = atof(token);
  } else
    return NULL;
  return p;
}
