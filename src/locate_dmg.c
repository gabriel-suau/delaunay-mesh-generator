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


int DMG_locTria_brute(DMG_pMesh mesh, double c[2]) {
  DMG_pTria pt;
  double *a, *b;
  int it, k, flag;

  /* Walk through the triangles */
  for (it = 1; it <= mesh->nt; it++) {
    pt = &mesh->tria[it];

    flag = 0;

    for (k = 0; k < 3; k++) {
      a = mesh->point[pt->v[DMG_tria_vert[k+1]]].c;
      b = mesh->point[pt->v[DMG_tria_vert[k+2]]].c;

      if (DMG_orient(a, b, c) < 0) {
        flag = 1;
        break;
      }
    }

    if (!flag) {
      return it;
    }
  }

  return 0; // error : point not found
}


int DMG_locTria(DMG_pMesh mesh, int start, double c[2]) {
  DMG_pTria pt;
  double *a, *b;
  int it, iter, k, flag, iadj, *adja;

  assert(mesh && mesh->tria);
  assert(start > 0);

  it = start;
  iter = 0;

  /* Walk through the triangles */
  while (iter < mesh->nt) {
    pt = &mesh->tria[it];

    iadj = 3 * it;
    adja = &mesh->adja[iadj];

    flag = 0;

    /* Iterate through the edges of triangle it */
    for (k = 0; k < 3; k++) {
      a = mesh->point[pt->v[DMG_tria_vert[k+1]]].c;
      b = mesh->point[pt->v[DMG_tria_vert[k+2]]].c;

      if (DMG_orient(a, b, c) < 0.0) {
        it = adja[k] / 3; /* mesh->adja[3 * it + k] */
        if (it == 0) {
          it = DMG_locTria_brute(mesh, c);
        }
        flag = 1;
        break;
      }
    }

    iter++;

    /* We found the triangle containing point c */
    if (!flag) break;
  }

  return it;
}
