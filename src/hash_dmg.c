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

  for (i = 0 ; i < mesh->nt ; i++) {
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

  /** Create the hash table for the edges */
  htab = (DMG_Hedge*) calloc(3 * mesh->nt, sizeof(DMG_Hedge));

  for (i = 0 ; i < 3 * mesh->nt ; i++) {
    htab[i] = (DMG_Hedge) {DMG_UNSET, DMG_UNSET, DMG_UNSET, DMG_UNSET, DMG_UNSET};
  }

  DMG_hashHedge(mesh, htab);

  /** Build the triangles adjacency table using the edge hash table*/
  mesh->adja = (int*) calloc(3 * mesh->nt, sizeof(int));
  hsize = mesh->np;

  for (i = 0 ; i < mesh->nt ; i++) {
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
      } while (key != DMG_UNSET);
    }
  }

  return DMG_SUCCESS;
}
