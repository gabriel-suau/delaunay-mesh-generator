#include "dmg.h"


int DMG_baryCoord(DMG_pMesh mesh, DMG_pTria pt, double c[2], double *det, double bc[3]) {
  double *pa, *pb, *pc;

  pa = mesh->point[pt->v[0]].c;
  pb = mesh->point[pt->v[1]].c;
  pc = mesh->point[pt->v[2]].c;

  /* Compute the triangle area */
  *det = DMG_computeArea(pa, pb, pc);
  *det = 1.0 / *det;

  /* Compute the barycentric coordinates */
  bc[1] = DMG_computeArea(pa, c, pc) * (*det);
  bc[2] = DMG_computeArea(pa, pb, c) * (*det);
  bc[0] = 1.0 - (bc[1] + bc[2]);

  return DMG_SUCCESS;
}


int DMG_isInTria(DMG_pMesh mesh, int k, double c[2]) {
  DMG_pTria pt;
  double det, bc[3];

  pt = &mesh->tria[k];

  DMG_baryCoord(mesh, pt, c, &det, bc);

  if (bc[0] > DMG_EPSILON && bc[1] > DMG_EPSILON && bc[2] > DMG_EPSILON)
    return 1;
  else
    return 0;
}
