#include "dmg.h"


double DMG_computeTriaArea(DMG_pMesh mesh, DMG_pTria pt) {
  double *a, *b, *c;

  a = mesh->point[pt->v[0]].c;
  b = mesh->point[pt->v[1]].c;
  c = mesh->point[pt->v[2]].c;

  return DMG_orient(a, b, c);
}


double DMG_computeTriaQual(DMG_pMesh mesh, DMG_pTria pt) {
  double abx, aby, acx, acy, bcx, bcy;
  double *a, *b, *c, area, h1, h2, h3, ht;

  a = mesh->point[pt->v[0]].c;
  b = mesh->point[pt->v[1]].c;
  c = mesh->point[pt->v[2]].c;

  abx = b[0] - a[0];
  aby = b[1] - a[1];
  acx = c[0] - a[0];
  acy = c[1] - a[1];
  bcx = c[0] - b[0];
  bcy = c[1] - b[1];

  area = abx * acy - aby * acx;

  if (area <= 0.) return 0.;

  h1 = abx * abx + aby * aby;
  h2 = acx * acx + acy * acy;
  h3 = bcx * bcx + bcy * bcy;

  ht = h1 + h2 + h3;
  if (ht > 0.)
    return area / ht;
  else
    return 0.;
}


int DMG_computeQual(DMG_pMesh mesh) {
  int i;
  DMG_pTria pt;

  for (i = 1; i <= mesh->nt ; i++) {
    pt = &mesh->tria[i];
    if (!DMG_TOK(pt)) continue;
    pt->qual = DMG_ALPHA_2D * DMG_computeTriaQual(mesh, pt);
  }

  return DMG_SUCCESS;
}


int DMG_displayQualHisto(DMG_pMesh mesh, int nclass) {
  int i, k, itmin, nt;
  DMG_pTria pt;
  double qualmin, qualmax, qualavg, qual;
  int hist[nclass];

  qualmin = 2.;
  qualmax = 0.;
  qualavg = 0.;
  itmin = 0;
  nt = 0;

  memset(hist, 0, nclass * sizeof(int));

  for (i = 1 ; i <= mesh->nt ; i++) {
    pt = &mesh->tria[i];
    if (!DMG_TOK(pt)) continue;
    nt++;
    qual = pt->qual;
    qualavg += qual;

    if (qual < qualmin){
      qualmin = qual;
      itmin = i;
    }

    qualmax = MAX2(qual, qualmax);

    k = MIN2(nclass - 1, (int)(nclass * qual));
    hist[k]++;
  }

  qualavg /= mesh->nt;

  /** Display min, max and avg quality */
  fprintf(stdout, "\n Mesh quality   %d\n", nt);
  fprintf(stdout, "    MAX : %f, AVG : %f, MIN : %f (%d)\n", qualmax, qualavg, qualmin, itmin);

  /** Display mesh quality histogram */
  fprintf(stdout, "    HISTOGRAM \n");
  for (k = nclass - 1 ; k >= 0 ; k--) {
    fprintf(stdout, "    %4.2f < Q < %4.2f : %4.2f %% (%d)\n", k/(float)nclass, (k+1)/(float)nclass, 100.*hist[k]/(float)nt, hist[k]);
  }
  fprintf(stdout, "\n");

  return DMG_SUCCESS;
}
