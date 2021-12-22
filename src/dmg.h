#ifndef DMG_H
#define DMG_H

#include "types_dmg.h"

#define DMG_EPSILON 1e-16

#define DMG_ALPHA_2D 3.464101615137755 /* 6. / sqrt(3.) */

#define DMG_NPMAX 5000
#define DMG_NTMAX 10000

#define DMG_REALLOC_MULT 1.5

#define DMG_LIST_SIZE 64

#define MAX2(a, b) (((a) >= (b)) ? (a) : (b))
#define MIN2(a, b) (((a) <= (b)) ? (a) : (b))
#define ABS(a) (((a) >= 0.) ? (a) : (-a))

/* Useful to avoid modulos when going through the vertices/edges of a triangle */
static const int DMG_tria_vert[5] = {0, 1, 2, 0, 1};

/* Creation and deletion of the main structures */
/**
 * \param mesh pointer toward a pointer toward the mesh structure
 * \return DMG_SUCCESS if success, DMG_FAILURE if fail
 *
 * Allocate the mesh structure and initialize the main fields.
 *
 * \remark mesh had to be a pointer toward a pointer so that the allocated memory
 * is not lost when exiting the function.
 */
int DMG_Init_mesh(DMG_pMesh *mesh);

/**
 * \param mesh pointer toward the mesh structure
 * \return DMG_SUCCESS (cannot fail)
 *
 * Free the mesh structure and its arrays.
 */
int DMG_Free_mesh(DMG_pMesh mesh);

/**
 * \param mesh pointer toward the mesh structure
 * \return DMG_SUCCESS if success, DMG_FAILURE if fail
 *
 * Allocate the mesh entites arrays using the current npmax, namax and ntmax values.
 * Initialize the unused chain to keep track of the empty available points and tria.
 */
int DMG_allocMesh(DMG_pMesh mesh);

/**
 * \param mesh pointer toward the mesh structure
 * \param c coordinates of the new point
 * \return global index of the new point if success, else return 0 (no slot available)
 *
 * Add a new point to the mesh, in the first unused slot of the point array, and update
 * the unused chain.
 */
int DMG_newPoint(DMG_pMesh mesh, double c[2]);

/**
 * \param mesh pointer toward the mesh structure
 * \param ip global index of the point
 *
 * Delete point ip of the mesh, and update the unused chain.
 */
void DMG_delPoint(DMG_pMesh mesh, int ip);

/**
 * \param mesh pointer toward the mesh structure
 * \return global index of the new tria if success, else return 0 (no slot available)
 *
 * Add a new triangle to the mesh, in the first unused slot of the tria array, and update
 * the unused chain.
 */
int DMG_newTria(DMG_pMesh mesh);

/**
 * \param mesh pointer toward the mesh structure
 * \param it global index of the triangle
 *
 * Delete triangle it of the mesh, zero the adjacency relationship, and update the unused chain.
 */
void DMG_delTria(DMG_pMesh mesh, int it);

/* Global mesh manipulations */
/**
 * \param mesh pointer toward the mesh structure
 *
 * Perform the constrained delaunay triangulation of the mesh. The mesh must be a boundary mesh,
 * therefore it has to contain no triangle. The constrained edges are contained in the edge field
 * of the mesh structure.
 */
int DMG_delaunay(DMG_pMesh mesh);

/**
 * \param mesh pointer toward the mesh structure
 *
 * Create the 4 vertices and 2 triangles of the bounding box. At the end of this function,
 * the 4 BB vertices are mesh->np-3, mesh->np-2, mesh->np-1 and mesh->np, and the 2 BB triangles
 * are triangles 1 and 2.
 */
int DMG_initDelaunay(DMG_pMesh mesh);

/**
 * \param mesh pointer toward the mesh structure
 *
 * Insert points 1 to mesh->np-4 (boundary points) in the mesh (the 4 points of the bounding box 
 * are mesh->np-3 to mesh->np and have already been inserted).
 */
int DMG_insertBdryPoints(DMG_pMesh mesh);

/**
 * \param mesh pointer toward the mesh structure
 *
 * Enforce the constrained edges in the mesh.
 */
