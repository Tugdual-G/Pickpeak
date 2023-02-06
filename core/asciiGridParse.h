/*
** This module take care of text data input comforming to
** the AAIGrid – Arc/Info ASCII Grid format.
**
** The module reads the header information and the data
** in separates functions.
**
** The parsing of the data can be done by directly
** loading all the data in the RAM, or by the use of
** a slower method but preserving RAM, which can be
** usefull, e.g. if the computation is run
** in parallel.
** NOTE fread is mean to be buffered,
** so i am not sure that it is the main cause of
** the performance change.
**
*/
#ifndef ASCIIGRIDPARSE_H_
#define ASCIIGRIDPARSE_H_
#include "array.h"

#define MAX_META 6 /* max posible number of meta tags in the header */
#define LENNAME 25 /* max length of the meta tags */

typedef struct header header; /* a temporary storage for parsing */
typedef struct Grid Grid;     /* store the ascii metadata and data */

/*
 * Read and store the metadata given
 * in the header of the ASCII file.
 * */
Grid read_ASCII_header(char *filename);

/* Reading the ascii raster data and storing
 * it in an array.
 */
void read_ASCII_data(Grid *grid, char fname[]);

/* Reading the ascii raster data and storing
 * it in an array by loading all the data in the RAM.
 */
void read_ASCII_data_fast(Grid *grid, char fname[]);

void print_info(Grid *raster);

/* return the max value of the raster, ingnoring the NODATA values*/
void maxv(Grid *grid);

/* save data as double in binary files, if needed */
void savebinary(char *filename, Grid raster);

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
  long int f_position; /* position of the data in the ascii file */
};

#endif // ASCIIGRIDPARSE_H_
