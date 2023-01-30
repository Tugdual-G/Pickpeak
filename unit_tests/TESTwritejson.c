#include "../core/writeJson.h"
#include <json-c/json.h>
// #include <ogr_srs_api.h>
#include "../core/array.h"
#include <stdio.h>

int main(void) {
  unsigned int n = 2;
  double_array y = createdoublearray(1, n);
  double_array x = createdoublearray(1, n);
  double_array z = createdoublearray(1, n);
  y.val[0] = 134;
  y.val[1] = 345;
  x.val[0] = 234;
  x.val[1] = 245;
  z.val[0] = 34;
  z.val[1] = 24;
  writeJsonFile("points.json", y, x, z);
  return 0;
}
