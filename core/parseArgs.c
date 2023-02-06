/*
** This module takes care of the command line interface, by
** parsing the arguments and options.
*/
#include "parseArgs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* macro to stringify constants */
#define STR1(x) #x
#define STR(x) STR1(x)

static void check_then_parse(char argv[], char argname[], char format[],
                             void *param);

static void print_help(char filename[]);

void parse(int argc, char *argv[], struct Param *param) {
  /*
  ** This is the command-line argument parsing function for pickpeak
  */

  if (argc < 2) {
    printf("\n Please provide enougth arguments \n\n");
    print_help("core/help.txt");
    exit(1);
  } else if (argc > 6 + MAXLENFNAME) {
    printf(
        "\n ERROR Too many arguments. NOTE: max number of input files = " STR(
            NFILES) " \n");
    exit(1);
  }

  /* Parameter to hold the number of input files */
  (*param).nin = 0;

  /* hold the input file names and path before we can know their lengths*/
  char temp_fname[MAXLENFNAME + 1] = {'\0'};

  /* The actual length of the input file name */
  int len_fname;

  /* flags to check if an argument has already been parsed */
  char mar = 0, rad = 0, in = 0, out = 0;

  int i = 1;
  while (i < argc) {
    if (!mar &&
        (strcoll(argv[i], "--margin") == 0 || strcoll(argv[i], "-m") == 0)) {
      ++i;
      check_then_parse(argv[i], "margin", "1d", &(*param).margin);
      ++i;
      mar = 1;
      continue;
    }
    if ((!in) &&
        (strcoll(argv[i], "--infile") == 0 || strcoll(argv[i], "-i") == 0)) {
      ++i;

      /* Retriving all input files names */
      while ((!(*argv[i] == '-')) && (i < argc)) {

        check_then_parse(argv[i], "input file", STR(MAXLENFNAME) "s",
                         temp_fname);

        len_fname = strlen(temp_fname);
        if (len_fname > MAXLENFNAME) {
          printf("\n ERROR: the input file name is too long.\n"
                 "maximum lenght = %u \n",
                 MAXLENFNAME);
          exit(1);
        }

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
        ++i;
        ++(*param).nin;
      }
      continue;
    }
    if (!out &&
        (strcoll(argv[i], "--outfile") == 0 || strcoll(argv[i], "-o") == 0)) {
      ++i;
      check_then_parse(argv[i], "output file", STR(MAXLENFNAME) "s",
                       &(*param).outfile);
      out = 1;
      ++i;
      continue;
    }
    if (!rad &&
        (strcoll(argv[i], "--radius") == 0 || strcoll(argv[i], "-R") == 0)) {
      ++i;
      check_then_parse(argv[i], "radius", "lf", &(*param).R);
      rad = 1;
      ++i;
      continue;
    }

    if (strcoll(argv[i], "--help") == 0) {
      print_help("core/help.txt");
      exit(1);
    }

    ++i;
  }
  if (!(out && in && rad)) {
    printf("\n  ARGUMENR PARSING ERROR  help : --help \n");
    exit(1);
  }
}

static void check_then_parse(char argv[], char argname[], char format[],
                             void *param) {

  char format_str[20] = {'\0'};
  snprintf(format_str, 19, "%%%s", format);
  if (sscanf(argv, format_str, param) == 1) {
  } else {
    printf("\n ARGUMENR PARSING ERROR : %s \n", argname);
    exit(1);
  }
}

static void print_help(char filename[]) {
  FILE *fp = NULL;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("\n ERROR cannot read help text file \n");
    exit(1);
  }
  char line[256] = {'\0'};
  while (fgets(line, 255, fp) != NULL) {
    fputs(line, stdout);
  }
  fclose(fp);
}
