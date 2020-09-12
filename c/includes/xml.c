#line 1 "/home/phil/c/z/xml/xml.c"
//------------------------------------------------------------------------------
// Parse Xml.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef Xml_included
#define Xml_included
#include <ctype.h>
#include <arenaRedBlackTree.c>
#include <arenaTree.c>
#include <readOnlyBytes.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Tree.

char XmltagName[256];                                                             // Tag name returned by tag

typedef struct XmlParse                                                           // Xml parse tree
 {const struct ProtoTypes_XmlParse *proto;                                        // Prototypes for methods
  FileName          fileName;                                                   // Source file containing Xml to be parsed
  ReadOnlyBytes     data;                                                       // Xml to be parsed
  ArenaTree         tree;                                                       // Parse tree created from Xml being parsed
  ArenaTree         errors;                                                     // List of errors in the xml source - the key of the node is the text of the error message, the extra data after the node is XmlError
  ArenaRedBlackTree possibilities;                                              // Possibility sets for each Dita tag
  ArenaRedBlackTree first;                                                      // First set of possibilities for each Dita tag
  ArenaRedBlackTree next;                                                       // Next set of possibilities for each Dita child tag under a given parent tag
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

static XmlTag makeXmlTag_XmlParse_ArenaTreeNode                                       //P Make a tag descriptor from a parse tree node holding the tag
 (const XmlParse        xml,                                                      // Xml parse tree
  const ArenaTreeNode node)                                                     // Node holding tag
 {XmlTag t = newXmlTag(({struct XmlTag t = {xml: xml, node: node, proto: &ProtoTypes_XmlTag}; t;}));
  return t;
 }

static XmlParse makeXmlParseFromFile                                                // Make a parse Xml from the contents of a file
 (FileName            fileName)                                                 // Name of file holding Xml
 {XmlParse              x = newXmlParse(({struct XmlParse t = {proto: &ProtoTypes_XmlParse};   t;}));
  const ArenaTree     t = x.tree      = makeArenaTree();                        // Parse tree,
  const ArenaTree     e = x.errors    = makeArenaTree();                        // Errors list
                      x.possibilities = makeArenaRedBlackTree();                // Single Step Validation
                      x.first         = makeArenaRedBlackTree();                // First set of possibilities for each tag
                      x.next          = makeArenaRedBlackTree();                // Next set of possibilities for each Dita child tag under a given parent tag
  const FileName      f = x.fileName  = fileName;                               // Name of file containing parse
  const ReadOnlyBytes b = x.data      = makeReadOnlyBytesFromFile(f);           // String to parse
  ArenaTreeNode       P = t.proto->root(t);                                             // Current parent node

  XmlParse error                                                                  // Report an error
   (const char * const p,                                                       // Pointer to position at which the error occurred
    const char *format, ...)                                                    // Message text
   {va_list va;
    va_start(va, format);
    char m[256]; vsnprintf(m, sizeof(m), format, va);                           // Format message
    va_end(va);
    const ArenaTreeNode n = e.proto->node(e, m);                                        // Save the text of the error message as the key of a node
    n.proto->setData(n, p - b.proto->data(b));                                                  // Save the offset of the error in the node data offset.
    n.proto->putTreeLast(n);                                                            // Add the error to the error list
    return x;
   } // error

  char *p  = b.proto->data(b); const char * const textStart = p;                        // Start of text to be parsed
  if  (*p != XmlOpen)                                                             // Insist that the first character is <
   {return error(p, "Xml must start with: %c\n", XmlOpen);
   }

  int remainderIsWhiteSpace(char *p)                                            // Find the next non space character in a zero terminated string
   {for(; *p; ++p) if (!isspace(*p)) return 0;                                  // Non white space
    return 1;                                                                   // Its all white sace
   }

  for(char *p = b.proto->data(b); *p;)                                                  // Break out tags and text
   {const char * const o = strchr(p, XmlOpen);                                    // Find next open

    if (o)                                                                      // Found next open
     {if (o > p)
       {int allBlank()                                                          // Check whether the text to be saved is all white space. We can recover the position of non whitespace from the offset saved in the data field of the node.
         {for(char * q = p; q < o; ++q) if (!isspace(*q)) return 0;
          return 1;
          }

        if (!allBlank())                                                        // Save the  text as the key of the node
         {const ArenaTreeNode n = t.proto->noden(t, p, o - p);                          // Create node
          P.proto->putLast(P, n);                                                       // Save node in parse tree
          P.proto->setData(P, p - textStart);                                           // Save text offset
         }
       }

      char * const c = strchr(o, XmlClose);                                       // Save tag: find corresponding close
      if (c)                                                                    // Found closing >
       {ArenaTreeNode save(void)                                                // Save tag as a new node
         {ArenaTreeNode n = t.proto->noden(t, o, c - o + 1);                            // Create node
          P.proto->putLast(P, n);                                                       // Save node in parse tree
          P.proto->setData(P, o - textStart);                                           // Save text offset in parse tree
          return n;
         }

        void end(void)                                                          // End tag
         {const size_t N = sizeof(XmltagName);
          char a[N+1]; strncpy(a, parseXmlTagName(P.proto->key(P)), N);                   // Start tag name
          char b[N+1]; strncpy(b, parseXmlTagName(o),       N);                   // End tag name

          if (strncmp(a, b, N))                                                 // Tag name mismatch
           {error(o, "End tag: %s does not match start tag: %s\n", b, a);
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

  if (1)                                                                        // Confirm there is exactly one root node
   {const ArenaTreeNode root = t.proto->root(t);
    const size_t N = root.proto->countChildren(root);
    char * const f = fileName.name, *p = b.proto->data(b);
    if (N == 0) return error(p, "No xml root tag found in file: %s\n",f);
    else if (N > 1) return error(p, "More than one root xml tag found in file: %s\n", f);
   }

  if (1)                                                                        // Make the single root xml tag the root of the parse tree
   {const ArenaTreeNode r = t.proto->root(t), f = r.proto->first(r);
    ArenaTreeContent *  a = r.proto->content(r), *b = f.proto->content(f);
    *a = *b;
   }

  b.proto->free(b);                                                                     // Free mapped input file as we can now reconstruct it from the parse tree

  return x;
 } // makeXmlParseFromFile

static XmlParse makeXmlParseFromString                                              // Make a parse Xml from a string.
 (const char * const string)                                                    // String of xml
 {const FileName f = makeFileNameTemporaryWithContent("zzz.xml", string, 0);    // Create a temporary file
  const XmlParse   x = makeXmlParseFromFile(f);                                     // Parse
  f.proto->unlink(f);
  return x;
 }

static void free_XmlParse                                                         // Free an xml parse
 (const XmlParse x)                                                               // Xml descriptor
 {const ArenaTree t = x.tree, e = x.errors;
  const ArenaRedBlackTree p = x.possibilities, f = x.first, n = x.next;
  t.proto->free(t); e.proto->free(e); p.proto->free(p); f.proto->free(f); n.proto->free(n);
 }

static size_t errors_XmlParse                                                     // Number of errors encountered while creating an Xml parse tree.
 (const XmlParse x)                                                               // Xml descriptor
 {return x.errors.proto->count(x.errors);
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

static char * tagName_XmlTag                                                      // Get the tag name from a node in the Xml parse tree. The tag name is returned in a reused static buffer that the caller should not free.
 (const XmlTag tag)                                                               // Tag
 {return parseXmlTagName(tag.node.proto->key(tag.node));
 }

static int tagNameEquals_XmlTag_string                                            // Check the name of a tag.
 (const XmlTag         tag,                                                       // Tag
  const char * const expected)                                                  // Expected name of tag
 {return !strcmp(tag.proto->tagName(tag), expected);
 }

static char * tagString_XmlTag                                                    //V Get the entire tag from a node in the Xml parse tree.
 (const XmlTag tag)                                                               // Tag
 {return tag.node.proto->key(tag.node);
 }

static int tagStringEquals_XmlTag_string                                          // Check the string of a tag.
 (const XmlTag         tag,                                                       // Tag
  const char * const expected)                                                  // Expected name of tag
 {return !strcmp(tag.proto->tagString(tag), expected);
 }

//D1 Navigation                                                                 // Navigate through an Xml parse tree.

#define Xmlfe( child, parent) for(XmlTag child = parent.proto->first(parent); child.proto->valid(child); child = child.proto->next(child))  // Each child in a parent from first to last
#define Xmlfer(child, parent) for(XmlTag child = parent.proto->last(parent);  child.proto->valid(child); child = child.proto->prev(child))) // Each child in a parent from last to first

static XmlTag first_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag parent)                                                            // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.node.proto->first(parent.node), proto: &ProtoTypes_XmlTag}; t;}));
 }
static XmlTag last_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag parent)                                                            // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.node.proto->last(parent.node), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 230 "/home/phil/c/z/xml/xml.c"
static XmlTag next_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag parent)                                                            // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.node.proto->next(parent.node), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 230 "/home/phil/c/z/xml/xml.c"
static XmlTag prev_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag parent)                                                            // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.node.proto->prev(parent.node), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 230 "/home/phil/c/z/xml/xml.c"
static XmlTag parent_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag parent)                                                            // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.node.proto->parent(parent.node), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 230 "/home/phil/c/z/xml/xml.c"

static XmlTag first_XmlParse                                                        // Return the first child tag in the specified Xml parse tree.
 (const XmlParse xml)                                                             // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: xml, node: xml.tree.proto->first(xml.tree), proto: &ProtoTypes_XmlTag}; t;}));
 }
