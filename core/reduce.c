/*
** This module apply a reduction operation on the input array.
** The shape of the input array (m,n) is reduced to (m/h + (1), n/h + (1)).
** The reduction operation consist of keeping the max value in the stencil.
*/
#include "reduce.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>

void max_reduce(double_array x_in, unsigned int h, double nodata,
                double_array xr_out, uint_array i_out, uint_array j_out) {

  unsigned int m = x_in.m, n = x_in.n;

  /* Final shape of the reduced array */
  unsigned int mf, nf;
  mf = (m % h == 0) ? m / h : m / h + 1;

  nf = (n % h == 0) ? n / h : n / h + 1;

  if (xr_out.n * xr_out.m < mf * nf) {
    printf("\n ERROR the output array, xr, is to small \n");
  }

  double_array xr_out0 = createdoublearray(nf, m);
  uint_array j_out0 = create_uintarray(nf, m);

  /* The first time i_out is useless */
  reduce_along_j0(x_in, h, nodata, xr_out0, j_out0);
  reduce_along_j1(xr_out0, h, j_out0, nodata, xr_out, j_out, i_out);

  freearray(xr_out0);
  freearray(j_out0);
}

void reduce_along_j0(double_array x_in, unsigned int h, double nodata,
                     double_array xr_out, uint_array j_max_out) {

  /*
  This is a reduction fonction,
  it divide the number of columns by h

  This fonction takes an array z and process each row by
  taking the max value in the h first columns, then reapeat
  for every set of colums [h*j, (j+1)*h[ .

  The max of each set of cells is written to a transposed array zr.
  The index information are transposed to.

  i_prev0 has the same shape as z, it carries the orginal index i of z
  before the first time z was processed. Thus no information is lost
  on the position of the max values.

  The best case is: z % h = 0

  This function is almost the same as reduce_along_j1,
  except that it do not need to return as much information.
  */

  /* For readability */
  double *restrict x = x_in.val;
  unsigned int *restrict j_out = j_max_out.val;
  double *restrict xr = xr_out.val;

  /* m is also the number of columns of the returned zr array */
  unsigned int n = x_in.n;
  unsigned int m = x_in.m;

  /* In case the domain is not divisible by h */
  unsigned int nxr;
  nxr = (n % h == 0) ? n / h : n / h + 1;

  if (xr_out.n * xr_out.m < m * nxr) {
    printf("\n ERROR the output array, xr, is to small in 0\n");
  }

  double vmax;
  unsigned int imax;

  unsigned int i, j;
  for (i = 0; i < m; i++) {
    for (j = 0; j < n / h; j++) {
      maxnan((x + i * n + j * h), h, nodata, &imax, &vmax);
      *(xr + j * m + i) = vmax;
      *(j_out + j * m + i) = imax + j * h;
    }
    if (n % h != 0) {
      maxnan((x + i * n + j * h), n % h, nodata, &imax, &vmax);
      *(xr + j * m + i) = vmax;
      *(j_out + j * m + i) = imax + j * h;
    }
  }
}

void reduce_along_j1(double_array x_in, unsigned int h, uint_array i_original,
                     double nodata, double_array xr_out, uint_array i_max_out,
                     uint_array j_max_out) {

  /*
  This is a reduction fonction,
  it divide the number of columns by h

  This fonction takes an array z and process each row by
  taking the max value in the h first columns, then reapeat
  for every set of colums [h*j, (j+1)*h[ .

  The max of each set of cells is written to a transposed array zr.
  The index information are transposed to.

  i_prev0 has the same shape as z, it carries the orginal index i of z
  before the first time z was processed. Thus no information is lost
  on the position of the max values.

  The best case is: z % h = 0
  */

  /* For readability */
  double *restrict x = x_in.val;
  unsigned int *restrict i_or = i_original.val;

  /* Pointer to pointer to modify m and n */
  unsigned int *restrict j_out = j_max_out.val;
  unsigned int *restrict i_out = i_max_out.val;
  double *restrict xr = xr_out.val;

  /* m is the number of columns of the returned zr array */
  unsigned int n = x_in.n;
  unsigned int m = x_in.m;

  /* In case the domain is not divisible by h */
  unsigned int nxr;
  nxr = (n % h == 0) ? n / h : n / h + 1;

  if (xr_out.n * xr_out.m < m * nxr) {
    printf("\n ERROR the output array, xr, is to small in 1\n");
  }

  double vmax;
  unsigned int imax;
  /* Shape xr (,) : x.m, x.n/h +(x.n%h)/(x.n%h) */

  unsigned int i, j;
  for (i = 0; i < m; i++) {
    for (j = 0; j < n / h; j++) {
      maxnan((x + i * n + j * h), h, nodata, &imax, &vmax);
      *(xr + j * m + i) = vmax;
      *(j_out + j * m + i) = imax + j * h;
      *(i_out + j * m + i) = *(i_or + i * n + j * h + imax);
    }
    if (n % h != 0) {
      maxnan((x + i * n + j * h), n % h, nodata, &imax, &vmax);
      *(xr + j * m + i) = vmax;
      *(j_out + j * m + i) = imax + j * h;
      *(i_out + j * m + i) = *(i_or + i * n + j * h + imax);
    }
  }
}

inline void maxnan(double *x, unsigned int lenx, double nodata,
                   unsigned int *imax, double *vmax) {
  /* Finds the max value in (x) (x+lenx) */
  unsigned int i = 0;
  while (*(x + i) == nodata && i < lenx) {
    i++;
  }
  *vmax = *(x + i);
  *imax = i;
  for (i = i; i < lenx; i++) {
    if (*vmax < *(x + i) && *(x + i) != nodata) {
      *vmax = *(x + i);
      *imax = i;
    }
  }
}
