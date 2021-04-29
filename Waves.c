/*
Waves.c
By Paul Dickens, 2005, 2006
Calculates the pressure and flow distribution along a flute.
*/
#include "Acoustics.h"
#include "ParseXML.h"
#include "Woodwind.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
int parseCommandLine(int argc, char **argv, char **holestring, double *xres,
                     int *midi, double *f, char **xml_filename);
/* Default x resolution */
#define XRES 2.0e-3;
int main(int argc, char **argv) {
  double f;
  double x, xres, xmin, xmax;
  int midi;
  char *holestring;
  char *xml_filename;
  Woodwind instrument;
  complex Z0, Zin, pin, Uin;
  double c;
  TransferMatrix m;
  complex p, U;
  double entryradius = WW_EMB_RADIUS;
  /* check correct usage */
  if (!parseCommandLine(argc, argv, &holestring, &xres, &midi, &f,
                        &xml_filename)) {
    fprintf(stderr, "Usage: Waves [OPTIONS] <midi> <frequency> <XML file>\n\n");
    fprintf(stderr, " Options:\n");
    fprintf(stderr, "\t-s <holestring>\n");
    fprintf(stderr, "\t-r <xres> (default 2.0)\n\n");
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
    fprintf(stderr, "Waves error: Waves failed to parse XML file.\n");
    return -1;
  }
  discretiseWoodwind(instrument, WW_MAX_LENGTH);
  setAirProperties(instrument, WW_T_0, WW_T_AMB, WW_T_GRAD, WW_HUMID, WW_X_CO2);
  /* set fingering from holestring and validate */
  if (!setFingering(instrument, holestring)) {
    fprintf(stderr, "Waves error: \"%s\" ", holestring);
    fprintf(stderr,
            "is an invalid fingering for the given woodwind definition.\n");
    return -1;
  }
  /* calculate Zin, c and Z0 */
  Zin = playedImpedance(f, instrument, midi);
  c = instrument->head->embouchureHole->c;
  Z0 = charZ(c, instrument->head->embouchureHole->rho, entryradius);
  /* calculate pin and Uin */
  Uin = real(1 / sqrt(modz(Zin) * modz(Zin) + modz(Z0) * modz(Z0)));
  // Uin = real(sqrt(2 * modz(Z0) * c / (modz(Z0)*modz(Z0)
  // + modz(Zin)*modz(Zin))));
  pin = multz(Zin, Uin);
  /* change pin to account for face impedance */
  pin = subz(pin, multz(faceZ(f, instrument->head, midi), Uin));
  /* calculate the limits of the instrument */
  xmin = ceil(-woodwindLengthNeg(instrument) / xres) * xres;
  xmax = ceil(woodwindLengthPos(instrument) / xres) * xres;
  for (x = xmin; x < xmax; x += xres) {
    m = woodwindMatrix(f, instrument,
                       entryradius / woodwindEntryRadius(instrument), x);
    invertm(m);
    p = addz(multz(m->A, pin), multz(m->B, Uin));
    U = addz(multz(m->C, pin), multz(m->D, Uin));
    // getZ0_c(instrument, x, &Z0, &c);
    printf("%.1f\t%.3f\t%.3f\n", x * 1e3, modz(p), modz(Z0) * modz(U));
  }
  return 0;
}
int parseCommandLine(int argc, char **argv, char **holestring, double *xres,
                     int *midi, double *f, char **xml_filename) {
  int i;
  int sflag = 0, rflag = 0;
  int numoptions = 2, numrequired = 3;
  int minargc = 1 + numrequired;
  int maxargc = minargc + 2 * numoptions;
  /* Check correct number of parameters */
  if ((argc < minargc) || (argc % 2 != minargc % 2) || (argc > maxargc))
    /* Set default options */
    *holestring = NULL;
  *xres = XRES;
  /* Check and set options */
  for (i = 1; i < (argc - numrequired); i += 2) {
    if (strcmp(argv[i], "-s") == 0) {
      if (sflag)
        return 0;
      *holestring = argv[i + 1];
      sflag = 1;
      continue;
    }
    if (strcmp(argv[i], "-r") == 0) {
      if (rflag)
        return 0;
      *xres = atof(argv[i + 1]);
      if (*xres <= 0.0) {
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
  /* Set midi */
  *midi = atof(argv[argc - numrequired]);
  /* Set f */
  *f = atof(argv[argc - numrequired + 1]);
  if (*f <= 0.0) {
    fprintf(stderr, "Invalid frequency\n");
    return 0;
  }
  /* Set XML filename */
  *xml_filename = argv[argc - numrequired + 2];
  return 1;
}
