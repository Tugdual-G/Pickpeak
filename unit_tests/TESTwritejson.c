#include "../core/writeJson.h"
#include <json-c/json.h>
// #include <ogr_srs_api.h>
#include <stdio.h>

int main(void) {
  double y[] = {46.51817, 46.44166};
  double x[] = {-0.66170, -0.66523};
  double z[] = {100, 34};
  int npoints = 2;
  writeJsonFile("points.json", y, x, z, npoints);
  return 0;
}
