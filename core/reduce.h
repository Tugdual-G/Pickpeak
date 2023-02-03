#ifndef FINDPEAKS_H_
#define FINDPEAKS_H_
#include "array.h"

void maxnan(double *x, unsigned int lenx, double nodata, unsigned int *imax,
            double *vmax);

void reduce_along_j0(double_array x_in, unsigned int h, double nodata,
                     double_array xr_out, uint_array j_max_out);

void reduce_along_j1(double_array x_in, unsigned int h, uint_array i_original,
                     double nodata, double_array xr_out, uint_array j_max_out,
                     uint_array i_max_out);

void max_reduce(double_array x_in, unsigned int h, double nodata,
                double_array xr_out, uint_array i_out, uint_array j_out);

#endif // FINDPEAKS_H_
