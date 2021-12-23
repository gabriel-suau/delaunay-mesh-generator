#ifndef TYPES_DMG_H
#define TYPES_DMG_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include <stdint.h>

#define DMG_FAILURE 1

#define DMG_SUCCESS 0

#define DMG_UNSET -1

enum DMG_Flags {
  DMG_FLAG_NOFLAG = 0,
  DMG_FLAG_WALK,
  DMG_FLAG_INQUEUE,
  DMG_FLAG_INCAV,
};

typedef struct {
  double c[2];
  int ref;
  int flag; /* Flag to tell il the point has been inserted in the triangulation */
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
  int np, na, nt; /* Number of entities */
  int npmax, namax, ntmax; /* Max number of entities (size of the arrays) */
  int npu, nau, ntu; /* Indices of the first unused entities */
  DMG_pPoint point;
  DMG_pEdge edge; /* Store the boundary edges (all those edges must exist in the mesh at the end of the triangulation)*/
  DMG_pTria tria;
  int *adja;
} DMG_Mesh;
typedef DMG_Mesh* DMG_pMesh;

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
