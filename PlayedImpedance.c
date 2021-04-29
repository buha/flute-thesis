/*
PlayedImpedance.c
By Paul Dickens, 2006
Physical model of the acoustic impedance of a played flute.
*/
#include "ParseXML.h"
#include "Vector.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int parseCommandLine(int argc, char **argv, double *flo, double *fhi,
                     double *fres, char **input_filename, char **xml_filename);
int parseInputFile(Vector midiv, Vector holestringv, char *input_filename);
/* Default spectrum range and resolution */
#define FLO 200.0
#define FHI 4000.0
#define FRES 2.0
int main(int argc, char **argv) {
  double f, flo, fhi, fres;
  char *input_filename;
  char *xml_filename;
  Vector midiv = createVector();
  Vector holestringv = createVector();
  Woodwind instrument;
  int i;
  int midi;
  char *holestring;
  double z_dB;
  /* check correct usage */
  if (!parseCommandLine(argc, argv, &flo, &fhi, &fres, &input_filename,
                        &xml_filename)) {
    fprintf(stderr,
            "Usage: PlayedImpedance [OPTIONS] <input file> <XML file>\n\n");
    fprintf(stderr, " Options:\n");
    fprintf(stderr, "\t-l <flo> (default 200.0)\n");
    fprintf(stderr, "\t-h <fhi> (default 4000.0)\n");
    fprintf(stderr, "\t-r <fres> (default 2.0)\n\n");
    fprintf(stderr, " <input file>:\n");
    fprintf(stderr, "\t- Must be a tab-delimited list of midi numbers and\n");
    fprintf(stderr, "\t holestrings, one set per line.\n\n");
    return -1;
  }
  /* parse input file */
  if (!parseInputFile(midiv, holestringv, input_filename)) {
    fprintf(stderr, "PlayedImpedance error: ");
    fprintf(stderr, "PlayedImpedance failed to parse input file.\n");
    return -1;
  }
  /* retrieve data structures from XML file */
  if (!parseXMLFile(xml_filename, &instrument)) {
    fprintf(stderr, "PlayedImpedance error: ");
    fprintf(stderr, "PlayedImpedance failed to parse XML file.\n");
    return -1;
  }
  discretiseWoodwind(instrument, WW_MAX_LENGTH);
  setAirProperties(instrument, WW_T_0, WW_T_AMB, WW_T_GRAD, WW_HUMID, WW_X_CO2);
  /* print the midi numbers as column labels */
  for (i = 0; i < sizeVector(midiv); i++) {
    /* set midi from vector */
    midi = atoi((char *)elementAt(midiv, i));
    printf("\t%.d", midi);
  }
  printf("\n");
  /* for each frequency in spectrum range... */
  for (f = flo; f <= fhi; f += fres) {
    printf("%.2f", f);
    /* for each fingering... */
    for (i = 0; i < sizeVector(midiv); i++) {
      /* print tab delimiter */
      printf("\t");
      /* set midi and holestring from vectors */
      midi = atoi((char *)elementAt(midiv, i));
      holestring = (char *)elementAt(holestringv, i);
      /* set and validate fingering */
      if (!setFingering(instrument, holestring)) {
        fprintf(stderr, "PlayedImpedance error: \"%s\" ",
                (char *)elementAt(holestringv, i));
        fprintf(stderr, "is an invalid fingering for the given woodwind ");
        fprintf(stderr, "definition.\n");
        return -1;
      }
      /* calculate and output impedance */
      z_dB = 20.0 * log10(modz(playedImpedance(f, instrument, midi)));
      printf("%.3f", z_dB);
    }
    /* print new line */
    printf("\n");
  }
  return 0;
}
int parseCommandLine(int argc, char **argv, double *flo, double *fhi,
                     double *fres, char **input_filename, char **xml_filename) {
  int i;
  double d;
  int lflag = 0, hflag = 0, rflag = 0;
  int numoptions = 3, numinputfiles = 2;
  int minargc = 1 + numinputfiles;
  int maxargc = minargc + 2 * numoptions;
  /* Check correct number of parameters */
  if ((argc < minargc) || (argc % 2 != minargc % 2) || (argc > maxargc))
    return 0;
  /* Set default options */
  *flo = FLO;
  *fhi = FHI;
  *fres = FRES;
  /* Check and set options */
  for (i = 1; i < (argc - numinputfiles); i += 2) {
    if (strcmp(argv[i], "-l") == 0) {
      if (lflag)
        return 0;
      *flo = atof(argv[i + 1]);
      if (*flo <= 0.0) {
        fprintf(stderr, "Invalid -l option\n");
        return 0;
      }
      lflag = 1;
      continue;
    }
    if (strcmp(argv[i], "-h") == 0) {
      if (hflag)
        return 0;
      *fhi = atof(argv[i + 1]);
      if (*fhi <= 0.0) {
        fprintf(stderr, "Invalid -h option\n");
        return 0;
      }
      hflag = 1;
      continue;
    }
    if (strcmp(argv[i], "-r") == 0) {
      if (rflag)
        return 0;
      *fres = atof(argv[i + 1]);
      if (*fres <= 0.0) {
        fprintf(stderr, "Invalid -r option\n");
        return 0;
      }
      rflag = 1;
      continue;
    }
    /* else invalid option */
    fprintf(stderr, "Invalid option: %s\n", argv[i]);
    return 0;
  }
  /* Swap frequency low and high values if inverted */
  if ((*flo > *fhi) && (*flo != 0.0) && (*fhi != 0.0)) {
    d = *flo;
    *flo = *fhi;
    *fhi = d;
  }
  /* Set input filename and XML filename */
  *input_filename = argv[argc - numinputfiles];
  *xml_filename = argv[argc - numinputfiles + 1];
  return 1;
}
int parseInputFile(Vector midiv, Vector holestringv, char *input_filename) {
  FILE *fp;
  char *line;
  char *delimiters = "\t\n";
  char *token;
  /* open input file */
  if ((fp = fopen(input_filename, "r")) == NULL)
    return 0;
  /* add each line (without newline) to hole string vector */
  while (1) {
    line = (char *)malloc(BUFSIZ * sizeof(char));
    if (fgets(line, BUFSIZ, fp) == NULL)
      break;
    token = strtok(line, delimiters);
    addElement(midiv, token);
    token = strtok(NULL, delimiters);
    addElement(holestringv, token);
  }
  return 1;
}
