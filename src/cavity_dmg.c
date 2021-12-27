#include "dmg.h"

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
          ptlist[ptcount++] = 0;
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
        ptlist[ptcount++] = 3 * it + k;
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
