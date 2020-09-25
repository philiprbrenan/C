//------------------------------------------------------------------------------
// Parse Xml.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <ctype.h>
#include <arenaTree.c>
#include <arenaList.c>
#include <stringBuffer.c>
#include <utilities.c>

#ifndef $_included
#define $_included

//D1 Structures                                                                 // Structures describing an Arena Tree.

char $tagName[256];                                                             // Tag name return area as xml tags are short

typedef struct $Parse                                                           // $ parse tree
 {const struct ProtoTypes_$Parse *proto;                                        // Prototypes for methods
  const StringBuffer      fileName;                                             // Source file containing $ to be parsed or zero if there is no file associated with the parse
  const ArenaList         tree;                                                 // Parse tree created from $ being parsed
  const ArenaList         errors;                                               // List of errors in the xml source - the key of the node is the text of the error message, the extra data after the node is $Error
 } $Parse;

typedef struct $Validate                                                        // $ single step validation
 {const struct ProtoTypes_$Validate *proto;                                     // Prototypes for methods
  const ArenaTree         possibilities;                                        // Possibility sets for each Dita tag
  const ArenaTree         first;                                                // First set of possibilities for each Dita tag
  const ArenaTree         next;                                                 // Next set of possibilities for each Dita child tag under a given parent tag
 } $Validate;

typedef struct $Tag                                                             // Tag in an $ parse tree
 {const struct ProtoTypes_$Tag *proto;                                          // Prototypes for methods
  const $Parse            xml;                                                  // $ parse tree
  ArenaListNode           node;                                                 // Arena tree node in the xml parse tree that contains this tag
 } $Tag;

#include <$$_prototypes.h>
#define makeLocalCopyOf$TagString(string,stringLength,tag) const size_t stringLength = content_ArenaListNode(tag.node)->length; char string[stringLength+1]; string[stringLength] = 0; memcpy(string, key_pointer_ArenaListNode(tag.node), stringLength); // Copy the key and the length of the key of the specified node to the stack.

#define $Open     '<'                                                           // Important $ characters
#define $Close    '>'
#define $Slash    '/'
#define $Question '?'
#define $Exclaim  '!'

static $Tag make$Tag_$Parse_ArenaListNode                                       //P Make a tag descriptor from a parse tree node holding the tag
 (const $Parse        xml,                                                      // Xml parse tree
  const ArenaListNode node)                                                     // Node holding tag
 {return new $Tag(xml: xml, node: node);
 }

static $Parse make$ParseFromFile                                                // Make a parse $ from the contents of a file
 (const char * const sourceFileName)                                            // Name of file holding $ to be parsed
 {$Parse p;                                                                     // The resulting parse

  ssize_t reader(char * location, size_t length)                                // Function to process the mapped file. Return non negative on success, negative on failure: this value will be returned to the caller.
   {$Parse q = make$ParseFromString(sourceFileName, location, length);          // Parse file contents
    memcpy((void *)&p, (void *)&q, sizeof($Parse));                             // Temporarily subvert the const settings
    return 1;
   }

  readFile(sourceFileName, reader);                                             // Read the source and parse it

  return p;
 }

static $Validate make$Validate()                                                // Make an $ single step validator
 {possibilities ◁ makeArenaTree();                                              // Single Step Validation
  first         ◁ makeArenaTreeWithWidth(sizeof(size_t));                       // First set of possibilities for each tag
  next          ◁ makeArenaTreeWithWidth(sizeof(size_t));                       // Next set of possibilities for each Dita child tag under a given parent tag
  $Validate valid = {possibilities: possibilities,
    first: first, next: next, proto: &ProtoTypes_$Validate};
  return valid;
 }

