
/*
** This is the main source file for the application.
**
** The program takes one ASCII grid as input and writes a
** GEOjson file to disk, containing the position and elevation of the
** isolated peaks found in the grid.
**
*/

#include "pickPeak.h"
#include "array.h"
#include "asciiGridParse.h"
#include "findPeak.h"
#include "parseArgs.h"
#include "writeJson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
  /*
   * The command line parameters are:
   *
   * -R for the radius of exclusion,
   * -i or --infile to wich is given the input file name,
   * -o or --outfile to wich is given the name of the output file
   * -m or --margin wich limit the spacial extent of the returned values
   */

  /* Parsing the command-line parameters */
  Param param = {.margin = 0};
  parse(argc, argv, &param);

  Grid grid = read_ASCII_header(param.infile[0]);

  /* R is transformed to index coordinates
   * the rest of the program mostly compute on index values
   */
  double R = param.R / grid.cellsize;

  clock_t begin = clock();

  read_ASCII_data_fast(&grid, param.infile[0]);

  clock_t end = clock();
  float t_total = ((float)(end - begin)) / CLOCKS_PER_SEC;
  printf("%-20s%5s%.3fs\n", " data-reading time ", ":", t_total);

  print_info(&grid);

  uint_array i_out;
  uint_array j_out;
  double_array z_out;

  /* Begining of the actual computation */
  begin = clock();
  findpeak(grid.data, R, param.margin, grid.NODATA_value, &z_out, &i_out,
           &j_out);
  end = clock();
  t_total = ((float)(end - begin)) / CLOCKS_PER_SEC;
  printf("%-20s%5s%.5fs\n", " computing time ", ":", t_total);

  if (i_out.n == 0 || i_out.m == 0) {
    printf("\n No peak found \n");
    exit(1);
  }

  printf("%-20s%5s%d\n", " peaks found ", ":", z_out.n);

  /* Transform index coordinates to real coordinates */
  double_array x = createdoublearray(1, z_out.n);
  double_array y = createdoublearray(1, z_out.n);
  transform_ortho(grid, i_out, j_out, &x, &y);

  /* Well ... */
  writeJsonFile(param.outfile, x, y, z_out);

  freearray(x);
  freearray(y);
  freearray(z_out);
  freearray(i_out);
  freearray(j_out);
  freearray(grid.data);
  free(param.infile[0]);
  return 0;
}

void transform_ortho(Grid grid, uint_array i_in, uint_array j_in,
                     double_array *x_t, double_array *y_t) {

  /* Transformation of the points from indicial coords back into original
   * raster projection coordinates
   *
   *
   * Here we are just applying the simple transform :
   *
   * |M00  0  M02|      |x|
   * | 0  M11 M12|   X  |y| = transformed array
   * | 0   0   1 |      |1|
   *
   * The convention in ASCII grid is that the last line
   * of the file is a the bottom (with north up)
   */

  double M00;
  double M02;
  double M12;

  /* Taking care of the type of discretization,
   * i.e. , are the values at the center or the corners of the cells */
  if (grid.centered) {
    M00 = grid.cellsize;
    M02 = grid.xllcorner;
    M12 = (grid.nrows - 1) * M00 + grid.yllcorner;
  } else {
    M00 = grid.cellsize;
    M02 = grid.xllcorner + M00 / 2;
    M12 = (grid.nrows) * M00 + grid.yllcorner - M00 / 2;
  }

  /* aliases values */
  unsigned int l = i_in.n * i_in.m;
  double *restrict x = (*x_t).val;
  double *restrict y = (*y_t).val;
  unsigned int *restrict i_i = i_in.val;
  unsigned int *restrict j_i = j_in.val;
  for (unsigned int i = 0; i < l; i++) {
    *(x + i) = (double)*(j_i + i) * M00 + M02;
    *(y + i) = -(double)*(i_i + i) * M00 + M12;
  }
}
