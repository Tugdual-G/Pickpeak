#include "mergeData.h"
#include "array.h"
#include "asciiGridParse.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void get_extent(LinkedGrid **gridlist, unsigned char ngrids, double *x_ll,
                double *y_ll, double *x_ur, double *y_ur) {

  /* Get the total spacial extent of a list of Grids */

  LinkedGrid grid = *gridlist[0];
  *x_ll = grid.xllcenter;
  *y_ll = grid.yllcenter;
  *x_ur = grid.xllcenter;
  *y_ur = grid.yllcenter;
  double temp_xur;
  double temp_yur;
  double dx = grid.cellsize;

  temp_xur = grid.xllcenter + dx * (grid.data.n - 1);
  temp_yur = grid.yllcenter + dx * (grid.data.m - 1);

  for (unsigned int i = 1; i < ngrids; ++i) {
    grid = *gridlist[i];
    if (*x_ll > grid.xllcenter) {
      *x_ll = grid.xllcenter;
    }
    if (*y_ll > grid.yllcenter) {
      *y_ll = grid.yllcenter;
    }

    temp_xur = grid.xllcenter + dx * (grid.data.n - 1);
    if (*x_ur < temp_xur) {
      *x_ur = temp_xur;
    }

    temp_yur = grid.yllcenter + dx * (grid.data.m - 1);
    if (*y_ur < temp_yur) {
      *y_ur = temp_yur;
    }
  }
}

void insert_array(double_array *patch, int i_ll, int j_ll,
                  double_array *canva) {
  /* Insert an array into a bigger array;
   *
   * i_ll, j_ll : coordinates of the first element of the inserted array in the
   * canva, negatives coordinates are posibles */

  unsigned int mp = (*patch).m, np = (*patch).n;
  unsigned int nc = (*canva).n, mc = (*canva).m;

  /* crop the inserted array if it exceed the canvas's shape */
  unsigned int m = (i_ll + mp <= mc) ? mp : mc - i_ll;
  unsigned int n = (j_ll + np <= nc) ? np : nc - j_ll;

  /* If i_ll or j_ll is negative */
  unsigned int i0 = (i_ll >= 0) ? 0 : -i_ll;
  unsigned int j0 = (j_ll >= 0) ? 0 : -j_ll;
  i_ll = (i_ll >= 0) ? i_ll : 0;
  j_ll = (j_ll >= 0) ? j_ll : 0;

  for (unsigned int i = 0; i < m - i0; ++i) {
    for (unsigned int j = 0; j < n - j0; ++j) {
      (*canva).val[(i_ll + i) * nc + (j_ll + j)] =
          (*patch).val[(i + i0) * np + (j + j0)];
    }
  }
}

double_array merge_window(LinkedGrid **grid_disposition, unsigned char ngrids,
                          unsigned int bbox[]) {

  /* bbox : bounding box with bounds included in the windows*/

  LinkedGrid *grid = NULL;
  unsigned int k;
  while ((grid == NULL) && (k < ngrids * ngrids)) {
    grid = grid_disposition[k];
    ++k;
  }
  if (k == ngrids * ngrids) {
    printf("\n ERROR: the subdomain disposition grid is empty \n");
    exit(1);
  }
  double nodata = (*grid).NODATA_value;
  unsigned int ncols = (*grid).ncols;
  unsigned int nrows = (*grid).nrows;

  unsigned int m_w = bbox[3] - bbox[2];
  unsigned int n_w = bbox[1] - bbox[0];
  double_array merged = createdoublearray(m_w, n_w);
  fill_double_array(&merged, nodata);

  /* finding wich datasets are inside the window */
  unsigned char pos_imin, pos_imax;
  unsigned char pos_jmin, pos_jmax;
  pos_imin = bbox[2] / nrows;
  pos_imax = bbox[3] / nrows;

  pos_jmin = bbox[0] / ncols;
  pos_jmax = bbox[1] / ncols;

  unsigned int i_ll, j_ll;
  for (unsigned char i = pos_imin; i < pos_imax + 1; ++i) {
    for (unsigned char j = pos_jmin; j < pos_jmax + 1; ++j) {
      grid = grid_disposition[i * ngrids + j];
      if (grid == NULL) {
        continue;
      }
      i_ll = i * nrows - bbox[2];
      j_ll = j * ncols - bbox[0];
      insert_array(&(*grid).data, i_ll, j_ll, &merged);
    }
  }
  return merged;
}

double_array merge(LinkedGrid **gridlist, unsigned char ngrids, double nodata) {
  /* gridlist : grids to be merged */

  LinkedGrid grid = *gridlist[0];
  double x_ll, y_ll;
  double x_ur, y_ur;
  double dx = grid.cellsize;

  get_extent(gridlist, ngrids, &x_ll, &y_ll, &x_ur, &y_ur);

  unsigned int m_t, n_t;
  m_t = (y_ur - y_ll) / dx + 1;
  n_t = (x_ur - x_ll) / dx + 1;

  double_array totaldomain = createdoublearray(m_t, n_t);
  fill_double_array(&totaldomain, nodata);

  unsigned int i0, j0;
  for (unsigned int i = 0; i < ngrids; ++i) {
    grid = *gridlist[i];
    i0 = (grid.yllcenter - y_ll) / dx; // TODO might fail
    j0 = (grid.xllcenter - x_ll) / dx; // TODO might fail
    insert_array(&grid.data, i0, j0, &totaldomain);
  }
  return totaldomain;
}

void get_position(LinkedGrid *subdomain, double xylowleftcorner[],
                  unsigned char *pos_i, unsigned char *pos_j) {
  /* Finds the position of a subdomain in the whole domain */
  /* The shape of the differents input datasets is suposed to be identical */
  /* x_ll and y_ll describe the position of the low left corner of the whole
   * domain*/
  double x_ll = xylowleftcorner[0], y_ll = xylowleftcorner[1];
  unsigned int nrows = (*subdomain).nrows;
  unsigned int ncols = (*subdomain).ncols;
  double dx = (*subdomain).cellsize;

  /* add + dx to avoid rounding (floring) error */
  *pos_i = (unsigned char)(((*subdomain).yllcenter - y_ll + dx) / (nrows * dx));
  *pos_j = (unsigned char)(((*subdomain).xllcenter - x_ll + dx) / (ncols * dx));
};
