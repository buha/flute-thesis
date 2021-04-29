/*
ParseXML.c
By Paul Dickens, 2006
Based on ParseXML.c by Andrew Botros, 2003
ParseXML.c reads a woodwind definition file for Impedance.c
to use in its calculation.
Refer to ParseXML.h for interface details.
*/
#include "ParseXML.h"
#include "Vector.h"
#include "Woodwind.h"
#include <ctype.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int parseXMLFile(char *xml_filename, Woodwind *w) {
  xmlDocPtr doc; /* the resulting document tree */
  /* Read and validate XML file */
  if (!parseAndValidateFile(xml_filename, &doc))
    return 0;
  /* Build instrument */
  if (!parseWoodwind(doc, w))
    return 0;
  /* Free XML data structure */
  xmlFreeDoc(doc);
  return 1;
}
int parseAndValidateFile(char *xml_filename, xmlDocPtr *doc) {
  xmlParserCtxtPtr ctxt; /* the parser context */
  /* create a parser context */
  ctxt = xmlNewParserCtxt();
  if (ctxt == NULL) {
    fprintf(stderr, "XML Error: Failed to allocate parser context\n");
    return 0;
  }
  /* parse the file, activating the DTD validation option */
  *doc = xmlCtxtReadFile(ctxt, xml_filename, NULL, XML_PARSE_DTDVALID);
  /* check if parsing succeeded */
  if (*doc == NULL) {
    fprintf(stderr, "XML Error: Failed to parse %s\n", xml_filename);
    return 0;
  }
  /* check if validation succeeded */
  if (ctxt->valid == 0) {
    fprintf(stderr, "XML Error: Failed to validate %s\n", xml_filename);
    return 0;
  }
  xmlFreeParserCtxt(ctxt);
  return 1;
}
int parseWoodwind(xmlDocPtr doc, Woodwind *w) {
  xmlNodePtr curnode;
  EmbouchureHole embouchureHole = NULL;
  Vector upstreamBore, downstreamBore, cells;
  double upstreamFlange, flange;
  Head head;
  /* Retrieve and validate root node */
  if ((curnode = getAndAssertDocRoot(doc)) == NULL)
    return 0;
  curnode = curnode->children;
  while (curnode != NULL) {
    if (isXMLElement(curnode, EMBOUCHUREHOLE))
      if (!parseEmbouchureHole(doc, curnode, &embouchureHole))
        return 0;
    if (isXMLElement(curnode, UPSTREAM))
      if (!parseUpstream(doc, curnode, &upstreamBore, &upstreamFlange))
        return 0;
    if (isXMLElement(curnode, DOWNSTREAM))
      if (!parseDownstream(doc, curnode, &downstreamBore, &cells, &flange))
        return 0;
    curnode = curnode->next;
  }
  head =
      createHead(embouchureHole, upstreamBore, upstreamFlange, downstreamBore);
  *w = createWoodwind(head, cells, flange);
  return 1;
}
int parseEmbouchureHole(xmlDocPtr doc, xmlNodePtr node, EmbouchureHole *h) {
  double radiusin, radiusout, length, boreRadius;
  node = node->children;
  while (node != NULL) {
    /* Fill Hole data */
    if (isXMLElement(node, RADIUSIN))
      radiusin = getAndScaleXMLDimensionData(doc, node);
    if (isXMLElement(node, RADIUSOUT))
      radiusout = getAndScaleXMLDimensionData(doc, node);
    if (isXMLElement(node, LENGTH))
      length = getAndScaleXMLDimensionData(doc, node);
    if (isXMLElement(node, BORERADIUS))
      boreRadius = getAndScaleXMLDimensionData(doc, node);
    node = node->next;
  }
  /* Ensure dimensions as expected */
  if ((radiusin <= 0.0) || (radiusout <= 0.0) || (boreRadius <= 0.0) ||
      (length <= 0.0)) {
    fprintf(stderr, "XML error: Embouchure hole has invalid dimensions.\n");
    return 0;
  }
  *h = createEmbouchureHole(radiusin, radiusout, length, boreRadius);
  return 1;
}
int parseHole(xmlDocPtr doc, xmlNodePtr node, Hole *h) {
  double radius, length, boreRadius;
  Key key = NULL;
  node = node->children;
  while (node != NULL) {
    /* Fill Hole data */
    if (isXMLElement(node, RADIUS))
      radius = getAndScaleXMLDimensionData(doc, node);
    if (isXMLElement(node, LENGTH))
      length = getAndScaleXMLDimensionData(doc, node);
    if (isXMLElement(node, BORERADIUS))
      boreRadius = getAndScaleXMLDimensionData(doc, node);
    if (isXMLElement(node, KEY))
      if (!parseKey(doc, node, &key))
        return 0;
    node = node->next;
  }
  /* Ensure dimensions as expected */
  if ((radius <= 0.0) || (boreRadius <= 0.0) || (length <= 0.0)) {
    fprintf(stderr, "XML error: Hole has invalid dimensions.\n");
    return 0;
  }
  *h = createHole(radius, length, boreRadius, key);
  return 1;
}
int parseKey(xmlDocPtr doc, xmlNodePtr node, Key *k) {
  double radius, holeRadius, height, thickness, wallThickness, chimneyHeight;
  node = node->children;
  while (node != NULL) {
    /* Fill Key data */
    if (isXMLElement(node, RADIUS))
      radius = getAndScaleXMLDimensionData(doc, node);
    if (isXMLElement(node, HOLERADIUS))
      holeRadius = getAndScaleXMLDimensionData(doc, node);
    if (isXMLElement(node, HEIGHT))
      height = getAndScaleXMLDimensionData(doc, node);
    if (isXMLElement(node, THICKNESS))
      thickness = getAndScaleXMLDimensionData(doc, node);
    if (isXMLElement(node, WALLTHICKNESS))
      wallThickness = getAndScaleXMLDimensionData(doc, node);
    if (isXMLElement(node, CHIMNEYHEIGHT))
      chimneyHeight = getAndScaleXMLDimensionData(doc, node);
    node = node->next;
  }
  /* Ensure dimensions as expected */
  if ((radius <= 0.0) || (holeRadius < 0.0) || (height <= 0.0) ||
      (thickness <= 0.0) || (wallThickness < 0.0) || (chimneyHeight < 0.0)) {
    fprintf(stderr, "XML error: Key has invalid dimensions.\n");
    return 0;
  }
  *k = createKey(radius, holeRadius, height, thickness, wallThickness,
                 chimneyHeight);
  return 1;
}
int parseUpstream(xmlDocPtr doc, xmlNodePtr node, Vector *bore,
                  double *flange) {
  BoreSegment s;
  *bore = createVector();
  *flange = getXMLDoubleAttribute(node, FLANGE);
  /* Get current node's children */
  node = node->children;
  /* Add to bore all segments until the end */
  while (node != NULL) {
    if (isXMLElement(node, BORE)) {
      if (!parseBore(doc, node, &s))
        return 0;
      addElement(*bore, s);
    }
    node = node->next;
  }
  /* Ensure flange as expected */
  if ((*flange < 0.0) && (*flange != -1.0)) {
    fprintf(stderr, "XML error: Flange is invalid.\n");
    return 0;
  }
  return 1;
}
int parseDownstream(xmlDocPtr doc, xmlNodePtr node, Vector *bore, Vector *cells,
                    double *flange) {
  UnitCell curCell;
  BoreSegment s;
  Hole h;
  Vector curBore;
  *bore = createVector();
  *cells = createVector();
  *flange = getXMLDoubleAttribute(node, FLANGE);
  curBore = *bore;
  /* Get current node's children */
  node = node->children;
  while (node != NULL) {
    if (isXMLElement(node, BORE)) {
      if (!parseBore(doc, node, &s))
        return 0;
      addElement(curBore, s);
    }
    if (isXMLElement(node, HOLE)) {
      if (!parseHole(doc, node, &h))
        return 0;
      curBore = createVector();
      curCell = createUnitCell(h, curBore);
      addElement(*cells, curCell);
    }
    node = node->next;
  }
  /* Ensure flange as expected */
  if ((*flange < 0.0) && (*flange != -1.0)) {
    fprintf(stderr, "XML error: Flange is invalid.\n");
    return 0;
  }
  return 1;
}
int parseBore(xmlDocPtr doc, xmlNodePtr node, BoreSegment *s) {
  double radius1, radius2, length;
  /* Search node's children for data elements */
  node = node->children;
  while (node != NULL) {
    if (isXMLElement(node, RADIUS1))
      radius1 = getAndScaleXMLDimensionData(doc, node);
    if (isXMLElement(node, RADIUS2))
      radius2 = getAndScaleXMLDimensionData(doc, node);
    if (isXMLElement(node, LENGTH))
      length = getAndScaleXMLDimensionData(doc, node);
    node = node->next;
  }
  /* Ensure dimensions as expected */
  if ((radius1 <= 0.0) || (radius2 <= 0.0) || (length <= 0.0)) {
    fprintf(stderr, "XML error: Bore has invalid dimensions.\n");
    return 0;
  }
  *s = createBoreSegment(radius1, radius2, length);
  return 1;
}
xmlNodePtr getAndAssertDocRoot(xmlDocPtr doc) {
  xmlNodePtr rootnode;
  /* Parse root node and ensure XML file is non-empty */
  if ((rootnode = xmlDocGetRootElement(doc)) == NULL) {
    fprintf(stderr, "XML error: Empty XML document.\n");
    return NULL;
  }
  /* Ensure root node as expected */
  if (!assertXMLElement(rootnode, WOODWIND)) {
    fprintf(stderr, "XML error: Document root is not <%s>.\n", WOODWIND);
    return NULL;
  }
  return rootnode;
}
int isXMLElement(xmlNodePtr node, const char *expectedname) {
  if (xmlStrcmp(node->name, (const xmlChar *)expectedname) == 0)
    return 1;
  else
    return 0;
}
int assertXMLElement(xmlNodePtr node, const char *expectedname) {
  if (isXMLElement(node, expectedname))
    return 1;
  else {
    fprintf(stderr, "XML error: Expected \"<%s>\", got \"<%s>\".\n",
            expectedname, (const char *)(node->name));
    return 0;
  }
}
char *getXMLAttribute(xmlNodePtr node, const char *attributename) {
  return (char *)xmlGetProp(node, (const xmlChar *)attributename);
}
double getXMLDoubleAttribute(xmlNodePtr node, const char *attributename) {
  char *cdata = getXMLAttribute(node, attributename);
  return atof(cdata);
}
char *getXMLData(xmlDocPtr doc, xmlNodePtr node) {
  return (char *)xmlNodeListGetString(doc, node->children, 1);
}
int getXMLIntegerData(xmlDocPtr doc, xmlNodePtr node) {
  char *pcdata = getXMLData(doc, node);
  return atoi(pcdata);
}
double getXMLDoubleData(xmlDocPtr doc, xmlNodePtr node) {
  char *pcdata = getXMLData(doc, node);
  return atof(pcdata);
}
double getAndScaleXMLDimensionData(xmlDocPtr doc, xmlNodePtr node) {
  return 1e-3 * getXMLDoubleData(doc, node);
}
