#include "dmg.h"


/* Check if the k-th edge of triangle it can be swapped by checking if the created triangles are not overlapping (non-convex quadrilateral). */
int DMG_chkSwap(DMG_pMesh mesh, int it, int k) {
  DMG_pTria pt;
  int *adja, jt, l;
  double *a, *p, *q, *d, adp, adq;

  assert (k >= 0  && k < 3);

  pt = &mesh->tria[it];

  a = mesh->point[pt->v[DMG_tria_vert[k]]].c;
  p = mesh->point[pt->v[DMG_tria_vert[k+1]]].c;
  q = mesh->point[pt->v[DMG_tria_vert[k+2]]].c;

  adja = &mesh->adja[3 * it];
  jt = adja[k] / 3;
  l = adja[k] % 3;

  pt = &mesh->tria[jt];
  d = mesh->point[pt->v[l]].c;

  adp = DMG_orient(a, d, p);
  adq = DMG_orient(a, d, q);

  /* b has to be on the right of edge (a,d) and c on the left of edge (a,d) */
  if (adp < 0 && adq > 0)
    return 1;
  else
    return 0;

}


int DMG_swap(DMG_pMesh mesh, int it, int k) {
  DMG_pTria pt;
  int *adja, jt, l, itadj, jtadj, adjait[3], adjajt[3], adjr, a, p, q, d;

  assert (k >= 0  && k < 3);

  /* Get the 4 vertices */
  pt = &mesh->tria[it];

  a = pt->v[DMG_tria_vert[k]];
  p = pt->v[DMG_tria_vert[k+1]];
  q = pt->v[DMG_tria_vert[k+2]];

  itadj = 3 * it;
  adja = &mesh->adja[itadj];

  jt = adja[k] / 3;
  l = adja[k] % 3;
  jtadj = 3 * jt;

  pt = &mesh->tria[jt];

  d = pt->v[l];

  /* Swap the edge by updating the vertices of each triangle */
  pt = &mesh->tria[it];
  pt->v[0] = q;
  pt->v[1] = a;
  pt->v[2] = d;

  pt = &mesh->tria[jt];
  pt->v[0] = p;
  pt->v[1] = d;
  pt->v[2] = a;

  /* Update the adjacency relations */
  memcpy(adjait, &mesh->adja[itadj], 3 * sizeof(int));
  memcpy(adjajt, &mesh->adja[jtadj], 3 * sizeof(int));

  /* Update it adjacency */
  adja = &mesh->adja[itadj];
  adja[0] = jtadj;
  adja[1] = adjajt[DMG_tria_vert[l+2]];
  adja[2] = adjait[DMG_tria_vert[k+1]];
  adjr = adja[1];
  mesh->adja[adjr] = itadj + 1;
  adjr = adja[2];
  mesh->adja[adjr] = itadj + 2;

  /* Update jt adjacency */
  adja = &mesh->adja[jtadj];
  adja[0] = itadj;
  adja[1] = adjait[DMG_tria_vert[k+2]];
  adja[2] = adjajt[DMG_tria_vert[l+1]];
  adjr = adja[1];
  mesh->adja[adjr] = jtadj + 1;
  adjr = adja[2];
  mesh->adja[adjr] = jtadj + 2;

  return jt;
}
