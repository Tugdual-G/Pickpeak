#include "findPeak.h"
#include "array.h"
#include "asciiGridParse.h"
#include "reduce.h"
#include <stdio.h>
#include <stdlib.h>

#define sqrt2 1.4142

int find_isolated(double_array x_in, int_array i_in, int_array j_in, double R,
                  int margin, double_array *x_out, int_array *i_out,
                  int_array *j_out) {

  // Find isolated peaks and return their
  // positions i_out, j_out, and height, x_out.
  //
  // R , xs, ys and bbox should be integers in index coordinates

  // int bbox[4] = {margin, x_in.n - margin, margin, x_in.m - margin};
  int i, j, k;

  R = R * R; // Radius of exclusion in index coord, R*R for efficiency
  int l = x_in.m * x_in.n;

  int idx[l + 1]; // A trick to discard no hight enougth points
  for (i = 0; i < l + 1; i++) {
    idx[i] = i;
  }

  double *x = (double *)x_in.val;

  int *i_o = (*i_out).val, // Rename for conveniance
      *j_o = (*j_out).val; // Store position of isolated peaks

  int *i_i = i_in.val, *j_i = j_in.val; // rename for conveniance

  char trigg; // =1 if the summit is isolated, else 0
  double d;   // distance between points

  k = 0;
  i = 0;
  while (i < l) {
    trigg = 1;
    for (j = 0; j < l; j++) {
      d = (*(i_i + i) - *(i_i + j)) * (*(i_i + i) - *(i_i + j)) +
          (*(j_i + i) - *(j_i + j)) * (*(j_i + i) - *(j_i + j));
      if (d < R) {
        // If the two summits have the same height,
        // keep only one
        if (*(x + i) < *(x + j)) {
          trigg = 0;
          break;
        } else {
          // idx + j can be compared againts,
          // but it is no more considered as
          // a potential isolated peak
          idx[j] = idx[j + 1];
        }
      }
    }
    // if (trigg == 1 && ((bbox[0] < *(j_i + i)) & (*(j_i + i) < bbox[1]) &
    //(bbox[2] < *(i_i + i)) & (*(i_i + i) < bbox[3]))) {
    if (trigg == 1) {
      *(i_o + k) = *(i_i + i);
      *(j_o + k) = *(j_i + i);
      *((*x_out).val + k) = *(x + i);
      k++;
    }
    i++;
    // finding the next potentialy isolated peak
    while (i != *(idx + i)) {
      i = *(idx + i);
    }
  }

  // Resizing arrays
  (*j_out).n = k;
  (*j_out).m = 1;
  (*i_out).n = k;
  (*i_out).m = 1;
  (*x_out).n = k;
  (*x_out).m = 1;
  return k;
}

void findpeak(double_array x, double R, int margin, double nodata,
              double_array *x_out, int_array *i_out, int_array *j_out) {

  /* R is in index units */
  int h = (int)(R / sqrt2 + 0.5); // +0.5 to round to the nearest
  if (h < 2) {
    printf("\n WARNING: R is to small in comparaison to the grid definition.\n"
           " R < gridstep*2*sqrt(2)  \n");
  }
  int m = x.m, n = x.n;

  if (h > m || h > n) {
    printf("\n WARNING: The exclusion radius exceed the data extent \n");
  }

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
  printf(" Exclusion Radius : %.2f  (grid steps) \n", R);
  printf(" first pass step  : %d \n", h);
  int_array ir = createintarray(m_o, n_o);
  int_array jr = createintarray(m_o, n_o);
  double_array xr = createdoublearray(m_o, n_o);

  max_reduce(x, h, nodata, xr, ir, jr);

  *i_out = createintarray(1, m_o * n_o);
  *j_out = createintarray(1, m_o * n_o);
  *x_out = createdoublearray(1, m_o * n_o);

  find_isolated(xr, ir, jr, R, margin, x_out, i_out, j_out);

  freearray(xr.val);
  freearray(ir.val);
  freearray(jr.val);
}
