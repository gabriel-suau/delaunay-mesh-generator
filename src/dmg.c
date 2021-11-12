#include "dmg.h"


int main(int argc, char **argv)
{
  DMG_Mesh* mesh;
  char *filein, *fileout, *qualout;
  int nclass = 5, list[64], i;
  double c[2];

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
  if (DMG_Init_mesh(&mesh) == DMG_FAILURE) {
    exit(EXIT_SUCCESS);
  }

  /** Read the mesh */
  if (DMG_loadMesh_medit(mesh, filein) == DMG_FAILURE) {
    DMG_Free_mesh(mesh);
    exit(EXIT_FAILURE);
  }

  /** Delaunay meshing */
  DMG_delaunay(mesh);

  /** Compute and display the quality of the mesh */
  DMG_computeQual(mesh);
  DMG_displayQualHisto(mesh, nclass);

  /** Save the mesh */
  if (DMG_saveMesh_medit(mesh, fileout) == DMG_FAILURE) {
    DMG_Free_mesh(mesh);
    exit(EXIT_FAILURE);
  }

  /** Save the quality */
  if (DMG_saveQual_medit(mesh, qualout) == DMG_FAILURE) {
    DMG_Free_mesh(mesh);
    exit(EXIT_FAILURE);
  }

  /** Free all allocated memory */
  free(filein); filein = NULL;
  free(fileout); fileout = NULL;
  free(qualout); qualout = NULL;
  DMG_Free_mesh(mesh);

  return EXIT_SUCCESS;
}
