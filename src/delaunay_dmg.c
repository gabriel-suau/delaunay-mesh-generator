#include "dmg.h"


int DMG_delaunay(DMG_pMesh mesh) {

  DMG_initDelaunay(mesh);

  DMG_insertBdryPoints(mesh);

  DMG_enforceBndry(mesh);

  DMG_deleteBoundingBox(mesh);

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
  int i, start;

  start = 1;

  for (i = 1 ; i <= mesh->np - 4 ; i++) {
    start = DMG_insertPoint(mesh, i, start);
  }

  return DMG_SUCCESS;
}


int DMG_enforceBndry(DMG_pMesh mesh) {
  DMG_pEdge pa;
  /* DMG_pTria pt; */
  int ia, it, nt, tlist[DMG_LIST_SIZE];

  /* For each edge of the boundary, list the edges that intersect it and proceed by local swapping to recover the boundary edge */
  for (ia = 1 ; ia <=mesh->na ; ia++) {
    pa = &mesh->edge[ia];
    nt = DMG_listCrossTriangles(mesh, pa, tlist);

    for (it = 0 ; it < nt ; it++) {
      /* pt = &mesh->tria[tlist[it]]; */
    }
  }

  return DMG_SUCCESS;
}


int DMG_deleteBoundingBox(DMG_pMesh mesh) {

  return DMG_SUCCESS;
}
