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

  /** Compute and display the quality of the mesh */
  DMG_computeQual(mesh);
  DMG_displayQualHisto(mesh, nclass);

  /** Find the tria containing point c(xmin + 0.5 * delta_x, ymin + 0.5 * delta_y) */
  DMG_delaunay(mesh);
  c[0] = mesh->min[0] + 0.5 * (mesh->max[0] - mesh->min[0]);
  c[1] = mesh->min[1] + 0.5 * (mesh->max[1] - mesh->min[1]);

  for (i = 0 ; i < 64 ; i++) {
    list[i] = 0;
  }

  DMG_setAdja(mesh);
  printf("%f, %f \n", c[0], c[1]);
  list[0] = DMG_locTria(mesh, 0, c);

  /** Create the cavity starting from this tria */
  DMG_createCavity(mesh, c, list);
  for (i = 0 ; i < 64 ; i++) {
    printf("%d ", list[i]);
  }
  printf("\n");

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
