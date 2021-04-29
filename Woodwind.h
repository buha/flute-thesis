/*
Woodwind.h
By Paul Dickens, 2005
A woodwind instrument.
*/
#ifndef WOODWIND_H_PROTECTOR
#define WOODWIND_H_PROTECTOR
#include "Complex.h"
#include "Map.h"
#include "TransferMatrix.h"
#include "Vector.h"
/* Maximum length of bore elements */
#define WW_MAX_LENGTH 5.0e-3
/* Temperature, humidity and CO2 */
#define WW_T_0 30.3
#define WW_T_AMB 21.0
#define WW_T_GRAD -7.7 // degrees C per metre
#define WW_HUMID 1
#define WW_X_CO2 0.025
/* Embouchure entry radius */
#define WW_EMB_RADIUS 3.9e-3;
/* Empirical corrections */
#define CORR_OPEN_FINGER_HOLE_LENGTH -0.15
#define CORR_OPEN_KEYED_HOLE_LENGTH 0.1
#define CORR_CLOSED_FINGER_HOLE_LENGTH -0.76
#define CORR_CLOSED_KEYED_HOLE_LENGTH -0.05
/* BoreSegment: */
typedef struct boresegment_str {
  double radius1;
  double radius2;
  double length;
  double c;
  double rho;
} * BoreSegment;
/* Key: */
typedef struct key_str {
  double radius;
  double holeRadius;
  double height;
  double thickness;
  double wallThickness;
  double chimneyHeight;
} * Key;
/* Hole: */
typedef struct hole_str {
  double radius;
  double length;
  double boreRadius;
  Key key;
  double c;
  double rho;
  char *fingering;
} * Hole;
/* EmbouchureHole: */
typedef struct embouchurehole_str {
  double radiusin;
  double radiusout;
  double length;
  double boreRadius;
  double c;
  double rho;
} * EmbouchureHole;
/* Head: */
typedef struct head_str {
  EmbouchureHole embouchureHole;
  Vector upstreamBore;
  double upstreamFlange;
  Vector downstreamBore;
  Map matrixMap;
} * Head;
/* UnitCell: */
typedef struct unitcell_str {
  Hole hole;
  Vector bore;
  Map openMatrixMap;
  Map closedMatrixMap;
} * UnitCell;
/* Woodwind: */
typedef struct woodwind_str {
  Head head;
  Vector cells;
  double flange;
} * Woodwind;
BoreSegment createBoreSegment(double radius1, double radius2, double length);
/*
Creates a new BoreSegment.
Parameters:
radius1: input radius in m
radius2: outpus radius in m
length: segment length in m
Returns:
a new BoreSegment with the given parameters
*/
Hole createHole(double radius, double length, double boreRadius, Key key);
/*
Creates a new Hole.
Parameters:
radius: hole radius in m
length: segment length in m
boreRadius: the radius of the instrument bore at the position of
the hole
Returns:
a new Hole with the given parameters
*/
Key createKey(double radius, double holeRadius, double height, double thickness,
              double wallThickness, double chimneyHeight);
/*
Creates a new Key.
Parameters:
radius: the radius of the key in m
holeradius: the radius of the hole in the key (for perforated
keys)
height: the regulation height of the open key
thickness: the thickness of the key
wallThickness: the thickness of the hole chimney
chimneyHeight: the height of the chimney above the bore
Returns:
a new Key with the given parameters
*/
EmbouchureHole createEmbouchureHole(double radiusin, double radiusout,
                                    double length, double boreRadius);
/*
Creates a new EmbouchureHole.
Parameters:
radiusin: the radius at the bore
radiusout: the radius at the outside
length: the embouchure hole length
boreRadius: the radius of the instrument bore at the position of
the hole
Returns:
a new EmbouchureHole with the given parameters
*/
Head createHead(EmbouchureHole embouchureHole, Vector upstreamBore,
                double upstreamFlange, Vector downstreamBore);
/*
Creates a new Head.
Parameters:
embouchureHole: the embouchure hole
upstreamBore: bore section between the embouchure hole and the
cork
upstream flange: the termination condition at the cork (default
-1)
downstreamBore: bore section between the embouchure hole and the
first tone hole
Returns:
a new Head with the given parameters
*/
UnitCell createUnitCell(Hole hole, Vector bore);
/*
Creates a new UnitCell.
Parameters:
hole: the tone hole
bore: the bore section downstream of the hole (optional)
Returns:
a new UnitCell with the given parameters
*/
Woodwind createWoodwind(Head head, Vector cells, double flange);
/*
Creates a new Woodwind.
Parameters:
head: the Head of the woodwind
cells: a vector of unit cells
flange: the termination condition at the end of the instrument
Returns:
a new Woodwind with the given parameters
*/
void setAirProperties(Woodwind w, double t_0, double t_amb, double t_grad,
                      double humid, double x_CO2);
