#line 1 "/home/phil/c/z/xml/backup/xml5.c"
//------------------------------------------------------------------------------
// Parse Xml.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef Xml5_included
#define Xml5_included
#include <ctype.h>
#include <arenaRedBlackTree.c>
#include <arenaTree.c>
#include <readOnlyBytes.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Tree.

char Xml5tagName[256];                                                             // Tag name returned by tag

typedef struct Xml5Parse                                                           // Xml5 parse tree
 {const struct ProtoTypes_Xml5Parse *proto;                                        // Prototypes for methods
  FileName          fileName;                                                   // Source file containing Xml5 to be parsed
  ReadOnlyBytes     data;                                                       // Xml5 to be parsed
  ArenaTree         tree;                                                       // Parse tree created from Xml5 being parsed
  ArenaTree         errors;                                                     // List of errors in the xml source - the key of the node is the text of the error message, the extra data after the node is Xml5Error
  ArenaRedBlackTree possibilities;                                              // Possibility sets for each Dita tag
  ArenaRedBlackTree first;                                                      // First set of possibilities for each Dita tag
  ArenaRedBlackTree next;                                                       // Next set of possibilities for each Dita child tag under a given parent tag
 } Xml5Parse;

typedef struct Xml5Tag                                                             // Tag in an Xml5 parse tree
 {const struct ProtoTypes_Xml5Tag *proto;                                          // Prototypes for methods
  Xml5Parse        xml;                                                            // Xml5 parse tree
  ArenaTreeNode node;                                                           // Arena tree node in the xml parse tree that contains this tag
 } Xml5Tag;

#include <xml5_prototypes.h>

#define Xml5Open     '<'                                                           // Important Xml5 characters
#define Xml5Close    '>'
#define Xml5Slash    '/'
#define Xml5Question '?'
#define Xml5Exclaim  '!'

static Xml5Tag makeXml5Tag_Xml5Parse_ArenaTreeNode                                       // Make a tag descriptor from a parse tree node holding the tag
 (const Xml5Parse        xml,                                                      // Xml parse tree
  const ArenaTreeNode node)                                                     // Node holding tag
 {Xml5Tag t = newXml5Tag(({struct Xml5Tag t = {xml: xml, node: node, proto: &ProtoTypes_Xml5Tag}; t;}));
  return t;
 }

static ArenaTreeNode node_Xml5Tag                                                  // Get a node from a tag
 (const Xml5Tag tag)                                                               // Tag
 {return tag.node;
 }

