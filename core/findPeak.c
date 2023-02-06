/*
** This module finds the isolated summits in the data.
*/

#include "findPeak.h"
#include "array.h"
#include "asciiGridParse.h"
#include "reduce.h"
#include <stdio.h>
#include <stdlib.h>

#define sqrt2 1.4142

static int find_isolated(double_array x_in, uint_array i_in, uint_array j_in,
                         unsigned int dim[], double R, unsigned int margin,
                         double nodata, double_array *x_out, uint_array *i_out,
                         uint_array *j_out);

static inline char check_neighbours(unsigned int *i_i, unsigned int *j_i,
                                    double *x, unsigned int *idx,
                                    unsigned int k, double R, double nodata,
                                    unsigned int m, unsigned int n);

void findpeak(double_array x, double R, unsigned int margin, double nodata,
              double_array *x_out, uint_array *i_out, uint_array *j_out) {

  /*
  ** This function return the position and the elevation of the isolated
  ** peaks, by first using a reduction of the data in the function
  ** max_reduce() then finding the isolated peaks in the reduced data.
  **
  ** x : elevation data, array of shape m,n
  ** R : exclusion radius, in index distance
  ** margin : if a margin is taken out of the domain, 1 or 0
  ** nodata : nodata value
  ** x_out : elevation of the isolated peaks, array of shape (1, count_peaks)
  ** i_out : index of the isolated peaks (row), array of shape (1, count_peaks)
  ** j_out : index of the isolated peaks (column),
  **         array of shape (1, count_peaks)
  */

  /* h is the step used for the input data reduction */
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

  unsigned int dim[] = {m, n};

  margin = (unsigned int)((margin > 0) * R);

  find_isolated(xr, ir, jr, dim, R, margin, nodata, x_out, i_out, j_out);

  freearray(xr);
  freearray(ir);
  freearray(jr);
}

static int find_isolated(double_array x_in, uint_array i_in, uint_array j_in,
                         unsigned int dim[], double R, unsigned int margin,
                         double nodata, double_array *restrict x_out,
                         uint_array *i_out, uint_array *j_out) {

  /*
   * Find isolated peaks in the previously reduced data and return their
   * positions i_out, j_out, and height, x_out.
   * R , xs, ys and bbox should be integers in index coordinates
   */

  unsigned int bbox[4] = {margin, dim[1] - margin, margin, dim[0] - margin};

  R = R * R; /* Radius of exclusion in index coord, R*R for efficiency */

  unsigned int m = x_in.m;
  unsigned int n = x_in.n;
  unsigned int l = m * n;

  /* list of the maxima wich are potential isolated summits */
  unsigned int idx[l + 1]; /* NOTE warning variable Length array */
  for (unsigned int i = 0; i < l + 1; ++i) {
    idx[i] = i;
  }

  /* Just some short-hands */
  double *x = x_in.val;
  unsigned int *i_i = i_in.val;
  unsigned int *j_i = j_in.val;
  /* Store position of isolated peaks */
  unsigned int *restrict i_o = (*i_out).val;
  unsigned int *restrict j_o = (*j_out).val;

  char trigg; /* = 1 if the summit is isolated, else 0 */

  unsigned int k = 0, count_peaks = 0;

  /* discard the eventual nodata values */
  while (*(x + k) == nodata) {
    ++k;
  }

  /* Start iterating over the maxima */
  while (k < l) {

    /* Check if the peak is isolated */
    trigg = check_neighbours(i_i, j_i, x, idx, k, R, nodata, m, n);

    /* Check if the peak is not in the margins */
    if (trigg && ((bbox[0] < *(j_i + k)) && (*(j_i + k) < bbox[1]) &&
                  (bbox[2] < *(i_i + k)) && (*(i_i + k) < bbox[3]))) {

      *(i_o + count_peaks) = *(i_i + k);
      *(j_o + count_peaks) = *(j_i + k);
      *((*x_out).val + count_peaks) = *(x + k);
      ++count_peaks;
    }

    /* finding the next potentialy isolated peak */
    ++k;
    trigg = 1;
    while (trigg) {
      while ((k != *(idx + k))) {
        k = *(idx + k);
      }
      trigg = 0;
      while ((*(x + k) == nodata) && (k < l)) {
        ++k;
        trigg = 1;
      }
    }
  }

  /* Resizing arrays */
  (*j_out).n = count_peaks;
  (*j_out).m = 1;
  (*i_out).n = count_peaks;
  (*i_out).m = 1;
  (*x_out).n = count_peaks;
  (*x_out).m = 1;
  return count_peaks;
}

inline static char check_neighbours(unsigned int *i_i, unsigned int *j_i,
                                    double *x, unsigned int *idx,
                                    unsigned int k, double R, double nodata,
                                    unsigned int m, unsigned int n) {

  /*
  ** This fuction check if a summit is the tallest among its neighbours.
  */
  unsigned int i0 = k / n;
  unsigned int j0 = (k % n);

  /*
   * srdgs define the indicial extent of research around a peak in the input
   * arrays
   */
  unsigned int srdgs = 2;
  /* Defining the stencil of peaks comparaison */
  unsigned int lim_inf_i = (srdgs < i0) ? i0 - srdgs : i0;
  unsigned int lim_sup_i = (srdgs < (m - i0)) ? (i0 + srdgs) : m;

  unsigned int lim_inf_j = (srdgs < j0) ? j0 - srdgs : j0;
  unsigned int lim_sup_j = (srdgs < (n - j0)) ? (j0 + srdgs) : n;

  /* Distance between the peaks */
  double d;

  for (unsigned int i1 = lim_inf_i; i1 < lim_sup_i; ++i1) {
    for (unsigned int j1 = lim_inf_j; j1 < lim_sup_j; ++j1) {

      /* Distance between the summit and the neighbour */
      d = (*(i_i + k) - *(i_i + i1 * n + j1)) *
              (*(i_i + k) - *(i_i + i1 * n + j1)) +
          (*(j_i + k) - *(j_i + i1 * n + j1)) *
              (*(j_i + k) - *(j_i + i1 * n + j1));

      if (d > R)
        /* check annother neighbour */
        continue;

      if ((*(x + k) >= *(x + i1 * n + j1))) {
        /* If the summit is taller than it's neighbour
         * discard the neighbour
         * check annother neighbour */
        idx[i1 * n + j1] = idx[i1 * n + j1 + 1];
        continue;
      }

      /* If the neighbour is not a nodata value
       * then the peak is not isolated */
      if (*(x + i1 * n + j1) != nodata) {
        return 0;
      }

      /* If all the above test fails
       * discard the neighbour from the list
       * of potential isolated peak : */
      idx[i1 * n + j1] = idx[i1 * n + j1 + 1];
    }
  }
  return 1;
}
