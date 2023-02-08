#ifndef MERGEDATA_H_
#define MERGEDATA_H_

#include "../core/array.h"
#include "../core/asciiGridParse.h"

/* Get the total spacial extent of a list of Grids */
void get_extent(Grid **gridlist, unsigned char ngrids, double *x_ll,
                double *y_ll, double *x_ur, double *y_ur);

/* Insert an array into a bigger array;
 *
 * i0, j0 : coordinates of the first element of the inserted array in the
 * canva */
void insert_array(double_array *patch, int i0, int j0, double_array *canva);

/* gridlist : grids to be merged */
double_array merge(Grid **gridlist, unsigned char ngrids, double nodata);

typedef struct SubGrid {
  unsigned int m;
  unsigned int n;
  unsigned char pos_i;
  unsigned char pos_j;
  double *data;
  unsigned int idxi;
  unsigned int idxj;
  unsigned int i_ll;
  unsigned int j_ll;
} SubGrid;

#endif // MERGEDATA_H_
