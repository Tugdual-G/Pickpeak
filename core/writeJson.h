#ifndef WRITEJSON_H_
#define WRITEJSON_H_
#include "array.h"
#include <json-c/json.h>
json_object *createFeatureCollection(double *y, double *x, double *z,
                                     int npoints);

json_object *create_point(double lat, double lon, double z);

json_object *create_crs(char *name);

int writeJsonFile(char *filename, double_array y, double_array x,
                  double_array z);

void printJson(json_object *json);

#endif // WRITEJSON_H_
