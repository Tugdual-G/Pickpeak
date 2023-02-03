#include "asciiGridParse.h"
#include "array.h"
#include "parseArgs.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define STR1(x) #x
#define STR(x) STR1(x)
#define max_str_length 16

Grid read_ASCII_header(char *filename) {
  /* Read and store the discretisation parameters given
   * in the header of the ASCII file */
  header headrow;
  FILE *fp = NULL;
  Grid raster = {
      .ncols = 0,
      .nrows = 0,
      .xllcenter = 0,
      .xllcorner = 0,
      .yllcenter = 0,
      .yllcorner = 0,
      .NODATA_value = -99999,
      .hasNODATAval = 0,
      .centered = 0,
      .f_position = 0,
      .data.val = NULL,
  };

  /* In case max and min are not attributed */
  raster.maxval = raster.NODATA_value;
  raster.minval = raster.NODATA_value;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("\n ERROR can't open file %s \n", filename);
    exit(1);
  }
  for (unsigned int i = 0; i < N_HEADERS; i++) {
    if (fscanf(fp, "%24[a-zA-Z_] %lf ", headrow.name, &headrow.val) == 2) {
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
        printf("\n ERROR: unknown header : %s \n", headrow.name);
        exit(1);
      }
    }
  }
  if (raster.ncols == 0 || raster.nrows == 0) {
    printf("ERROR array of %d cols and %d rows \n", raster.ncols, raster.nrows);
    exit(1);
  }

  if ((raster.f_position = ftell(fp)) == -1) {
    printf("\n ERROR cannot find position in file \n");
    exit(1);
  }
  raster.maxval = raster.NODATA_value;
  raster.minval = raster.NODATA_value;
  fclose(fp);
  return raster;
}

// void read_ASCII_data(Grid *grid, char fname[]) {
//
///* Reading the ascii raster data and storing
//* it in an array*/
// FILE *fp = NULL;
// fp = fopen(fname, "r");
// if (fp == NULL) {
// printf("\n ERROR can't open file %s \n", fname);
// exit(1);
//}
//
///* Set the file pointer a the right place in the file
//* to start reading the data, and not the header
//*/
// if (fseek(fp, (*grid).f_position, SEEK_SET) != 0) {
// printf("\n ERROR: cannot find file stream position \n");
// exit(1);
//}
//
// unsigned int m = (*grid).nrows, n = (*grid).ncols;
//(*grid).data = createdoublearray(m, n);
// double *array = (*grid).data.val;
//
///* Assuming EOF is not equal to 1  */
// unsigned int i = 0;
// while (fscanf(fp, "%lf ", (array + i)) == 1) {
//++i;
//}
// if (i != m * n) {
// printf("\n DATA PARSING ERROR \n");
// exit(1);
//}
//
// fclose(fp);
//}
void read_ASCII_data(Grid *grid, char fname[]) {

  /* Reading the ascii raster data and storing
   * it in an array*/
  FILE *fp = NULL;
  fp = fopen(fname, "r");
  if (fp == NULL) {
    printf("\n ERROR can't open file %s \n", fname);
    exit(1);
  }

  /* Set the file pointer a the right place in the file
   * to start reading the data, and not the header
   */
  if (fseek(fp, (*grid).f_position, SEEK_SET) != 0) {
    printf("\n ERROR: cannot find file stream position \n");
    exit(1);
  }

  unsigned int m = (*grid).nrows, n = (*grid).ncols;
  (*grid).data = createdoublearray(m, n);
  double *array = (*grid).data.val;

  /* Assuming EOF is not equal to 1  */

  char temp_str[max_str_length + 1] = {'\0'};
  unsigned int i = 0, j = 0;

  char flag = EOF + 1;
  j = 0;
  while (flag != EOF) {
    while (((temp_str[j] = getc(fp)) != '\n') && (temp_str[j] != ' ') &&
           (temp_str[j] != EOF) && (j < max_str_length)) {
      j++;
    }
    flag = temp_str[j];
    temp_str[j] = '\0';
    *(array + i) = atof(temp_str);

    while ((flag != '\n') && (flag != ' ') && (flag != EOF)) {
      flag = getc(fp);
    }

    // printf(" %s, ", temp_str);

    while (((flag == '\n') || (flag == ' ')) && (flag != EOF)) {
      flag = getc(fp);
    }
    temp_str[0] = flag;
    j = 1;
    i++;
  }

  if (i != m * n) {
    printf("\n DATA PARSING ERROR \n");
    exit(1);
  }

  fclose(fp);
}

void print_info(Grid *raster) {
  maxv(raster);
  printf("\n");
  printf(" ncols      : %d \n", (*raster).ncols);
  printf(" nrows      : %d \n", (*raster).nrows);
  if ((*raster).centered == 1) {
    printf(" xllcenter  : %.2lf \n", (*raster).xllcenter);
    printf(" yllcenter  : %.2lf \n", (*raster).yllcenter);
  } else {
    printf(" xllcorner  : %.2lf \n", (*raster).xllcorner);
    printf(" yllcorner  : %.2lf \n", (*raster).yllcorner);
  }
  printf(" cellsize   : %.2lf \n", (*raster).cellsize);
  if ((*raster).hasNODATAval == 1) {
    printf(" nodata     : unknown \n");
  } else {
    printf(" nodata     : %.2lf \n", (*raster).NODATA_value);
  }
  printf(" maxval     : %.2lf \n", (*raster).maxval);
  printf(" minval     : %.2lf \n", (*raster).minval);
  fflush(stdout);
}

void maxv(Grid *grid) {
  /* Finds the max value in the raster */
  double max = (*grid).NODATA_value;
  double min = (*grid).NODATA_value;
  double *x = (*grid).data.val;
  unsigned int l = (*grid).ncols * (*grid).nrows;

  /* getting rid of the eventual nodata values at the begining
   * of the parsing, to initialize max and min correctly
   */
  unsigned int i = 0;
  while ((*(x + i) == (*grid).NODATA_value) && (i < (l - 1))) {
    i++;
  }
  max = *(x + i);
  min = *(x + i);
  for (i = i; i < l; i++) {
    if (*(x + i) > max && (*(x + i) != (*grid).NODATA_value)) {
      max = *(x + i);
    } else if (*(x + i) < min && (*(x + i) != (*grid).NODATA_value)) {
      min = *(x + i);
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
    exit(1);
  }
  fwrite(raster.data.val, sizeof(double), raster.ncols * raster.nrows, fptr);
  fclose(fptr);
}