static Xml5Parse makeXml5ParseFromFile                                                // Parse Xml5 held in a file
 (FileName            fileName)                                                 // Name of file holding Xml5
 {Xml5Parse              x = newXml5Parse(({struct Xml5Parse t = {proto: &ProtoTypes_Xml5Parse};   t;}));
  const ArenaTree     t = x.tree      = makeArenaTree();                        // Parse tree,
  const ArenaTree     e = x.errors    = makeArenaTree();                        // Errors list
                      x.possibilities = makeArenaRedBlackTree();                // Single Step Validation
                      x.first         = makeArenaRedBlackTree();                // First set of possibilities for each tag
                      x.next          = makeArenaRedBlackTree();                // Next set of possibilities for each Dita child tag under a given parent tag
  const FileName      f = x.fileName  = fileName;                               // Name of file containing parse
  const ReadOnlyBytes b = x.data      = makeReadOnlyBytesFromFile(f);           // String to parse
  ArenaTreeNode       P = t.proto->root(t);                                             // Current parent node

  Xml5Parse error                                                                  // Report an error
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
  if  (*p != Xml5Open)                                                             // Insist that the first character is <
   {return error(p, "Xml5 must start with: %c\n", Xml5Open);
   }

  int remainderIsWhiteSpace(char *p)                                            // Find the next non space character in a zero terminated string
   {for(; *p; ++p) if (!isspace(*p)) return 0;                                  // Non white space
    return 1;                                                                   // Its all white sace
   }

  for(char *p = b.proto->data(b); *p;)                                                  // Break out tags and text
   {const char * const o = strchr(p, Xml5Open);                                    // Find next open

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

      char * const c = strchr(o, Xml5Close);                                       // Save tag: find corresponding close
      if (c)                                                                    // Found closing >
       {ArenaTreeNode save(void)                                                // Save tag as a new node
         {ArenaTreeNode n = t.proto->noden(t, o, c - o + 1);                            // Create node
          P.proto->putLast(P, n);                                                       // Save node in parse tree
          P.proto->setData(P, o - textStart);                                           // Save text offset in parse tree
          return n;
         }

        void end(void)                                                          // End tag
         {const size_t N = sizeof(Xml5tagName);
          char a[N+1]; strncpy(a, parseXml5TagName(P.proto->key(P)), N);                   // Start tag name
          char b[N+1]; strncpy(b, parseXml5TagName(o),       N);                   // End tag name

          if (strncmp(a, b, N))                                                 // Tag name mismatch
           {error(o, "End tag: %s does not match start tag: %s\n", b, a);
           }

          if (!P.proto->isRoot(P)) P = P.proto->parent(P);                                      // Go up one level if possible
          else if (remainderIsWhiteSpace(p)) {}                                 // On root - ignore trailing white space
          else error(o, "Ignoring text beyond closing tag\n");                  // On root with remaining text
         }

        const char oo = *(o+1), cc = *(c-1);                                    // First character after <, last character before >
        if        (oo == Xml5Slash)     end();                                     // Closing tag
        else if   (oo == Xml5Question) {}                                          // Ignore ?xml
        else if   (oo == Xml5Exclaim)  {}                                          // Ignore !DOCTYPE
        else if   (cc == Xml5Slash)     save();                                    // Unitag
        else P = save();                                                        // Opening tag - make current node the current parent
        p = c + 1;                                                              // Start of text
       }

      else return error(o, "Cannot find closing: %c\n", Xml5Close);                // No closing > present
     }
    else                                                                        // Check that trailing text is all spaces
     {if (remainderIsWhiteSpace(p)) return x;                                   // End of Xml5 text with just trailing white space
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
 }

static void free_Xml5Parse                                                         // Free an xml parse
 (const Xml5Parse x)                                                               // Xml5 descriptor
 {const ArenaTree t = x.tree, e = x.errors;
  const ArenaRedBlackTree p = x.possibilities, f = x.first, n = x.next;
  t.proto->free(t); e.proto->free(e); p.proto->free(p); f.proto->free(f); n.proto->free(n);
 }

static size_t errors_Xml5Parse                                                     // Number of errors encountered while creating an Xml5 parse tree.
 (const Xml5Parse x)                                                               // Xml5 descriptor
 {return x.errors.proto->count(x.errors) - 1;                                                  // The root node is included in the count but does not containg an error message - so we discount it
 }

static char  * parseXml5TagName                                                    // Get the tag name from an Xml5 tag
 (const char * const tagString)                                                 // String representation of a tag
 {const char * const p = tagString;
  if (*p == XmlOpen)                                                            // Tag
   {const size_t s = strspn(p, "< /?!"), f = strcspn(p+s, "> /?!");             // Start and finish of tag
    if (f > 0)
     {if (f < sizeof(Xml5tagName))                                                 // Return tag name
       {char * const q = stpncpy(Xml5tagName, p+s, f);
        *q = 0;
        return Xml5tagName;                                                        // Add terminating
       }
      printStackBackTrace("Tag too long in %s\n", tagString);
     }
    printStackBackTrace("No tag in %s\n", tagString);
   }
  return strcpy(Xml5tagName, "text");                                              // In a text tag
 }

static char * tagName_Xml5Tag                                                      // Get the tag name from a node in the Xml5 parse tree. The tag name is returned in a reused static buffer that the caller should not free.
 (const Xml5Tag tag)                                                               // Tag
 {const ArenaTreeNode n = tag.proto->node(tag);                                           // Parse tree node defining tag
  return parseXml5TagName(n.proto->key(n));
 }

static int tagNameEquals_Xml5Tag_string                                            // Check the name of a tag.
 (const Xml5Tag         tag,                                                       // Tag
  const char * const expected)                                                  // Expected name of tag
 {return !strcmp(tag.proto->tagName(tag), expected);
 }

static char * tagString_Xml5Tag                                                    //V Get the entire tag from a node in the Xml5 parse tree.
 (const Xml5Tag tag)                                                               // Tag
 {const ArenaTreeNode n = tag.proto->node(tag);                                           // Parse tree node defining tag
  return              n.proto->key(n);
 }

static int tagStringEquals_Xml5Tag_string                                          // Check the string of a tag.
 (const Xml5Tag         tag,                                                       // Tag
  const char * const expected)                                                  // Expected name of tag
 {return !strcmp(tag.proto->tagString(tag), expected);
 }

//D1 Navigation                                                                 // Navigate through an Xml5 parse tree.

#define Xml5fe( child, parent) for(Xml5Tag child = parent.proto->first(parent); child.node.offset; child = child.proto->next(child)) // Each child in a parent from first to last
#define Xml5fer(child, parent) for(Xml5Tag child = parent.proto->last(parent);  child.node.offset; child = child.proto->prev(child))) // Each child in a parent from last to first

static Xml5Tag first_Xml5Tag                                                          // Return the first child tag under the specified parent tag.
 (const Xml5Tag parent)                                                            // Parent tag
 {return newXml5Tag(({struct Xml5Tag t = {xml: parent.xml, node: parent.node.proto->first(parent.node), proto: &ProtoTypes_Xml5Tag}; t;}));
 }

static Xml5Tag last_Xml5Tag                                                           // Return the last child tag under the specified parent tag.
 (const Xml5Tag parent)                                                            // Parent tag
 {return newXml5Tag(({struct Xml5Tag t = {xml: parent.xml, node: parent.node.proto->last(parent.node), proto: &ProtoTypes_Xml5Tag}; t;}));
 }

static Xml5Tag next_Xml5Tag                                                           // Return the next sibling tag after this one.
 (const Xml5Tag sibling)                                                           // Sibling tag
 {return newXml5Tag(({struct Xml5Tag t = {xml: sibling.xml, node: sibling.node.proto->next(sibling.node), proto: &ProtoTypes_Xml5Tag}; t;}));
 }

static Xml5Tag prev_Xml5Tag                                                           // Return the previous sibling tag before this one.
 (const Xml5Tag sibling)                                                           // Sibling tag
 {return newXml5Tag(({struct Xml5Tag t = {xml: sibling.xml, node: sibling.node.proto->prev(sibling.node), proto: &ProtoTypes_Xml5Tag}; t;}));
 }

//D1 Search                                                                     // Search the Xml5 parse tree

static int valid_Xml5Tag                                                           // Check that a tag is valid.
 (const Xml5Tag tag)                                                               // Tag
 {return tag.node.offset;                                                       // A tag is valid unless it is the root node of the arena tree backing the Xml5 parse tree.
 }

static Xml5Tag findFirstChild_Xml5Tag_Xml5Tag_string                                     // Find the first child tag with the specified name under the specified parent tag.
 (const Xml5Tag   parent,                                                          // Parent
  const char * const key)                                                             // Name of the tag to find
 {Xml5fe(child, parent)                                                            // Each child of the parent
   {if (child.proto->tagNameEquals(child, key)) return child;                                   // First child with matching tag
   }
  return newXml5Tag(({struct Xml5Tag t = {proto: &ProtoTypes_Xml5Tag};   t;}));
 }

static Xml5Tag findFirstChild_Xml5Tag_Xml5Parse_string                                   // Find the first child tag with the specified name under the root tag of the specified parse tree.
 (const Xml5Parse xml,                                                             // Parent
  const char * const key)                                                       // Name of the tag to find
 {const Xml5Tag root = xml.proto->root(xml);                                                 // Root tag
  return root.proto->findFirstChild(root, key);                                            // Each child of the parent
 }

static Xml5Tag findFirstTag_Xml5Tag_Xml5Tag_string                                       // Find the first tag with the specified name in the Xml5 parse tree starting at the specified tag.
 (const Xml5Tag   parent,                                                          // Parent tag
  const char * const key)                                                       // Name of the tag to find
 {jmp_buf found;
  Xml5Tag T = newXml5Tag(({struct Xml5Tag t = {proto: &ProtoTypes_Xml5Tag};   t;}));                                                            // Tag found if any

  void find(const Xml5Tag tag)                                                     // Check whether the name of the tag matches the specified key
   {if (tag.proto->tagNameEquals(tag, key))
     {T = tag;                                                                  // Found
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) parent.proto->by(parent, find);                                        // Search the Xml5 parse tree
  return T;
 }

static Xml5Tag findFirstTag_Xml5Tag_Xml5Parse_string                                     // Find the first tag in an Xml5 parse tree with the specified name.
 (const Xml5Parse       xml,                                                       // Xml parse tree
  const char * const key)                                                       // Name of the tag to find
 {const Xml5Tag t = newXml5Tag(({struct Xml5Tag t = {xml: xml, node: xml.tree.proto->root(xml.tree), proto: &ProtoTypes_Xml5Tag}; t;}));
  return t.proto->findFirstTag(t, key);
 }

static Xml5Tag root_Xml5Parse                                                         // Return the root tag of the specified Xml5 parse tree
 (const Xml5Parse xml)                                                             // Xml5 parse tree
 {return newXml5Tag(({struct Xml5Tag t = {xml: xml, node: xml.tree.proto->root(xml.tree), proto: &ProtoTypes_Xml5Tag}; t;}));
 }

static Xml5Tag root_Xml5Tag                                                           // Return the root tsg of the Xml5 parse tree containing the specified tag.
 (const Xml5Tag tag)                                                               // Tag
 {return newXml5Tag(({struct Xml5Tag t = {xml: tag.xml, node: tag.xml.tree.proto->root(tag.xml.tree), proto: &ProtoTypes_Xml5Tag}; t;}));
 }

static int equals_Xml5Tag_Xml5Tag                                                     // Confirm that two tags refer to the same text location in the source document.
 (const Xml5Tag a,                                                                 // First tag
  const Xml5Tag b)                                                                 // Second tag
 {return a.node.offset == b.node.offset;
 }

//D1 Traverse                                                                   // Traverse the Xml5 parse tree.

static void by_Xml5Tag_sub                                                         // Traverse the Xml5 parse tree rooted at the specified tag in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const Xml5Tag tag,                                                               // Starting tag
  void (* const function) (const Xml5Tag tag))                                     // Function to call on each tag
 {void f(const ArenaTreeNode node)
   {function(newXml5Tag(({struct Xml5Tag t = {xml: tag.xml, node: node, proto: &ProtoTypes_Xml5Tag}; t;})));
   }
  tag.node.proto->by(tag.node, f);
 }

static void by_Xml5Parse_sub                                                       // Traverse the Xml5 parse tree in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const Xml5Parse xml,                                                             // Xml5 parse tree
  void (* const function) (const Xml5Tag tag))                                     // Function to call on each tag
 {const Xml5Tag root = xml.proto->root(xml);
  root.proto->by(root, function);
 }

