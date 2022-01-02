#ifndef TYPES_DMG_H
#define TYPES_DMG_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>

#include "dmgversion.h"

#define DMG_FAILURE 1

#define DMG_SUCCESS 0

#define DMG_UNSET -1

#define DMG_NULPT 10
#define DMG_VALIDPT 11
#define DMG_BDYPT 12

enum DMG_FieldPointMethod {
  DMG_FPM_EdgeBased,
  DMG_FPM_VoronoiVertex,
  DMG_FPM_VoronoiSegment
};

enum DMG_Flags {
  DMG_FLAG_NOFLAG = 0,
  DMG_FLAG_WALK,
  DMG_FLAG_INQUEUE,
  DMG_FLAG_INCAV,
};

typedef struct {
  double c[2];
  double h; /* Prescribed edge size at the vertex */
  int ref;
  int flag; /* Flag to tell il the point has been inserted in the triangulation */
  int tag;
  int tmp;
} DMG_Point;
typedef DMG_Point* DMG_pPoint;

typedef struct {
  int v[2];
  int tmp;
  int ref;
} DMG_Edge;
typedef DMG_Edge* DMG_pEdge;

typedef struct {
  int v[3];
  int ref;
  double qual;
  int flag; /* Flag to know if the triangle has been visited when searching for the cavity of a point */
} DMG_Tria;
typedef DMG_Tria* DMG_pTria;

typedef struct {
  int a, b;
  int adj1, adj2;
  int nxt;
} DMG_Hedge;

typedef struct {
  int ver;
  int dim;
  double min[2], max[2];
  double hmin, hmax;
  int np, na, nt; /* Number of entities */
  int npmax, namax, ntmax; /* Max number of entities (size of the arrays) */
  int npu, nau, ntu; /* Indices of the first unused entities */
  int npi; /* Used to keep the index of the first BB vertex */
  DMG_pPoint point;
  DMG_pEdge edge; /* Store the boundary edges (all those edges must exist in the mesh at the end of the triangulation)*/
  DMG_pTria tria;
  int *adja;
} DMG_Mesh;
typedef DMG_Mesh* DMG_pMesh;

/* Grid structure */
typedef struct {
  double h; /* Step size */
  double xmin, ymin;
  int nx, ny; /* Number of cells in the x and y directions. */
  int *ucell; /* Array of bool indicating if cell (i * nx + j) contains a vertex of the mesh or not */
} DMG_Grid;

/* One node of the queue (implemented as a singly linked list) */
typedef struct DMG_Qnode_s {
  int key;
  struct DMG_Qnode_s *next;
} DMG_Qnode;

/* Queue */
typedef struct {
  DMG_Qnode *front, *rear;
} DMG_Queue;

#endif /* TYPES_DMG_H */
