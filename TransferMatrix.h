/*
TransferMatrix.h
By Paul Dickens, 2005
Complex transfer matrix library.
*/
#ifndef TRANSFERMATRIX_H_PROTECTOR
#define TRANSFERMATRIX_H_PROTECTOR
#include "Complex.h"
/* TransferMatrix: { A, B, C, D } */
typedef struct transferMatrix_str {
  complex A;
  complex B;
  complex C;
  complex D;
} * TransferMatrix;
TransferMatrix createTransferMatrix(complex A, complex B, complex C, complex D);
/*
Creates a new TransferMatrix given the complex elements
Returns:
The matrix (A, B, C, D).
*/
TransferMatrix identitym();
/*
Returns:
The identity matrix (1, 0, 0, 1).
*/
void lmultm(TransferMatrix m, TransferMatrix mult);
/*
Left-multiplies matrix m with matrix mult.
*/
void rmultm(TransferMatrix m, TransferMatrix mult);
/*
Right-multiplies matrix m with matrix mult.
*/
void invertm(TransferMatrix m);
/*
Inverts matrix m.
*/
complex calcZin(TransferMatrix m, complex Zload);
/*
Calculates the input impedance given TransferMatrix m and load
Zload.
*/
#endif
