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
#include <stringBuffer.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Tree.

char $tagName[256];                                                             // Tag name returned by tag

typedef struct $Parse                                                           // $ parse tree
 {const struct ProtoTypes_$Parse *proto;                                        // Prototypes for methods
  FileName          fileName;                                                   // Source file containing $ to be parsed
  ReadOnlyBytes     data;                                                       // $ to be parsed
  ArenaTree         tree;                                                       // Parse tree created from $ being parsed
  ArenaTree         errors;                                                     // List of errors in the xml source - the key of the node is the text of the error message, the extra data after the node is $Error
  ArenaRedBlackTree possibilities;                                              // Possibility sets for each Dita tag
  ArenaRedBlackTree first;                                                      // First set of possibilities for each Dita tag
  ArenaRedBlackTree next;                                                       // Next set of possibilities for each Dita child tag under a given parent tag
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

static $Tag make$Tag_$Parse_ArenaTreeNode                                       //P Make a tag descriptor from a parse tree node holding the tag
 (const $Parse        xml,                                                      // Xml parse tree
  const ArenaTreeNode node)                                                     // Node holding tag
 {$Tag t = new$Tag(xml: xml, node: node);
  return t;
 }

static $Parse make$ParseFromFile                                                // Make a parse $ from the contents of a file
 (FileName            fileName)                                                 // Name of file holding $
 {$Parse              x = new$Parse;
  const ArenaTree     t = x.tree      = makeArenaTree();                        // Parse tree,
  const ArenaTree     e = x.errors    = makeArenaTree();                        // Errors list
                      x.possibilities = makeArenaRedBlackTree();                // Single Step Validation
                      x.first         = makeArenaRedBlackTree();                // First set of possibilities for each tag
                      x.next          = makeArenaRedBlackTree();                // Next set of possibilities for each Dita child tag under a given parent tag
  const FileName      f = x.fileName  = fileName;                               // Name of file containing parse
  const ReadOnlyBytes b = x.data      = makeReadOnlyBytesFromFile(f);           // String to parse
  ArenaTreeNode       P = t ▷ root;                                             // Current parent node

  $Parse error                                                                  // Report an error
   (const char * const p,                                                       // Pointer to position at which the error occurred
    const char *format, ...)                                                    // Message text
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
   {const char * const o = strchr(p, $Open);                                    // Find next open

    if (o)                                                                      // Found next open
     {if (o > p)
       {int allBlank()                                                          // Check whether the text to be saved is all white space. We can recover the position of non whitespace from the offset saved in the data field of the node.
         {for(char * q = p; q < o; ++q) if (!isspace(*q)) return 0;
          return 1;
          }

        if (!allBlank())                                                        // Save the  text as the key of the node
         {const ArenaTreeNode n = t ▷ noden(p, o - p);                          // Create node
          P ▷ putLast(n);                                                       // Save node in parse tree
          P ▷ setData(p - textStart);                                           // Save text offset
         }
       }

      char * const c = strchr(o, $Close);                                       // Save tag: find corresponding close
      if (c)                                                                    // Found closing >
       {ArenaTreeNode save(void)                                                // Save tag as a new node
         {ArenaTreeNode n = t ▷ noden(o, c - o + 1);                            // Create node
          P ▷ putLast(n);                                                       // Save node in parse tree
          P ▷ setData(o - textStart);                                           // Save text offset in parse tree
          return n;
         }

        void end(void)                                                          // End tag
         {const size_t N = sizeof($tagName);
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
     {if (!remainderIsWhiteSpace(p)) error(p, "Ignoring text at end\n");
      break;
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
   {const ArenaTreeNode r = t ▷ root,     f = r ▷ first;
    ArenaTreeContent *  a = r ▷ content, *b = f ▷ content;
    *a = *b;
    ArenaTreefe(c, r) c ▷ setUp(r);
   }

  b ▷ free;                                                                     // Free mapped input file as we can now reconstruct it from the parse tree

  return x;
 } // make$ParseFromFile

static $Parse make$ParseFromString                                              // Make a parse $ from a string.
 (const char * const string)                                                    // String of xml
 {const FileName f = makeFileNameTemporaryWithContent("zzz.xml", string, 0);    // Create a temporary file
  const $Parse   x = make$ParseFromFile(f);                                     // Parse
  f ▷ unlink;
  return x;
 }

static void free_$Parse                                                         // Free an xml parse
 (const $Parse x)                                                               // $ descriptor
 {const ArenaTree t = x.tree, e = x.errors;
  const ArenaRedBlackTree p = x.possibilities, f = x.first, n = x.next;
  t ▷ free; e ▷ free; p ▷ free; f ▷ free; n ▷ free;
 }

static size_t errors_$Parse                                                     // Number of errors encountered while creating an $ parse tree.
 (const $Parse x)                                                               // $ descriptor
 {return x.errors ▷ count;
 }

static char  * parse$TagName                                                    // Get the tag name from an $ tag
 (const char * const tagString)                                                 // String representation of a tag
 {const char * const p = tagString;
  if (*p == $Open)                                                              // Tag
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

static char * tagName_$Tag                                                      // Get the tag name from a node in the $ parse tree. The tag name is returned in a reused static buffer that the caller should not free.
 (const $Tag tag)                                                               // Tag
 {return parse$TagName(tag.node ▷ key);
 }

static int tagNameEquals_$Tag_string                                            // Check the name of a tag.
 (const $Tag         tag,                                                       // Tag
  const char * const expected)                                                  // Expected name of tag
 {return !strcmp(tag ▷ tagName, expected);
 }

static char * tagString_$Tag                                                    //V Get the entire tag from a node in the $ parse tree.
 (const $Tag tag)                                                               // Tag
 {return tag.node ▷ key;
 }

static int tagStringEquals_$Tag_string                                          // Check the string of a tag.
 (const $Tag         tag,                                                       // Tag
  const char * const expected)                                                  // Expected name of tag
 {return !strcmp(tag ▷ tagString, expected);
 }

//D1 Navigation                                                                 // Navigate through an $ parse tree.

#define $fe( child, parent) for($Tag child = parent ▷ first; child ▷ valid; child = child ▷ next)  // Each child in a parent from first to last
#define $fer(child, parent) for($Tag child = parent ▷ last;  child ▷ valid; child = child ▷ prev)) // Each child in a parent from last to first

static $Tag first_$Tag                                                          // Return the first child tag under the specified parent tag.
 (const $Tag parent)                                                            // Parent tag
 {return new $Tag(xml: parent.xml, node: parent.node ▷ first);
 }
duplicate s/first/last/  s/first/next/ s/first/prev/ s/first/parent/

static $Tag first_$Parse                                                        // Return the first child tag in the specified $ parse tree.
 (const $Parse xml)                                                             // Parent tag
 {return new $Tag(xml: xml, node: xml.tree ▷ first);
 }
duplicate s/first/last/

//D1 Location                                                                   // Check the current location in the $ parse tre

static int isFirst_$Tag                                                         // Check that the specified tag is first under its parent
 (const $Tag tag)                                                               // Tag
 {if (!tag ▷ valid) return 1;                                                   // The root tag is always first
  const $Tag parent = tag ▷ parent, f = parent ▷ first;
  return f ▷ equals(tag);
 }
duplicate s/first/last/g,s/First/Last/g

//D1 Text methods                                                               // Methods that operate on text tags

static char * text_string_$Tag                                                  // Return the text of a text tag if we are on a text tag.
 (const $Tag tag)                                                               // Tag
 {char * const k = tag ▷ tagString;
  return *k == $Open ? 0 : k;                                                   // If the start of the key is not < then it must be text
 }

static int onlyText_$Tag                                                        // Return true if a tag contains just one text element and nothing else
 (const $Tag parent)                                                            // Tag
 {const size_t n = parent ▷ countChildren;
  if (n != 1) return 0;                                                         // If there is more than one tag than there must be a non text tag present to separate the text
  const $Tag first = parent ▷ first;
  return !!first ▷ text;
 }

//D1 Statistics                                                                 // Counts on the tag

static int empty_$Tag                                                           // Check whether the specified parent tag is empty
 (const $Tag parent)                                                            // Parent tag
 {return !parent ▷ countChildren;
 }

static size_t openChildren_$Tag                                                 // Count the nunber of child tags that are open tags
 (const $Tag parent)                                                            // Parent tag
 {size_t n = 0;
  $fe(child, parent) if (child ▷ countChildren > 0) ++n;
  return n;
 }

//D1 Search                                                                     // Search the $ parse tree

static int valid_$Tag                                                           // Check that a tag is valid.
 (const $Tag tag)                                                               // Tag
 {return tag.node.offset;                                                       // A tag is valid unless it is the root node of the arena tree backing the $ parse tree.
 }

static $Tag findFirstChild_$Tag_$Tag_string                                     // Find the first child tag with the specified name under the specified parent tag.
 (const $Tag   parent,                                                          // Parent
  const char * const key)                                                       // Name of the tag to find
 {$fe(child, parent)                                                            // Each child of the parent
   {if (child ▷ tagNameEquals(key)) return child;                               // First child with matching tag
   }
  return new $Tag;
 }

static $Tag findFirstChild_$Tag_$Parse_string                                   // Find the first child tag with the specified name under the root tag of the specified parse tree.
 (const $Parse xml,                                                             // Parent
  const char * const key)                                                       // Name of the tag to find
 {const $Tag root = xml ▷ root;                                                 // Root tag
  return root ▷ findFirstChild(key);                                            // Each child of the parent
 }

static $Tag findFirstTag_$Tag_$Tag_string                                       // Find the first tag with the specified name in the $ parse tree starting at the specified tag.
 (const $Tag   parent,                                                          // Parent tag
  const char * const key)                                                       // Name of the tag to find
 {jmp_buf found;
  $Tag T = new $Tag;                                                            // Tag found if any

  void find(const $Tag tag)                                                     // Check whether the name of the tag matches the specified key
   {if (tag ▷ tagNameEquals(key))
     {T = tag;                                                                  // Found
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) parent ▷ by(find);                                        // Search the $ parse tree
  return T;
 }

static $Tag findFirstTag_$Tag_$Parse_string                                     // Find the first tag in an $ parse tree with the specified name.
 (const $Parse       xml,                                                       // Xml parse tree
  const char * const key)                                                       // Name of the tag to find
 {const $Tag t = new $Tag(xml: xml, node: xml.tree ▷ root);
  return t ▷ findFirstTag(key);
 }

static $Tag root_$Parse                                                         // Return the root tag of the specified $ parse tree
 (const $Parse xml)                                                             // $ parse tree
 {return new $Tag(xml: xml, node: xml.tree ▷ root);
 }

static $Tag root_$Tag                                                           // Return the root tsg of the $ parse tree containing the specified tag.
 (const $Tag tag)                                                               // Tag
 {return new $Tag(xml: tag.xml, node: tag.xml.tree ▷ root);
 }

static int equals_$Tag_$Tag                                                     // Confirm that two tags refer to the same text location in the source document.
 (const $Tag a,                                                                 // First tag
  const $Tag b)                                                                 // Second tag
 {return a.node.offset == b.node.offset;
 }

//D1 Traverse                                                                   // Traverse the $ parse tree.

static void by_$Tag_sub                                                         // Traverse the $ parse tree rooted at the specified tag in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const $Tag tag,                                                               // Starting tag
  void (* const function) (const $Tag tag))                                     // Function to call on each tag
 {void f(const ArenaTreeNode node)
   {function(new $Tag(xml: tag.xml, node: node));
   }
  tag.node ▷ by(f);
 }

static void by_$Parse_sub                                                       // Traverse the $ parse tree in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const $Parse xml,                                                             // $ parse tree
  void (* const function) (const $Tag tag))                                     // Function to call on each tag
 {const $Tag root = xml ▷ root;
  root ▷ by(function);
 }

static  size_t countChildren_size_$                                             // Count the number of tags at the first level in an xml parse tree.
 (const $Parse xml)                                                             // $ parse
 {const ArenaTreeNode root = xml.tree ▷ root;
  return root ▷ countChildren - 1;                                              // Count the level immediately below the root tag of which there is always just one
 }

static  size_t countChildren_size_$Tag                                          // Count the number of child tags directly under a parent tag
 (const $Tag parent)                                                            // Parent tag
 {return parent.node ▷ countChildren;
 }

static  size_t count_size_$                                                     // Count the number of tags in an $ parse tree
 (const $Parse x)                                                               // $ parse tree
 {return x.tree ▷ count;
 }

static  size_t count_size_$Tag                                                  // Count the number of tags under the specified tag in an $ parse tree
 (const $Tag t)                                                                 // Parent tag
 {return t.node ▷ count;
 }

//D1 Wrap and Unwrap                                                            // Wrap and unwrap nodes

static void wrap_$Tag_string                                                    // Wrap a specified tag with a new tag
 (const $Tag         tag,                                                       // Tag
  const char * const string)                                                    // Wrapper without the leading < or trailing or >                                                                              //const char * void (* const function) (const $Tag tag))
 {char s[strlen(string)+4], *p = s;
  *p++ = $Open;
   p   = stpcpy(p, string);
  *p++ = $Close;
  *p   = 0;

  tag.node ▷ wrap(s);
 }

//D1 Print                                                                      // Print an $ parse tree starting at the specified tag.

static ReadOnlyBytes prettyPrint_readOnlyBytes_$Tag                             // Print the $ parse tree starting at the specified tag with additional spacing between tags to make the tree easier to read.
 ($Tag tag)                                                                     // Starting tag
 {StringBuffer p = makeStringBuffer();

  void print(const $Tag parent, int depth)                                      // Print the specified parent and its children
   {void space()                                                                // Format with white space
     {p ▷ add("\n");
      for(int i = 0; i < depth; ++i) p ▷ add(" ");
     }
    void open()                                                                 // Add close tag
     {p ▷ add(parent ▷ tagString);
     }
    void close()                                                                // Add close tag
     {p ▷ addFormat("%c%c%s%c", $Open, $Slash, parent ▷ tagName, $Close);
     }
    if (parent ▷ empty) open();                                                 // Parent is empty
    else if (!parent ▷ openChildren)                                            // Parent contains only text and singletons
     {open();
      $fe(child, parent) p ▷ add(child ▷ tagString);
      close();
     }
    else                                                                        // Parent contains open tags
     {space(); open();
      $fe(child, parent) print(child, depth+1);
      space(); close();
      if (!parent ▷ isLast) space();                                              // If we are last the next end tag will be offset anyway
     }
   }

  print(tag, 0);
  const ReadOnlyBytes r = p ▷ readOnlyBytes;
  p ▷ free;
  return r;
 }

static ReadOnlyBytes prettyPrint_readOnlyBytes_$                                // Print the $ parse tree with additional spacing between tags to make the tree easier to read.
 ($Parse xml)                                                                   // $ parse tree
 {const $Tag root = xml ▷ root;
  return root ▷ prettyPrint;
 }

static int prettyPrintsAs_int_$_string                                          // Check that the $ parse tree prints as expected
 ($Parse xml,                                                                   // $ parse tree
  const char * const expected)                                                  // Expected pretty print
 {const ReadOnlyBytes r = xml ▷ prettyPrint;
  const int result =  r ▷ equalsString(expected);

  if (!result)                                                                   // Strings match
   {for(size_t i = 0; i < r.length; ++i)
     {char a = *(r.data+i), b = *(expected+i);
      if (a != b)
       {say("Strings differ in prettyPrintsAs"
            " at position: %lu on characters:  %c and %c\n", i, a, b);
        r ▷ free;
        return 0;
       }
     }
   }

  r ▷ free;
  return 1;
 }

static void prettyPrintAssert_$Tag                                              // Pretty print the $ parse tree starting at the specified tag as an assert statement
 (const $Tag         tag,                                                       // Starting tag
  const char * const variable)                                                  // The name of the variable preceding this call
 {const FileName f = makeFileName("/home/phil/c/z/z/zzz.txt");
  if (! f ▷ size) return;                                                       // File does not exist - create it by hand to make this function work
  const ReadOnlyBytes r = tag ▷ prettyPrint;
  StringBuffer        s = makeStringBuffer();
  ArenaTree           t = r ▷ splitNewLine;
  s ▷ addFormat("assert(prettyPrintsAs_int_$_string(%s,\n", variable);
  ArenaTreefe(line, t)
   {const char * const k = line ▷ key;
    const size_t N = strlen(k);
    char l[N]; strncpy(l, k, N); if (l[N-1] == '\n') l[N-1] = 0;                // Remove any end of line
    s ▷ addDoubleQuote;
    s ▷ add(l);
    s ▷ addQuotedNewLine;
    s ▷ addDoubleQuote;
    s ▷ addNewLine;
   }
  s ▷ add("));\n\n");
  ReadOnlyBytes R = s ▷ readOnlyBytes;
  R ▷ writeFile(f);
  f ▷ free; r ▷ free; R ▷ free; t ▷ free;
 }

static void prettyPrintAssert_$_string                                          // Pretty print the $ parse tree as an assert statement
 (const $Parse       xml,                                                       // $
  const char * const variable)                                                  // The name of the variable preceding this call
 {const $Tag t = xml ▷ root;
  t ▷ prettyPrintAssert(variable);
 }

static ReadOnlyBytes print_readOnlyBytes_$Tag                                   // Print the parse tree as a string starting at the specified tag.
 (const $Tag tag)                                                               // Starting tag
 {const ArenaTreeNode n = tag.node;
  return              n ▷ print;
 }

static ReadOnlyBytes print_ReadOnlyBytes_$Parse                                 // Print an entire $ parse tree as a string.
 (const $Parse        xml)                                                      // $ parse tree
 {const ArenaTree     t = xml.tree;
  const ArenaTreeNode r = t ▷ root;
  return              r ▷ print;
 }

static int printsAs_$Tag                                                        // Check the print of an $ parse tree starting at the specified tag is as expected.
 (const $Tag          tag,                                                      // Starting tag
  const char *  const expected)                                                 // Expected string
 {const ReadOnlyBytes s = tag ▷ print;
  const int       r = s ▷ equalsString(expected);
  s ▷ free;
  return r;
 }

static int printsAs_$Parse                                                      // Check the print of an $ parse tree is as expected.
 (const $Parse  xml,                                                            // $ parse tree
  const char *  expected)                                                       // Expected string
 {const ReadOnlyBytes s = xml ▷ print;
  const int       r = s ▷ equalsString(expected);
  s ▷ free;
  return r;
 }

static int printContains_$Tag                                                   // Check the print of an $ parse tree starting at the specified tag contains the expected string
 (const $Tag    tag,                                                            // Starting tag
  const char *  expected)                                                       // Expected string
 {const ReadOnlyBytes s = tag ▷ print;
  const int       r = s ▷ containsString(expected);
  s ▷ free;
  return r;
 }

static int printContains_$Parse                                                 // Check the print of an $ parse tree contains the expected string.
 (const $Parse  xml,                                                            // $ parse tree
  const char *  expected)                                                       // Expected string
 {const ReadOnlyBytes s = xml ▷ print;
  const int       r = s ▷ containsString(expected);
  s ▷ free;
  return r;
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

static int develop()                                                            // Test whether we are local or on github
 {return !strcmp(getenv("HOME"), "/home/phil");
 }

void test0()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer //Terrors //Tmake$ParseFromFile
 {$Parse x = make$ParseFromString("<a>aa<b>bb<c/>cc</b>dd<d><e/><f>F</f><g/></d><h/><i/></A>h");

  assert(x ▷ prettyPrintsAs("\n"
"<a>aa<b>bb<c/>cc</b>dd\n"
" <d><e/><f>F</f><g/>\n"
" </d>\n"
" <h/><i/>\n"
"</a>\n"));

  if (1)
   {ArenaTree e = x.errors;
    assert(x ▷ errors == 2);
    assert(e ▷ printContains("End tag: A does not match start tag: a"));
    assert(e ▷ printContains("Ignoring text at end"));
   }

  $Tag b = x ▷ findFirstTag("b"); assert(b ▷ valid); assert(b ▷ tagNameEquals("b")); assert(!b ▷ tagNameEquals("B"));
  $Tag c = x ▷ findFirstTag("c"); assert(c ▷ valid); assert(c ▷ tagNameEquals("c")); assert(!c ▷ tagNameEquals("C"));
  $Tag d = x ▷ findFirstTag("d"); assert(d ▷ valid); assert(d ▷ tagNameEquals("d")); assert(!d ▷ tagNameEquals("D"));
  $Tag e = d ▷ first;             assert(e ▷ valid); assert(e ▷ tagNameEquals("e")); assert(!e ▷ tagNameEquals("E"));
  $Tag g = d ▷ last;              assert(g ▷ valid); assert(g ▷ tagNameEquals("g")); assert(!g ▷ tagNameEquals("G"));
  $Tag f = g ▷ prev;              assert(f ▷ valid); assert(f ▷ tagNameEquals("f")); assert(!f ▷ tagNameEquals("F"));
  $Tag h = x ▷ findFirstTag("h"); assert(h ▷ valid); assert(h ▷ tagNameEquals("h")); assert(!h ▷ tagNameEquals("H"));
  $Tag i = x ▷ findFirstTag("i"); assert(i ▷ valid); assert(i ▷ tagNameEquals("i")); assert(!i ▷ tagNameEquals("I"));

  assert(e ▷ equals(f ▷ prev));

  $Tag B = b ▷ first;             assert(B ▷ valid); assert(B ▷ tagNameEquals("text")); assert(B ▷ tagStringEquals("bb"));

  x ▷ free;
 }

void test1()                                                                    //Tfirst //Tlast //Tprev //Tnext //Tequals //Tcount //TcountChildren //TfindFirstTag //TfindFirstChild //Tmake$ParseFromString //Tparse$TagName //TtagName //TtagNameEquals //Tvalid //TtagString //TtagStringEquals //Tparent //Troot //Twrap
 {$Parse x = make$ParseFromString
   ("<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j></j></a>");

  assert(!x ▷ errors);
//        x ▷ prettyPrintAssert("x");
  assert(prettyPrintsAs_int_Xml_string(x,
"\n"
"<a>\n"
" <b><c/>\n"
"  <d><e/>e<f/>f<g>g</g>\n"
"  </d>\n"
"  <h>h</h>\n"
" </b>\n"
" <i/>i<j>\n"
"</a>\n"
));

  $Tag a = x ▷ root;                assert(!a ▷ valid); assert(a ▷ tagNameEquals("a"));
  $Tag b = x ▷ findFirstTag  ("b"); assert( b ▷ valid); assert(b ▷ tagNameEquals("b")); assert(a ▷ equals(b ▷ parent));
  $Tag c = x ▷ findFirstTag  ("c"); assert( c ▷ valid); assert(c ▷ tagNameEquals("c")); assert(b ▷ equals(c ▷ parent));
  $Tag d = b ▷ findFirstChild("d"); assert( d ▷ valid); assert(d ▷ tagNameEquals("d")); assert(b ▷ equals(d ▷ parent));
  $Tag e = d ▷ findFirstChild("e"); assert( e ▷ valid); assert(e ▷ tagNameEquals("e")); assert(d ▷ equals(e ▷ parent));
  $Tag f = d ▷ findFirstChild("f"); assert( f ▷ valid); assert(f ▷ tagNameEquals("f")); assert(d ▷ equals(f ▷ parent));
  $Tag g = d ▷ findFirstChild("g"); assert( g ▷ valid); assert(g ▷ tagNameEquals("g")); assert(d ▷ equals(g ▷ parent));
  $Tag h = b ▷ findFirstChild("h"); assert( h ▷ valid); assert(h ▷ tagNameEquals("h")); assert(b ▷ equals(h ▷ parent));
  $Tag i = x ▷ findFirstTag  ("i"); assert( i ▷ valid); assert(i ▷ tagNameEquals("i")); assert(a ▷ equals(i ▷ parent));
  $Tag j = x ▷ findFirstTag  ("j"); assert( j ▷ valid); assert(j ▷ tagNameEquals("j")); assert(a ▷ equals(j ▷ parent));

  assert(b ▷ equals(x ▷ first));
  assert(c ▷ equals(b ▷ first));
  assert(h ▷ equals(b ▷ last));
  assert(j ▷ equals(x ▷ last));

  assert(h ▷ equals(d ▷ next));
  assert(c ▷ equals(d ▷ prev));

  assert(x ▷ count         == 14);
  assert(x ▷ countChildren == 3);

  assert(b ▷ count         == 10);
  assert(b ▷ countChildren == 3);

  assert(!strcmp(parse$TagName(b.node ▷ key), "b"));
  assert(!strcmp(b ▷ tagName,                 "b"));

  assert(!strcmp(b ▷ tagString, "<b>" ));
  assert(!strcmp(c ▷ tagString, "<c/>"));
  assert(b ▷ tagStringEquals(   "<b>" ));
  assert(c ▷ tagStringEquals(   "<c/>"));

  $Tag H = h ▷ first;
  assert(!strcmp(H.node ▷ key,     "h"));
  assert(!strcmp(H      ▷ tagName, "text"));

  assert(!strcmp(  H ▷ tagString,         "h" ));
  assert(          H ▷ tagStringEquals(   "h"));

  assert(a ▷ equals(b ▷ parent));

  f ▷ wrap("F id='1'");
//x ▷ prettyPrintAssert("x");
assert(prettyPrintsAs_int_Xml_string(x,
"\n"
"<a>\n"
" <b><c/>\n"
"  <d><e/>e<F id='1'><f/></F>f<g>g</g>\n"
"  </d>\n"
"  <h>h</h>\n"
" </b>\n"
" <i/>i<j>\n"
"</a>\n"
));

  x ▷ free;
 }

void test2()                                                                    //Tprint
 {char file[128] =  "/home/phil/c/z/xml/samples/foreword.dita";
  if (!develop()) strcpy(file, "c/z/xml/samples/foreword.dita");

  $Parse x = make$ParseFromFile(makeFileName(file));

  $Tag p = x ▷ findFirstTag("p"), c = x ▷ findFirstTag("conbody");

  assert( p ▷ valid);
  assert( p ▷ printContains("<p>DITA represents a fundamental shift"));
  assert(!p ▷ printContains("Define the markup that"));
  assert( p ▷ tagNameEquals("p"));

  assert( c ▷ valid);
  assert( c ▷ printContains("<p>DITA represents a fundamental shift"));
  assert( c ▷ printContains("Define the markup that"));
  assert( c ▷ tagNameEquals("conbody"));

  x ▷ free;
 }

void test3()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {char *file =       "/home/phil/c/z/xml/validation/validation.xml";
  if (!develop()) return;                                                       // Does not work on gitHub - possibly out of memory or Cpu?
  $Parse xml = make$ParseFromFile(makeFileName(file));

  $Tag possibilities = xml ▷ findFirstChild("possibilities");                   // The possibilities possibilitiesArray assigns a number to each set of possible tags
  assert(possibilities ▷ valid);

  $Tag possibilitiesArray = possibilities ▷ findFirstChild("array");
  assert(possibilitiesArray ▷ valid);
  size_t possibilitiesCount = possibilitiesArray ▷ countChildren;
  assert(possibilitiesCount == 383);

  $Tag first = xml ▷ findFirstChild("first");                                   // The tags that can come first under a specified parent tag
  assert(first ▷ valid);

  $Tag firstHash = first ▷ findFirstChild("hash");
  assert(firstHash ▷ valid);
  assert(firstHash ▷ countChildren == 374);

  $Tag next = xml ▷ findFirstChild("next");                                     // The tags that can come next under a specified parent tag
  assert(next ▷ valid);

  $Tag nextParentHash = next ▷ findFirstChild("hash");
  assert(nextParentHash ▷ valid);
  assert(nextParentHash ▷ countChildren == 374);

  if (1)                                                                        // Possibilities possibilitiesArray
   {char buffer[256];
    ArenaRedBlackTreeNode pa[possibilitiesCount];
    assert(xml.possibilities ▷ count == 0);
    size_t i = 0;

    $fe(element, possibilitiesArray)                                            // Load possibilities
     {sprintf(buffer, "%lu", i);                                                // Offset as string for use as a key
      ArenaRedBlackTreeNode f = xml.possibilities ▷ add(buffer);                // Possibility set number
      assert(f ▷ valid);
      pa[i] = f;                                                                // Cache node offset

      $fe(hash, element)                                                        // Array of hashes
       {$fe(hashKey, hash)                                                      // Keys in each hash
         {char * name = hashKey ▷ tagName;
          if (strcmp(name, "text")) f ▷ add(hashKey ▷ tagName);
         }
       }
      ++i;
     }

    $fe(firstTag, firstHash)                                                    // Load first tags
     {$Tag value = firstTag ▷ first;                                            // Value tag
      char * t = value ▷ tagString;                                             // Possibility number as string
      size_t a = strtol(t, 0, 0);                                               // Possibility number
      ArenaRedBlackTreeNode f = xml.first ▷ add(firstTag ▷ tagName);            // First possibilities for this tag
      f ▷ setData(pa[a].offset);                                                // The first sub tag possibilities for this tag
     }

    $fe(nextParentTag, nextParentHash)                                          // Load next parents
     {char * tag = nextParentTag ▷ tagName;                                     // Next parent tag
      ArenaRedBlackTreeNode f = xml.next ▷ add(tag);                            // Add next parent tag to next tree
      assert(f ▷ valid);

      $Tag nextChildHash = nextParentTag ▷ findFirstChild("hash");

      $fe(nextChildTag, nextChildHash)                                          // Load next children
       {$Tag value = nextChildTag ▷ first;                                      // Value tag
        char * t = value ▷ tagString;                                           // Possibility number as string
        size_t a = strtol(t, 0, 0);                                             // Possibility number
        ArenaRedBlackTreeNode F = f ▷ add(nextChildTag ▷ tagName);              // Next possibilities for the child tag
        assert(F ▷ valid);
        F ▷ setData(pa[a].offset);                                              // The next possibilities for this child under this parent
       }
     }
   }
  assert(xml.possibilities ▷ b2SumW8 == 199);
  assert(xml.first         ▷ b2SumW8 ==  19);
  assert(xml.next          ▷ b2SumW8 ==  19);

  assert(xml.possibilities ▷ used    == 192344);
  assert(xml.first         ▷ used    ==  14128);
  assert(xml.next          ▷ used    == 330562);


  if (1)                                                                        // Check that 'appendix' is a possible first element of 'appendices'.
   {ArenaRedBlackTreeNode f = xml.first ▷ locate("appendices");
                   assert(f ▷ valid);
                   assert(f ▷ getData == 4771);                                 // Offset to set of first possibilities for Appendices tag

    ArenaRedBlackTreeNode  n = xml.possibilities ▷ nodeFromOffset(f ▷ getData); // Create a node representing the Appendices Tag first possibilities set
    assert(n ▷ equalsString("8"));                                              // Appendices is tag 8
    ArenaRedBlackTreeNode p = n ▷ locate("appendix");                           // Check whether Appendix is in the set of first tag possibilities for Appendices
                   assert(p ▷ valid);                                           // Appendix is in the first tag set of Appendices
   }

  if (1)                                                                        // Check that 'dd' can follow child 'dt' under parent 'dlentry'.
   {ArenaRedBlackTreeNode F = xml.next ▷ ll("dlentry", "dt", 0);                // Parent + child
                   assert(F ▷ valid);
    ArenaRedBlackTreeNode n = xml.possibilities ▷ nodeFromOffset(F ▷ getData);  // Next child possibilities under parent
    ArenaRedBlackTreeNode p = n ▷ locate("dd");
                   assert(p ▷ valid);                                           // Check whether 'dd' can follow 'dt' under 'dlentry'
   }

  xml ▷ free;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, 0};
//{void (*tests[])(void) = {test0, 0};
  run_tests("$", 1, tests);
  return 0;
 }

#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
