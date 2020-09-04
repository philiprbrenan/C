#line 1 "/home/phil/c/z/xml/xml.c"
//------------------------------------------------------------------------------
// Parse Xml.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef Xml_included
#define Xml_included
#include <assert.h>
#include <arenaTree.c>
#include <ctype.h>
#include <fcntl.h>
#include <readOnlyBytes.c>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fileName.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Tree.

typedef struct XmlParse                                                           // Xml parse tree
 {const struct ProtoTypes_XmlParse *proto;                                        // Prototypes for methods
  FileName      fileName;                                                       // Source file containing Xml to be parsed
  ReadOnlyBytes data;                                                           // Xml to be parsed
  ArenaTree     tree;                                                           // Parse tree created from Xml being parsed
  ArenaTree     errors;                                                         // List of errors in the xml source - the key of the node is the text of the error message, the extra data after the node is XmlError
 } XmlParse;

typedef struct XmlTag                                                             // Tag in an Xml parse tree
 {const struct ProtoTypes_XmlTag *proto;                                          // Prototypes for methods
  XmlParse        xml;                                                            // Xml parse tree
  ArenaTreeNode node;                                                           // Arena tree node in the xml parse tree that contains this tag
 } XmlTag;

#include <xml_prototypes.h>

#define XmlOpen     '<'                                                           // Important Xml characters
#define XmlClose    '>'
#define XmlSlash    '/'
#define XmlQuestion '?'
#define XmlExclaim  '!'
#define XmlmakeString(args...) ({char *m; int r = asprintf(&m, args); if(r){} m;})

static XmlTag makeXmlTag_XmlParse_ArenaTreeNode                                       // Make a tag descriptor from a parse tree node holding the tag
 (XmlParse        xml,                                                            // Xml parse tree
  ArenaTreeNode node)                                                           // Node holding tag
 {XmlTag t = newXmlTag(({struct XmlTag t = {xml: xml, node: node, proto: &ProtoTypes_XmlTag}; t;}));
  return t;
 }

static ArenaTreeNode node_XmlTag                                                  // Get a node from a tag
 (XmlTag tag)                                                                     // Tag
 {return tag.node;
 }

static XmlParse makeXmlParseFromFile                                                // Parse Xml held in a file
 (FileName      fileName)                                                       // Name of file holding Xml
 {XmlParse        x = newXmlParse(({struct XmlParse t = {proto: &ProtoTypes_XmlParse};   t;}));
  ArenaTree     t = x.tree     = makeArenaTree();                               // Parse tree,
  ArenaTree     e = x.errors   = makeArenaTree();                               // Errors list
  FileName      f = x.fileName = fileName;                                      // Name of file containing parse
  ReadOnlyBytes b = x.data     = makeReadOnlyBytesFromFile(f);                  // String to parse
  ArenaTreeNode P = t.proto->root(t);                                                   // Current parent node

  XmlParse error                                                                  // Report an error
   (char *p,                                                                    // Pointer to position at which the error occurred
    char *m)                                                                    // Message text
   {const ArenaTreeNode n = e.proto->node(e, m);                                         // Save the text of the error message as the key of a node
    n.proto->setData(n, p - b.proto->data(b));                                                  // Save the offset of the error in the node data offset.
    n.proto->putTreeLast(n);                                                            // Add the error to the error list
    return x;
   } // error

  char *p  = b.proto->data(b);                                                          // Start of text to be parsed
  if  (*p != XmlOpen)                                                             // Insist that the first character is <
   {return error(p, XmlmakeString("Xml must start with: %c\n", XmlOpen));
   }

  int remainderIsWhiteSpace(char *p)                                            // Find the next non space character in a zero terminated string
   {for(; *p; ++p) if (!isspace(*p)) return 0;                                  // Non white space
    return 1;                                                                   // Its all white sace
   }

  for(char *p = b.proto->data(b); *p;)                                                  // Break out tags and text
   {char *o = strchr(p, XmlOpen);                                                 // Find next open
    if (o)                                                                      // Found next open
     {if (o > p) P.proto->putLast(P, t.proto->noden(t, p, o - p));                              // Save text preceding open if any

      char *c = strchr(o, XmlClose);                                              // Save tag: find corresponding close
      if (c)                                                                    // Found closing >
       {ArenaTreeNode save(void)                                                // Save tag as a new node
         {return P.proto->putLast(P, t.proto->noden(t, o, c - o + 1));
         }

        const char oo = *(o+1), cc = *(c-1);                                    // First character after <, last character before >
        if (oo == XmlSlash)                                                       // Closing tag
         {save();
          if (!P.proto->isRoot(P)) P = P.proto->parent(P);                                      // Go up one level if possible
          else if (remainderIsWhiteSpace(p)) {}                                 // On root - ignore trailing white space
          else return error(o, strdup("Ignoring text beyond closing tag\n"));   // On root with remaining text
         }
        else if (oo == XmlQuestion) {}                                            // Ignore ?xml
        else if (oo == XmlExclaim)  {}                                            // Ignore !DOCTYPE
        else if (cc == XmlSlash)    {save();}                                     // Unitag
        else P = save();                                                        // Opening tag - make current node the current parent
        p = c + 1;                                                              // Start of text
       }

      else return error(o, XmlmakeString("No closing: %c\n", XmlClose));            // No closing >
     }
    else                                                                        // Check that trailing text is all spaces
     {if (remainderIsWhiteSpace(p)) return x;                                   // End of Xml text with just trailing white space
      return error(p, "Ignoring text at end\n");
     }
   }

  return x;
 }

