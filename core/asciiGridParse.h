#ifndef ASCIIGRIDPARSE_H_
#define ASCIIGRIDPARSE_H_

#define N_HEADERS 6
#include "array.h"

typedef struct header header;
typedef struct Grid Grid;

void savebinary(char *filename, Grid raster);

Grid read_ASCIIgrid(char *filename);

void print_info(Grid raster);

void maxv(Grid *grid);

struct header {
  char name[25];
  double val;
};

struct Grid {
  int ncols;
  int nrows;
  double xllcorner;
  double yllcorner;
  double cellsize;
  double NODATA_value;
  char hasNODATAval;
  double maxval;
  double minval;
  double xllcenter;
  double yllcenter;
  char centered;
  double_array data;
};

#endif // ASCIIGRIDPARSE_H_
