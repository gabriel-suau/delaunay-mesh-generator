#include "dmg.h"


int DMG_loadMesh_medit(DMG_pMesh mesh, char *filename) {
  FILE *file = NULL;
  DMG_pPoint ppt;
  DMG_pEdge pa;
  DMG_pTria pt;
  char chain[127];
  int i, tmp;
  double dummy;

  if (mesh == NULL) {
    fprintf(stderr, "Error: %s: mesh struct not allocated\n", __func__);
    return DMG_FAILURE;
  }

  file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Error: %s: can't open %s \n", __func__, filename);
    return DMG_FAILURE;    
  }

  while(fscanf(file, "%s", chain) != EOF && strncmp(chain, "End", strlen("End"))){

    if (!strncmp(chain, "MeshVersionFormatted", strlen("MeshVersionFormatted"))) {
      fscanf(file, "%d", &mesh->ver);
      continue;
    }

    if (!strncmp(chain, "Dimension", strlen("Dimension"))) {
      fscanf(file, "%d", &mesh->dim);
      continue;
    }

    if (!strncmp(chain, "Vertices", strlen("Vertices"))) {
      fscanf(file, "%d", &mesh->np);
      mesh->point = (DMG_Point*) malloc(mesh->np * sizeof(DMG_Point));
      if (mesh->dim == 2) {
        for (i = 0 ; i < mesh->np ; i++) {
          ppt = &mesh->point[i];
          fscanf(file, "%lf %lf %d", &ppt->c[0], &ppt->c[1], &ppt->ref); 
        }
      } else if (mesh->dim == 3) {
        mesh->dim = 2;
        for (i = 0 ; i < mesh->np ; i++) {
          ppt = &mesh->point[i];
          fscanf(file, "%lf %lf %lf %d", &ppt->c[0], &ppt->c[1], &dummy, &ppt->ref); 
        }
      }
      continue;
    }

    if (!strncmp(chain, "Edges", strlen("Edges"))) {
      fscanf(file, "%d", &mesh->na);
      mesh->edge = (DMG_Edge*) malloc(mesh->na * sizeof(DMG_Edge));
      for (i = 0 ; i < mesh->na ; i++) {
        pa = &mesh->edge[i];
        fscanf(file, "%d %d %d", &pa->v[0], &pa->v[1], &pa->ref);
        pa->v[0]--;
        pa->v[1]--;
      }
      continue;
    }

    if (!strncmp(chain, "Triangles", strlen("Triangles"))) {
      fscanf(file, "%d", &mesh->nt);
      mesh->tria = (DMG_Tria*) malloc(mesh->nt * sizeof(DMG_Tria));

      for (i = 0 ; i < mesh->nt ; i++) {
        pt = &mesh->tria[i];
        fscanf(file, "%d %d %d %d", &pt->v[0], &pt->v[1], &pt->v[2], &pt->ref);
        pt->v[0]--;
        pt->v[1]--;
        pt->v[2]--;

        memset(pt->edge, DMG_UNSET, 3 * sizeof(int));

        if (DMG_computeTriaArea(mesh, pt) < 0.) {
          tmp = pt->v[2];
          pt->v[2] = pt->v[1];
          pt->v[1] = tmp;
        }
      }

      continue;
    }

  }

  fclose(file);

  return DMG_SUCCESS;
}



