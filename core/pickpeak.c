// #include "pickpeak.h"
#include "array.h"
#include "asciiGridParse.h"
#include "findpeak.h"
#include "parseArgs.h"
#include "writeJson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void transform_ortho(Grid grid, int_array i_in, int_array j_in,
                     double_array *restrict x_t, double_array *restrict y_t) {
  //
  // Here we are just applying the simple transform
  //
  //  |M00 M01 M02|      |x|
  //  |M10 M11 M12|   X  |y| = transformed array
  //  |M20 ...    |      |1|
  //

  int M00 = grid.cellsize;
  int M11 = M00; // Why not, since stepsize is the same for each direction on
                 // asciiGrids
  int M02 = grid.xllcorner;
  int M12 = grid.yllcorner;

  int l = i_in.n;
  double *x = (*x_t).val;
  double *y = (*y_t).val;
  int *i_i = i_in.val;
  int *j_i = j_in.val;

  int i;
  for (i = 0; i < l; i++) {
    *(x + i) = *(i_i + i) * M00 + M02;
    *(y + i) = *(j_i + i) * M11 + M12;
  }
}

int main(int argc, char *argv[]) {

  Param param = {.margin = 0, .R = 2};
  parse(argc, argv, &param);

  Grid grid = read_ASCIIgrid(param.infile);
  double_array array = grid.data;
  int_array i_out;
  int_array j_out;
  double_array z_out;
  double R = param.R;

  findpeak(array, R, param.margin, grid.NODATA_value, &z_out, &i_out, &j_out);

  if (z_out.m != 1 || z_out.n != i_out.n || i_out.m != j_out.m) {
    printf("\n  array size error \n");
    return 0;
  }

  // Transform index coordinates to real coordinates
  double_array x = createdoublearray(1, z_out.n);
  double_array y = createdoublearray(1, z_out.n);

  transform_ortho(grid, i_out, j_out, &x, &y);

  writeJsonFile(param.outfile, y, x, z_out);
}