static XmlTag last_XmlParse                                                        // Return the first child tag in the specified Xml parse tree.
 (const XmlParse xml)                                                             // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: xml, node: xml.tree.proto->last(xml.tree), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 236 "/home/phil/c/z/xml/xml.c"

//D1 Search                                                                     // Search the Xml parse tree

static int valid_XmlTag                                                           // Check that a tag is valid.
 (const XmlTag tag)                                                               // Tag
 {return tag.node.offset;                                                       // A tag is valid unless it is the root node of the arena tree backing the Xml parse tree.
 }

static XmlTag findFirstChild_XmlTag_XmlTag_string                                     // Find the first child tag with the specified name under the specified parent tag.
 (const XmlTag   parent,                                                          // Parent
  const char * const key)                                                       // Name of the tag to find
 {Xmlfe(child, parent)                                                            // Each child of the parent
   {if (child.proto->tagNameEquals(child, key)) return child;                               // First child with matching tag
   }
  return newXmlTag(({struct XmlTag t = {proto: &ProtoTypes_XmlTag};   t;}));
 }

static XmlTag findFirstChild_XmlTag_XmlParse_string                                   // Find the first child tag with the specified name under the root tag of the specified parse tree.
 (const XmlParse xml,                                                             // Parent
  const char * const key)                                                       // Name of the tag to find
 {const XmlTag root = xml.proto->root(xml);                                                 // Root tag
  return root.proto->findFirstChild(root, key);                                            // Each child of the parent
 }

