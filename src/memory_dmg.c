#include "dmg.h"


int DMG_Init_mesh(DMG_pMesh *mesh) {
  *mesh = (DMG_pMesh)malloc(sizeof(DMG_Mesh));

  if (*mesh == NULL) {
    fprintf(stderr, "Error: %s: mesh struct allocation failed\n", __func__);
    return DMG_FAILURE;
  }

  (*mesh)->ver = (*mesh)->dim = DMG_UNSET;
  (*mesh)->np = (*mesh)->na = (*mesh)->nt = 0;
  (*mesh)->npu = (*mesh)->nau = (*mesh)->ntu = 0;
  (*mesh)->npmax = (*mesh)->namax = (*mesh)->ntmax = 0;
  (*mesh)->min[0] = (*mesh)->min[1] = (*mesh)->hmin = DBL_MAX;
  (*mesh)->max[0] = (*mesh)->max[1] = (*mesh)->hmax = -DBL_MAX;

  return DMG_SUCCESS;
}


int DMG_Free_mesh(DMG_pMesh mesh) {
  if (mesh == NULL) {
    return DMG_SUCCESS;
  }

  if (mesh != NULL) {
    if (mesh->point) {
      free(mesh->point);
      mesh->point = NULL;
      mesh->np = 0;
    }
    if (mesh->edge) {
      free(mesh->edge);
      mesh->edge = NULL;
      mesh->na = 0;
    }
    if (mesh->tria) {
      free(mesh->tria);
      mesh->edge = NULL;
      mesh->nt = 0;
    }
    if (mesh->adja) {
      free(mesh->adja);
      mesh->adja = NULL;
    }
    free(mesh);
    mesh = NULL;    
  }

  return DMG_SUCCESS;
}


int DMG_allocMesh(DMG_pMesh mesh) {
  int i;

  mesh->npmax = MAX2(1.5 * mesh->np, DMG_NPMAX);
  mesh->ntmax = MAX2(1.5 * mesh->nt, DMG_NTMAX);
  mesh->namax = mesh->na;

  mesh->point = (DMG_pPoint) calloc(mesh->npmax + 1, sizeof(DMG_Point));
  mesh->edge = (DMG_pEdge) calloc(mesh->namax + 1, sizeof(DMG_Edge));
  mesh->tria = (DMG_pTria) calloc(mesh->ntmax + 1, sizeof(DMG_Tria));

  if (mesh->point == NULL || mesh->edge == NULL || mesh->tria == NULL)
    return DMG_FAILURE;

  mesh->npu = mesh->np + 1;
  mesh->nau = 0;
  mesh->ntu = mesh->nt + 1;

  /* Set the tmp field of the unused point to be equal to the next unused point */
  /* This is used to keep track of the empty available points */
  for (i = mesh->npu ; i < mesh->npmax ; i++) {
    mesh->point[i].tmp = i + 1;
  }
  for (i = mesh->ntu ; i < mesh->ntmax ; i++) {
    mesh->tria[i].v[2] = i + 1;
  }

  /* Allocate the adjacency table */
  mesh->adja = (int*) calloc(3 * (mesh->ntmax + 1), sizeof(int));

  return DMG_SUCCESS;
}


int DMG_newPoint(DMG_pMesh mesh, double c[2]) {
  DMG_pPoint ppt;
  int ip;

  /* No memory available for a new point */
  if (!mesh->npu) return 0;

  /* New point takes the first available slot in the point array */
  ip = mesh->npu;

  if (mesh->npu > mesh->np) mesh->np = mesh->npu;
  ppt = &mesh->point[ip];
  memcpy(ppt->c, c, 2 * sizeof(double));
  mesh->npu = ppt->tmp;
  ppt->tmp = 0;
  ppt->tag = DMG_VALIDPOINT;

  return ip;
}


void DMG_delPoint(DMG_pMesh mesh, int ip) {
  DMG_pPoint ppt;

  ppt = &mesh->point[ip];

  memset(ppt, 0, sizeof(DMG_Point));
  ppt->tag = DMG_NULPOINT;
  ppt->tmp = mesh->npu;

  mesh->npu = ip;
  if (ip == mesh->np) mesh->np--;
}


int DMG_newTria(DMG_pMesh mesh) {
  DMG_pTria pt;
  int it;

  if (!mesh->ntu) return 0;

  /* New tria takes the first available slot in the tria array */
  it = mesh->ntu;

  if (mesh->ntu > mesh->nt) mesh->nt = mesh->ntu;

  pt = &mesh->tria[it];
  mesh->ntu = pt->v[2];
  pt->v[2] = 0;
  pt->ref = 0;
  pt->flag = 0;

  return it;
}


void DMG_delTria(DMG_pMesh mesh, int it) {
  DMG_pTria pt;
  int iadj, k, jadr;

  pt = &mesh->tria[it];
  memset(pt, 0, sizeof(DMG_Tria));
  pt->v[2] = mesh->ntu;
  pt->qual = 0.0;

  /* Zero the adjacency */
  iadj = 3 * it;
  if (mesh->adja != NULL) {
    for (k = 0 ; k < 3 ; k++) {
      jadr = mesh->adja[iadj + k];
      mesh->adja[jadr] = 0;
    }
    memset(&mesh->adja[iadj], 0, 3 * sizeof(int));
  }

  mesh->ntu = it;
  if (it == mesh->nt) mesh->nt--;
}

int DMG_packMesh(DMG_pMesh mesh) {
  DMG_pTria pt, pt1;
  int it, k, iadj, *adja, *adja1;

  if (!mesh->nt) {
    return DMG_SUCCESS;
  }

  it = 1;
  do {
    pt = &mesh->tria[it];
    if (!DMG_TOK(pt)) {
      pt1 = &mesh->tria[mesh->nt];
      memcpy(pt, pt1, sizeof(DMG_Tria));
      iadj = 3 * it;
      adja = &mesh->adja[iadj];
      iadj = 3 * mesh->nt;
      adja1 = &mesh->adja[iadj];
      for (k = 0 ; k < 3 ; k++) {
        adja[k] = adja1[k];
        if (!adja[k]) continue;
        iadj = adja[k];
        mesh->adja[iadj] = 3 * it + k;;
      }
      DMG_delTria(mesh, mesh->nt);
    }
  } while (++it < mesh->nt);

  if (mesh->nt >= mesh->ntmax - 1)
    mesh->ntu = 0;
  else
    mesh->ntu = mesh->nt + 1;

  if (mesh->ntu)
    for (k = mesh->ntu ; k < mesh->ntmax - 1 ; k++)
      mesh->tria[k].v[2] = k + 1;
      
  return DMG_SUCCESS;
}
