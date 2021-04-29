/*
Analysis.c
By Andrew Botros, 2001-2004
Modified by Paul Dickens, 2006
Analyses an acoustic impedance spectrum for playable
notes. See Analysis.h for interface details.
*/
#include "Analysis.h"
#include "Minima.h"
#include "Note.h"
#include "ParseImpedance.h"
#include "Point.h"
#include "Vector.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void Analysis(char *filename, int applypitchcorrection, int displayharmonicity,
              AnalysisType at) {
  Vector fileData = createVector(), seriesData, minv;
  int midi;
  Minimum m;
  int i;
  int series;
  /* read and parse data file into a Vector of Vectors of Points */
  if (!parseImpedanceFile(fileData, filename)) {
    fprintf(stderr, "AnalyseNotes error: ");
    fprintf(stderr, "AnalyseNotes failed to parse impedance file.\n");
    return;
  }
  for (series = 0; series < sizeVector(fileData); series++) {
    seriesData = (Vector)elementAt(fileData, series);
    midi = *((int *)elementAt(seriesData, 0));
    popFront(seriesData);
    /* evaluate all minima in the data */
    minv = minima(seriesData);
    /* print MIDI number */
    printf("%d\t", midi);
    /* determine playable minima */
    for (i = 0; i < sizeVector(minv); i++) {
      m = (Minimum)elementAt(minv, i);
      /* evaluate musical note from frequency (do not round) */
      m->note = note(m->f, 0);
      if (m->note->midi == midi)
        analyseNote(m, applypitchcorrection, displayharmonicity, at == NOTES);
    }
    printf("\n");
  }
  return;
}
int analyseNote(Minimum m, int applypitchcorrection, int displayharmonicity,
                int output) {
  /* determine if Minimum is playable */
  if (!playable(m))
    return 0;
  /* determine playability and strength levels of playable Minimum */
  double playability = playabilityLevel(m);
  double strength = strengthLevel(m);
  /* pitch correct Minimum if desired */
  if (applypitchcorrection)
    pitchCorrection(m);
  /* evaluate musical note from frequency (do not round) */
  m->note = note(m->f, 0);
  if (m->note == NULL)
    return 0;
  else {
    if (output) {
      /* output notes */
      if (displayharmonicity)
        printf("%.1f\t%.1f\t%.1f\t%d\t%.1f\t%.0f\t%.1f", playability, strength,
               m->f, m->note->cents, m->Z, m->numharm, m->meanharmZ);
      else
        printf("%.1f\t%.1f\t%.1f\t%d\t%.1f", playability, strength, m->f,
               m->note->cents, m->Z);
    }
    return 1;
  }
}
int playable(Minimum m) {
  /* implement C5.0 decision tree */
  if (m->Z <= 122.6) {
    if (!isValidNum(m->R_min_dZ) || (m->R_min_dZ <= -2.6))
      return 0;
    else {
      if (m->Z <= 116.7)
        return 1;
      else {
        if (!isValidNum(m->R_min_df) || (m->R_min_df <= 261.1))
          return 0;
        else
          return 1;
      }
    }
  } else
    return 0;
}
double playabilityLevel(Minimum m) {
  /* implement M5' model tree */
  Minimum mcomp = getCompletedMinimum(m);
  double LM, LM1, LM2;
  LM1 = -0.0028 * mcomp->Z + 0.0090 * mcomp->numharm +
        0.0023 * mcomp->meanharmZ + 0.0002 * mcomp->L_min_df +
        0.0040 * mcomp->L_min_dZ - 0.0005 * mcomp->L_max_df +
        0.0033 * mcomp->L_max_dZ + 0.0002 * mcomp->R_max_df + 2.7930;
  LM2 = -0.0012 * mcomp->Z + 0.0975 * mcomp->numharm +
        0.0210 * mcomp->meanharmZ + 0.0011 * mcomp->L_min_df +
        0.0346 * mcomp->L_min_dZ - 0.0045 * mcomp->L_max_df +
        0.0421 * mcomp->L_max_dZ + 0.0017 * mcomp->R_max_df - 1.8692;
  if (mcomp->Z <= 103.25)
    LM = LM1;
  else
    LM = LM2;
  return LM;
}
double strengthLevel(Minimum m) {
  /* implement M5' model tree */
  Minimum mcomp = getCompletedMinimum(m);
  double LM, LM1, LM2, LM3, LM4, LM5, LM6, LM7, LM8, LM9;
  LM1 = 0.0001 * mcomp->f - 0.0059 * mcomp->Z + 0.1052 * mcomp->numharm +
        0.0027 * mcomp->L_min_df + 0.0359 * mcomp->L_min_dZ +
        0.0002 * mcomp->R_min_df - 0.0260 * mcomp->R_min_dZ -
        0.0117 * mcomp->L_max_df + 0.0496 * mcomp->L_max_dZ +
        0.0012 * mcomp->R_max_df - 0.0318 * mcomp->R_max_dZ + 3.4900;
  LM2 = 0.0014 * mcomp->f - 0.0632 * mcomp->Z + 0.0397 * mcomp->numharm -
        0.0109 * mcomp->meanharmZ + 0.0020 * mcomp->L_min_df +
        0.0003 * mcomp->R_min_df + 0.0001 * mcomp->R_min_dZ +
        0.0021 * mcomp->L_max_df - 0.0059 * mcomp->L_max_dZ -
        0.0003 * mcomp->R_max_df + 7.6356;
  LM3 = 0.0014 * mcomp->f - 0.0632 * mcomp->Z + 0.0397 * mcomp->numharm -
        0.0109 * mcomp->meanharmZ + 0.0020 * mcomp->L_min_df -
        0.0001 * mcomp->R_min_df + 0.0001 * mcomp->R_min_dZ +
        0.0021 * mcomp->L_max_df - 0.0059 * mcomp->L_max_dZ -
        0.0003 * mcomp->R_max_df + 7.8619;
  LM4 = 0.0013 * mcomp->f - 0.0632 * mcomp->Z + 0.0397 * mcomp->numharm -
        0.0109 * mcomp->meanharmZ + 0.0024 * mcomp->L_min_df +
        0.0003 * mcomp->R_min_df - 0.0403 * mcomp->R_min_dZ -
        0.0010 * mcomp->L_max_df - 0.0059 * mcomp->L_max_dZ -
        0.0003 * mcomp->R_max_df + 8.8859;
  LM5 = 0.0013 * mcomp->f - 0.0632 * mcomp->Z + 0.0397 * mcomp->numharm -
        0.0109 * mcomp->meanharmZ + 0.0040 * mcomp->L_min_df +
        0.0003 * mcomp->R_min_df + 0.0127 * mcomp->R_min_dZ -
        0.0010 * mcomp->L_max_df - 0.0059 * mcomp->L_max_dZ -
        0.0003 * mcomp->R_max_df + 8.1013;
  LM6 = 0.0012 * mcomp->f - 0.0450 * mcomp->Z + 0.0149 * mcomp->numharm -
        0.0207 * mcomp->meanharmZ + 0.0007 * mcomp->L_min_df +
        0.0003 * mcomp->R_min_df - 0.0319 * mcomp->R_min_dZ -
        0.0028 * mcomp->L_max_df + 0.0122 * mcomp->L_max_dZ -
        0.0003 * mcomp->R_max_df + 9.4582;
  LM7 = 0.0015 * mcomp->f - 0.0450 * mcomp->Z + 0.0656 * mcomp->numharm -
        0.0290 * mcomp->meanharmZ + 0.0007 * mcomp->L_min_df +
        0.0003 * mcomp->R_min_df - 0.0140 * mcomp->R_min_dZ -
        0.0040 * mcomp->L_max_df + 0.0887 * mcomp->L_max_dZ -
        0.0003 * mcomp->R_max_df + 5.1755;
  LM8 = 0.0015 * mcomp->f - 0.0450 * mcomp->Z + 0.1310 * mcomp->numharm -
        0.0290 * mcomp->meanharmZ + 0.0007 * mcomp->L_min_df +
        0.0003 * mcomp->R_min_df - 0.0140 * mcomp->R_min_dZ -
        0.0040 * mcomp->L_max_df + 0.0887 * mcomp->L_max_dZ -
        0.0003 * mcomp->R_max_df + 4.8486;
  LM9 = -0.0021 * mcomp->f - 0.0450 * mcomp->Z + 0.0545 * mcomp->numharm -
        0.0290 * mcomp->meanharmZ + 0.0007 * mcomp->L_min_df +
        0.0003 * mcomp->R_min_df - 0.0140 * mcomp->R_min_dZ -
        0.0040 * mcomp->L_max_df + 0.0766 * mcomp->L_max_dZ -
        0.0003 * mcomp->R_max_df + 8.2141;
  if (mcomp->L_max_df <= 139.55)
    LM = LM1;
  else {
    if (mcomp->R_min_df <= 367.35) {
      if (mcomp->f <= 654.25) {
        if (mcomp->L_max_df <= 204.55)
          LM = LM2;
        else
          LM = LM3;
      } else {
        if (mcomp->R_min_dZ <= 10.65)
          LM = LM4;
        else
          LM = LM5;
      }
    } else {
      if (mcomp->L_max_df <= 213.9)
        LM = LM6;
      else {
        if (mcomp->L_max_dZ <= 54.6) {
          if (mcomp->numharm <= 3.5)
            LM = LM7;
          else
            LM = LM8;
        } else
          LM = LM9;
      }
    }
  }
  return LM;
}
Minimum getCompletedMinimum(Minimum m) {
  Minimum mcomp = (Minimum)malloc(sizeof(*m));
  if (isValidNum(m->f))
    mcomp->f = m->f;
  else
    mcomp->f = 1209.0;
  if (isValidNum(m->Z))
    mcomp->Z = m->Z;
  else
    mcomp->Z = 108.0;
  if (isValidNum(m->B))
    mcomp->B = m->B;
  else
    mcomp->B = 28.9;
  if (isValidNum(m->numharm))
    mcomp->numharm = m->numharm;
  else
    mcomp->numharm = 2.6;
  if (isValidNum(m->meanharmZ))
    mcomp->meanharmZ = m->meanharmZ;
  else
    mcomp->meanharmZ = 122.4;
  if (isValidNum(m->L_min_df))
    mcomp->L_min_df = m->L_min_df;
  else
    mcomp->L_min_df = 329.0;
  if (isValidNum(m->L_min_dZ))
    mcomp->L_min_dZ = m->L_min_dZ;
  else
    mcomp->L_min_dZ = -0.1;
  if (isValidNum(m->R_min_df))
    mcomp->R_min_df = m->R_min_df;
  else
    mcomp->R_min_df = 357.2;
  if (isValidNum(m->R_min_dZ))
    mcomp->R_min_dZ = m->R_min_dZ;
  else
    mcomp->R_min_dZ = 7.1;
  if (isValidNum(m->L_max_df))
    mcomp->L_max_df = m->L_max_df;
  else
    mcomp->L_max_df = 129.7;
  if (isValidNum(m->L_max_dZ))
    mcomp->L_max_dZ = m->L_max_dZ;
  else
    mcomp->L_max_dZ = 36.2;
  if (isValidNum(m->R_max_df))
    mcomp->R_max_df = m->R_max_df;
  else
    mcomp->R_max_df = 259.8;
  if (isValidNum(m->R_max_dZ))
    mcomp->R_max_dZ = m->R_max_dZ;
  else
    mcomp->R_max_dZ = 31.9;
  return mcomp;
}
/* legacy code - not used currently */
void pitchCorrection(Minimum m) {
  double f;
  double fcalc = m->f;
  if (fcalc < 350.0)
    f = fcalc * pow(2.0, 13.0 / 1200.0) * pow(fcalc / 233.0, 2.93 / 100.0);
  if (fcalc >= 350.0 && fcalc < 1700.0)
    f = fcalc * pow(2.0, 50.0 / 1200.0) * pow(fcalc / 233.0, -2.31 / 100.0);
  if (fcalc >= 1700.0)
    f = fcalc * pow(2.0, -125.0 / 1200.0) * pow(fcalc / 233.0, 2.6 / 100.0);
  m->f = f;
}
void analyseMultiphonics2(Vector playableminv) {
  int i, j;
  Minimum m1, m2;
  Vector minv;
  /* for each possible pair of playable notes */
  for (i = 0; i < sizeVector(playableminv) - 1; i++) {
    m1 = (Minimum)elementAt(playableminv, i);
    for (j = i + 1; j < sizeVector(playableminv); j++) {
      m2 = (Minimum)elementAt(playableminv, j);
      /* if the notes are not harmonic, output */
      if (!harmonic(m1, m2, playableminv)) {
        minv = createVector();
        addElement(minv, m1);
        addElement(minv, m2);
        printf("%s\t%d\t%s\t%d\t%d\t%d\t%s\n", m1->note->name, m1->note->midi,
               m2->note->name, m2->note->midi,
               noteDistance(m1, m2, playableminv), pitchIndex(minv),
               allNotes(playableminv));
      }
    }
  }
}
void analyseMultiphonics3(Vector playableminv) {
  int i, j, k;
  Minimum m1, m2, m3;
  Vector minv;
  /* for each possible trio of playable notes */
  for (i = 0; i < sizeVector(playableminv) - 2; i++) {
    m1 = (Minimum)elementAt(playableminv, i);
    for (j = i + 1; j < sizeVector(playableminv) - 1; j++) {
      m2 = (Minimum)elementAt(playableminv, j);
      for (k = j + 1; k < sizeVector(playableminv); k++) {
        m3 = (Minimum)elementAt(playableminv, k);
        /* if all pairs of the trio are not harmonic, output */
        if (!harmonic(m1, m2, playableminv) &&
            !harmonic(m2, m3, playableminv) &&
            !harmonic(m1, m3, playableminv)) {
          minv = createVector();
          addElement(minv, m1);
          addElement(minv, m2);
          addElement(minv, m3);
          printf("%s\t%d\t%s\t%d\t%s\t%d\t%d\t%d\t%s\n", m1->note->name,
                 m1->note->midi, m2->note->name, m2->note->midi, m3->note->name,
                 m3->note->midi, noteDistance(m1, m3, playableminv),
                 pitchIndex(minv), allNotes(playableminv));
        }
      }
    }
  }
}
int noteDistance(Minimum m1, Minimum m2, Vector playableminv) {
  int i;
  Minimum m;
  int posm1 = -1, posm2 = -1;
  /* find the positions of each minimum in vector */
  for (i = 0; i < sizeVector(playableminv); i++) {
    m = (Minimum)elementAt(playableminv, i);
    if (m == m1)
      posm1 = i;
    if (m == m2)
      posm2 = i;
  }
  /* check for an invalid condition */
  if ((posm1 == -1) || (posm2 == -1) || (posm1 >= posm2))
    return -1;
  else
    return (posm2 - posm1);
}
int pitchIndex(Vector minv) {
  int i;
  Minimum m;
  int index = 0;
  /* calculate sum of squares of cents */
  for (i = 0; i < sizeVector(minv); i++) {
    m = (Minimum)elementAt(minv, i);
    index = index + (m->note->cents) * (m->note->cents);
  }
  return index;
}
char *allNotes(Vector playableminv) {
  int i;
  Minimum m;
  char *all_notes = (char *)malloc(BUFSIZ * sizeof(char));
  /* for each note that is playable, concatenate into
  one string delimited by a ';' */
  for (i = 0; i < sizeVector(playableminv); i++) {
    m = (Minimum)elementAt(playableminv, i);
    if (i == 0)
      sprintf(all_notes, noteString(m->note));
    else {
      strcat(all_notes, ";");
      strcat(all_notes, noteString(m->note));
    }
  }
  /* return concatenated string */
  return all_notes;
}
int harmonic(Minimum m1, Minimum m2, Vector playableminv) {
  int i;
  Minimum m;
  int n = sizeVector(playableminv);
  int midis[n];
  int dmidi;
  int num_harmonic_midis = 11;
  int harmonic_midis[] = {0, 12, 19, 24, 28, 31, 33, 34, 36, 38, 40};
  /* get the midi numbers of all playable minima. The difference in
  midi number of two notes is the number of semitones between
  them. */
  for (i = 0; i < n; i++) {
    m = (Minimum)elementAt(playableminv, i);
    midis[i] = m->note->midi;
  }
  /* calculate number of semitones between two minima */
  dmidi = m2->note->midi - m1->note->midi;
  /* if the minima are directly harmonically related (integer
  frequency ratio) */
  if (inArray(harmonic_midis, num_harmonic_midis, dmidi))
    return 1;
  /* if the minima have a common playable fundamental */
  if ((dmidi == 7 || dmidi == 16 || dmidi == 21 || dmidi == 22 ||
       dmidi == 26) &&
      inArray(midis, n, m1->note->midi - 12))
    return 1;
  if ((dmidi == 5 || dmidi == 9 || dmidi == 14 || dmidi == 15 || dmidi == 17 ||
       dmidi == 21) &&
      inArray(midis, n, m1->note->midi - 19))
    return 1;
  if ((dmidi == 4 || dmidi == 7 || dmidi == 9 || dmidi == 10 || dmidi == 14 ||
       dmidi == 16) &&
      inArray(midis, n, m1->note->midi - 24))
    return 1;
  if ((dmidi == 3 || dmidi == 5 || dmidi == 6 || dmidi == 8 || dmidi == 10) &&
      inArray(midis, n, m1->note->midi - 28))
    return 1;
  if ((dmidi == 2 || dmidi == 3 || dmidi == 5 || dmidi == 7 || dmidi == 9) &&
      inArray(midis, n, m1->note->midi - 31))
    return 1;
  if ((dmidi == 3 || dmidi == 5 || dmidi == 7) &&
      inArray(midis, n, m1->note->midi - 33))
    return 1;
  if ((dmidi == 2 || dmidi == 4 || dmidi == 6) &&
      inArray(midis, n, m1->note->midi - 34))
    return 1;
  if ((dmidi == 2 || dmidi == 4) && inArray(midis, n, m1->note->midi - 36))
    return 1;
  if (dmidi == 2 && inArray(midis, n, m1->note->midi - 38))
    return 1;
  /* otherwise, not harmonic */
  return 0;
}
int inArray(int *array, int size, int num) {
  int i;
  /* look for num in array */
  for (i = 0; i < size; i++) {
    if (array[i] == num)
      return 1;
  }
  return 0;
}
