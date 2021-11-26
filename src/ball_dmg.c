#include "dmg.h"


int DMG_findBall(DMG_pMesh mesh, int start, int iploc, int *list) {
  DMG_Queue *q;
  DMG_pTria pt;
  int it, k1, k2, l, count, iadj, adjr, *adja;

  assert(list != NULL);
  assert(iploc >= 0 && iploc < 3);

  it = start;
  count = 0;

  /* BFS */
  q = DMG_createQueue();
  pt = &mesh->tria[it];
  adjr = 3 * it + iploc;
  pt->flag = 1;
  DMG_enQueue(q, adjr);

  while (!DMG_qIsEmpty(q)) {
    adjr = DMG_deQueue(q);
    list[count++] = adjr;

    iadj = 3 * (adjr / 3);
    iploc = adjr % 3;
    adja = &mesh->adja[iadj];

    /* Local indices of the 2 other vertices of the triangle */
    k1 = DMG_tria_vert[iploc + 1];
    k2 = DMG_tria_vert[iploc + 2];

    /* Adjacent triangle by local edge k1 */
    if (adja[k1]) {
      it = adja[k1] / 3;
      l = adja[k1] % 3;
      pt = &mesh->tria[it];
      iploc = DMG_tria_vert[l + 1];
      adjr = 3 * it + iploc;
      if (!pt->flag) {
        pt->flag = 1;
        DMG_enQueue(q, adjr);
      }
    }

    /* Adjacent triangle by local edge k2 */
    if (adja[k2]) {
      it = adja[k2] / 3;
      l = adja[k2] % 3;
      pt = &mesh->tria[it];
      iploc = DMG_tria_vert[l + 2];
      adjr = 3 * it + iploc;
      if (!pt->flag) {
        pt->flag = 1;
        DMG_enQueue(q, adjr);
      }
    }

  }

  DMG_freeQueue(q);

  for (it = 0 ; it < count ; it++) {
    mesh->tria[list[it] / 3].flag = 0;
  }

  return count;
}
