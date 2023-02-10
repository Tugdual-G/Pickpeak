#include "../core/array.h"
#include "../core/mergeData.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#define BG "\x1b[48;2;"
#define RST "\x1b[0m"

void ascent_array(double_array *array, double val) {
  for (unsigned int i = 0; i < (*array).m * (*array).n; ++i) {
    (*array).val[i] = i + val;
  }
}

double max_array(double_array array) {
  unsigned int m = array.m, n = array.n;
  double max = *array.val;
  for (unsigned int i = 0; i < m * n; ++i) {
    if (*(array.val + i) > max) {
      max = *(array.val + i);
    }
  }
  return max;
}

double min_array(double_array array) {
  unsigned int m = array.m, n = array.n;
  double min = *array.val;
  for (unsigned int i = 0; i < m * n; ++i) {
    if (*(array.val + i) < min) {
      min = *(array.val + i);
    }
  }
  return min;
}

void array_to_colors(double_array *array, unsigned char scale,
                     unsigned char step) {

  FILE *fp = NULL;
  fp = fopen("rgbcmap.asc", "r");
  if (fp == NULL) {
    exit(1);
  }

  char c;
  unsigned int cmaplength = 0;
  while ((c = getc(fp)) != EOF) {
    if (c == '\n') {
      ++cmaplength;
    }
  }

  unsigned char cmap[(1 + cmaplength) * 3];
  unsigned int colorintensity;
  fseek(fp, 0, SEEK_SET);

  /* scanning the colormap rgb values */
  unsigned int i = 0;
  while (fscanf(fp, "%u ", &colorintensity) != EOF) {
    cmap[i] = (unsigned char)colorintensity;
    ++i;
  }
  fclose(fp);

  unsigned int m = (*array).m, n = (*array).n;
  double max = max_array(*array);
  double min = min_array(*array);
  double array_range = (max - min);

  /* Value used to normalize the data */
  unsigned char maxc = cmaplength - 1;

  char spaces[scale * 2 + 1];
  for (unsigned char i = 0; i < scale * 2; ++i) {
    spaces[i] = ' ';
  }
  spaces[scale * 2] = '\0';

  unsigned int cmapidx;
  unsigned char R, G, B;
  printf("\n");
  for (unsigned int i = 0; i < m; ++i) {
    for (unsigned char k = 0; k < scale; ++k) {
      printf("\t");
      for (unsigned int j = 0; j < n; ++j) {
        cmapidx = (unsigned int)(((*array).val[i * n + j] - min) * maxc /
                                 array_range);
        R = cmap[cmapidx * 3];
        G = cmap[cmapidx * 3 + 1];
        B = cmap[cmapidx * 3 + 2];
        printf(BG "%u;%u;%um%s" RST, R, G, B, spaces);
      }
      printf("\n");
    }
  }
  printf("\n");
}

void printarray(double_array array) {
  printf("\n");
  for (unsigned int i = 0; i < array.m; ++i) {
    printf("\t");
    for (unsigned int j = 0; j < array.n; ++j) {
      printf("   %.0lf", array.val[i * array.n + j]);
    }
    printf("\n\n");
  }
}

char is_data_in_window(unsigned int bbox[], AllGrids allgrids) {
  /* Check if there is data in the bounding box
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

AllGrids divide_domain(AllGrids allgrids, unsigned int step) {

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

int main(void) {
  unsigned int m = 5, n = 5;
  double cellsize = 1;
  unsigned char ngrids = 3;
  LinkedGrid *gridlist[ngrids];

  LinkedGrid grid0 = {.xllcenter = 0,
                      .yllcenter = 0,
                      .cellsize = 1,
                      .nrows = m,
                      .ncols = n,
                      .NODATA_value = 0,
                      .data = createdoublearray(m, n)};
  ascent_array(&grid0.data, 5);
  gridlist[0] = &grid0;

  LinkedGrid grid1 = {.xllcenter = n * cellsize,
                      .yllcenter = 0,
                      .cellsize = 1,
                      .nrows = m,
                      .ncols = n,
                      .NODATA_value = 0,
                      .data = createdoublearray(m, n)};
  ascent_array(&grid1.data, 50);
  gridlist[1] = &grid1;

  LinkedGrid grid2 = {.xllcenter = n * cellsize,
                      .yllcenter = m * cellsize,
                      .cellsize = 1,
                      .nrows = m,
                      .ncols = n,
                      .NODATA_value = 0,
                      .data = createdoublearray(m, n)};
  ascent_array(&grid2.data, 100);
  gridlist[2] = &grid2;

  double_array totaldomain;
  totaldomain = merge(gridlist, ngrids, 0);
  array_to_colors(&totaldomain, 2, 1);

  AllGrids allgrids;
  allgrids = link_grids(gridlist, ngrids);

  // double_array window;
  // unsigned int bbox[] = {0, 4, 0, 4};
  //  window = merge_window(allgrids, bbox);
  //  array_to_colors(&window, 2, 1);

  printf("________________\n");

  AllGrids nw_dom;
  LinkedGrid *grid = NULL;
  nw_dom = divide_domain(allgrids, 2);
  for (unsigned int i = 0; i < nw_dom.ngrids_i; ++i) {
    for (unsigned int j = 0; j < nw_dom.ngrids_j; ++j) {
      grid = nw_dom.grids_disposition[i * nw_dom.ngrids_j + j];
      if (grid == NULL) {
        continue;
      }
      array_to_colors(&(*grid).data, 2, 1);
    }
  }

  // freearray(window);
  freearray(totaldomain);
  freearray(grid0.data);
  freearray(grid1.data);
  freearray(grid2.data);
  return 0;
}
