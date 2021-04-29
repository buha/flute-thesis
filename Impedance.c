/*
Impedance.c
By Paul Dickens, 2005
Physical model of the acoustic impedance of a flute.
*/
#include "ParseXML.h"
#include "Woodwind.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int parseCommandLine(int argc, char **argv, char **holestring, double *temp,
                     double *humid, double *flo, double *fhi, double *fres,
                     double *entryratio, char **xml_filename);
/* Default parameter values */
#define TEMP 25.0
#define HUMID 0.5
#define X_CO2 0.0004
#define FLO 200.0
#define FHI 4000.0
#define FRES 2.0
#define ENTRYRATIO 1.0
int main(int argc, char **argv) {
  char *holestring;
  double temp, humid;
  double f, flo, fhi, fres, entryratio;
  char *xml_filename;
  Woodwind instrument;
  complex Z;
  /* check correct usage */
  if (!parseCommandLine(argc, argv, &holestring, &temp, &humid, &flo, &fhi,
                        &fres, &entryratio, &xml_filename)) {
    fprintf(stderr, "Usage: Impedance [OPTIONS] <XML file>\n\n");
    fprintf(stderr, " Options:\n");
    fprintf(stderr, "\t-s <holestring>\n");
    fprintf(stderr, "\t-t <temperature> (default 25.0 degC)\n");
    fprintf(stderr, "\t-u <humidity> (default 0.5)\n");
    fprintf(stderr, "\t-l <flo> (default 100.0)\n");
    fprintf(stderr, "\t-h <fhi> (default 4000.0)\n");
    fprintf(stderr, "\t-r <fres> (default 2.0)\n");
    fprintf(stderr, "\t-e <entryratio> (default 1.0)\n\n");
    fprintf(stderr, " <holestring>:\n");
    fprintf(stderr, "\t- Optional if no holes are defined in XML file.\n");
    fprintf(stderr, "\t- Must be a sequence of 'O' (open hole) ");
    fprintf(stderr, "and 'X' (closed hole) characters.\n");
    fprintf(stderr,
            "\t- Length must be equal to the number of defined holes.\n");
    fprintf(stderr, "\t- e.g. \"XXOOOOOOXOOOOXOOO\"\n\n");
    return -1;
  }
  /* retrieve data structures from XML file */
  if (!parseXMLFile(xml_filename, &instrument)) {
    fprintf(stderr, "Impedance error: Impedance failed to parse XML file.\n");
    return -1;
  }
  /* set the air properties (speed of sound, density) for each
  segment */
  setAirProperties(instrument, temp, temp, 0, humid, X_CO2);
  /* set fingering from holestring and validate */
  if (!setFingering(instrument, holestring)) {
    fprintf(stderr, "Impedance error: \"%s\" ", holestring);
    fprintf(stderr,
            "is an invalid fingering for the given woodwind definition.\n");
    return -1;
  }
  /* for each frequency in spectrum range... */
  for (f = flo; f <= fhi; f += fres) {
    /* calculate impedance */
    Z = impedance(f, instrument, entryratio);
    /* print output */
    printf("%e\t%e\t%e\n", f, Z.Re, Z.Im);
  }
  return 0;
}
int parseCommandLine(int argc, char **argv, char **holestring, double *temp,
                     double *humid, double *flo, double *fhi, double *fres,
                     double *entryratio, char **xml_filename) {
  int i;
  double d;
  int sflag = 0, tflag = 0, uflag = 0, lflag = 0, hflag = 0, rflag = 0,
      eflag = 0;
  int numoptions = 7, numinputfiles = 1;
  int minargc = 1 + numinputfiles;
  int maxargc = minargc + 2 * numoptions;
  /* Check correct number of parameters */
  if ((argc < minargc) || (argc % 2 != minargc % 2) || (argc > maxargc))
    return 0;
  /* Set default options */
  *holestring = NULL;
  *temp = TEMP;
  *humid = HUMID;
  *flo = FLO;
  *fhi = FHI;
  *fres = FRES;
  *entryratio = ENTRYRATIO;
  /* Check and set options */
  for (i = 1; i < (argc - numinputfiles); i += 2) {
    if (strcmp(argv[i], "-s") == 0) {
      if (sflag)
        return 0;
      *holestring = argv[i + 1];
      sflag = 1;
      continue;
    }
    if (strcmp(argv[i], "-t") == 0) {
      if (tflag)
        return 0;
      *temp = atof(argv[i + 1]);
      if ((*temp < 0.0) || (*temp > 30.0)) {
        fprintf(stderr, "Invalid -t option\n");
        return 0;
      }
      tflag = 1;
      continue;
    }
    if (strcmp(argv[i], "-u") == 0) {
      if (uflag)
        return 0;
      *humid = atof(argv[i + 1]);
      if ((*humid < 0.0) || (*humid > 1.0)) {
        fprintf(stderr, "Invalid -u option\n");
        return 0;
      }
      uflag = 1;
      continue;
    }
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
    if (strcmp(argv[i], "-e") == 0) {
      if (eflag)
        return 0;
      *entryratio = atof(argv[i + 1]);
      if ((*entryratio <= 0.0) || (*entryratio > 1.0)) {
        fprintf(stderr, "Invalid -e option\n");
        return 0;
      }
      eflag = 1;
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
  /* Set XML filename */
  *xml_filename = argv[argc - numinputfiles];
  return 1;
}
