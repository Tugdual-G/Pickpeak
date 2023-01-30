#include "array.h"
#include <stdio.h>
#include <stdlib.h>

static int createcount = 0;
static int freecount = 0;

double_array createdoublearray(int m, int n) {
  // create 1d or 2d arrays, 1d have shape (0,n)
  if (n == 0 || m == 0) {
    printf("\n ERROR cannot create array with shape 0 \n");
    exit(1);
  }
  double_array array = {.m = m, .n = n};
  array.val = NULL;
  array.val = (double *)malloc(sizeof(double) * (size_t)m * (size_t)n);
  if (array.val == NULL) {
    printf("\n ERROR allocation failled");
    exit(1);
  }
  createcount++;
  return array;
}

int_array createintarray(int m, int n) {
  // create 1d or 2d arrays, 1d have shape (0,n)
  if (n == 0 || m == 0) {
    printf("\n ERROR cannot create array with shape 0 \n");
    exit(1);
  }
  int_array array = {.m = m, .n = n};
  array.val = NULL;
  array.val = (int *)malloc(sizeof(int) * (size_t)m * (size_t)n);
  if (array.val == NULL) {
    printf("\n ERROR allocation failled");
    exit(1);
  }
  createcount++;
  return array;
}

void freearray(void *arrayval) {
  if ((arrayval) != NULL) {
    free(arrayval);
  } else {
    printf("\n ERROR cannot free array ");
    exit(1);
  }
  freecount++;
}

void checkmem(void) {
  // a bit useless
  if (createcount - freecount != 0) {
    printf(" \n Memory not released \n");
  }
}
