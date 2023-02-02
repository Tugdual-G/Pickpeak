#include "parseArgs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse(int argc, char *argv[], struct Param *param) {
  /*
  ** This is the command-line argument parsing function for pickpeak
  */

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
  char arg_symbol = '-';
  char temp_fname[LENFNAME + 1];
  temp_fname[LENFNAME] = '\0';
  int len_fname;
  int i = 1;
  char mar = 0, rad = 0, in = 0, out = 0;

  while (i < argc - 1) {
    if ((strcoll(argv[i], "--margin") == 0 || strcoll(argv[i], "-m") == 0) &
        !mar) {
      i++;
      if (sscanf(argv[i], "%1d", &(*param).margin) == 1) {
        i++;
      } else {
        printf("\n ARGUMENR PARSING ERROR : margin \n");
        exit(1);
      }
    } else if ((in == 0) && (strcoll(argv[i], "--infile") == 0 ||
                             strcoll(argv[i], "-i") == 0)) {
      i++;
      while ((!(*argv[i] == arg_symbol)) && (i < argc)) {
        /* Retriving all input files names */

        if (sscanf(argv[i], "%" STR(LENFNAME) "s", temp_fname) == 1) {

          len_fname = strnlen(temp_fname, LENFNAME);

          (*param).infile[(*param).nin] = NULL;
          (*param).infile[(*param).nin] =
              (char *)malloc(sizeof(char) * (len_fname + 1));
          if ((*param).infile[(*param).nin] == NULL) {
            printf("\n allocation ERROR \n");
            exit(1);
          }
          strncpy((*param).infile[(*param).nin], temp_fname, len_fname + 1);
          (*param).infile[(*param).nin][len_fname] = '\0';
          in = 1;
          i++;
          (*param).nin++;
        } else {
          printf("\n ARGUMENR PARSING ERROR : infile \n");
          exit(1);
        }
      }
    } else if (!out && (strcoll(argv[i], "--outfile") == 0 ||
                        strcoll(argv[i], "-o") == 0)) {
      i++;
      if (sscanf(argv[i], "%" STR(LENFNAME) "s", (*param).outfile) == 1) {
        out = 1;
        i++;
      } else {
        printf("\n ARGUMENR PARSING ERROR : outfile \n");
        exit(1);
      }
    } else if ((rad == 0) && (strcoll(argv[i], "--radius") == 0 ||
                              strcoll(argv[i], "-R") == 0)) {
      i++;
      if (sscanf(argv[i], "%lf", &(*param).R) == 1) {
        rad = 1;
        i++;
      } else {
        printf("\n ARGUMENR PARSING ERROR : radius \n");
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
