#ifndef DMG_H
#define DMG_H

#include "types_dmg.h"

/* Initialization and deletion of the structures */
int DMG_Init_mesh(DMG_pMesh *mesh);
int DMG_Free_mesh(DMG_pMesh mesh);

/* I/O */
int DMG_loadMesh_medit(DMG_pMesh mesh, char *filename);
int DMG_saveMesh_medit(DMG_pMesh mesh, char *filename);
int DMG_saveMeshAs3D_medit(DMG_pMesh mesh, char *filename);

/* Quality */
int DMG_computeQual(DMG_pMesh mesh);
double DMG_computeArea(double a[2], double b[2], double c[2]);
double DMG_computeTriaArea(DMG_pMesh mesh, DMG_Tria *pt);
double DMG_computeTriaQual(DMG_pMesh mesh, DMG_Tria *pt);
int DMG_displayQualHisto(DMG_pMesh mesh, int nclass);

/* Locate */
int DMG_baryCoord(DMG_pMesh mesh, DMG_Tria *pt, double c[2], double *det, double bc[3]);
int DMG_locTria(DMG_pMesh mesh, int start, double c[2], double bc[3]);

/* Adjacency relations */
int DMG_hashHedge(DMG_pMesh mesh);
int DMG_setAdja(DMG_pMesh mesh);
int buildVerticesBalls2D(DMG_pMesh mesh);

#endif /* DMG_H */