static  size_t countChildren_size_Xml5Tag                                          // Count the number of child tags directly under a parent tag
 (const Xml5Tag parent)                                                            // Parent tag
 {return parent.node.proto->countChildren(parent.node);
 }

static  size_t count_size_Xml5                                                     // Count the number of tags in an Xml5 parse tree
 (const Xml5Parse x)                                                               // Xml5 parse tree
 {return x.tree.proto->count(x.tree);
 }


//D1 Print                                                                        // Print an Xml5 parse tree starting at the specified tag.
//static ReadOnlyBytes print2_readOnlyBytes_Xml5Tag                                  // Print the Xml5 parse tree as a string starting at the specified tag.
// (const Xml5Tag tag)                                                               // Starting tag
// {size_t  l = 0;                                                                // Length of output string
//  void len(const Xml5Tag parent)                                                   // Find the size of buffer we will need
//   {l += 2 * strlen(parent.proto->tag(parent));                                              // Parent tag
//    Xml5fe(child, parent) len(child);                                              // Each child
//   }
//
//  Xml5String p;
//  void print(const Xml5Node parent)                                                // Print the children of the specified parent
//   {const Xml5String k = parent.proto->key(parent);
//    p = stpcpy(p, k);
//    Xml5fe(child, parent) print(child);                                            // Each child
//    *p = 0;                                                                     // End the string so far
//   }
//
//  len(node);
//  const Xml5String s = p = alloc(l+1); *s = 0;
//  print(node);
//  return newReadOnlyBytes(({struct ReadOnlyBytes t = {data:s, length: l, ReadOnlyBytesAllocator_malloc, proto: &ProtoTypes_ReadOnlyBytes}; t;}));
// }

