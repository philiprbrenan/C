#line 1 "/home/phil/c/z/xml/xml.c"
//------------------------------------------------------------------------------
// Parse Xml.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <ctype.h>
#include <arenaRedBlackTree.c>
#include <arenaTree.c>
#include <stringBuffer.c>
#include <utilities.c>

#ifndef Xml_included
#define Xml_included

//D1 Structures                                                                 // Structures describing an Arena Tree.

char XmltagName[256];                                                             // Tag name return area as xml tags are short

typedef struct XmlParse                                                           // Xml parse tree
 {const struct ProtoTypes_XmlParse *proto;                                        // Prototypes for methods
  const StringBuffer      fileName;                                             // Source file containing Xml to be parsed
  const StringBuffer      data;                                                 // Xml to be parsed
  const ArenaTree         tree;                                                 // Parse tree created from Xml being parsed
  const ArenaTree         errors;                                               // List of errors in the xml source - the key of the node is the text of the error message, the extra data after the node is XmlError
  const ArenaRedBlackTree possibilities;                                        // Possibility sets for each Dita tag
  const ArenaRedBlackTree first;                                                // First set of possibilities for each Dita tag
  const ArenaRedBlackTree next;                                                 // Next set of possibilities for each Dita child tag under a given parent tag
 } XmlParse;

