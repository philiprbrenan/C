//------------------------------------------------------------------------------
// Parse Xml.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef $_included
#define $_included
#include <ctype.h>
#include <arenaRedBlackTree.c>
#include <arenaTree.c>
#include <readOnlyBytes.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Tree.

char $tagName[256];                                                             // Tag name returned by getTag

typedef struct $Parse                                                           // $ parse tree
 {const struct ProtoTypes_$Parse *proto;                                        // Prototypes for methods
  FileName          fileName;                                                   // Source file containing $ to be parsed
  ReadOnlyBytes     data;                                                       // $ to be parsed
  ArenaTree         tree;                                                       // Parse tree created from $ being parsed
  ArenaTree         errors;                                                     // List of errors in the xml source - the key of the node is the text of the error message, the extra data after the node is $Error
  ArenaRedBlackTree validation;                                                 // Single Step Validation relationships
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
 (FileName          fileName)                                                   // Name of file holding $
 {$Parse            x = new$Parse;
  ArenaTree         t = x.tree       = makeArenaTree();                         // Parse tree,
  ArenaTree         e = x.errors     = makeArenaTree();                         // Errors list
                        x.validation = makeArenaRedBlackTree();                 // Single Step Validation
  FileName          f = x.fileName   = fileName;                                // Name of file containing parse
  ReadOnlyBytes     b = x.data       = makeReadOnlyBytesFromFile(f);            // String to parse
  ArenaTreeNode     P = t ▷ root;                                               // Current parent node

  $Parse error                                                                  // Report an error
   (char *p,                                                                    // Pointer to position at which the error occurred
    char *format, ...)                                                          // Message text
   {va_list va;
    va_start(va, format);
    char m[256]; vsnprintf(m, sizeof(m), format, va);                           // Format message
    va_end(va);
    const ArenaTreeNode n = e ▷ node(m);                                        // Save the text of the error message as the key of a node
    n ▷ setData(p - b ▷ data);                                                  // Save the offset of the error in the node data offset.
    n ▷ putTreeLast;                                                            // Add the error to the error list
    return x;
   } // error

  char *p  = b ▷ data; const char * const textStart = p;                        // Start of text to be parsed
  if  (*p != $Open)                                                             // Insist that the first character is <
   {return error(p, "$ must start with: %c\n", $Open);
   }

  int remainderIsWhiteSpace(char *p)                                            // Find the next non space character in a zero terminated string
   {for(; *p; ++p) if (!isspace(*p)) return 0;                                  // Non white space
    return 1;                                                                   // Its all white sace
   }

  for(char *p = b ▷ data; *p;)                                                  // Break out tags and text
   {char *o = strchr(p, $Open);                                                 // Find next open

    if (o)                                                                      // Found next open
     {if (o > p)                                                                // Save text preceding open if any and not all white space. We can recover the position of non whitespace from the offset saved in the data field of the node.
       {int allBlank()
         {for(char * q = p; q < o; ++q) if (!isspace(*q)) return 0;
          return 1;
          }

        if (!allBlank())                                                        // Save text
         {ArenaTreeNode n = t ▷ noden(p, o - p);                                // Create node
          P ▷ putLast(n);                                                       // Save node in parse tree
          P ▷ setData(p - textStart);                                           // Save text offset
         }
       }

      char *c = strchr(o, $Close);                                              // Save tag: find corresponding close
      if (c)                                                                    // Found closing >
       {ArenaTreeNode save(void)                                                // Save tag as a new node
         {ArenaTreeNode n = t ▷ noden(o, c - o + 1);                            // Create node
          P ▷ putLast(n);                                                       // Save node in parse tree
          P ▷ setData(o - textStart);                                           // Save text offset in parse tree
          return n;
         }

        void end(void)                                                          // End tag
         {size_t N = sizeof($tagName);
          char a[N+1]; strncpy(a, parse$TagName(P ▷ key), N);                   // Start tag name
          char b[N+1]; strncpy(b, parse$TagName(o),       N);                   // End tag name

          if (strncmp(a, b, N))                                                 // Tag name mismatch
           {error(o, "End tag: %s does not match start tag: %s\n", b, a);
           }

          if (!P ▷ isRoot) P = P ▷ parent;                                      // Go up one level if possible
          else if (remainderIsWhiteSpace(p)) {}                                 // On root - ignore trailing white space
          else error(o, "Ignoring text beyond closing tag\n");                  // On root with remaining text
         }

        const char oo = *(o+1), cc = *(c-1);                                    // First character after <, last character before >
        if        (oo == $Slash)     end();                                     // Closing tag
        else if   (oo == $Question) {}                                          // Ignore ?xml
        else if   (oo == $Exclaim)  {}                                          // Ignore !DOCTYPE
        else if   (cc == $Slash)     save();                                    // Unitag
        else P = save();                                                        // Opening tag - make current node the current parent
        p = c + 1;                                                              // Start of text
       }

      else return error(o, "Cannot find closing: %c\n", $Close);                // No closing > present
     }
    else                                                                        // Check that trailing text is all spaces
     {if (remainderIsWhiteSpace(p)) return x;                                   // End of $ text with just trailing white space
      return error(p, "Ignoring text at end\n");
     }
   }

  if (1)                                                                        // Confirm there is exactly one root node
   {const ArenaTreeNode root = t ▷ root;
    const size_t N = root ▷ countChildren;
    char * const f = fileName.name, *p = b ▷ data;
    if (N == 0) return error(p, "No xml root tag found in file: %s\n",f);
    else if (N > 1) return error(p, "More than one root xml tag found in file: %s\n", f);
   }

  if (1)                                                                        // Make the single root xml tag the root of the parse tree
   {ArenaTreeNode r = t ▷ root, f = r ▷ first;
    ArenaTreeContent *a = r ▷ content, *b = f ▷ content;
    *a = *b;
   }

  return x;
 }

static void free_$Parse                                                         // Free an xml parse
 ($Parse x)                                                                     // $ descriptor
 {ArenaTree t = x.tree, e = x.errors;
  t ▷ free; e ▷ free;
 }

static size_t errors_$Parse                                                     // Number of errors encountered while creating an $ parse tree.
 ($Parse x)                                                                     // $ descriptor
 {ArenaTree e = x.errors;
  return e ▷ count - 1;                                                           // The root node is included in the count but does not containg an error message - so we discount it
 }

static char  * parse$TagName                                                    // Get the tag name from an $ tag
 (const char * const tagString)                                                 // String representation of a tag
 {const char * const p = tagString;
  if (*p == XmlOpen)                                                            // Tag
   {const size_t s = strspn(p, "< /?!"), f = strcspn(p+s, "> /?!");             // Start and finish of tag
    if (f > 0)
     {if (f < sizeof($tagName))                                                 // Return tag name
       {char * const q = stpncpy($tagName, p+s, f);
        *q = 0;
        return $tagName;                                                        // Add terminating
       }
      printStackBackTrace("Tag too long in %s\n", tagString);
     }
    printStackBackTrace("No tag in %s\n", tagString);
   }
  return strcpy($tagName, "text");                                              // In a text tag
 }

static char * getTag_$Tag                                                       // Get the tag name from a node in the $ parse tree. The tag name is returned in a reused static buffer that the caller should not free.
 ($Tag tag)                                                                     // Tag
 {ArenaTreeNode n = tag ▷ node;                                                 // Parse tree node defining tag
  return parse$TagName(n ▷ key);
 }

static int tagNameIs_$Tag_string                                                // Check the name of a tag.
 ($Tag         tag,                                                             // Tag
  char * const expected)                                                        // Expected name of tag
 {return !strcmp(tag ▷ getTag, expected);
 }

//D1 Navigation                                                                 // Navigate through an $ parse tree.

#define $fe( child, parent) for($Tag child = parent ▷ first; child.node.offset; child = child ▷ next) // Each child in a parent from first to last
#define $fer(child, parent) for($Tag child = parent ▷ last;  child.node.offset; child = child ▷ prev)) // Each child in a parent from last to first

static $Tag first_$Tag                                                          // Return the first child tag under the specified parent tag.
 ($Tag parent)                                                                  // Parent tag
 {return new $Tag(xml: parent.xml, node: parent.node ▷ first);
 }

static $Tag last_$Tag                                                           // Return the last child tag under the specified parent tag.
 ($Tag parent)                                                                  // Parent tag
 {return new $Tag(xml: parent.xml, node: parent.node ▷ first);
 }

static $Tag next_$Tag                                                           // Return the next sibling tag after this one.
 ($Tag sibling)                                                                 // Sibling tag
 {return new $Tag(xml: sibling.xml, node: sibling.node ▷ next);
 }

static $Tag prev_$Tag                                                           // Return the previous sibling tag before this one.
 ($Tag sibling)                                                                 // Sibling tag
 {return new $Tag(xml: sibling.xml, node: sibling.node ▷ prev);
 }

//D1 Search                                                                     // Search the $ parse tree

static int valid_$Tag                                                           // Check that a tag is valid.
 (const $Tag tag)                                                               // Tag
 {return tag.node.offset;                                                       // A tag is valid unless it is the root node of the arena tree backing the $ parse tree.
 }

static $Tag findFirstChild_$Tag_$Tag_string                                     // Find the first child tag with the specified name under the specified parent tag.
 ($Tag         parent,                                                          // Parent
  char * const key)                                                             // Name of the tag to find
 {$fe(child, parent)                                                            // Each child of the parent
   {if (child ▷ tagNameIs(key)) return child;                                   // First child with matching tag
   }
  return new $Tag;
 }

static $Tag findFirstChild_$Tag_$Parse_string                                   // Find the first child tag with the specified name under the root tag of the specified parse tree.
 ($Parse       xml,                                                             // Parent
  char * const key)                                                             // Name of the tag to find
 {$Tag root = xml ▷ root;                                                       // Root tag
  return root ▷ findFirstChild(key);                                            // Each child of the parent
 }

static $Tag findFirstTag_$Tag_$Tag_string                                       // Find the first tag with the specified name in the $ parse tree starting at the specified tag.
 ($Tag         parent,                                                          // Parent tag
  char * const key)                                                             // Name of the tag to find
 {jmp_buf found;
  $Tag T = new $Tag;                                                            // Tag found if any

  void find($Tag tag)                                                           // Check whether the name of the tag matches the specified key
   {if (tag ▷ tagNameIs(key))
     {T = tag;                                                                  // Found
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) parent ▷ by(find);                                        // Search the $ parse tree
  return T;
 }

static $Tag findFirstTag_$Tag_$Parse_string                                     // Find the first tag in an $ parse tree with the specified name.
 ($Parse       xml,                                                             // Xml parse tree
  char * const key)                                                             // Name of the tag to find
 {const $Tag t = new $Tag(xml: xml, node: xml.tree ▷ root);
  return t ▷ findFirstTag(key);
 }

static $Tag root_$Parse                                                         // Return the root tag of the specified $ parse tree
 ($Parse xml)                                                                   // $ parse tree
 {return new $Tag(xml: xml, node: xml.tree ▷ root);
 }

static $Tag root_$Tag                                                           // Return the root tsg of the $ parse tree containing the specified tag.
 ($Tag tag)                                                                     // Tag
 {return new $Tag(xml: tag.xml, node: tag.xml.tree ▷ root);
 }

//D1 Traverse                                                                   // Traverse the $ parse tree.

static void by_$Tag_sub                                                         // Traverse the $ parse tree rooted at the specified tag in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 ($Tag tag,                                                                     // Starting tag
  void (* const function) (const $Tag tag))                                     // Function to call on each tag
 {void f(const ArenaTreeNode node)
   {function(new $Tag(xml: tag.xml, node: node));
   }
  tag.node ▷ by(f);
 }

static void by_$Parse_sub                                                       // Traverse the $ parse tree in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 ($Parse xml,                                                                   // $ parse tree
  void (* const function) (const $Tag tag))                                     // Function to call on each tag
 {$Tag root = xml ▷ root;
  root ▷ by(function);
 }

