#include <math.h>
#include "dmg.h"

double DMG_orient(const double a[2], const double b[2], const double c[2]) {
  double abx, aby, acx, acy;

  abx = b[0] - a[0];
  aby = b[1] - a[1];
  acx = c[0] - a[0];
  acy = c[1] - a[1];

  return abx * acy - aby * acx;
}

int DMG_segSegIntersect(const double a[2], const double b[2], const double p[2], const double q[2]) {
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

double DMG_lengthsq(const double a[2], const double b[2]) {
  double abx, aby;

  abx = b[0] - a[0];
  aby = b[1] - a[1];

  return abx * abx + aby * aby;
}

double DMG_length(const double a[2], const double b[2]) {
  return sqrt(DMG_lengthsq(a, b));
}


int DMG_computeSizeMap(DMG_pMesh mesh) {
  DMG_pPoint ppt1, ppt2;
  DMG_pEdge pa;
  int ia, ip;
  double *a, *b, h;

  assert (mesh && mesh->point && mesh->edge);

  for (ip = 1 ; ip <= mesh->np ; ip++) {
    ppt1 = &mesh->point[ip];
    if (!DMG_VOK(ppt1)) continue;
    mesh->point[ip].tmp = 0;
  }

  for (ia = 1 ; ia <= mesh->na ; ia++) {
    pa = &mesh->edge[ia];
    ppt1 = &mesh->point[pa->v[0]];
    ppt2 = &mesh->point[pa->v[1]];
    if (!(DMG_VOK(ppt1) && DMG_VOK(ppt2))) {
      return DMG_FAILURE;
    }
    a = ppt1->c;
    b = ppt2->c;
    h = DMG_length(a, b);
    ppt1->h += h;
    ppt2->h += h;
    if (h < mesh->hmin) mesh->hmin = h;
    if (h > mesh->hmax) mesh->hmax = h;
    ppt1->tmp++;
    ppt2->tmp++;
  }

  for (ip = 1 ; ip <= mesh->np ; ip++) {
    ppt1 = &mesh->point[ip];
    if (!DMG_VOK(ppt1)) continue;
    if (!ppt1->tmp) {
      return DMG_FAILURE;
    }
    ppt1->h /= ppt1->tmp;
    ppt1->tmp = 0;
  }

  return DMG_SUCCESS;
}