int DMG_enforceBndry(DMG_pMesh mesh);

/**
 * \param mesh pointer toward the mesh structure
 *
 * Remove the triangles outside the domain boundaries.
 */
int DMG_markSubDomains(DMG_pMesh mesh);

/**
 * \param mesh pointer toward the mesh structure
 *
 * Insert the volumes points inside the mesh.
 */
int DMG_insertVolumePoints(DMG_pMesh mesh);

/* I/O */
/**
 * \param mesh pointer toward the mesh structure
 * \param filename name of the .mesh file
 *
 * Load a mesh from a medit ASCII (.mesh) formatted file.
 */
int DMG_loadMesh_medit(DMG_pMesh mesh, char *filename);

/**
 * \param mesh pointer toward the mesh structure
 * \param filename name of the .mesh file
 *
 * Save a mesh to a medit ASCII (.mesh) formatted file.
 */
int DMG_saveMesh_medit(DMG_pMesh mesh, char *filename);

/**
 * \param mesh pointer toward the mesh structure
 * \param filename name of the .mesh file
 *
 * Save a mesh to a medit ASCII (.mesh) formatted file as if it were 3D.
 * The z coordinate is set to zero.
 */
int DMG_saveMeshAs3D_medit(DMG_pMesh mesh, char *filename);

/**
 * \param mesh pointer toward the mesh structure
 * \param filename name of the .mesh file
 *
 * Save the quality of the mesh to a medit ASCII (.sol) formatted file.
 */
int DMG_saveQual_medit(DMG_pMesh mesh, char *filename);

/* Geometric routines */
/**
 * \param[in] a coordinates of the first point
 * \param[in] a coordinates of the second point
 * \param[in] c coordinates of the third point
 * \return signed area of triangle (a, b, c)
 *
 * Compute the signed area of triangle (a, b, c) :
 *
 *        |ax  ay  1|
 * area = |bx  by  1|
 *        |cx  cy  1|
 *
 * The result is :
 *    > 0 if a, b, c are ordered counterclockwise
 *    < 0 if a, b, c are ordered clockwise
 *    = 0 if a, b, c are colinear
 */
double DMG_orient(double a[2], double b[2], double c[2]);

/**
 * \param[in] a coordinates of the first point of the triangle
 * \param[in] b coordinates of the second point of the triangle
 * \param[in] c coordinates of the third point of the triangle
 * \param[in] d coordinates of the tested point
 * \return the value of the circumcircle determinant
 *
 * Compute the value of the following determinant:
 *
 *       | ax  ay  (ax^2 + ay^2)  1|
 *       | bx  by  (bx^2 + by^2)  1|
 *   D = | cx  cy  (cx^2 + cy^2)  1|
 *       | dx  dy  (dx^2 + dy^2)  1|
 *
 * The result is : 
 *    > 0 if d lies inside the circumcircle of triangle (a, b, c)
 *    < 0 if d lies outside the circumcircle of triangle (a, b, c)
 *    = 0 if d lies on the circumcircle of triangle (a, b, c)
 *
 * \remark a, b and c must me in counterclockwise order; if they are in clockwise order,
 * the result is the opposite.
 */
double DMG_inCircle(double a[2], double b[2], double c[2], double d[2]);

/**
 * \param[in] mesh pointer toward the mesh structure
 * \param[in] pt pointer toward the triangle structure
 * \param[in] c coordinates of the point
 * \param[out] det signed area of the triangle pt
 * \param[out] bc barycentric coordinates of point c
 * \return 1 if the c lies inside the triangle, else 0
 *
 * Compute the barycentric coordinates of point c in triangle pt.
 *
 * \remark This function is not used in the final code.
 */
int DMG_baryCoord(DMG_pMesh mesh, DMG_pTria pt, double c[2], double *det, double bc[3]);

/**
 * \param[in] mesh pointer toward the mesh structure
 * \param[in] start index of the starting triangle
 * \param[in] c coordinates of the point to be located
 * \return index of the triangle containing the point c
 *
 * Find the triangle containing point c starting from triangle start,
 * and travelling by adjacency. If this strategy fails, perform an exhaustive
 * search among all the triangles (call to DMG_loctria_brute).
 */
