#include "dmg.h"


int DMG_insertPoint(DMG_pMesh mesh, int ip, int start) {
  DMG_pPoint ppt;
  int list[DMG_LIST_SIZE], ptcount;

  ppt = &mesh->point[ip];
  ppt->flag = 1;
  start = DMG_locTria(mesh, start, ppt->c);
  ptcount = DMG_createCavity(mesh, ppt->c, start, list);
  start = DMG_createBall(mesh, ip, ptcount, list);

  return start;
}


int DMG_createCavity(DMG_pMesh mesh, double d[2], int start, int *ptlist) {
  DMG_Queue *q;
  DMG_pTria pt, ptmp;
  int i, k, it, jt, incount, ptcount, adjcount, iadj, *adja, cavity[DMG_LIST_SIZE], adjlist[DMG_LIST_SIZE];
  double *a, *b, *c;

  memset(cavity, 0, DMG_LIST_SIZE * sizeof(int));
  memset(adjlist, 0, DMG_LIST_SIZE * sizeof(int));

  it = start;
  incount = ptcount = adjcount = 0;

  /* BFS */
  q = DMG_createQueue();
  pt = &mesh->tria[it];
  pt->flag = 1;
  DMG_enQueue(q, it);

  while (!DMG_qIsEmpty(q)) {
    it = DMG_deQueue(q);
    pt = &mesh->tria[it];

    iadj = 3 * it;
    adja = &mesh->adja[iadj];

    a = mesh->point[pt->v[0]].c;
    b = mesh->point[pt->v[1]].c;
    c = mesh->point[pt->v[2]].c;

    /* Si le triangle ne verifie pas le critere de delaunay, ajouter son indice dans la liste de la cavite et mettre ses voisins qui n'ont pas encore été visités dans la queue. */
    if (DMG_inCircle(a, b, c, d) > 0) {
      pt->flag = 2;
      cavity[incount++] = it;
      for (k = 0 ; k < 3 ; k++) {
        jt = adja[k] / 3;
        /* Domain boundary : recuperer l'arete de la frontiere*/
        if (!jt) {
          ptlist[ptcount++] = pt->v[DMG_tria_vert[k+1]];
          ptlist[ptcount++] = pt->v[DMG_tria_vert[k+2]];
        } else {
          ptmp = &mesh->tria[jt];
          if (!ptmp->flag) {
            ptmp->flag = 1;
            DMG_enQueue(q, jt);
          }
        }
      }
    }
    /* Sinon, c'est un triangle adjacent a la cavité, sauvegarder son indice dans une liste */
    else {
      adjlist[adjcount++] = it;
    }
  }

  DMG_freeQueue(q);

  /* Pour chaque triangle adjacent a la cavité, récupérer les arêtes par 
     lesquelles il voit la cavité (orientées positivement par rapport à la cavité) */
  for (i = 0 ; i < adjcount ; i++) {
    it = adjlist[i];
    pt = &mesh->tria[it];

    iadj = 3 * it;
    adja = &mesh->adja[iadj];

    for (k = 0 ; k < 3 ; k++) {
      jt = adja[k] / 3;
      if (!jt) continue;
      ptmp = &mesh->tria[jt];
      if (ptmp->flag == 2) {
        ptlist[ptcount++] = pt->v[DMG_tria_vert[k+2]];
        ptlist[ptcount++] = pt->v[DMG_tria_vert[k+1]];
      } 
    }    
  }

  /* Create the cavity by deleting the triangles */
  for (k = 0 ; k < incount ; k++) {
    DMG_delTria(mesh, cavity[k]);
  }

  for (k = 0 ; k < adjcount ; k++) {
    mesh->tria[adjlist[k]].flag = 0;
  }

  return ptcount;
}


int DMG_createBall(DMG_pMesh mesh, int ip, int ptcount, const int *ptlist) {
  DMG_pTria pt;
  int k, ia, ib, it;

  it = 0;

  assert(ptcount && ptlist != NULL);

  /* Create the triangles */
  for (k = 0 ; k < ptcount ; k+=2) {
    ia = ptlist[k];
    ib = ptlist[k+1];
    it = DMG_newTria(mesh);
    pt = &mesh->tria[it];
    pt->v[0] = ip;
    pt->v[1] = ia;
    pt->v[2] = ib;
  }

  /* Create the adjacency relations */
  DMG_setAdja(mesh);

  return it;
}


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


int DMG_swap(DMG_pMesh mesh, int it, int k, int ita[3], int jta[3]) {
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

  ita[DMG_tria_vert[k]] = 0;
  ita[DMG_tria_vert[k+1]] = itadj + 2;
  ita[DMG_tria_vert[k+2]] = jtadj + 1;

  jta[DMG_tria_vert[l]] = 0;
  jta[DMG_tria_vert[l+1]] = jtadj + 2;
  jta[DMG_tria_vert[l+2]] = itadj + 1;

  return jt;

}
