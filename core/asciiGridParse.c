#include "asciiGridParse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int alloccount = 0;
static int freecount = 0;

void print_info(Grid raster) {
  printf("\n");
  printf(" ncols      : %d \n", raster.ncols);
  printf(" nrows      : %d \n", raster.nrows);
  if (raster.centered == 1) {
    printf(" xllcenter  : %lf \n", raster.xllcenter);
    printf(" yllcenter  : %lf \n", raster.yllcenter);
  } else {
    printf(" xllcorner  : %lf \n", raster.xllcorner);
    printf(" yllcorner  : %lf \n", raster.yllcorner);
  }
  printf(" cellsize   : %lf \n", raster.cellsize);
  if (raster.hasNODATAval == 1) {
    printf(" nodata     : not known \n");
  } else {
    printf(" nodata     : %lf \n", raster.NODATA_value);
  }
  printf(" maxval     : %lf \n", raster.maxval);
  printf(" minval     : %lf \n", raster.minval);
}

Grid *read_ASCIIgrid(char *filename) {
  // declare variables
  header headrow;
  FILE *fp = NULL;
  Grid *raster = NULL;
  raster = malloc(sizeof(Grid));
  if (raster == NULL) {
    printf("\n ALLOCATION ERROR in read_ASCIIgrid \n");
    exit(1);
  }
  alloccount++;

  (*raster).NODATA_value = -99999;
  (*raster).nrows = 0;
  (*raster).ncols = 0;
  (*raster).data = NULL;
  (*raster).hasNODATAval = 1;
  (*raster).maxval = (*raster).NODATA_value;
  (*raster).minval = -(*raster).NODATA_value;

  fp = fopen(filename, "r");
  if (fp) {

    int i;
    for (i = 0; i < N_HEADERS; i++) {
      if (fscanf(fp, "%s %lf", headrow.name, &headrow.val) == 2) {
        if (strcoll(headrow.name, "ncols") == 0) {
          (*raster).ncols = (int)headrow.val;
        } else if (strcoll(headrow.name, "nrows") == 0) {
          (*raster).nrows = (int)headrow.val;
        } else if (strcoll(headrow.name, "xllcorner") == 0) {
          (*raster).xllcorner = headrow.val;
        } else if (strcoll(headrow.name, "yllcorner") == 0) {
          (*raster).yllcorner = headrow.val;
        } else if (strcoll(headrow.name, "cellsize") == 0) {
          (*raster).cellsize = headrow.val;
        } else if (strcoll(headrow.name, "NODATA_value") == 0) {
          (*raster).NODATA_value = headrow.val;
          (*raster).hasNODATAval = 0;
        } else if (strcoll(headrow.name, "xllcenter") == 0) {
          (*raster).xllcenter = headrow.val;
          (*raster).centered = 1;
        } else if (strcoll(headrow.name, "yllcenter") == 0) {
          (*raster).yllcenter = headrow.val;
          (*raster).centered = 1;
        } else {
          printf("\n HEADER NOT READEN \n");
        }
      };
    }
    if ((*raster).ncols == 0 || (*raster).nrows == 0) {
      printf("ERROR array of %d cols and %d rows", (*raster).ncols,
             (*raster).nrows);
      exit(1);
    }

    (*raster).data =
        (double *)malloc(sizeof(double) * (*raster).nrows * (*raster).ncols);
    if ((*raster).data == NULL) {
      printf("ERROR no memory allocated for (*raster).data");
      exit(1);
    }
    alloccount++;

    (*raster).maxval = (*raster).NODATA_value;
    (*raster).minval = -(*raster).NODATA_value;

    int j = 0;
    int m = (*raster).nrows, n = (*raster).ncols;
    for (i = 0; i < m; i++) {
      for (j = 0; j < n; j++) {
        if (fscanf(fp, "%lf", ((*raster).data + i * n + j)) != 1) {
          printf("\n erreur de lecture des valeurs numeriques \n");
        } else if (*((*raster).data + i * n + j) < (*raster).minval) {
          (*raster).minval = *((*raster).data + i * n + j);
        } else if (*((*raster).data + i * n + j) > (*raster).maxval) {
          (*raster).maxval = *((*raster).data + i * n + j);
        }
      }
    }

    fclose(fp);
    return raster;
  }
  return NULL;
}

void savebinary(char *filename, Grid raster) {
  FILE *fptr;
  fptr = fopen(filename, "wb");
  if (fptr == NULL) {
    printf("not written to disk");
    exit(0);
  }
  fwrite(raster.data, sizeof(double), raster.ncols * raster.nrows, fptr);
  fclose(fptr);
}