/*
Sets the speed of sound and density of air along the instrument.
Parameters:
w: the instrument
t_0: the temperature at x = 0 (embouchure hole) in deg C
t_amb: the ambient temperature (deg C)
t_grad: the temperature gradient (deg C / m)
humid: the relative humidity (between 0 and 1)
x_CO2: the molar fraction of carbon dioxide
*/
void discretiseWoodwind(Woodwind w, double maxLength);
/*
Cuts up instrument so that no segment is longer than maxLength.
Parameters:
w: the instrument
maxLength: the maximum segment length
*/
void discretiseBore(Vector bore, double maxLength);
/*
Cuts up a bore so that no segment is longer than maxLength.
Parameters:
w: the instrument
maxLength: the maximum segment length
*/
int setFingering(Woodwind w, char *holestring);
/*
Sets the woodwind fingering to the given string.
Parameters:
w: the instrument
holestring: a string of 'X's and 'O's representing the fingering
Returns:
1 if the operation was sucessful
0 otherwise
*/
TransferMatrix boreSegmentMatrix(double f, BoreSegment s, double x);
/*
Calculates the TransferMatrix for a BoreSegment.
Parameters:
f: the frequency in Hz
s: the BoreSegment
x: distance along the BoreSegment to calculate
Returns:
the TransferMatrix for the BoreSegment
*/
TransferMatrix boreMatrix(double f, Vector bore, double x);
/*
Calculates the TransferMatrix for a bore.
Parameters:
f: the frequency in Hz
bore: the bore
x: distance along the bore to calculate
Returns:
the TransferMatrix for the bore
*/
TransferMatrix headMatrix(double f, Head h, double entryratio, double x);
/*
Calculates the TransferMatrix for a Head.
Parameters:
f: the frequency in Hz
h: the Head
entryratio: the ratio of the input side radius (impedance head or
embouchure) to the entry radius of the instrument
x: distance along the Head to calculate
Returns:
the TransferMatrix for the Head
*/
complex faceZ(double f, Head head, int midi);
/*
Calculates the radiation impeance of the player's face.
Parameters:
f: the frequency in Hz
head: the Head
midi: the MIDI number for the played note
Returns:
the radiation impedance
*/
TransferMatrix unitCellMatrix(double f, UnitCell c, double x);
/*
Calculates the TransferMatrix for a UnitCell.
Parameters:
f: the frequency in Hz
c: the UnitCell
x: distance along the UnitCell to calculate
Returns:
the TransferMatrix for the UnitCell
*/
TransferMatrix woodwindMatrix(double f, Woodwind w, double entryratio,
                              double x);
/*
Calculates the TransferMatrix for a Woodwind.
Parameters:
f: the frequency in Hz
w: the Woodwind
entryratio: the ratio of the input side radius (impedance head or
embouchure) to the entry radius of the instrument
x: distance along the Woodwind to calculate
Returns:
the TransferMatrix for the Woodwind
*/
int getZ0_c(Woodwind w, double x, complex *Z0, double *c);
/*
Calculates the characteristic impedance and speed of sound at a
given lateral position along an instrument.
Parameters:
w: the Woodwind
x: the distance along the Woodwind
Z0: the return variable for the characteristic impedance
c: the return variable for the speed of sound
Returns:
1 if x within range
0 otherwise
*/
double boreLength(Vector bore);
/*
Calculates the length of a bore.
Parameters:
bore: a vector of BoreSegments
Returns:
the total length of the bore
*/
double woodwindLengthPos(Woodwind w);
/*
Calculates the length of a Woodwind in the positive direction.
Parameters:
w: the Woodwind
Returns:
the length of the Woodwind from the embouchure hole in the
positive direction
*/
double woodwindLengthNeg(Woodwind w);
/*
Calculates the length of a Woodwind in the negative direction.
Parameters:
w: the Woodwind
Returns:
the length of the Woodwind from the embouchure hole in the
negative direction
*/
complex impedance(double f, Woodwind w, double entryratio);
/*
Calculates the input impedance of a Woodwind.
Parameters:
f: the frequency in Hz
w: the Woodwind
entryratio: the ratio of the input side radius (impedance head or
embouchure) to the entry radius of the instrument
Returns:
the input impedance of the woodwind
*/
complex playedImpedance(double f, Woodwind w, int midi);
/*
Calculates the input impedance of a Woodwind in combination with the
player's face impedance.
Parameters:
f: the frequency in Hz
w: the Woodwind
midi: the MIDI number of the played note
Returns:
the input impedance of the played woodwind
*/
double woodwindEntryRadius(Woodwind w);
/*
Calculates the radius of the woodwind at the entry.
Parameters:
w: the Woodwind
Returns:
the outside radius of the embouchure hole (if present)
otherwise the input radius of the first BoreSegment
*/
complex woodwindLoadZ(double f, Woodwind w);
/*
Calculates the load impedance at the end of a Woodwind.
Parameters:
f: the frequency in Hz
w: the Woodwind
Returns:
the load impedance of the woodwind
*/
complex woodwindDownstreamZ(double f, Woodwind w);
/*
Calculates the input impedance of the section of the Woodwind
downstream of the embouchure hole.
Parameters:
f: the frequency in Hz
w: the Woodwind
Returns:
the downstream impedance of the Woodwind
*/
TransferMatrix traverseHoleMatrix(double f, Hole h);
/*
Calculates the transfer matrix relating the acoustic parameters on
each side of the hole.
Parameters:
f: the frequency.
h: the hole.
Returns:
The transfer matrix for the hole.
*/
complex holeInputImpedance(double f, Hole hole);
/*
Calculates the input impedance of a Hole.
Parameters:
f: the frequency.
h: the hole.
Returns:
The input impedance of the hole.
*/
complex closedFingerHoleLoadZ(double f, Hole hole);
/*
Calculates the load impedance of a closed finger hole.
Parameters:
f: the frequency.
h: the hole.
Returns:
The load impedance impedance of the hole.
*/
complex closedKeyedHoleLoadZ(double f, Hole hole);
/*
Calculates the load impedance of a closed keyed hole.
Parameters:
f: the frequency.
h: the hole.
Returns:
The load impedance impedance of the hole.
*/
complex openFingerHoleLoadZ(double f, Hole hole);
/*
Calculates the load (radiation) impedance of an open finger hole.
Parameters:
f: the frequency.
h: the hole.
Returns:
The load (radiation) impedance impedance of the hole.
*/
complex openKeyedHoleLoadZ(double f, Hole hole);
/*
Calculates the load (radiation) impedance of an open keyed hole.
Parameters:
f: the frequency.
h: the hole.
Returns:
The load (radiation) impedance impedance of the hole.
*/
complex holeInnerRadiationImpedance(double f, Hole hole);
/*
Calculates the inner radiation impedance of a hole.
Parameters:
f: the frequency.
h: the hole.
Returns:
The inner radiation impedance impedance of the hole.
*/
complex holeSeriesImpedance(double f, Hole hole);
/*
Calculates the series impedance of a hole.
Parameters:
f: the frequency.
h: the hole.
Returns:
The series impedance impedance of the hole.
*/
TransferMatrix embouchureMatrix(double f, EmbouchureHole h, double entryratio,
                                complex branchZ);
