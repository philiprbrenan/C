#line 1 "/home/phil/c/z/xml/xml.c"
//------------------------------------------------------------------------------
// Parse Xml.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef Xml_included
#define Xml_included
#include <arenaTree.c>
#include <assert.h>
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

typedef struct Xml                                                                // Parse tree
 {const struct ProtoTypes_Xml *proto;                                             // Prototypes for methods
  FileName      fileName;                                                       // Source file containing Xml to be parsed
  ReadOnlyBytes data;                                                           // Xml to be parsed
  ArenaTree     tree;                                                           // Parse tree created from Xml being parsed
  ArenaTree     errors;                                                         // List of errors in the xml source - the key of the node is the text of the error message, the extra data after the node is XmlError
 } Xml;

typedef struct XmlError                                                           // Error location
 {size_t byte;                                                                  // The byte at which the error occured
  size_t line;                                                                  // Line in which the error occured
  size_t lineOffset;                                                            // Offset in the line of the error
 } XmlError;

typedef struct XmlNode                                                            // Node in the parse tree.
 {const struct ProtoTypes_XmlNode *proto;                                         // Prototypes for XmlNode methods
  ArenaTreeNode node;                                                           // Prototypes for ArenaTreeNodes
 } XmlNode;

#include <xml_prototypes.h>
#define XmlOpen     '<'
#define XmlClose    '>'
#define XmlSlash    '/'
#define XmlQuestion '?'
#define XmlExclaim  '!'
#define XmlmakeString(args...) ({char *m; int r = asprintf(&m, args); if (r) {} m;})

Xml newXmlParseTreeFromFile                                                         // Parse Xml held in a file
 (const char *   fileName)                                                      // Name of file holding Xml
 {ArenaTree      t = newArenaTree(({struct ArenaTree t = {proto: &ProtoTypes_ArenaTree};   t;})), errors = newArenaTree(({struct ArenaTree t = {proto: &ProtoTypes_ArenaTree};   t;}));                       // Parse tree
  ArenaTreeNode  P = t.proto->root(t);                                                  // Current parent node
  const FileName f = newFileName(fileName);
  ReadOnlyBytes  b = newReadOnlyBytesFromFile(f);
  Xml              x = {proto:&ProtoTypes_Xml, fileName: newFileName(fileName),     // Xml parse tree
                     tree: t, errors: errors, data: b};
  Xml error                                                                       // Report an error
   (char *p,                                                                    // Pointer to position at which the error occurred
    char *m)                                                                    // Message text
   {struct XmlError E;                                                            // Error description
    E.byte = p - b.proto->data(b);                                                      // Byte offset of error
    size_t line  = 0, offset = 0;                                               // Find line an offset in line
    for(char *q  = p; *q; ++q)                                                  // Line number and position in that line
     {if (*q == '\n') {++line; offset = 0;} else ++offset;                      // New line
      if ( q == p)                                                              // Error point
       {E.line = line; E.lineOffset = offset;                                   // Record position
        ArenaTreeNode n = errors.proto->newNStrExtra(errors, m, strlen(m), sizeof(E), &E);   // Create message node - the text of the message  is the key of the node
        errors.proto->putLast(errors, n);                                                    // Record the error message
        return x;                                                               // Return with error detail
       }
     }
    printStackBackTrace("Cannot locate position within Xml\n");                   // This should not happen
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
     {if (o > p) P.proto->putLast(P, t.proto->newNStr(t, p, o - p));                            // Save text preceding open if any

      char *c = strchr(o, XmlClose);                                              // Save tag: find corresponding close

      if (c)                                                                    // Found closing >
       {ArenaTreeNode save(void){return P.proto->putLast(P, t.proto->newNStr(t, o, c - o + 1));}// Save tag as a new node

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

static void free_Xml                                                              // Free an xml tree
 (Xml x)                                                                          // Xml descriptor
 {ArenaTree t = x.tree, e = x.errors;
  t.proto->free(t); e.proto->free(e);
 }

static char * print_Xml                                                           // Print an Xml parse tree
 (Xml x)                                                                          // Xml descriptor
 {ArenaTree     t = x.tree;
  ArenaTreeNode r = t.proto->root(t);
  return        r.proto->print(r);
 }

static int printsAs_Xml                                                           // Check the print of a parse tree is as expected.
 (const Xml      x,                                                               // Xml descriptor
  const char * expected)                                                        // Expected print string
 {char    * s = x.proto->print(x);
  const int r = !strcmp(s, expected);
  free     (s);
  return    r;
 }

XmlNode nodeFromArenaTreeNode                                                     // Convert an Arena Tree Node to an Xml Node.
 (ArenaTreeNode node)                                                           // Node in parse tree
 {XmlNode  n;                                                                     // Arena tree node
         n.node  = node;                                                        // Copy node
         n.proto = &ProtoTypes_XmlNode;                                           // Set prototypes
  return n;
 }

static ReadOnlyBytes getTag_Xml                                                   // Get the tag name.
 (XmlNode node)                                                                   // Node in parse tree
 {ArenaTreeNode   n = node.node;                                                // Arena tree node from Xml node
  ArenaTreeString k = n.proto->key(n);                                                  // Point to key
  if (*k == XmlOpen)                                                            // Tag
   {const size_t s = strspn(k, "< /?!"), f = strcspn(k+s, "> /?!");             // Start and finish of tag
//say("AAAA %lu %lu\n", s, f);
    if (f > 0) return newReadOnlyBytesFromAllocatedString(k+s, f);              // Read only bytes representation of the tag
    printStackBackTrace("No tag in %s\n", k);
   }
  return newReadOnlyBytesFromAllocatedString("text", 0);                        // Text
 }

static void by_Xml_sub                                                            // Traverse an xml parse tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (Xml xml,                                                                        // Xml parse tree
  void (* const function) (const XmlNode node))                                   // Function
 {const ArenaTree tree = xml.tree;                                              // Parse tree
  void f(const ArenaTreeNode parent)
   {function(nodeFromArenaTreeNode(parent));
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
  Xml             x = newXmlParseTreeFromFile(file);
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
  Xml x = newXmlParseTreeFromFile(file);

  ArenaTree     p   = x.tree;
  ArenaTreeNode n;

  if (p.proto->findFirstKey(p, "<p>", &n))
   {char *p = n.proto->print(n);
    assert(strstr(p, "<p>DITA represents a fundamental shift"));
    free(p);

    XmlNode N = nodeFromArenaTreeNode(n);
    ReadOnlyBytes r = N.proto->getTag(N);
    assert(       r.proto->equalsString(r, "p"));
   }

  if (p.proto->findFirstKey(p, "</p>", &n))
   {XmlNode N = nodeFromArenaTreeNode(n);
    ReadOnlyBytes r = N.proto->getTag(N);
    assert(       r.proto->equalsString(r, "p"));
   }

  if (p.proto->findFirstKey(p, "<conbody>", &n))
   {XmlNode N = nodeFromArenaTreeNode(n);
    ReadOnlyBytes r = N.proto->getTag(N);
    assert(       r.proto->equalsString(r, "conbody"));
   }

  x.proto->free(x);
 }

void test2()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {char *file =      "/home/phil/c/z/xml/validation/validation.xml";
  Xml x = newXmlParseTreeFromFile(file);

  void look(XmlNode node)
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
