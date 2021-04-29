/*
Map.c
By Paul Dickens, 2005
Map.c is a Linked List implementation of a Map (associative array).
Refer to Map.h for interface details.
*/
#include "Map.h"
#include <stdlib.h>
Map createMap(void) { return createVector(); }
void put(Map m, double key, void *value) {
  int i;
  Pair cur;
  Pair newPair;
  /* search for key */
  for (i = 0; i < sizeMap(m); i++) {
    cur = (Pair)elementAt(m, i);
    if ((cur->key) == key) {
      cur->value = value;
      return;
    }
  }
  /* if key not found, make new entry */
  newPair = malloc(sizeof(*newPair));
  newPair->key = key;
  newPair->value = value;
  addElement(m, newPair);
}
void *get(Map m, double key) {
  int i;
  Pair cur;
  /* search for key */
  for (i = 0; i < sizeMap(m); i++) {
    cur = (Pair)elementAt(m, i);
    if ((cur->key) == key)
      return cur->value;
  }
  /* if key not found, return NULL */
  return NULL;
}
int containsKey(Map m, double key) {
  int i;
  Pair cur;
  /* search for key */
  for (i = 0; i < sizeMap(m); i++) {
    cur = (Pair)elementAt(m, i);
    if ((cur->key) == key)
      return 1;
  }
  /* if key not found, return false */
  return 0;
}
void clear(Map m) {
  while (sizeMap(m) > 0)
    popFront(m);
}
int sizeMap(Map m) { return sizeVector(m); }
