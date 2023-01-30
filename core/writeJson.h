#ifndef WRITEJSON_H_
#define WRITEJSON_H_
#include <json-c/json.h>

json_object *create_point(double lat, double lon, double z);

json_object *create_crs(char *name);

int writeJsonFile(char *filename, double *y, double *x, double *z, int npoints);

#endif // WRITEJSON_H_