typedef struct XmlTag                                                             // Tag in an Xml parse tree
 {const struct ProtoTypes_XmlTag *proto;                                          // Prototypes for methods
  const XmlParse            xml;                                                  // Xml parse tree
  ArenaTreeNode           node;                                                 // Arena tree node in the xml parse tree that contains this tag
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
 {return newXmlTag(({struct XmlTag t = {xml: xml, node: node, proto: &ProtoTypes_XmlTag}; t;}));
 }

static XmlParse makeXmlParseFromFile                                                // Make a parse Xml from the contents of a file
 (const char * const sourceFileName)                                            // Name of file holding Xml
 {const typeof(makeStringBufferFromString(sourceFileName)) sourceFN = makeStringBufferFromString(sourceFileName);                   // File to parse
  const typeof(sourceFN.proto->readFile(sourceFN)) sourceBuffer = sourceFN.proto->readFile(sourceFN);                                          // String to parse
  const typeof(makeArenaTree()) t = makeArenaTree();                                              // Parse tree,
  const typeof(makeArenaTree()) errors = makeArenaTree();                                              // Errors list
  const typeof(makeArenaRedBlackTree()) possibilities = makeArenaRedBlackTree();                                      // Single Step Validation
  const typeof(makeArenaRedBlackTree()) first = makeArenaRedBlackTree();                                      // First set of possibilities for each tag
  const typeof(makeArenaRedBlackTree()) next = makeArenaRedBlackTree();                                      // Next set of possibilities for each Dita child tag under a given parent tag
  const XmlParse  x = {data: sourceBuffer, fileName: sourceFN, tree: t, errors: errors, possibilities: possibilities, first: first, next: next, proto: &ProtoTypes_XmlParse};
  typeof(t.proto->root(t)) P = t.proto->root(t);                                                                 // Current parent node

  const typeof(sourceBuffer.string.proto->first(sourceBuffer.string)) sourceBufferFirst = sourceBuffer.string.proto->first(sourceBuffer.string);                              // First node in the strin buffer holding the source to be parsed
  typeof(sourceBufferFirst.proto->key(sourceBufferFirst)) source = sourceBufferFirst.proto->key(sourceBufferFirst);                                // The source is all in one node so we use it in the string buffer rather than copying it out.

  XmlParse error                                                                  // Report an error message adding the file name involved on the following line
   (const char * const p,                                                       // Pointer to position at which the error occurred
    const char * const format, ...)                                             // Message text
   {va_list va;

    const typeof(makeStringBuffer()) m = makeStringBuffer();                                                     // Build the error message
    const typeof(p - source) o = p - source;                                                             // Offset of the error
    va_start(va, format);
    m.proto->addVaFormat(m, format, va);                                                // Format error message in a buffer of adequate size
    va_end(va);
    m.proto->add(m, "\n  File     : "); m.proto->addStringBuffer(m, sourceFN);            // Add file name
    m.proto->addFormat(m, "\n  At offset: %lu\n", o);                                   // Add offset in file

    const typeof(errors.proto->nodeFromStringBuffer(errors, m)) n = errors.proto->nodeFromStringBuffer(errors, m);                                       // Save the text of the error message as the key of a node
    n.proto->setData(n, o);                                                             // Save the offset of the error in the node data offset.
    n.proto->putTreeLast(n);                                                            // Add the error to the error list
    m.proto->free(m);                                                                   // Free string buffer

    return x;
   } // error

  typeof(source) p = source;                                                                   // Start of text to be parsed
  if  (*p != XmlOpen) return error(p, "Xml must start with: %c", XmlOpen);            // Insist that the first character is <

  int remainderIsWhiteSpace(const char *p)                                      // Find the next non space character in a zero terminated string
   {for(; *p; ++p) if (!isspace(*p)) return 0;                                  // Non white space
    return 1;                                                                   // Its all white sace
   }

  for(char * p = source; *p;)                                                   // Break out tags and text
   {const typeof(strchr(p, XmlOpen)) o = strchr(p, XmlOpen);                                                       // Find next open

    if (o)                                                                      // Found next open
     {if (o > p)
       {int allBlank()                                                          // Check whether the text to be saved is all white space. We can recover the position of non whitespace from the offset saved in the data field of the node.
         {for(const char * q = p; q < o; ++q) if (!isspace(*q)) return 0;
          return 1;
          }

        if (!allBlank())                                                        // Save the  text as the key of the node
         {const typeof(t.proto->noden(t, p, o - p)) n = t.proto->noden(t, p, o - p);                                              // Create node
          P.proto->putLast(P, n);                                                       // Save node in parse tree
          P.proto->setData(P, p - source);                                              // Save text offset
         }
       }

          const typeof(strchr(o, XmlClose)) c = strchr(o, XmlClose);                                                // Save tag: find corresponding close
      if (c)                                                                    // Found closing >
       {ArenaTreeNode save(void)                                                // Save tag as a new node
         {const typeof(t.proto->noden(t, o, c - o + 1)) n = t.proto->noden(t, o, c - o + 1);                                          // Create node
          P.proto->putLast(P, n);                                                       // Save node in parse tree
          P.proto->setData(P, o - source);                                              // Save text offset in parse tree
          return n;
         }

        void end(void)                                                          // End tag
         {lsprintf(a, 256, "%s", parseXmlTagName(P.proto->key(P)));                       // Start tag name
          lsprintf(b, 256, "%s", parseXmlTagName(o));                             // End tag name

          if (strcmp(a, b))                                                     // Tag name mismatch
           {error(o, "End tag: %s does not match start tag: %s", b, a);
           }

          if (!P.proto->isRoot(P)) P = P.proto->parent(P);                                      // Go up one level if possible
          else if (remainderIsWhiteSpace(p)) {}                                 // On root - ignore trailing white space
          else error(o, "Ignoring text beyond closing tag");                    // On root with remaining text
         }

        const char oo = *(o+1), cc = *(c-1);                                    // First character after <, last character before >
        if        (oo == XmlSlash)     end();                                     // Closing tag
        else if   (oo == XmlQuestion) {}                                          // Ignore ?xml
        else if   (oo == XmlExclaim)  {}                                          // Ignore !DOCTYPE
        else if   (cc == XmlSlash)     save();                                    // Unitag
        else P = save();                                                        // Opening tag - make current node the current parent
        p = c + 1;                                                              // Start of text
       }

      else return error(o, "Cannot find closing: %c", XmlClose);                  // No closing > present
     }
    else                                                                        // Check that trailing text is all spaces
     {if (!remainderIsWhiteSpace(p)) error(p, "Ignoring text at end");
      break;
     }
   }

  if (1)                                                                        // Confirm there is exactly one root node
   {const typeof(t.proto->root(t)) root = t.proto->root(t);
    const typeof(root.proto->countChildren(root)) N = root.proto->countChildren(root);
    if (N == 0)     return error(source, "No xml root tag found");
    else if (N > 1) return error(source, "More than one root xml tag found");
   }

  if (1)                                                                        // Make the single root xml tag the root of the parse tree
   {const typeof(t.proto->root(t)) r = t.proto->root(t);
    const typeof(r.proto->first(r)) f = r.proto->first(r);
    ArenaTreeContent * const a = r.proto->content(r), * const b = f.proto->content(f);
    *a = *b;
    ArenaTreefe(c, r) c.proto->setUp(c, r);
   }

  sourceFN.proto->free(sourceFN); sourceBuffer.proto->free(sourceBuffer);                                         // Free mapped input file as we can now reconstruct it from the parse tree

  return x;
 } // makeXmlParseFromFile

static XmlParse makeXmlParseFromString                                              // Make a parse Xml from a string.
 (const char * const string)                                                    // String of xml
 {const typeof(makeStringBufferFromString(string)) f = makeStringBufferFromString(string);                                       // File base name
  const typeof(f.proto->writeTemporaryFile(f, "zzz.xml")) t = f.proto->writeTemporaryFile(f, "zzz.xml");                                        // Create a temporary file
  char T[t.proto->length(t) + 1]; t.proto->string(t, T);                                        // Name of temporary file
  const typeof(makeXmlParseFromFile(T)) x = makeXmlParseFromFile(T);                                                    // Parse temporary file
  unlink(T); f.proto->free(f); t.proto->free(t);                                                // Unlink temporary file and free string buffers
  return x;
 }

static void free_XmlParse                                                         // Free an xml parse
 (const XmlParse x)                                                               // Xml descriptor
 {const typeof(x.tree) t = x.tree; const typeof(x.errors) e = x.errors;
  const typeof(x.possibilities) p = x.possibilities;
  const typeof(x.first) f = x.first; const typeof(x.next) n = x.next;
  t.proto->free(t); e.proto->free(e); p.proto->free(p); f.proto->free(f); n.proto->free(n);
 }

static size_t errors_XmlParse                                                     // Number of errors encountered while creating an Xml parse tree.
 (const XmlParse x)                                                               // Xml descriptor
 {return x.errors.proto->count(x.errors);
 }

static char  * parseXmlTagName                                                    // Get the tag name from an Xml tag
 (const char * const tagString)                                                 // String representation of a tag
 {const char * const p = tagString;
  if (*p == XmlOpen)                                                              // Tag
   {const size_t s = strspn(p, "< /?!"), f = strcspn(p+s, "> /?!");             // Start and finish of tag
    if (f > 0)
     {if (f < sizeof(XmltagName))                                                 // Return tag name
       { const typeof(stpncpy(XmltagName, p+s, f)) q = stpncpy(XmltagName, p+s, f);
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

#define Xmlfe( child, parent) for(XmlTag child = newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.proto->first(parent).node, proto: &ProtoTypes_XmlTag}; t;})); child.proto->valid(child); child.node.offset = child.proto->next(child).node.offset)  // Each child in a parent from first to last
#define Xmlfer(child, parent) for(XmlTag child = newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.proto->last(parent).node, proto: &ProtoTypes_XmlTag}; t;}));  child.proto->valid(child); child.node.offset = child.proto->prev(child).node.offset) // Each child in a parent from last to first

static XmlTag first_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag parent)                                                            // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.node.proto->first(parent.node), proto: &ProtoTypes_XmlTag}; t;}));
 }
static XmlTag last_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag parent)                                                            // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.node.proto->last(parent.node), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 242 "/home/phil/c/z/xml/xml.c"
static XmlTag next_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag parent)                                                            // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.node.proto->next(parent.node), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 242 "/home/phil/c/z/xml/xml.c"
static XmlTag prev_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag parent)                                                            // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.node.proto->prev(parent.node), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 242 "/home/phil/c/z/xml/xml.c"
static XmlTag parent_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag parent)                                                            // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.node.proto->parent(parent.node), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 242 "/home/phil/c/z/xml/xml.c"

