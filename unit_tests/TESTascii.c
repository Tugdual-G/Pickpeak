#include "../core/asciiGridParse.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  char fname[] = "BDALTIV2_25M_FXX_0275_6650_MNT_LAMB93_IGN69.asc";
  Grid raster;
  raster = read_ASCIIgrid(fname);
  print_info(raster);
  savebinary("binary.double", raster);
  free(raster.data.val);
}
