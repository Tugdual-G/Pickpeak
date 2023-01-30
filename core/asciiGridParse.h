#ifndef ASCIIGRIDPARSE_H_
#define ASCIIGRIDPARSE_H_

#define N_HEADERS 6

typedef struct header header;
typedef struct Grid Grid;

void savebinary(char *filename, Grid raster);

Grid *read_ASCIIgrid(char *filename);

void print_info(Grid raster);

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
  double *data;
};

#endif // ASCIIGRIDPARSE_H_