static $Parse make$ParseFromString                                              // Make a parse $ from the contents of a file presented as a string
 (const char * const sourceFileName,                                            // Name of file holding $ or zero if no file is associated with this source
  char       * const source,                                                    // Source text to be parsed
  const size_t       length)                                                    // Length of source text to be parsed
 {sourceFN      ◁ makeStringBufferFromString(sourceFileName ? : "");            // File to parse
  parseTree     ◁ makeArenaList();                                              // Parse tree,
  errors        ◁ makeArenaListWithWidth(sizeof(size_t));                       // Errors list with location of error
  $Parse xml    = {fileName: sourceFN, tree: parseTree,                         // Create $ descriptor
                   errors: errors, proto: &ProtoTypes_$Parse};

  currentParent ◀ parseTree ▷ root;                                             // Current parent node

  $Parse error                                                                  // Report an error message adding the file name involved on the following line
   (const char * const p,                                                       // Pointer to position at which the error occurred
    const char * const format, ...)                                             // Message text
   {va_list va;

    m ◁ makeStringBuffer();                                                     // Build the error message
    o ◁ p - source;                                                             // Offset of the error

    va_start(va, format); m ▷ addVaFormat(format, va); va_end(va);              // Format error message in a buffer of adequate size

    m ▷ add      ("\n  File     : "); m ▷ addStringBuffer(sourceFN);            // Add file name
    m ▷ addFormat("\n  At offset: %lu\n", o);                                   // Add offset in file
    makeLocalCopyOfStringBuffer(M, L, m);                                       // The error message

    n ◁ errors ▷ node(M, L);                                                    // Save the error message
    n ▷ putTreeLast;                                                            // Add the error to the error list
    *(size_t *)(n ▷ data) = o;                                                  // Save offset of error

    m ▷ free;                                                                   // Free string buffer

    return xml;
   } // error

  if  (*source != $Open) return error(0, "$ must start with: %c", $Open);       // Insist that the first character is <

  int remainderIsWhiteSpace(const char *p)                                      // Find the next non space character in a zero terminated string
   {for(; *p; ++p) if (!isspace(*p)) return 0;                                  // Non white space
    return 1;                                                                   // Its all white sace
   } // remainderIsWhiteSpace

  assert(!source[length]);                                                      // Check we have a trailing zero so that the following loop terminates
  for(char * p = source; *p;)                                                   // Break out tags and text
   {o ◁ strchr(p, $Open);                                                       // Find next open

    if (o)                                                                      // Found next open
     {if (o > p)
       {int allBlank()                                                          // Check whether the text to be saved is all white space. We can recover the position of non whitespace from the offset saved in the data field of the node.
         {for(const char * q = p; q < o; ++q) if (!isspace(*q)) return 0;
          return 1;
          }

        if (!allBlank())                                                        // Save the  text as the key of the node
         {n ◁ parseTree ▷ node(p, o - p);                                       // Create node
          currentParent ▷ putLast(n);                                           // Save node in parse tree
         }
       }
          c ◁ strchr(o, $Close);                                                // Save tag: find corresponding close
      if (c)                                                                    // Found closing >
       {void save(void)                                                         // Save tag as a new node
         {n ◁ parseTree ▷ node(o, c - o + 1);                                   // Create node
          currentParent ▷ putLast(n);                                           // Save node in parse tree
          currentParent = n;
         }
        void saveSingleton(void)                                                // Save singleton
         {const size_t l = c - o + 1;                                           // Length of current tag string
          char b[l+1]; memcpy(b, o, l);                                         // Copy tag string
          b[l-1] = 0; b[l-2] = $Close;                                          // Convert from singleton to opener
          n ◁ parseTree ▷ node(b, l-1);                                         // Create node
          currentParent ▷ putLast(n);                                           // Save node in parse tree
         }

        void end(void)                                                          // End tag
         {lsprintf(a, 256, "%s", parse$TagName(currentParent ▷ key));           // Start tag name
          lsprintf(b, 256, "%s", parse$TagName(o));                             // End tag name

          if (strcmp(a, b))                                                     // Tag name mismatch
           {error(o, "End tag: %s does not match start tag: %s", b, a);
           }

          if (!currentParent ▷ isRoot) currentParent = currentParent ▷ parent;  // Go up one level if possible
          else if (remainderIsWhiteSpace(p)) {}                                 // On root - ignore trailing white space
          else error(o, "Ignoring text beyond closing tag");                    // On root with remaining text
         }

        const char oo = *(o+1), cc = *(c-1);                                    // First character after <, last character before >
        if        (oo == $Slash)     end();                                     // Closing tag
        else if   (oo == $Question) {}                                          // Ignore ?xml
        else if   (oo == $Exclaim)  {}                                          // Ignore !DOCTYPE
        else if   (cc == $Slash)     saveSingleton();                           // Singleton
        else save();                                                            // Opening tag - make current node the current parent
        p = c + 1;                                                              // Start of text
       }

      else return error(o, "Cannot find closing: %c", $Close);                  // No closing > present
     }
    else                                                                        // Check that trailing text is all spaces
     {if (!remainderIsWhiteSpace(p)) error(p, "Ignoring text at end");
      break;
     }
   }

  if (1)                                                                        // Confirm there is exactly one root node
   {root ◁ parseTree ▷ root;
    N ◁ root ▷ countChildren;
    if (N == 0)     return error(source, "No xml root tag found");
    else if (N > 1) return error(source, "More than one root xml tag found");
   }

  if (0)                                                                        // Make the single root xml tag the root of the parse tree
   {r ◁ parseTree ▷ root;
    f ◁ r ▷ first;
    ArenaListContent * const a = r ▷ content, * const b = f ▷ content;
    *a = *b;
    ArenaListfe(c, r) c ▷ setUp(r);
   }

  return xml;
 } // make$ParseFromFile

