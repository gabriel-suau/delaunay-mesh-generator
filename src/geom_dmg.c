#include "dmg.h"

#define DMG_EPSTRIA -1e-18


double DMG_orient(double a[2], double b[2], double c[2]) {
  double abx, aby, acx, acy;

  abx = b[0] - a[0];
  aby = b[1] - a[1];
  acx = c[0] - a[0];
  acy = c[1] - a[1];

  return abx * acy - aby * acx;
}


double DMG_inCircle(double a[2], double b[2], double c[2], double d[2]) {
  double adx, ady, bdx, bdy, cdx, cdy;
  double abdet, bcdet, cadet;
  double adsq, bdsq, cdsq;

  adx = a[0] - d[0];
  ady = a[1] - d[1];
  bdx = b[0] - d[0];
  bdy = b[1] - d[1];
  cdx = c[0] - d[0];
  cdy = c[1] - d[1];

  abdet = adx * bdy - bdx * ady;
  bcdet = bdx * cdy - cdx * bdy;
  cadet = cdx * ady - adx * cdy;
  adsq = adx * adx + ady * ady;
  bdsq = bdx * bdx + bdy * bdy;
  cdsq = cdx * cdx + cdy * cdy;

  return adsq * bcdet + bdsq * cadet + cdsq * abdet;
}


/* Compute the barycentric coordinates of point c in triangle pt */
/* Also return 1 if c is in triangle, 0 if c is not in triangle */
int DMG_baryCoord(DMG_pMesh mesh, DMG_pTria pt, double c[2], double *det, double bc[3]) {
  double *pa, *pb, *pc;

  pa = mesh->point[pt->v[0]].c;
  pb = mesh->point[pt->v[1]].c;
  pc = mesh->point[pt->v[2]].c;

  /* Compute the triangle area */
  *det = DMG_orient(pa, pb, pc);
  *det = 1.0 / *det;

  /* Compute the barycentric coordinates */
  bc[1] = DMG_orient(pa, c, pc) * (*det);
  bc[2] = DMG_orient(pa, pb, c) * (*det);
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


int DMG_createCavity(DMG_pMesh mesh, double c[2], int it, int *list) {
  DMG_pTria pt;
  int i;

  list  = (int*)malloc(sizeof(int));
  list[0] = it;

  pt = &mesh->tria[it];
  
  return DMG_SUCCESS;
}

