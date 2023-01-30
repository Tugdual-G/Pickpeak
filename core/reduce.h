#ifndef FINDPEAKS_H_
#define FINDPEAKS_H_
#include "array.h"

void maxnan(double *x, int lenx, double nodata, int *imax, double *vmax);

void reduce_along_j0(double_array x_in, int h, double nodata,
                     double_array xr_out, int_array j_max_out);

void reduce_along_j1(double_array x_in, int h, int_array i_original,
                     double nodata, double_array xr_out, int_array j_max_out,
                     int_array i_max_out);

void max_reduce(double_array x_in, int h, double nodata, double_array xr_out,
                int_array i_out, int_array j_out);

#endif // FINDPEAKS_H_
