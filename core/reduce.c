#include "reduce.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>

void maxnan(double *x, int lenx, double nodata, int *imax, double *vmax) {
  /* Finds the max value in (x) (x+lenx) */
  int i = 0;
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

void reduce_along_j0(double_array x_in, int h, double nodata,
                     double_array xr_out, int_array j_max_out) {

  /*
  This is a reduction fonction,
  it divide the number of columns by h

  This fonction takes an array z and process each row by
  taking the max value in the h first columns, then jump h
  columns and reapeat.

  The max of each batch of cell is written to a transposed array zr,
  index information are transposed to.

  i_prev0 has the same shape as z, it carries the orginal index i of z
  before the first time z was processed. Thus no information is lost
  on the position of the max values.

  The best case is: z % h = 0
  in this case some max values could be skiped at the edge.
  */

  // For readability
  double *restrict x = x_in.val;

  // Pointer to pointer to modify m and n
  int *restrict j_out = j_max_out.val;
  double *restrict xr = xr_out.val;

  // m is the number of columns of the returned zr array
  int n = x_in.n;
  int m = x_in.m;

  // In case the domain is not divisible by h
  int nxr;
  if (n % h == 0) {
    nxr = n / h;
  } else {
    nxr = n / h + 1;
  }

  if (xr_out.n * xr_out.m < m * nxr) {
    printf("\n ERROR the output array, xr, is to small in 0\n");
  }

  double vmax;
  int imax;
  // Shape xr (,) : x.m, x.n//h +(x.n%h)/(x.n%h)

  int i, j;
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

void reduce_along_j1(double_array x_in, int h, int_array i_original,
                     double nodata, double_array xr_out, int_array i_max_out,
                     int_array j_max_out) {

  /*
  This is a reduction fonction,
  it divide the number of columns by h

  This fonction takes an array z and process each row by
  taking the max value in the h first columns, then jump h
  columns and reapeat.

  The max of each batch of cell is written to a transposed array zr,
  index information are transposed to.

  i_prev0 has the same shape as z, it carries the orginal index i of z
  before the first time z was processed. Thus no information is lost
  on the position of the max values.

  The best case is: z % h = 0
  in this case some max values could be skiped at the edge.
  */

  // For readability
  double *restrict x = x_in.val;
  int *restrict i_or = i_original.val;

  // Pointer to pointer to modify m and n
  int *restrict j_out = j_max_out.val;
  int *restrict i_out = i_max_out.val;
  double *restrict xr = xr_out.val;

  // m is the number of columns of the returned zr array
  int n = x_in.n;
  int m = x_in.m;

  // In case the domain is not divisible by h
  int nxr;
  if (n % h == 0) {
    nxr = n / h;
  } else {
    nxr = n / h + 1;
  }

  if (xr_out.n * xr_out.m < m * nxr) {
    printf("\n ERROR the output array, xr, is to small in 1\n");
  }

  double vmax;
  int imax;
  // Shape xr (,) : x.m, x.n//h +(x.n%h)/(x.n%h)

  int i, j;
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

void max_reduce(double_array x_in, int h, double nodata, double_array xr_out,
                int_array i_out, int_array j_out) {
  int m = x_in.m, n = x_in.n;

  // Final shape of the reduced array
  int mf, nf;
  if (m % h == 0) {
    mf = m / h;
  } else {
    mf = m / h + 1;
  }
  if (n % h == 0) {
    nf = n / h;
  } else {
    nf = n / h + 1;
  }
  if (xr_out.n * xr_out.m < mf * nf) {
    printf("\n ERROR the output array, xr, is to small \n");
  }

  double_array xr_out0 = createdoublearray(nf, m);
  int_array j_out0 = createintarray(nf, m);

  // The first time i_out is useless
  reduce_along_j0(x_in, h, nodata, xr_out0, j_out0);
  reduce_along_j1(xr_out0, h, j_out0, nodata, xr_out, j_out, i_out);

  freearray(xr_out0);
  freearray(j_out0);
}
