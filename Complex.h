/*
Complex.h
By Andrew Botros, 2000-2004
Hyperbolic trig fns and expz added by Paul Dickens, 2005
Complex arithmetic library.
*/
#ifndef COMPLEX_H_PROTECTOR
#define COMPLEX_H_PROTECTOR
/* complex: { Re(z), Im(z) } */
typedef struct complex_str {
  double Re;
  double Im;
} complex;
/* externally defined complex numbers zero, one, j and inf for
convenience */
extern complex zero;
extern complex one;
extern complex j;
extern complex inf;
double modz(complex z);
/*
Parameters:
z: given complex number.
Returns:
|z| ... Modulus of z
*/
double argz(complex z);
/*
Parameters:
z: given complex number.
Returns:
arg z ... Principal arg of z
*/
int equalz(complex z1, complex z2);
/*
Parameters:
z1: given complex number.
z2: given complex number.
Returns:
1 if z1 equals z2, 0 otherwise.
*/
complex real(double x);
/*
Parameters:
x: given double.
Returns:
A complex struct z with Re z = x and Im z = 0
*/
complex imaginary(double y);
/*
Parameters:
y: given double
Returns:
A complex struct z with Re z = 0 and Im z = y
*/
complex addz(complex z1, complex z2);
/*
Parameters:
z1: given complex number.
z2: given complex number.
Returns:
z1 + z2 ... The sum of z1 and z2
*/
complex subz(complex z1, complex z2);
/*
Parameters:
z1: given complex number.
z2: given complex number.
Returns:
z1 - z2 ... z2 subtracted from z1
*/
complex multz(complex z1, complex z2);
/*
Parameters:
z1: given complex number.
z2: given complex number.
Returns:
z1 x z2 ... The product of z1 and z2
*/
complex divz(complex z1, complex z2);
/*
Parameters:
z1: given complex number.
z2: given complex number.
Returns:
z1 / z2 ... z1 divided by z2
*/
complex expz(complex z1);
/*
Parameters:
z1: given complex number.
Returns:
exp(z1) ... The complex exponential of z1
*/
complex expjz(complex z1);
/*
Parameters:
z1: given complex number.
Returns:
exp(j x z1) ... The complex exponential of (j x z1)
*/
complex coshz(complex z1);
/*
Parameters:
z1: given complex number.
Returns:
cosh z1 ... The complex hyperbolic cosine of z1
*/
complex sinhz(complex z1);
/*
Parameters:
z1: given complex number.
Returns:
sinh z1 ... The complex hyperbolic sine of z1
*/
complex cosz(complex z1);
/*
Parameters:
z1: given complex number.
Returns:
cos z1 ... The complex cosine of z1
*/
complex sinz(complex z1);
/*
Parameters:
z1: given complex number.
Returns:
sin z1 ... The complex sine of z1
*/
complex tanz(complex z1);
/*
Parameters:
z1: given complex number.
Returns:
tan z1 ... The complex tan of z1
*/
complex cotz(complex z1);
/*
Parameters:
z1: given complex number.
Returns:
cot z1 ... The complex cotan of z1
*/
complex logz(complex z1);
/*
Parameters:
z1: given complex number.
Returns:
log z1 ... The complex principal logarithm of z1
*/
complex arctanz(complex z1);
/*
Parameters:
z1: given complex number.
Returns:
arctan z1 ... The complex inverse tan of z1
*/
complex parallel(complex z1, complex z2);
/*
Parameters:
z1: given complex number.
z2: given complex number.
Returns:
The complex representation of z1 || z2 (in the electrical sense).
*/
complex sqrtz(complex z1);
/*
Parameters:
z1: given complex number.
Returns:
The square root of z1.
*/
void printComplex(complex z);
/*
Prints a complex struct to stdout.
Parameters:
z: given complex number.
*/
#endif