static XmlTag findFirstTag_XmlTag_XmlTag_string                                       // Find the first tag with the specified name in the Xml parse tree starting at the specified tag.
 (const XmlTag   parent,                                                          // Parent tag
  const char * const key)                                                       // Name of the tag to find
 {jmp_buf found;
  XmlTag T = newXmlTag(({struct XmlTag t = {proto: &ProtoTypes_XmlTag};   t;}));                                                            // Tag found if any

  void find(const XmlTag tag)                                                     // Check whether the name of the tag matches the specified key
   {if (tag.proto->tagNameEquals(tag, key))
     {T = tag;                                                                  // Found
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) parent.proto->by(parent, find);                                        // Search the Xml parse tree
  return T;
 }

static XmlTag findFirstTag_XmlTag_XmlParse_string                                     // Find the first tag in an Xml parse tree with the specified name.
 (const XmlParse       xml,                                                       // Xml parse tree
  const char * const key)                                                       // Name of the tag to find
 {const XmlTag t = newXmlTag(({struct XmlTag t = {xml: xml, node: xml.tree.proto->root(xml.tree), proto: &ProtoTypes_XmlTag}; t;}));
  return t.proto->findFirstTag(t, key);
 }

static XmlTag root_XmlParse                                                         // Return the root tag of the specified Xml parse tree
 (const XmlParse xml)                                                             // Xml parse tree
 {return newXmlTag(({struct XmlTag t = {xml: xml, node: xml.tree.proto->root(xml.tree), proto: &ProtoTypes_XmlTag}; t;}));
 }

static XmlTag root_XmlTag                                                           // Return the root tsg of the Xml parse tree containing the specified tag.
 (const XmlTag tag)                                                               // Tag
 {return newXmlTag(({struct XmlTag t = {xml: tag.xml, node: tag.xml.tree.proto->root(tag.xml.tree), proto: &ProtoTypes_XmlTag}; t;}));
 }

static int equals_XmlTag_XmlTag                                                     // Confirm that two tags refer to the same text location in the source document.
 (const XmlTag a,                                                                 // First tag
  const XmlTag b)                                                                 // Second tag
 {return a.node.offset == b.node.offset;
 }

//D1 Traverse                                                                   // Traverse the Xml parse tree.

static void by_XmlTag_sub                                                         // Traverse the Xml parse tree rooted at the specified tag in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const XmlTag tag,                                                               // Starting tag
  void (* const function) (const XmlTag tag))                                     // Function to call on each tag
 {void f(const ArenaTreeNode node)
   {function(newXmlTag(({struct XmlTag t = {xml: tag.xml, node: node, proto: &ProtoTypes_XmlTag}; t;})));
   }
  tag.node.proto->by(tag.node, f);
 }

