#include "parseArgs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Arglength 257

void parse(int argc, char *argv[], struct Param *param) {
  char args[Arglength];
  int i;
  if (argc < 7) {
    printf("\n Please provide enougth arguments \n\n");
    exit(1);
  }
  for (i = 1; i < argc; i++) {
    strncat(args, argv[i], Arglength - 1);
  }
  char mar = 0, in = 0, out = 0, rad = 0;
  for (i = 0; i < argc; i++) {
    if ((strcoll(argv[i], "--margin") == 0 || strcoll(argv[i], "-m") == 0) &
        (mar == 0)) {
      if (sscanf(argv[i + 1], "%u", &(*param).margin) == 1) {

      } else {
        printf("\n   ARGUMENR PARSING ERROR : margin \n");
        exit(1);
      }
      mar = 1;
    }
    if ((in == 0) &&
        (strcoll(argv[i], "--infile") == 0 || strcoll(argv[i], "-i") == 0)) {
      printf("in");
      if (sscanf(argv[i + 1], "%s", (*param).infile) == 1) {
      } else {
        printf("\n  ARGUMENR PARSING ERROR : infile \n");
        exit(1);
      }
      in = 1;
    }
    if ((out == 0) &&
        (strcoll(argv[i], "--outfile") == 0 || strcoll(argv[i], "-o") == 0)) {
      printf("out");
      if (sscanf(argv[i + 1], "%250s", (*param).outfile) == 1) {
      } else {
        printf("\n  ARGUMENR PARSING ERROR : outfile \n");
        exit(1);
      }
      out = 1;
    }
    if ((rad == 0) && (strcoll(argv[i], "-R") == 0)) {
      if (sscanf(argv[i + 1], "%250lf", &(*param).R) == 1) {
      } else {
        printf("\n  ARGUMENR PARSING ERROR : R \n");
        exit(1);
      }
      rad = 1;
    }
  }
  if (!(out && in && rad)) {
    printf("\n  ARGUMENR PARSING ERROR  help : --help \n");
    exit(1);
  }
}
