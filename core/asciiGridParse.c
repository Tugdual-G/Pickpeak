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

#include "asciiGridParse.h"
#include "array.h"
#include "parseArgs.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR1(x) #x /* Macro to stringify constants */
#define STR(x) STR1(x)
#define MAX_SIGNIFICANT_DIGITS 16

Grid read_ASCII_header(char *filename) {
  /*
   * Read and store the metadata given
   * in the header of the ASCII file.
   * The metadata parsing is not case sensitive.
   * Otherwise the identification is strict.
   * */

  header temp_header_row;
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

  FILE *fp = NULL;
  fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("\n ERROR can't open file %s \n", filename);
    exit(1);
  }
  /* Checking for every possible header information */
  for (unsigned int i = 0; i < MAX_META; ++i) {
    if (fscanf(fp, "%24[a-zA-Z_] %lf ", temp_header_row.name,
               &temp_header_row.val) == 2) {
      if (strcoll(temp_header_row.name, "ncols") == 0) {
        raster.ncols = (int)temp_header_row.val;
      } else if (strcoll(temp_header_row.name, "nrows") == 0) {
        raster.nrows = (int)temp_header_row.val;
      } else if (strcoll(temp_header_row.name, "xllcorner") == 0) {
        raster.xllcorner = temp_header_row.val;
      } else if (strcoll(temp_header_row.name, "yllcorner") == 0) {
        raster.yllcorner = temp_header_row.val;
      } else if (strcoll(temp_header_row.name, "cellsize") == 0) {
        raster.cellsize = temp_header_row.val;
      } else if (strcoll(temp_header_row.name, "NODATA_value") == 0) {
        raster.NODATA_value = temp_header_row.val;
        raster.hasNODATAval = 0;
      } else if (strcoll(temp_header_row.name, "xllcenter") == 0) {
        raster.xllcenter = temp_header_row.val;
        raster.centered = 1;
      } else if (strcoll(temp_header_row.name, "yllcenter") == 0) {
        raster.yllcenter = temp_header_row.val;
        raster.centered = 1;
      } else {
        printf("\n ERROR: unknown header : %s \n", temp_header_row.name);
        exit(1);
      }
    }
  }
  if (raster.ncols == 0 || raster.nrows == 0) {
    printf("ERROR array of %d cols and %d rows \n", raster.ncols, raster.nrows);
    exit(1);
  }

  /* Keeping the starting position of the data in the file */
  if ((raster.f_position = ftell(fp)) == -1) {
    printf("\n ERROR cannot find position in file \n");
    exit(1);
  }
  raster.maxval = raster.NODATA_value;
  raster.minval = raster.NODATA_value;
  fclose(fp);
  return raster;
}

void read_ASCII_data(Grid *grid, char fname[]) {

  /* Reading the ascii raster data and storing
   * it in an array.
   *
   * NOTE : This method is slow since it probably has to
   * make more sistem call in the background than
   * the read_ASII_data_fast method.
   */
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

  /* Place to store the data */
  unsigned int m = (*grid).nrows, n = (*grid).ncols;
  (*grid).data = createdoublearray(m, n);
  double *array = (*grid).data.val;

  /* Data parsing */
  /* Assuming EOF is not equal to 1  */
  unsigned int i = 0;
  while (fscanf(fp, "%lf ", (array + i)) == 1) {
    ++i;
  }
  if (i != m * n) {
    printf("\n DATA PARSING ERROR \n");
    exit(1);
  }
  fclose(fp);
}

void read_ASCII_data_fast(Grid *grid, char fname[]) {

  /* Reading the ascii raster data and storing
   * it in an array
   *
   * NOTE : This function loads the data into the RAM
   * Which is way faster than using multiples
   * fread.
   *
   * NOTE : If there is more figures by value
   *  in the data than MAX_SIGNIFICANT_DIGITS,
   *  they are discarded.
   *
   *
   * WARNING : the full fille is loaded in one chunk.
   */

  FILE *fp = NULL;
  fp = fopen(fname, "rb");
  if (fp == NULL) {
    printf("\n ERROR can't open file %s \n", fname);
    exit(1);
  }

  /* In order to measure the size of the file */
  if (fseek(fp, 0, SEEK_END) != 0) {
    printf("\n ERROR: cannot find file stream position \n");
    exit(1);
  }

  /* Getting the size of the data we are about to read */
  size_t data_size = (size_t)((ftell(fp)) - (*grid).f_position);

  /* Set the file stream a the right place
   * to start reading the data
   */
  if (fseek(fp, (*grid).f_position, SEEK_SET) != 0) {
    printf("\n ERROR: cannot find file stream position \n");
    exit(1);
  }

  /* Place to store the data in it's final form */
  unsigned int m = (*grid).nrows, n = (*grid).ncols;
  (*grid).data = createdoublearray(m, n);
  double *array = (*grid).data.val;

  /* Temp storage for the ascii form of a data point */
  char temp_str[MAX_SIGNIFICANT_DIGITS + 1] = {'\0'};
  unsigned int i = 0, j = 0, k = 0;

  /* storage of the full file in it's original format */
  char *data;
  data = NULL;
  data = (char *)malloc(sizeof(char) * data_size);
  if (data == NULL) {
    printf("\n ERROR cannot allocate memory \n");
    exit(1);
  }
  if (fread(data, sizeof(char), data_size, fp) != data_size) {
    printf("\n ERROR cannot read data \n");
    exit(1);
  }

  fclose(fp);
  /* Start of the parsing process */
  k = 0;
  while (k < data_size) {
    /* Scaning values one by one */
    j = 0;
    while ((data[k] != '\n') && (data[k] != ' ') &&
           (j < MAX_SIGNIFICANT_DIGITS) && (k < data_size)) {
      temp_str[j] = data[k];
      ++j;
      ++k;
    }

    temp_str[j] = '\0';
    *(array + i) = atof(temp_str);

    /* going through the eventual remainings figures */
    while ((data[k] != '\n') && (data[k] != ' ') && (k < data_size)) {
      ++k;
    }

    /* Going through the space or newline caracters separing the data values */
    while (((data[k] == '\n') || (data[k] == ' ')) && (k < data_size)) {
      ++k;
    }
    ++i;
  }

  if (i != m * n) {
    printf("\n DATA PARSING ERROR, %d elements readen instead of %d \n", i,
           m * n);
    exit(1);
  }

  free(data);
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
    ++i;
  }
  max = *(x + i);
  min = *(x + i);
  for (i = i; i < l; ++i) {
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
