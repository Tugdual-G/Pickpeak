#include "findPeak.h"
#include "array.h"
#include "asciiGridParse.h"
#include "reduce.h"
#include <stdio.h>
#include <stdlib.h>

#define sqrt2 1.4142

int find_isolated(double_array x_in, int_array i_in, int_array j_in, int dim[2],
                  double R, int h, int margin, double_array *x_out,
                  int_array *i_out, int_array *j_out) {

  // Find isolated peaks and return their
  // positions i_out, j_out, and height, x_out.
  //
  // R , xs, ys and bbox should be integers in index coordinates

  int bbox[4] = {margin, dim[1] - margin, margin, dim[0] - margin};
  int k = 0, c = 0;
  int m = x_in.m;
  int n = x_in.n;

  R = R * R; // Radius of exclusion in index coord, R*R for efficiency

  // srdgs define the area research around a peak for taller peaks;
  int srdgs = 2;
  int l = m * n;

  int idx[l + 1];
  for (k = 0; k < l + 1; k++) {
    idx[k] = k;
  }

  double *x = x_in.val;

  int *i_o = (*i_out).val, // Rename for conveniance
      *j_o = (*j_out).val; // Store position of isolated peaks

  int *i_i = i_in.val, *j_i = j_in.val; // rename for conveniance

  char trigg; // =1 if the summit is isolated, else 0
  double d;   // distance between points

  k = 0;
  int i0 = 0;
  int j0 = 0;
  int i1 = 0;
  int j1 = 0;
  while (k < l) {
    trigg = 1;
    i0 = k / n;
    j0 = k % n;
    for (i1 = i0 - srdgs * (srdgs < i0);
         i1 < ((i0 + srdgs) * (srdgs < (m - i0)) + m * (m - i0 <= srdgs));
         i1++) {
      for (j1 = j0 - srdgs * (srdgs < j0);
           j1 < ((j0 + srdgs) * (srdgs < (n - j0)) + n * ((n - j0) <= srdgs));
           j1++) {
        d = (*(i_i + k) - *(i_i + i1 * n + j1)) *
                (*(i_i + k) - *(i_i + i1 * n + j1)) +
            (*(j_i + k) - *(j_i + i1 * n + j1)) *
                (*(j_i + k) - *(j_i + i1 * n + j1));
        if (d < R) {
          // If the two summits have the same height,
          // keep only one
          if (*(x + k) < *(x + i1 * n + j1)) {
            trigg = 0;
            i1 = m + 1;
            j1 = n + 1;
          } else {
            // idx + j can be compared againts,
            // but it is no more considered as
            // a potential isolated peak
            idx[i1 * n + j1] = idx[i1 * n + j1 + 1];
          }
        }
        // if (j1 > n) {
        // printf(" i1:%d j1:%d ", i1, j1);
        //}
      }
    }
    if (trigg == 1 && ((bbox[0] < *(j_i + k)) && (*(j_i + k) < bbox[1]) &&
                       (bbox[2] < *(i_i + k)) && (*(i_i + k) < bbox[3]))) {
      // if (trigg == 1) {
      *(i_o + c) = *(i_i + k);
      *(j_o + c) = *(j_i + k);
      *((*x_out).val + c) = *(x + k);
      c++;
    }
    k++;
    // finding the next potentialy isolated peak
    while (k != *(idx + k)) {
      k = *(idx + k);
    }
  }

  // Resizing arrays
  (*j_out).n = c;
  (*j_out).m = 1;
  (*i_out).n = c;
  (*i_out).m = 1;
  (*x_out).n = c;
  (*x_out).m = 1;
  return c;
}

void findpeak(double_array x, double R, int margin, double nodata,
              double_array *x_out, int_array *i_out, int_array *j_out) {

  /* R is in index units */
  int h = (int)(R / sqrt2 + 0.5); // +0.5 to round to the nearest
  if (h < 2) {
    printf("\n WARNING: R is to small in comparaison to the grid definition.\n"
           " R < gridstep*sqrt(2)  \n");
    h = 2;
  }
  int m = x.m, n = x.n;

  if (h > m || h > n) {
    printf("\n NOTE: The exclusion radius exceed the data extent \n");
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
  if (xr.n == 0 || xr.m == 0) {
    printf("\n ERROR during reduction \n");
  }

  *i_out = createintarray(1, m_o * n_o);
  *j_out = createintarray(1, m_o * n_o);
  *x_out = createdoublearray(1, m_o * n_o);

  int dim[2] = {m, n};
  margin = (int)(margin > 0) * (int)R;

  find_isolated(xr, ir, jr, dim, R, h, margin, x_out, i_out, j_out);

  freearray(xr.val);
  freearray(ir.val);
  freearray(jr.val);
}
