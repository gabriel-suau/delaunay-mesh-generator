#ifndef DMG_H
#define DMG_H

#include "types_dmg.h"

#define DMG_EPSILON 1e-16

#define DMG_ALPHA_2D 3.464101615137755 /* 6. / sqrt(3.) */

#define DMG_NPMAX 10000
#define DMG_NTMAX 20000

#define DMG_REALLOC_MULT 1.5

#define DMG_LIST_SIZE 256

/* Used for the hash function */
#define KA 7
#define KB 11

#define MAX2(a, b) (((a) >= (b)) ? (a) : (b))
#define MIN2(a, b) (((a) <= (b)) ? (a) : (b))
#define ABS(a) (((a) >= 0.) ? (a) : (-a))

#define DMG_VOK(ppt) (ppt && ((ppt)->tag != DMG_NULPT))
#define DMG_TOK(pt)  (pt && ((pt)->v[0] > 0))

/* Useful to avoid modulos when going through the vertices/edges of a triangle */
static const int DMG_tria_vert[5] = {0, 1, 2, 0, 1};


/* memory_dmg.c */
/**
 * \param mesh pointer toward a pointer toward the mesh structure
 * \return DMG_SUCCESS if success, DMG_FAILURE if fail
 *
 * Allocate the mesh structure and initialize the main fields.
 *
 * \remark mesh has to be a pointer toward a pointer so that the allocated memory
 * is not lost when exiting the function.
 */
int DMG_Init_mesh(DMG_pMesh *mesh);
/**
 * \param mesh pointer toward the mesh structure
 * \return DMG_SUCCESS (cannot fail)
 *
 * Free the mesh structure and their arrays.
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
/**
 * \param mesh pointer toward the mesh structure
 *
 * Pack the point/triangle arrays.
 */
int DMG_packMesh(DMG_pMesh mesh);


/* delaunay_dmg.c */
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
int DMG_insertPoint(DMG_pMesh mesh, int ip, int start);
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
 * Field point creation using one of the methods listed in the DMG_FieldPointMethod enum.
 */
int DMG_refineDelaunay(DMG_pMesh mesh);


/* enforcement_dmg.c */
/**
 * \param mesh pointer toward the mesh structure
 *
 * Enforce the constrained edges in the mesh.
 */
int DMG_enforceBndry(DMG_pMesh mesh);


/* domains_dmg.c */
/**
 * \param mesh pointer toward the mesh structure
 *
 * Mark the triangles that are exterior to the domain.
 */
int DMG_markSubDomains(DMG_pMesh mesh);
/**
 * \param mesh pointer toward the mesh structure
 *
 * Delete the triangles that were marked in the markSubdomains function, 
 * as well as the 4 BB vertices.
 */
int DMG_removeExterior(DMG_pMesh mesh);

/**
 * \param mesh pointer toward the mesh structure
 *
 * Perform mesh optimizations (swaps, collapses, smoothing).
 */
int DMG_optimizeMesh(DMG_pMesh mesh);


/* inout_dmg.c */
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
/**
 * \param mesh pointer toward the mesh structure
 * \param filename name of the .mesh file
 *
 * Save the size map to a medit ASCII (.sol) formatted file.
 */
int DMG_saveSizeMap_medit(DMG_pMesh mesh, char *filename);


/* geom_dmg.c */
/**
 * \param[in] a coordinates of the first point
 * \param[in] b coordinates of the second point
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
double DMG_orient(const double a[2], const double b[2], const double c[2]);
double DMG_lengthsq(const double a[2], const double b[2]);
double DMG_length(const double a[2], const double b[2]);
/**
 * \param[in] a coordinates of the first point of the first segment
 * \param[in] b coordinates of the second point of the first segment
 * \param[in] p coordinates of the first point of the second segment
 * \param[in] q coordinates of the second point of the second segment
 * \return 1 if the segments (ab) and (pq) intersect, else 0
 *
 * 2D segment-segment intersection test.
 *
 */