static $Parse parse$FromString                                                  // Make a parse $ from a string.
 (char * string)                                                                // String of $
 {return make$ParseFromString(0, string, strlen(string));
 }

static void free_$Parse                                                         // Free an $ parse
 (const $Parse x)                                                               // $ descriptor
 {x.tree ▷ free; x.errors ▷ free; x.fileName ▷ free;
 }

static void free_$Validate                                                      // Free an $ validator
 (const $Validate x)                                                               // $ validator
 {x.possibilities ▷ free; x.first ▷ free; x.next ▷ free;
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
       { q ◁ stpncpy($tagName, p+s, f);
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

static size_t tagStringLength_$Tag                                              // Get the length of the entire tag
 (const $Tag tag)                                                               // Tag
 {return tag.node ▷ length;
 }

static int tagStringEquals_$Tag_string                                          // Check the string of a tag.
 (const $Tag         tag,                                                       // Tag
  const char * const expected)                                                  // Expected name of tag
 {return tag.node ▷ equalsString(expected);
 }

//D1 Navigation                                                                 // Navigate through an $ parse tree.

#define $fe( child, parent) for($Tag child = new $Tag(xml: parent.xml, node: parent ▷ first.node); child ▷ valid; child.node.offset = child ▷ next.node.offset)  // Each child in a parent from first to last
#define $fer(child, parent) for($Tag child = new $Tag(xml: parent.xml, node: parent ▷ last.node);  child ▷ valid; child.node.offset = child ▷ prev.node.offset) // Each child in a parent from last to first

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

static int isRoot_$Tag                                                          // Check whether the specified tag is the root tag
 (const $Tag tag)                                                               // Tag
 {return !tag ▷ valid;
 }

static int isFirst_$Tag                                                         // Check that the specified tag is first under its parent
 (const $Tag tag)                                                               // Tag
 {if (tag ▷ isRoot) return 1;                                                   // The root tag is always first
      parent ◁ tag ▷ parent;
  f ◁ parent ▷ first;
  return f ▷ equals(tag);
 }
duplicate s/first/last/g,s/First/Last/g

//D1 Text methods                                                               // Methods that operate on text tags

static int isText_$Tag                                                          // Check whether we are on a text element
 (const $Tag tag)                                                               // Tag
 {const char * const c = tag ▷ tagString;
  return c[0] != $Open;
 }

static int onlyText_$Tag                                                        // Return true if a tag contains just one text element and nothing else
 (const $Tag parent)                                                            // Tag
 {n ◁ parent ▷ countChildren;
  if (n != 1) return 0;                                                         // If there is more than one tag than there must be a non text tag present to separate the text
  f ◁ parent ▷ first;
  return f ▷ isText;
 }

//D1 Statistics                                                                 // Counts on the tag

static int empty_$Tag                                                           // Check whether the specified parent tag is empty
 (const $Tag parent)                                                            // Parent tag
 {return !parent ▷ countChildren;
 }

static int isTag                                                                // Check that the tag is a tag rather than text
 (const $Tag tag)                                                               // Parent tag
 {return !tag ▷ isText;
 }

static int singleton_$Tag                                                       // Check that the tag has no children
 (const $Tag tag)                                                               // Parent tag
 {return tag ▷ empty;
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
 {    root ◁ xml ▷ root;                                                        // Root tag
  f ◁ root ▷ first;
  return f ▷ findFirstChild(key);                                               // Each child of the parent
 }

static $Tag findFirstTag_$Tag_$Tag_string                                       // Find the first tag with the specified name in the $ parse tree starting at the specified tag.
 (const $Tag   parent,                                                          // Parent tag
  const char * const key)                                                       // Name of the tag to find
 {jmp_buf found;
  $Tag T = new $Tag(xml: parent.xml);                                           // Tag found if any

  void find(const $Tag tag)                                                     // Check whether the name of the tag matches the specified key
   {if (tag ▷ tagNameEquals(key))
     {T.node = tag.node;                                                        // Found matching node
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) parent ▷ by(find);                                        // Search the $ parse tree
  return T;
 }

static $Tag findFirstTag_$Tag_$Parse_string                                     // Find the first tag in an $ parse tree with the specified name.
 (const $Parse       xml,                                                       // Xml parse tree
  const char * const key)                                                       // Name of the tag to find
 {jmp_buf found;
  $Tag T = new $Tag(xml: xml);                                                  // Tag found if any

  void find(const $Tag tag)                                                     // Check whether the name of the tag matches the specified key
   {if (tag ▷ tagNameEquals(key))
     {T.node = tag.node;                                                        // Found matching node
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) xml ▷ by(find);                                           // Search the $ parse tree
  return T;
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
 {void f(const ArenaListNode node)
   {function(new $Tag(xml: tag.xml, node: node));
   }
  tag.node ▷ by(f);
 }

static void by_$Parse_sub                                                       // Traverse the $ parse tree in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const $Parse xml,                                                             // $ parse tree
  void (* const function) (const $Tag tag))                                     // Function to call on each tag
 {void f(const ArenaListNode node)
   {function(new $Tag(xml: xml, node: node));
   }
  xml.tree ▷ by(f);
 }

static  size_t countChildren_size_$                                             // Count the number of tags at the first level in an xml parse tree.
 (const $Parse xml)                                                             // $ parse
 {       root ◁ xml.tree ▷ root;                                                // Root node
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

//D1 Edit                                                                       // Change the structure of an $ parse tree.

static void changeName_$Tag                                                     // Change the name of the specified tag.
 (const $Tag         tag,                                                       // Tag
  const char * const newName)                                                   // New name                                                                               //vconst $Tag t)                                                                 // Tag
 {oldName ◁ tag ▷ tagName;                                                      // Old name
  string  ◁ tag ▷ tagString;                                                    // Tag string
  o ◁ strlen(oldName); n ◁ strlen(newName); l ◁ strlen(string);
  if (n > o) printStackBackTraceAndExit(1, "New code needed to deal with the case where the new name is longer than the old name\n");

  char key[l + n + 1], * p = key, * const q = strstr(string, oldName);
  p = stpncpy(p, string, q - string);
  p = stpcpy (p, newName);
  p = stpcpy (p, q + o);
  tag.node ▷ setKey(key, strlen(key));
 }

//D1 Wrap and Unwrap                                                            // Wrap and unwrap nodes

static $Tag wrap_$Tag_string                                                    // Wrap a specified tag with a new tag and return the newly createdf wraping tag.
 (const $Tag         tag,                                                       // Tag
  const char * const string)                                                    // Wrapper without the leading < or trailing or >
 {if (tag ▷ isRoot) return tag;
  char s[strlen(string)+4], *p = s;
  *p++ = $Open;
   p   = stpcpy(p, string);
  *p++ = $Close;
  *p   = 0;

  const ArenaListNode n =  tag.node ▷ wrap(s);
  return new $Tag(xml: tag.xml, node: n);
 }

static $Tag unwrap_$Tag                                                         // Unwrap the specified tag.
 (const $Tag         tag)                                                       // Tag
 {if (!tag ▷ isRoot) tag.node ▷ unwrap;                                         // Cannot unwrap the root tag
  return tag;
 }

//D1 Print                                                                      // Print an $ parse tree starting at the specified tag.

static StringBuffer print_stringBuffer_$Tag                                     // Print the $ parse tree starting at the specified tag with additional spacing between tags to make the tree easier to read.
 (const $Tag tag)                                                               // Starting tag
 {const StringBuffer p = makeStringBuffer();

  void print(const $Tag parent, const int depth)                                // Print the specified parent and its children
   {void open()                                                                 // Add open tag
     {if (parent ▷  isText)
       {p ▷ addn(parent ▷ tagString, parent ▷ tagStringLength);
       }
      else if (parent ▷ singleton)                                              // Write tag with no children as a singleton
       {makeLocalCopyOf$TagString(s, l, parent);
        p ▷ addChar($Open);
        p ▷ addn(s+1, l-2);
        p ▷ addFormat("%c%c", $Slash, $Close);
       }
      else                                                                      // Opener
       {p ▷ addn(parent ▷ tagString, parent ▷ tagStringLength);
       }
     }
    void close()                                                                // Add close tag unless we are on text
     {if (parent ▷ isTag && !parent ▷ empty)
       {p ▷ addFormat("%c%c%s%c", $Open, $Slash, parent ▷ tagName, $Close);
       }
     }

    open();
    $fe(child, parent) print(child, depth+1);
    close();
   }

  print(tag, 0);
  return p;
 }

static StringBuffer print_stringBuffer_$                                        // Print the $ parse tree with additional spacing between tags to make the tree easier to read.
 (const $Parse xml)                                                             // $ parse tree
 {    root ◁ xml ▷ root;
  f ◁ root ▷ first;
  return f ▷ print;
 }

static int printsAs_int_$_string                                                // Check that the $ parse tree prints as expected
 (const $Parse xml,                                                             // $ parse tree
  const char * const expected)                                                  // Expected print
 {s ◁ xml ▷ print;

  if (!s ▷ equalsString(expected))                                              // Strings do not match
   {makeLocalCopyOfStringBuffer(S, N, s);                                       // Local copy
    for(size_t i = 0; i < N; ++i)
     {const char a = *(S+i), b = *(expected+i);
      if (a != b)
       {say("Strings differ at position: %lu on characters:"
            " %d=%c and %d=%c\n", i+1, a, a, b, b);
        say("Have:%s\nWant:%s\n", S, expected);
        say("     ");
        for(int i = 0; i < 10; ++i) say("0 2 4 6 8 ");     say("\n");
        say("     ");
        for(int i = 0; i < 10; ++i) say("%d         ", i); say("\n");
        s ▷ free;
        return 0;
       }
     }
   }

  s ▷ free;
  return 1;
 }
duplicate s/_\$_/_\$Tag_/,s/Parse\sxml/Tag\x20tag/,s/xml/tag/

static void printAssert_$Tag                                                    //P Print the $ parse tree starting at the specified tag as an assert statement
 (const $Tag         tag,                                                       // Starting tag
  const char * const variable)                                                  // The name of the variable preceding this call
 {t ◁ tag ▷ print;
  t ▷ splitLines;
  s ◁ makeStringBuffer();
  s ▷ addFormat("assert(printsAs_int_$_string(%s,\n", variable);
  ArenaListfe(line, t.string)
   {makeLocalCopyOfArenaListKey(l, n, line);
    if (l[n-1] == '\n') l[n-1] = 0;                                             // Remove any end of line
    s ▷ addDoubleQuote;
    s ▷ add(l);
    s ▷ addQuotedNewLine;
    s ▷ addDoubleQuote;
    s ▷ addNewLine;
   }
  s ▷ add("));\n\n");
  s ▷ writeFile("/home/phil/c/z/z/zzz.txt");
  t ▷ free; s ▷ free;
 }

static void printAssert_$_string                                                //P Pretty print the $ parse tree as an assert statement
 (const $Parse       xml,                                                       // $
  const char * const variable)                                                  // The name of the variable preceding this call
 {const $Tag t = xml ▷ root;
  t ▷ printAssert(variable);
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

static int develop()                                                            // Test whether we are local or on github
 {return !strcmp(getenv("HOME"), "/home/phil");
 }

void test0()                                                                    //TnewArenaList //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer //Terrors //Tmake$ParseFromFile //Tempty //TisTag //tText
 {x ◁ parse$FromString("<a>aa<b>bb<c/>cc</b>dd<d><e/><f>F</f><g/></d><h/><i/></A>h");
  ✓ x ▷ printsAs("<a>aa<b>bb<c/>cc</b>dd<d><e/><f>F</f><g/></d><h/><i/></a>");

  if (1)
   {ArenaList e = x.errors;
    ✓ x ▷ errors == 2;
    ✓ e ▷ printContains("End tag: A does not match start tag: a");
    ✓ e ▷ printContains("Ignoring text at end");
   }

  b ◁ x ▷ findFirstTag("b"); ✓b ▷ valid; ✓b ▷ tagNameEquals("b"); ✓!b ▷ tagNameEquals("B");
  c ◁ x ▷ findFirstTag("c"); ✓c ▷ valid; ✓c ▷ tagNameEquals("c"); ✓!c ▷ tagNameEquals("C");
  d ◁ x ▷ findFirstTag("d"); ✓d ▷ valid; ✓d ▷ tagNameEquals("d"); ✓!d ▷ tagNameEquals("D");
  e ◁ d ▷ first;             ✓e ▷ valid; ✓e ▷ tagNameEquals("e"); ✓!e ▷ tagNameEquals("E");
  g ◁ d ▷ last;              ✓g ▷ valid; ✓g ▷ tagNameEquals("g"); ✓!g ▷ tagNameEquals("G");
  f ◁ g ▷ prev;              ✓f ▷ valid; ✓f ▷ tagNameEquals("f"); ✓!f ▷ tagNameEquals("F");
  h ◁ x ▷ findFirstTag("h"); ✓h ▷ valid; ✓h ▷ tagNameEquals("h"); ✓!h ▷ tagNameEquals("H");
  i ◁ x ▷ findFirstTag("i"); ✓i ▷ valid; ✓i ▷ tagNameEquals("i"); ✓!i ▷ tagNameEquals("I");

  ✓!b ▷ empty;
  ✓ b ▷ isTag;
  ✓ c ▷ empty;
  ✓ e ▷ equals(f ▷ prev);

    B ◁ b ▷ first;
  ✓ B ▷ isText;
  ✓!B ▷ isTag;
  ✓ B ▷ tagNameEquals("text");
  ✓ B ▷ tagStringEquals("bb");

  x ▷ free;
 } // test0

void test1()                                                                    //Tfirst //Tlast //Tprev //Tnext //Tequals //Tcount //TcountChildren //TfindFirstTag //TfindFirstChild //Tparse$FromString //Tparse$TagName //TtagName //TtagNameEquals //Tvalid //TtagString //TtagStringEquals //Tparent //Troot //Twrap //Tunwrap //TchangeName //Tsingleton //TtagStringLength //TisText //TisFirst //TisLast //TisRoot
 {char * xml = "<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j></j></a>";
  x ◁ parse$FromString(xml);

  ✓ !x ▷ errors;
//        x ▷ printAssert("x");
  ✓ x ▷ printsAs("<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j/></a>");

  r ◁ x ▷ root;                ✓r ▷ isRoot;
  a ◁ r ▷ first;               ✓a ▷ valid; ✓a ▷ tagNameEquals("a");
  b ◁ x ▷ findFirstTag  ("b"); ✓b ▷ valid; ✓b ▷ tagNameEquals("b"); ✓a ▷ equals(b ▷ parent);
  c ◁ x ▷ findFirstTag  ("c"); ✓c ▷ valid; ✓c ▷ tagNameEquals("c"); ✓b ▷ equals(c ▷ parent);
  d ◁ b ▷ findFirstChild("d"); ✓d ▷ valid; ✓d ▷ tagNameEquals("d"); ✓b ▷ equals(d ▷ parent);
  e ◁ d ▷ findFirstChild("e"); ✓e ▷ valid; ✓e ▷ tagNameEquals("e"); ✓d ▷ equals(e ▷ parent);
  f ◁ d ▷ findFirstChild("f"); ✓f ▷ valid; ✓f ▷ tagNameEquals("f"); ✓d ▷ equals(f ▷ parent);
  g ◁ d ▷ findFirstChild("g"); ✓g ▷ valid; ✓g ▷ tagNameEquals("g"); ✓d ▷ equals(g ▷ parent);
  h ◁ b ▷ findFirstChild("h"); ✓h ▷ valid; ✓h ▷ tagNameEquals("h"); ✓b ▷ equals(h ▷ parent);
  i ◁ x ▷ findFirstTag  ("i"); ✓i ▷ valid; ✓i ▷ tagNameEquals("i"); ✓a ▷ equals(i ▷ parent);
  j ◁ x ▷ findFirstTag  ("j"); ✓j ▷ valid; ✓j ▷ tagNameEquals("j"); ✓a ▷ equals(j ▷ parent);

  ✓ b ▷ equals(a ▷ first);  ✓  b ▷ isFirst;   ✓ !d ▷ isFirst; ✓ !b ▷ isRoot;
  ✓ c ▷ equals(b ▷ first);  ✓  c ▷ singleton;
  ✓ h ▷ equals(b ▷ last);   ✓  h ▷ isLast;    ✓ !d ▷ isLast;
  ✓ j ▷ equals(a ▷ last);

  ✓ h ▷ equals(d ▷ next);
  ✓ c ▷ equals(d ▷ prev);

  ✓ a ▷ count         == 14;

  ✓ a ▷ countChildren == 4;

  ✓ b ▷ count         == 10;
  ✓ b ▷ countChildren == 3;

  ✓ !strcmp(parse$TagName(b.node ▷ key), "b");
  ✓ !strcmp(b ▷ tagName,                 "b");

  ✓ !strncmp(b ▷ tagString, "<b>", b ▷ tagStringLength);
  ✓ !strncmp(c ▷ tagString, "<c>", c ▷ tagStringLength);                  // Singletons are converted to empty openers
  ✓ b ▷ tagStringEquals(    "<b>" );
  ✓ c ▷ tagStringEquals(    "<c>");

  H ◁ h ▷ first;
  ✓          H ▷ isText;
  ✓          H.node ▷ equalsString("h");
  ✓ !strcmp (H ▷ tagName, "text");

  ✓ !strncmp(H ▷ tagString,      "h", H ▷ tagStringLength);
  ✓          H ▷ tagStringEquals("h");

  ✓ a ▷ equals(b ▷ parent);

  F ◁ f ▷ wrap("FFFF id='1'");
//d ▷ printAssert("d");
  ✓ printsAs_int_$Tag_string(d, "<d><e/>e<FFFF id='1'><f/></FFFF>f<g>g</g></d>");

  F ▷ changeName("NNNN");
//d ▷ printAssert("d");
  ✓ printsAs_int_$Tag_string(d, "<d><e/>e<NNNN id='1'><f/></NNNN>f<g>g</g></d>");

  F ▷ unwrap;
//d ▷ printAssert("d");
  ✓ printsAs_int_$Tag_string(d, "<d><e/>e<f/>f<g>g</g></d>");

  x ▷ free;
 } // test1

void test2()                                                                    //Tprint
 {char file[128] =  "/home/phil/c/z/xml/samples/foreword.dita";
  if (!develop()) strcpy(file, "/home/runner/work/C/C/c/z/xml/samples/foreword.dita");

  x ◁ make$ParseFromFile(file);

  p ◁ x ▷ findFirstTag("p"); c ◁ x ▷ findFirstTag("conbody");

  ✓ p ▷ valid;
  ✓ p ▷ tagNameEquals("p");

  ✓ c ▷ valid;
  ✓ c ▷ tagNameEquals("conbody");

  x ▷ free;
 } // test2

void test3()                                                                    //Tmake$ParseFromString //TprintsAs //TonlyText //Tby
 {xml ◁ "<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j/></a>";
    x ◁ make$ParseFromString(0, (void *)xml, strlen(xml));

  ✓ x ▷ printsAs("<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j/></a>");

    h ◁ x ▷ findFirstTag("h");
  ✓ h ▷ valid;
  ✓ h ▷ onlyText;

    s ◁ makeStringBuffer();

    void action(const $Tag tag)
     {if (tag ▷ isTag) s ▷ add(tag ▷ tagName);
     }

    x ▷ by(action);
    s ▷ joinWith(" ");
  ✓ s ▷ equalsString("c e f g d h b i j a");

    x ▷ free; s ▷ free;
 } // test3

void test4()                                                                    //TnewArenaList //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {file ◁       "/home/phil/c/z/xml/validation/validation.xml";
  if (!develop()) return;                                                       // Does not work on gitHub - possibly out of memory or Cpu?

  xml ◁ make$ParseFromFile((char *)file);
  validate ◁ make$Validate();

    possibilities ◁ xml ▷ findFirstChild("possibilities");                      // The possibilities possibilitiesArray assigns a number to each set of possible tags
  ✓ possibilities ▷ valid;

    possibilitiesArray ◁ possibilities ▷ findFirstChild("array");
  ✓ possibilitiesArray ▷ valid;
    possibilitiesCount ◁ possibilitiesArray ▷ countChildren;
  ✓ possibilitiesCount == 383;

    first ◁ xml ▷ findFirstChild("first");                                      // The tags that can come first under a specified parent tag
  ✓ first ▷ valid;

    firstHash ◁ first ▷ findFirstChild("hash");
  ✓ firstHash ▷ valid;
  ✓ firstHash ▷ countChildren == 374;

    next ◁ xml ▷ findFirstChild("next");                                        // The tags that can come next under a specified parent tag
  ✓ next ▷ valid;

    nextParentHash ◁ next ▷ findFirstChild("hash");
  ✓ nextParentHash ▷ valid;
  ✓ nextParentHash ▷ countChildren == 374;

  if (1)                                                                        // Possibilities possibilitiesArray
   {ArenaTreeNode pa[possibilitiesCount];
    ✓ validate.possibilities ▷ count == 0;
    size_t i = 0;

    $fe(element, possibilitiesArray)                                            // Load possibilities
     {char buffer[256];
      sprintf(buffer, "%lu", i);                                                // Offset as string for use as a key
      f ◁ validate.possibilities ▷ add(buffer, strlen(buffer));                 // Possibility set number
      ✓ f ▷ valid;
      pa[i] = f;                                                                // Cache node offset

      $fe(hash, element)                                                        // Array of hashes
       {$fe(hashKey, hash)                                                      // Keys in each hash
         {name ◁ hashKey ▷ tagName;
          if (strcmp(name, "text")) f ▷ add(name, strlen(name));
         }
       }
      ++i;
     }

    $fe(firstTag, firstHash)                                                    // Load first tags
     {value ◁ firstTag ▷ first;                                                 // Value tag
      makeLocalCopyOf$TagString(t, n, value);
      a ◁ strtol(t, 0, 0);                                                      // Possibility number
      N ◁ firstTag  ▷ tagName;                                                  // Name of tag
      f ◁ validate.first ▷ add(N, strlen(N));                                   // First possibilities for this tag
      *(size_t *)(f ▷ data) = pa[a].offset;                                     // Save the offset of the possibilities set for the first possible tag
     }

    $fe(nextParentTag, nextParentHash)                                          // Load next parents
     {tag  ◁ nextParentTag ▷ tagName;                                           // Next parent tag
        nt ◁ validate.next ▷ add(tag, strlen(tag));                             // Add next parent tag to next tree
      ✓ nt ▷ valid;

      nextChildHash ◁ nextParentTag ▷ findFirstChild("hash");

      $fe(nextChildTag, nextChildHash)                                          // Load next children
       {value ◁ nextChildTag ▷ first;                                           // Value tag
        makeLocalCopyOf$TagString(t, n, value);
        a ◁ strtol(t, 0, 0);                                                    // Possibility number
        name ◁ nextChildTag ▷ tagName;                                          // Next possibilities for the child tag
          F ◁ nt ▷ add(name, strlen(name));                                     // Next possibilities for the child tag
        ✓ F ▷ valid;
        *(size_t *)(F ▷ data) = pa[a].offset;                                   // Save the offset of the possibilities set for the next possible tag
       }
     }
   }

  if (1)                                                                        // Check that 'appendix' is a possible first element of 'appendices'.
   {  f ◁ validate.first ▷ locate("appendices", 10);
    ✓ f ▷ valid;

      n ◁ validate.possibilities ▷ nodeFromOffset(*(size_t *)(f ▷ data));       // Create a node representing the Appendices Tag first possibilities set
    ✓ n ▷ equalsString("8");                                                    // Appendices is tag 8
      p ◁ n ▷ locate("appendix", 8);                                            // Check whether Appendix is in the set of first tag possibilities for Appendices
    ✓ p ▷ valid;                                                                // Appendix is in the first tag set of Appendices
   }

  if (1)                                                                        // Check that 'dd' can follow child 'dt' under parent 'dlentry'.
   {  F ◁ validate.next ▷ ll("dlentry", "dt", 0);                               // Parent + child
    ✓ F ▷ valid;
      n ◁ validate.possibilities ▷ nodeFromOffset(*(size_t *)(F ▷ data));       // Next child possibilities under parent
      p ◁ n ▷ locate("dd", 2);
    ✓ p ▷ valid;                                                                // Check whether 'dd' can follow 'dt' under 'dlentry'
   }

  xml ▷ free; validate ▷ free;
 } // test4

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4, 0};
//{void (*tests[])(void) = {test0, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
