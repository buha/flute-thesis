/*
Minima.c
By Andrew Botros, 2001-2004.
Modified by Paul Dickens, 2006-2007.
Characterises the minima of acoustic impedance spectra.
Refer to Minima.h for interface details.
IMPORTANT: Calculations are optimised for a spectrum resolution of
2Hz.
NOTE: The weighting of the frequency of an impedance extremum fit to
favour the absolute emtremum has been changed to an option in the
function 'parabolaExt' with a default of 0 (no weighting). A block
has also been added to the function 'extrema' to handle the case
when the next point to be considered is equal to the previous one
(this fixed a bug in the code).
*/
/* necessary define for some gcc math.h functions */
#ifndef _ISOC99_SOURCE
#define _ISOC99_SOURCE
#endif
#include "Minima.h"
#include "Note.h"
#include "Point.h"
#include "Vector.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* number of data points approximated around each minimum */
#define NUM_POINTS 11
/* number of points which must be consecutively increasing or
decreasing to define the spectrum as rising or dropping. */
#define TWEAK 5
/* weight fit to absolute extremum? */
#define WEIGHT 0
/* to evaluate harmonics, a window is defined around the frequency f
of the fundamental minimum: f +/- HARMONIC_WINDOW/100. Minima which
lie within integer multiples of this window are deemed to be
harmonics. */
#define HARMONIC_WINDOW 5.0
/* maximum number of harmonics used to calculate average impedance of
harmonics */
#define HARMONICS_AVERAGED 3
Vector minima(Vector allpoints) {
  int i, j;
  Vector extv = extrema(allpoints);
  Extremum e, searche;
  Minimum m;
  Vector minv = createVector();
  /* for each minimum in extrema list */
  for (i = 0; i < sizeVector(extv); i++) {
    e = (Extremum)elementAt(extv, i);
    if (e->type == MINIMUM) {
      /* create Minimum struct with same f, Z, B */
      m = (Minimum)malloc(sizeof(*m));
      m->f = e->f;
      m->Z = e->Z;
      m->B = e->B;
      /* search for left maximum and calculate df, dZ */
      for (j = i - 1; j >= 0; j--) {
        searche = (Extremum)elementAt(extv, j);
        if (searche->type == MAXIMUM) {
          m->L_max_df = e->f - searche->f;
          m->L_max_dZ = searche->Z - e->Z;
          break;
        }
      }
      /* if none found, set to nan */
      if (j < 0) {
        m->L_max_df = invalidNum();
        m->L_max_dZ = invalidNum();
      }
      /* search for left minimum and calculate df, dZ */
      for (j = i - 1; j >= 0; j--) {
        searche = (Extremum)elementAt(extv, j);
        if (searche->type == MINIMUM) {
          m->L_min_df = e->f - searche->f;
          m->L_min_dZ = searche->Z - e->Z;
          break;
        }
      }
      /* if none found, set to nan */
      if (j < 0) {
        m->L_min_df = invalidNum();
        m->L_min_dZ = invalidNum();
      }
      /* search for left maximum and calculate df, dZ */
      for (j = i + 1; j < sizeVector(extv); j++) {
        searche = (Extremum)elementAt(extv, j);
        if (searche->type == MAXIMUM) {
          m->R_max_df = searche->f - e->f;
          m->R_max_dZ = searche->Z - e->Z;
          break;
        }
      }
      /* if none found, set to nan */
      if (j >= sizeVector(extv)) {
        m->R_max_df = invalidNum();
        m->R_max_dZ = invalidNum();
      }
      /* search for left minimum and calculate df, dZ */
      for (j = i + 1; j < sizeVector(extv); j++) {
        searche = (Extremum)elementAt(extv, j);
        if (searche->type == MINIMUM) {
          m->R_min_df = searche->f - e->f;
          m->R_min_dZ = searche->Z - e->Z;
          break;
        }
      }
      /* if none found, set to nan */
      if (j >= sizeVector(extv)) {
        m->R_min_df = invalidNum();
        m->R_min_dZ = invalidNum();
      }
      /* add minimum to minimum vector */
      addElement(minv, m);
    }
  }
  /* set the harmonic details of all minima in vector */
  harmonicity(minv);
  return minv;
}
Vector extrema(Vector allpoints) {
  int i, j;
  int seq_inc, seq_dec;
  int descent, ascent;
  Extremum e;
  Point p;
  Vector extv = createVector();
  Vector points = createVector();
  /* load vector of points with the initial portion of data.
  All points are equal to the first point in the data. */
  for (i = 0; i < NUM_POINTS; i++) {
    p = (Point)malloc(sizeof(*p));
    *p = *((Point)elementAt(allpoints, 0));
    addElement(points, p);
  }
  /* initialise flags */
  seq_inc = 0;
  seq_dec = 0;
  descent = 0;
  ascent = 0;
  /* progress points one at a time till end of data set */
  for (i = 0; progressPoints(points, allpoints, i); i++) {
    /* if graph increases... */
    if (((Point)elementAt(points, sizeVector(points) - 1))->y >
        ((Point)elementAt(points, sizeVector(points) - 2))->y) {
      /* tally number of points increased ... set flag when
      we have moved up far enough to expect minimum/maximum */
      seq_inc += 2;
      if (seq_inc / 2 >= TWEAK)
        ascent = 1;
      if (seq_dec / 2 > 0)
        seq_dec -= 2;
      if (descent && ascent) {
        /* run parabola least squares fit on points, add minimum to
        vector */
        for (j = 0; j < (NUM_POINTS / 2 - TWEAK); j++) {
          progressPoints(points, allpoints, i);
          i++;
        }
        e = parabolaExt(points, MINIMUM, WEIGHT);
        if (e != NULL)
          addElement(extv, e);
        /* reset flags and remember we're ascending */
        seq_inc = 0;
        seq_dec = 0;
        descent = 0;
        ascent = 1;
      }
    }
    /* if graph decreases... */
    if (((Point)elementAt(points, sizeVector(points) - 1))->y <
        ((Point)elementAt(points, sizeVector(points) - 2))->y) {
      /* tally number of points increased ... set flag when
      we have moved up far enough to expect minimum/maximum */
      seq_dec += 2;
      if (seq_dec / 2 >= TWEAK)
        descent = 1;
      if (seq_inc / 2 > 0)
        seq_inc -= 2;
      if (ascent && descent) {
        /* run parabola least squares fit on points, add minimum to
        vector */
        for (j = 0; j < (NUM_POINTS / 2 - TWEAK); j++) {
          progressPoints(points, allpoints, i);
          i++;
        }
        e = parabolaExt(points, MAXIMUM, WEIGHT);
        if (e != NULL)
          addElement(extv, e);
        /* reset flags and remember we're descending*/
        seq_inc = 0;
        seq_dec = 0;
        descent = 1;
        ascent = 0;
      }
    }
    /* if graph neither increases nor decreases... */
    if (((Point)elementAt(points, sizeVector(points) - 1))->y ==
        ((Point)elementAt(points, sizeVector(points) - 2))->y) {
      if (descent) {
        seq_inc++;
        seq_dec--;
      }
      if (ascent) {
        seq_dec++;
        seq_inc--;
      }
    }
  }
  return extv;
}
Extremum parabolaExt(Vector points, minmax type, int weight) {
  Extremum ext = (Extremum)malloc(sizeof(*ext));
  int numext;
  double a, b, c, delta;
  double x0, y0;
  double minf;
  double maxf;
  /* compute least squares parabolic fit from data vector */
  delta = det(D(points));
  a = (1.0 / delta) * det(A(points));
  b = (1.0 / delta) * det(B(points));
  c = (1.0 / delta) * det(C(points));
  if (type == MINIMUM) {
    ext->type = MINIMUM;
    /* determine analytical minimum from parabola fit */
    x0 = -1.0 * (b / (2.0 * c));
    y0 = a + b * x0 + c * x0 * x0;
    if (weight) {
      /* find frequency of absolute minimum in data vector */
      minf = lowest(points);
      numext = numExtrema(points, MINIMUM);
      /* if only one minimum in set, heavily weight to absolute
      minimum */
      if (numext == 1) {
        ext->f = (x0 * minf) / (0.95 * x0 + 0.05 * minf);
      }
      /* otherwise, moderately weight towards absolute minimum */
      else {
        ext->f = (x0 * minf) / (0.75 * x0 + 0.25 * minf);
      }
    }
    /* if weight is false, use fit to determine f */
    else
      ext->f = x0;
    /* construct minimum ...
    impedance is parabola minimum
    bandwidth and Q factor is 3dB span of parabola */
    ext->Z = y0;
    ext->B =
        2.0 * ((sqrt((y0 + 3.0 - a) / c + (b / (2.0 * c)) * (b / (2.0 * c))) -
                (b / (2.0 * c))) -
               x0);
  } else {
    ext->type = MAXIMUM;
    /* determine analytical maximum from parabola fit */
    x0 = -1.0 * (b / (2.0 * c));
    y0 = a + b * x0 + c * x0 * x0;
    if (weight) {
      /* find frequency of absolute maximum in data vector */
      maxf = highest(points);
      numext = numExtrema(points, MAXIMUM);
      /* if only one maximum in set, heavily weight to absolute
      maximum */
      if (numext == 1) {
        ext->f = (x0 * maxf) / (0.95 * x0 + 0.05 * maxf);
      }
      /* otherwise, moderately weight towards absolute maximum */
      else {
        ext->f = (x0 * maxf) / (0.75 * x0 + 0.25 * maxf);
      }
    }
    /* if weight is false, use fit to determine f */
    else
      ext->f = x0;
    /* construct maximum
    impedance is parabola maximum
    bandwidth and Q factor is 3dB span of parabola */
    ext->Z = y0;
    ext->B =
        2.0 * ((sqrt((y0 - 3.0 - a) / c + (b / (2.0 * c)) * (b / (2.0 * c))) -
                (b / (2.0 * c))) -
               x0);
  }
  return ext;
}
int progressPoints(Vector points, Vector allpoints, int index) {
  Point p = (Point)malloc(sizeof(*p));
  /* read points if data still in file */
  if (index < sizeVector(allpoints)) {
    *p = *((Point)elementAt(allpoints, index));
    /* remove oldest point and add newly read point */
    popFront(points);
    addElement(points, p);
    return 1;
  }
  /* otherwise end of data reached */
  else
    return 0;
}
int numExtrema(Vector points, minmax type) {
  int i;
  int nmin = 0;
  int nmax = 0;
  int descent = 1;
  Point p1, p2;
  p1 = (Point)elementAt(points, 0);
  /* run through all points given */
  for (i = 1; i < sizeVector(points); i++) {
    p2 = (Point)elementAt(points, i);
    /* if next point increases */
    if (p2->y > p1->y && descent) {
      nmin++;
      descent = 0;
    }
    /* if next point decreases, indicate decrease */
    if (p2->y < p1->y && !descent) {
      nmax++;
      descent = 1;
    }
    p1 = p2;
  }
  if (type == MINIMUM)
    return nmin;
  else
    return nmax;
}
double lowest(Vector points) {
  int i;
  Point p;
  double minf, minZ;
  /* minimum is set to first point */
  p = (Point)elementAt(points, 0);
  minf = p->x;
  minZ = p->y;
  /* find lowest point in data vector and return it */
  for (i = 1; i < sizeVector(points); i++) {
    p = (Point)elementAt(points, i);
    if (p->y < minZ) {
      minf = p->x;
      minZ = p->y;
    }
  }
  return minf;
}
double highest(Vector points) {
  int i;
  Point p;
  double maxf, maxZ;
  /* maximum is set to first point */
  p = (Point)elementAt(points, 0);
  maxf = p->x;
  maxZ = p->y;
  /* find highest point in data vector and return it */
  for (i = 1; i < sizeVector(points); i++) {
    p = (Point)elementAt(points, i);
    if (p->y > maxZ) {
      maxf = p->x;
      maxZ = p->y;
    }
  }
  return maxf;
}
void harmonicity(Vector minv) {
  int i;
  Vector harmv;
  /* for each Minimum in vector... */
  for (i = 0; i < sizeVector(minv); i++) {
    /* find all the harmonic minima of the particular Minimum */
    harmv = harmonics(minv, i);
    /* record the number of harmonics the Minimum has */
    ((Minimum)elementAt(minv, i))->numharm = (double)sizeVector(harmv);
    /* calculate and record the weighted average impedance of
    these harmonic minima. */
    ((Minimum)elementAt(minv, i))->meanharmZ = harmAvgZ(harmv);
  }
  return;
}
Vector harmonics(Vector minv, int pos) {
  int i;
  int harmonic;
  double window, left_bound, right_bound;
  double df;
  int curharmonic;
  int found;
  /* get the ratios of minima frequencies following
  the Minimum in question */
  double *ha = harmonicsArray(minv, pos);
  int arraysize = sizeVector(minv) - pos - 1;
  Vector harmv = createVector();
  Harmonic h;
  i = 1;
  /* search for possible integer harmonics until
  last Minimum is reached */
  for (harmonic = 2;; harmonic++) {
    i--;
    /* calculate a window in which harmonic will be defined.
    The widest window allowable is 0.5. */
    window = (HARMONIC_WINDOW / 100.0) * harmonic;
    if (window > 0.5)
      window = 0.5;
    left_bound = harmonic - window;
    right_bound = harmonic + window;
    found = 0;
    /* search for any Minimum which exists in the defined window.
    Compare this to previously found minima in the same
    window, and choose the Minimum closest to the integer
    mark as the harmonic. */
    while (i < arraysize) {
      if (ha[i] >= right_bound) {
        i++;
        break;
      } else {
        if ((ha[i] > left_bound) && (ha[i] < right_bound)) {
          if (found) {
            if (fabs(harmonic - ha[i]) < df) {
              df = fabs(harmonic - ha[i]);
              curharmonic = i;
            }
          } else {
            df = fabs(harmonic - ha[i]);
            curharmonic = i;
            found = 1;
          }
        }
        i++;
      }
    }
    /* if a minimum is found in this window, enter the rounded off
    integer ratio and the impedance level of the harmonic into
    a Harmonic structure and insert this into a vector. */
    if (found && (ha[i - 1] >= harmonic)) {
      h = (Harmonic)malloc(sizeof(*h));
      h->n = round(ha[curharmonic]);
      h->Z = ((Minimum)elementAt(minv, pos + curharmonic + 1))->Z;
      addElement(harmv, h);
    }
    /* if last Minimum reached, break */
    if (i == arraysize)
      break;
  }
  /* return all found harmonics */
  return harmv;
}
double harmAvgZ(Vector harmv) {
  int i;
  int numharm;
  double totalZ = 0.0;
  double totalfraction = 0.0;
  Harmonic h;
  /* maximum number of harmonics averaged
  is either HARMONICS_AVERAGED or the number
  of harmonics present */
  if (sizeVector(harmv) < HARMONICS_AVERAGED)
    numharm = sizeVector(harmv);
  else
    numharm = HARMONICS_AVERAGED;
  /* if no harmonics present, return NaN */
  if (numharm == 0)
    return invalidNum();
  /* for the first few appropriate harmonics,
  compute and return the weighted sum of their impedance
  levels, with the second harmonic (if present) having a
  weighting of 1/2, the third harmonic (if present)
  having a weighting of 1/3, etc. */
  for (i = 0; i < numharm; i++) {
    h = (Harmonic)elementAt(harmv, i);
    totalZ = totalZ + (h->Z) / (h->n);
    totalfraction = totalfraction + 1.0 / (double)(h->n);
  }
  return totalZ / totalfraction;
}
double *harmonicsArray(Vector minv, int pos) {
  int i, j = 0;
  int length = sizeVector(minv) - pos - 1;
  double *ha = (double *)malloc(length * sizeof(double));
  double f;
  Minimum m;
  /* get Minimum at required position in vector */
  m = (Minimum)elementAt(minv, pos);
  f = m->f;
  /* for each Minimum following the chosen Minimum,
  calculate frequency ratio of the two minima and
  place result in double array. */
  for (i = pos + 1; i < sizeVector(minv); i++) {
    m = (Minimum)elementAt(minv, i);
    ha[j] = m->f / f;
    j++;
  }
  return ha;
}
double invalidNum(void) { return atof("+NAN"); }
int isValidNum(double d) {
  int flag = 1;
  char *plusnantest = (char *)malloc(BUFSIZ);
  char *minusnantest = (char *)malloc(BUFSIZ);
  char *dstring = (char *)malloc(BUFSIZ);
  /* generate all possible floating point errors and
  compare these errors to the given number */
  sprintf(plusnantest, "%f", atof("+NAN"));
  sprintf(minusnantest, "%f", atof("-NAN"));
  sprintf(dstring, "%f", d);
  if (strcmp(dstring, plusnantest) == 0)
    flag = 0;
  if (strcmp(dstring, minusnantest) == 0)
    flag = 0;
  if (d == atof("+INF"))
    flag = 0;
  if (d == atof("-INF"))
    flag = 0;
  return flag;
}
/* Refer to Minima.h for documentation on the following */
double det(double **M) {
  return ((M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2])) -
          (M[2][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2])) +
          (M[3][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2])));
}
double **A(Vector points) {
  double **M;
  double x, y;
  int i, j;
  M = (double **)malloc(4 * sizeof(double *));
  for (i = 0; i <= 3; i++)
    M[i] = (double *)malloc(4 * sizeof(double));
  for (i = 1; i <= 3; i++) {
    for (j = 1; j <= 3; j++)
      M[i][j] = 0.0;
  }
  for (i = 0; i < sizeVector(points); i++) {
    x = ((Point)elementAt(points, i))->x;
    y = ((Point)elementAt(points, i))->y;
    M[1][1] += y;
    M[1][2] += x;
    M[1][3] += x * x;
    M[2][1] += x * y;
    M[2][2] += x * x;
    M[2][3] += x * x * x;
    M[3][1] += x * x * y;
    M[3][2] += x * x * x;
    M[3][3] += x * x * x * x;
  }
  return M;
}
double **B(Vector points) {
  double **M;
  double x, y;
  int i, j;
  M = (double **)malloc(4 * sizeof(double *));
  for (i = 0; i <= 3; i++)
    M[i] = (double *)malloc(4 * sizeof(double));
  for (i = 1; i <= 3; i++) {
    for (j = 1; j <= 3; j++)
      M[i][j] = 0.0;
  }
  for (i = 0; i < sizeVector(points); i++) {
    x = ((Point)elementAt(points, i))->x;
    y = ((Point)elementAt(points, i))->y;
    M[1][1] += 1.0;
    M[1][2] += y;
    M[1][3] += x * x;
    M[2][1] += x;
    M[2][2] += x * y;
    M[2][3] += x * x * x;
    M[3][1] += x * x;
    M[3][2] += x * x * y;
    M[3][3] += x * x * x * x;
  }
  return M;
}
double **C(Vector points) {
  double **M;
  double x, y;
  int i, j;
  M = (double **)malloc(4 * sizeof(double *));
  for (i = 0; i <= 3; i++)
    M[i] = (double *)malloc(4 * sizeof(double));
  for (i = 1; i <= 3; i++) {
    for (j = 1; j <= 3; j++)
      M[i][j] = 0.0;
  }
  for (i = 0; i < sizeVector(points); i++) {
    x = ((Point)elementAt(points, i))->x;
    y = ((Point)elementAt(points, i))->y;
    M[1][1] += 1.0;
    M[1][2] += x;
    M[1][3] += y;
    M[2][1] += x;
    M[2][2] += x * x;
    M[2][3] += x * y;
    M[3][1] += x * x;
    M[3][2] += x * x * x;
    M[3][3] += x * x * y;
  }
  return M;
}
double **D(Vector points) {
  double **M;
  double x, y;
  int i, j;
  M = (double **)malloc(4 * sizeof(double *));
  for (i = 0; i <= 3; i++)
    M[i] = (double *)malloc(4 * sizeof(double));
  for (i = 1; i <= 3; i++) {
    for (j = 1; j <= 3; j++)
      M[i][j] = 0.0;
  }
  for (i = 0; i < sizeVector(points); i++) {
    x = ((Point)elementAt(points, i))->x;
    y = ((Point)elementAt(points, i))->y;
    M[1][1] += 1.0;
    M[1][2] += x;
    M[1][3] += x * x;
    M[2][1] += x;
    M[2][2] += x * x;
    M[2][3] += x * x * x;
    M[3][1] += x * x;
    M[3][2] += x * x * x;
    M[3][3] += x * x * x * x;
  }
  return M;
}
