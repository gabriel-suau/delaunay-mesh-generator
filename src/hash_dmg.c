#include "dmg.h"


int DMG_hashHedge(DMG_pMesh mesh) {

  if (!mesh || !mesh->np || !mesh->nt) {
    fprintf(stderr, "Error : %s:%d : Cannot hash the edges of an empty mesh ! \n", __func__, __LINE__);
    return DMG_FAILURE;
  }

  return DMG_SUCCESS;
}


int buildVerticesBalls2D(DMG_pMesh mesh) {
  int i, j, k, v, ntot, *nneigh;
  DMG_pTria tria;

  if (!mesh || !mesh->np || !mesh->nt) {
    fprintf(stderr, "Error : %s:%d : Cannot build the neighbours table of an empty mesh ! \n", __func__, __LINE__);
    return DMG_FAILURE;
  }

  /** Build the head table */
  nneigh = (int*) calloc(mesh->np, sizeof(int));
  mesh->head = (int*) calloc(mesh->np, sizeof(int));
  ntot = 0;

  /* Count the number of neighbour triangles for each vertex */
  for (i = 0 ; i < mesh->nt ; i++) {
    tria = &mesh->tria[i];
    nneigh[tria->v[0]]++;
    nneigh[tria->v[1]]++;
    nneigh[tria->v[2]]++;
    ntot += 3;
  }

  /* Sum the number of neighbours to build the head table */
  for (i = 1 ; i < mesh->np ; i++) {
    mesh->head[i] = nneigh[i - 1] + mesh->head[i - 1];
  }
  for (i = 0 ; i < mesh->np ; i++) {
    if (nneigh[i] == 0) mesh->head[i] = DMG_UNSET;
  }

  /** Build the second table containing the indices of the triangles */
  memset(nneigh, 0, mesh->np * sizeof(int));
  mesh->neigh = (int*) malloc(ntot * sizeof(int));
  for (i = 0 ; i < mesh->nt ; i++) {
    tria = &mesh->tria[i];
    for (j = 0 ; j < 3 ; j++) {
      v = tria->v[j];
      if (mesh->head[v] == DMG_UNSET) continue;
      k = mesh->head[v] + nneigh[v];
      mesh->neigh[k] = i;
      nneigh[v]++;
    }
  }

  free(nneigh); nneigh = NULL;

  return DMG_SUCCESS;
}
