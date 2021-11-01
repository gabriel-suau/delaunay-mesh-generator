#include "dmg.h"

#define DMG_EPSTRIA -1e-18

/* Compute the barycentric coordinates of point c in triangle pt */
/* Also return 1 if c is in triangle, 0 if c is not in triangle */
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

  if ((bc[0] > DMG_EPSTRIA) && (bc[1] > DMG_EPSTRIA) && (bc[2] > DMG_EPSTRIA))
    return 1;
  else
    return 0;
}


int DMG_locTria(DMG_pMesh mesh, int start, double c[2], double bc[3]) {
  DMG_pTria pt;
  double det;
  int it, j, iter, dir[3], *adja, iadj;

  it = start;
  iter = 0;

  while (iter < mesh->nt) {
    pt  = &mesh->tria[it];

    /* Compute the barycentric coordinates of point c in triangle it */
    if (DMG_baryCoord(mesh, pt, c, &det, bc)) break;

    dir[0] = (bc[0] < -DMG_EPSILON) ? 1 : 0;
    dir[1] = (bc[1] < -DMG_EPSILON) ? 1 : 0;
    dir[2] = (bc[2] < -DMG_EPSILON) ? 1 : 0;

    iadj = 3 * it;
    adja = &mesh->adja[iadj];

    for (j = 0 ; j < 3 ; j++) {
      if (dir[j]) {
        it = adja[j] / 3; /* mesh->adja[iadj + j] / 3 */
        break;
      }
    }

    iter++;
  }

  return it;
}
