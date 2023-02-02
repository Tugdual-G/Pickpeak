#ifndef PARSEARGS_H_
#define PARSEARGS_H_
#define STR1(x) #x
#define STR(x) STR1(x)

#define LENFNAME 512
#define NFILES 50

typedef struct Param Param;

struct Param {
  int margin;
  char *infile[NFILES];
  int nin;
  char outfile[LENFNAME + 1];
  double R;
};

void parse(int argc, char *argv[], struct Param *param);
#endif // PARSEARGS_H_
