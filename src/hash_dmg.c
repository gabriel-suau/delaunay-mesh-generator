#include "dmg.h"

#define KA 7
#define KB 11

int DMG_hashHedge(DMG_pMesh mesh, DMG_Hedge *htab) {
  int hnxt, i, j, key, iadj, a, b, vmin, vmax, hsize, flag;
  DMG_pTria pt;
  DMG_Hedge *hedge;

  if (mesh == NULL || !mesh->np || !mesh->nt) {
    fprintf(stderr, "Error : %s:%d : Cannot hash the edges of an empty mesh ! \n", __func__, __LINE__);
    return DMG_FAILURE;
  }
  if (htab == NULL) {
    fprintf(stderr, "Error : %s:%d : hash table not allocated ! \n", __func__, __LINE__);
    return DMG_FAILURE;
  }

  hnxt = hsize = mesh->np;

  for (i = 1 ; i <= mesh->nt ; i++) {
    pt = &mesh->tria[i];
    /* Run through the edges of the triangle */
    for (j = 0 ; j < 3 ; j++) {
      a = pt->v[DMG_tria_vert[j+1]];
      b = pt->v[DMG_tria_vert[j+2]];
      vmin = MIN2(a, b);
      vmax = MAX2(a, b);

      /* Compute the hash key and the adjacency relation */
      key = (KA * vmin + KB * vmax) % hsize;
      iadj = 3 * i + j;

      /* If the key does not exist, create the first hedge corresponding to this key */
      if (htab[key].a == 0) {
        htab[key].a = vmin;
        htab[key].b = vmax;
        htab[key].adj1 = iadj;
      }
      /* Else, search for the corresponding hedge object in the hedge chain cooresponding to this key */
      else {
        flag = 0;
        do {
          hedge = &htab[key];
          /* If found, set the second adjacency relation */
          if (hedge->a == vmin && hedge->b == vmax) {
            hedge->adj2 = iadj;
            flag = 1;
            break;
          }
          key = hedge->nxt;
        } while (key != 0);
        /* If not found, create the hedge object and set the nxt field of the last seen hedge object to point to this hedge */
        if (!flag) {
          htab[hnxt].a = vmin;
          htab[hnxt].b = vmax;
          htab[hnxt].adj1 = iadj;
          hedge->nxt = hnxt;
          hnxt++;
        }
      }
    }
  }

  return DMG_SUCCESS;
}


int DMG_setAdja(DMG_pMesh mesh) {
  int i, j, a, b, vmin, vmax, key, hsize, iadj;
  DMG_pTria pt;
  DMG_Hedge *hedge, *htab;

  if (!mesh || !mesh->np || !mesh->nt) {
    fprintf(stderr, "Error : %s:%d : Cannot build the adjacency table of an empty mesh ! \n", __func__, __LINE__);
    return DMG_FAILURE;
  }
  if (mesh->adja == NULL) {
    fprintf(stderr, "Error : %s:%d : Adjacency table not allocated ! \n", __func__, __LINE__);
    return DMG_FAILURE;
  }

  /** Create the hash table for the edges */
  htab = (DMG_Hedge*) calloc(3 * (mesh->ntmax + 1), sizeof(DMG_Hedge));

  DMG_hashHedge(mesh, htab);

  /** Build the triangles adjacency table using the edge hash table*/
  hsize = mesh->np;

  for (i = 1 ; i <= mesh->nt ; i++) {
    pt = &mesh->tria[i];
    for (j = 0 ; j < 3 ; j++) {
      a = pt->v[DMG_tria_vert[j+1]];
      b = pt->v[DMG_tria_vert[j+2]];
      vmin = MIN2(a, b);
      vmax = MAX2(a, b);

      key = (KA * vmin + KB * vmax) % hsize;

      iadj = 3 * i + j;

      do {
        hedge = &htab[key];
        if (hedge->a == vmin && hedge->b == vmax) {
          if (hedge->adj1 == iadj) {
            mesh->adja[iadj] = hedge->adj2;
          } else {
            mesh->adja[iadj] = hedge->adj1;
          }
          break;
        }
        else {
          key = htab[key].nxt;
        }
      } while (key != 0);
    }
  }

  free(htab);

  return DMG_SUCCESS;
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

int DMG_createBall(DMG_pMesh mesh, int ip, int ptcount, int *ptlist) {
  DMG_pTria pt;
  int k, ia, ib, it;

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
