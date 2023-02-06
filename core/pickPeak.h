/*
** This is the main module of the program.
*/
#ifndef PICKPEAK_H_
#define PICKPEAK_H_
#include "array.h"
#include "asciiGridParse.h"

/* Perform a linear transformation of the data's coordinates.
 * from indicial coordinates back to the original grid metric.
 */
void transform_ortho(Grid grid, uint_array i_in, uint_array j_in,
                     double_array *x_t, double_array *y_t);

#endif // PICKPEAK_H_
