#ifndef FINDPEAK_H_
#define FINDPEAK_H_
#include "reduce.h"

void findpeak(double_array x, double R, int margin, double nodata,
              double_array *x_out, int_array *i_out, int_array *j_out);

int find_isolated(double_array x_in, int_array i_in, int_array j_in, int dim[2],
                  double R, int h, int margin, double_array *x_out,
                  int_array *i_out, int_array *j_out);

#endif // FINDPEAK_H_
