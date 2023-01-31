#include "asciiGridParse.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_info(Grid raster) {
  maxv(&raster);
  printf("\n");
  printf(" ncols      : %d \n", raster.ncols);
  printf(" nrows      : %d \n", raster.nrows);
  if (raster.centered == 1) {
    printf(" xllcenter  : %.2lf \n", raster.xllcenter);
    printf(" yllcenter  : %.2lf \n", raster.yllcenter);
  } else {
    printf(" xllcorner  : %.2lf \n", raster.xllcorner);
    printf(" yllcorner  : %.2lf \n", raster.yllcorner);
  }
  printf(" cellsize   : %.2lf \n", raster.cellsize);
  if (raster.hasNODATAval == 1) {
    printf(" nodata     : not known \n");
  } else {
    printf(" nodata     : %.2lf \n", raster.NODATA_value);
  }
  printf(" maxval     : %.2lf \n", raster.maxval);
  printf(" minval     : %.2lf \n", raster.minval);
}

Grid read_ASCIIgrid(char *filename) {
  // declare variables
  header headrow;
  FILE *fp = NULL;
  Grid raster;

  raster.NODATA_value = -99999;
  raster.cellsize = 1;
  raster.nrows = 0;
  raster.ncols = 0;
  raster.data.val = NULL;
  raster.hasNODATAval = 1;
  raster.maxval = raster.NODATA_value;
  raster.minval = -raster.NODATA_value;

  fp = fopen(filename, "r");
  if (fp) {
    int i;
    for (i = 0; i < N_HEADERS; i++) {
      if (fscanf(fp, "%s %lf", headrow.name, &headrow.val) == 2) {
        if (strcoll(headrow.name, "ncols") == 0) {
          raster.ncols = (int)headrow.val;
        } else if (strcoll(headrow.name, "nrows") == 0) {
          raster.nrows = (int)headrow.val;
        } else if (strcoll(headrow.name, "xllcorner") == 0) {
          raster.xllcorner = headrow.val;
        } else if (strcoll(headrow.name, "yllcorner") == 0) {
          raster.yllcorner = headrow.val;
        } else if (strcoll(headrow.name, "cellsize") == 0) {
          raster.cellsize = headrow.val;
        } else if (strcoll(headrow.name, "NODATA_value") == 0) {
          raster.NODATA_value = headrow.val;
          raster.hasNODATAval = 0;
        } else if (strcoll(headrow.name, "xllcenter") == 0) {
          raster.xllcenter = headrow.val;
          raster.centered = 1;
        } else if (strcoll(headrow.name, "yllcenter") == 0) {
          raster.yllcenter = headrow.val;
          raster.centered = 1;
        } else {
          printf("\n HEADER NOT READEN \n");
        }
      };
    }
    if (raster.ncols == 0 || raster.nrows == 0) {
      printf("ERROR array of %d cols and %d rows", raster.ncols, raster.nrows);
      exit(1);
    }

    int m = raster.nrows, n = raster.ncols;
    raster.data = createdoublearray(m, n);
    double *array = raster.data.val;

    raster.maxval = raster.NODATA_value;
    raster.minval = -raster.NODATA_value;

    int j = 0;
    for (i = 0; i < m; i++) {
      for (j = 0; j < n; j++) {
        if (fscanf(fp, "%lf", (array + i * n + j)) != 1) {
          printf("\n erreur de lecture des valeurs numeriques \n");
        }
      }
    }

    fclose(fp);
  }
  return raster;
}

void maxv(Grid *grid) {
  // Finds the max value in the raster
  int i;
  int j = 0;
  double max;
  double min;
  max = *((*grid).data.val);
  min = *((*grid).data.val);
  for (i = 0; i < (*grid).nrows; i++) {
    for (j = 0; j < (*grid).ncols; j++) {
      if (*((*grid).data.val + i * (*grid).ncols + j) > max) {
        max = *((*grid).data.val + i * (*grid).ncols + j);
      } else if (*((*grid).data.val + i * (*grid).ncols + j) < min) {
        min = *((*grid).data.val + i * (*grid).ncols + j);
      }
    }
  }
  (*grid).maxval = max;
  (*grid).minval = min;
}

void savebinary(char *filename, Grid raster) {
  FILE *fptr;
  fptr = fopen(filename, "wb");
  if (fptr == NULL) {
    printf("not written to disk");
    exit(0);
  }
  fwrite(raster.data.val, sizeof(double), raster.ncols * raster.nrows, fptr);
  fclose(fptr);
}
