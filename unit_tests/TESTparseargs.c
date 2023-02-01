#include "../core/parseArgs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *(argv[])) {
  Param param = {.margin = 0};
  parse(argc, argv, &param);
  printf("\n m: %d  ", param.margin);
  int i;
  printf(", in : ");
  for (i = 0; i < param.nin; i++) {
    printf("%s  ", param.infile[i]);
  }
  printf(" m: %s \n", param.outfile);
  return 0;
}