int DMG_locTria(DMG_pMesh mesh, int start, double c[2]);

/**
 * \param[in] mesh pointer toward the mesh structure
 * \param[in] c coordinates of the point to be located
 * \return index of the triangle containing the point c if success, 0 if fail.
 *
 * Find the triangle containing point c with an exhaustive search.
 */
int DMG_locTria_brute(DMG_pMesh mesh, double c[2]);
int DMG_listCrossTriangles(DMG_pMesh mesh, int i1, int i2, int it, int *list);
int DMG_chkDelaunay(DMG_pMesh mesh);

/* Ball computation */
/**
 * \param[in] mesh pointer toward the mesh structure
 * \param[in] it index of the starting triangle
 * \param[in] iploc local index of the point in triangle it
 * \param[out] list ball of point iploc
 * \return number of triangles in list
 *
 * Find the ball of local point iploc in triangle it starting from triangle it.
 *
 * \remark on output, list[k] = 3 * jt + l where jt is the index of the triangle
 * and l = (0, 1 or 2) is the local index of the point in triangle jt
 */
int DMG_findBall(DMG_pMesh mesh, int it, int iploc, int *list);

/* Quality */
/**
 * \param[in] mesh pointer toward the mesh structure
 * \param[in] pt pointer toward the triangle structure
 * \return the signed area of triangle pt
 *
 * Compute the signed area of triangle pt
 */
double DMG_computeTriaArea(DMG_pMesh mesh, DMG_pTria pt);

/**
 * \param[in] mesh pointer toward the mesh structure
 * \param[in] pt pointer toward the triangle structure
 * \return the quality of triangle pt, or 0 if the triangle is badly oriented
 *
 * Compute the quality Q of triangle pt defined as : 
 *             area(pt)       
 * Q = -----------------------
 *      l12^2 + l23^2 + l31^2 
 */
double DMG_computeTriaQual(DMG_pMesh mesh, DMG_pTria pt);

/**
 * \param[in] mesh pointer toward the mesh structure
 * \return the quality of all triangles in the mesh
 */
int DMG_computeQual(DMG_pMesh mesh);

/**
 * \param[in] mesh pointer toward the mesh structure
 * \param[in] nclass number of classes of the histogram
 * \return DMG_SUCCESS
 *
 * Display a quality histogram on the standard output
 */
int DMG_displayQualHisto(DMG_pMesh mesh, int nclass);

/* Adjacency relations */
int DMG_hashHedge(DMG_pMesh mesh, DMG_Hedge *htab);
int DMG_setAdja(DMG_pMesh mesh);

/* Local modifications */
int DMG_insertPoint(DMG_pMesh mesh, int ip, int start);
int DMG_createCavity(DMG_pMesh mesh, double d[2], int start, int *ptlist);
int DMG_createBall(DMG_pMesh mesh, int ip, int ptcount, const int *ptlist);
int DMG_chkSwap(DMG_pMesh mesh, int it, int k);

/**
 * \param[in] mesh pointer toward the mesh structure
 * \param[in] it index of the triangle
 * \param[in] k local index of the edge to swap
 * \param[out] ita map the old adjacencies of triangle it to the new ones
 * \param[out] jta map the old adjacencies of triangle jt to the new ones
 * \return index jt of the triangle adjacent to it by the swapped edge
 *
 * Perform the swap of local edge k of triangle it. Example :
 * Let us consider it = (q, a, p) and jt = (q, p, b). After the swap,
 * they become it = (q, a, b) and jt = (p, b, a). Adjacency relationships are updated so that
 * after the swap, triangles it and jt are adjacent by their edge number 0.
 */
int DMG_swap(DMG_pMesh mesh, int it, int k, int ita[3], int jta[3]);

/* Queue manipulations */
DMG_Queue* DMG_createQueue();
void DMG_freeQueue(DMG_Queue *q);
int DMG_enQueue(DMG_Queue *q, int k);
int DMG_deQueue(DMG_Queue *q);
int DMG_qIsEmpty(DMG_Queue *q);

#endif /* DMG_H */