static void by_XmlParse_sub                                                       // Traverse the Xml parse tree in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const XmlParse xml,                                                             // Xml parse tree
  void (* const function) (const XmlTag tag))                                     // Function to call on each tag
 {const XmlTag root = xml.proto->root(xml);
  root.proto->by(root, function);
 }

static  size_t countChildren_size_Xml                                             // Count the number of tags at the first level in an xml parse tree.
 (const XmlParse xml)                                                             // Xml parse
 {const ArenaTreeNode root = xml.tree.proto->root(xml.tree);
  return root.proto->countChildren(root) - 1;                                              // Count the level immediately below the root tag of which there is always just one
 }

static  size_t countChildren_size_XmlTag                                          // Count the number of child tags directly under a parent tag
 (const XmlTag parent)                                                            // Parent tag
 {return parent.node.proto->countChildren(parent.node);
 }

static  size_t count_size_Xml                                                     // Count the number of tags in an Xml parse tree
 (const XmlParse x)                                                               // Xml parse tree
 {return x.tree.proto->count(x.tree);
 }

static  size_t count_size_XmlTag                                                  // Count the number of tags under the specified tag in an Xml parse tree
 (const XmlTag t)                                                                 // Parent tag
 {return t.node.proto->count(t.node);
 }


//D1 Print                                                                        // Print an Xml parse tree starting at the specified tag.
//static ReadOnlyBytes print2_readOnlyBytes_XmlTag                                  // Print the Xml parse tree as a string starting at the specified tag.
// (const XmlTag tag)                                                               // Starting tag
// {size_t  l = 0;                                                                // Length of output string
//  void len(const XmlTag parent)                                                   // Find the size of buffer we will need
//   {l += 2 * strlen(parent.proto->tag(parent));                                              // Parent tag
//    Xmlfe(child, parent) len(child);                                              // Each child
//   }
//
//  XmlString p;
//  void print(const XmlNode parent)                                                // Print the children of the specified parent
//   {const XmlString k = parent.proto->key(parent);
//    p = stpcpy(p, k);
//    Xmlfe(child, parent) print(child);                                            // Each child
//    *p = 0;                                                                     // End the string so far
//   }
//
//  len(node);
//  const XmlString s = p = alloc(l+1); *s = 0;
//  print(node);
//  return newReadOnlyBytes(({struct ReadOnlyBytes t = {data:s, length: l, ReadOnlyBytesAllocator_malloc, proto: &ProtoTypes_ReadOnlyBytes}; t;}));
// }

