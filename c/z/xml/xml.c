//------------------------------------------------------------------------------
// Parse Xml.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <arenaTree.c>
#include <rectangle.c>
#include <stringBuffer.c>
#include <utilities.c>

#ifndef $_included
#define $_included

//D1 Structures                                                                 // Structures in memory describing an $ parse tree.

#define $tagNameLength 1024                                                     // Tag name return area as xml tags are of limited size
char $tagName[$tagNameLength+1];                                                // Tag name return area as xml tags are short

typedef struct $Parse                                                           // $ parse tree
 {const struct ProtoTypes_$Parse *proto;                                        // Prototypes for methods
  const StringBuffer      fileName;                                             // Source file containing $ to be parsed or zero if there is no file associated with the parse
  const ArenaList         tree;                                                 // Parse tree created from $ being parsed
  const ArenaList         errors;                                               // List of errors in the xml source - the key of the node is the text of the error message, the extra data after the node is $Error
 } $Parse;

typedef struct $Validate                                                        // $ single step validation
 {const struct ProtoTypes_$Validate *proto;                                     // Prototypes for methods
  ArenaTree    possibilities;                                                   // Possibility sets for each Dita tag
  ArenaTree    first;                                                           // First set of possibilities for each Dita tag
  ArenaTree    next;                                                            // Next set of possibilities for each Dita child tag under a given parent tag
 } $Validate;

typedef struct $Tag                                                             // Tag in an $ parse tree
 {const struct ProtoTypes_$Tag *proto;                                          // Prototypes for methods
  const $Parse            xml;                                                  // $ parse tree
  ArenaListNode           node;                                                 // Arena tree node in the xml parse tree that contains this tag
 } $Tag;

typedef struct $NodeExt                                                         // Extra data held in each node of the parse tree
 {unsigned char           text;                                                 // True if this is a text node
 } $NodeExt;

typedef struct $ParseOptions                                                    // Options for an $ parse
 {const struct ProtoTypes_$ParseOptions *proto;                                 // Prototypes for methods
  const char  *           location;                                             // Location of string in memory to be parsed
  const size_t            length;                                               // Length of string ot be parsed
  const size_t            width;                                                // Size of the extra data attached to each parse tree node
  StringBuffer            fileName;                                             // File name optionally associated with parse
 } $ParseOptions;

#include <$$_prototypes.h>
#define makeLocalCopyOf$TagString(string,stringLength,tag) stringLength ◁ content__ArenaListNode(&tag.node)->length; char string[stringLength+1]; string[stringLength] = 0; memcpy(string, key_pointer__ArenaListNode(&tag.node), stringLength); // Copy the key and the length of the key of the specified node to the stack.

#define $Open     '<'                                                           // Important $ characters
#define $Close    '>'
#define $Slash    '/'
#define $Question '?'
#define $Exclaim  '!'

