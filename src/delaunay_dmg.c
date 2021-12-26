#include "dmg.h"


int DMG_delaunay(DMG_pMesh mesh) {

  DMG_initDelaunay(mesh);

  DMG_insertBdryPoints(mesh);

  DMG_enforceBndry(mesh);

  DMG_markSubDomains(mesh);

  DMG_refineDelaunay(mesh);

  /* DMG_packMesh(mesh); */

  return DMG_SUCCESS;
}


int DMG_initDelaunay(DMG_pMesh mesh) {
  DMG_pPoint ppt;
  DMG_pTria pt;
  int i, ip1, ip2, ip3, ip4, it1, it2, iadj, *adja;
  double c[2], delta[2];

  /* Compute the bounding box and set the min and max fields */
  for (i = 1 ; i <= mesh->np ; i++) {
    ppt = &mesh->point[i];
    if (ppt->c[0] < mesh->min[0]) mesh->min[0] = ppt->c[0];
    if (ppt->c[0] > mesh->max[0]) mesh->max[0] = ppt->c[0];
    if (ppt->c[1] < mesh->min[1]) mesh->min[1] = ppt->c[1];
    if (ppt->c[1] > mesh->max[1]) mesh->max[1] = ppt->c[1];
  }

  /* Create the 4 points of the bounding box */
  delta[0] = (mesh->max[0] - mesh->min[0]);
  delta[1] = (mesh->max[1] - mesh->min[1]);

  /* Bottom left corner */
  c[0] = mesh->min[0] - 0.1 * delta[0];
  c[1] = mesh->min[1] - 0.1 * delta[1];
  ip1 = DMG_newPoint(mesh, c);

  /* Bottom right corner */
  c[0] = mesh->max[0] + 0.1 * delta[0];
  c[1] = mesh->min[1] - 0.1 * delta[1];
  ip2 = DMG_newPoint(mesh, c);

  /* Top left corner */
  c[0] = mesh->min[0] - 0.1 * delta[0];
  c[1] = mesh->max[1] + 0.1 * delta[1];
  ip3 = DMG_newPoint(mesh, c);

  /* Top right corner */
  c[0] = mesh->max[0] + 0.1 * delta[0];
  c[1] = mesh->max[1] + 0.1 * delta[1];
  ip4 = DMG_newPoint(mesh, c);

  assert ( ip1 == mesh->np-3 );
  assert ( ip2 == mesh->np-2 );
  assert ( ip3 == mesh->np-1 );
  assert ( ip4 == mesh->np );

  /* Keep in memory the index of the firt BB vertex */
  mesh->npi = ip1;

  /* Create the 2 first triangles */
  it1 = DMG_newTria(mesh);
  pt = &mesh->tria[it1];
  pt->v[0] = ip1;
  pt->v[1] = ip4;
  pt->v[2] = ip3;

  it2 = DMG_newTria(mesh);
  pt = &mesh->tria[it2];
  pt->v[0] = ip1;
  pt->v[1] = ip2;
  pt->v[2] = ip4;

  assert( it1 == mesh->nt-1 );
  assert( it2 == mesh->nt );

  /* Adjacency relations */
  iadj = 3 * it1;
  adja = &mesh->adja[iadj];
  adja[2] = 3 * it2 + 1;

  iadj = 3 * it2;
  adja = &mesh->adja[iadj];
  adja[1] = 3 * it1 + 2;

  return DMG_SUCCESS;
}


int DMG_insertBdryPoints(DMG_pMesh mesh) {
  int i, start;

  start = 1;

  for (i = 1 ; i <= mesh->np - 4 ; i++) {
    start = DMG_insertPoint(mesh, i, start);
  }

  return DMG_SUCCESS;
}


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
      if (mesh->tria[it].ref == -1) {
        flag = 1;
        break;
      }
    }
    color++;
  }

  DMG_freeQueue(q);

  /* Search for one triangle adjacent to a BB triangle by a constrained edge. */
  /* Delete all triangles that do not have its color */
  it = ppt->tmp;
  color = mesh->tria[it].ref;
  for (it = 1 ; it <= mesh->nt ; it++) {
    pt = &mesh->tria[it];
    if (pt->ref != color) continue;
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
      DMG_delTria(mesh, k);
    else
      pt->ref = 0;
  }

  /* Delete the bounding box vertices */
  DMG_delPoint(mesh, mesh->npi+3);
  DMG_delPoint(mesh, mesh->npi+2);
  DMG_delPoint(mesh, mesh->npi+1);
  DMG_delPoint(mesh, mesh->npi);

  return DMG_SUCCESS;
}


int DMG_refineDelaunay(DMG_pMesh mesh) {
  DMG_pTria pt;
  DMG_pPoint ppta, pptb;
  DMG_Hedge *htab, *hedge;
  int it, jt, j, k, a, b, c, vmin, vmax, key, hsize, n, ptcount;
  double *ca, *cb, nab[2], cc[2], d, r, alpha;

  htab = (DMG_Hedge*) calloc(3 * (mesh->ntmax + 1), sizeof(DMG_Hedge));

  DMG_computeSizeMap(mesh);

  DMG_hashHedge(mesh, htab);

  hsize = mesh->np;
  ptcount = 0;

  /* Loop through all the edges of the mesh. The hedge->adj1 field is used to
     mark the edges that have already been visited from another triangle. */
  for (it = 1 ; it <= mesh->nt ; it++) {
    pt = &mesh->tria[it];

    if (!DMG_TOK(pt)) continue;

    jt = it;

    for (k = 0 ; k < 3 ; k++) {
      a = pt->v[DMG_tria_vert[k + 1]];
      b = pt->v[DMG_tria_vert[k + 2]];
      vmin = MIN2(a, b);
      vmax = MAX2(a, b);

      key = (KA * vmin + KB *vmax) % hsize;

      do {
        hedge = &htab[key];

        /* The edge has already been visited from another triangle */
        if (!hedge->adj1 || !(hedge->a == vmin && hedge->b == vmax)) {
          key = htab[key].nxt;
        }
        else {
          hedge->adj1 = 0;
          ppta = &mesh->point[a];
          pptb = &mesh->point[b];

          ca = ppta->c;
          cb = pptb->c;
          memcpy(cc, ca, 2 * sizeof(double));

          d = DMG_length(ca, cb);
          n = MAX2((int)(2.0 * d / (ppta->h + pptb->h)) - 1, 0);
          r = (pptb->h - ppta->h) / (n + 2);

          d = 1.0 / d;
          nab[0] = (cb[0] - ca[0]) * d;
          nab[1] = (cb[1] - ca[1]) * d;
          alpha = ppta->h;

          for (j = 0 ; j < n ; j++) {
            alpha += r;
            cc[0] = cc[0] + alpha * nab[0];
            cc[1] = cc[1] + alpha * nab[1];
            c = DMG_newPoint(mesh, cc);
            mesh->point[c].h = alpha;
          }

          ptcount += n;

          break;
        }

      } while (key != 0);

    } /* for k < 3 */

  } /* for it <= mesh->nt */

  free(htab); htab = NULL;

  for (k = mesh->np - ptcount + 1; k <= mesh->np ; k++) {
    jt = DMG_insertPoint(mesh, k, jt);
  }

  return DMG_SUCCESS;
}
