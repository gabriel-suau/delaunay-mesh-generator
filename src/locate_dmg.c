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

/* rad is the radius squared */
int DMG_findCircumcircle(DMG_pMesh mesh, DMG_pTria pt, double c[2], double *rad) {
  double *c1, *c2, *c3, m1[2], m2[2], sl1, sl2, b1, b2;

  c1 = mesh->point[pt->v[0]].c;
  c2 = mesh->point[pt->v[0]].c;
  c3 = mesh->point[pt->v[0]].c;

  /* Middle of local edge n°1 */
  m1[0] = 0.5 * (c3[0] - c2[0]);
  m1[1] = 0.5 * (c3[1] - c2[1]);
  /* Middle of local edge n°2 */
  m2[0] = 0.5 * (c3[0] - c1[0]);
  m2[1] = 0.5 * (c3[1] - c1[1]);

  /* Inverse opposite of the slopes of the segments */
  sl1 = (c2[0] - c3[0]) / (c3[1] - c2[1]);
  sl2 = (c1[0] - c3[0]) / (c3[1] - c1[1]);

  /* Ordonee a lorigine */
  b1 = m1[1] - sl1 * m1[0];
  b2 = m2[1] - sl2 * m2[0];

  /* Circumcenter coordinates */
  c[0] = (b2 - b1) / (sl1 - sl2);
  c[1] = sl2 * c[0] + b2;

  /* Squared radius */
  *rad = (c1[0] - c[0]) * (c1[0] - c[0]) + (c1[1] - c[1]) * (c1[1] - c[1]);

  return DMG_SUCCESS;
}


int DMG_isInCircumcircle(DMG_pMesh mesh, double c[2], DMG_pTria pt) {
  double center[2], rad, dist;

  DMG_findCircumcircle(mesh, pt, center, &rad);

  dist = (c[0] - center[0]) * (c[0] - center[0]) + (c[1] - center[1]) * (c[1] - center[1]);

  return (dist < rad ? 1 : 0);
}


int DMG_createCavity(DMG_pMesh mesh, double c[2], int it, int *list) {
  DMG_pTria pt;
  int i;

  list  = (int*)malloc(sizeof(int));
  list[0] = it;

  pt = &mesh->tria[it];
  
  return DMG_SUCCESS;
}
