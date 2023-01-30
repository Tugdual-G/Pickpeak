#ifndef ARRAY_H_
#define ARRAY_H_

typedef struct double_array double_array;
typedef struct int_array int_array;

double_array createdoublearray(int m, int n);

int_array createintarray(int m, int n);

void freearray(void *arrayval);

void checkmem(void);

struct double_array {
  double *val;
  int m;
  int n;
};

struct int_array {
  int *val;
  int m;
  int n;
};

#endif // ARRAY_H_
