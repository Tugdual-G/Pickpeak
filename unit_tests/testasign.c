#include "../core/array.h"
#include "asign_.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  double_array *arr;
  dosomething(arr);
  printf(" \n ok \n");
  printf(" %d , %d \n", (*arr).m, (*arr).n);
  freearray((*arr).val);
}
