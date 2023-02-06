#include "../core/array.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Grid {
  double_array data;
  double xllcenter;
  double yllcenter;
  double cellsize;
} Grid;

typedef struct Gridlinklist {
  Grid *grid;
  struct Gridlinklist *next;
} Gridlinklist;

void init_array(double_array *array, double val) {
  for (unsigned int i = 0; i < (*array).m * (*array).n; ++i) {
    (*array).val[i] = val;
  }
}

void get_extent(Gridlinklist grid, double *x_ll, double *y_ll, double *x_ur,
                double *y_ur) {

  *x_ll = (*grid.grid).xllcenter;
  *y_ll = (*grid.grid).yllcenter;
  *x_ur = (*grid.grid).xllcenter;
  *y_ur = (*grid.grid).yllcenter;
  double temp_xur;
  double temp_yur;
  double dx = ((*grid.grid).cellsize);

  temp_xur = (*grid.grid).xllcenter + dx * ((*grid.grid).data.n - 1);
  temp_yur = (*grid.grid).yllcenter + dx * ((*grid.grid).data.m - 1);

  while (grid.next != NULL) {
    grid = *grid.next;
    if (*x_ll > (*grid.grid).xllcenter) {
      *x_ll = (*grid.grid).xllcenter;
    }
    if (*y_ll > (*grid.grid).yllcenter) {
      *y_ll = (*grid.grid).yllcenter;
    }

    temp_xur = (*grid.grid).xllcenter + dx * ((*grid.grid).data.n - 1);
    if (*x_ur < temp_xur) {
      *x_ur = temp_xur;
    }

    temp_yur = (*grid.grid).yllcenter + dx * ((*grid.grid).data.m - 1);
    if (*y_ur < temp_yur) {
      *y_ur = temp_yur;
    }
  }
}

Gridlinklist *add_elem(Gridlinklist *head, Grid *nextgrid) {
  Gridlinklist *newhead;
  newhead = (Gridlinklist *)malloc(sizeof(Gridlinklist));
  (*newhead).grid = nextgrid;
  (*newhead).next = NULL;
  (*head).next = newhead;
  return newhead;
}

void freeGridlinklist(Gridlinklist *start) {
  Gridlinklist *prev;
  Gridlinklist *current = start;
  while ((*current).next != NULL) {
    prev = current;
    current = (*current).next;
    if ((*(*prev).grid).data.val != NULL) {
      free((*(*prev).grid).data.val);
    }
    if (prev != NULL) {
      free(prev);
    }
  }
}

void paint_array(double_array *patch, unsigned int i0, unsigned int j0,
                 double_array *canva) {
  unsigned int mp = (*patch).m, np = (*patch).n;
  unsigned int nc = (*canva).n;

  for (unsigned int i = 0; i < mp; ++i) {
    for (unsigned int j = 0; j < np; ++j) {
      (*canva).val[(i0 + i) * nc + (j0 + j)] = (*patch).val[i * np + j];
    }
  }
}

double_array merge(Gridlinklist *root, double nodata) {
  double x_ll, y_ll;
  double x_ur, y_ur;
  double dx = (*(*root).grid).cellsize;

  get_extent(*root, &x_ll, &y_ll, &x_ur, &y_ur);

  unsigned int m_t, n_t;
  m_t = (y_ur - y_ll) / dx + 1;
  n_t = (x_ur - x_ll) / dx + 1;

  double_array totaldomain = createdoublearray(m_t, n_t);
  init_array(&totaldomain, nodata);

  unsigned int i0, j0;
  Gridlinklist *grid = root;
  while (grid != NULL) {
    i0 = ((*(*grid).grid).yllcenter - y_ll) / dx; // TODO might fail
    j0 = ((*(*grid).grid).xllcenter - x_ll) / dx; // TODO might fail
    paint_array(&(*(*grid).grid).data, i0, j0, &totaldomain);
    grid = (*grid).next;
  }
  return totaldomain;
}

void printarray(double_array array) {
  printf("\n");
  for (unsigned int i = 0; i < array.m; ++i) {
    printf("\t");
    for (unsigned int j = 0; j < array.n; ++j) {
      printf("   %.0lf", array.val[i * array.n + j]);
    }
    printf("\n\n");
  }
}

int main(void) {
  unsigned int m = 5, n = 5;
  double cellsize = 1;
  Grid grid0 = {.xllcenter = -1,
                .yllcenter = -1,
                .cellsize = 1,
                .data = createdoublearray(m, n)};
  init_array(&grid0.data, 1);

  Gridlinklist root = {.grid = &grid0, .next = NULL};
  Gridlinklist *head;

  Grid grid1 = {.xllcenter = n * cellsize,
                .yllcenter = 0,
                .cellsize = 1,
                .data = createdoublearray(m, n)};
  init_array(&grid1.data, 2);
  head = add_elem(&root, &grid1);

  Grid grid2 = {.xllcenter = n * cellsize,
                .yllcenter = m * cellsize,
                .cellsize = 1,
                .data = createdoublearray(m + 1, n + 3)};
  init_array(&grid2.data, 3);
  head = add_elem(head, &grid2);

  double x_ll, y_ll;
  double x_ur, y_ur;

  get_extent(root, &x_ll, &y_ll, &x_ur, &y_ur);

  printf("\n x_ll:%lf, y_ll:%lf, x_ur:%lf, y_ur:%lf \n", x_ll, y_ll, x_ur,
         y_ur);

  double_array totaldomain;
  totaldomain = merge(&root, 0);
  printarray(totaldomain);

  freeGridlinklist(root.next);
  return 0;
}