static ReadOnlyBytes print_readOnlyBytes_XmlTag                                   // Print the parse tree as a string starting at the specified tag.
 (const XmlTag tag)                                                               // Starting tag
 {const ArenaTreeNode n = tag.node;
  return              n.proto->print(n);
 }

static ReadOnlyBytes print_ReadOnlyBytes_XmlParse                                 // Print an entire Xml parse tree as a string.
 (const XmlParse        xml)                                                      // Xml parse tree
 {const ArenaTree     t = xml.tree;
  const ArenaTreeNode r = t.proto->root(t);
  return              r.proto->print(r);
 }

static int printsAs_XmlTag                                                        // Check the print of an Xml parse tree starting at the specified tag is as expected.
 (const XmlTag          tag,                                                      // Starting tag
  const char *  const expected)                                                 // Expected string
 {const ReadOnlyBytes s = tag.proto->print(tag);
  const int       r = s.proto->equalsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printsAs_XmlParse                                                      // Check the print of an Xml parse tree is as expected.
 (const XmlParse  xml,                                                            // Xml parse tree
  const char *  expected)                                                       // Expected string
 {const ReadOnlyBytes s = xml.proto->print(xml);
  const int       r = s.proto->equalsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printContains_XmlTag                                                   // Check the print of an Xml parse tree starting at the specified tag contains the expected string
 (const XmlTag    tag,                                                            // Starting tag
  const char *  expected)                                                       // Expected string
 {const ReadOnlyBytes s = tag.proto->print(tag);
  const int       r = s.proto->containsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printContains_XmlParse                                                 // Check the print of an Xml parse tree contains the expected string.
 (const XmlParse  xml,                                                            // Xml parse tree
  const char *  expected)                                                       // Expected string
 {const ReadOnlyBytes s = xml.proto->print(xml);
  const int       r = s.proto->containsString(s, expected);
  s.proto->free(s);
  return r;
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

static int develop()                                                            // Test whether we are local or on github
 {return !strcmp(getenv("HOME"), "/home/phil");
 }

void test0()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer //Terrors //TmakeXmlParseFromFile
 {XmlParse x = makeXmlParseFromString("<a>aa<b>bb<c/>cc</b>dd<d><e/><f/><g/></d><h/><i/></A>h");

  if (1)
   {ArenaTree e = x.errors;
    assert(x.proto->errors(x) == 2);
    assert(e.proto->printContains(e, "End tag: A does not match start tag: a"));
    assert(e.proto->printContains(e, "Ignoring text at end"));
   }

  XmlTag b = x.proto->findFirstTag(x, "b"); assert(b.proto->valid(b)); assert(b.proto->tagNameEquals(b, "b")); assert(!b.proto->tagNameEquals(b, "B"));
  XmlTag c = x.proto->findFirstTag(x, "c"); assert(c.proto->valid(c)); assert(c.proto->tagNameEquals(c, "c")); assert(!c.proto->tagNameEquals(c, "C"));
  XmlTag d = x.proto->findFirstTag(x, "d"); assert(d.proto->valid(d)); assert(d.proto->tagNameEquals(d, "d")); assert(!d.proto->tagNameEquals(d, "D"));
  XmlTag e = d.proto->first(d);             assert(e.proto->valid(e)); assert(e.proto->tagNameEquals(e, "e")); assert(!e.proto->tagNameEquals(e, "E"));
  XmlTag g = d.proto->last(d);              assert(g.proto->valid(g)); assert(g.proto->tagNameEquals(g, "g")); assert(!g.proto->tagNameEquals(g, "G"));
  XmlTag f = g.proto->prev(g);              assert(f.proto->valid(f)); assert(f.proto->tagNameEquals(f, "f")); assert(!f.proto->tagNameEquals(f, "F"));
  XmlTag h = x.proto->findFirstTag(x, "h"); assert(h.proto->valid(h)); assert(h.proto->tagNameEquals(h, "h")); assert(!h.proto->tagNameEquals(h, "H"));
  XmlTag i = x.proto->findFirstTag(x, "i"); assert(i.proto->valid(i)); assert(i.proto->tagNameEquals(i, "i")); assert(!i.proto->tagNameEquals(i, "I"));

  assert(e.proto->equals(e, f.proto->prev(f)));

  XmlTag B = b.proto->first(b);             assert(B.proto->valid(B)); assert(B.proto->tagNameEquals(B, "text")); assert(B.proto->tagStringEquals(B, "bb"));

  x.proto->free(x);
 }

void test1()                                                                    //Tfirst //Tlast //Tprev //Tnext //Tequals //Tcount //TcountChildren //TfindFirstTag //TfindFirstChild //TmakeXmlParseFromString //TparseXmlTagName //TtagName //TtagNameEquals
 {XmlParse x = makeXmlParseFromString(
"<a>"
"  <b>"
"    <c/>"
"    <d><e/>e<f/>f"
"      <g>g</g>"
"    </d>"
"    <h>h</h>"
"  </b>"
"  <i/>i<j></j>"
"</a>");

  assert(!x.errors.proto->count(x.errors));

  XmlTag b = x.proto->findFirstTag(x, "b"); assert(b.proto->valid(b)); assert(b.proto->tagNameEquals(b, "b"));
  XmlTag c = x.proto->findFirstTag(x, "c"); assert(c.proto->valid(c)); assert(c.proto->tagNameEquals(c, "c"));
  XmlTag d = b.proto->findFirstChild(b, "d"); assert(d.proto->valid(d)); assert(d.proto->tagNameEquals(d, "d"));
  XmlTag e = d.proto->findFirstChild(d, "e"); assert(e.proto->valid(e)); assert(e.proto->tagNameEquals(e, "e"));
  XmlTag f = d.proto->findFirstChild(d, "f"); assert(f.proto->valid(f)); assert(f.proto->tagNameEquals(f, "f"));
  XmlTag g = d.proto->findFirstChild(d, "g"); assert(g.proto->valid(g)); assert(g.proto->tagNameEquals(g, "g"));
  XmlTag h = b.proto->findFirstChild(b, "h"); assert(h.proto->valid(h)); assert(h.proto->tagNameEquals(h, "h"));
  XmlTag i = x.proto->findFirstTag(x, "i"); assert(i.proto->valid(i)); assert(i.proto->tagNameEquals(i, "i"));
  XmlTag j = x.proto->findFirstTag(x, "j"); assert(j.proto->valid(j)); assert(j.proto->tagNameEquals(j, "j"));

  assert(b.proto->equals(b, x.proto->first(x)));
  assert(c.proto->equals(c, b.proto->first(b)));
  assert(h.proto->equals(h, b.proto->last(b)));
  assert(j.proto->equals(j, x.proto->last(x)));

  assert(h.proto->equals(h, d.proto->next(d)));
  assert(c.proto->equals(c, d.proto->prev(d)));

  assert(x.proto->count(x)         == 14);
  assert(x.proto->countChildren(x) == 3);

  assert(b.proto->count(b)         == 10);
  assert(b.proto->countChildren(b) == 3);

  assert(!strcmp(parseXmlTagName(b.node.proto->key(b.node)), "b"));
  assert(!strcmp(b.proto->tagName(b),                 "b"));

  assert(!strcmp(b.proto->tagString(b), "<b>" ));
  assert(!strcmp(c.proto->tagString(c), "<c/>"));
  assert(b.proto->tagStringEquals(b,    "<b>" ));
  assert(c.proto->tagStringEquals(c,    "<c/>"));

  XmlTag H = h.proto->first(h);
  assert(!strcmp(H.node.proto->key(H.node),     "h"));
  assert(!strcmp(H.proto->tagName(H), "text"));

  assert(!strcmp(  H.proto->tagString(H),         "h" ));
  assert(          H.proto->tagStringEquals(H,    "h"));

  x.proto->free(x);
 }

void test2()                                                                    //Tprint
 {char file[128] =  "/home/phil/c/z/xml/samples/foreword.dita";
  if (!develop()) strcpy(file, "c/z/xml/samples/foreword.dita");

  XmlParse x = makeXmlParseFromFile(makeFileName(file));

  XmlTag p = x.proto->findFirstTag(x, "p"), c = x.proto->findFirstTag(x, "conbody");

  assert( p.proto->valid(p));
  assert( p.proto->printContains(p, "<p>DITA represents a fundamental shift"));
  assert(!p.proto->printContains(p, "Define the markup that"));
  assert( p.proto->tagNameEquals(p, "p"));

  assert( c.proto->valid(c));
  assert( c.proto->printContains(c, "<p>DITA represents a fundamental shift"));
  assert( c.proto->printContains(c, "Define the markup that"));
  assert( c.proto->tagNameEquals(c, "conbody"));

  x.proto->free(x);
 }

void test3()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {char *file =       "/home/phil/c/z/xml/validation/validation.xml";
  if (!develop()) return;                                                       // Does not work on gitHub - possibly out of memory or Cpu?
  XmlParse xml = makeXmlParseFromFile(makeFileName(file));

  XmlTag possibilities = xml.proto->findFirstChild(xml, "possibilities");                   // The possibilities possibilitiesArray assigns a number to each set of possible tags
  assert(possibilities.proto->valid(possibilities));

  XmlTag possibilitiesArray = possibilities.proto->findFirstChild(possibilities, "array");
  assert(possibilitiesArray.proto->valid(possibilitiesArray));
  size_t possibilitiesCount = possibilitiesArray.proto->countChildren(possibilitiesArray);
  assert(possibilitiesCount == 383);

  XmlTag first = xml.proto->findFirstChild(xml, "first");                                   // The tags that can come first under a specified parent tag
  assert(first.proto->valid(first));

  XmlTag firstHash = first.proto->findFirstChild(first, "hash");
  assert(firstHash.proto->valid(firstHash));
  assert(firstHash.proto->countChildren(firstHash) == 374);

  XmlTag next = xml.proto->findFirstChild(xml, "next");                                     // The tags that can come next under a specified parent tag
  assert(next.proto->valid(next));

  XmlTag nextParentHash = next.proto->findFirstChild(next, "hash");
  assert(nextParentHash.proto->valid(nextParentHash));
  assert(nextParentHash.proto->countChildren(nextParentHash) == 374);

  if (1)                                                                        // Possibilities possibilitiesArray
   {char buffer[256];
    ArenaRedBlackTreeNode pa[possibilitiesCount];
    assert(xml.possibilities.proto->count(xml.possibilities) == 0);
    size_t i = 0;

    Xmlfe(element, possibilitiesArray)                                            // Load possibilities
     {sprintf(buffer, "%lu", i);                                                // Offset as string for use as a key
      ArenaRedBlackTreeNode f = xml.possibilities.proto->add(xml.possibilities, buffer);                // Possibility set number
      assert(f.proto->valid(f));
      pa[i] = f;                                                                // Cache node offset

      Xmlfe(hash, element)                                                        // Array of hashes
       {Xmlfe(hashKey, hash)                                                      // Keys in each hash
         {char * name = hashKey.proto->tagName(hashKey);
          if (strcmp(name, "text")) f.proto->add(f, hashKey.proto->tagName(hashKey));
         }
       }
      ++i;
     }

    Xmlfe(firstTag, firstHash)                                                    // Load first tags
     {XmlTag value = firstTag.proto->first(firstTag);                                            // Value tag
      char * t = value.proto->tagString(value);                                             // Possibility number as string
      size_t a = strtol(t, 0, 0);                                               // Possibility number
      ArenaRedBlackTreeNode f = xml.first.proto->add(xml.first, firstTag.proto->tagName(firstTag));            // First possibilities for this tag
      f.proto->setData(f, pa[a].offset);                                                // The first sub tag possibilities for this tag
     }

    Xmlfe(nextParentTag, nextParentHash)                                          // Load next parents
     {char * tag = nextParentTag.proto->tagName(nextParentTag);                                     // Next parent tag
      ArenaRedBlackTreeNode f = xml.next.proto->add(xml.next, tag);                            // Add next parent tag to next tree
      assert(f.proto->valid(f));

      XmlTag nextChildHash = nextParentTag.proto->findFirstChild(nextParentTag, "hash");

      Xmlfe(nextChildTag, nextChildHash)                                          // Load next children
       {XmlTag value = nextChildTag.proto->first(nextChildTag);                                      // Value tag
        char * t = value.proto->tagString(value);                                           // Possibility number as string
        size_t a = strtol(t, 0, 0);                                             // Possibility number
        ArenaRedBlackTreeNode F = f.proto->add(f, nextChildTag.proto->tagName(nextChildTag));              // Next possibilities for the child tag
        assert(F.proto->valid(F));
        F.proto->setData(F, pa[a].offset);                                              // The next possibilities for this child under this parent
       }
     }
   }
  assert(xml.possibilities.proto->b2SumW8(xml.possibilities) == 199);
  assert(xml.first.proto->b2SumW8(xml.first) ==  19);
  assert(xml.next.proto->b2SumW8(xml.next) ==  19);

  assert(xml.possibilities.proto->used(xml.possibilities)    == 192344);
  assert(xml.first.proto->used(xml.first)    ==  14128);
  assert(xml.next.proto->used(xml.next)    == 330562);


  if (1)                                                                        // Check that 'appendix' is a possible first element of 'appendices'.
   {ArenaRedBlackTreeNode f = xml.first.proto->locate(xml.first, "appendices");
                   assert(f.proto->valid(f));
                   assert(f.proto->getData(f) == 4771);                                 // Offset to set of first possibilities for Appendices tag

    ArenaRedBlackTreeNode  n = xml.possibilities.proto->nodeFromOffset(xml.possibilities, f.proto->getData(f)); // Create a node representing the Appendices Tag first possibilities set
    assert(n.proto->equalsString(n, "8"));                                              // Appendices is tag 8
    ArenaRedBlackTreeNode p = n.proto->locate(n, "appendix");                           // Check whether Appendix is in the set of first tag possibilities for Appendices
                   assert(p.proto->valid(p));                                           // Appendix is in the first tag set of Appendices
   }

  if (1)                                                                        // Check that 'dd' can follow child 'dt' under parent 'dlentry'.
   {ArenaRedBlackTreeNode F = xml.next.proto->ll(xml.next, "dlentry", "dt", 0);                // Parent + child
                   assert(F.proto->valid(F));
    ArenaRedBlackTreeNode n = xml.possibilities.proto->nodeFromOffset(xml.possibilities, F.proto->getData(F));  // Next child possibilities under parent
    ArenaRedBlackTreeNode p = n.proto->locate(n, "dd");
                   assert(p.proto->valid(p));                                           // Check whether 'dd' can follow 'dt' under 'dlentry'
   }

  xml.proto->free(xml);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, 0};
// {void (*tests[])(void) = {test0, 0};
  run_tests("Xml", 1, tests);
  return 0;
 }

#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
