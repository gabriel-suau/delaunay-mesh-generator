#include "dmg.h"


int DMG_Init_mesh(DMG_pMesh *mesh) {
  *mesh = (DMG_pMesh)malloc(sizeof(DMG_Mesh));

  if (mesh == NULL) {
    fprintf(stderr, "Error: %s: mesh struct allocation failed\n", __func__);
    return DMG_FAILURE;
  }

  (*mesh)->ver = (*mesh)->dim = DMG_UNSET;
  (*mesh)->np = (*mesh)->na = (*mesh)->nt = 0;
  (*mesh)->npu = (*mesh)->nau = (*mesh)->ntu = 0;
  (*mesh)->npmax = (*mesh)->namax = (*mesh)->ntmax = 0;
  (*mesh)->min[0] = (*mesh)->min[1] = DBL_MAX;
  (*mesh)->max[0] = (*mesh)->max[1] = -DBL_MAX;

  return DMG_SUCCESS;
}


int DMG_Free_mesh(DMG_pMesh mesh) {
  if (mesh == NULL) {
    return DMG_SUCCESS;
  }

  if (mesh->point) {
    free(mesh->point);
    mesh->point = NULL;
    mesh->np = DMG_UNSET;
  }
  if (mesh->edge) {
    free(mesh->edge);
    mesh->edge = NULL;
    mesh->na = DMG_UNSET;
  }
  if (mesh->tria) {
    free(mesh->tria);
    mesh->edge = NULL;
    mesh->nt = DMG_UNSET;
  }
  if (mesh->adja) {
    free(mesh->adja);
    mesh->adja = NULL;
  }

  free(mesh);
  mesh = NULL;

  return DMG_SUCCESS;
}


int DMG_allocMesh(DMG_pMesh mesh) {
  int i;

  mesh->npmax = MAX2(1.5 * mesh->np, DMG_NPMAX);
  mesh->ntmax = MAX2(1.5 * mesh->nt, DMG_NTMAX);
  mesh->namax = mesh->na;

  mesh->point = (DMG_pPoint) calloc(mesh->npmax, sizeof(DMG_Point));
  mesh->edge = (DMG_pEdge) calloc(mesh->namax, sizeof(DMG_Edge));
  mesh->tria = (DMG_pTria) calloc(mesh->ntmax, sizeof(DMG_Tria));

  if (mesh->point == NULL || mesh->edge == NULL || mesh->tria == NULL)
    return DMG_FAILURE;

  mesh->npu = mesh->np;
  mesh->nau = 0;
  mesh->ntu = mesh->nt;

  /* Set the tmp field of the unused point to be equal to the next unused point */
  /* This is used to keep track of the empty available points */
  for (i = mesh->npu ; i < mesh->npmax-1 ; i++) {
    mesh->point[i].tmp = i + 1;
  }
  for (i = mesh->ntu ; i < mesh->ntmax-1 ; i++) {
    mesh->tria[i].v[2] = i + 1;
  }

  /* Allocate the adjacency table */
  mesh->adja = (int*) calloc(3 * mesh->ntmax, sizeof(int));

  return DMG_SUCCESS;
}


int DMG_newPoint(DMG_pMesh mesh, double c[2]) {
  DMG_pPoint ppt;
  int ip;

  /* No memory available for a new point */
  if (mesh->npu == DMG_UNSET) return 0;

  /* New point takes the first available slot in the point array */
  ip = mesh->npu;

  if (mesh->npu >= mesh->np) mesh->np = mesh->npu + 1;
  ppt = &mesh->point[ip];
  memcpy(ppt->c, c, 2 * sizeof(double));
  mesh->npu = ppt->tmp;
  ppt->tmp = 0;

  return ip;
}


void DMG_delPoint(DMG_pMesh mesh, int ip) {
  DMG_pPoint ppt;

  ppt = &mesh->point[ip];
  memset(ppt, 0, sizeof(DMG_Point));
  ppt->tmp = mesh->npu;

  mesh->npu = ip;
  if (ip == mesh->np - 1) mesh->np--;
}


int DMG_newTria(DMG_pMesh mesh) {
  DMG_pTria pt;
  int it;

  if (mesh->ntu == DMG_UNSET) return 0;

  /* New tria takes the first available slot in the tria array */
  it = mesh->ntu;

  if (mesh->ntu >= mesh->nt) mesh->nt = mesh->ntu + 1;

  pt = &mesh->tria[it];
  mesh->ntu = pt->v[2];
  pt->v[2] = 0;
  pt->ref = 0;
  pt->flag = 0;

  return it;
}


void DMG_delTria(DMG_pMesh mesh, int it) {
  DMG_pTria pt;
  int iadj;

  pt = &mesh->tria[it];
  memset(pt, 0, sizeof(DMG_Tria));
  pt->v[2] = mesh->ntu;
  pt->qual = 0.0;

  iadj = 3 * it;
  if (mesh->adja)
    memset(&mesh->adja[iadj], 0, 3 * sizeof(int));

  mesh->ntu = it;
  if (it == mesh->nt - 1) mesh->nt--;
}
