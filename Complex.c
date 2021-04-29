/*
Complex.c
By Andrew Botros, 2000-2004
Hyperbolic trig fns and expz added by Paul Dickens, 2005
Complex arithmetic library.
Refer to Complex.h for interface details.
Refer to the following for algebraic expressions:
Brown, J.W., Churchill, R.W., 1996.
Complex Variables and Applications.
6th ed. McGraw-Hill, New York,
*/
#include "Complex.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
/* zero, one, j and inf defined for convenience */
complex zero = {0.0, 0.0};
complex j = {0.0, 1.0};
complex one = {1.0, 0.0};
complex inf = {DBL_MAX, 0.0};
double modz(complex z) { return sqrt(z.Re * z.Re + z.Im * z.Im); }
double argz(complex z) { return atan2(z.Im, z.Re); }
complex real(double x) {
  complex z = {x, 0.0};
  return z;
}
complex imaginary(double y) {
  complex z = {0.0, y};
  return z;
}
int equalz(complex z1, complex z2) {
  return (z1.Re == z2.Re) && (z1.Im == z2.Im);
}
complex addz(complex z1, complex z2) {
  complex z;
  z.Re = z1.Re + z2.Re;
  z.Im = z1.Im + z2.Im;
  return z;
}
complex subz(complex z1, complex z2) {
  complex z;
  z.Re = z1.Re - z2.Re;
  z.Im = z1.Im - z2.Im;
  return z;
}
complex multz(complex z1, complex z2) {
  complex z;
  z.Re = z1.Re * z2.Re - z1.Im * z2.Im;
  z.Im = z1.Re * z2.Im + z1.Im * z2.Re;
  return z;
}
complex divz(complex z1, complex z2) {
  if (equalz(z2, zero))
    return inf;
  complex z;
  z.Re = (z1.Re * z2.Re + z1.Im * z2.Im) / (z2.Re * z2.Re + z2.Im * z2.Im);
  z.Im = (z1.Im * z2.Re - z1.Re * z2.Im) / (z2.Re * z2.Re + z2.Im * z2.Im);
  return z;
}
complex expz(complex z1) {
  complex z;
  z.Re = exp(z1.Re) * cos(z1.Im);
  z.Im = exp(z1.Re) * sin(z1.Im);
  return z;
}
complex expjz(complex z1) { return expz(multz(j, z1)); }
complex coshz(complex z1) {
  complex minus_one = {-1.0, 0.0};
  complex two = {2.0, 0.0};
  complex minus_z1 = multz(minus_one, z1);
  return divz(addz(expz(z1), expz(minus_z1)), two);
}
complex sinhz(complex z1) {
  complex minus_one = {-1.0, 0.0};
  complex two = {2.0, 0.0};
  complex minus_z1 = multz(minus_one, z1);
  return divz(subz(expz(z1), expz(minus_z1)), two);
}
complex cosz(complex z1) { return coshz(multz(j, z1)); }
complex sinz(complex z1) { return divz(sinhz(multz(j, z1)), j); }
complex tanz(complex z1) { return divz(sinz(z1), cosz(z1)); }
complex cotz(complex z1) { return divz(cosz(z1), sinz(z1)); }
complex logz(complex z1) {
  complex result;
  result.Re = log(modz(z1));
  result.Im = argz(z1);
  return result;
}
complex arctanz(complex z1) {
  complex two = {2.0, 0.0};
  return multz(divz(j, two), logz(divz(addz(j, z1), subz(j, z1))));
}
complex parallel(complex z1, complex z2) {
  return divz(multz(z1, z2), addz(z1, z2));
}
complex sqrtz(complex z1) {
  double A = modz(z1);
  double theta = argz(z1);
  A = sqrt(A);
  theta = theta / 2;
  return multz(real(A), expjz(real(theta)));
}
void printComplex(complex z) { printf("%e%+ei", z.Re, z.Im); }
