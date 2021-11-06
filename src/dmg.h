#ifndef DMG_H
#define DMG_H

#include "types_dmg.h"

/* Useful to avoid modulos when going through the vertices/edges of a triangle */
static const int DMG_tria_vert[5] = {0, 1, 2, 0, 1};

/* Initialization and deletion of the main structures */
int DMG_Init_mesh(DMG_pMesh *mesh);
int DMG_Free_mesh(DMG_pMesh mesh);

/* I/O */
int DMG_loadMesh_medit(DMG_pMesh mesh, char *filename);
int DMG_saveMesh_medit(DMG_pMesh mesh, char *filename);
int DMG_saveMeshAs3D_medit(DMG_pMesh mesh, char *filename);
int DMG_saveQual_medit(DMG_pMesh mesh, char *filename);

/* Geometric routines */
double DMG_orient(double a[2], double b[2], double c[2]);
double DMG_inCircle(double a[2], double b[2], double c[2], double d[2]);
int DMG_baryCoord(DMG_pMesh mesh, DMG_pTria pt, double c[2], double *det, double bc[3]);
int DMG_locTria(DMG_pMesh mesh, int start, double c[2]);
int DMG_locTria_bary(DMG_pMesh mesh, int start, double c[2], double bc[3]);
int DMG_createCavity(DMG_pMesh mesh, double d[2], int it, int *list);

/*  Quality */
double DMG_computeTriaArea(DMG_pMesh mesh, DMG_pTria pt);
double DMG_computeTriaQual(DMG_pMesh mesh, DMG_pTria pt);
int DMG_computeQual(DMG_pMesh mesh);
int DMG_displayQualHisto(DMG_pMesh mesh, int nclass);

/* Adjacency relations */
int DMG_hashHedge(DMG_pMesh mesh, DMG_Hedge *htab);
int DMG_setAdja(DMG_pMesh mesh);

/* Queue manipulations */
DMG_Queue* DMG_createQueue();
DMG_Qnode* DMG_newNode(int k);
int DMG_enQueue(DMG_Queue *q, int k);
int DMG_deQueue(DMG_Queue *q);
int DMG_qFront(DMG_Queue *q);
int DMG_qRear(DMG_Queue *q);
int DMG_qIsEmpty(DMG_Queue *q);

#endif /* DMG_H */
