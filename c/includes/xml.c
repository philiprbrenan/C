#line 1 "/home/phil/c/z/xml/xml.c"
//------------------------------------------------------------------------------
// Parse Xml.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef Xml_included
#define Xml_included
#include <ctype.h>
#include <arenaTree.c>
#include <readOnlyBytes.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Tree.

char XmltagName[256];                                                             // Tag name returned by getTag

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
    char *format, ...)                                                          // Message text
   {va_list va;
    va_start(va, format);
    char m[256]; vsnprintf(m, sizeof(m), format, va);                           // Format message
    va_end(va);
    const ArenaTreeNode n = e.proto->node(e, m);                                        // Save the text of the error message as the key of a node
    n.proto->setData(n, p - b.proto->data(b));                                                  // Save the offset of the error in the node data offset.
    n.proto->putTreeLast(n);                                                            // Add the error to the error list
    return x;
   } // error

  char *p  = b.proto->data(b);                                                          // Start of text to be parsed
  if  (*p != XmlOpen)                                                             // Insist that the first character is <
   {return error(p, "Xml must start with: %c\n", XmlOpen);
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

        void end(void)                                                          // End tag
         {size_t N = sizeof(XmltagName);
          char a[N+1]; strncpy(a, parseXmlTagName(P.proto->key(P)), N);                   // Start tag name
          char b[N+1]; strncpy(b, parseXmlTagName(o),       N);                   // End tag name

          if (strncmp(a, b, N))                                                 // Tag name mismatch
           {error(o, "End tag: %s does not match start tag: %s\n", a, b);
           }

          if (!P.proto->isRoot(P)) P = P.proto->parent(P);                                      // Go up one level if possible
          else if (remainderIsWhiteSpace(p)) {}                                 // On root - ignore trailing white space
          else error(o, "Ignoring text beyond closing tag\n");                  // On root with remaining text
         }

        const char oo = *(o+1), cc = *(c-1);                                    // First character after <, last character before >
        if        (oo == XmlSlash)     end();                                     // Closing tag
        else if   (oo == XmlQuestion) {}                                          // Ignore ?xml
        else if   (oo == XmlExclaim)  {}                                          // Ignore !DOCTYPE
        else if   (cc == XmlSlash)     save();                                    // Unitag
        else P = save();                                                        // Opening tag - make current node the current parent
        p = c + 1;                                                              // Start of text
       }

      else return error(o, "Cannot find closing: %c\n", XmlClose);                // No closing > present
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

static size_t errors_XmlParse                                                     // Number of errors encountered while creating an Xml parse tree.
 (XmlParse x)                                                                     // Xml descriptor
 {ArenaTree e = x.errors;
  return e.proto->count(e) - 1;                                                           // The root node is included in the count but does not containg an error message - so we discount it
 }

static char  * parseXmlTagName                                                    // Get the tag name from an Xml tag
 (const char * const tagString)                                                 // String representation of a tag
 {const char * const p = tagString;
  if (*p == XmlOpen)                                                            // Tag
   {const size_t s = strspn(p, "< /?!"), f = strcspn(p+s, "> /?!");             // Start and finish of tag
    if (f > 0)
     {if (f < sizeof(XmltagName))                                                 // Return tag name
       {char * const q = stpncpy(XmltagName, p+s, f);
        *q = 0;
        return XmltagName;                                                        // Add terminating
       }
      printStackBackTrace("Tag too long in %s\n", tagString);
     }
    printStackBackTrace("No tag in %s\n", tagString);
   }
  return strcpy(XmltagName, "text");                                              // In a text tag
 }

static char * getTag_XmlTag                                                       // Get the tag name from a node in the Xml parse tree. The tag name is returned in a reused static buffer that the caller should not free.
 (XmlTag tag)                                                                     // Tag
 {ArenaTreeNode n = tag.proto->node(tag);                                                 // Parse tree node defining tag
  return parseXmlTagName(n.proto->key(n));
 }

static int tagNameIs_XmlTag_string                                                // Check the name of a tag.
 (XmlTag         tag,                                                             // Tag
  char * const expected)                                                        // Expected name of tag
 {return !strcmp(tag.proto->getTag(tag), expected);
 }

