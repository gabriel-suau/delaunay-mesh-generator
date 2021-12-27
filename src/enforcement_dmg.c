#include "dmg.h"


int DMG_enforceBndry(DMG_pMesh mesh) {
  DMG_pPoint ppt;
  DMG_pEdge pa;
  DMG_pTria pt;
  DMG_Queue *queue;
  int i, k, iploc, i1, i2, it, nanoex, nswap, tcount, list[DMG_LIST_SIZE], iadj, tmp;
  double *a, *b, *p, *q;

  nanoex = nswap = 0;

  /* Set the tmp field of each point to the index of one triangle they belong to */
  for (i = 1 ; i <= mesh->nt ; i++) {
    pt = &mesh->tria[i];
    if (!DMG_TOK(pt)) continue;
    for (k = 0 ; k < 3 ; k++) {
      mesh->point[pt->v[k]].tmp = i;
    }
  }

  /* Loop over all the edge constraints */
  for (i = 1 ; i <= mesh->na ; i++) {
    pa = &mesh->edge[i];

    /* Coordinates of the vertices of the edge */
    p = mesh->point[pa->v[0]].c;
    q = mesh->point[pa->v[1]].c;

    ppt = &mesh->point[pa->v[0]];
    it = ppt->tmp;
    pt = &mesh->tria[it];

    if (pt->v[0] == pa->v[0])
      iploc = 0;
    else if (pt->v[1] == pa->v[0])
      iploc = 1;
    else
      iploc = 2;

    tcount = DMG_findBall(mesh, it, iploc, list);

    /* Run through the triangles of the ball to see if the edge matches an existing edge */
    for (k = 0 ; k < tcount ; k++) {
      it = list[k] / 3;
      iploc = list[k] % 3;
      pt = &mesh->tria[it];
      i1 = DMG_tria_vert[iploc + 1];
      i2 = DMG_tria_vert[iploc + 2];

      if (pt->v[i1] == pa->v[1] || pt->v[i2] == pa->v[1]) {
        break;
      }
    }

    /* If the edge was found, go to the next edge */
    if (k < tcount) continue;

    printf("edge %d with points %d, %d needs to be enforced\n", i, pa->v[0], pa->v[1]);

    nanoex++;

    /* If the edge was not found, rerun through the triangles of the ball and find 
       the triangle whose edge iploc is crossed by the constrained edge. */
    for (k = 0 ; k < tcount ; k++) {
      it = list[k] / 3;
      iploc = list[k] % 3;
      pt = &mesh->tria[it];
      i1 = DMG_tria_vert[iploc + 1];
      i2 = DMG_tria_vert[iploc + 2];

      a = mesh->point[pt->v[i1]].c;
      b = mesh->point[pt->v[i2]].c;

      if (DMG_segSegIntersect(a, b, p, q)) break;
    }

    assert ( k < tcount );

    /* Put all triangles that intersect the constrained edge in a queue */
    queue = DMG_createQueue();
    DMG_enQueue(queue, it);
    iadj = 3 * it + iploc;
    do {
      iadj = mesh->adja[iadj];
      it = iadj / 3;
      iploc = iadj % 3;
      DMG_enQueue(queue, it);

      pt = &mesh->tria[it];

      /* If iploc matches the second contrained edge vertex, we got them all. */
      if (pt->v[iploc] == pa->v[1]) break;

      /* Check the 2 other edges to find the next triangle */
      a = mesh->point[pt->v[iploc]].c;
      b = mesh->point[pt->v[DMG_tria_vert[iploc+1]]].c;
      if (DMG_segSegIntersect(a, b, p, q)) {
        iadj = 3 * it + DMG_tria_vert[iploc + 2];
        continue;
      }
      b = mesh->point[pt->v[DMG_tria_vert[iploc+2]]].c;
      if (DMG_segSegIntersect(a, b, p, q)) {
        iadj = 3 * it + DMG_tria_vert[iploc + 1];
        continue;
      }

    } while (1);

    /* While the queue is not empty, test every edge of each triangle. Try to swap the edges that
     * intersect the constrained edge. If the swap is illegal, requeue the triangle. */
    while (!DMG_qIsEmpty(queue)) {
      it = DMG_deQueue(queue);
      pt = &mesh->tria[it];
      tmp = 0;
      /* This might be faster using the segment-triangle intersection routine */
      for (k = 0 ; k < 3 ; k++) {
        a = mesh->point[pt->v[DMG_tria_vert[k+1]]].c;
        b = mesh->point[pt->v[DMG_tria_vert[k+2]]].c;
        if (DMG_segSegIntersect(a, b, p, q)) {
          if (DMG_chkSwap(mesh, it, DMG_tria_vert[k])) {
            DMG_swap(mesh, it, k);
            nswap++; 
          }
          else if (!tmp) {
            DMG_enQueue(queue, it);
            tmp = 1;
          }
        }
      }
    }

    DMG_freeQueue(queue);

  }

  if (nanoex)
    printf("%d edge(s) enforced with %d swap(s)\n", nanoex, nswap);

  return DMG_SUCCESS;
}