static XmlTag first_XmlParse                                                        // Return the first child tag in the specified Xml parse tree.
 (const XmlParse xml)                                                             // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: xml, node: xml.tree.proto->first(xml.tree), proto: &ProtoTypes_XmlTag}; t;}));
 }
static XmlTag last_XmlParse                                                        // Return the first child tag in the specified Xml parse tree.
 (const XmlParse xml)                                                             // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: xml, node: xml.tree.proto->last(xml.tree), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 248 "/home/phil/c/z/xml/xml.c"

//D1 Location                                                                   // Check the current location in the Xml parse tre

static int isRoot_XmlTag                                                          // Check whether the specified tag is the root tag
 (const XmlTag tag)                                                               // Tag
 {return !tag.proto->valid(tag);
 }

static int isFirst_XmlTag                                                         // Check that the specified tag is first under its parent
 (const XmlTag tag)                                                               // Tag
 {if (tag.proto->isRoot(tag)) return 1;                                                   // The root tag is always first
      const typeof(tag.proto->parent(tag)) parent = tag.proto->parent(tag);
  const typeof(parent.proto->first(parent)) f = parent.proto->first(parent);
  return f.proto->equals(f, tag);
 }
static int isLast_XmlTag                                                         // Check that the specified tag is last under its parent
 (const XmlTag tag)                                                               // Tag
 {if (tag.proto->isRoot(tag)) return 1;                                                   // The root tag is always last
      const typeof(tag.proto->parent(tag)) parent = tag.proto->parent(tag);
  const typeof(parent.proto->last(parent)) f = parent.proto->last(parent);
  return f.proto->equals(f, tag);
 }
