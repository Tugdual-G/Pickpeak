#include "parseArgs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse(int argc, char *argv[], struct Param *param) {
  if (argc < 7) {
    printf("\n Please provide enougth arguments \n\n");
    exit(1);
  } else if (argc > 6 + LENFNAME) {
    printf(
        "\n ERROR Too many arguments. NOTE: max number of input files = " STR(
            NFILES) " \n");
    exit(1);
  }
  (*param).nin = 0;
  char a = '-';
  int i = 1;
  char mar = 0, rad = 0, in = 0, out = 0;

  while (!(rad && in && out) & (i < argc - 1)) {
    if ((strcoll(argv[i], "--margin") == 0 || strcoll(argv[i], "-m") == 0) &
        (mar == 0)) {
      i++;
      if (sscanf(argv[i], "%1d", &(*param).margin) == 1) {
        i++;
      } else {
        printf("\n   ARGUMENR PARSING ERROR : margin \n");
        exit(1);
      }
    } else if ((in == 0) && (strcoll(argv[i], "--infile") == 0 ||
                             strcoll(argv[i], "-i") == 0)) {
      i++;
      while (!(*argv[i] == a) && i < argc) {
        if (sscanf(argv[i], "%" STR(LENFNAME) "s",
                   (*param).infile[(*param).nin]) == 1) {
          in = 1;
          i++;
          (*param).nin++;
          if (i >= NFILES) {
            printf(
                "\n  WARNING number of input files exceed " STR(NFILES) " \n");
          }
        } else {
          printf("\n  ARGUMENR PARSING ERROR : infile \n");
          exit(1);
        }
      }
    } else if ((out == 0) && (strcoll(argv[i], "--outfile") == 0 ||
                              strcoll(argv[i], "-o") == 0)) {
      i++;
      if (sscanf(argv[i], "%" STR(LENFNAME) "s", (*param).outfile) == 1) {
        out = 1;
        i++;
      } else {
        printf("\n  ARGUMENR PARSING ERROR : outfile \n");
        exit(1);
      }
    } else if ((rad == 0) && (strcoll(argv[i], "--radius") == 0 ||
                              strcoll(argv[i], "-R") == 0)) {
      i++;
      if (sscanf(argv[i], "%lf", &(*param).R) == 1) {
        rad = 1;
        i++;
      } else {
        printf("\n  ARGUMENR PARSING ERROR : radius \n");
        exit(1);
      }
    } else {
      i++;
    }
  }
  if (!(out && in && rad)) {
    printf("\n  ARGUMENR PARSING ERROR  help : --help \n");
    exit(1);
  }
}
