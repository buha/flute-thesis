/*
TransferMatrix.c
By Paul Dickens, 2005
Complex transfer matrix library.
Refer to TransferMatrix.h for interface details.
*/
#include "TransferMatrix.h"
#include <stdlib.h>
TransferMatrix createTransferMatrix(complex A, complex B, complex C,
                                    complex D) {
  TransferMatrix m = malloc(sizeof(*m));
  m->A = A;
  m->B = B;
  m->C = C;
  m->D = D;
  return m;
}
TransferMatrix identitym() {
  return createTransferMatrix(one, zero, zero, one);
}
void lmultm(TransferMatrix m, TransferMatrix mult) {
  complex A, B, C, D;
  A = addz(multz(mult->A, m->A), multz(mult->B, m->C));
  B = addz(multz(mult->A, m->B), multz(mult->B, m->D));
  C = addz(multz(mult->C, m->A), multz(mult->D, m->C));
  D = addz(multz(mult->C, m->B), multz(mult->D, m->D));
  m->A = A;
  m->B = B;
  m->C = C;
  m->D = D;
}
void rmultm(TransferMatrix m, TransferMatrix mult) {
  complex A, B, C, D;
  A = addz(multz(m->A, mult->A), multz(m->B, mult->C));
  B = addz(multz(m->A, mult->B), multz(m->B, mult->D));
  C = addz(multz(m->C, mult->A), multz(m->D, mult->C));
  D = addz(multz(m->C, mult->B), multz(m->D, mult->D));
  m->A = A;
  m->B = B;
  m->C = C;
  m->D = D;
}
complex calcZin(TransferMatrix m, complex Zload) {
  complex Zin, p1, p2, U1, U2;
  p2 = (equalz(Zload, inf)) ? one : divz(Zload, addz(Zload, one));
  U2 = (equalz(Zload, inf)) ? zero : divz(one, addz(Zload, one));
  p1 = addz(multz(m->A, p2), multz(m->B, U2));
  U1 = addz(multz(m->C, p2), multz(m->D, U2));
  Zin = divz(p1, U1);
  return Zin;
}
void invertm(TransferMatrix m) {
  complex det = subz(multz(m->A, m->D), multz(m->B, m->C));
  complex A = m->A;
  m->A = divz(m->D, det);
  m->B = divz(multz(real(-1.0), m->B), det);
  m->C = divz(multz(real(-1.0), m->C), det);
  m->D = divz(A, det);
}
