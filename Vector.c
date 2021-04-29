/*
Vector.c
By Andrew Botros, 2001-2003
Vector.c is a Vector ADT.
Refer to Vector.h for interface details.
*/
#include "Vector.h"
#include <stdio.h>
#include <stdlib.h>
Vector createVector(void) {
  /* allocate memory for *Vector */
  Vector v = (Vector)malloc(sizeof(*v));
  /* initialise num to 0, pointers to NULL */
  v->num = 0;
  v->head = NULL;
  v->tail = NULL;
  return v;
}
void addElement(Vector v, void *object) {
  /* if the Vector is empty, head and tail point to new object */
  if (v->tail == NULL) {
    v->tail = (Node)malloc(sizeof(*(v->tail)));
    v->tail->object = object;
    v->tail->next = NULL;
    v->head = v->tail;
  }
  /* else, add to end of Vector and update tail */
  else {
    v->tail->next = (Node)malloc(sizeof(*(v->tail->next)));
    v->tail->next->object = object;
    v->tail->next->next = NULL;
    v->tail = v->tail->next;
  }
  /* increment size */
  v->num++;
  return;
}
void insertAt(Vector v, void *object, int index) {
  int i;
  Node cur;
  Node prev;
  /* if Vector is empty and insert at 0, head and tail point to new
  object */
  if ((v->num == 0) && (index == 0)) {
    v->tail = (Node)malloc(sizeof(*(v->tail)));
    v->tail->object = object;
    v->tail->next = NULL;
    v->head = v->tail;
  } else {
    /* if inserted at beginning, update head */
    if (index == 0) {
      cur = v->head;
      v->head = (Node)malloc(sizeof(*(v->head)));
      v->head->object = object;
      v->head->next = cur;
    } else {
      /* if inserted at end, update tail */
      if (index == v->num) {
        cur = v->tail;
        v->tail = (Node)malloc(sizeof(*(v->tail)));
        v->tail->object = object;
        v->tail->next = NULL;
        cur->next = v->tail;
      }
      /* otherwise, find position in Vector and update appropriately
       */
      else {
        cur = v->head->next;
        prev = v->head;
        for (i = 1; i < index; i++) {
          cur = cur->next;
          prev = prev->next;
        }
        prev->next = (Node)malloc(sizeof(*(prev->next)));
        prev->next->object = object;
        prev->next->next = cur;
      }
    }
  }
  /* increment size */
  v->num++;
  return;
}
void setAt(Vector v, void *object, int index) {
  int i;
  Node cur;
  cur = v->head;
  /* find indexed element */
  for (i = 0; i < index; i++) {
    cur = cur->next;
  }
  /* update indexed element with given object */
  cur->object = object;
  return;
}
void popFront(Vector v) {
  Node second_element;
  /* do nothing if empty vector */
  if (sizeVector(v) == 0)
    return;
  /* if only 1 element, free head and reinitialise Vector */
  if (sizeVector(v) == 1) {
    free(v->head);
    v->head = NULL;
    v->tail = NULL;
    v->num--;
    return;
  }
  /* otherwise remove free head and update head pointer to second
  element */
  if (sizeVector(v) > 1) {
    second_element = v->head->next;
    free(v->head);
    v->head = second_element;
    v->num--;
    return;
  }
}
void popBack(Vector v) {
  Node cur;
  /* do nothing if empty vector */
  if (sizeVector(v) == 0)
    return;
  /* if only 1 element, free tail and reinitialise Vector */
  if (sizeVector(v) == 1) {
    free(v->tail);
    v->head = NULL;
    v->tail = NULL;
    v->num--;
    return;
  }
  /* otherwise remove free tail and update tail pointer to second last
  element */
  if (sizeVector(v) > 1) {
    /* advance cur to second last element */
    for (cur = v->head; cur->next != v->tail; cur = cur->next)
      ;
    free(v->tail);
    v->tail = cur;
    v->num--;
    return;
  }
}
void *elementAt(Vector v, int index) {
  int i;
  Node cur;
  cur = v->head;
  /* find indexed element */
  for (i = 0; i < index; i++) {
    cur = cur->next;
  }
  /* return found element */
  return cur->object;
}
int sizeVector(Vector v) { return v->num; }
