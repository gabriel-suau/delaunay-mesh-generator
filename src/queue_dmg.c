#include "dmg.h"

DMG_Queue* DMG_createQueue() {
  DMG_Queue* q = (DMG_Queue*)malloc(sizeof(DMG_Queue));

  if (!q) return NULL;

  q->front = q->rear = NULL;

  return q;
}

DMG_Qnode* DMG_newNode(int k) {
  DMG_Qnode* node = (DMG_Qnode*)malloc(sizeof(DMG_Qnode));

  if (!node) return NULL;

  node->key = k;
  node->next = NULL;

  return node;
}

void DMG_enQueue(DMG_Queue *q, int k) {
  DMG_Qnode *node = DMG_newNode(k);

  if (q->rear == NULL) {
    q->front = q->rear = node;
    return;
  }

  q->rear->next = node;
  q->rear = node;
}

void DMG_deQueue(DMG_Queue *q) {

  if (q->front == NULL) {
    return;
  }

  DMG_Qnode *node = q->front;

  q->front = q->front->next;

  if (q->front == NULL) {
    q->rear = NULL;
  }

  free(node);
}

inline int DMG_qFront(DMG_Queue* q) {
  if (q->front == NULL)
    return DMG_UNSET;
  else
    return q->front->key;
}

inline int DMG_qRear(DMG_Queue* q) {
  if (q->rear == NULL)
    return DMG_UNSET;
  else
    return q->rear->key;
}