int DMG_saveMesh_medit(DMG_pMesh mesh, char *filename) {
  DMG_pPoint ppt;
  DMG_pEdge pa;
  DMG_pTria pt;
  FILE *file = NULL;
  char chain[127];
  int i;

  if (mesh == NULL) {
    fprintf(stderr, "Error: %s: mesh struct not allocated\n", __func__);
    return DMG_FAILURE;
  }
  if (!mesh->np || !mesh->nt) {
    fprintf(stderr, "Error: %s: can't save an empty mesh\n", __func__);
    return DMG_FAILURE;
  }

  file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Error: %s: can't open %s \n", __func__, filename);
    return DMG_FAILURE;
  }

  /** Header */
  strcpy(chain, "MeshVersionFormatted 2\n");
  fprintf(file, "%s", chain);
  strcpy(chain, "\nDimension 2\n");
  fprintf(file, "%s", chain);

  /** Vertices */
  strcpy(chain, "\nVertices\n");
  fprintf(file, "%s", chain);
  fprintf(file, "%d\n", mesh->np);
  for (i = 0 ; i < mesh->np ; i++) {
    ppt = &mesh->point[i];
    fprintf(file, "%lf %lf %d\n", ppt->c[0], ppt->c[1], ppt->ref);
  }

  /** DMG_Edges */
  strcpy(chain, "\nEdges\n");
  fprintf(file, "%s", chain);
  fprintf(file, "%d\n", mesh->na);
  for (i = 0 ; i < mesh->na ; i++) {
    pa = &mesh->edge[i];
    fprintf(file, "%d %d %d\n", pa->v[0]+1, pa->v[1]+1, pa->ref);
  }

  /** DMG_Trias */
  strcpy(chain, "\nTriangles\n");
  fprintf(file, "%s", chain);
  fprintf(file, "%d\n", mesh->nt);
  for (i = 0 ; i < mesh->nt ; i++) {
    pt = &mesh->tria[i];
    fprintf(file, "%d %d %d %d\n", pt->v[0]+1, pt->v[1]+1, pt->v[2]+1, pt->ref);
  }

  /** End string*/
  strcpy(chain, "\nEnd\n");
  fprintf(file, "%s", chain);

  fclose(file);

  return DMG_SUCCESS;
}


int DMG_saveMeshAs3D_medit(DMG_pMesh mesh, char *filename) {
  DMG_pPoint ppt;
  DMG_pEdge pa;
  DMG_pTria pt;
  FILE *file = NULL;
  char chain[127];
  int i;

  if (mesh == NULL) {
    fprintf(stderr, "Error: %s:%d: mesh struct not allocated\n", __func__, __LINE__);
    return DMG_FAILURE;
  }
  if (!mesh->np || !mesh->nt) {
    fprintf(stderr, "Error: %s:%d: can't save an empty mesh\n", __func__, __LINE__);
    return DMG_FAILURE;
  }

  file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Error: %s:%d: can't open %s \n", __func__, __LINE__, filename);
    return DMG_FAILURE;
  }

  /** Header */
  strcpy(chain, "MeshVersionFormatted 2\n");
  fprintf(file, "%s", chain);
  strcpy(chain, "\nDimension 3\n");
  fprintf(file, "%s", chain);

  /** Vertices */
  strcpy(chain, "\nVertices\n");
  fprintf(file, "%s", chain);
  fprintf(file, "%d\n", mesh->np);
  for (i = 0 ; i < mesh->np ; i++) {
    ppt = &mesh->point[i];
    fprintf(file, "%lf %lf 0 %d\n", ppt->c[0], ppt->c[1], ppt->ref);
  }

  /** DMG_Edges */
  strcpy(chain, "\nEdges\n");
  fprintf(file, "%s", chain);
  fprintf(file, "%d\n", mesh->na);
  for (i = 0 ; i < mesh->na ; i++) {
    pa = &mesh->edge[i];
    fprintf(file, "%d %d %d\n", pa->v[0]+1, pa->v[1]+1, pa->ref);
  }

  /** DMG_Trias */
  strcpy(chain, "\nTriangles\n");
  fprintf(file, "%s", chain);
  fprintf(file, "%d\n", mesh->nt);
  for (i = 0 ; i < mesh->nt ; i++) {
    pt = &mesh->tria[i];
    fprintf(file, "%d %d %d %d\n", pt->v[0]+1, pt->v[1]+1, pt->v[2]+1, pt->ref);
  }

  /** End string*/
  strcpy(chain, "\nEnd\n");
  fprintf(file, "%s", chain);
  
  fclose(file);

  return DMG_SUCCESS;
}
