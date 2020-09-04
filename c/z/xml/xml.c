//------------------------------------------------------------------------------
// Parse Xml.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef $_included
#define $_included
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

typedef struct $Parse                                                           // $ parse tree
 {const struct ProtoTypes_$Parse *proto;                                        // Prototypes for methods
  FileName      fileName;                                                       // Source file containing $ to be parsed
  ReadOnlyBytes data;                                                           // $ to be parsed
  ArenaTree     tree;                                                           // Parse tree created from $ being parsed
  ArenaTree     errors;                                                         // List of errors in the xml source - the key of the node is the text of the error message, the extra data after the node is $Error
 } $Parse;

typedef struct $Tag                                                             // Tag in an $ parse tree
 {const struct ProtoTypes_$Tag *proto;                                          // Prototypes for methods
  $Parse        xml;                                                            // $ parse tree
  ArenaTreeNode node;                                                           // Arena tree node in the xml parse tree that contains this tag
 } $Tag;

#include <$$_prototypes.h>

#define $Open     '<'                                                           // Important $ characters
#define $Close    '>'
#define $Slash    '/'
#define $Question '?'
#define $Exclaim  '!'
#define $makeString(args...) ({char *m; int r = asprintf(&m, args); if(r){} m;})

static $Tag make$Tag_$Parse_ArenaTreeNode                                       // Make a tag descriptor from a parse tree node holding the tag
 ($Parse        xml,                                                            // Xml parse tree
  ArenaTreeNode node)                                                           // Node holding tag
 {$Tag t = new$Tag(xml: xml, node: node);
  return t;
 }

static ArenaTreeNode node_$Tag                                                  // Get a node from a tag
 ($Tag tag)                                                                     // Tag
 {return tag.node;
 }

static $Parse make$ParseFromFile                                                // Parse $ held in a file
 (FileName      fileName)                                                       // Name of file holding $
 {$Parse        x = new$Parse;
  ArenaTree     t = x.tree     = makeArenaTree();                               // Parse tree,
  ArenaTree     e = x.errors   = makeArenaTree();                               // Errors list
  FileName      f = x.fileName = fileName;                                      // Name of file containing parse
  ReadOnlyBytes b = x.data     = makeReadOnlyBytesFromFile(f);                  // String to parse
  ArenaTreeNode P = t ▷ root;                                                   // Current parent node

  $Parse error                                                                  // Report an error
   (char *p,                                                                    // Pointer to position at which the error occurred
    char *m)                                                                    // Message text
   {const ArenaTreeNode n = e ▷ node(m);                                         // Save the text of the error message as the key of a node
    n ▷ setData(p - b ▷ data);                                                  // Save the offset of the error in the node data offset.
    n ▷ putTreeLast;                                                            // Add the error to the error list
    return x;
   } // error

  char *p  = b ▷ data;                                                          // Start of text to be parsed
  if  (*p != $Open)                                                             // Insist that the first character is <
   {return error(p, $makeString("$ must start with: %c\n", $Open));
   }

  int remainderIsWhiteSpace(char *p)                                            // Find the next non space character in a zero terminated string
   {for(; *p; ++p) if (!isspace(*p)) return 0;                                  // Non white space
    return 1;                                                                   // Its all white sace
   }

  for(char *p = b ▷ data; *p;)                                                  // Break out tags and text
   {char *o = strchr(p, $Open);                                                 // Find next open
    if (o)                                                                      // Found next open
     {if (o > p) P ▷ putLast(t ▷ noden(p, o - p));                              // Save text preceding open if any

      char *c = strchr(o, $Close);                                              // Save tag: find corresponding close
      if (c)                                                                    // Found closing >
       {ArenaTreeNode save(void)                                                // Save tag as a new node
         {return P ▷ putLast(t ▷ noden(o, c - o + 1));
         }

        const char oo = *(o+1), cc = *(c-1);                                    // First character after <, last character before >
        if (oo == $Slash)                                                       // Closing tag
         {save();
          if (!P ▷ isRoot) P = P ▷ parent;                                      // Go up one level if possible
          else if (remainderIsWhiteSpace(p)) {}                                 // On root - ignore trailing white space
          else return error(o, strdup("Ignoring text beyond closing tag\n"));   // On root with remaining text
         }
        else if (oo == $Question) {}                                            // Ignore ?xml
        else if (oo == $Exclaim)  {}                                            // Ignore !DOCTYPE
        else if (cc == $Slash)    {save();}                                     // Unitag
        else P = save();                                                        // Opening tag - make current node the current parent
        p = c + 1;                                                              // Start of text
       }

      else return error(o, $makeString("No closing: %c\n", $Close));            // No closing >
     }
    else                                                                        // Check that trailing text is all spaces
     {if (remainderIsWhiteSpace(p)) return x;                                   // End of $ text with just trailing white space
      return error(p, "Ignoring text at end\n");
     }
   }

  return x;
 }