static int findFirstTag_int_tree_string_XmlNodePointer                            // Find the first node in an Xml parse tree with the specified tag.
 (XmlParse       xml,                                                             // Xml parse tree
  char * const key,                                                             // Name of the tag to find
  XmlTag * const result)                                                          // Output area for tag if found
 {jmp_buf found;
  ArenaTreeNode F;                                                              // Matching node in Xml parse tree.

  void find(typeof(F) node)                                                     // Check whether the key of the current node matches the specified key
   {if (parseXmlTagName(node.proto->key(node)) && !strcmp(XmltagName, key))
     {F = node;                                                                   // Found
      longjmp(found, 1);
     }
   }

  if (!setjmp(found))                                                           // Search the tree
   {ArenaTree t = xml.tree;
    t.proto->by(t, find);
    return 0;                                                                   // No matching tag
   }
  else                                                                          // Found matching tag
   {*result = newXmlTag(({struct XmlTag t = {xml: xml, node: F, proto: &ProtoTypes_XmlTag}; t;}));
    return 1;
   }
 }

//D1 Traverse                                                                   // Traverse the Xml parse tree.

static void by_Xml_sub                                                            // Traverse an xml parse tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (XmlParse xml,                                                                   // Xml parse tree
  void (* const function) (const ArenaTreeNode node))                           // Function
 {const ArenaTree tree = xml.tree;                                              // Parse tree
  void f(const ArenaTreeNode parent)
   {function(parent);
   }
  tree.proto->by(tree, f);
 }

//D1 Print                                                                      // Print an Xml parse tree starting at the specified tag.

static ReadOnlyBytes print_readOnlyBytes_XmlTag                                   // Print the parse tree as a string starting at the specified tag.
 (const XmlTag tag)                                                               // Starting tag
 {const ArenaTreeNode n = tag.node;
  return              n.proto->print(n);
 }

static ReadOnlyBytes print_ReadOnlyBytes_XmlParse                                 // Print an entire Xml parse tree as a string.
 (XmlParse xml)                                                                   // Xml parse tree
 {ArenaTree     t = xml.tree;
  ArenaTreeNode r = t.proto->root(t);
  return            r.proto->print(r);
 }

static int printsAs_XmlTag                                                        // Check the print of an Xml parse tree starting at the specified tag is as expected.
 (const XmlTag    tag,                                                            // Starting tag
  const char *  expected)                                                       // Expected string
 {ReadOnlyBytes s = tag.proto->print(tag);
  const int     r = s.proto->equalsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printsAs_XmlParse                                                      // Check the print of an Xml parse tree is as expected.
 (const XmlParse  xml,                                                            // Xml parse tree
  const char *  expected)                                                       // Expected string
 {ReadOnlyBytes s = xml.proto->print(xml);
  const int     r = s.proto->equalsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printContains_XmlTag                                                   // Check the print of an Xml parse tree starting at the specified tag contains the expected string
 (const XmlTag    tag,                                                            // Starting tag
  const char *  expected)                                                       // Expected string
 {ReadOnlyBytes s = tag.proto->print(tag);
  const int     r = s.proto->containsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printContains_XmlParse                                                 // Check the print of an Xml parse tree contains the expected string.
 (const XmlParse  xml,                                                            // Xml parse tree
  const char *  expected)                                                       // Expected string
 {ReadOnlyBytes s = xml.proto->print(xml);
  const int     r = s.proto->containsString(s, expected);
  s.proto->free(s);
  return r;
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

static int develop()                                                            // Test whether we are local or on github
 {return !strcmp(getenv("HOME"), "/home/phil");
 }

void test0()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer //Terrors
 {char file[128] =   "/home/phil/c/z/xml/test.xml";
  if (!develop()) strcpy(file,  "c/z/xml/test.xml");

  XmlParse x = makeXmlParseFromFile(makeFileName(file));

  if (1)
   {ArenaTree e = x.errors;
    assert(x.proto->errors(x) == 1);
    assert(!strcmp(e.proto->print(e).data, "Ignoring text at end\n"));
   }

  XmlTag   t;

  assert( x.proto->findFirstTag(x, "b", &t));
  assert(!x.proto->findFirstTag(x, "d", &t));

  x.proto->free(x);
 }

void test1()                                                                    //Tprint
 {char file[128] =  "/home/phil/c/z/xml/samples/foreword.dita";
  if (!develop()) strcpy(file, "c/z/xml/samples/foreword.dita");
  XmlParse x = makeXmlParseFromFile(makeFileName(file));
  XmlTag   t;

  assert(x.proto->findFirstTag(x, "p", &t));
  assert(t.proto->printContains(t, "<p>DITA represents a fundamental shift"));
  assert(t.proto->tagNameIs(t, "p"));

  assert(x.proto->findFirstTag(x, "conbody", &t));
  assert(t.proto->tagNameIs(t, "conbody"));

  x.proto->free(x);
 }

void test2()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {char *file =      "/home/phil/c/z/xml/validation/validation.xml";
  XmlParse x = makeXmlParseFromFile(makeFileName(file));
  XmlTag   possibilities;

  if (!x.proto->findFirstTag(x, "possibilities", &possibilities)) printStackBackTrace
   ("Cannot find possibilities");

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
