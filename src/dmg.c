#include "dmg.h"


int main(int argc, char **argv)
{
  DMG_pMesh mesh;
  char *filein, *fileout, *qualout;
  int nclass = 5;

  mesh = NULL;
  filein = fileout = qualout = NULL;

  /** Check arguments */
  if (argc != 3) {
    fprintf(stderr, "Usage : %s filein fileout \n", argv[0]);
    exit(EXIT_FAILURE);
  }

  filein = (char*) malloc((strlen(argv[1]) + 1) * sizeof(char));
  if (filein == NULL) {
    fprintf(stderr, "Empty filein \n");
    exit(EXIT_FAILURE);
  }
  strcpy(filein, argv[1]);

  fileout = (char*) malloc((strlen(argv[2]) + 1) * sizeof(char));
  if (fileout == NULL) {
    fprintf(stderr, "Empty fileout \n");
    exit(EXIT_FAILURE);
  }
  strcpy(fileout, argv[2]);

  qualout = (char*) malloc(strlen(fileout) * sizeof(char));
  strncpy(qualout, fileout, strlen(fileout) - strlen(".mesh"));
  strcat(qualout, ".sol");

  /** Init the mesh struct */
  if (DMG_Init_mesh(&mesh) == DMG_FAILURE)
    goto free_and_return;

  /** Read the mesh */
  if (DMG_loadMesh_medit(mesh, filein) == DMG_FAILURE)
    goto free_and_return;

  /** Delaunay meshing */
  if (DMG_delaunay(mesh) == DMG_FAILURE)
    goto free_and_return;

  /** Compute and display the quality of the mesh */
  DMG_computeQual(mesh);
  DMG_displayQualHisto(mesh, nclass);

  /** Save the mesh */
  if (DMG_saveMesh_medit(mesh, fileout) == DMG_FAILURE)
    goto free_and_return;

  /** Save the size map */
  if (DMG_saveSizeMap_medit(mesh, qualout) == DMG_FAILURE)
    goto free_and_return;

  /* /\** Save the quality *\/ */
  /* if (DMG_saveQual_medit(mesh, qualout) == DMG_FAILURE) */
  /*   goto free_and_return; */

  /** Free all allocated memory */
 free_and_return:
  free(filein); filein = NULL;
  free(fileout); fileout = NULL;
  free(qualout); qualout = NULL;
  DMG_Free_mesh(mesh);

  return EXIT_SUCCESS;
}
