#include "../core/array.h"
#include "../core/asciiGridParse.h"
#include "../core/reduce.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  // For Testing purposes

  double nodata = -9.0;
  int nval = 9;
  double val[] = {1, 5, 3, nodata, 6, 2, 7, 0, 9};
  int m = 7, n = 11, h = 2;
  // double R = h * sqrt(2);
  double_array x = createdoublearray(m, n);

  // Filling array with test values
  int i, j;
  for (i = 0; i < m; i++) {
    printf("\n");
    if (i % h == 0) {
      printf("\n\n");
    }
    for (j = 0; j < n; j++) {

      if (j % h == 0) {
        printf("  ");
      }

      *(i * n + j + x.val) = val[(i * n + j) % (nval)];
      printf("   %d,", (int)*(i * n + j + x.val));
    }
  }
  printf("\n \n");

  /* R is in index units */
  int m_o, n_o; // size of the reduction output
  if (m % h == 0) {
    m_o = m / h;
  } else {
    m_o = m / h + 1;
  }
  if (n % h == 0) {
    n_o = n / h;
  } else {
    n_o = n / h + 1;
  }

  printf("\n Step : %d \n", h);
  printf(" input format (%d, %d) \n", m, n);
  printf(" output format (%d, %d) \n", m_o, n_o);

  int_array ir = createintarray(m_o, n_o), jr = createintarray(m_o, n_o);
  double_array xr = createdoublearray(m_o, n_o);

  max_reduce(x, h, nodata, xr, ir, jr);

  // Checking for errors in max indexes
  int im, jm;
  double vm;

  for (i = 0; i < m_o; i++) {
    printf("\n");
    for (j = 0; j < n_o; j++) {
      im = *(i * n_o + j + ir.val);
      jm = *(i * n_o + j + jr.val);
      vm = *(i * n_o + j + xr.val);
      printf(" %d ", (int)vm);
    }
  }
  printf("\n \n");
  printf("\n output residual: \n");
  for (i = 0; i < m_o; i++) {
    printf("\n");
    for (j = 0; j < n_o; j++) {
      im = *(i * n_o + j + ir.val);
      jm = *(i * n_o + j + jr.val);
      vm = *(i * n_o + j + xr.val);

      printf(" %lf ", vm - *(x.val + im * n + jm));
      if (vm == nodata) {
        printf(" \n nodata ERROR \n");
      }
    }
  }

  freearray(xr.val);
  freearray(ir.val);
  freearray(jr.val);
  freearray(x.val);

  checkmem();
  printf("\n\n");
  return 0;
}
