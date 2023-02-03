#ifndef FINDPEAK_H_
#define FINDPEAK_H_
#include "array.h"
#include "reduce.h"

void findpeak(double_array x, double R, unsigned int margin, double nodata,
              double_array *x_out, uint_array *i_out, uint_array *j_out);

#endif // FINDPEAK_H_
