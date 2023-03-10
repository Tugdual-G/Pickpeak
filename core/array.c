#include "array.h"
#include <stdio.h>
#include <stdlib.h>

static int createcount = 0;
static int freecount = 0;

double_array createdoublearray(unsigned int m, unsigned int n) {
  // create 1d or 2d arrays, 1d have shape (0,n)
  if (n <= 0 || m <= 0) {
    printf("\n ERROR cannot create array type double with shape 0 \n");
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

uint_array create_uintarray(unsigned int m, unsigned int n) {
  // create 1d or 2d arrays, 1d have shape (0,n)
  if (n <= 0 || m <= 0) {
    printf("\n ERROR cannot create integer array with shape 0 \n");
    exit(1);
  }
  uint_array array = {.m = m, .n = n};
  array.val = NULL;
  array.val =
      (unsigned int *)malloc(sizeof(unsigned int) * (size_t)m * (size_t)n);
  if (array.val == NULL) {
    printf("\n ERROR allocation failled");
    exit(1);
  }
  createcount++;
  return array;
}

void freearrayDouble(double_array array) {
  if ((array.val) != NULL) {
    free(array.val);
  } else {
    printf("\n ERROR cannot free array ");
    exit(1);
  }
  freecount++;
}
void freearrayInt(uint_array array) {
  if ((array.val) != NULL) {
    free(array.val);
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
