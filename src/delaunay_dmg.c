#include "dmg.h"


int DMG_delaunay(DMG_pMesh mesh) {

  DMG_initDelaunay(mesh);

  DMG_insertBdryPoints(mesh);

  return DMG_SUCCESS;
}


int DMG_initDelaunay(DMG_pMesh mesh) {
  DMG_pPoint ppt;
  DMG_pTria pt;
  int i, ip1, ip2, ip3, ip4, it1, it2, iadj, *adja;
  double c[2], delta[2];

  /* Compute the bounding box and set the min and max fields */
  for (i = 1 ; i <= mesh->np ; i++) {
    ppt = &mesh->point[i];
    if (ppt->c[0] < mesh->min[0]) mesh->min[0] = ppt->c[0];
    if (ppt->c[0] > mesh->max[0]) mesh->max[0] = ppt->c[0];
    if (ppt->c[1] < mesh->min[1]) mesh->min[1] = ppt->c[1];
    if (ppt->c[1] > mesh->max[1]) mesh->max[1] = ppt->c[1];
  }

  /* Create the 4 points of the bounding box */
  delta[0] = (mesh->max[0] - mesh->min[0]);
  delta[1] = (mesh->max[1] - mesh->min[1]);

  /* Bottom left corner */
  c[0] = mesh->min[0] - 0.1 * delta[0];
  c[1] = mesh->min[1] - 0.1 * delta[1];
  ip1 = DMG_newPoint(mesh, c);

  /* Bottom right corner */
  c[0] = mesh->max[0] + 0.1 * delta[0];
  c[1] = mesh->min[1] - 0.1 * delta[1];
  ip2 = DMG_newPoint(mesh, c);

  /* Top left corner */
  c[0] = mesh->min[0] - 0.1 * delta[0];
  c[1] = mesh->max[1] + 0.1 * delta[1];
  ip3 = DMG_newPoint(mesh, c);

  /* Top right corner */
  c[0] = mesh->max[0] + 0.1 * delta[0];
  c[1] = mesh->max[1] + 0.1 * delta[1];
  ip4 = DMG_newPoint(mesh, c);

  assert ( ip1 == mesh->np-3 );
  assert ( ip2 == mesh->np-2 );
  assert ( ip3 == mesh->np-1 );
  assert ( ip4 == mesh->np );

  /* Create the 2 first triangles */
  it1 = DMG_newTria(mesh);
  pt = &mesh->tria[it1];
  pt->v[0] = ip1;
  pt->v[1] = ip4;
  pt->v[2] = ip3;

  it2 = DMG_newTria(mesh);
  pt = &mesh->tria[it2];
  pt->v[0] = ip1;
  pt->v[1] = ip2;
  pt->v[2] = ip4;

  assert( it1 == mesh->nt-1 );
  assert( it2 == mesh->nt );

  /* Adjacency relations */
  iadj = 3 * it1;
  adja = &mesh->adja[iadj];
  adja[2] = 3 * it2 + 1;

  iadj = 3 * it2;
  adja = &mesh->adja[iadj];
  adja[1] = 3 * it1 + 2;

  return DMG_SUCCESS;
}


int DMG_insertBdryPoints(DMG_pMesh mesh) {
  DMG_pPoint ppt;
  int i, k, start, list[DMG_LIST_SIZE], adjcount;

  memset(list, 0, DMG_LIST_SIZE * sizeof(int));

  start = 1;

  for (i = 1 ; i <= mesh->np - 4 ; i++) {
    ppt = &mesh->point[i];
    ppt->flag = 1;
    start = DMG_locTria(mesh, start, ppt->c);
    adjcount = DMG_createCavity(mesh, ppt->c, start, list);
    printf("%d \n", adjcount);
    for (k = 0 ; k < adjcount ; k++) {
      printf("%d ", list[k]);
    }
    printf("\n");
    /* list contains the adjacency relations by which the triangles adjacent to the cavity see the cavity */
    DMG_createBall(mesh, i, adjcount, list);
    /* list now contains the indices of the newly created triangles */
    start = list[0];
  }

  return DMG_SUCCESS;
}
