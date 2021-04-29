/*
AnalyseNotes.c
By Andrew Botros, 2004
Modified by Paul Dickens, 2006
Analyses acoustic impedance spectra for playable notes.
*/
#include "Analysis.h"
#include <stdio.h>
#include <string.h>
int parseCommandLine(int argc, char **argv, int *displayharmonicity,
                     char **filename);
int main(int argc, char **argv) {
  int displayharmonicity;
  /* do not apply the pitch correction */
  int applypitchcorrection = 0;
  char *filename;
  /* check correct usage */
  if (!parseCommandLine(argc, argv, &displayharmonicity, &filename)) {
    fprintf(stdout, "Usage: AnalyseNotes [OPTIONS] <filename>\n\n");
    fprintf(stderr, " Options:\n");
    fprintf(stderr, "\t-h (Displays harmonicity data)\n\n");
    return -1;
  }
  /* perform analysis on impedance data file */
  Analysis(filename, applypitchcorrection, displayharmonicity, NOTES);
  return 0;
}
int parseCommandLine(int argc, char **argv, int *displayharmonicity,
                     char **filename) {
  int i;
  int hflag = 0;
  int numoptions = 1, numinputfiles = 1;
  int minargc = 1 + numinputfiles;
  int maxargc = minargc + numoptions;
  /* Check correct number of parameters */
  if ((argc < minargc) || (argc > maxargc))
    return 0;
  /* Set default options */
  *displayharmonicity = 0;
  /* Check and set options */
  for (i = 1; i < (argc - numinputfiles); i++) {
    if (strcmp(argv[i], "-h") == 0) {
      if (hflag)
        return 0;
      *displayharmonicity = 1;
      hflag = 1;
      continue;
    }
    /* else invalid option */
    fprintf(stderr, "Invalid option: %s\n", argv[i]);
    return 0;
  }
  /* Set filename */
  *filename = argv[argc - numinputfiles];
  return 1;
}