#line 264 "/home/phil/c/z/xml/xml.c"

//D1 Text methods                                                               // Methods that operate on text tags

static char * text_string_XmlTag                                                  // Return the text of a text tag if we are on a text tag.
 (const XmlTag tag)                                                               // Tag
 {char * const k = tag.proto->tagString(tag);
  return *k == XmlOpen ? 0 : k;                                                   // If the start of the key is not < then it must be text
 }

static int onlyText_XmlTag                                                        // Return true if a tag contains just one text element and nothing else
 (const XmlTag parent)                                                            // Tag
 {const typeof(parent.proto->countChildren(parent)) n = parent.proto->countChildren(parent);
  if (n != 1) return 0;                                                         // If there is more than one tag than there must be a non text tag present to separate the text
  const typeof(parent.proto->first(parent)) f = parent.proto->first(parent);
  return !!f.proto->text(f);
 }

//D1 Statistics                                                                 // Counts on the tag

static int empty_XmlTag                                                           // Check whether the specified parent tag is empty
 (const XmlTag parent)                                                            // Parent tag
 {return !parent.proto->countChildren(parent);
 }

static size_t openChildren_XmlTag                                                 // Count the nunber of child tags that are open tags
 (const XmlTag parent)                                                            // Parent tag
 {size_t n = 0;
  Xmlfe(child, parent) if (child.proto->countChildren(child) > 0) ++n;
  return n;
 }

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
 {       const typeof(xml.proto->root(xml)) root = xml.proto->root(xml);                                                     // Root tag
  return root.proto->findFirstChild(root, key);                                            // Each child of the parent
 }