$Parse make$Parse                                                               // Perform requested parse
 ($ParseOptions options)                                                        // Name of file holding $ or zero if no file is associated with this source
 {sourceFN      ◁ options.fileName;                                             // File to parse if any
  parseTree     ◁ makeArenaListWithWidth(sizeof($NodeExt));                     // Parse tree which has nodes with extension data
  errors        ◁ makeArenaListWithWidth(sizeof(size_t));                       // Errors list with location of error
  $Parse xml    = {fileName: sourceFN, tree: parseTree,                         // Create $ descriptor
                   errors: errors, proto: &ProtoTypes_$Parse};

  source ◁ options.location; length ◁ options.length;                           // Zero terminated string to parse
  assert(!source[length]);                                                      // Check we have a trailing zero so that the following loop terminates

  currentParent ◀ parseTree ▷ root;                                             // Current parent node

  $Parse error                                                                  // Report an error message adding the file name involved on the following line
   (const char * const p,                                                       // Pointer to position at which the error occurred
    const char * const format, ...)                                             // Message text
   {va_list va;

    m ◁ makeStringBuffer();                                                     // Build the error message
    o ◁ p - source;                                                             // Offset of the error

    va_start(va, format); m ▷ addVaFormat(format, va); va_end(va);              // Format error message in a buffer of adequate size
    if (sourceFN.proto)                                                         // Add file name
     {m ▷ add    ("\n  File     : ");
      m ▷ addStringBuffer(sourceFN);
     }
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

  for(const char * p = source; *p;)                                             // Break out tags and text
   {o ◁ strchr(p, $Open);                                                       // Find next open

    if (o)                                                                      // Found next open
     {if (o > p)
       {int allBlank()                                                          // Check whether the text to be saved is all white space. We can recover the position of non whitespace from the offset saved in the data field of the node.
         {for(const char * q = p; q < o; ++q) if (!isspace(*q)) return 0;
          return 1;
          }

        if (!allBlank())                                                        // Save the text as the key of the node
         {n ◀ parseTree ▷ node(p, o - p);                                       // Create node
          $NodeExt e = {text: 1};                                               // Text marker
          n ▷ setData(&e);                                                      // Save  text marker
          currentParent ▷ putLast(n);                                           // Save node in parse tree
         }
       }
          c ◁ strchr(o, $Close);                                                // Save tag: find corresponding close
      if (c)                                                                    // Found closing >
       {void save(void)                                                         // Save tag as a new node
         {n ◁ parseTree ▷ node(o + 1, c - o - 1);                               // Create node
          currentParent ▷ putLast(n);                                           // Save node in parse tree
          currentParent = n;
         }
        void saveSingleton(void)                                                // Save singleton
         {const size_t l = c - o - 2;                                           // Length of current tag string
          char b[l+1]; memcpy(b, o+1, l); b[l] = 0;                             // Copy tag string
          n ◁ parseTree ▷ node(b, l);                                           // Create node
          currentParent ▷ putLast(n);                                           // Save node in parse tree
         }

        void end(void)                                                          // End tag
         {makeLocalCopyOfArenaListKey(k, l, currentParent);
          char a[$tagNameLength+1]; strcpy(a, parse$TagName(k, l));             // Start tag name - we have to make copies because there is only one tag name buffer
          char *b = parse$TagNameInAngleBrackets(o);                            // End tag name

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

//  if (0)                                                                        // Make the single root xml tag the root of the parse tree
//   {r ◁ parseTree ▷ root;
//    f ◁ r ▷ first;
//    ArenaListContent * const a = r ▷ content, * const b = f ▷ content;
//    *a = *b;
//    ArenaListfe(c, r) c ▷ setUp(r);
//   }

  return xml;
 } // make$Parse

$Parse make$ParseFromFile                                                       // Parse $ held in a file
 (const char * const sourceFileName,                                            // Name of file holding $ to be parsed
  const size_t width)                                                           // Size of extra data associated with each node in the parse tree
 {$Parse p;                                                                     // The resulting parse

  ssize_t reader(char * location, size_t length)                                // Function to process the mapped file. Return non negative on success, negative on failure: this value will be returned to the caller.
   {StringBuffer  f = makeStringBufferFromString(sourceFileName);
    $ParseOptions o = new $ParseOptions(location: location, length: length, width: width, fileName: f);
    $Parse        q = make$Parse(o);                                            // Parse
    memcpy((void *)&p, (void *)&q, sizeof($Parse));                             // Temporarily subvert the const settings to return the results
    return 1;
   }

  readFile(sourceFileName, reader);                                             // Read the source and parse it

  return p;
 }

$Parse make$ParseFromString                                                     // Parse $  held in a string
 (const char * const source,                                                    // Source text to be parsed
  const size_t       length,                                                    // Length of source text to be parsed
  const size_t       width)                                                     // Size of extra data associated with each node in the parse tree
 {$ParseOptions o = new $ParseOptions(location: source, length: length, width: width);
  return make$Parse(o);                                                         // Parse
 }

$Validate make$Validate()                                                       // Make an $ single step validator
 {possibilities ◁ makeArenaTree();                                              // Single Step Validation
  first         ◁ makeArenaTreeWithWidth(sizeof(size_t));                       // First set of possibilities for each tag
  next          ◁ makeArenaTreeWithWidth(sizeof(size_t));                       // Next set of possibilities for each Dita child tag under a given parent tag
  $Validate valid = {possibilities: possibilities,
    first: first, next: next, proto: &ProtoTypes_$Validate};
  return valid;
 }

static $Parse  parse$FromString                                                 // Make a parse $ from a string.
 (const char * const string,                                                    // String of $
  const size_t       width)                                                     // Size of extra data associated with each node in the parse tree
 {return make$ParseFromString(string, strlen(string), width);
 }

static void free_$Parse                                                         // Free an $ parse
 (const $Parse * x)                                                             // $ descriptor
 {x->tree ▷ free; x->errors ▷ free; if (x->fileName.proto) x->fileName ▷ free;
 }

static void free_$Validate                                                      // Free an $ validator
 ($Validate * x)                                                                // $ validator
 {x->possibilities ▷ free;
  x->first ▷ free;
  x->next ▷ free;
 }

static size_t errors_size_$Parse                                                // Number of errors encountered while creating an $ parse tree.
 (const $Parse * x)                                                             // $ descriptor
 {return x->errors ▷ count;
 }

static char  * parse$TagNameInAngleBrackets                                     //P Get the tag name from an $ tag string. The tag name will be truncated if it is longer than $tagNameLength. The returned  name is held in the fixed buffer $tagName.
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

static  char * parse$TagName                                                    //P Get the tag name from a string assuming the tag name starts at the start of the string and is either terminated by a space or the end of the string.  The tag name will be truncated if it is longer than $tagNameLength. The returned  name is held in the fixed buffer $tagName.
 (const char * tag,                                                             // Tag
  const size_t length)                                                          // Length
 {size_t N = length < $tagNameLength ? length : $tagNameLength;
  for(size_t i = 0; i < N; ++i)                                                 // Copy tag string up to first space
   {$tagName[i] = tag[i];
    if (tag[i] == ' ') {$tagName[i] = 0; return $tagName;}
   }
  $tagName[N] = 0;
  return $tagName;
 }

static char * tagName_string_$Tag                                               // Get the tag name from a node in the $ parse tree. The tag name is returned in a reused static buffer that the caller should not free.
 (const $Tag * Tag)                                                             // Tag
 {tag ◁  *Tag;
  if (tag ▷ isText) return "text";                                              // Text node
  makeLocalCopyOf$TagString(t, l, tag);
  return parse$TagName(t, l);
 }

static int tagNameEquals_int_$Tag_string                                        // Check the name of a tag.
 (const $Tag *       tag,                                                       // Tag
  const char * const expected)                                                  // Expected name of tag
 {return !strcmp(tag ▶ tagName, expected);
 }

static char * tagString_string_$Tag                                             //V Get the entire tag from a node in the $ parse tree.
 (const $Tag * tag)                                                             // Tag
 {return tag->node ▷ key;
 }

static size_t tagStringLength_size_$Tag                                         // Get the length of the entire tag
 (const $Tag * tag)                                                             // Tag
 {return tag->node ▷ length;
 }

static int tagStringEquals_int_$Tag_string                                      // Check the string of a tag.
 (const $Tag *       tag,                                                       // Tag
  const char * const expected)                                                  // Expected name of tag
 {return tag->node ▷ equalsString(expected);
 }

//D1 Navigation                                                                 // Navigate through an $ parse tree.

#define $fe( child, parent) for($Tag child = new $Tag(xml: parent.xml, node: parent ▷ first.node); child ▷ valid; child.node.offset = child ▷ next.node.offset)  // Each child in a parent from first to last
#define $fer(child, parent) for($Tag child = new $Tag(xml: parent.xml, node: parent ▷ last.node);  child ▷ valid; child.node.offset = child ▷ prev.node.offset) // Each child in a parent from last to first

static $Tag first_$Tag                                                          // Return the first child tag under the specified parent tag.
 (const $Tag * parent)                                                          // Parent tag
 {return new $Tag(xml: parent->xml, node: parent->node ▷ first);
 }
duplicate s/first/last/  s/first/next/ s/first/prev/ s/first/parent/

static $Tag first_$Parse                                                        // Return the first child tag in the specified $ parse tree.
 (const $Parse * xml)                                                           // Parent tag
 {return new $Tag(xml: *xml, node: xml->tree ▷ first);
 }
duplicate s/first/last/

//D1 Location                                                                   // Check the current location in the $ parse tre

static int depth_int_$Tag                                                       // The depth of a tag
 (const $Tag * Tag)                                                             // Tag
 {i ◀ 0;
  tag ◁ *Tag;
  for($Tag t = tag; t ▷ valid; t.node.offset = t ▷ parent.node.offset) ++i;     // Count up to root
  return i - 1;                                                                 // We have included the root node
 }

static int isRoot_int_$Tag                                                      // Check whether the specified tag is the root tag
 (const $Tag * tag)                                                             // Tag
 {p ◁ tag ▶ parent;                                                             // Possibly the root of the ArenaList
  return !p ▷ valid;                                                            // The root of the xml tree is one level below the root of the ArenaList
 }

static int isFirst_int_$Tag                                                     // Check that the specified tag is first under its parent
 (const $Tag * Tag)                                                             // Tag
 {tag ◁ *Tag;
  if (tag ▷ isRoot) return 1;                                                   // The root tag is always first
      parent ◁ tag ▷ parent;
  f ◁ parent ▷ first;
  return f ▷ equals(tag);
 }
duplicate s/first/last/g,s/First/Last/g

//D1 Text methods                                                               // Methods that operate on text tags

static int isText_int_$Tag                                                      // Check whether we are on a text element
 (const $Tag * tag)                                                             // Tag
 {$NodeExt e;                                                                   // Extension data for node
  tag->node ▷ getData(&e);                                                      // Get extension data from node
  return e.text;                                                                // Text or not
 }

static int onlyText_$Tag                                                        // Return true if a tag contains just one text element and nothing else
 (const $Tag * parent)                                                          // Tag
 {n ◁ parent ▶ countChildren;
  if (n != 1) return 0;                                                         // If there is more than one tag than there must be a non text tag present to separate the text
  f ◁ parent ▶ first;
  return f ▷ isText;
 }

//D1 Statistics                                                                 // Counts on the tag

static int empty_int_$Tag                                                       // Check whether the specified parent tag is empty
 (const $Tag * parent)                                                          // Parent tag
 {return !parent ▶ countChildren;
 }

static int isTag_int_tag                                                        // Check that the tag is a tag not text
 (const $Tag * tag)                                                             // Parent tag
 {return !tag ▶ isText;
 }

static int hasText_int_$Tag                                                     // Check whether the tag contains a text element
 (const $Tag * Parent)                                                          // Parent tag
 {parent ◁ *Parent;
   $fe(child, parent) if (child ▷ isText) return 1;
  return 0;
 }

static int stayInLine_int_$Tag                                                  // Check whether a tag is text or is preceded or followed by text
 (const $Tag * Tag)                                                             // Tag
 {tag ◁ *Tag;
  if ( tag ▷ isText)                   return 1;
  if (!tag ▷ isFirst) {p ◁ tag ▷ prev; return p ▷ isText;}
  if (!tag ▷ isLast)  {n ◁ tag ▷ next; return n ▷ isText;}
  return 0;
 }

static size_t length_size__$Tag                                                 // Length of a tag string
 (const $Tag * tag)                                                             // $Tag
 {return tag->node ▷ content->length;
 }

//D1 Search                                                                     // Search the $ parse tree

static int valid_int_$Tag                                                       // Check that a tag is valid.
 (const $Tag * tag)                                                             // Tag
 {return tag->node.offset;                                                      // A tag is valid unless it is the root node of the arena tree backing the $ parse tree.
 }

static $Tag findFirstChild_$Tag_$Tag_string                                     // Find the first child tag with the specified name under the specified parent tag.
 (const $Tag *       Parent,                                                    // Parent
  const char * const key)                                                       // Name of the tag to find
 {parent ◁ *Parent;
  $fe(child, parent)                                                            // Each child of the parent
   {if (child ▷ tagNameEquals(key)) return child;                               // First child with matching tag
   }
  return new $Tag;
 }

static $Tag findFirstChild_$Tag_$Parse_string                                   // Find the first child tag with the specified name under the root tag of the specified parse tree.
 (const $Parse * xml,                                                           // Parent
  const char   * const key)                                                     // Name of the tag to find
 {    root ◁ xml ▶ root;                                                        // Root tag
  f ◁ root ▷ first;
  return f ▷ findFirstChild(key);                                               // Each child of the parent
 }

static $Tag findFirstTag_$Tag_$Tag_string                                       // Find the first tag with the specified name in the $ parse tree starting at the specified tag.
 (const $Tag *       parent,                                                    // Parent tag
  const char * const key)                                                       // Name of the tag to find
 {jmp_buf found;
  $Tag T = new $Tag(xml: parent->xml);                                          // Tag found if any

  void find(const $Tag tag)                                                     // Check whether the name of the tag matches the specified key
   {if (tag ▷ tagNameEquals(key))
     {T.node = tag.node;                                                        // Found matching node
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) parent ▶ by(find);                                        // Search the $ parse tree
  return T;
 }

static $Tag findFirstTag_$Tag_$Parse_string                                     // Find the first tag in an $ parse tree with the specified name.
 (const $Parse *       xml,                                                     // Xml parse tree
  const char   * const key)                                                     // Name of the tag to find
 {jmp_buf found;
  $Tag T = new $Tag(xml: *xml);                                                 // Tag found if any

  void find(const $Tag tag)                                                     // Check whether the name of the tag matches the specified key
   {if (tag ▷ tagNameEquals(key))
     {T.node = tag.node;                                                        // Found matching node
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) xml ▶ by(find);                                           // Search the $ parse tree
  return T;
 }

static $Tag root_$Parse                                                         // Return the root tag of the specified $ parse tree
 (const $Parse * xml)                                                           // $ parse tree
 {return new $Tag(xml: *xml, node: xml->tree ▷ root);
 }

static $Tag root_$Tag                                                           // Return the root tsg of the $ parse tree containing the specified tag.
 (const $Tag * tag)                                                             // Tag
 {return new $Tag(xml: tag->xml, node: tag->xml.tree ▷ root);
 }

static int equals_$Tag_$Tag                                                     // Confirm that two tags refer to the same text location in the source document.
 (const $Tag * a,                                                               // First tag
  const $Tag b)                                                                 // Second tag
 {return a->node.offset == b.node.offset;
 }

//D1 Traverse                                                                   // Traverse the $ parse tree.

static void by_$Tag_sub                                                         // Traverse the $ parse tree rooted at the specified tag in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const $Tag * tag,                                                             // Starting tag
  void (* const function) (const $Tag tag))                                     // Function to call on each tag
 {void f(const ArenaListNode node)
   {function(new $Tag(xml: tag->xml, node: node));
   }
  tag->node ▷ by(f);
 }

static void by_$Parse_sub                                                       // Traverse the $ parse tree in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const $Parse * xml,                                                           // $ parse tree
  void (* const function) (const $Tag tag))                                     // Function to call on each tag
 {void f(const ArenaListNode node)
   {function(new $Tag(xml: *xml, node: node));
   }
  F ◁ xml->tree ▷ first;
  F ▷ by(f);
 }

static void scan_$Tag_sub                                                       // Traverse the $ parse tree rooted at the specified tag calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The $ is buffered allowing changes to be made to the structure of the $ without disruption as long as each child checks its context.
 (const $Tag * tag,                                                             // Starting tag
  void (* const function) ($Tag tag, int start, int depth))                     // Function to call on each tag: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
 {void f(const ArenaListNode node,   int start, int depth)
   {function(new $Tag(xml: tag->xml, node: node), start, depth);
   }
  tag->node ▷ scan(f);
 }

static void scan_$Parse_sub                                                     // Traverse the $ parse tree calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The $ is buffered allowing changes to be made to the structure of the $ without disruption as long as each child checks its context.
 (const $Parse * xml,                                                           // $ parse tree
  void (* const function) ($Tag tag, int start, int depth))                     // Function to call on each tag: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
 {void f(const ArenaListNode node,   int start, int depth)
   {function(new $Tag(xml: *xml, node: node), start, depth);
   }
  xml->tree ▷ scan(f);
 }

static  size_t countChildren_size_$                                             // Count the number of tags at the first level in an xml parse tree.
 (const $Parse * xml)                                                           // $ parse
 {       root ◁ xml->tree ▷ root;                                               // Root node
  return root ▷ countChildren - 1;                                              // Count the level immediately below the root tag of which there is always just one
 }

static  size_t countChildren_size_$Tag                                          // Count the number of child tags directly under a parent tag
 (const $Tag * parent)                                                          // Parent tag
 {return parent->node ▷ countChildren;
 }

static  size_t count_size_$                                                     // Count the number of tags in an $ parse tree
 (const $Parse * x)                                                             // $ parse tree
 {return x->tree ▷ count;
 }

static  size_t count_size_$Tag                                                  // Count the number of tags under the specified tag in an $ parse tree
 (const $Tag * t)                                                               // Parent tag
 {return t->node ▷ count;
 }

//D1 Edit                                                                       // Change the structure of an $ parse tree.

static void changeName_$Tag                                                     // Change the name of the specified tag.
 (const $Tag * Tag,                                                             // Tag
  const char * const newName)                                                   // New name                                                                               //vconst $Tag t)                                                                 // Tag
 {tag ◁ *Tag;
  oldName ◁ tag ▷ tagName;                                                      // Old name
  string  ◁ tag ▷ tagString;                                                    // Tag string
  o ◁ strlen(oldName); n ◁ strlen(newName); l ◁ strlen(string);
  if (n > o) printStackBackTraceAndExit(1, "New code needed to deal with the case where the new name is longer than the old name\n");

  char key[l + n + 1], * p = key, * const q = strstr(string, oldName);
  p = stpncpy(p, string, q - string);
  p = stpcpy (p, newName);
  p = stpcpy (p, q + o);
  ArenaListNode t = *(ArenaListNode *)(&tag.node);
  t ▷ setKey(key, strlen(key));
 }

static void insertChar__$Tag_char_size                                          // Insert the specified character into the string of a tag at the specified position.
 ($Tag  * tag,                                                                  // $Tag
  const char  ins,                                                              // Character to insert
  size_t      pos)                                                              // Position in key. 0 prepends the char, while >= length appends the char.
 {tag->node ▷ insertChar(ins, pos);
 }

static void replaceChar__$Tag_char_size                                         // Replace the character at the specified position in the key string of a tag with the specified character.
 (const $Tag  * tag,                                                            // $Tag
  const char  repl,                                                             // Replacement character
  size_t      pos)                                                              // Position in key. 0 replaces the first character.  No replacement occurs if the requested character is beyond the end of the key string
 {tag->node ▷ replaceChar(repl, pos);
 }

static void swapChars__$Tag_size                                                // Delete the character at the specified position in the string of a tag.
 (const $Tag  * tag,                                                            // $Tag
  size_t      pos)                                                              // Position in key. 0 deletes the first character.  No deletion occurs if the requested character is beyond the end of the key string
 {tag->node ▷ swapChars(pos);
 }

static void deleteChar__$Tag_size                                               // Delete the character at the specified position in the string of a tag.
 (const $Tag  * tag,                                                            // $Tag
  size_t      pos)                                                              // Position in key. 0 deletes the first character.  No deletion occurs if the requested character is beyond the end of the key string
 {tag->node ▷ deleteChar(pos);
 }

//D1 Wrap and Unwrap                                                            // Wrap and unwrap nodes

static $Tag wrap_$Tag_string                                                    // Wrap a specified tag with a new tag and return the newly createdf wraping tag.
 (const $Tag *       Tag,                                                       // Tag
  const char * const string)                                                    // Wrapper without the leading < or trailing or >
 {tag ◁ *Tag;                                                                   // Tag
  if (tag ▷ isRoot) return tag;
  char s[strlen(string)+4], *p = stpcpy(s, string); *p = 0;
  const ArenaListNode n =  tag.node ▷ wrap(s);
  return new $Tag(xml: tag.xml, node: n);
 }

static $Tag unwrap_$Tag                                                         // Unwrap the specified tag.
 (const $Tag  *      tag)                                                       // Tag
 {if (!tag ▶ isRoot) tag->node ▷ unwrap;                                        // Cannot unwrap the root tag
  return *tag;
 }

//D1 Print                                                                      // Print an $ parse tree starting at the specified tag.

static StringBuffer prettyPrint_stringBuffer_$Tag                               // Pretty print the $ parse tree starting at the specified tag with additional spacing between tags to make the tree easier to read.
 (const $Tag * tag)                                                             // Starting tag
 {p ◀ makeStringBuffer();

  void print(const $Tag parent, const int depth)                                // Print the specified parent and its children
   {void open()                                                                 // Add open tag
     {if (parent ▷ isText)
       {p ▷ addn(parent ▷ tagString, parent ▷ tagStringLength);
       }
      else if (parent ▷ empty)                                                  // Write tag with no children as a singleton
       {makeLocalCopyOf$TagString(s, l, parent);
        p ▷ addChar($Open);
        p ▷ addn(s, l);
        p ▷ addFormat("%c%c", $Slash, $Close);
       }
      else if (parent ▷ stayInLine || parent ▷ isRoot)                          // Opener preceded or followed by text
       {p ▷ addChar($Open);
        p ▷ addn(parent ▷ tagString, parent ▷ tagStringLength);
        p ▷ addChar($Close);
       }
      else                                                                      // Opener
       {p ▷ addNewLine; p ▷ addSpaces(depth*2);
        p ▷ addChar($Open);
        p ▷ addn(parent ▷ tagString, parent ▷ tagStringLength);
        p ▷ addChar($Close);
       }
     }
    void close()                                                                // Add close tag unless we are on text
     {if (parent ▷ isTag && !parent ▷ empty)
       {if (!parent ▷ stayInLine) {p ▷ addNewLine; p ▷ addSpaces(depth*2);}
        p ▷ addFormat("%c%c%s%c", $Open, $Slash, parent ▷ tagName, $Close);
       }
     }

    open();
    $fe(child, parent) print(child, depth+1);
    close();
   }

  print(*tag, 0);
  p ▷ addNewLine;                                                               // Terminating new line
  p ▷ join;
  return p;
 } // prettyPrint

static StringBuffer prettyPrint_stringBuffer_$                                  // Pretty print the $ parse tree with additional spacing between tags to make the tree easier to read.
 (const $Parse * xml)                                                           // $ parse tree
 {    root ◁ xml ▶ root;
  f ◁ root ▷ first;
  return f ▷ prettyPrint;
 } // prettyPrint

static StringBuffer print_stringBuffer_$Tag                                     // Print the $ parse tree starting at the specified tag with additional spacing between tags to make the tree easier to read.
 (const $Tag * tag)                                                             // Starting tag
 {const StringBuffer p = makeStringBuffer();

  void print(const $Tag parent, const int depth)                                // Print the specified parent and its children
   {void open()                                                                 // Add open tag
     {if (parent ▷ isText)
       {p ▷ addn(parent ▷ tagString, parent ▷ tagStringLength);
       }
      else if (parent ▷ empty)                                                  // Write tag with no children as a singleton
       {makeLocalCopyOf$TagString(s, l, parent);
        p ▷ addChar($Open);
        p ▷ addn(s, l);
        p ▷ addFormat("%c%c", $Slash, $Close);
       }
      else                                                                      // Opener
       {p ▷ addChar($Open);
        p ▷ addn(parent ▷ tagString, parent ▷ tagStringLength);
        p ▷ addChar($Close);
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

  print(*tag, 0);
  return p;
 }

static StringBuffer print_stringBuffer_$                                        // Print the $ parse tree with additional spacing between tags to make the tree easier to read.
 (const $Parse * xml)                                                           // $ parse tree
 {    root ◁ xml ▶ root;
  f ◁ root ▷ first;
  return f ▷ print;
 }

static int printsAs_int_$_string                                                // Check that the $ parse tree prints as expected
 (const $Parse *       xml,                                                     // $ parse tree
  const char   * const expected)                                                // Expected print
 {s ◁ xml ▶ print;
  r ◁ s ▷ printsAs(expected);
  s ▷ free;
  return r;
 }

static int printsAs_int_$Tag_string                                             // Check that the $ parse tree prints as expected
 (const $Tag * tag,                                                             // $ parse tree
  const char * const expected)                                                  // Expected print
 {s ◁ tag ▶ print;
  r ◁ s ▷ printsAs(expected);
  s ▷ free;
  return r;
 }

static void printAssert_$Tag                                                    //P Print the $ parse tree starting at the specified tag as an assert statement
 (const $Tag *       tag,                                                       // Starting tag
  const char * const variable)                                                  // The name of the variable preceding this call
 {t ◀ tag ▶ print;
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
 (const $Parse *     xml,                                                       // $
  const char * const variable)                                                  // The name of the variable preceding this call
 {const $Tag t = xml ▶ root;
  t ▷ printAssert(variable);
 }

static void dump_$                                                              //P Print the $ parse tree starting to stderr
 (const $Parse * x)                                                             // $
 {t ◀ x ▶ prettyPrint;
  t ▷ join;
  t ▷ dump;
  t ▷ free;
 }

static void dump_$Tag                                                           //P Dump the specified tag
 (const $Tag * Tag)                                                             // $
 {tag ◁ *Tag;
  makeLocalCopyOf$TagString(t, l, tag);
  say("%s\n", t);
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TnewArenaList //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer //Terrors //Tmake$ParseFromFile //Tempty //TisTag //tText
 {x ◁ parse$FromString("<a>aa<b>bb<c/>cc</b>dd<d><e/><f>F</f><g/></d><h/><i/></A>h", 0);
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

void test1()                                                                    //Tfirst //Tlast //Tprev //Tnext //Tequals //Tcount //TcountChildren //TfindFirstTag //TfindFirstChild //Tparse$FromString //Tparse$TagName //TtagName //TtagNameEquals //Tvalid //TtagString //TtagStringEquals //Tparent //Troot //Twrap //Tunwrap //TchangeName //TtagStringLength //TisText //TisFirst //TisLast //TisRoot //TstayInLine
 {char * xml = "<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j></j></a>";
  x ◁ parse$FromString(xml, 0);

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
  ✓ c ▷ equals(b ▷ first);  ✓  c ▷ empty;     ✓ !c ▷ stayInLine;
  ✓ h ▷ equals(b ▷ last);   ✓  h ▷ isLast;    ✓ !d ▷ isLast;
  ✓ j ▷ equals(a ▷ last);

  ✓ h ▷ equals(d ▷ next);
  ✓ c ▷ equals(d ▷ prev);

  ✓ a ▷ count         == 14;
  ✓ a ▷ countChildren ==  4;

  ✓ b ▷ count         == 10;
  ✓ b ▷ countChildren ==  3;

  ✓ !strcmp(parse$TagName(b.node ▷ key, b.node ▷ length), "b");
  ✓ !strcmp(b ▷ tagName,                                  "b");

  ✓ !strncmp(b ▷ tagString, "b", b ▷ tagStringLength);
  ✓ !strncmp(c ▷ tagString, "c", c ▷ tagStringLength);
  ✓ b ▷ tagStringEquals(    "b");
  ✓ c ▷ tagStringEquals(    "c");

  H ◁ h ▷ first;
  ✓          H ▷ isText;
  ✓          H.node ▷ equalsString("h");
  ✓ !strcmp (H ▷ tagName, "text");

  ✓ !strncmp(H ▷ tagString,      "h", H ▷ tagStringLength);
  ✓          H ▷ tagStringEquals("h");

  ✓ a ▷ equals(b ▷ parent);

  F ◁ f ▷ wrap("FFFF id='1'");
//d ▷ printAssert("d");
  ✓ d ▷ printsAs("<d><e/>e<FFFF id='1'><f/></FFFF>f<g>g</g></d>");

  F ▷ changeName("NNNN");
//d ▷ printAssert("d");
  ✓ d ▷ printsAs("<d><e/>e<NNNN id='1'><f/></NNNN>f<g>g</g></d>");

  F ▷ unwrap;
//d ▷ printAssert("d");
  ✓ d ▷ printsAs("<d><e/>e<f/>f<g>g</g></d>");

  x ▷ free;
 } // test1

void test2()                                                                    //Tprint
 {char * string = ◉;
<?xml version="1.0"?>
<!DOCTYPE concept PUBLIC "-//OASIS//DTD DITA Concept//EN" "concept.dtd">
<!-- Created with XMetaL (http://www.xmetal.com) -->
<concept id="concept_6FD9560EEEED4FB0A70086250EE15F88">
  <title>Foreword</title>
  <shortdesc><?xm-replace_text Short Description?>
  </shortdesc>
  <conbody>
   <p>DITA represents a fundamental shift in the approach to writing and
    publishing, and to a certain extent, the carefully accumulated practice of
    hundreds of years of publication projects has to be reviewed and redefined.
   </p>
   <section id="section_73DD7CCCB3734B7DB29E4D200702B61F"
    audience="contributor">
    <title>Rationale</title>
    <lq reftitle="Deborah Pickett, Yahoo! DITA Users Group"
     href="http://tech.dir.groups.yahoo.com/group/dita-users/message/12656"> I would
      suggest that users who are interested in a particular feature get together and
      define their own best practices for said feature. Define the markup that
      authors should use, and define the processing expectations of that markup. If
      this unofficial spec is reasonable, then vendors will find it in their
      interests to support it rather than striking out on their own. (It's this kind
      of distributed, grassroots power-to-the-users ideology that the DITA TC wants
      to instill in the community.) I've found that vendors (both commercial and
      non-commercial) are usually very willing to accommodate users' requirements,
      once they know what those requirements are.
    </lq>
    <lq reftitle="Joe Gershon, OASIS DITA TC Mailing List">The problem is
      that the vast majority of new users are misusing DITA to a point that they are
      often worse off than they were in Word or FrameMaker. Most companies choose not
      to hire an expert to help them migrate to DITA, and they get what they pay for.
      I'd like us as a TC (and moving forward via the Adoption SC) to put tutorials,
      white papers, etc. in place to educate new users to help them to do the right
      thing from the beginning. At the same time, we must develop tutorials, white
      papers, and other guides that cover as many use cases and user groups as we're
      aware of and address their needs, suggesting best practices and, where
      relevant, providing unofficial packages of DTDs and supporting code (e.g.
      DITA-OT plugins).
    </lq>
   </section>
  </conbody>
</concept>
◉

  x ◁ make$ParseFromString(string, strlen(string), 0);

  p ◁ x ▷ findFirstTag("p"); c ◁ x ▷ findFirstTag("conbody");

  ✓ p ▷ valid;
  ✓ p ▷ tagNameEquals("p");

  ✓ c ▷ valid;
  ✓ c ▷ tagNameEquals("conbody");

  x ▷ free;
 } // test2

void test3()                                                                    //Tmake$ParseFromString //TprintsAs //TonlyText //Tby //Tdepth
 {xml ◁ "<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j/></a>";
    x ◁ make$ParseFromString(xml, strlen(xml), 0);

  ✓ x ▷ printsAs("<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j/></a>");

    h ◁ x ▷ findFirstTag("h");
  ✓ h ▷ valid;
  ✓ h ▷ onlyText;
  ✓ h ▷ depth == 2;

    s ◀ makeStringBuffer();

    void action(const $Tag tag)
     {if (tag ▷ isTag) s ▷ add(tag ▷ tagName);
     }

    x ▷ by(action);
    s ▷ joinWith(" ");
  ✓ s ▷ equalsString("c e f g d h b i j a");

    x ▷ free; s ▷ free;
 } // test3

void test4()                                                                    //TnewArenaList //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {file ◁  "/home/phil/c/z/xml/validation/validation.xml";
  if (!developmentMode()) return;                                               // Does not work on gitHub - possibly out of memory or Cpu?

  xml ◁ make$ParseFromFile(file, 0);
  validate ◀ make$Validate();

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
    i ◀ 0ul;

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

      n ◁ validate.possibilities ▷ offset(*(size_t *)(f ▷ data));               // Create a node representing the Appendices Tag first possibilities set
    ✓ n ▷ equalsString("8");                                                    // Appendices is tag 8
      p ◁ n ▷ locate("appendix", 8);                                            // Check whether Appendix is in the set of first tag possibilities for Appendices
    ✓ p ▷ valid;                                                                // Appendix is in the first tag set of Appendices
   }

  if (1)                                                                        // Check that 'dd' can follow child 'dt' under parent 'dlentry'.
   {  F ◁ validate.next ▷ ll("dlentry", "dt", 0);                               // Parent + child
    ✓ F ▷ valid;
      n ◁ validate.possibilities ▷ offset(*(size_t *)(F ▷ data));               // Next child possibilities under parent
      p ◁ n ▷ locate("dd", 2);
    ✓ p ▷ valid;                                                                // Check whether 'dd' can follow 'dt' under 'dlentry'
   }

  xml ▷ free; validate ▷ free;
 } // test4

void test5()                                                                    //ThasText //TstayInline //TprettyPrint
 {xml ◁ "<a><b>bb bb <c/> ccc</b><d> <i/> <j> jj <k/> kk</j> </d></a>";         // Avoiding trailing blanks

  x ◁ parse$FromString(xml, 0);  ✓ ! x ▷ errors;
  b ◁ x ▷ findFirstTag("b");     ✓   b ▷ hasText;
                                 ✓ ! b ▷ stayInLine;
  c ◁ x ▷ findFirstTag("c");     ✓   c ▷ stayInLine;
  C ◁ c ▷ next;                  ✓   C ▷ stayInLine;
  d ◁ x ▷ findFirstTag("d");     ✓ ! d ▷ hasText;

    p ◁ x ▷ prettyPrint;
  ✓ p ▷ printsAs(◉);
<a>
  <b>bb bb <c/> ccc
  </b>
  <d><i/>
    <j> jj <k/> kk
    </j>
  </d>
</a>
◉

  x ▷ free; p ▷ free;
 }

void test6()                                                                    //Tscan
 {xml  ◁ "<a><b><c/><d><e/>ee<f/>ff<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";
     x ◁ parse$FromString(xml, 0);
  ✓ !x ▷ errors;

  s ◁ makeStringBuffer();

  void action(const $Tag tag, int start, int depth)
   {makeLocalCopyOf$TagString(t, l, tag);
    s ▷ addFormat("%s(%d,%d) ", t, start, depth);
   }

  x ▷ scan(action);
  s ▷ equalsString("(1,0) <a>(1,1) <b>(1,2) <c>(0,3) <d>(1,3) <e>(0,4) ee(0,4) <f>(0,4) ff(0,4) <g>(1,4) ggg(0,5) <g>(-1,4) <d>(-1,3) <h>(1,3) hh hh(0,4) <h>(-1,3) <b>(-1,2) <i>(0,2) i(0,2) <j>(0,2) <a>(-1,1) (-1,0)");
  x ▷ free; s ▷ free;
 }

void test7()                                                                    //Tlength //TinsertChar //TdeleteChar //TreplaceChar //TswapChars
 {xml  ◁ "<a><abcd>1234</abcd></a>";
     x ◁ parse$FromString(xml, 0);
  ✓ !x ▷ errors;

  a ◀ x ▷ findFirstChild("abcd");
  t ◀ a ▷ first;

  a ▷ insertChar('D', 3);  ✓ x ▷ printsAs("<a><abcDd>1234</abcDd></a>");
  t ▷ insertChar('T', 3);  ✓ x ▷ printsAs("<a><abcDd>123T4</abcDd></a>");
  ✓ a ▷ length == 5;       ✓ t ▷ length == 5;

  a ▷ replaceChar('e', 3); ✓ x ▷ printsAs("<a><abced>123T4</abced></a>");
  t ▷ replaceChar('t', 3); ✓ x ▷ printsAs("<a><abced>123t4</abced></a>");
  ✓ a ▷ length == 5;       ✓ t ▷ length == 5;

  a ▷ deleteChar(3);       ✓ x ▷ printsAs("<a><abcd>123t4</abcd></a>");
  t ▷ deleteChar(3);       ✓ x ▷ printsAs("<a><abcd>1234</abcd></a>");
  ✓ a ▷ length == 4;       ✓ t ▷ length == 4;

  a ▷ swapChars (3);       ✓ x ▷ printsAs("<a><abdc>1234</abdc></a>");
  t ▷ swapChars (1);       ✓ x ▷ printsAs("<a><abdc>2134</abdc></a>");
  ✓ a ▷ length == 4;       ✓ t ▷ length == 4;

  x ▷ free;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4,
                           test5, test6, test7, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