/*
Calculates the TransferMatrix for an EmbouchureHole.
Parameters:
f: the frequency in Hz
h: the EmbouchureHole
entryratio: the ratio of the input side radius (impedance head or
embouchure) to the outside radius of the embouchure hole
branchZ: the impedance of the impedance branch (upstream or
downstream section)
Returns:
the TransferMatrix relating the acoustic parameters on the input
side of the embouchure hole to those at the output side (inside of
the instrument)
*/
double embouchureLengthCorrection(double a, double b);
/*
Returns the empirically-determined embouchure length correction.
Parameters:
a: the bore radius at the embouchure hole
b: the inside radius of the embouchure hole
Returns:
the length correction
*/
complex embouchureSeriesResistance(double f, EmbouchureHole h,
                                   double entryratio);
/*
Returns the empirically-determined embouchure series resistance
correction.
Parameters:
f: the frequency in Hz
h: the EmbouchureHole
entryratio: the ratio of the input side radius (impedance head or
embouchure) to the outside radius of the embouchure hole
branchZ: the impedance of the impedance branch (upstream or
downstream section)
Returns:
the series resistance
*/
complex embouchureShuntConductance(double f, EmbouchureHole h,
                                   double entryratio);
/*
Returns the empirically-determined embouchure shunt conductance
correction.
Parameters:
f: the frequency in Hz
h: the EmbouchureHole
entryratio: the ratio of the input side radius (impedance head or
embouchure) to the outside radius of the embouchure hole
branchZ: the impedance of the impedance branch (upstream or
downstream section)
Returns:
the shunt conductance
*/
double matchingLengthCorrection(double a, double b);
/*
Returns the length correction corresponding to the 'matching volume'
at the junction between a bore and a hole.
Parameters:
a: the bore radius at the hole
b: the hole radius
Returns:
the matching length correction
*/
double innerRadiationLengthCorrection(double a, double b);
/*
Returns the length correction corresponding to inner radiation at
the junction between a bore and a hole.
Parameters:
a: the bore radius at the hole
b: the hole radius
Returns:
the inner radiation length correction
*/
double closedHoleSeriesLengthCorrection(double a, double b, double t);
/*
Returns the length correction corresponding to flow widening at the
junction between a bore and a closed hole.
Parameters:
a: the bore radius at the hole
b: the hole radius
Returns:
the series length correction
*/
double openHoleSeriesLengthCorrection(double a, double b);
/*
Returns the length correction corresponding to flow widening at the
junction between a bore and an open hole.
Parameters:
a: the bore radius at the hole
b: the hole radius
Returns:
the series length correction
*/
#endif