static XmlTag findFirstTag_XmlTag_XmlTag_string                                       // Find the first tag with the specified name in the Xml parse tree starting at the specified tag.
 (const XmlTag   parent,                                                          // Parent tag
  const char * const key)                                                       // Name of the tag to find
 {jmp_buf found;
  XmlTag T = newXmlTag(({struct XmlTag t = {xml: parent.xml, proto: &ProtoTypes_XmlTag}; t;}));                                           // Tag found if any

  void find(const XmlTag tag)                                                     // Check whether the name of the tag matches the specified key
   {if (tag.proto->tagNameEquals(tag, key))
     {T.node = tag.node;                                                        // Found matching node
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) parent.proto->by(parent, find);                                        // Search the Xml parse tree
  return T;
 }

static XmlTag findFirstTag_XmlTag_XmlParse_string                                     // Find the first tag in an Xml parse tree with the specified name.
 (const XmlParse       xml,                                                       // Xml parse tree
  const char * const key)                                                       // Name of the tag to find
 {jmp_buf found;
  XmlTag T = newXmlTag(({struct XmlTag t = {xml: xml, proto: &ProtoTypes_XmlTag}; t;}));                                                  // Tag found if any

  void find(const XmlTag tag)                                                     // Check whether the name of the tag matches the specified key
   {if (tag.proto->tagNameEquals(tag, key))
     {T.node = tag.node;                                                        // Found matching node
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) xml.proto->by(xml, find);                                           // Search the Xml parse tree
  return T;
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
 {void f(const ArenaTreeNode node)
   {function(newXmlTag(({struct XmlTag t = {xml: xml, node: node, proto: &ProtoTypes_XmlTag}; t;})));
   }
  xml.tree.proto->by(xml.tree, f);
 }

static  size_t countChildren_size_Xml                                             // Count the number of tags at the first level in an xml parse tree.
 (const XmlParse xml)                                                             // Xml parse
 {       const typeof(xml.tree.proto->root(xml.tree)) root = xml.tree.proto->root(xml.tree);                                                // Root node
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

//D1 Edit                                                                       // Change the structure of an Xml parse tree.

static void changeName_XmlTag                                                     // Change the name of the specified tag.
 (const XmlTag         tag,                                                       // Tag
  const char * const newName)                                                   // New name                                                                               //vconst XmlTag t)                                                                 // Tag
 {const typeof(tag.proto->tagName(tag)) oldName = tag.proto->tagName(tag);                                                      // Old name
  const typeof(tag.proto->tagString(tag)) string = tag.proto->tagString(tag);                                                    // Tag string
  const typeof(strlen(oldName)) o = strlen(oldName); const typeof(strlen(newName)) n = strlen(newName); const typeof(strlen(string)) l = strlen(string);

  char key[l + n + 1], * p = key, * const q = strstr(string, oldName);
  p = stpncpy(p, string, q - string);
  p = stpcpy (p, newName);
  p = stpcpy (p, q + o);
  tag.node.proto->setKey(tag.node, key);
 }

//D1 Wrap and Unwrap                                                            // Wrap and unwrap nodes

static XmlTag wrap_XmlTag_string                                                    // Wrap a specified tag with a new tag and return the newly createdf wraping tag.
 (const XmlTag         tag,                                                       // Tag
  const char * const string)                                                    // Wrapper without the leading < or trailing or >
 {if (tag.proto->isRoot(tag)) return tag;
  char s[strlen(string)+4], *p = s;
  *p++ = XmlOpen;
   p   = stpcpy(p, string);
  *p++ = XmlClose;
  *p   = 0;

  const ArenaTreeNode n =  tag.node.proto->wrap(tag.node, s);
  return newXmlTag(({struct XmlTag t = {xml: tag.xml, node: n, proto: &ProtoTypes_XmlTag}; t;}));
 }

static XmlTag unwrap_XmlTag                                                         // Unwrap the specified tag.
 (const XmlTag         tag)                                                       // Tag
 {if (!tag.proto->isRoot(tag)) tag.node.proto->unwrap(tag.node);                                         // Cannot unwrap the root tag
  return tag;
 }

//D1 Print                                                                      // Print an Xml parse tree starting at the specified tag.

static StringBuffer prettyPrint_stringBuffer_XmlTag                               // Print the Xml parse tree starting at the specified tag with additional spacing between tags to make the tree easier to read.
 (const XmlTag tag)                                                               // Starting tag
 {const StringBuffer p = makeStringBuffer();

  void print(const XmlTag parent, const int depth)                                // Print the specified parent and its children
   {void space()                                                                // Format with white space
     {p.proto->add(p, "\n");
      for(int i = 0; i < depth; ++i) p.proto->add(p, " ");
     }
    void open()                                                                 // Add close tag
     {p.proto->add(p, parent.proto->tagString(parent));
     }
    void close()                                                                // Add close tag
     {p.proto->addFormat(p, "%c%c%s%c", XmlOpen, XmlSlash, parent.proto->tagName(parent), XmlClose);
     }
    if      ( parent.proto->empty(parent)) open();                                           // Parent is empty
    else if (!parent.proto->openChildren(parent))                                            // Parent contains only text and singletons
     {open();
      Xmlfe(child, parent) p.proto->add(p, child.proto->tagString(child));
      close();
     }
    else                                                                        // Parent contains open tags
     {space(); open();
      Xmlfe(child, parent) print(child, depth+1);
      space(); close();
      if (!parent.proto->isLast(parent)) space();                                            // If we are last the next end tag will be offset anyway
     }
   }

  print(tag, 0);
  return p;
 }

static StringBuffer prettyPrint_stringBuffer_Xml                                  // Print the Xml parse tree with additional spacing between tags to make the tree easier to read.
 (const XmlParse xml)                                                             // Xml parse tree
 {       const typeof(xml.proto->root(xml)) root = xml.proto->root(xml);
  return root.proto->prettyPrint(root);
 }

static int prettyPrintsAs_int_Xml_string                                          // Check that the Xml parse tree prints as expected
 (const XmlParse xml,                                                             // Xml parse tree
  const char * const expected)                                                  // Expected pretty print
 {const typeof(xml.proto->prettyPrint(xml)) s = xml.proto->prettyPrint(xml);

  if (!s.proto->equalsString(s, expected))                                              // Strings do not match
   {const typeof(s.proto->length(s)) N = s.proto->length(s);
    char S[N+1]; s.proto->string(s, S);
    for(size_t i = 0; i < N; ++i)
     {const char a = *(S+i), b = *(expected+i);
      if (a != b)
       {say("Strings differ in prettyPrintsAs"
            " at position: %lu on characters:  %c and %c\n", i, a, b);
        s.proto->free(s);
        return 0;
       }
     }
   }

  s.proto->free(s);
  return 1;
 }
static int prettyPrintsAs_int_XmlTag_string                                          // Check that the Xml parse tree prints as expected
 (const XmlTag tag,                                                             // Xml parse tree
  const char * const expected)                                                  // Expected pretty print
 {const typeof(tag.proto->prettyPrint(tag)) s = tag.proto->prettyPrint(tag);

  if (!s.proto->equalsString(s, expected))                                              // Strings do not match
   {const typeof(s.proto->length(s)) N = s.proto->length(s);
    char S[N+1]; s.proto->string(s, S);
    for(size_t i = 0; i < N; ++i)
     {const char a = *(S+i), b = *(expected+i);
      if (a != b)
       {say("Strings differ in prettyPrintsAs"
            " at position: %lu on characters:  %c and %c\n", i, a, b);
        s.proto->free(s);
        return 0;
       }
     }
   }

  s.proto->free(s);
  return 1;
 }
#line 511 "/home/phil/c/z/xml/xml.c"

static void prettyPrintAssert_XmlTag                                              // Pretty print the Xml parse tree starting at the specified tag as an assert statement
 (const XmlTag         tag,                                                       // Starting tag
  const char * const variable)                                                  // The name of the variable preceding this call
 {const typeof(tag.proto->prettyPrint(tag)) t = tag.proto->prettyPrint(tag);
  t.proto->splitLines(t);

  const typeof(makeStringBuffer()) s = makeStringBuffer();
  s.proto->addFormat(s, "assert(prettyPrintsAs_int_Xml_string(%s,\n", variable);
  ArenaTreefe(line, t.string)
   {const typeof(line.proto->key(line)) k = line.proto->key(line);
    const typeof(strlen(k)) N = strlen(k);
    char l[N]; strncpy(l, k, N); if (l[N-1] == '\n') l[N-1] = 0;                // Remove any end of line
    s.proto->addDoubleQuote(s);
    s.proto->add(s, l);
    s.proto->addQuotedNewLine(s);
    s.proto->addDoubleQuote(s);
    s.proto->addNewLine(s);
   }
  s.proto->add(s, "));\n\n");
  s.proto->writeFile(s, "/home/phil/c/z/z/zzz.txt");
  t.proto->free(t); s.proto->free(s);
 }

static void prettyPrintAssert_Xml_string                                          // Pretty print the Xml parse tree as an assert statement
 (const XmlParse       xml,                                                       // Xml
  const char * const variable)                                                  // The name of the variable preceding this call
 {const XmlTag t = xml.proto->root(xml);
  t.proto->prettyPrintAssert(t, variable);
 }

static StringBuffer print_stringBuffer_XmlTag                                     // Print the parse tree as a string buffer starting at the specified tag.
 (const XmlTag tag)                                                               // Starting tag
 {const ArenaTreeNode n = tag.node;
  return              n.proto->print(n);
 }

static StringBuffer print_stringBuffer_XmlParse                                   // Print an entire Xml parse tree as a string.
 (const XmlParse        xml)                                                      // Xml parse tree
 {const ArenaTree     t = xml.tree;
  const ArenaTreeNode r = t.proto->root(t);
  return              r.proto->print(r);
 }

static int printsAs_XmlTag                                                        // Check the print of an Xml parse tree starting at the specified tag is as expected.
 (const XmlTag          tag,                                                      // Starting tag
  const char *  const expected)                                                 // Expected string
 {const StringBuffer s = tag.proto->print(tag);
  const int       r = s.proto->equalsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printsAs_XmlParse                                                      // Check the print of an Xml parse tree is as expected.
 (const XmlParse       xml,                                                       // Xml parse tree
  const char * const expected)                                                  // Expected string
 {const StringBuffer s = xml.proto->print(xml);
  const int       r = s.proto->equalsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printContains_XmlTag                                                   // Check the print of an Xml parse tree starting at the specified tag contains the expected string
 (const XmlTag    tag,                                                            // Starting tag
  const char *  const expected)                                                 // Expected string
 {const StringBuffer s = tag.proto->print(tag);
  const int       r = s.proto->containsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printContains_XmlParse                                                 // Check the print of an Xml parse tree contains the expected string.
 (const XmlParse       xml,                                                       // Xml parse tree
  const char * const expected)                                                  // Expected string
 {const StringBuffer s = xml.proto->print(xml);
  const int       r = s.proto->containsString(s, expected);
  s.proto->free(s);
  return r;
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

static int develop()                                                            // Test whether we are local or on github
 {return !strcmp(getenv("HOME"), "/home/phil");
 }

void test0()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer //Terrors //TmakeXmlParseFromFile
 {XmlParse x = makeXmlParseFromString("<a>aa<b>bb<c/>cc</b>dd<d><e/><f>F</f><g/></d><h/><i/></A>h");

  assert(x.proto->prettyPrintsAs(x, "\n"
"<a>aa<b>bb<c/>cc</b>dd\n"
" <d><e/><f>F</f><g/>\n"
" </d>\n"
" <h/><i/>\n"
"</a>\n"));

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
 } // test0

void test1()                                                                    //Tfirst //Tlast //Tprev //Tnext //Tequals //Tcount //TcountChildren //TfindFirstTag //TfindFirstChild //TmakeXmlParseFromString //TparseXmlTagName //TtagName //TtagNameEquals //Tvalid //TtagString //TtagStringEquals //Tparent //Troot //Twrap //Tunwrap //TchangeName
 {XmlParse x = makeXmlParseFromString
   ("<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j></j></a>");

  assert(!x.proto->errors(x));
//        x.proto->prettyPrintAssert(x, "x");
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

  const typeof(x.proto->root(x)) a = x.proto->root(x);                assert(!a.proto->valid(a)); assert(a.proto->tagNameEquals(a, "a"));
  const typeof(x.proto->findFirstTag(x, "b")) b = x.proto->findFirstTag(x, "b"); assert( b.proto->valid(b)); assert(b.proto->tagNameEquals(b, "b")); assert(a.proto->equals(a, b.proto->parent(b)));
  const typeof(x.proto->findFirstTag(x, "c")) c = x.proto->findFirstTag(x, "c"); assert( c.proto->valid(c)); assert(c.proto->tagNameEquals(c, "c")); assert(b.proto->equals(b, c.proto->parent(c)));
  const typeof(b.proto->findFirstChild(b, "d")) d = b.proto->findFirstChild(b, "d"); assert( d.proto->valid(d)); assert(d.proto->tagNameEquals(d, "d")); assert(b.proto->equals(b, d.proto->parent(d)));
  const typeof(d.proto->findFirstChild(d, "e")) e = d.proto->findFirstChild(d, "e"); assert( e.proto->valid(e)); assert(e.proto->tagNameEquals(e, "e")); assert(d.proto->equals(d, e.proto->parent(e)));
  const typeof(d.proto->findFirstChild(d, "f")) f = d.proto->findFirstChild(d, "f"); assert( f.proto->valid(f)); assert(f.proto->tagNameEquals(f, "f")); assert(d.proto->equals(d, f.proto->parent(f)));
  const typeof(d.proto->findFirstChild(d, "g")) g = d.proto->findFirstChild(d, "g"); assert( g.proto->valid(g)); assert(g.proto->tagNameEquals(g, "g")); assert(d.proto->equals(d, g.proto->parent(g)));
  const typeof(b.proto->findFirstChild(b, "h")) h = b.proto->findFirstChild(b, "h"); assert( h.proto->valid(h)); assert(h.proto->tagNameEquals(h, "h")); assert(b.proto->equals(b, h.proto->parent(h)));
  const typeof(x.proto->findFirstTag(x, "i")) i = x.proto->findFirstTag(x, "i"); assert( i.proto->valid(i)); assert(i.proto->tagNameEquals(i, "i")); assert(a.proto->equals(a, i.proto->parent(i)));
  const typeof(x.proto->findFirstTag(x, "j")) j = x.proto->findFirstTag(x, "j"); assert( j.proto->valid(j)); assert(j.proto->tagNameEquals(j, "j")); assert(a.proto->equals(a, j.proto->parent(j)));

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

  assert(a.proto->equals(a, b.proto->parent(b)));

  XmlTag F = f.proto->wrap(f, "FFFF id='1'");
//d.proto->prettyPrintAssert(d, "d");
  assert(prettyPrintsAs_int_XmlTag_string(d,
"\n"
"<d><e/>e<FFFF id='1'><f/></FFFF>f<g>g</g>\n"
"</d>\n"
));

  F.proto->changeName(F, "NNNN");
//d.proto->prettyPrintAssert(d, "d");
  assert(prettyPrintsAs_int_XmlTag_string(d,
"\n"
"<d><e/>e<NNNN id='1'><f/></NNNN>f<g>g</g>\n"
"</d>\n"
));

  F.proto->unwrap(F);
//d.proto->prettyPrintAssert(d, "d");
  assert(prettyPrintsAs_int_XmlTag_string(d,
"\n"
"<d><e/>e<f/>f<g>g</g>\n"
"</d>\n"
));

  x.proto->free(x);
 } // test1

void test2()                                                                    //Tprint
 {char file[128] =  "/home/phil/c/z/xml/samples/foreword.dita";
  if (!develop()) strcpy(file, "/home/runner/work/C/C/c/z/xml/samples/foreword.dita");

  XmlParse x = makeXmlParseFromFile(file);

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
 } // test2

void test3()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {char *file =       "/home/phil/c/z/xml/validation/validation.xml";
  if (!develop()) return;                                                       // Does not work on gitHub - possibly out of memory or Cpu?
  XmlParse xml = makeXmlParseFromFile(file);

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
 } // test3

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, 0};
//{void (*tests[])(void) = {test0, 0};
  run_tests("Xml", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
