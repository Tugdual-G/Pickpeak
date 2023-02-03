#ifndef ASCIIGRIDPARSE_H_
#define ASCIIGRIDPARSE_H_
#include "array.h"

#define N_HEADERS 6
#define LENNAME 25

typedef struct header header;
typedef struct Grid Grid;

void savebinary(char *filename, Grid raster);

Grid read_ASCII_header(char *filename);

void read_ASCII_data(Grid *grid, char fname[]);

void print_info(Grid *raster);

void maxv(Grid *grid);

struct header {
  char name[LENNAME];
  double val;
};

struct Grid {
  unsigned int ncols;
  unsigned int nrows;
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
  long int f_position;
};

#endif // ASCIIGRIDPARSE_H_