//D1 Print                                                                      // Print an $ parse tree starting at the specified tag.

static ReadOnlyBytes print_readOnlyBytes_$Tag                                   // Print the parse tree as a string starting at the specified tag.
 (const $Tag tag)                                                               // Starting tag
 {const ArenaTreeNode n = tag.node;
  return              n ▷ print;
 }

static ReadOnlyBytes print_ReadOnlyBytes_$Parse                                 // Print an entire $ parse tree as a string.
 ($Parse xml)                                                                   // $ parse tree
 {ArenaTree     t = xml.tree;
  ArenaTreeNode r = t ▷ root;
  return            r ▷ print;
 }

static int printsAs_$Tag                                                        // Check the print of an $ parse tree starting at the specified tag is as expected.
 (const $Tag    tag,                                                            // Starting tag
  const char *  expected)                                                       // Expected string
 {ReadOnlyBytes s = tag ▷ print;
  const int     r = s ▷ equalsString(expected);
  s ▷ free;
  return r;
 }

static int printsAs_$Parse                                                      // Check the print of an $ parse tree is as expected.
 (const $Parse  xml,                                                            // $ parse tree
  const char *  expected)                                                       // Expected string
 {ReadOnlyBytes s = xml ▷ print;
  const int     r = s ▷ equalsString(expected);
  s ▷ free;
  return r;
 }

