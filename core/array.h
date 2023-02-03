#ifndef ARRAY_H_
#define ARRAY_H_

typedef struct double_array double_array;
typedef struct uint_array uint_array;

#define freearray(x)                                                           \
  _Generic((x), double_array                                                   \
           : freearrayDouble, uint_array                                       \
           : freearrayInt, default                                             \
           : freearrayDouble)(x)

double_array createdoublearray(unsigned int m, unsigned int n);

uint_array create_uintarray(unsigned int m, unsigned int n);

void freearrayDouble(double_array array);
void freearrayInt(uint_array array);

void checkmem(void);

struct double_array {
  double *val;
  unsigned int m;
  unsigned int n;
};

struct uint_array {
  unsigned int *val;
  unsigned int m;
  unsigned int n;
};

#endif // ARRAY_H_
