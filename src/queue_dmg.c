#include "dmg.h"

DMG_Queue* DMG_createQueue() {
  DMG_Queue* q = (DMG_Queue*)malloc(sizeof(DMG_Queue));

  if (q == NULL) return NULL;

  q->front = q->rear = NULL;

  return q;
}


void DMG_freeQueue(DMG_Queue *q) {

  if (q == NULL) return;

  while(!DMG_qIsEmpty(q)) {
    DMG_deQueue(q);
  }

  free(q);
  q = NULL;
}

DMG_Qnode* DMG_newNode(int k) {
  DMG_Qnode* node = (DMG_Qnode*)malloc(sizeof(DMG_Qnode));

  if (node == NULL) return NULL;

  node->key = k;
  node->next = NULL;

  return node;
}

int DMG_enQueue(DMG_Queue *q, int k) {
  DMG_Qnode *node = DMG_newNode(k);

  if (q->rear == NULL) {
    q->front = q->rear = node;
    return k;
  }

  q->rear->next = node;
  q->rear = node;
  return k;
}

int DMG_deQueue(DMG_Queue *q) {
  int key;

  if (q->front == NULL) {
    return DMG_UNSET;
  }

  DMG_Qnode *node = q->front;

  q->front = q->front->next;

  if (q->front == NULL) {
    q->rear = NULL;
  }

  key = node->key;
  free(node);

  return key;
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

inline int DMG_qIsEmpty(DMG_Queue *q) {
  return (q->front == NULL) ? 1 : 0;
}
