/*
Map.h
By Paul Dickens, 2005
Map.c is a Linked List implementation of a Map (associative array).
*/
#ifndef MAP_H_PROTECTOR
#define MAP_H_PROTECTOR
#include "Vector.h"
/* Pair: { double key, pointer to value } */
typedef struct Pair_str {
  double key;
  void *value;
} * Pair;
/* Map: { a Vector of mappings. } */
typedef Vector Map;
Map createMap(void);
/*
Initialises a map with no mappings.
Returns:
A Map if successful, NULL otherwise.
*/
void put(Map m, double key, void *value);
/*
Assigns the given value to the given key. If the map already
contains the key, then its value is reset.
Parameters:
m: the Map to put new mapping in.
key: a double key.
value: a pointer to the value.
*/
void *get(Map m, double key);
/*
Returns the data structure pointer within the given map mapped to
the given key.
Parameters:
m: the Map to retrieve from.
key: the key to get the mapping of.
Returns:
The stored void* pointer within the Map.
NULL if the Map does not contain the given key.
*/
int containsKey(Map m, double key);
/*
Returns true if the given Map contains the given key.
Parameters:
m: the Map to check the keys of.
key: the key to check.
Returns:
1 if the Map contains the given key.
0 otherwise.
*/
void clear(Map m);
/*
Removes all mappings from the given Map.
Parameters:
m: the Map to clear.
*/
int sizeMap(Map m);
/*
Returns the number of mappings in the map.
Parameters:
m: the Map.
Returns:
The size of the map.
*/
#endif
