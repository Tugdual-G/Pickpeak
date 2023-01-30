#ifndef PARSEARGS_H_
#define PARSEARGS_H_
#define Arglength 257

typedef struct Param Param;

struct Param {
  int margin;
  char infile[4 * Arglength];
  char outfile[Arglength];
  double R;
};

void parse(int argc, char *argv[], struct Param *param);
#endif // PARSEARGS_H_
