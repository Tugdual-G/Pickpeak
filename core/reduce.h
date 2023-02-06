/*
** This module apply a reduction operation on the input array.
** The shape of the input array (m,n) is reduced to (m/h + (1), n/h + (1)).
** The reduction operation consist of keeping the max value in the stencil.
*/
#ifndef FINDPEAKS_H_
#define FINDPEAKS_H_
#include "array.h"

/* provide the reduced input, which is the max of each h*h group of cells */
void max_reduce(double_array x_in, unsigned int h, double nodata,
                double_array xr_out, uint_array i_out, uint_array j_out);

#endif // FINDPEAKS_H_
