#include "../core/array.h"
#include "../core/asciiGridParse.h"
#include "../core/findpeak.h"
#include "../core/reduce.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  // For Testing purposes
  double nodata = -9.0;
  int nval = 9;
  double val[] = {1, 5, 3, nodata, 6, 2, 7, 0, 8};
  int m = 7, n = 11;
  int h = 3;
  double R = h * sqrt(2);
  double_array x = createdoublearray(m, n);
  int margin = 0;

  // Filling array with test values
  int i, j;
  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++) {
      *(i * n + j + x.val) = val[(i * n + j) % (nval)];
    }
  }
  *(x.val + 15) = 11;

  for (i = 0; i < m; i++) {
    printf("\n");
    if (i % h == 0) {
      printf("\n\n");
    }
    for (j = 0; j < n; j++) {
      if (j % h == 0) {
        printf("  ");
      }
      printf("   %d,", (int)*(i * n + j + x.val));
    }
  }
  printf("\n \n");

  /* R is in index units */
  int_array *j_r, *i_r;
  double_array *xr;
  // !!!! Don't forget to do the double free !!!!
  i_r = malloc(sizeof(int_array));
  j_r = malloc(sizeof(int_array));
  xr = malloc(sizeof(double_array));

  findpeak(x, R, margin, nodata, xr, i_r, j_r);

  // Checking for errors
  int im, jm;
  double vm;
  for (i = 0; i < (*xr).n; i++) {
    if (i > 4) {
      printf("\n");
    }
    im = *(i + (*i_r).val);
    jm = *(i + (*j_r).val);
    vm = *(i + (*xr).val);
    printf("  (%d, %d, %lf) ", im, jm, vm);
    if (vm == nodata) {
      printf(" \n nodata ERROR \n");
    }
  }
  printf("\n  number of max found %d \n", (*i_r).n);

  freearray(x.val);
  freearray((*xr).val);
  freearray((*i_r).val);
  freearray((*j_r).val);
  free(i_r);
  free(j_r);
  free(xr);
  checkmem();
  printf("\n\n");
  return 0;
}