static void free_$Parse                                                         // Free an xml parse
 ($Parse x)                                                                     // $ descriptor
 {ArenaTree t = x.tree, e = x.errors;
  t ▷ free; e ▷ free;
 }

static char * print_$Parse                                                      // Print an $ parse tree
 ($Parse x)                                                                     // $ parse tree
 {ArenaTree     t = x.tree;
  ArenaTreeNode r = t ▷ root;
  return        r ▷ print;
 }

static int printsAs_$Parse                                                      // Check the print of a parse tree is as expected.
 (const $Parse x,                                                               // $ descriptor
  const char * expected)                                                        // Expected print string
 {char    * s = x ▷ print;
  const int r = !strcmp(s, expected);
  free     (s);
  return    r;
 }

char $tagName[256];                                                             // Tag name returned by getTag
static char * getTag_$                                                          // Get the tag name from a node in the $ parse tree. The tag name is returned in a reused static buffer that the caller should not free.
 ($Tag tag)                                                                     // Tag
 {ArenaTreeNode   n = tag  ▷ node;                                             // Point to key
  ArenaTreeString k = n    ▷ key;                                               // Point to key
  if (*k == XmlOpen)                                                            // Tag
   {const size_t s = strspn(k, "< /?!"), f = strcspn(k+s, "> /?!");             // Start and finish of tag
    if (f > 0)
     {if (f < sizeof($tagName)) return strncpy($tagName, k+s, f);               // Return tag name
      printStackBackTrace("Tag  too long in %s\n", k);
     }
    printStackBackTrace("No tag in %s\n", k);
   }
  return strcpy($tagName, "text");                                              // In a text tag
 }

static void by_$_sub                                                            // Traverse an xml parse tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 ($Parse xml,                                                                   // Xml parse tree
  void (* const function) (const ArenaTreeNode node))                           // Function
 {const ArenaTree tree = xml.tree;                                              // Parse tree
  void f(const ArenaTreeNode parent)
   {function(parent);
   }
  tree ▷ by(f);
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

static int develop()                                                            // Test whether we are local or on github
 {return !strcmp(getenv("HOME"), "/home/phil");
 }

void test0()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {char file[128] =   "/home/phil/c/z/xml/test.xml";
  if (!develop()) strcpy(file,  "c/z/xml/test.xml");
  $Parse        x = make$ParseFromFile(makeFileName(file));
  ArenaTree     p = x.tree;
  ArenaTreeNode n;

  if (     p ▷ findFirstKey("<b>", &n))
  assert(! p ▷ findFirstKey("<d>", &n));
  if (     p ▷ findFirstKey( "d",  &n))

  x ▷ free;
 }

void test1()                                                                    //Tprint
 {char file[128] =  "/home/phil/c/z/xml/samples/foreword.dita";
  if (!develop()) strcpy(file, "c/z/xml/samples/foreword.dita");
  $Parse x = make$ParseFromFile(makeFileName(file));

  ArenaTree     p = x.tree;
  ArenaTreeNode n;

  if (p ▷ findFirstKey("<p>", &n))
   {char *p = n ▷ print;
    assert(strstr(p, "<p>DITA represents a fundamental shift"));
    free(p);

    $Tag t = x ▷ make$Tag(n);
    assert(!strcmp(t ▷ getTag, "p"));
   }

  if (p ▷ findFirstKey("</p>", &n))
   {$Tag t = x ▷ make$Tag(n);
    assert(!strcmp(t ▷ getTag, "p"));
   }

  if (p ▷ findFirstKey("<conbody>", &n))
   {$Tag t = x ▷ make$Tag(n);
    assert(!strcmp(t ▷ getTag, "conbody"));
   }

  x ▷ free;
 }

void test2()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {char *file =      "/home/phil/c/z/xml/validation/validation.xml";
  $Parse x = make$ParseFromFile(makeFileName(file));

  void look(ArenaTreeNode node)
   {//const ReadOnlyBytes tag = node ▷ getTag;
   }

  x ▷ by(look);

  x ▷ free;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, 0};
  run_tests("$", 1, tests);
  return 0;
 }

#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
