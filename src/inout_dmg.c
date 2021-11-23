#include "dmg.h"


int DMG_loadMesh_medit(DMG_pMesh mesh, char *filename) {
  FILE *file = NULL;
  DMG_pPoint ppt;
  DMG_pEdge pa;
  DMG_pTria pt;
  int posnp, posna, posnt;
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

  /* Scan a first time to get the nb of entities */
  while(fscanf(file, "%s", chain) != EOF && strncmp(chain, "End", strlen("End"))){

    if (!strncmp(chain, "MeshVersionFormatted", strlen("MeshVersionFormatted"))) {
      fscanf(file, "%d", &mesh->ver);
      continue;
    }
    else if (!strncmp(chain, "Dimension", strlen("Dimension"))) {
      fscanf(file, "%d", &mesh->dim);
      continue;
    }
    else if (!strncmp(chain, "Vertices", strlen("Vertices"))) {
      fscanf(file, "%d", &mesh->np);
      posnp = ftell(file);
      continue;
    }
    else if (!strncmp(chain, "Edges", strlen("Edges"))) {
      fscanf(file, "%d", &mesh->na);
      posna = ftell(file);
      continue;
    }
    else if (!strncmp(chain, "Triangles", strlen("Triangles"))) {
      fscanf(file, "%d", &mesh->nt);
      posnt = ftell(file);
      continue;
    }

  }

  if (!mesh->np) {
    fprintf(stdout, "Error: %s:%d : No point in the input mesh\n", __func__, __LINE__);
    return DMG_FAILURE;
  }

  /* Allocate memory */
  if (DMG_allocMesh(mesh) == DMG_FAILURE) {
    fprintf(stderr, "Error! %s:%d : Could not allocate the mesh entities arrays.\n", __func__, __LINE__);
    return DMG_FAILURE;
  }

  /* Read the mesh */

  /* Vertices */
  rewind(file);
  fseek(file, posnp, SEEK_SET);
  if (mesh->dim == 2) {
    for (i = 1 ; i <= mesh->np ; i++) {
      ppt = &mesh->point[i];
      fscanf(file, "%lf %lf %d", &ppt->c[0], &ppt->c[1], &ppt->ref);
    }
  } else if (mesh->dim == 3) {
    mesh->dim = 2;
    for (i = 1 ; i <= mesh->np ; i++) {
      ppt = &mesh->point[i];
      fscanf(file, "%lf %lf %lf %d", &ppt->c[0], &ppt->c[1], &dummy, &ppt->ref); 
    }
  }

  /* Edges */
  rewind(file);
  fseek(file, posna, SEEK_SET);
  for (i = 1 ; i <= mesh->na ; i++) {
    pa = &mesh->edge[i];
    fscanf(file, "%d %d %d", &pa->v[0], &pa->v[1], &pa->ref);
  }

  /* Triangles */
  if (mesh->nt) {
    rewind(file);
    fseek(file, posnt, SEEK_SET);
    for (i = 1 ; i <= mesh->nt ; i++) {
      pt = &mesh->tria[i];
      fscanf(file, "%d %d %d %d", &pt->v[0], &pt->v[1], &pt->v[2], &pt->ref);
      pt->flag = 0;

      if (DMG_computeTriaArea(mesh, pt) < 0.) {
        tmp = pt->v[2];
        pt->v[2] = pt->v[1];
        pt->v[1] = tmp;
      }
    }    
  }

  fclose(file);

  /* Print the nmber of entities */
  fprintf(stdout, "%d/%d (%d) vertices \n", mesh->np, mesh->npmax, mesh->npu);
  fprintf(stdout, "%d/%d (%d) edges \n", mesh->na, mesh->namax, mesh->nau);
  fprintf(stdout, "%d/%d (%d) triangles \n", mesh->nt, mesh->ntmax, mesh->ntu);

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
  for (i = 1 ; i <= mesh->np ; i++) {
    ppt = &mesh->point[i];
    fprintf(file, "%lf %lf %d\n", ppt->c[0], ppt->c[1], ppt->ref);
  }

  /** DMG_Edges */
  strcpy(chain, "\nEdges\n");
  fprintf(file, "%s", chain);
  fprintf(file, "%d\n", mesh->na);
  for (i = 1 ; i <= mesh->na ; i++) {
    pa = &mesh->edge[i];
    fprintf(file, "%d %d %d\n", pa->v[0], pa->v[1], pa->ref);
  }

  /** DMG_Trias */
  strcpy(chain, "\nTriangles\n");
  fprintf(file, "%s", chain);
  fprintf(file, "%d\n", mesh->nt);
  for (i = 1 ; i <= mesh->nt ; i++) {
    pt = &mesh->tria[i];
    fprintf(file, "%d %d %d %d\n", pt->v[0], pt->v[1], pt->v[2], pt->ref);
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
  for (i = 1 ; i <= mesh->np ; i++) {
    ppt = &mesh->point[i];
    fprintf(file, "%lf %lf 0 %d\n", ppt->c[0], ppt->c[1], ppt->ref);
  }

  /** DMG_Edges */
  strcpy(chain, "\nEdges\n");
  fprintf(file, "%s", chain);
  fprintf(file, "%d\n", mesh->na);
  for (i = 1 ; i <= mesh->na ; i++) {
    pa = &mesh->edge[i];
    fprintf(file, "%d %d %d\n", pa->v[0], pa->v[1], pa->ref);
  }

  /** DMG_Trias */
  strcpy(chain, "\nTriangles\n");
  fprintf(file, "%s", chain);
  fprintf(file, "%d\n", mesh->nt);
  for (i = 1 ; i <= mesh->nt ; i++) {
    pt = &mesh->tria[i];
    fprintf(file, "%d %d %d %d\n", pt->v[0], pt->v[1], pt->v[2], pt->ref);
  }

  /** End string*/
  strcpy(chain, "\nEnd\n");
  fprintf(file, "%s", chain);
  
  fclose(file);

  return DMG_SUCCESS;
}


int DMG_saveQual_medit(DMG_pMesh mesh, char *filename) { 
  DMG_pTria pt;
  FILE *file = NULL;
  char chain[127];
  int i;

  if (mesh == NULL) {
    fprintf(stderr, "Error: %s: mesh struct not allocated\n", __func__);
    return DMG_FAILURE;
  }
  if (!mesh->np || !mesh->nt) {
    fprintf(stderr, "Error: %s:%d: can't save quality of an empty mesh\n", __func__, __LINE__);
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

  /** Quality of triangles */
  strcpy(chain, "\nSolAtTriangles\n");
  fprintf(file, "%s", chain);
  fprintf(file, "%d\n", mesh->nt);
  fprintf(file, "1 1\n");
  for (i = 1 ; i <= mesh->nt ; i++) {
    pt = &mesh->tria[i];
    fprintf(file, "%lf\n", pt->qual);
  }

  /** End string*/
  strcpy(chain, "\nEnd\n");
  fprintf(file, "%s", chain);

  fclose(file);

  return DMG_SUCCESS;
}