static void free_XmlParse                                                         // Free an xml parse
 (XmlParse x)                                                                     // Xml descriptor
 {ArenaTree t = x.tree, e = x.errors;
  t.proto->free(t); e.proto->free(e);
 }

static char * print_XmlParse                                                      // Print an Xml parse tree
 (XmlParse x)                                                                     // Xml parse tree
 {ArenaTree     t = x.tree;
  ArenaTreeNode r = t.proto->root(t);
  return        r.proto->print(r);
 }

static int printsAs_XmlParse                                                      // Check the print of a parse tree is as expected.
 (const XmlParse x,                                                               // Xml descriptor
  const char * expected)                                                        // Expected print string
 {char    * s = x.proto->print(x);
  const int r = !strcmp(s, expected);
  free     (s);
  return    r;
 }

char XmltagName[256];                                                             // Tag name returned by getTag
static char * getTag_Xml                                                          // Get the tag name from a node in the Xml parse tree. The tag name is returned in a reused static buffer that the caller should not free.
 (XmlTag tag)                                                                     // Tag
 {ArenaTreeNode   n = tag.proto->node(tag);                                             // Point to key
  ArenaTreeString k = n.proto->key(n);                                               // Point to key
  if (*k == XmlOpen)                                                            // Tag
   {const size_t s = strspn(k, "< /?!"), f = strcspn(k+s, "> /?!");             // Start and finish of tag
    if (f > 0)
     {if (f < sizeof(XmltagName)) return strncpy(XmltagName, k+s, f);               // Return tag name
      printStackBackTrace("Tag  too long in %s\n", k);
     }
    printStackBackTrace("No tag in %s\n", k);
   }
  return strcpy(XmltagName, "text");                                              // In a text tag
 }

static void by_Xml_sub                                                            // Traverse an xml parse tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (XmlParse xml,                                                                   // Xml parse tree
  void (* const function) (const ArenaTreeNode node))                           // Function
 {const ArenaTree tree = xml.tree;                                              // Parse tree
  void f(const ArenaTreeNode parent)
   {function(parent);
   }
  tree.proto->by(tree, f);
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

static int develop()                                                            // Test whether we are local or on github
 {return !strcmp(getenv("HOME"), "/home/phil");
 }

void test0()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {char file[128] =   "/home/phil/c/z/xml/test.xml";
  if (!develop()) strcpy(file,  "c/z/xml/test.xml");
  XmlParse        x = makeXmlParseFromFile(makeFileName(file));
  ArenaTree     p = x.tree;
  ArenaTreeNode n;

  if (     p.proto->findFirstKey(p, "<b>", &n))
  assert(! p.proto->findFirstKey(p, "<d>", &n));
  if (     p.proto->findFirstKey(p,  "d",  &n))

  x.proto->free(x);
 }

void test1()                                                                    //Tprint
 {char file[128] =  "/home/phil/c/z/xml/samples/foreword.dita";
  if (!develop()) strcpy(file, "c/z/xml/samples/foreword.dita");
  XmlParse x = makeXmlParseFromFile(makeFileName(file));

  ArenaTree     p = x.tree;
  ArenaTreeNode n;

  if (p.proto->findFirstKey(p, "<p>", &n))
   {char *p = n.proto->print(n);
    assert(strstr(p, "<p>DITA represents a fundamental shift"));
    free(p);

    XmlTag t = x.proto->makeXmlTag(x, n);
    assert(!strcmp(t.proto->getTag(t), "p"));
   }

  if (p.proto->findFirstKey(p, "</p>", &n))
   {XmlTag t = x.proto->makeXmlTag(x, n);
    assert(!strcmp(t.proto->getTag(t), "p"));
   }

  if (p.proto->findFirstKey(p, "<conbody>", &n))
   {XmlTag t = x.proto->makeXmlTag(x, n);
    assert(!strcmp(t.proto->getTag(t), "conbody"));
   }

  x.proto->free(x);
 }

void test2()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {char *file =      "/home/phil/c/z/xml/validation/validation.xml";
  XmlParse x = makeXmlParseFromFile(makeFileName(file));

  void look(ArenaTreeNode node)
   {//const ReadOnlyBytes tag = node.proto->getTag(node);
   }

  x.proto->by(x, look);

  x.proto->free(x);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, 0};
  run_tests("Xml", 1, tests);
  return 0;
 }

#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
