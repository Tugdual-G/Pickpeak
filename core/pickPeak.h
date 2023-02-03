#ifndef PICKPEAK_H_
#define PICKPEAK_H_
#include "array.h"
#include "asciiGridParse.h"

void transform_ortho(Grid grid, uint_array i_in, uint_array j_in,
                     double_array *x_t, double_array *y_t);

#endif // PICKPEAK_H_