int DMG_segSegIntersect(const double a[2], const double b[2], const double p[2], const double q[2]);
/**
 * \param[in] a coordinates of the first triangle point
 * \param[in] b coordinates of the second triangle point
 * \param[in] c coordinates of the third triangle point
 * \param[in] p coordinates of the first segment point
 * \param[in] q coordinates of the second segment point
 * \return sum (integer between 0 and 7)
 *
 * Implementation of the 2D segment-triangle intersection test.
 * The result is a sum whose value is given by : 
 * sum = 0 
 *     + 1 if edge (ab) and (pq) intersect
 *     + 2 if edge (bc) and (pq) intersect
 *     + 4 if edge (ca) and (pq) intersect
 */
int DMG_triaSegIntersect(const double a[2], const double b[2], const double c[2], const double p[2], const double q[2]);
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
double DMG_inCircle(const double a[2], const double b[2], const double c[2], const double d[2]);
/**
 * \param[in] mesh pointer toward the mesh structure
 * \return DMG_SUCCESS if success, DMG_FAILURE if fail
 *
 * Find the triangle containing point c with an exhaustive search.
 */
int DMG_computeSizeMap(DMG_pMesh mesh);


/* locate_dmg.c */
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


/* ball_dmg.c */
int DMG_createBall(DMG_pMesh mesh, int ip, int ptcount, const int *ptlist);
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


/* quality_dmg.c */
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


/* hash_dmg.c */
int DMG_hashHedge(DMG_pMesh mesh, DMG_Hedge *htab);
int DMG_setAdja(DMG_pMesh mesh);


/* cavity_dmg.c */
int DMG_createCavity(DMG_pMesh mesh, double d[2], int start, int *ptlist);


/* local_dmg.c */
/** 
 * \param[in] mesh pointer toward the mesh structure
 * \param[in] it index of the triangle
 * \param[in] k local index of the edge to swap
 * \return 1 if the swap is legal, else 0
 *
 * Check if the swap of edge k (local index) in triangle it is legal.
 */
int DMG_chkSwap(DMG_pMesh mesh, int it, int k);
/**
 * \param[in] mesh pointer toward the mesh structure
 * \param[in] it index of the triangle
 * \param[in] k local index of the edge to swap
 * \return index jt of the triangle adjacent to it by the swapped edge
 *
 * Perform the swap of local edge k of triangle it. Example :
 * Let us consider it = (q, a, p) and jt = (q, p, b). After the swap,
 * they become it = (q, a, b) and jt = (p, b, a). Adjacency relationships are updated so that
 * after the swap, triangles it and jt are adjacent by their edge number 0.
 */
int DMG_swap(DMG_pMesh mesh, int it, int k);
/** 
 * \param[in] mesh pointer toward the mesh structure
 * \param[in] it index of the triangle
 * \param[in] k local index of the edge to swap
 * \return 1 if the swap is legal, else 0
 *
 * Check if the swap of edge k (local index) in triangle it is legal.
 */
int DMG_chkCol(DMG_pMesh mesh, int it, int k);
/**
 * \param[in] mesh pointer toward the mesh structure
 * \param[in] it index of the triangle
 * \param[in] k local index of the edge to swap
 * \return index jt of the triangle adjacent to it by the swapped edge
 *
 * Perform the swap of local edge k of triangle it. Example :
 * Let us consider it = (q, a, p) and jt = (q, p, b). After the swap,
 * they become it = (q, a, b) and jt = (p, b, a). Adjacency relationships are updated so that
 * after the swap, triangles it and jt are adjacent by their edge number 0.
 */
int DMG_collapse(DMG_pMesh mesh, int count, int *list);


/* queue_dmg.c */
DMG_Queue* DMG_createQueue();
void DMG_freeQueue(DMG_Queue *q);
int DMG_enQueue(DMG_Queue *q, int k);
int DMG_deQueue(DMG_Queue *q);
int DMG_qIsEmpty(DMG_Queue *q);


/* grid_dmg.c */
DMG_Grid* DMG_createGrid(double min[2], double max[2], double h);
void DMG_freeGrid(DMG_Grid *g);
int DMG_gCell(DMG_Grid *g, double c[2]);

#endif /* DMG_H */
