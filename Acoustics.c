/*
Acoustics.c
By Paul Dickens, 2005
Acoustics library.
Refer to Acoustics.h for interface details.
*/
#include "Acoustics.h"
#include "Complex.h"
#include <gsl/gsl_sf_bessel.h>
#include <math.h>
double saturationVapourPressureWater(double T) {
  double C1, C2, C3, C4;
  C1 = 1.2811805e-5;
  C2 = 1.9509874e-2;
  C3 = 34.04926034;
  C4 = 6.3536311e3;
  return exp(C1 * pow(T, 2) - C2 * T + C3 - C4 / T);
}
double speedSound(double t, double p, double h, double x_c) {
  double f, T, p_sv, x_w;
  /* Calculate mole fraction of water vapour using equation in Cramer
  Appendix (p. 2515) */
  f = 1.00062 + 3.14e-8 * p + 5.6e-7 * pow(t, 2);
  T = t + 273.15;
  p_sv = saturationVapourPressureWater(T);
  x_w = h * f * p_sv / p;
  return speedSoundCramer(t, p, x_w, x_c);
}
double speedSoundCramer(double t, double p, double x_w, double x_c) {
  int i;
  double c = 0;
  double a[] = {331.5024,  0.603055,  -0.000528, 51.471935,
                0.1495874, -0.000782, -1.82e-7,  3.73e-8,
                -2.93e-10, -85.20931, -0.228525, 5.91e-5,
                -2.835149, -2.15e-13, 29.179762, 0.000486};
  double coeff[] = {1.0,
                    t,
                    pow(t, 2),
                    x_w,
                    t * x_w,
                    pow(t, 2) * x_w,
                    p,
                    t * p,
                    pow(t, 2) * p,
                    x_c,
                    t * x_c,
                    pow(t, 2) * x_c,
                    pow(x_w, 2),
                    pow(p, 2),
                    pow(x_c, 2),
                    x_w * p * x_c};
  for (i = 0; i < 16; i++)
    c += a[i] * coeff[i];
  return c;
}
double densityAirIdeal(double t, double p, double h, double x_c) {
  /* temperature in Kelvin */
  double T = 273.15 + t;
  /* gas constant for dry air in J/kg.K */
  double R_a = 287.05;
  /* gas constant for water vapour in J/kg.K */
  double R_w = 461.5;
  /* gas constant for carbon dioxide in J/kg.K */
  double R_c = 189;
  /* partial pressures of dry air, water vapour and CO2 */
  double p_a, p_w, p_c;
  p_w = h * saturationVapourPressureWater(T);
  p_c = x_c * p;
  p_a = p - p_w - p_c;
  return p_a / (R_a * T) + p_w / (R_w * T) + p_c / (R_c * T);
}
double densityAirGiacomo(double t, double p, double h, double x_c) {
  double T = 273.15 + t;
  double R = 8.314472;
  double M_w = 18.015e-3;
  double M_a, f, p_sv, x_w, Z;
  M_a = (28.9635 + 12.011 * (x_c - 0.0004)) * 1e-3;
  /* Calculate mole fraction of water vapour using equation in Cramer
  Appendix (p. 2515) */
  f = 1.00062 + 3.14e-8 * p + 5.6e-7 * pow(t, 2);
  p_sv = saturationVapourPressureWater(T);
  x_w = h * f * p_sv / p;
  Z = compressibilityAir(t, p, x_w);
  return p * M_a * (1 - x_w * (1 - M_w / M_a)) / (Z * R * T);
}
double compressibilityAir(double t, double p, double x_w) {
  double T = 273.15 + t;
  double a_0, a_1, a_2, b_0, b_1, c_0, c_1, d, e;
  a_0 = 1.62419e-6;
  a_1 = -2.8969e-8;
  a_2 = 1.0880e-10;
  b_0 = 5.757e-6;
  b_1 = -2.589e-8;
  c_0 = 1.9297e-4;
  c_1 = -2.285e-6;
  d = 1.73e-11;
  e = -1.034e-8;
  return 1 -
         p *
             (a_0 + a_1 * t + a_2 * pow(t, 2) + (b_0 + b_1 * t) * x_w +
              (c_0 + c_1 * t) * pow(x_w, 2)) /
             T +
         pow(p, 2) * (d + e * pow(x_w, 2)) / pow(T, 2);
}
complex waveNum(double f, double c, double a, double alphacorrection) {
  /* implement wave number equation */
  complex z;
  double w = 2 * M_PI * f;
  z.Re = w / phaseVel(f, c, a);
  z.Im = (-1) * attenCoeff(f, a, alphacorrection);
  return z;
}
double phaseVel(double f, double c, double a) {
  /* implement phase velocity equation */
  return c * (1.0 - (1.65e-3 / (a * sqrt(f))));
}
double attenCoeff(double f, double a, double alphacorrection) {
  /* implement attenuation coefficient equation */
  return alphacorrection * (3.0e-5 * pow(f, 0.5)) / a;
}
complex charZ(double c, double rho, double a) {
  /* implement characteristic impedance equation */
  double Zo = rho * c / (M_PI * a * a);
  complex z = {Zo, 0};
  return z;
}
complex radiationZ(double f, double c, double rho, double a, double flange) {
  complex Z0, Z_u, Z_f, Z;
  complex d_u, d_f, d;
  double b, a_on_b;
  double k = (2 * M_PI * f) / c, ka = k * a;
  double modR_edge, phaseR_edge;
  complex R_norefl, R_edge, R;
  if (flange < 0.0)
    return inf;
  if (flange == 0.0)
    return unflangedZ(f, c, rho, a);
  /* calculate radiationZ for unflanged and flanged pipe */
  Z_u = unflangedZ(f, c, rho, a);
  Z_f = flangedZ(f, c, rho, a);
  /* calculate the characteristic impedance */
  Z0 = charZ(c, rho, a);
  /* calculate complex end corrections for unflanged and flanged
  pipe */
  d_u = divz(arctanz(divz(Z_u, multz(j, Z0))), real(k));
  d_f = divz(arctanz(divz(Z_f, multz(j, Z0))), real(k));
  b = a * (1 + flange);
  a_on_b = a / b;
  /* calculate the length correction */
  d = addz(addz(d_f, multz(real(a_on_b), subz(d_u, d_f))),
           real(0.057 * a_on_b * (1 - pow(a_on_b, 5)) * a));
  /* calculate the reflection coefficient (42) */
  R_norefl = multz(real(-1), expz(multz(imaginary(-2 * k), d)));
  modR_edge =
      -0.43 * (b - a) * a / pow(b, 2) * pow(sin(k * b / (1.85 - a_on_b)), 2);
  phaseR_edge = -k * b * (1 + a_on_b * (2.3 - a_on_b - 0.3 * pow(ka, 2)));
  R_edge = multz(real(modR_edge), expjz(real(phaseR_edge)));
  R = addz(R_norefl, R_edge);
  /* calculate and return the impedance */
  Z = multz(Z0, divz(addz(one, R), subz(one, R)));
  return Z;
}
complex unflangedZ(double f, double c, double rho, double a) {
  double k = (2 * M_PI * f) / c;
  double ka = k * a;
  complex d;
  double modR;
  complex Z0 = charZ(c, rho, a), Z;
  /* define the end correction for the low frequency limit */
  d.Re = 0.6133 * a;
  /* determine the frequency-dependent end correction (14b) */
  d.Re = d.Re * ((1 + 0.044 * pow(ka, 2)) / (1 + 0.19 * pow(ka, 2)) -
                 0.02 * pow(sin(2 * ka), 2));
  /* determine the modulus of the reflection coefficient (14c) */
  modR = (1 + 0.2 * ka - 0.084 * pow(ka, 2)) /
         (1 + 0.2 * ka + (0.5 - 0.084) * pow(ka, 2));
  /* calculate the imaginary part of the end correction */
  d.Im = log(modR) / (2 * k);
  /* calculate the impedance (9) */
  Z = multz(j, multz(Z0, tanz(multz(real(k), d))));
  return Z;
}
complex flangedZ(double f, double c, double rho, double a) {
  double k = (2 * M_PI * f) / c;
  double ka = k * a;
  complex d;
  double modR;
  complex Z0 = charZ(c, rho, a), Z;
  /* define the end correction for the low frequency limit */
  d.Re = 0.8216 * a;
  /* determine the frequency-dependent end correction (15a) */
  d.Re = d.Re / (1 + pow(0.77 * ka, 2) / (1 + 0.77 * ka));
  /* determine the modulus of the reflection coefficient (15b) */
  modR = (1 + 0.323 * ka - 0.077 * pow(ka, 2)) /
         (1 + 0.323 * ka + (1 - 0.077) * pow(ka, 2));
  /* calculate the imaginary part of the end correction */
  d.Im = log(modR) / (2 * k);
  /* calculate the impedance (9) */
  Z = multz(j, multz(Z0, tanz(multz(real(k), d))));
  return Z;
}
TransferMatrix tubeMatrix(double f, double c, double rho, double L, double a,
                          double alphacorrection) {
  complex Zo, jkL, A, B, C, D;
  /* check for zero length segment */
  if (L == 0.0)
    return identitym();
  Zo = charZ(c, rho, a);
  jkL = multz(j, multz(waveNum(f, c, a, alphacorrection), real(L)));
  A = coshz(jkL);
  B = multz(Zo, sinhz(jkL));
  C = divz(sinhz(jkL), Zo);
  D = A;
  return createTransferMatrix(A, B, C, D);
  ;
}
TransferMatrix coneMatrix(double f, double c, double rho, double L, double a1,
                          double a2, double alphacorrection) {
  complex k, kL, kx1, kx2, theta1, theta2, sintheta1, sintheta2;
  complex A, B, C, D;
  double S1, S2, x1, x2, a;
  double rhoc = rho * c;
  /* check for zero length segment */
  if (L == 0.0)
    return identitym();
  /* calculate apex distance for both ends of conical section
  based on similar triangles */
  x1 = L / (a2 / a1 - 1.0);
  x2 = x1 + L;
  a2 = a1 * (1.0 + L / x1);
  /* calculate areas of each end based on given radii */
  S1 = M_PI * a1 * a1;
  S2 = M_PI * a2 * a2;
  /* use geometric mean of radii for attenuation purposes */
  a = sqrt(a1 * a2);
  /* implement cone impedance calculation */
  k = waveNum(f, c, a, alphacorrection);
  kL = multz(k, real(L));
  kx1 = multz(k, real(x1));
  kx2 = multz(k, real(x2));
  theta1 = arctanz(kx1);
  theta2 = arctanz(kx2);
  sintheta1 = sinz(theta1);
  sintheta2 = sinz(theta2);
  A = multz(real(-1), divz(sinz(subz(kL, theta2)), sintheta2));
  B = multz(j, multz(real(rhoc / S2), sinz(kL)));
  C = multz(imaginary(S1 / rhoc), divz(sinz(addz(kL, subz(theta1, theta2))),
                                       multz(sintheta1, sintheta2)));
  D = multz(real(S1 / S2), divz(sinz(addz(kL, theta1)), sintheta1));
  return createTransferMatrix(A, B, C, D);
}
TransferMatrix discontinuityMatrix(double f, double c, double rho, double a1,
                                   double a2) {
  TransferMatrix m = identitym();
  int n, N = 100;
  double F0n, gamma_n;
  double x = a1 / a2;
  double omega = 2 * M_PI * f;
  double S2 = M_PI * pow(a2, 2);
  complex k_squared, k, Zchar, corr = zero;
  for (n = 1; n <= N; n++) {
    gamma_n = gsl_sf_bessel_zero_J1(n);
    F0n = 2 * gsl_sf_bessel_J1(x * gamma_n) /
          (x * gamma_n * gsl_sf_bessel_J0(gamma_n));
    k_squared = real(pow(omega / c, 2) - pow(gamma_n / a2, 2));
    k = multz(real(-1), sqrtz(k_squared));
    Zchar = divz(real(omega * rho / S2), k);
    corr = addz(corr, multz(Zchar, real(pow(F0n, 2))));
  }
  m->B = corr;
  return m;
}
