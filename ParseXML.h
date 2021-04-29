/*
ParseXML.c
By Paul Dickens, 2006
Based on ParseXML.c by Andrew Botros, 2003
ParseXML.c reads a woodwind definition file for Impedance.c
to use in its calculation.
*/
#ifndef PARSEXML_H_PROTECTOR
#define PARSEXML_H_PROTECTOR
#include "Vector.h"
#include "Woodwind.h"
#include <libxml/parser.h>
/* Elements and attributes of the woodwind schema */
#define WOODWIND "woodwind"
#define DESCRIPTION "description"
#define EMBOUCHUREHOLE "embouchurehole"
#define UPSTREAM "upstream"
#define DOWNSTREAM "downstream"
#define BORE "bore"
#define HOLE "hole"
#define RADIUS "radius"
#define RADIUSIN "radiusin"
#define RADIUSOUT "radiusout"
#define RADIUS1 "radius1"
#define RADIUS2 "radius2"
#define BORERADIUS "boreradius"
#define LENGTH "length"
#define FLANGE "flange"
#define KEY "key"
#define HOLERADIUS "holeradius"
#define HEIGHT "height"
#define THICKNESS "thickness"
#define WALLTHICKNESS "wallthickness"
#define CHIMNEYHEIGHT "chimneyheight"
int parseXMLFile(char *xml_filename, Woodwind *w);
/*
Creates a Woodwind struct after parsing the given XML definition
file.
Parameters:
xml_filename: filename of the XML file.
w: to be initialised with data given in XML file.
Returns
1 if the parse was successful.
0 otherwise.
*/
int parseAndValidateFile(char *xml_filename, xmlDocPtr *doc);
/*
Opens XML file and validates it against the (internally defined)
DTD.
Parameters:
xml_filename: filename of the XML file.
doc: pointer to xmlDocPtr initialised to xml_filename by libxml.
Returns:
1 if file is opened correctly and is valid.
0 otherwise.
*/
int parseWoodwind(xmlDocPtr doc, Woodwind *w);
/*
Creates a Woodwind struct from data in the xml document.
Parameters:
doc: the parsed xmlDocPtr of the XML document.
w: to be initialised with data in doc.
Returns:
1 if all parameters are valid.
0 otherwise.
*/
int parseEmbouchureHole(xmlDocPtr doc, xmlNodePtr node, EmbouchureHole *h);
/*
Creates an EmbouchureHole struct from data in the xml document.
Parameters:
doc: the parsed xmlDocPtr of the XML document.
node: the current node.
h: to be initialised with data in doc.
Returns:
1 if all parameters are valid.
0 otherwise.
*/
int parseHole(xmlDocPtr doc, xmlNodePtr node, Hole *h);
/*
Creates a Hole struct from data in the xml document.
Parameters:
doc: the parsed xmlDocPtr of the XML document.
node: the current node.
h: to be initialised with data in doc.
Returns:
1 if all parameters are valid.
0 otherwise.
*/
int parseKey(xmlDocPtr doc, xmlNodePtr node, Key *k);
/*
Creates a Key struct from data in the xml document.
Parameters:
doc: the parsed xmlDocPtr of the XML document.
node: the current node.
k: to be initialised with data in doc.
Returns:
1 if all parameters are valid.
0 otherwise.
*/
int parseUpstream(xmlDocPtr doc, xmlNodePtr node, Vector *bore, double *flange);
/*
Creates a TerminalSection of BoreSegments from data in the xml
document.
Parameters:
doc: the parsed xmlDocPtr of the XML document.
node: the current node.
bore: to be initialised with data in doc.
flange: the impedance condition at the end of the pipe.
Returns:
1 if all parameters are valid.
0 otherwise.
*/
int parseDownstream(xmlDocPtr doc, xmlNodePtr node, Vector *bore, Vector *cells,
                    double *flange);
/*
Creates a Vector of BoreSegments and a Vector of UnitCells
from data in the xml document.
Parameters:
doc: the parsed xmlDocPtr of the XML document.
node: the current node.
bore: to be initialised with data in doc.
cells: to be initialised with data in doc.
flange: the impedance condition at the end of the pipe.
Returns:
1 if all parameters are valid.
0 otherwise.
*/
int parseBore(xmlDocPtr doc, xmlNodePtr node, BoreSegment *s);
/*
Creates a BoreSegment struct from data in the xml document.
Parameters:
doc: the parsed xmlDocPtr of the XML document.
node: the current node.
s: to be initialised with data in doc.
Returns:
1 if all parameters are valid.
0 otherwise.
*/
xmlNodePtr getAndAssertDocRoot(xmlDocPtr doc);
/*
Retrieves the root node of the XML document,
asserting that it is a <woodwind> element.
Parameters:
doc: the parsed xmlDocPtr of the XML document.
Returns:
An xmlNodePtr, pointing at the root node.
NULL if <woodwind> root element is not found.
*/
int isXMLElement(xmlNodePtr node, const char *expectedname);
/*
Determines whether the supplied element node is the same type
as the expected element name.
Parameters:
node: the supplied xmlNodePtr.
expected_name: the expected element name.
Returns:
1 if names are equivalent.
0 otherwise.
*/
int assertXMLElement(xmlNodePtr node, const char *expectedname);
/*
Asserts that the supplied element node is the same type
as the expected element name.
Parameters:
node: the supplied xmlNodePtr.
expectedname: the expected element name.
Returns:
1 if names are equivalent.
0 otherwise (also prints error to stderr).
*/
char *getXMLAttribute(xmlNodePtr node, const char *attributename);
/*
Retrieves the attribute value of a given attribute name within
a given node.
Parameters:
node: the supplied xmlNodePtr.
attributename: the supplied attribute name.
Returns:
The attribute value.
NULL if attribute does not exist.
*/
double getXMLDoubleAttribute(xmlNodePtr node, const char *attributename);
/*
Retrieves the attribute value of a given attribute name within
a given node (as a double).
Parameters:
node: the supplied xmlNodePtr.
attributename: the supplied attribute name.
Returns:
The attribute value.
*/
char *getXMLData(xmlDocPtr doc, xmlNodePtr node);
/*
Retrieves the element data within a given node.
Parameters:
doc: the supplied xmlDocPtr.
node: the supplied xmlNodePtr.
Returns:
The element's value.
*/
int getXMLIntegerData(xmlDocPtr doc, xmlNodePtr node);
/*
Retrieves the integer element data within a given node.
Parameters:
doc: the supplied xmlDocPtr.
node: the supplied xmlNodePtr.
Returns:
The element's integer value.
*/
double getXMLDoubleData(xmlDocPtr doc, xmlNodePtr node);
/*
Retrieves the floating point element data within a given node.
Parameters:
doc: the supplied xmlDocPtr.
node: the supplied xmlNodePtr.
Returns:
The element's double value.
*/
double getAndScaleXMLDimensionData(xmlDocPtr doc, xmlNodePtr node);
/*
Retrieves the dimension (mm) element data within a given node
and rescales to SI units (m).
Parameters:
doc: the supplied xmlDocPtr.
node: the supplied xmlNodePtr.
Returns:
The element's double value.
*/
#endif
