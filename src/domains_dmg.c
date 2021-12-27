#include "dmg.h"


int DMG_markSubDomains(DMG_pMesh mesh) {
  DMG_pPoint ppt;
  DMG_pEdge pa;
  DMG_pTria pt;
  DMG_Queue *q;
  int it, ia, k, color, iadj, *adja, a, b, flag;

  /* Assign ref = -1 to all triangles and assign the tmp field of
     each point to the index of one triangle it belongs to. */
  for (it = 1 ; it <= mesh->nt ; it++) {
    pt = &mesh->tria[it];
    if (!DMG_TOK(pt)) continue;
    pt->ref = -1;
    for (k = 0 ; k < 3 ; k++) {
      mesh->point[pt->v[k]].tmp = it;
    }
  }

  color = 0;
  flag = 1;

  ppt = &mesh->point[mesh->npi];
  it = ppt->tmp;
  pt = &mesh->tria[it];
  pt->ref = color;

  q = DMG_createQueue();

  while (flag) {

    /* BFS */
    DMG_enQueue(q, it);

    while (!DMG_qIsEmpty(q)) {
      it = DMG_deQueue(q);
      pt = &mesh->tria[it];

      iadj = 3 * it;
      adja = &mesh->adja[iadj];

      for (k = 0 ; k < 3 ; k++) {
        it = adja[k] / 3;
        if (!it || mesh->tria[it].ref != -1) continue;
        a = pt->v[DMG_tria_vert[k+1]];
        b = pt->v[DMG_tria_vert[k+2]];
        /* TODO : Try to remove this loop using a hash table or something */
        for (ia = 1 ; ia <= mesh->na ; ia++) {
          pa = &mesh->edge[ia];
          if ((a == pa->v[0] && b == pa->v[1]) ||
              (a == pa->v[1] && b == pa->v[0])) {
            break;
          }
        }
        if (ia > mesh->na) {
          mesh->tria[it].ref = color;
          DMG_enQueue(q, it);
        }
      }
    }

    flag = 0;

    for (it = 1 ; it <= mesh->nt ; it++) {
      pt = &mesh->tria[it];
      if (!DMG_TOK(pt)) continue;
      if (pt->ref == -1) {
        flag = 1;
        break;
      }
    }
    color++;
  }

  DMG_freeQueue(q);

  /* Search for one triangle adjacent to a BB triangle by a constrained edge : this 
   * triangle it belongs to the domain. Loop over the triangles. For each triangle 
   * jt, set its ref to 0 if it has the same color as it, else set its ref to 1. */
  it = ppt->tmp;
  color = mesh->tria[it].ref;
  for (it = 1 ; it <= mesh->nt ; it++) {
    pt = &mesh->tria[it];
    if (!DMG_TOK(pt) || pt->ref != color) continue;
    adja = &mesh->adja[3 * it];
    for (k = 0 ; k < 3 ; k++) {
      it = adja[k] / 3;
      if (!it) continue;
      a = pt->v[DMG_tria_vert[k+1]];
      b = pt->v[DMG_tria_vert[k+2]];
      for (ia = 1 ; ia <= mesh->na ; ia++) {
        pa = &mesh->edge[ia];
        if ((a == pa->v[0] && b == pa->v[1]) ||
            (a == pa->v[1] && b == pa->v[0])) {
          goto found;
        }
      }
    }
  }

 found:
  color = mesh->tria[it].ref;
  for (k = 1 ; k <= mesh->nt ; k++) {
    pt = &mesh->tria[k];
    if (!DMG_TOK(pt)) continue;
    if (pt->ref != color)
      pt->ref = 1;
    else
      pt->ref = 0;
  }

  return DMG_SUCCESS;
}


int DMG_removeExterior(DMG_pMesh mesh) {
  DMG_pTria pt;
  int k;

  for (k = 1 ; k <= mesh->nt ; k++) {
    pt = &mesh->tria[k];
    if (!DMG_TOK(pt)) continue;
    if (pt->ref == 1) DMG_delTria(mesh, k);
  }

  DMG_delPoint(mesh, mesh->npi);
  DMG_delPoint(mesh, mesh->npi+1);
  DMG_delPoint(mesh, mesh->npi+2);
  DMG_delPoint(mesh, mesh->npi+3);

  return DMG_SUCCESS;
}
