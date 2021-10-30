#ifndef TYPES_DMG_H
#define TYPES_DMG_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define DMG_FAILURE 1

#define DMG_SUCCESS 0

#define DMG_UNSET -1

#define DMG_EPSILON 1e-15

#define DMG_ALPHA_2D 3.464101615137755 /* 6. / sqrt(3.) */

#define MAX2(a,b) (((a) > (b)) ? (a) : (b))
#define MIN2(a,b) (((a) < (b)) ? (a) : (b))
#define ABS(a) (((a) > 0.) ? (a) : (-a))

typedef struct {
  double c[2];
  int ref;
} DMG_Point; typedef DMG_Point* DMG_pPoint;

typedef struct {
  int v[2];
  int ref;
} DMG_Edge; typedef DMG_Edge* DMG_pEdge;

typedef struct {
  int v[3];
  int edge[3];
  int ref;
  double qual;
} DMG_Tria; typedef DMG_Tria* DMG_pTria;

typedef struct {
  int a, b;
  int adj1, adj2;
  int nxt;
} Hedge;

typedef struct {
  int ver;
  int dim;
  int np, na, nt;
  DMG_Point *point;
  DMG_Edge *edge;
  DMG_Tria *tria;
  int *head, *neigh;
  int *adja;
  Hedge *htab;
} DMG_Mesh; typedef DMG_Mesh* DMG_pMesh;

#endif /* TYPES_DMG_H */
