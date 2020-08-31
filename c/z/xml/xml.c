//------------------------------------------------------------------------------
// Parse Xml.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef $_included
#define $_included
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

typedef struct $                                                                // Parse tree
 {const struct ProtoTypes_$ *proto;                                             // Prototypes for methods
  FileName      fileName;                                                       // Source file containing $ to be parsed
  ReadOnlyBytes data;                                                           // $ to be parsed
  ArenaTree     tree;                                                           // Parse tree created from $ being parsed
  ArenaTree     errors;                                                         // List of errors in the xml source - the key of the node is the text of the error message, the extra data after the node is $Error
 } $;

typedef struct $Error                                                           // Error location
 {size_t byte;                                                                  // The byte at which the error occured
  size_t line;                                                                  // Line in which the error occured
  size_t lineOffset;                                                            // Offset in the line of the error
 } $Error;

typedef struct $Node                                                            // Node in the parse tree.
 {const struct ProtoTypes_$Node *proto;                                         // Prototypes for $Node methods
  ArenaTreeNode node;                                                           // Prototypes for ArenaTreeNodes
 } $Node;

#include <$$_prototypes.h>
#define $Open     '<'
#define $Close    '>'
#define $Slash    '/'
#define $Question '?'
#define $Exclaim  '!'
#define $makeString(args...) ({char *m; int r = asprintf(&m, args); if (r) {} m;})

$ new$ParseTreeFromFile                                                         // Parse $ held in a file
 (const char *   fileName)                                                      // Name of file holding $
 {ArenaTree      t = newArenaTree, errors = newArenaTree;                       // Parse tree
  ArenaTreeNode  P = t ▷ root;                                                  // Current parent node
  const FileName f = newFileName(fileName);
  ReadOnlyBytes  b = newReadOnlyBytesFromFile(f);
  $              x = {proto:&ProtoTypes_$, fileName: newFileName(fileName),     // Xml parse tree
                     tree: t, errors: errors, data: b};
//b ▷ writeOpenFile(stderr);

  $ error                                                                       // Report an error
   (char *p,                                                                    // Pointer to position at which the error occurred
    char *m)                                                                    // Message text
   {struct $Error E;                                                            // Error description
    E.byte = p - b ▷ data;                                                      // Byte offset of error
    size_t line  = 0, offset = 0;                                               // Find line an offset in line
    for(char *q  = p; *q; ++q)                                                  // Line number and position in that line
     {if (*q == '\n') {++line; offset = 0;} else ++offset;                      // New line
      if ( q == p)                                                              // Error point
       {E.line = line; E.lineOffset = offset;                                   // Record position
        ArenaTreeNode n = errors ▷ newNStrExtra(m, strlen(m), sizeof(E), &E);   // Create message node - the text of the message  is the key of the node
        errors ▷ putLast(n);                                                    // Record the error message
        return x;                                                               // Return with error detail
       }
     }
    printStackBackTrace("Cannot locate position within $\n");                   // This should not happen
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
     {if (o > p) P ▷ putLast(t ▷ newNStr(p, o - p));                            // Save text preceding open if any

      char *c = strchr(o, $Close);                                              // Save tag: find corresponding close

      if (c)                                                                    // Found closing >
       {ArenaTreeNode save(void){return P ▷ putLast(t ▷ newNStr(o, c - o + 1));}// Save tag as a new node

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

static void free_$                                                              // Free an xml tree
 ($ x)                                                                          // $ descriptor
 {ArenaTree t = x.tree, e = x.errors;
  t ▷ free; e ▷ free;
 }

static char * print_$                                                           // Print an $ parse tree
 ($ x)                                                                          // $ descriptor
 {ArenaTree     t = x.tree;
  ArenaTreeNode r = t ▷ root;
  return        r ▷ print;
 }

static int printsAs_$                                                           // Check the print of a parse tree is as expected.
 (const $      x,                                                               // $ descriptor
  const char * expected)                                                        // Expected print string
 {char    * s = x ▷ print;
  const int r = !strcmp(s, expected);
  free     (s);
  return    r;
 }

$Node nodeFromArenaTreeNode                                                     // Convert an Arena Tree Node to an $ Node.
 (ArenaTreeNode node)                                                           // Node in parse tree
 {$Node  n;                                                                     // Arena tree node
         n.node  = node;                                                        // Copy node
         n.proto = &ProtoTypes_$Node;                                           // Set prototypes
  return n;
 }

static ReadOnlyBytes getTag_$                                                   // Get the tag name.
 ($Node node)                                                                   // Node in parse tree
 {ArenaTreeNode   n = node.node;                                                // Arena tree node from $ node
  ArenaTreeString k = n ▷ key;                                                  // Point to key
  if (*k == XmlOpen)                                                            // Tag
   {const size_t s = strspn(k, "< /?!"), f = strcspn(k+s, "> /?!");             // Start and finish of tag
//say("AAAA %lu %lu\n", s, f);
    if (f > 0) return newReadOnlyBytesFromAllocatedString(k+s, f);              // Read only bytes representation of the tag
    printStackBackTrace("No tag in %s\n", k);
   }
  return newReadOnlyBytesFromAllocatedString("text", 0);                        // Text
 }

static void by_$_sub                                                            // Traverse an xml parse tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 ($ xml,                                                                        // Xml parse tree
  void (* const function) (const $Node node))                                   // Function
 {const ArenaTree tree = xml.tree;                                              // Parse tree
  void f(const ArenaTreeNode parent)
   {function(nodeFromArenaTreeNode(parent));
   }
  tree ▷ by(f);
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {const char *file = "/home/phil/c/z/xml/test.xml";
  $             x = new$ParseTreeFromFile(file);
  ArenaTree     p = x.tree;
  ArenaTreeNode n;

  if (     p ▷ findFirstKey("<b>", &n))
  assert(! p ▷ findFirstKey("<d>", &n));
  if (     p ▷ findFirstKey( "d",  &n))

  x ▷ free;
 }

void test1()                                                                    //Tprint
 {const char *file = "/home/phil/c/z/xml/samples/foreword.dita";
  $ x = new$ParseTreeFromFile(file);

  ArenaTree     p   = x.tree;
  ArenaTreeNode n;

  if (p ▷ findFirstKey("<p>", &n))
   {char *p = n ▷ print;
    assert(strstr(p, "<p>DITA represents a fundamental shift"));
    free(p);

    $Node N = nodeFromArenaTreeNode(n);
    ReadOnlyBytes r = N ▷ getTag;
    assert(       r     ▷ equalsString("p"));
   }

  if (p ▷ findFirstKey("</p>", &n))
   {$Node N = nodeFromArenaTreeNode(n);
    ReadOnlyBytes r = N ▷ getTag;
    assert(       r     ▷ equalsString("p"));
   }

  if (p ▷ findFirstKey("<conbody>", &n))
   {$Node N = nodeFromArenaTreeNode(n);
    ReadOnlyBytes r = N ▷ getTag;
    assert(       r     ▷ equalsString("conbody"));
   }

  x ▷ free;
 }

void test2()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {const char *file = "/home/phil/c/z/xml/validation/validation.xml";
  $ x = new$ParseTreeFromFile(file);

  void look($Node node)
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
