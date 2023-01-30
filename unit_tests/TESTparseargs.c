#include "../core/parseArgs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *(argv[])) {
  Param param = {.margin = 0};
  parse(argc, argv, &param);
  printf("\n margin :%d \n", param.margin);
  printf(" R :%lf \n", param.R);
  printf(" in :%s \n", param.infile);
  printf(" out :%s \n", param.outfile);
}
