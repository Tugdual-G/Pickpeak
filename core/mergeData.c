#include "mergeData.h"
#include "array.h"
#include "asciiGridParse.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static char is_data_in_window(unsigned int bbox[], AllGrids allgrids);

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
  unsigned int m = ((i_ll + mp) <= mc) ? mp : mc - i_ll;
  unsigned int n = ((j_ll + np) <= nc) ? np : nc - j_ll;
  // printf("m %u, n %u  ", m, n);

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

double_array merge_window(AllGrids allgrids, unsigned int bbox[]) {

  /*
   * grid_disposition contain the spacial agencement of the subdomains.
   *  it is of shape (ngrids,ngrids), each cell contains NULL or a pointer to
   *  the coresponding grid.
   * bbox : bounding box with bounds included in the windows*/

  unsigned int m_w = (1 + bbox[3]) - bbox[2];
  unsigned int n_w = (1 + bbox[1]) - bbox[0];
  double_array merged = createdoublearray(m_w, n_w);

  LinkedGrid *grid = NULL;
  unsigned int k = 0;
  unsigned char ngridj = allgrids.ngrids_j, ngridi = allgrids.ngrids_i;

  while ((grid == NULL) && (k < ngridj * ngridi)) {
    grid = allgrids.grids_disposition[k];
    ++k;
  }
  if (grid == NULL) {
    freearray(merged);
    merged.val = NULL;
    return merged;
  }

  double nodata = (*grid).NODATA_value;
  unsigned int ncols = (*grid).ncols;
  unsigned int nrows = (*grid).nrows;
  fill_double_array(&merged, nodata);

  /* finding which datasets are inside the window */
  unsigned char pos_imin, pos_imax;
  unsigned char pos_jmin, pos_jmax;
  pos_imin = bbox[2] / nrows;
  pos_imax = bbox[3] / nrows;
  // printf("%u,%u \n", pos_imin, pos_imax);
  pos_imax = (pos_imax < ngridi) ? pos_imax : ngridi - 1;

  pos_imin = (pos_imin >= ngridi) ? pos_imax + 1 : pos_imin;

  pos_jmin = bbox[0] / ncols;
  pos_jmax = bbox[1] / ncols;
  pos_jmax = (pos_jmax < ngridj) ? pos_jmax : ngridj - 1;
  pos_jmin = (pos_jmin >= ngridj) ? pos_jmax + 1 : pos_jmin;

  unsigned int i_ll, j_ll;
  for (unsigned char i = pos_imin; i < pos_imax + 1; ++i) {
    for (unsigned char j = pos_jmin; j < pos_jmax + 1; ++j) {
      grid = allgrids.grids_disposition[i * ngridj + j];
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

AllGrids link_grids(LinkedGrid **gridlists, unsigned char ngrids) {
  /* find the neighbours of each subdomains: west east south north */
  /* Return a grid describing the spatial disposition of the subdomain */
  if (ngrids == 0) {
    printf("\n ERROR: number of grids provided in grid list is 0 \n");
  }
  double x_ll, y_ll, x_ur, y_ur;
  get_extent(gridlists, ngrids, &x_ll, &y_ll, &x_ur, &y_ur);
  double xy_ll[] = {x_ll, y_ll};
  unsigned char xpos;
  unsigned char ypos;

  unsigned char ngridi = 0;
  unsigned char ngridj = 0;

  LinkedGrid *grid_ptr = NULL;
  for (unsigned char i = 0; i < ngrids; ++i) {
    grid_ptr = gridlists[i];
    if (grid_ptr == NULL) {
      printf("\n ERROR: the list contains a null ptr \n");
      exit(1);
    }
    get_position(grid_ptr, xy_ll, &ypos, &xpos);
    ngridi = (ngridi < ypos + 1) ? ypos + 1 : ngridi;
    ngridj = (ngridj < xpos + 1) ? xpos + 1 : ngridj;
  }

  /* Just to shut the compiler warning off */
  if (grid_ptr == NULL) {
    printf("\n ERROR: the list contains a null ptr \n");
    exit(1);
  }

  LinkedGrid **grids_disposition = NULL;
  grids_disposition =
      (LinkedGrid **)malloc(sizeof(LinkedGrid *) * ngridi * ngridj);
  if (grids_disposition == NULL) {
    printf("\n ERROR: cannot allocate memory for grid positions \n");
    exit(1);
  }
  for (unsigned int i = 0; i < ngridi * ngridj; ++i) {
    grids_disposition[i] = NULL;
  }

  for (unsigned char i = 0; i < ngrids; ++i) {
    grid_ptr = gridlists[i];
    get_position(grid_ptr, xy_ll, &ypos, &xpos);
    grids_disposition[ypos * ngridj + xpos] = grid_ptr;
  }
  AllGrids allgrids = {
      .totalcols = (*gridlists[0]).ncols * ngridj,
      .totalrows = (*gridlists[0]).nrows * ngridi,
      .grids_disposition = grids_disposition,
      .ngrids_i = ngridi,
      .ngrids_j = ngridj,
      .sub_ncols = (*grid_ptr).ncols,
      .sub_nrows = (*grid_ptr).nrows,
  };

  for (unsigned char i = 0; i < ngridi; ++i) {
    for (unsigned char j = 0; j < ngridj; ++j) {
      grid_ptr = grids_disposition[i * ngridi + j];
      if (grid_ptr == NULL) {
        continue;
      }
      (*grid_ptr).west = (j > 0) ? grids_disposition[i * ngridi + j - 1] : NULL;

      (*grid_ptr).east =
          (j < ngridj - 1) ? grids_disposition[i * ngridi + j + 1] : NULL;

      (*grid_ptr).south =
          (i > 0) ? grids_disposition[(i - 1) * ngridi + j] : NULL;

      (*grid_ptr).north =
          (i < ngridi - 1) ? grids_disposition[(i + 1) * ngridi + j] : NULL;
    }
  }
  return allgrids;
}

AllGrids divide_domain(AllGrids allgrids, unsigned int step) {
  /* Return a new domain made of subdomains whose shapes are
   * multiples of the step size (except near the border of the domain).
   */

  unsigned int totalrows = allgrids.totalrows;
  unsigned int totalcols = allgrids.totalcols;
  unsigned int newsub_m, newsub_n;
  /* If the step is larger than the total domain, hay no problema */
  newsub_m = allgrids.sub_nrows - (allgrids.sub_nrows % step);
  newsub_m = (newsub_m == 0) ? step : newsub_m;

  newsub_n = allgrids.sub_ncols - (allgrids.sub_ncols % step);
  newsub_n = (newsub_n == 0) ? step : newsub_n;

  unsigned char new_ngridi, new_ngridj;
  new_ngridi = (totalrows % newsub_n == 0) ? totalrows / newsub_n
                                           : totalrows / newsub_n + 1;
  new_ngridj = (totalcols % newsub_m == 0) ? totalcols / newsub_m
                                           : totalcols / newsub_m + 1;

  LinkedGrid **gridisposition = NULL;
  gridisposition =
      (LinkedGrid **)malloc(sizeof(LinkedGrid *) * new_ngridi * new_ngridj);
  if (gridisposition == NULL) {
    exit(1);
  }
  for (unsigned int i = 0; i < new_ngridi; ++i) {
    for (unsigned int j = 0; j < new_ngridj; ++j) {
      gridisposition[i * new_ngridj + j] = NULL;
    }
  }

  AllGrids new_allgrids = {
      .grids_disposition = gridisposition,
      .ngrids_i = new_ngridi,
      .ngrids_j = new_ngridj,
      .sub_nrows = newsub_m,
      .sub_ncols = newsub_n,
      .totalrows = totalrows,
      .totalcols = totalcols,
  };

  unsigned int bbox[4];
  LinkedGrid *grid = NULL;
  for (unsigned int i = 0; i < new_ngridi; ++i) {
    for (unsigned int j = 0; j < new_ngridj; ++j) {
      bbox[0] = i * newsub_m;
      bbox[1] = (i + 1) * newsub_m - 1;
      bbox[2] = j * newsub_n;
      bbox[3] = (j + 1) * newsub_n - 1;

      /* crop if it exceed data limits */
      bbox[1] = (bbox[1] > totalcols - 1) ? totalcols - 1 : bbox[1];
      bbox[3] = (bbox[3] > totalrows - 1) ? totalrows - 1 : bbox[3];

      if (!is_data_in_window(bbox, allgrids)) {
        continue;
      }
      grid = NULL;
      grid = malloc(sizeof(LinkedGrid));
      if (grid == NULL) {
        exit(1);
      }
      (*grid).ncols = 0;
      (*grid).nrows = 0;
      (*grid).xllcenter = 0;
      (*grid).yllcenter = 0;
      printf("%u, %u, %u, %u \n", bbox[0], bbox[1], bbox[2], bbox[3]);
      fflush(stdout);
      (*grid).data = merge_window(allgrids, bbox);
      gridisposition[i * new_ngridj + j] = grid;
    }
  }
  return new_allgrids;
}

static char is_data_in_window(unsigned int bbox[], AllGrids allgrids) {
  /* Check if there is data in the bounding box
   * The bbox bounds are included in the domain.
   * The bounds must be specified in index coordinates.
   * bbox = [left, rigth, bottom, top] */

  unsigned int ncols = allgrids.sub_ncols;
  unsigned int nrows = allgrids.sub_nrows;
  char i0, i1, j0, j1;
  j0 = bbox[0] / ncols;
  j1 = bbox[1] / ncols;
  i0 = bbox[2] / nrows;
  i1 = bbox[3] / nrows;

  unsigned char ngrid_j = allgrids.ngrids_j;
  void *grid = NULL;
  for (unsigned char i = i0; i < i1 + 1; ++i) {
    for (unsigned char j = j0; j < j1 + 1; ++j) {
      grid = allgrids.grids_disposition[i * ngrid_j + j];
      if (grid != NULL) {
        return 1;
      }
    }
  }
  return 0;
}

double_array merge_allgrids(AllGrids allgrids) {
  /* merge all the grids contained in allgrids */

  unsigned int m_w = allgrids.totalrows, n_w = allgrids.totalcols;
  double_array merged = createdoublearray(m_w, n_w);

  LinkedGrid *grid = NULL;
  unsigned int k = 0;
  unsigned char ngridj = allgrids.ngrids_j, ngridi = allgrids.ngrids_i;

  while ((grid == NULL) && (k < ngridj * ngridi)) {
    grid = allgrids.grids_disposition[k];
    ++k;
  }
  if (grid == NULL) {
    freearray(merged);
    merged.val = NULL;
    return merged;
  }

  double nodata = (*grid).NODATA_value;
  fill_double_array(&merged, nodata);
  unsigned int i_ll, j_ll;
  for (unsigned char i = 0; i < allgrids.ngrids_i; ++i) {
    for (unsigned char j = 0; j < allgrids.ngrids_j; ++j) {
      grid = allgrids.grids_disposition[i * ngridj + j];
      if (grid == NULL) {
        continue;
      }
      i_ll = i * allgrids.sub_nrows;
      j_ll = j * allgrids.sub_ncols;
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
