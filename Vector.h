/*
Vector.h
By Andrew Botros, 2001-2003
Vector.c is a Vector ADT.
*/
#ifndef VECTOR_H_PROTECTOR
#define VECTOR_H_PROTECTOR
/* Node: { pointer to data struct, pointer to next Node } */
typedef struct Node_str {
  void *object;
  struct Node_str *next;
} * Node;
/* Vector:
{ size count, pointer to head Node, pointer to tail Node } */
typedef struct Root_str {
  int num;
  Node head;
  Node tail;
} * Vector;
Vector createVector(void);
/*
Initialises a vector with num = 0, head = NULL, tail = NULL.
Returns:
A Vector if successful, NULL otherwise.
*/
void addElement(Vector v, void *object);
/*
Adds a data structure pointer to the end of the given vector.
Parameters:
v: the Vector to add to.
object: a pointer to the data structure to be added.
*/
void insertAt(Vector v, void *object, int index);
/*
Inserts a data structure pointer within the given vector at the
given position.
All following pointers in the Vector are shifted right by index +1.
Parameters:
v: the Vector to add to.
object: a pointer to the data structure to be added.
index: the position of the inserted pointer (head is index 0).
NOTE: no bounds checking is performed.
*/
void setAt(Vector v, void *object, int index);
/*
Sets the data structure pointer within the given vector at the given
position.
Parameters:
v: the Vector to be updated.
object: a pointer to the data structure to be set.
index: the position of the pointer to be altered (head is index
0).
NOTE: no bounds checking is performed.
*/
void popFront(Vector v);
/*
Removes the Node at the head of the Vector (index 0).
If Vector is empty, nothing is done.
All pointers in the Vector are shifted left by index -1.
Parameters:
v: the Vector to be updated.
*/
void popBack(Vector v);
/*
Removes the Node at the tail of the Vector.
If Vector is empty, nothing is done.
Parameters:
v: the Vector to be updated.
*/
void *elementAt(Vector v, int index);
/*
Returns the data structure pointer within the given vector at the
given index.
Parameters:
v: the Vector to retrieve from.
index: the position of the pointer requested (head is index 0).
Returns:
The stored void* pointer within the Vector.
NOTE: no bounds checking is performed.
*/
int sizeVector(Vector v);
/*
Returns the current number of data structure pointers stored within
the given Vector.
Parameters:
v: the given Vector.
Returns:
The number of stored elements.
*/
#endif
