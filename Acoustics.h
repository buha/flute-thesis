/*
Acoustics.h
By Paul Dickens, 2005
Acoustics library.
*/
#ifndef ACOUSTICS_H_PROTECTOR
#define ACOUSTICS_H_PROTECTOR
#include "Complex.h"
#include "TransferMatrix.h"
double saturationVapourPressureWater(double T);
/*
Calculates the saturation vapour pressure of water.
Parameters:
T: the temperature in Kelvin.
Returns:
The vapour pressure in Pa.
*/
double speedSound(double t, double p, double h, double x_c);
/*
Calculates the speed of sound.
Based on Owen Cramer (1993) J. Acoust. Soc. Am. 93(5) p2510-2616;
formula at p. 2514
Parameters:
t: the temperature in Celsius (between 0 and 30 degrees C).
h: the relative humidity expressed as a fraction (between 0 and
1).
x_c: the mole fraction of carbon dioxide (between 0 and 0.01).
(note that the parameter ranges are not enforced)
(note that Cramer gives x_c = 0.000314 for air)
Returns:
The speed of sound in m/s.
*/
double speedSoundCramer(double t, double p, double x_w, double x_c);
/*
Calculates the speed of sound.
Based on Owen Cramer (1993) J. Acoust. Soc. Am. 93(5) p2510-2616;
formula at p2514.
Parameters:
t: the temperature in Celsius (between 0 and 30 degrees C).
p: the pressure in Pa (between 75 and 102 kPa).
x_w: the mole fraction of water vapour (between 0 and 0.06).
x_c: the mole fraction of carbon dioxide (between 0 and 0.01).
(note that the parameter ranges are not enforced)
Returns:
The speed of sound in m/s.
*/
double densityAirIdeal(double t, double p, double h, double x_c);
/*
Calculates the density of air using the ideal gas law.
For a more accurate treatment, see Giacomo (1982), Metrologia 18,
33--40.
Parameters:
t: the temperature in Celsius.
p: the pressure in Pa.
h: the relative humidity (between 0 and 1).
x_c: the mole fraction of carbon dioxide.
Returns:
The density of air in kg/m3.
*/
double densityAirGiacomo(double t, double p, double h, double x_c);
/*
Calculates the density of air.
Based on Giacomo (1982), Metrologia 18, 33--40.
Parameters:
t: the temperature in Celsius.
p: the pressure in Pa.
h: the relative humidity (between 0 and 1).
x_c: the mole fraction of carbon dioxide.
Returns:
The density of air in kg/m3.
*/
double compressibilityAir(double t, double p, double x_w);
/*
Calculates the compressibility of air.
Parameters:
t: the temperature in Celsius.
p: the pressure in Pa.
h: the relative humidity (between 0 and 1).
x_w: the mole fraction of water vapour.
Returns:
The compressibility of air.
*/
complex waveNum(double f, double c, double a, double alphacorrection);
/*
Calculates k, the complex wave number for a given segment which
includes wall losses. Refer to Fletcher and Rossing (1998).
Parameters:
f: the frequency.
c: the speed of sound
a: the radius of the tube in metres.
alphacorrection: the multiplicative attenuation coefficient
factor.
Returns:
The complex wave number k, including wall losses.
*/
double phaseVel(double f, double c, double a);
/*
Calculates the phase velocity of a given pipe, part of the wave
number.
Refer to Fletcher and Rossing (1998).
Parameters:
f: the frequency.
a: the radius of the tube in metres.
Returns:
The phase velocity.
*/
double attenCoeff(double f, double a, double alphacorrection);
/*
Calculates the attentuation coefficient (alpha) of a given segment,
part of the wave number. A multiplicative factor alphacorrection is
also applied. Refer to Fletcher and Rossing (1998).
Parameters:
f: the frequency.
a: the radius of the tube in metres.
alphacorrection: the multiplicative attenuation coefficient
factor.
Returns:
The attenuation coefficient.
*/
complex charZ(double c, double rho, double a);
/*
Calculates the characteristic impedance of a given cylindrical pipe.
Refer to Fletcher and Rossing (1998).
Parameters:
c: the speed of sound.
rho: the density of air.
a: the radius of the tube in metres.
Returns:
The complex characteristic impedance of the tube.
*/
complex radiationZ(double f, double c, double rho, double a, double flange);
/*
Calculates the radiation impedance of a termination.
Refer to Dalmont (2001) JSV 244, 505-34.
Parameters:
f: the frequency.
c: the speed of sound.
rho: the density of air.
a: the radius of the tube in metres.
flange: -1 for a stopped termination, otherwise ratio of annulus
thickness to a.
Returns:
The complex radiation impedance.
*/
complex unflangedZ(double f, double c, double rho, double a);
/*
Calculates the radiation impedance of an unflanged pipe.
Refer to Dalmont (2001) JSV 244, 505-34.
Parameters:
f: the frequency.
c: the speed of sound.
rho: the density of air.
a: the radius of the tube in metres.
Returns:
The complex radiation impedance.
*/
complex flangedZ(double f, double c, double rho, double a);
/*
Calculates the radiation impedance of a flanged pipe.
Refer to Dalmont (2001) JSV 244, 505-34.
Parameters:
f: the frequency.
c: the speed of sound.
rho: the density of air.
a: the radius of the tube in metres.
Returns:
The complex radiation impedance.
*/
TransferMatrix tubeMatrix(double f, double c, double rho, double L, double a,
                          double alphacorrection);
/*
Calculates the transfer matrix of a cylindrical tube.
The transfer matrix T relates pressure and flow at the input
to the pressure and flow at the output. Flow is positive
into the input and out of the output.
Parameters:
f: the frequency.
c: the speed of sound.
rho: the density of air.
L: the length of the tube in metres.
a: the radius of the tube in metres.
alphacorrection: the multiplicative attenuation coefficient
factor.
Returns:
The transfer matrix of the tube.
*/
TransferMatrix coneMatrix(double f, double c, double rho, double L, double a1,
                          double a2, double alphacorrection);
/*
Calculates the transfer matrix of a truncated cone.
The transfer matrix T relates pressure and flow at the input
to the pressure and flow at the output. Flow is positive
into the input and out of the output.
Parameters:
f: the frequency.
c: the speed of sound.
rho: the density of air.
L: the length of the pipe in metres.
a1: the radius of the pipe at input in metres.
a2: the radius of the pipe at output in metres.
alphacorrection: the multiplicative attenuation coefficient
factor.
Returns:
The transfer matrix of the pipe.
*/
TransferMatrix discontinuityMatrix(double f, double c, double rho, double a1,
                                   double a2);
/*
Calculates the transfer matrix at a discontinuity.
Based on Pagneux et al. (1996) J. Acoust. Soc. Am. 100 p2034-48;
Parameters:
f: the frequency.
c: the speed of sound.
rho: the density of air.
a1: the radius at the input side of the discontinuity.
a2: the radius at the output side of the discontinuity.
Returns:
The transfer matrix of the discontinuity.
*/
#endif
