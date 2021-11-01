#include "dmg.h"

#define KA 7
#define KB 11

int DMG_hashHedge(DMG_pMesh mesh) {
  int hnxt, i, j, key, iadj, a, b, vmin, vmax, hsize, flag;
  DMG_pTria pt;
  DMG_Hedge *htab, *hedge;

  if (!mesh || !mesh->np || !mesh->nt) {
    fprintf(stderr, "Error : %s:%d : Cannot hash the edges of an empty mesh ! \n", __func__, __LINE__);
    return DMG_FAILURE;
  }

  mesh->htab = (DMG_Hedge*) calloc(3 * mesh->nt, sizeof(DMG_Hedge));
  for (i = 0 ; i < 3 * mesh->nt ; i++) {
    mesh->htab[i] = (DMG_Hedge) {DMG_UNSET, DMG_UNSET, DMG_UNSET, DMG_UNSET, DMG_UNSET};
  }

  htab = mesh->htab;
  hnxt = hsize = mesh->np;

  for (i = 0 ; i < mesh->nt ; i++) {
    pt = &mesh->tria[i];
    /* Run through the edges of the triangle */
    for (j = 0 ; j < 3 ; j++) {
      a = pt->v[(j + 1)%3];
      b = pt->v[(j + 2)%3];
      vmin = MIN2(a, b);
      vmax = MAX2(a, b);

      /* Compute the hash key and the adjacency relation */
      key = (KA * vmin + KB * vmax) % hsize;
      iadj = 3 * i + j;

      /* If the key does not exist, create the first hedge corresponding to this key */
      if (htab[key].a == DMG_UNSET) {
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
        } while (key != DMG_UNSET);
        /* If not found, create the hedge object and set the nxt field of the last seen hedge object to point this hedge */
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
  DMG_Hedge *hedge;

  if (!mesh || !mesh->np || !mesh->nt) {
    fprintf(stderr, "Error : %s:%d : Cannot build the adjacency table of an empty mesh ! \n", __func__, __LINE__);
    return DMG_FAILURE;
  }
  if (!mesh->htab) {
    fprintf(stderr, "Error : %s:%d : Cannot build the adjacency table without the hedge hash table ! \n", __func__, __LINE__);
    return DMG_FAILURE;
  }

  mesh->adja = (int*) calloc(3 * mesh->nt, sizeof(int));
  hsize = mesh->np;

  for (i = 0 ; i < mesh->nt ; i++) {
    pt = &mesh->tria[i];
    for (j = 0 ; j < 3 ; j++) {
      a = pt->v[(j + 1)%3];
      b = pt->v[(j + 2)%3];
      vmin = MIN2(a, b);
      vmax = MAX2(a, b);

      key = (KA * vmin + KB * vmax) % hsize;

      iadj = 3 * i + j;

      while (key != DMG_UNSET) {
        hedge = &mesh->htab[key];
        if (hedge->a == vmin && hedge->b == vmax) {
          if (hedge->adj1 == iadj) mesh->adja[iadj] = hedge->adj2;
          else mesh->adja[iadj] = hedge->adj1;
          break;
        }
      }
    }
  }

  return DMG_SUCCESS;
}

int buildVerticesBalls2D(DMG_pMesh mesh) {
  int i, j, k, v, ntot, *nneigh;
  DMG_pTria tria;

  if (!mesh || !mesh->np || !mesh->nt) {
    fprintf(stderr, "Error : %s:%d : Cannot build the neighbours table of an empty mesh ! \n", __func__, __LINE__);
    return DMG_FAILURE;
  }

  /** Build the head table */
  nneigh = (int*) calloc(mesh->np, sizeof(int));
  mesh->head = (int*) calloc(mesh->np, sizeof(int));
  ntot = 0;

  /* Count the number of neighbour triangles for each vertex */
  for (i = 0 ; i < mesh->nt ; i++) {
    tria = &mesh->tria[i];
    nneigh[tria->v[0]]++;
    nneigh[tria->v[1]]++;
    nneigh[tria->v[2]]++;
    ntot += 3;
  }

  /* Sum the number of neighbours to build the head table */
  for (i = 1 ; i < mesh->np ; i++) {
    mesh->head[i] = nneigh[i - 1] + mesh->head[i - 1];
  }
  for (i = 0 ; i < mesh->np ; i++) {
    if (nneigh[i] == 0) mesh->head[i] = DMG_UNSET;
  }

  /** Build the second table containing the indices of the triangles */
  memset(nneigh, 0, mesh->np * sizeof(int));
  mesh->neigh = (int*) malloc(ntot * sizeof(int));
  for (i = 0 ; i < mesh->nt ; i++) {
    tria = &mesh->tria[i];
    for (j = 0 ; j < 3 ; j++) {
      v = tria->v[j];
      if (mesh->head[v] == DMG_UNSET) continue;
      k = mesh->head[v] + nneigh[v];
      mesh->neigh[k] = i;
      nneigh[v]++;
    }
  }

  free(nneigh); nneigh = NULL;

  return DMG_SUCCESS;
}
