/*
Woodwind.c
By Paul Dickens, 2005, 2006.
A woodwind instrument. See Woodwind.h for interface details.
*/
#include "Woodwind.h"
#include "Acoustics.h"
#include "Map.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define P_ATM 101325 // 1 atm pressure
BoreSegment createBoreSegment(double radius1, double radius2, double length) {
  BoreSegment s = (BoreSegment)malloc(sizeof(*s));
  s->radius1 = radius1;
  s->radius2 = radius2;
  s->length = length;
  s->c = 0.0;
  s->rho = 0.0;
  return s;
}
Hole createHole(double radius, double length, double boreRadius, Key key) {
  Hole h = (Hole)malloc(sizeof(*h));
  h->radius = radius;
  h->length = length;
  h->boreRadius = boreRadius;
  h->key = key;
  h->c = 0.0;
  h->rho = 0.0;
  return h;
}
Key createKey(double radius, double holeRadius, double height, double thickness,
              double wallThickness, double chimneyHeight) {
  Key k = (Key)malloc(sizeof(*k));
  k->radius = radius;
  k->holeRadius = holeRadius;
  k->height = height;
  k->thickness = thickness;
  k->wallThickness = wallThickness;
  k->chimneyHeight = chimneyHeight;
  return k;
}
EmbouchureHole createEmbouchureHole(double radiusin, double radiusout,
                                    double length, double boreRadius) {
  EmbouchureHole h = (EmbouchureHole)malloc(sizeof(*h));
  h->radiusin = radiusin;
  h->radiusout = radiusout;
  h->length = length;
  h->boreRadius = boreRadius;
  h->c = 0.0;
  h->rho = 0.0;
  return h;
}
Head createHead(EmbouchureHole embouchureHole, Vector upstreamBore,
                double upstreamFlange, Vector downstreamBore) {
  Head h = (Head)malloc(sizeof(*h));
  h->embouchureHole = embouchureHole;
  h->upstreamBore = upstreamBore;
  h->upstreamFlange = upstreamFlange;
  h->downstreamBore = downstreamBore;
  h->matrixMap = createMap();
  return h;
}
UnitCell createUnitCell(Hole hole, Vector bore) {
  UnitCell c = (UnitCell)malloc(sizeof(*c));
  c->hole = hole;
  c->bore = bore;
  c->openMatrixMap = createMap();
  c->closedMatrixMap = createMap();
  return c;
}
Woodwind createWoodwind(Head head, Vector cells, double flange) {
  Woodwind w = (Woodwind)malloc(sizeof(*w));
  w->head = head;
  w->cells = cells;
  w->flange = flange;
  return w;
}
void setAirProperties(Woodwind w, double t_0, double t_amb, double t_grad,
                      double humid, double x_CO2) {
  Head h = w->head;
  double temp, x;
  int segmentCount, cellCount;
  BoreSegment s;
  UnitCell cell;
  temp = t_0;
  if (w->head->embouchureHole != NULL) {
    /* set c and rho for the embouchure hole */
    h->embouchureHole->c = speedSound(temp, P_ATM, humid, x_CO2);
    h->embouchureHole->rho = densityAirGiacomo(temp, P_ATM, humid, x_CO2);
    x = 0;
    /* for each bore segment in upstream */
    for (segmentCount = 0; segmentCount < sizeVector(h->upstreamBore);
         segmentCount++) {
      s = (BoreSegment)elementAt(h->upstreamBore, segmentCount);
      temp = t_0 + t_grad * (x + s->length / 2);
      if (temp < t_amb)
        temp = t_amb;
      s->c = speedSound(temp, P_ATM, humid, x_CO2);
      s->rho = densityAirGiacomo(temp, P_ATM, humid, x_CO2);
      x += s->length;
    }
  }
  x = 0;
  /* for each bore segment in downstream */
  for (segmentCount = 0; segmentCount < sizeVector(h->downstreamBore);
       segmentCount++) {
    s = (BoreSegment)elementAt(h->downstreamBore, segmentCount);
    temp = t_0 + t_grad * (x + s->length / 2);
    if (temp < t_amb)
      temp = t_amb;
    s->c = speedSound(temp, P_ATM, humid, x_CO2);
    s->rho = densityAirGiacomo(temp, P_ATM, humid, x_CO2);
    x += s->length;
  }
  /* for each unit cell */
  for (cellCount = 0; cellCount < sizeVector(w->cells); cellCount++) {
    cell = (UnitCell)elementAt(w->cells, cellCount);
    temp = t_0 + t_grad * x;
    if (temp < t_amb)
      temp = t_amb;
    cell->hole->c = speedSound(temp, P_ATM, humid, x_CO2);
    cell->hole->rho = densityAirGiacomo(temp, P_ATM, humid, x_CO2);
    /* for each bore segment in unit cell */
    for (segmentCount = 0; segmentCount < sizeVector(cell->bore);
         segmentCount++) {
      s = (BoreSegment)elementAt(cell->bore, segmentCount);
      temp = t_0 + t_grad * (x + s->length / 2);
      if (temp < t_amb)
        temp = t_amb;
      s->c = speedSound(temp, P_ATM, humid, x_CO2);
      s->rho = densityAirGiacomo(temp, P_ATM, humid, x_CO2);
      x += s->length;
    }
  }
}
void discretiseWoodwind(Woodwind w, double maxLength) {
  int cellCount;
  UnitCell cell;
  discretiseBore(w->head->upstreamBore, maxLength);
  discretiseBore(w->head->downstreamBore, maxLength);
  for (cellCount = 0; cellCount < sizeVector(w->cells); cellCount++) {
    cell = (UnitCell)elementAt(w->cells, cellCount);
    discretiseBore(cell->bore, maxLength);
  }
}
void discretiseBore(Vector bore, double maxLength) {
  int segmentCount, newSegmentCount, numSegments;
  BoreSegment s, newSegment;
  double radius1, radius2, length;
  /* for each bore segment in bore */
  for (segmentCount = 0; segmentCount < sizeVector(bore); segmentCount++) {
    s = (BoreSegment)elementAt(bore, segmentCount);
    if (s->length <= maxLength)
      continue;
    numSegments = ceil(s->length / maxLength);
    radius1 = s->radius1;
    length = s->length / numSegments;
    for (newSegmentCount = 1; newSegmentCount <= numSegments;
         newSegmentCount++) {
      radius2 = (newSegmentCount * s->radius2 +
                 (numSegments - newSegmentCount) * s->radius1) /
                numSegments;
      newSegment = createBoreSegment(radius1, radius2, length);
      if (newSegmentCount == 1)
        setAt(bore, newSegment, segmentCount);
      else
        insertAt(bore, newSegment, ++segmentCount);
      radius1 = radius2;
    }
  }
}
int setFingering(Woodwind w, char *holestring) {
  int numholes = sizeVector(w->cells);
  int i;
  UnitCell cell;
  /* check for no holes */
  if ((holestring == NULL) || (numholes == 0)) {
    if ((holestring == NULL) && (numholes == 0))
      return 1;
    else
      return 0;
  }
  /* check for wrong number of holes entered */
  if (strlen(holestring) != numholes)
    return 0;
  /* set fingering array based on command line hole string */
  for (i = 0; i < numholes; i++) {
    cell = (UnitCell)elementAt(w->cells, i);
    if (holestring[i] == 'O')
      cell->hole->fingering = "OPEN";
    if (holestring[i] == 'X')
      cell->hole->fingering = "CLOSED";
    if ((holestring[i] != 'O') && (holestring[i] != 'X'))
      return 0;
  }
  return 1;
}
TransferMatrix boreSegmentMatrix(double f, BoreSegment s, double x) {
  TransferMatrix m;
  double length, radius1, radius2;
  radius1 = s->radius1;
  if (x >= s->length) {
    length = s->length;
    radius2 = s->radius2;
  } else {
    length = x;
    radius2 =
        (s->radius2 * length + s->radius1 * (s->length - length)) / (s->length);
  }
  m = (radius1 == radius2)
          ? tubeMatrix(f, s->c, s->rho, length, radius1, 1)
          : coneMatrix(f, s->c, s->rho, length, radius1, radius2, 1);
  return m;
}
TransferMatrix boreMatrix(double f, Vector bore, double x) {
  TransferMatrix m = identitym();
  int n = 0;
  BoreSegment s;
  while ((x > 0) && (n < sizeVector(bore))) {
    s = (BoreSegment)elementAt(bore, n);
    rmultm(m, boreSegmentMatrix(f, s, x));
    x -= s->length;
    n++;
  }
  return m;
}
TransferMatrix headMatrix(double f, Head h, double entryratio, double x) {
  TransferMatrix m, branchMatrix;
  complex branchZ, ZL;
  BoreSegment lastSegment;
  Map map = h->matrixMap;
  if ((x >= boreLength(h->downstreamBore)) && containsKey(map, f))
    m = (TransferMatrix)get(map, f);
  else {
    m = identitym();
    if (h->embouchureHole != NULL) {
      branchMatrix =
          boreMatrix(f, h->upstreamBore, boreLength(h->upstreamBore));
      lastSegment = (BoreSegment)elementAt(h->upstreamBore,
                                           sizeVector(h->upstreamBore) - 1);
      ZL = radiationZ(f, lastSegment->c, lastSegment->rho, lastSegment->radius2,
                      h->upstreamFlange);
      branchZ = calcZin(branchMatrix, ZL);
      rmultm(m, embouchureMatrix(f, h->embouchureHole, entryratio, branchZ));
    }
    if (x > 0)
      rmultm(m, boreMatrix(f, h->downstreamBore, x));
    if (x >= boreLength(h->downstreamBore)) {
      clear(map);
      put(map, f, m);
    }
  }
  return m;
}
complex faceZ(double f, Head head, int midi) {
  BoreSegment s = (BoreSegment)elementAt(head->downstreamBore, 0);
  double c;
  double rho;
  double entryradius = WW_EMB_RADIUS;
  double corr = 2.9370 * log(midi) - 11.6284;
  if (head->embouchureHole) {
    c = head->embouchureHole->c;
    rho = head->embouchureHole->rho;
  } else {
    c = s->c;
    rho = s->rho;
  }
  return multz(flangedZ(f, c, rho, entryradius), real(corr));
}
TransferMatrix unitCellMatrix(double f, UnitCell c, double x) {
  TransferMatrix m;
  Map map = (strcmp(c->hole->fingering, "OPEN") == 0) ? c->openMatrixMap
                                                      : c->closedMatrixMap;
  if ((x >= boreLength(c->bore)) && containsKey(map, f))
    m = (TransferMatrix)get(map, f);
  else {
    m = traverseHoleMatrix(f, c->hole);
    if ((x > 0) && (c->bore != NULL))
      rmultm(m, boreMatrix(f, c->bore, x));
    if (x >= boreLength(c->bore)) {
      clear(map);
      put(map, f, m);
    }
  }
  return m;
}
TransferMatrix woodwindMatrix(double f, Woodwind w, double entryratio,
                              double x) {
  TransferMatrix m = identitym();
  Head h;
  complex branchZ;
  UnitCell cell;
  int cellCount = 0;
  if (x >= 0) {
    rmultm(m, headMatrix(f, w->head, entryratio, x));
    x -= boreLength(w->head->downstreamBore);
    while (x > 0 && cellCount < sizeVector(w->cells)) {
      cell = (UnitCell)elementAt(w->cells, cellCount);
      rmultm(m, unitCellMatrix(f, cell, x));
      x -= boreLength(cell->bore);
      cellCount++;
    }
  } else {
    h = w->head;
    if (h->embouchureHole != NULL) {
      branchZ = woodwindDownstreamZ(f, w);
      rmultm(m, embouchureMatrix(f, h->embouchureHole, entryratio, branchZ));
    }
    rmultm(m, boreMatrix(f, h->upstreamBore, -x));
  }
  return m;
}
int getZ0_c(Woodwind w, double x, complex *Z0, double *c) {
  Vector bore;
  int n = 0, cellCount = 0;
  UnitCell cell;
  BoreSegment s;
  double radius;
  if (x == 0) {
    *Z0 = charZ(w->head->embouchureHole->c, w->head->embouchureHole->rho,
                w->head->embouchureHole->boreRadius);
    *c = w->head->embouchureHole->c;
    return 1;
  }
  if (x > 0) {
    /* find the bore or hole at the point x */
    bore = w->head->downstreamBore;
    /* if x is outside of the current bore, try the next one */
    while (x >= boreLength(bore) && cellCount < sizeVector(w->cells)) {
      /* subtract from x the length of the previous bore */
      x -= boreLength(bore);
      /* find the next bore */
      cell = (UnitCell)elementAt(w->cells, cellCount++);
      bore = cell->bore;
    }
  } else if (x < 0) {
    bore = w->head->upstreamBore;
    x = -x;
  }
  /* if we are at a hole */
  if (x == 0) {
    *Z0 = charZ(cell->hole->c, cell->hole->rho, cell->hole->boreRadius);
    *c = cell->hole->c;
    return 1;
  }
  /* otherwise, find the correct BoreSegment */
  else {
    s = (BoreSegment)elementAt(bore, n);
    while (x > s->length) {
      /* subtract from x the length of the previous segment */
      x -= s->length;
      /* get the next segment */
      s = (BoreSegment)elementAt(bore, ++n);
    }
  }
  /* if x is outside the BoreSegment, return 0 */
  if (x > s->length)
    return 0;
  /* otherwise do a linear interpolation */
  else {
    radius = (s->radius2 * x + s->radius1 * (s->length - x)) / (s->length);
  }
  *Z0 = charZ(s->c, s->rho, radius);
  *c = s->c;
  return 1;
}
double boreLength(Vector bore) {
  int n;
  BoreSegment s;
  double length = 0;
  for (n = 0; n < sizeVector(bore); n++) {
    s = (BoreSegment)elementAt(bore, n);
    length += s->length;
  }
  return length;
}
double woodwindLengthPos(Woodwind w) {
  int cellCount;
  UnitCell cell;
  double length = 0;
  length += boreLength(w->head->downstreamBore);
  for (cellCount = 0; cellCount < sizeVector(w->cells); cellCount++) {
    cell = (UnitCell)elementAt(w->cells, cellCount);
    length += boreLength(cell->bore);
  }
  return length;
}
double woodwindLengthNeg(Woodwind w) {
  return boreLength(w->head->upstreamBore);
}
complex impedance(double f, Woodwind w, double entryratio) {
  TransferMatrix matrix =
      woodwindMatrix(f, w, entryratio, woodwindLengthPos(w));
  return calcZin(matrix, woodwindLoadZ(f, w));
}
complex playedImpedance(double f, Woodwind w, int midi) {
  double entryradius = WW_EMB_RADIUS;
  complex Z = impedance(f, w, entryradius / woodwindEntryRadius(w));
  Z = addz(Z, faceZ(f, w->head, midi));
  return Z;
}
double woodwindEntryRadius(Woodwind w) {
  double a;
  BoreSegment s;
  if (w->head->embouchureHole != NULL)
    a = w->head->embouchureHole->radiusout;
  else {
    s = (BoreSegment)elementAt(w->head->downstreamBore, 0);
    a = s->radius1;
  }
  return a;
}
complex woodwindLoadZ(double f, Woodwind w) {
  Vector lastBore;
  UnitCell lastCell;
  BoreSegment s;
  if (sizeVector(w->cells) == 0)
    lastBore = w->head->downstreamBore;
  else {
    lastCell = (UnitCell)elementAt(w->cells, sizeVector(w->cells) - 1);
    lastBore = lastCell->bore;
  }
  s = (BoreSegment)elementAt(lastBore, sizeVector(lastBore) - 1);
  return radiationZ(f, s->c, s->rho, s->radius2, w->flange);
}
complex woodwindDownstreamZ(double f, Woodwind w) {
  TransferMatrix m;
  UnitCell cell;
  int cellCount;
  m = boreMatrix(f, w->head->downstreamBore,
                 boreLength(w->head->downstreamBore));
  for (cellCount = 0; cellCount < sizeVector(w->cells); cellCount++) {
    cell = (UnitCell)elementAt(w->cells, cellCount);
    rmultm(m, unitCellMatrix(f, cell, boreLength(cell->bore)));
  }
  return calcZin(m, woodwindLoadZ(f, w));
}
TransferMatrix traverseHoleMatrix(double f, Hole hole) {
  TransferMatrix m = identitym();
  complex Z_hole, Z_i, Z_a;
  /* calculate the input impedance to the hole */
  Z_hole = holeInputImpedance(f, hole);
  /* calculate the inner radiation impedance */
  Z_i = holeInnerRadiationImpedance(f, hole);
  /* calculate the series impedance */
  Z_a = holeSeriesImpedance(f, hole);
  /* assign the impedances to the correct matrix element */
  m->C = divz(one, addz(Z_i, Z_hole));
  m->B = Z_a;
  return m;
}
complex holeInputImpedance(double f, Hole hole) {
  TransferMatrix holeMatrix;
  double t;
  double a = hole->boreRadius;
  double b = hole->radius;
  complex Z_L;
  /* calculate the length t (inluding the matching length
  correction) */
  t = hole->length + matchingLengthCorrection(a, b);
  /* calculate the matrix (with losses) for the tube section
  comprising the hole and matching length */
  holeMatrix = tubeMatrix(f, hole->c, hole->rho, t, b, 1);
  /* calculate the load (radiation) impedance of the hole */
  if (strcmp(hole->fingering, "CLOSED") == 0)
    Z_L = (hole->key == NULL) ? closedFingerHoleLoadZ(f, hole)
                              : closedKeyedHoleLoadZ(f, hole);
  else
    Z_L = (hole->key == NULL) ? openFingerHoleLoadZ(f, hole)
                              : openKeyedHoleLoadZ(f, hole);
  return calcZin(holeMatrix, Z_L);
}
complex closedFingerHoleLoadZ(double f, Hole hole) {
  double a = hole->boreRadius;
  double b = hole->radius;
  double delta = b / a;
  double t_finger;
  double Z0 = charZ(hole->c, hole->rho, hole->radius).Re;
  double k = (2 * M_PI * f) / hole->c;
  t_finger = CORR_CLOSED_FINGER_HOLE_LENGTH * delta * b;
  return imaginary(-Z0 / tan(k * t_finger));
}
complex closedKeyedHoleLoadZ(double f, Hole hole) {
  double b = hole->radius;
  double t_keypad;
  double Z0 = charZ(hole->c, hole->rho, hole->radius).Re;
  double k = (2 * M_PI * f) / hole->c;
  t_keypad = CORR_CLOSED_KEYED_HOLE_LENGTH * b;
  if (t_keypad == 0)
    return inf;
  return imaginary(-Z0 / tan(k * t_keypad));
}
complex openFingerHoleLoadZ(double f, Hole hole) {
  double a, b, k;
  complex Z_flanged, Z0, d_flanged, d_cyl, Z;
  /* a is the radius of the hole */
  a = hole->radius;
  /* b is the radius of the (cylindrical) flange */
  b = hole->boreRadius + hole->length;
  /* calculate the wavenumber */
  k = (2 * M_PI * f) / hole->c;
  /* calculate the characteristic impedance */
  Z0 = charZ(hole->c, hole->rho, a);
  /* calculate the impedance of the (infinitely) flanged hole */
  Z_flanged = flangedZ(f, hole->c, hole->rho, a);
  /* calculate complex end corrections */
  d_flanged = divz(arctanz(divz(Z_flanged, multz(j, Z0))), real(k));
  d_cyl = subz(d_flanged, real(0.47 * a * pow(a / b, 0.8)));
  /* add empirical correction */
  d_cyl = addz(d_cyl, real(CORR_OPEN_FINGER_HOLE_LENGTH * a));
  Z = multz(j, multz(Z0, tanz(multz(real(k), d_cyl))));
  return Z;
}
complex openKeyedHoleLoadZ(double f, Hole hole) {
  double a, d, q, h, e, w;
  double d_corr, d_e_on_a, k;
  complex Z_circ, Z0, d_circ, d_disk, Z, R;
  Key key = hole->key;
  a = hole->radius;
  d = key->radius;
  q = key->holeRadius;
  h = key->height;
  e = key->thickness;
  w = key->wallThickness;
  /* hack for classical flutes since the keyed holes have no
  chimneys */
  if (key->chimneyHeight == 0)
    w = DBL_MAX;
  k = (2 * M_PI * f) / hole->c;
  /* calculate the characteristic impedance */
  Z0 = charZ(hole->c, hole->rho, a);
  /* Dalmont et al. (2001) Radiation impedance of tubes with different
  flanges: Numerical and experimental investigations, Journal of
  Sound and Vibration 244(3), 505--534, eqs. (48, 51, 52) */
  d_corr = a / (3.5 * pow(h / a, 0.8) * pow(h / a + 3 * w / a, -0.4) +
                30 * pow(h / d, 2.6));
  /* add empirical length correction */
  d_corr = d_corr + CORR_OPEN_KEYED_HOLE_LENGTH * a;
  if (q > 0) {
    d_e_on_a = 1.64 * a / q - 0.15 * a / d - 1.1 + e * a / (q * q);
    d_corr = d_corr / (1 + 5 * pow(d_e_on_a, -1.35) * pow(h / a, -0.2));
  }
  Z_circ = radiationZ(f, hole->c, hole->rho, a, w / a);
  /* calculate complex end corrections */
  d_circ = divz(arctanz(divz(Z_circ, multz(j, Z0))), real(k));
  d_disk = addz(d_circ, real(d_corr));
  Z = multz(j, multz(Z0, tanz(multz(real(k), d_disk))));
  /* add empirical resistance */
  R = multz(Z0, real(0.4 * pow(k * a, 2)));
  Z = addz(Z, R);
  return Z;
}
complex holeInnerRadiationImpedance(double f, Hole hole) {
  double t_i;
  double Z0 = charZ(hole->c, hole->rho, hole->radius).Re;
  double k = (2 * M_PI * f) / hole->c;
  t_i = innerRadiationLengthCorrection(hole->boreRadius, hole->radius);
  return imaginary(t_i * k * Z0);
}
complex holeSeriesImpedance(double f, Hole hole) {
  double a = hole->boreRadius;
  double b = hole->radius;
  double delta = b / a;
  double t = hole->length, t_0 = 0, t_a;
  double Z0 = charZ(hole->c, hole->rho, hole->boreRadius).Re;
  double k = (2 * M_PI * f) / hole->c;
  if (strcmp(hole->fingering, "CLOSED") == 0) {
    if (hole->key == NULL)
      t_0 = b * (0.55 - 0.15 / cosh(9 * t / a) +
                 0.4 / cosh(6.5 * t / a) * (delta - 1));
    t_a = closedHoleSeriesLengthCorrection(a, b, t - t_0);
  } else
    t_a = openHoleSeriesLengthCorrection(a, b);
  return imaginary(t_a * k * Z0);
}
TransferMatrix embouchureMatrix(double f, EmbouchureHole h, double entryratio,
                                complex branchZ) {
  TransferMatrix m, riserMatrix, cornerMatrix, innerRadMatrix;
  double t_m, t_i, t_a;
  double radiusin, radiusout;
  complex Z_i, Z_a;
  double Z0_hole = charZ(h->c, h->rho, h->radiusin).Re;
  double Z0_bore = charZ(h->c, h->rho, h->boreRadius).Re;
  double k = (2 * M_PI * f) / h->c;
  /* calculate the matching length correction */
  t_m = matchingLengthCorrection(h->boreRadius, h->radiusin);
  /* introduce lossy elements to account for the discontinuity */
  m = identitym();
  m->B = embouchureSeriesResistance(f, h, entryratio);
  m->C = embouchureShuntConductance(f, h, entryratio);
  /* calculate the matrix (with losses) for the tube section
  comprising the hole and matching length */
  radiusin = h->radiusin;
  radiusout = entryratio * h->radiusout;
  riserMatrix = (radiusin == radiusout)
                    ? tubeMatrix(f, h->c, h->rho, h->length + t_m, radiusin, 1)
                    : coneMatrix(f, h->c, h->rho, h->length + t_m, radiusout,
                                 radiusin, 1);
  rmultm(m, riserMatrix);
  /* calculate the inner radiation impedance */
  t_i = innerRadiationLengthCorrection(h->boreRadius, h->radiusin);
  /* add extra length correction for the embouchure hole */
  t_i = t_i + embouchureLengthCorrection(h->boreRadius, h->radiusin);
  Z_i = imaginary(t_i * k * Z0_hole);
  /* add the inner radiation impedance to the matrix m by
  multiplication */
  innerRadMatrix = identitym();
  innerRadMatrix->B = Z_i;
  rmultm(m, innerRadMatrix);
  /* calculate the series impedance */
  t_a = openHoleSeriesLengthCorrection(h->boreRadius, h->radiusin);
  Z_a = imaginary(t_a * k * Z0_bore);
  /* add half the series impedance to the branch impedance */
  branchZ = addz(branchZ, divz(Z_a, real(2.0)));
  /* multiply m by matrix representing the corner */
  cornerMatrix = identitym();
  cornerMatrix->C = divz(one, branchZ);
  cornerMatrix->B = divz(Z_a, real(2.0));
  rmultm(m, cornerMatrix);
  return m;
}
double embouchureLengthCorrection(double a, double b) {
  double delta = b / a;
  return b * 0.5 * pow(delta, 2);
}
complex embouchureSeriesResistance(double f, EmbouchureHole h,
                                   double entryratio) {
  double Z0 = charZ(h->c, h->rho, entryratio * h->radiusout).Re;
  return real(Z0 * 6.9e-6 * f);
}
complex embouchureShuntConductance(double f, EmbouchureHole h,
                                   double entryratio) {
  double Z0 = charZ(h->c, h->rho, entryratio * h->radiusout).Re;
  return real(1.3e-4 * f / Z0);
}
double matchingLengthCorrection(double a, double b) {
  /* Dalmont et al. (2002) Experimental Determination of the
  Equivalent Circuit of an Open Side Hole: Linear and Non Linear
  Behaviour, eqn. (6) */
  /* Nederveen et al. (1998) Corrections for woodwind tone-holes, eqn.
  (37) */
  double delta = b / a;
  return b * delta * (1 + 0.207 * pow(delta, 3)) / 8;
}
double innerRadiationLengthCorrection(double a, double b) {
  double delta = b / a;
  /* Dalmont et al. (2002), eqn. (4) */
  /* Nederveen et al. (1998), eqn. (40) */
  return b * (0.82 - 1.4 * pow(delta, 2) + 0.75 * pow(delta, 2.7));
}
double closedHoleSeriesLengthCorrection(double a, double b, double t) {
  double delta = b / a;
  if (t > 0)
    return -b * pow(delta, 2) /
           (1.78 / tanh(1.84 * t / b) + 0.940 + 0.540 * delta +
            0.285 * pow(delta, 2));
  else if (t < 0)
    return -b * pow(delta, 2) /
           (1.78 / (1.84 * t / b) + 0.940 + 0.540 * delta +
            0.285 * pow(delta, 2));
  else
    return 0;
}
double openHoleSeriesLengthCorrection(double a, double b) {
  double delta = b / a;
  /* Dubos et al. (1999), eqn. (74), modified to remove dependence on
  t */
  return -b * pow(delta, 2) /
         (1.78 + 0.940 + 0.540 * delta + 0.285 * pow(delta, 2));
}
