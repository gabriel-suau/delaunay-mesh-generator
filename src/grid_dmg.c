#include "dmg.h"

DMG_Grid *DMG_createGrid(double min[2], double max[2], double h) {
  DMG_Grid *g = (DMG_Grid*)malloc(sizeof(DMG_Grid));

  if (g == NULL) return NULL;

  g->h = h;
  g->xmin = min[0];
  g->ymin = min[1];
  g->nx = (int)(max[0] - min[0]) / h;
  g->ny = (int)(max[1] - min[1]) / h;
  g->ucell = (int*)calloc(g->nx * g->ny, sizeof(int));

  return g;
}

void DMG_freeGrid(DMG_Grid *g) {

  if (g == NULL) return;

  free(g->ucell);
  free(g);
  g = NULL;
}

int DMG_gCell(DMG_Grid *g, double c[2]) {
  int i, j;

  j = (int)(c[0] / g->h);
  i = (int)(c[1] / g->h);

  return i * g->nx + j;
}
