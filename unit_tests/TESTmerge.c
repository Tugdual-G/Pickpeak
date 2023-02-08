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
  while (fscanf(fp, "%d ", &colorintensity) != EOF) {
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

LinkedGrid **link_grids(LinkedGrid **gridlists, unsigned char ngrids) {
  /* find the neighbours of each subdomains: west east south north */
  double x_ll, y_ll, x_ur, y_ur;
  get_extent(gridlists, ngrids, &x_ll, &y_ll, &x_ur, &y_ur);
  double xy_ll[] = {x_ll, y_ll};
  unsigned char xpos;
  unsigned char ypos;

  LinkedGrid **grids_disposition;
  grids_disposition =
      (LinkedGrid **)malloc(sizeof(LinkedGrid *) * ngrids * ngrids);

  for (unsigned int i = 0; i < ngrids * ngrids; ++i) {
    grids_disposition[i] = NULL;
  }

  LinkedGrid *grid_ptr;
  for (unsigned char i = 0; i < ngrids; ++i) {
    grid_ptr = gridlists[i];
    get_position(grid_ptr, xy_ll, &ypos, &xpos);
    grids_disposition[ypos * ngrids + xpos] = grid_ptr;
  }

  for (unsigned char i = 0; i < ngrids; ++i) {
    for (unsigned char j = 0; j < ngrids; ++j) {
      grid_ptr = grids_disposition[i * ngrids + j];
      if (grid_ptr == NULL) {
        continue;
      }
      (*grid_ptr).west = (j > 0) ? grids_disposition[i * ngrids + j - 1] : NULL;

      (*grid_ptr).east =
          (j < ngrids - 1) ? grids_disposition[i * ngrids + j + 1] : NULL;

      (*grid_ptr).south =
          (i > 0) ? grids_disposition[(i - 1) * ngrids + j] : NULL;

      (*grid_ptr).north =
          (i < ngrids - 1) ? grids_disposition[(i + 1) * ngrids + j] : NULL;
    }
  }
  return grids_disposition;
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

  double x_ll, y_ll;
  double x_ur, y_ur;

  double_array totaldomain;
  totaldomain = merge(gridlist, ngrids, 0);
  array_to_colors(&totaldomain, 2, 1);

  LinkedGrid **griddisposition;
  griddisposition = link_grids(gridlist, ngrids);

  double_array window;
  unsigned int bbox[] = {3, 11, 4, 7};
  window = merge_window(griddisposition, ngrids, bbox);
  array_to_colors(&window, 2, 1);

  freearray(totaldomain);
  freearray(grid0.data);
  freearray(grid1.data);
  freearray(grid2.data);
  return 0;
}
