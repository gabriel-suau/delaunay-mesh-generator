#ifndef DMG_H
#define DMG_H

#include "types_dmg.h"

#define DMG_EPSILON 1e-16

#define DMG_ALPHA_2D 3.464101615137755 /* 6. / sqrt(3.) */

#define MAX2(a,b) (((a) > (b)) ? (a) : (b))
#define MIN2(a,b) (((a) < (b)) ? (a) : (b))
#define ABS(a) (((a) > 0.) ? (a) : (-a))

#define DMG_NPMAX 5000
#define DMG_NTMAX 10000

#define DMG_REALLOC_MULT 1.5

/* Useful to avoid modulos when going through the vertices/edges of a triangle */
static const int DMG_tria_vert[5] = {0, 1, 2, 0, 1};

/* Creation and deletion of the main structures */
int DMG_Init_mesh(DMG_pMesh *mesh);
int DMG_Free_mesh(DMG_pMesh mesh);
int DMG_allocMesh(DMG_pMesh mesh);
int DMG_newPoint(DMG_pMesh mesh, double c[2]);
void DMG_delPoint(DMG_pMesh mesh, int i);
int DMG_newTria(DMG_pMesh mesh);
void DMG_delTria(DMG_pMesh mesh, int i);

/* Mesh manipulations */
int DMG_delaunay(DMG_pMesh mesh); /* Main function */
int DMG_initDelaunay(DMG_pMesh mesh); /* Create the 4 vertices of the bounding box and the 2 first triangles */
int DMG_insertBdryPoints(DMG_pMesh mesh); /* Insert the boundary points in the delaunay triangulation */
int DMG_forceBndry(DMG_pMesh mesh);

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
int DMG_createCavity(DMG_pMesh mesh, double d[2], int *list);

/* Quality */
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
