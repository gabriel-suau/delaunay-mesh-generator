#include "dmg.h"

#define DMG_EPSTRIA -1e-18

double DMG_orient(const double a[2], const double b[2], const double c[2]) {
  double abx, aby, acx, acy;

  abx = b[0] - a[0];
  aby = b[1] - a[1];
  acx = c[0] - a[0];
  acy = c[1] - a[1];

  return abx * acy - aby * acx;
}

int DMG_segIntersect(const double a[2], const double b[2], const double p[2], const double q[2]) {
  double abx, aby, apx, apy, pqx, pqy;
  double ab_ap, ab_pq, ap_pq;

  abx = b[0] - a[0];
  aby = b[1] - a[1];
  apx = p[0] - a[0];
  apy = p[1] - a[1];
  pqx = q[0] - p[0];
  pqy = q[1] - p[1];

  ab_ap = abx * apy - aby * apx;
  ab_pq = abx * pqy - aby * pqx;
  ap_pq = apx * pqy - apy * pqx;

  if ((ab_ap * (ab_ap + ab_pq) < 0) && (ap_pq * (ap_pq - ab_pq)) < 0)
    return 1;
  else
    return 0;
}

int DMG_triaSegIntersect(const double a[2], const double b[2], const double c[2], const double p[2], const double  q[2]) {
  double abx, aby, apx, apy, pqx, pqy, bcx, bcy, bpx, bpy, cax, cay, cpx, cpy;
  double ab_ap, ab_pq, ap_pq, bc_bp, bc_pq, bp_pq, ca_cp, ca_pq, cp_pq;
  int code;

  code = 0;

  /* Compute vectors coordinates */
  abx = b[0] - a[0];
  aby = b[1] - a[1];
  apx = p[0] - a[0];
  apy = p[1] - a[1];
  pqx = q[0] - p[0];
  pqy = q[1] - p[1];

  bcx = c[0] - b[0];
  bcy = c[1] - b[1];
  bpx = p[0] - b[0];
  bpy = p[1] - b[1];

  cax = a[0] - c[0];
  cay = a[1] - c[1];
  cpx = p[0] - c[0];
  cpy = p[1] - c[1];

  /* Compute cross products */
  ab_ap = abx * apy - aby * apx;
  ab_pq = abx * pqy - aby * pqx;
  ap_pq = apx * pqy - apy * pqx;
  bc_pq = bcx * pqy - bcy * pqx;
  bc_bp = bcx * bpy - bcy * bpx;
  ca_cp = cax * cpy - cay * cpx;

  /* Deduce the three other cross products */
  bp_pq = ap_pq - ab_pq;
  ca_pq = -(ap_pq + bc_pq);
  cp_pq = ca_pq * ap_pq;

  /* Test edge (ab) */
  if ((ab_ap * (ab_ap + ab_pq) < 0.0) && (ap_pq * (ap_pq - ab_pq) < 0.0))
    code += 1;

  /* Test edge (bc) */
  if ((bc_bp * (bc_bp + bc_pq) < 0.0) && (bp_pq * (ap_pq - bc_pq) < 0.0))
    code += 2;

  /* Test edge (ca) */
  if ((ca_cp * (ca_cp + ca_pq) < 0.0) && (cp_pq * (ap_pq - ca_pq) < 0.0))
    code += 4;

  return code;
}

double DMG_inCircle(const double a[2], const double b[2], const double c[2], const double d[2]) {
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


int DMG_chkDelaunay(DMG_pMesh mesh) {
  DMG_pTria pt;
  int it, jt, k, iadj, *adja;
  double *a, *b, *c, *d;

  for (it = 1 ; it <= mesh->nt ; it++) {
    pt = &mesh->tria[it];

    a = mesh->point[pt->v[0]].c;
    b = mesh->point[pt->v[1]].c;
    c = mesh->point[pt->v[2]].c;

    iadj = 3 * it;
    adja = &mesh->adja[iadj];

    for (k = 0 ; k < 3 ; k++) {
      jt = adja[k] / 3;
      pt = &mesh->tria[jt];
      d = mesh->point[pt->v[DMG_tria_vert[k]]].c;
      if (DMG_inCircle(a, b, c, d) > 0) {
        fprintf(stderr, "Warning : element %d and vertex %d violate the delaunay criterion.", it, pt->v[DMG_tria_vert[k]]);
        return 0;
      }
    }
  }

  return 1;
}
