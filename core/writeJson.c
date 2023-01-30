#include "writeJson.h"
#include <json-c/json.h>
// #include <ogr_srs_api.h>
#include <stdio.h>

json_object *create_point(double lat, double lon, double z) {
  json_object *feature = json_object_new_object();

  json_object_object_add(feature, "type", json_object_new_string("Feature"));

  json_object *geometry = json_object_new_object();
  json_object_object_add(geometry, "type", json_object_new_string("Point"));
  json_object *point = json_object_new_array();
  json_object_array_add(point, json_object_new_double(lat));
  json_object_array_add(point, json_object_new_double(lon));
  json_object_object_add(geometry, "coordinates", point);

  json_object *properties = json_object_new_object();
  json_object_object_add(properties, "elevation", json_object_new_double(z));

  json_object_object_add(feature, "geometry", geometry);
  json_object_object_add(feature, "properties", properties);

  return feature;
}

json_object *create_crs(char *name) {
  json_object *crs = json_object_new_object();
  json_object_object_add(crs, "type", json_object_new_string("name"));

  json_object *properties = json_object_new_object();
  json_object_object_add(properties, "name", json_object_new_string(name));
  json_object_object_add(crs, "properties", properties);

  return crs;
}
/*
int transformpoints(double *y, double *x, double *z, int npts) {
  OGRSpatialReferenceH epsg2153 =
      OSRNewSpatialReference(SRS_PT_LAMBERT_CONFORMAL_CONIC_2SP);
  OGRSpatialReferenceH wgs84 = OSRNewSpatialReference(SRS_WKT_WGS84_LAT_LONG);
  OGRCoordinateTransformationH trans =
      OCTNewCoordinateTransformation(epsg2153, wgs84);
  int val = OCTTransform(trans, npts, x, y, z);
  if (val == 0) {
    printf("\n transform error \n");
  }
  return 0;
}
*/

int writeJsonFile(char *filename, double *y, double *x, double *z,
                  int npoints) {
  json_object *root = json_object_new_object();
  json_object_object_add(root, "type",
                         json_object_new_string("FeatureCollection"));
  // json_object_object_add(root, "crs", create_crs("epsg:2154\0"));

  json_object *collection = json_object_new_array();
  int i;
  for (i = 0; i < npoints; i++) {
    json_object_array_add(collection, create_point(y[i], x[i], z[i]));
  }

  json_object_object_add(root, "features", collection);

  printf("The json representation:\n\n%s\n\n",
         json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY));

  FILE *fptr = NULL;
  fptr = fopen(filename, "w");
  if (fptr == NULL) {
    printf("\n ERROR Cannot create file %s \n", filename);
    exit(0);
  }
  fputs(json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY), fptr);
  fclose(fptr);

  json_object_put(root);
  return 0;
}

int main(void) {
  double y[] = {46.51817, 46.44166};
  double x[] = {-0.66170, -0.66523};
  double z[] = {100, 34};
  int npoints = 2;
  writeJsonFile("points.json", y, x, z, npoints);
  return 0;
}
