#ifndef MERGEDATA_H_
#define MERGEDATA_H_

#include "../core/array.h"
#include "../core/asciiGridParse.h"

typedef struct SubGrid SubGrid;
typedef struct LinkedGrid LinkedGrid;
typedef struct AllGrids AllGrids;

/* Get the total spacial extent of a list of Grids */
void get_extent(LinkedGrid **gridlist, unsigned char ngrids, double *x_ll,
                double *y_ll, double *x_ur, double *y_ur);

/* Insert an array into a bigger array;
 *
 * i0, j0 : coordinates of the first element of the inserted array in the
 * canva */
void insert_array(double_array *patch, int i0, int j0, double_array *canva);

/* gridlist : grids to be merged */
double_array merge(LinkedGrid **gridlist, unsigned char ngrids, double nodata);

/* return the position of a subdomain in the domain */
void get_position(LinkedGrid *subdomain, double xylowleftcorner[],
                  unsigned char *pos_i, unsigned char *pos_j);

/* merge subdomains in the data window specified by bbox :
 * [left, rigth,bottom, top]*/
double_array merge_window(AllGrids allgrids, unsigned int bbox[]);

/* find the neighbours of each subdomains: west east south north */
AllGrids link_grids(LinkedGrid **gridlists, unsigned char ngrids);

struct AllGrids {
  unsigned int totalcols;
  unsigned int totalrows;
  LinkedGrid **grids_disposition;
  unsigned char ngrids_i;
  unsigned char ngrids_j;
};

struct SubGrid {
  unsigned int m;
  unsigned int n;
  unsigned char pos_i;
  unsigned char pos_j;
  double *data;
  unsigned int idxi;
  unsigned int idxj;
  unsigned int i_ll;
  unsigned int j_ll;
};

struct LinkedGrid {
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
  long int f_position;     /* position of the data in the ascii file */
  struct LinkedGrid *west; /* west neighbouring grid */
  struct LinkedGrid *east; /* east neighbouring grid */
  struct LinkedGrid *south;
  struct LinkedGrid *north;
};

#endif // MERGEDATA_H_