static ReadOnlyBytes print_readOnlyBytes_Xml5Tag                                   // Print the parse tree as a string starting at the specified tag.
 (const Xml5Tag tag)                                                               // Starting tag
 {const ArenaTreeNode n = tag.node;
  return              n.proto->print(n);
 }

static ReadOnlyBytes print_ReadOnlyBytes_Xml5Parse                                 // Print an entire Xml5 parse tree as a string.
 (const Xml5Parse        xml)                                                      // Xml5 parse tree
 {const ArenaTree     t = xml.tree;
  const ArenaTreeNode r = t.proto->root(t);
  return              r.proto->print(r);
 }

static int printsAs_Xml5Tag                                                        // Check the print of an Xml5 parse tree starting at the specified tag is as expected.
 (const Xml5Tag          tag,                                                      // Starting tag
  const char *  const expected)                                                 // Expected string
 {const ReadOnlyBytes s = tag.proto->print(tag);
  const int       r = s.proto->equalsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printsAs_Xml5Parse                                                      // Check the print of an Xml5 parse tree is as expected.
 (const Xml5Parse  xml,                                                            // Xml5 parse tree
  const char *  expected)                                                       // Expected string
 {const ReadOnlyBytes s = xml.proto->print(xml);
  const int       r = s.proto->equalsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printContains_Xml5Tag                                                   // Check the print of an Xml5 parse tree starting at the specified tag contains the expected string
 (const Xml5Tag    tag,                                                            // Starting tag
  const char *  expected)                                                       // Expected string
 {const ReadOnlyBytes s = tag.proto->print(tag);
  const int       r = s.proto->containsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printContains_Xml5Parse                                                 // Check the print of an Xml5 parse tree contains the expected string.
 (const Xml5Parse  xml,                                                            // Xml5 parse tree
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

void test0()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer //Terrors
 {char file[128] =   "/home/phil/c/z/xml/test.xml";
  if (!develop()) strcpy(file,  "c/z/xml/test.xml");

  Xml5Parse x = makeXml5ParseFromFile(makeFileName(file));

  if (1)
   {ArenaTree e = x.errors;
    assert(x.proto->errors(x) == 2);
    assert(e.proto->printContains(e, "End tag: A does not match start tag: a"));
    assert(e.proto->printContains(e, "Ignoring text at end"));
   }

  Xml5Tag b = x.proto->findFirstTag(x, "b"); assert(b.proto->valid(b)); assert(b.proto->tagNameEquals(b, "b")); assert(!b.proto->tagNameEquals(b, "B"));
  Xml5Tag c = x.proto->findFirstTag(x, "c"); assert(c.proto->valid(c)); assert(c.proto->tagNameEquals(c, "c")); assert(!c.proto->tagNameEquals(c, "C"));
  Xml5Tag d = x.proto->findFirstTag(x, "d"); assert(d.proto->valid(d)); assert(d.proto->tagNameEquals(d, "d")); assert(!d.proto->tagNameEquals(d, "D"));
  Xml5Tag e = d.proto->first(d);             assert(e.proto->valid(e)); assert(e.proto->tagNameEquals(e, "e")); assert(!e.proto->tagNameEquals(e, "E"));
  Xml5Tag g = d.proto->last(d);              assert(g.proto->valid(g)); assert(g.proto->tagNameEquals(g, "g")); assert(!g.proto->tagNameEquals(g, "G"));
  Xml5Tag f = g.proto->prev(g);              assert(f.proto->valid(f)); assert(f.proto->tagNameEquals(f, "f")); assert(!f.proto->tagNameEquals(f, "F"));
  Xml5Tag h = x.proto->findFirstTag(x, "h"); assert(h.proto->valid(h)); assert(h.proto->tagNameEquals(h, "h")); assert(!h.proto->tagNameEquals(h, "H"));
  Xml5Tag i = x.proto->findFirstTag(x, "i"); assert(i.proto->valid(i)); assert(i.proto->tagNameEquals(i, "i")); assert(!i.proto->tagNameEquals(i, "I"));

  assert(e.proto->equals(e, f.proto->prev(f)));

  Xml5Tag B = b.proto->first(b);             assert(B.proto->valid(B)); assert(B.proto->tagNameEquals(B, "text")); assert(B.proto->tagStringEquals(B, "bb"));

  x.proto->free(x);
 }

void test1()                                                                    //Tprint
 {char file[128] =  "/home/phil/c/z/xml/samples/foreword.dita";
  if (!develop()) strcpy(file, "c/z/xml/samples/foreword.dita");

  Xml5Parse x = makeXml5ParseFromFile(makeFileName(file));

  Xml5Tag p = x.proto->findFirstTag(x, "p"), c = x.proto->findFirstTag(x, "conbody");

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

void test2()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {char *file =       "/home/phil/c/z/xml/validation/validation.xml";
  if (!develop()) return;                                                       // Does not work on gitHub - possibly out of memory or Cpu?
  Xml5Parse xml = makeXml5ParseFromFile(makeFileName(file));

  Xml5Tag possibilities = xml.proto->findFirstChild(xml, "possibilities");                   // The possibilities possibilitiesArray assigns a number to each set of possible tags
  assert(possibilities.proto->valid(possibilities));

  Xml5Tag possibilitiesArray = possibilities.proto->findFirstChild(possibilities, "array");
  assert(possibilitiesArray.proto->valid(possibilitiesArray));
  size_t possibilitiesCount = possibilitiesArray.proto->countChildren(possibilitiesArray);
  assert(possibilitiesCount == 383);

  Xml5Tag first = xml.proto->findFirstChild(xml, "first");                                   // The tags that can come first under a specified parent tag
  assert(first.proto->valid(first));

  Xml5Tag firstHash = first.proto->findFirstChild(first, "hash");
  assert(firstHash.proto->valid(firstHash));
  assert(firstHash.proto->countChildren(firstHash) == 374);

  Xml5Tag next = xml.proto->findFirstChild(xml, "next");                                     // The tags that can come next under a specified parent tag
  assert(next.proto->valid(next));

  Xml5Tag nextParentHash = next.proto->findFirstChild(next, "hash");
  assert(nextParentHash.proto->valid(nextParentHash));
  assert(nextParentHash.proto->countChildren(nextParentHash) == 374);

  if (1)                                                                        // Possibilities possibilitiesArray
   {char buffer[256];
    ArenaRedBlackTreeNode pa[possibilitiesCount];
    assert(xml.possibilities.proto->count(xml.possibilities) == 0);
    size_t i = 0;

    Xml5fe(element, possibilitiesArray)                                            // Load possibilities
     {sprintf(buffer, "%lu", i);                                                // Offset as string for use as a key
      ArenaRedBlackTreeFound f = xml.possibilities.proto->add(xml.possibilities, buffer);               // Possibility set number
      assert(f.node.proto->valid(f.node));
      pa[i] = f.node;                                                           // Cache node offset

      Xml5fe(hash, element)                                                        // Array of hashes
       {Xml5fe(hashKey, hash)                                                      // Keys in each hash
         {char * name = hashKey.proto->tagName(hashKey);
          if (strcmp(name, "text")) f.node.proto->add(f.node, hashKey.proto->tagName(hashKey));
         }
       }
      ++i;
     }

    Xml5fe(firstTag, firstHash)                                                    // Load first tags
     {Xml5Tag value = firstTag.proto->first(firstTag);                                            // Value tag
      char * t = value.proto->tagString(value);                                             // Possibility number as string
      size_t a = strtol(t, 0, 0);                                               // Possibility number
      ArenaRedBlackTreeFound f = xml.first.proto->add(xml.first, firstTag.proto->tagName(firstTag));           // First possibilities for this tag
      f.node.proto->setData(f.node, pa[a].offset);                                           // The first sub tag possibilities for this tag
     }

    Xml5fe(nextParentTag, nextParentHash)                                          // Load next parents
     {ArenaRedBlackTreeFound f = xml.next.proto->add(xml.next, nextParentTag.proto->tagName(nextParentTag));       // Next parent tag
say("AAAA %s\n", f.node.proto->key(f.node));
say("PPPP %s\n", xml.next.proto->sprint(xml.next).data);
      assert(f.node.proto->valid(f.node));
      Xml5Tag nextChildHash = nextParentTag.proto->findFirstChild(nextParentTag, "hash");
      Xml5fe(nextChildTag, nextChildHash)                                          // Load next children
       {Xml5Tag value = nextChildTag.proto->first(nextChildTag);                                      // Value tag
        char * t = value.proto->tagString(value);                                           // Possibility number as string
        size_t a = strtol(t, 0, 0);                                             // Possibility number
        ArenaRedBlackTreeFound F = f.node.proto->add(f.node, nextChildTag.proto->tagName(nextChildTag));        // Next possibilities for the child tag
        assert(F.node.proto->valid(F.node));
        F.node.proto->setData(F.node, pa[a].offset);                                         // The next possibilities for this child under this parent
say("BBBB %s %s %lu\n", f.node.proto->key(f.node), F.node.proto->key(F.node), a);
exit(1);
       }
     }
   }
say("AAAA possibilities: %lu\n", xml.possibilities.arena->used);
say("BBBB first        : %lu\n", xml.first.arena->used);
say("CCCC next         : %lu\n", xml.next.arena->used);


  if (1)                                                                        // Check that 'appendix' is a possible first element of 'appendices'.
   {ArenaRedBlackTreeFound f = xml.first.proto->find(xml.first, "appendices");
                    assert(f.node.proto->valid(f.node));
    assert(f.node.proto->getData(f.node) == 4771);                                           // Offset to set of first possibilities for Appendices tag
    ArenaRedBlackTreeNode  n = xml.possibilities.proto->nodeFromOffset(xml.possibilities, f.node.proto->getData(f.node)); // Create a node representing the Appendices Tag first possibilities set
    assert(n.proto->equalsString(n, "8"));                                              // Appendices is tag 8
    ArenaRedBlackTreeFound p = n.proto->find(n, "appendix");                            // Check whether Appendix is in the set of first tag possibilities for Appendices
    assert(p.node.proto->valid(p.node));                                                     // Appendix is in the first tag set of Appendices
   }

  if (1)                                                                        // Check that 'dd' can follow child 'dt' under parent 'dlentry'.
   {ArenaRedBlackTreeFound f = xml.next.proto->find(xml.next, "dlentry");                      // Parent
                    assert(f.node.proto->valid(f.node));
    ArenaRedBlackTreeFound F = f.node.proto->find(f.node, "dt");                             // Child
                    assert(F.node.proto->valid(F.node));
    ArenaRedBlackTreeNode  n = xml.possibilities.proto->nodeFromOffset(xml.possibilities, F.node.proto->getData(F.node)); // Next child possibilities under parent
    ArenaRedBlackTreeFound p = n.proto->find(n, "dd");
    assert(p.node.proto->valid(p.node));                                                     // Check whether 'dd' can follow 'dt' under 'dlentry'
   }

  xml.proto->free(xml);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, 0};
// {void (*tests[])(void) = {test0, 0};
  run_tests("Xml5", 1, tests);
  return 0;
 }

#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
