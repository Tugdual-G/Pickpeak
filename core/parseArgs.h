/*
** This module takes care of the command line interface, by
** parsing the arguments and options.
*/
#ifndef PARSEARGS_H_
#define PARSEARGS_H_

#define LENFNAME 512
#define NFILES 50

typedef struct Param Param;

/*
** This is the command-line argument parsing function for pickpeak
*/
void parse(int argc, char *argv[], struct Param *param);

struct Param {
  int margin;
  char *infile[NFILES];
  int nin;
  char outfile[LENFNAME + 1];
  double R;
};
#endif // PARSEARGS_H_