static int printContains_$Tag                                                   // Check the print of an $ parse tree starting at the specified tag contains the expected string
 (const $Tag    tag,                                                            // Starting tag
  const char *  expected)                                                       // Expected string
 {ReadOnlyBytes s = tag ▷ print;
  const int     r = s ▷ containsString(expected);
  s ▷ free;
  return r;
 }

static int printContains_$Parse                                                 // Check the print of an $ parse tree contains the expected string.
 (const $Parse  xml,                                                            // $ parse tree
  const char *  expected)                                                       // Expected string
 {ReadOnlyBytes s = xml ▷ print;
  const int     r = s ▷ containsString(expected);
  s ▷ free;
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

  $Parse x = make$ParseFromFile(makeFileName(file));

  if (1)
   {ArenaTree e = x.errors;
    assert(x ▷ errors == 2);
    assert(e ▷ printContains("End tag: A does not match start tag: a"));
    assert(e ▷ printContains("Ignoring text at end"));
   }

  $Tag b = x ▷ findFirstTag("b"), d = x ▷ findFirstTag("d");

  assert( b ▷ valid);  assert(b ▷ tagNameIs("b"));
  assert(!d ▷ valid);

  x ▷ free;
 }

void test1()                                                                    //Tprint
 {char file[128] =  "/home/phil/c/z/xml/samples/foreword.dita";
  if (!develop()) strcpy(file, "c/z/xml/samples/foreword.dita");

  $Parse x = make$ParseFromFile(makeFileName(file));

  $Tag p = x ▷ findFirstTag("p"), c = x ▷ findFirstTag("conbody");

  assert( p ▷ valid);
  assert( p ▷ printContains("<p>DITA represents a fundamental shift"));
  assert(!p ▷ printContains("Define the markup that"));
  assert( p ▷ tagNameIs("p"));

  assert( c ▷ valid);
  assert( c ▷ printContains("<p>DITA represents a fundamental shift"));
  assert( c ▷ printContains("Define the markup that"));
  assert( c ▷ tagNameIs("conbody"));

  x ▷ free;
 }

void test2()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {char *file = "/home/phil/c/z/xml/validation/validation.xml";
  $Parse xml = make$ParseFromFile(makeFileName(file));

  $Tag possibilities = xml ▷ findFirstChild("possibilities");
  assert(possibilities ▷ valid);

  $Tag array = possibilities ▷ findFirstTag("array");
  assert(array ▷ valid);

  if (1)                                                                        // Possibilities array
   {char buffer[256];
    size_t i = 0;
    $fe(element, array)
     {sprintf(buffer, "%lu", i);
      ArenaRedBlackTreeFound f = xml.validation ▷ add(buffer);

      $fe(hash, element)
       {$fe(key, hash)
         {char * const name = key ▷ getTag;
          if (strcmp(name, "text")) f.node ▷ add(key ▷ getTag);
         }
       }
      ++i;
     }
   }

  xml ▷ free;
 }

int main(void)                                                                  // Run tests
// {void (*tests[])(void) = {test0, test1, test2, 0};
 {void (*tests[])(void) = {test2, 0};
  run_tests("$", 1, tests);
  return 0;
 }

#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
