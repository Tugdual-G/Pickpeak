#ifndef FINDPEAK_H_
#define FINDPEAK_H_
#include "reduce.h"

void findpeak(double_array x, double R, int margin, double nodata,
              double_array *x_out, int_array *i_out, int_array *j_out);

#endif // FINDPEAK_H_
