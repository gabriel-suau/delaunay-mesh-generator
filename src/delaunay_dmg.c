#include "dmg.h"


int DMG_delaunay(DMG_pMesh mesh) {

  DMG_initDelaunay(mesh);

  DMG_insertBdryPoints(mesh);

  DMG_enforceBndry(mesh);

  DMG_markSubDomains(mesh);

  DMG_refineDelaunay(mesh);

  DMG_removeExterior(mesh);

  DMG_packMesh(mesh);

  /** Compute and display the quality of the mesh */
  DMG_computeQual(mesh);
  DMG_displayQualHisto(mesh, 5);

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

  if (!ip1){
    DMG_POINT_REALLOC(mesh, ip1, DMG_REALLOC_MULT,
                      fprintf(stderr, " ## Error: %s: unable to"
                              " allocate a new point\n", __func__);
                      return DMG_FAILURE, c);
  }

  /* Bottom right corner */
  c[0] = mesh->max[0] + 0.1 * delta[0];
  c[1] = mesh->min[1] - 0.1 * delta[1];
  ip2 = DMG_newPoint(mesh, c);

  if (!ip2){
    DMG_POINT_REALLOC(mesh, ip2, DMG_REALLOC_MULT,
                      fprintf(stderr, " ## Error: %s: unable to"
                              " allocate a new point\n", __func__);
                      return DMG_FAILURE, c);
  }

  /* Top left corner */
  c[0] = mesh->min[0] - 0.1 * delta[0];
  c[1] = mesh->max[1] + 0.1 * delta[1];
  ip3 = DMG_newPoint(mesh, c);

  if (!ip3){
    DMG_POINT_REALLOC(mesh, ip3, DMG_REALLOC_MULT,
                      fprintf(stderr, " ## Error: %s: unable to"
                              " allocate a new point\n", __func__);
                      return DMG_FAILURE, c);
  }

  /* Top right corner */
  c[0] = mesh->max[0] + 0.1 * delta[0];
  c[1] = mesh->max[1] + 0.1 * delta[1];
  ip4 = DMG_newPoint(mesh, c);

  if (!ip4){
    DMG_POINT_REALLOC(mesh, ip4, DMG_REALLOC_MULT,
                      fprintf(stderr, " ## Error: %s: unable to"
                              " allocate a new point\n", __func__);
                      return DMG_FAILURE, c);
  }

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


int DMG_insertPoint(DMG_pMesh mesh, int ip, int start) {
  DMG_pPoint ppt;
  int list[DMG_LIST_SIZE], ptcount;

  ppt = &mesh->point[ip];
  ppt->flag = 1;

  start = DMG_locTria(mesh, start, ppt->c);

  if (!start) return 0;

  ptcount = DMG_createCavity(mesh, ppt->c, start, list);

  if (!ptcount) return 0;

  start = DMG_createBall(mesh, ip, ptcount, list);

  if (!start) return 0;

  return start;
}


int DMG_insertBdryPoints(DMG_pMesh mesh) {
  int i, start;

  start = 1;

  for (i = 1 ; i <= mesh->np - 4 ; i++) {
    start = DMG_insertPoint(mesh, i, start);
  }

  return DMG_SUCCESS;
}


int DMG_refineDelaunay(DMG_pMesh mesh) {
  DMG_pTria pt;
  DMG_pPoint ppta, pptb;
  DMG_Hedge *htab, *hedge;
  DMG_Grid *g;
  int it, jt, j, k, a, b, c, vmin, vmax, key, hsize, n, ptcount, ip, gsize;
  double *ca, *cb, nab[2], cc[2], d, r, alpha, hmin;

  htab = (DMG_Hedge*) calloc(3 * (mesh->ntmax + 1), sizeof(DMG_Hedge));

  DMG_computeSizeMap(mesh);

  g = DMG_createGrid(mesh->min, mesh->max, mesh->hmin);
  gsize = g->nx * g->ny;

  /* Cut the edges until all are saturated */
  do {
    DMG_hashHedge(mesh, htab);

    hsize = mesh->np;
    ptcount = 0;

    /* Loop through all the edges of the mesh. The hedge->adj1 field is used to
       mark the edges that have already been visited from another triangle. */
    for (it = 1 ; it <= mesh->nt ; it++) {
      pt = &mesh->tria[it];

      if (!DMG_TOK(pt) || pt->ref == 1) continue;

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
              ip = DMG_newPoint(mesh, cc);
              if (!ip) {
                DMG_POINT_REALLOC(mesh, ip, DMG_REALLOC_MULT,
                                  fprintf(stderr, " ## Error: %s: unable to"
                                          " allocate a new point\n", __func__);
                                  return DMG_FAILURE, cc);
              }
              mesh->point[ip].h = alpha;
            }

            ptcount += n;

            break;
          }

        } while (key != 0);

      } /* for k < 3 */

    } /* for it <= mesh->nt */

    /* Filter out points using the background grid */
    for (k = mesh->np - ptcount + 1; k <= mesh->np ; k++) {
      ppta = &mesh->point[k];
      it = DMG_gCell(g, ppta->c);

      if (g->ucell[it]) {
        ptcount--;
        continue;
      }

      else {

        if (it + 1 < gsize) {
          ip = g->ucell[it + 1];
          if (ip) {
            pptb = &mesh->point[ip];
            hmin = MAX2(ppta->h, pptb->h);
            if (DMG_lengthsq(ppta->c, pptb->c) < hmin * hmin) {
              ppta->tag = DMG_NULPT;
              ptcount--;
              continue;
            }
          }
        }

        if (it - 1 >= 0) {
          ip = g->ucell[it - 1];
          if (ip) {
            pptb = &mesh->point[ip];
            hmin = MAX2(ppta->h, pptb->h);
            if (DMG_lengthsq(ppta->c, pptb->c) < hmin * hmin) {
              ppta->tag = DMG_NULPT;
              ptcount--;
              continue;
            }
          }
        }

        if (it + g->nx < gsize) {
          ip = g->ucell[it + g->nx];
          if (ip) {
            pptb = &mesh->point[ip];
            hmin = MAX2(ppta->h, pptb->h);
            if (DMG_lengthsq(ppta->c, pptb->c) < hmin * hmin) {
              ppta->tag = DMG_NULPT;
              ptcount--;
              continue;
            }
          }
        }

        if (it + g->nx + 1 < gsize) {
          ip = g->ucell[it + g->nx + 1];
          if (ip) {
            pptb = &mesh->point[ip];
            hmin = MAX2(ppta->h, pptb->h);
            if (DMG_lengthsq(ppta->c, pptb->c) < hmin * hmin) {
              ppta->tag = DMG_NULPT;
              ptcount--;
              continue;
            }
          }
        }

        if (it + g->nx - 1 < gsize) {
          ip = g->ucell[it + g->nx - 1];
          if (ip) {
            pptb = &mesh->point[ip];
            hmin = MAX2(ppta->h, pptb->h);
            if (DMG_lengthsq(ppta->c, pptb->c) < hmin * hmin) {
              ppta->tag = DMG_NULPT;
              ptcount--;
              continue;
            }
          }
        }

        if (it - g->nx >= 0) {
          ip = g->ucell[it - g->nx];
          if (ip) {
            pptb = &mesh->point[ip];
            hmin = MAX2(ppta->h, pptb->h);
            if (DMG_lengthsq(ppta->c, pptb->c) < hmin * hmin) {
              ppta->tag = DMG_NULPT;
              ptcount--;
              continue;
            }
          }
        }

        if (it - g->nx + 1 >= 0) {
          ip = g->ucell[it - g->nx + 1];
          if (ip) {
            pptb = &mesh->point[ip];
            hmin = MAX2(ppta->h, pptb->h);
            if (DMG_lengthsq(ppta->c, pptb->c) < hmin * hmin) {
              ppta->tag = DMG_NULPT;
              ptcount--;
              continue;
            }
          }
        }

        if (it - g->nx - 1 >= 0) {
          ip = g->ucell[it - g->nx - 1];
          if (ip) {
            pptb = &mesh->point[ip];
            hmin = MAX2(ppta->h, pptb->h);
            if (DMG_lengthsq(ppta->c, pptb->c) < hmin * hmin) {
              ppta->tag = DMG_NULPT;
              ptcount--;
              continue;
            }
          }
        }

        jt = DMG_insertPoint(mesh, k, jt);
        g->ucell[it] = k;
      }
    }

    memset(htab, 0, 3 * (mesh->ntmax + 1) * sizeof(DMG_Hedge));

  } while (ptcount);

  DMG_freeGrid(g);

  free(htab); htab = NULL;

  return DMG_SUCCESS;
}

