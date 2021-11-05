#include "dmg.h"


int DMG_Init_mesh(DMG_pMesh *mesh) {
  *mesh = (DMG_pMesh)malloc(sizeof(DMG_Mesh));

  if (mesh == NULL) {
    fprintf(stderr, "Error: %s: mesh struct allocation failed\n", __func__);
    return DMG_FAILURE;
  }
  (*mesh)->np = (*mesh)->na = (*mesh)->nt = (*mesh)->ver = (*mesh)->dim = DMG_UNSET;

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
