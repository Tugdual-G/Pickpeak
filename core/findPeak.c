#include "findPeak.h"
#include "array.h"
#include "asciiGridParse.h"
#include "reduce.h"
#include <stdio.h>
#include <stdlib.h>

#define sqrt2 1.4142

int find_isolated(double_array x_in, uint_array i_in, uint_array j_in,
                  unsigned int dim[2], double R, unsigned int margin,
                  double nodata, double_array *x_out, uint_array *i_out,
                  uint_array *j_out);

void findpeak(double_array x, double R, unsigned int margin, double nodata,
              double_array *x_out, uint_array *i_out, uint_array *j_out) {

  /* R is in index units */
  unsigned int h =
      (unsigned int)(R / sqrt2 + 0.5); /* +0.5 to round to the nearest */

  if (h < 2) {
    printf("\n WARNING: R is to small in comparaison to the grid definition.\n"
           " R < gridstep*sqrt(2) \n");
    h = 2;
  }
  unsigned int m = x.m, n = x.n;

  if (h > m || h > n) {
    printf("\n NOTE: The exclusion radius exceed the data extent \n");
  }

  unsigned int m_o, n_o; /* size of the reduction output */

  m_o = (m % h == 0) ? m / h : m / h + 1;
  n_o = (n % h == 0) ? n / h : n / h + 1;

  printf("%-20s%5s%.2f (grid steps)\n", " Exclusion Radius ", ":", R);
  printf("%-20s%5s%d\n", " first pass step ", ":", h);
  uint_array ir = create_uintarray(m_o, n_o);
  uint_array jr = create_uintarray(m_o, n_o);
  double_array xr = createdoublearray(m_o, n_o);

  max_reduce(x, h, nodata, xr, ir, jr);
  if (xr.n == 0 || xr.m == 0) {
    printf("\n ERROR during reduction \n");
  }

  *i_out = create_uintarray(1, m_o * n_o);
  *j_out = create_uintarray(1, m_o * n_o);
  *x_out = createdoublearray(1, m_o * n_o);

  unsigned int dim[2] = {m, n};

  margin = (unsigned int)((margin > 0) * R);

  find_isolated(xr, ir, jr, dim, R, margin, nodata, x_out, i_out, j_out);

  freearray(xr);
  freearray(ir);
  freearray(jr);
}

int find_isolated(double_array x_in, uint_array i_in, uint_array j_in,
                  unsigned int dim[2], double R, unsigned int margin,
                  double nodata, double_array *x_out, uint_array *i_out,
                  uint_array *j_out) {

  /*
   * Find isolated peaks and return their
   * positions i_out, j_out, and height, x_out.
   * R , xs, ys and bbox should be integers in index coordinates
   */

  unsigned int bbox[4] = {margin, dim[1] - margin, margin, dim[0] - margin};

  R = R * R; /* Radius of exclusion in index coord, R*R for efficiency */

  unsigned int m = x_in.m;
  unsigned int n = x_in.n;
  /*
   * srdgs define the indicial extent of research around a peak in the input
   * arrays
   * */
  unsigned int srdgs = 2;
  unsigned int lim_sup_i, lim_inf_i, lim_sup_j, lim_inf_j;

  unsigned int l = m * n;
  unsigned int idx[l + 1]; /* NOTE warning variable Length array */
  for (unsigned int i = 0; i < l + 1; i++) {
    idx[i] = i;
  }

  double *x = x_in.val;

  unsigned int *restrict i_o = (*i_out).val, /* Rename for conveniance */
      *restrict j_o = (*j_out).val; /* Store position of isolated peaks */

  unsigned int *restrict i_i = i_in.val;
  unsigned int *restrict j_i = j_in.val; /* rename for conveniance */

  char trigg; /* =1 if the summit is isolated, else 0 */
  double d;   /* distance between points */

  unsigned int k = 0, count_ispeak = 0, i0 = 0, j0 = 0;
  while (*(x + k) == nodata) {
    k++;
  }
  while (k < l) {
    trigg = 1;
    i0 = k / n;
    j0 = (k % n);

    lim_inf_i = (srdgs < i0) ? i0 : i0 - srdgs;
    lim_sup_i = (srdgs < (m - i0)) ? (i0 + srdgs) : m;

    lim_inf_j = (srdgs < j0) ? j0 : j0 - srdgs;
    lim_sup_j = (srdgs < (n - j0)) ? (j0 + srdgs) : n;

    for (unsigned int i1 = lim_inf_i; i1 < lim_sup_i; i1++) {
      for (unsigned int j1 = lim_inf_j; j1 < lim_sup_j; j1++) {

        d = (*(i_i + k) - *(i_i + i1 * n + j1)) *
                (*(i_i + k) - *(i_i + i1 * n + j1)) +
            (*(j_i + k) - *(j_i + i1 * n + j1)) *
                (*(j_i + k) - *(j_i + i1 * n + j1));
        if (d < R) {
          /* If the two summits have the same height, */
          /* keep only one */
          /* I prefer to check nodata here instead
           * of just before computing d,
           * to reduce memory transactions.
           */
          if ((*(x + k) < *(x + i1 * n + j1)) &&
              (*(x + i1 * n + j1) != nodata)) {
            trigg = 0;
            i1 = m + 1;
            j1 = n + 1;
          } else {
            /* idx + j can be compared againts,
             * but it is no more considered as
             * a potential isolated peak */
            idx[i1 * n + j1] = idx[i1 * n + j1 + 1];
          }
        }
      }
    }
    if (trigg && ((bbox[0] < *(j_i + k)) && (*(j_i + k) < bbox[1]) &&
                  (bbox[2] < *(i_i + k)) && (*(i_i + k) < bbox[3]))) {
      *(i_o + count_ispeak) = *(i_i + k);
      *(j_o + count_ispeak) = *(j_i + k);
      *((*x_out).val + count_ispeak) = *(x + k);
      count_ispeak++;
    }
    k++;
    /* finding the next potentialy isolated peak */

    while ((k != *(idx + k))) {
      k = *(idx + k);
    }
    while ((*(x + k) == nodata) && (k < l)) {
      k++;
    }
  }

  /* Resizing arrays */
  (*j_out).n = count_ispeak;
  (*j_out).m = 1;
  (*i_out).n = count_ispeak;
  (*i_out).m = 1;
  (*x_out).n = count_ispeak;
  (*x_out).m = 1;
  return count_ispeak;
}
