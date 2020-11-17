#line 1 "/home/phil/c/z/xml/xml.c"
//------------------------------------------------------------------------------
// Parse Xml.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <arenaTree.c>
#include <rectangle.c>
#include <stringBuffer.c>
#include <utilities.c>

#ifndef Xml_included
#define Xml_included

//D1 Structures                                                                 // Structures in memory describing an Xml parse tree.

#define XmltagNameLength 1024                                                     // Tag name return area as xml tags are of limited size
char XmltagName[XmltagNameLength+1];                                                // Tag name return area as xml tags are short

typedef struct XmlParse                                                           // Xml parse tree
 {const struct ProtoTypes_XmlParse *proto;                                        // Prototypes for methods
  const StringBuffer      fileName;                                             // Source file containing Xml to be parsed or zero if there is no file associated with the parse
  const ArenaList         tree;                                                 // Parse tree created from Xml being parsed
  const ArenaList         errors;                                               // List of errors in the xml source - the key of the node is the text of the error message, the extra data after the node is XmlError
 } XmlParse;

typedef struct XmlValidate                                                        // Xml single step validation
 {const struct ProtoTypes_XmlValidate *proto;                                     // Prototypes for methods
  ArenaTree    possibilities;                                                   // Possibility sets for each Dita tag
  ArenaTree    first;                                                           // First set of possibilities for each Dita tag
  ArenaTree    next;                                                            // Next set of possibilities for each Dita child tag under a given parent tag
 } XmlValidate;

typedef struct XmlTag                                                             // Tag in an Xml parse tree
 {const struct ProtoTypes_XmlTag *proto;                                          // Prototypes for methods
  const XmlParse            xml;                                                  // Xml parse tree
  ArenaListNode           node;                                                 // Arena tree node in the xml parse tree that contains this tag
 } XmlTag;

typedef struct XmlNodeExt                                                         // Extra data held in each node of the parse tree
 {unsigned char           text;                                                 // True if this is a text node
 } XmlNodeExt;

typedef struct XmlParseOptions                                                    // Options for an Xml parse
 {const struct ProtoTypes_XmlParseOptions *proto;                                 // Prototypes for methods
  const char  *           location;                                             // Location of string in memory to be parsed
  const size_t            length;                                               // Length of string ot be parsed
  const size_t            width;                                                // Size of the extra data attached to each parse tree node
  StringBuffer            fileName;                                             // File name optionally associated with parse
 } XmlParseOptions;

#include <xml_prototypes.h>
#define makeLocalCopyOfXmlTagString(string,stringLength,tag) const typeof(content__ArenaListNode(&tag.node)->length) stringLength = content__ArenaListNode(&tag.node)->length; char string[stringLength+1]; string[stringLength] = 0; memcpy(string, key_pointer__ArenaListNode(&tag.node), stringLength); // Copy the key and the length of the key of the specified node to the stack.

#define XmlOpen     '<'                                                           // Important Xml characters
#define XmlClose    '>'
#define XmlSlash    '/'
#define XmlQuestion '?'
#define XmlExclaim  '!'

XmlParse makeXmlParse                                                               // Perform requested parse
 (XmlParseOptions options)                                                        // Name of file holding Xml or zero if no file is associated with this source
 {const typeof(options.fileName) sourceFN = options.fileName;                                             // File to parse if any
  const typeof(makeArenaListWithWidth(sizeof(XmlNodeExt))) parseTree = makeArenaListWithWidth(sizeof(XmlNodeExt));                     // Parse tree which has nodes with extension data
  const typeof(makeArenaListWithWidth(sizeof(size_t))) errors = makeArenaListWithWidth(sizeof(size_t));                       // Errors list with location of error
  XmlParse xml    = {fileName: sourceFN, tree: parseTree,                         // Create Xml descriptor
                   errors: errors, proto: &ProtoTypes_XmlParse};

  const typeof(options.location) source = options.location; const typeof(options.length) length = options.length;                           // Zero terminated string to parse
  assert(!source[length]);                                                      // Check we have a trailing zero so that the following loop terminates

  typeof(parseTree.proto->root(&parseTree)) currentParent = parseTree.proto->root(&parseTree);                                             // Current parent node

  XmlParse error                                                                  // Report an error message adding the file name involved on the following line
   (const char * const p,                                                       // Pointer to position at which the error occurred
    const char * const format, ...)                                             // Message text
   {va_list va;

    const typeof(makeStringBuffer()) m = makeStringBuffer();                                                     // Build the error message
    const typeof(p - source) o = p - source;                                                             // Offset of the error

    va_start(va, format); m.proto->addVaFormat(&m, format, va); va_end(va);              // Format error message in a buffer of adequate size
    if (sourceFN.proto)                                                         // Add file name
     {m.proto->add(&m, "\n  File     : ");
      m.proto->addStringBuffer(&m, sourceFN);
     }
    m.proto->addFormat(&m, "\n  At offset: %lu\n", o);                                   // Add offset in file
    makeLocalCopyOfStringBuffer(M, L, m);                                       // The error message

    const typeof(errors.proto->node(&errors, M, L)) n = errors.proto->node(&errors, M, L);                                                    // Save the error message
    n.proto->putTreeLast(&n);                                                            // Add the error to the error list
    *(size_t *)(n.proto->data(&n)) = o;                                                  // Save offset of error

    m.proto->free(&m);                                                                   // Free string buffer

    return xml;
   } // error

  if  (*source != XmlOpen) return error(0, "Xml must start with: %c", XmlOpen);       // Insist that the first character is <

  int remainderIsWhiteSpace(const char *p)                                      // Find the next non space character in a zero terminated string
   {for(; *p; ++p) if (!isspace(*p)) return 0;                                  // Non white space
    return 1;                                                                   // Its all white sace
   } // remainderIsWhiteSpace

  for(const char * p = source; *p;)                                             // Break out tags and text
   {const typeof(strchr(p, XmlOpen)) o = strchr(p, XmlOpen);                                                       // Find next open

    if (o)                                                                      // Found next open
     {if (o > p)
       {int allBlank()                                                          // Check whether the text to be saved is all white space. We can recover the position of non whitespace from the offset saved in the data field of the node.
         {for(const char * q = p; q < o; ++q) if (!isspace(*q)) return 0;
          return 1;
          }

        if (!allBlank())                                                        // Save the text as the key of the node
         {typeof(parseTree.proto->node(&parseTree, p, o - p)) n = parseTree.proto->node(&parseTree, p, o - p);                                       // Create node
          XmlNodeExt e = {text: 1};                                               // Text marker
          n.proto->setData(&n, &e);                                                      // Save  text marker
          currentParent.proto->putLast(&currentParent, n);                                           // Save node in parse tree
         }
       }
          const typeof(strchr(o, XmlClose)) c = strchr(o, XmlClose);                                                // Save tag: find corresponding close
      if (c)                                                                    // Found closing >
       {void save(void)                                                         // Save tag as a new node
         {const typeof(parseTree.proto->node(&parseTree, o + 1, c - o - 1)) n = parseTree.proto->node(&parseTree, o + 1, c - o - 1);                               // Create node
          currentParent.proto->putLast(&currentParent, n);                                           // Save node in parse tree
          currentParent = n;
         }
        void saveSingleton(void)                                                // Save singleton
         {const size_t l = c - o - 2;                                           // Length of current tag string
          char b[l+1]; memcpy(b, o+1, l); b[l] = 0;                             // Copy tag string
          const typeof(parseTree.proto->node(&parseTree, b, l)) n = parseTree.proto->node(&parseTree, b, l);                                           // Create node
          currentParent.proto->putLast(&currentParent, n);                                           // Save node in parse tree
         }

        void end(void)                                                          // End tag
         {makeLocalCopyOfArenaListKey(k, l, currentParent);
          char a[XmltagNameLength+1]; strcpy(a, parseXmlTagName(k, l));             // Start tag name - we have to make copies because there is only one tag name buffer
          char *b = parseXmlTagNameInAngleBrackets(o);                            // End tag name

          if (strcmp(a, b))                                                     // Tag name mismatch
           {error(o, "End tag: %s does not match start tag: %s", b, a);
           }

          if (!currentParent.proto->isRoot(&currentParent)) currentParent = currentParent.proto->parent(&currentParent);  // Go up one level if possible
          else if (remainderIsWhiteSpace(p)) {}                                 // On root - ignore trailing white space
          else error(o, "Ignoring text beyond closing tag");                    // On root with remaining text
         }

        const char oo = *(o+1), cc = *(c-1);                                    // First character after <, last character before >
        if        (oo == XmlSlash)     end();                                     // Closing tag
        else if   (oo == XmlQuestion) {}                                          // Ignore ?xml
        else if   (oo == XmlExclaim)  {}                                          // Ignore !DOCTYPE
        else if   (cc == XmlSlash)     saveSingleton();                           // Singleton
        else save();                                                            // Opening tag - make current node the current parent
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
   {const typeof(parseTree.proto->root(&parseTree)) root = parseTree.proto->root(&parseTree);
    const typeof(root.proto->countChildren(&root)) N = root.proto->countChildren(&root);
    if (N == 0)     return error(source, "No xml root tag found");
    else if (N > 1) return error(source, "More than one root xml tag found");
   }

//  if (0)                                                                        // Make the single root xml tag the root of the parse tree
//   {const typeof(parseTree.proto->root(&parseTree)) r = parseTree.proto->root(&parseTree);
//    const typeof(r.proto->first(&r)) f = r.proto->first(&r);
//    ArenaListContent * const a = r.proto->content(&r), * const b = f.proto->content(&f);
//    *a = *b;
//    ArenaListfe(c, r) c.proto->setUp(&c, r);
//   }

  return xml;
 } // makeXmlParse

XmlParse makeXmlParseFromFile                                                       // Parse Xml held in a file
 (const char * const sourceFileName,                                            // Name of file holding Xml to be parsed
  const size_t width)                                                           // Size of extra data associated with each node in the parse tree
 {XmlParse p;                                                                     // The resulting parse

  ssize_t reader(char * location, size_t length)                                // Function to process the mapped file. Return non negative on success, negative on failure: this value will be returned to the caller.
   {StringBuffer  f = makeStringBufferFromString(sourceFileName);
    XmlParseOptions o = newXmlParseOptions(({struct XmlParseOptions t = {location: location, length: length, width: width, fileName: f, proto: &ProtoTypes_XmlParseOptions}; t;}));
    XmlParse        q = makeXmlParse(o);                                            // Parse
    memcpy((void *)&p, (void *)&q, sizeof(XmlParse));                             // Temporarily subvert the const settings to return the results
    return 1;
   }

  readFile(sourceFileName, reader);                                             // Read the source and parse it

  return p;
 }

XmlParse makeXmlParseFromString                                                     // Parse Xml  held in a string
 (const char * const source,                                                    // Source text to be parsed
  const size_t       length,                                                    // Length of source text to be parsed
  const size_t       width)                                                     // Size of extra data associated with each node in the parse tree
 {XmlParseOptions o = newXmlParseOptions(({struct XmlParseOptions t = {location: source, length: length, width: width, proto: &ProtoTypes_XmlParseOptions}; t;}));
  return makeXmlParse(o);                                                         // Parse
 }

XmlValidate makeXmlValidate()                                                       // Make an Xml single step validator
 {const typeof(makeArenaTree()) possibilities = makeArenaTree();                                              // Single Step Validation
  const typeof(makeArenaTreeWithWidth(sizeof(size_t))) first = makeArenaTreeWithWidth(sizeof(size_t));                       // First set of possibilities for each tag
  const typeof(makeArenaTreeWithWidth(sizeof(size_t))) next = makeArenaTreeWithWidth(sizeof(size_t));                       // Next set of possibilities for each Dita child tag under a given parent tag
  XmlValidate valid = {possibilities: possibilities,
    first: first, next: next, proto: &ProtoTypes_XmlValidate};
  return valid;
 }

static XmlParse  parseXmlFromString                                                 // Make a parse Xml from a string.
 (const char * const string,                                                    // String of Xml
  const size_t       width)                                                     // Size of extra data associated with each node in the parse tree
 {return makeXmlParseFromString(string, strlen(string), width);
 }

static void free_XmlParse                                                         // Free an Xml parse
 (const XmlParse * x)                                                             // Xml descriptor
 {x->tree.proto->free(&x->tree); x->errors.proto->free(&x->errors); if (x->fileName.proto) x->fileName.proto->free(&x->fileName);
 }

static void free_XmlValidate                                                      // Free an Xml validator
 (XmlValidate * x)                                                                // Xml validator
 {x->possibilities.proto->free(&x->possibilities);
  x->first.proto->free(&x->first);
  x->next.proto->free(&x->next);
 }

static size_t errors_size_XmlParse                                                // Number of errors encountered while creating an Xml parse tree.
 (const XmlParse * x)                                                             // Xml descriptor
 {return x->errors.proto->count(&x->errors);
 }

static char  * parseXmlTagNameInAngleBrackets                                     //P Get the tag name from an Xml tag string. The tag name will be truncated if it is longer than XmltagNameLength. The returned  name is held in the fixed buffer XmltagName.
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

static  char * parseXmlTagName                                                    //P Get the tag name from a string assuming the tag name starts at the start of the string and is either terminated by a space or the end of the string.  The tag name will be truncated if it is longer than XmltagNameLength. The returned  name is held in the fixed buffer XmltagName.
 (const char * tag,                                                             // Tag
  const size_t length)                                                          // Length
 {size_t N = length < XmltagNameLength ? length : XmltagNameLength;
  for(size_t i = 0; i < N; ++i)                                                 // Copy tag string up to first space
   {XmltagName[i] = tag[i];
    if (tag[i] == ' ') {XmltagName[i] = 0; return XmltagName;}
   }
  XmltagName[N] = 0;
  return XmltagName;
 }

static char * tagName_string_XmlTag                                               // Get the tag name from a node in the Xml parse tree. The tag name is returned in a reused static buffer that the caller should not free.
 (const XmlTag * Tag)                                                             // Tag
 {const typeof(*Tag) tag = *Tag;
  if (tag.proto->isText(&tag)) return "text";                                              // Text node
  makeLocalCopyOfXmlTagString(t, l, tag);
  return parseXmlTagName(t, l);
 }

static int tagNameEquals_int_XmlTag_string                                        // Check the name of a tag.
 (const XmlTag *       tag,                                                       // Tag
  const char * const expected)                                                  // Expected name of tag
 {return !strcmp(tag->proto->tagName(tag), expected);
 }

static char * tagString_string_XmlTag                                             //V Get the entire tag from a node in the Xml parse tree.
 (const XmlTag * tag)                                                             // Tag
 {return tag->node.proto->key(&tag->node);
 }

static size_t tagStringLength_size_XmlTag                                         // Get the length of the entire tag
 (const XmlTag * tag)                                                             // Tag
 {return tag->node.proto->length(&tag->node);
 }

static int tagStringEquals_int_XmlTag_string                                      // Check the string of a tag.
 (const XmlTag *       tag,                                                       // Tag
  const char * const expected)                                                  // Expected name of tag
 {return tag->node.proto->equalsString(&tag->node, expected);
 }

//D1 Navigation                                                                 // Navigate through an Xml parse tree.

#define Xmlfe( child, parent) for(XmlTag child = newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.proto->first(&parent).node, proto: &ProtoTypes_XmlTag}; t;})); child.proto->valid(&child); child.node.offset = child.proto->next(&child).node.offset)  // Each child in a parent from first to last
#define Xmlfer(child, parent) for(XmlTag child = newXmlTag(({struct XmlTag t = {xml: parent.xml, node: parent.proto->last(&parent).node, proto: &ProtoTypes_XmlTag}; t;}));  child.proto->valid(&child); child.node.offset = child.proto->prev(&child).node.offset) // Each child in a parent from last to first

static XmlTag first_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag * parent)                                                          // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent->xml, node: parent->node.proto->first(&parent->node), proto: &ProtoTypes_XmlTag}; t;}));
 }
static XmlTag last_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag * parent)                                                          // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent->xml, node: parent->node.proto->last(&parent->node), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 313 "/home/phil/c/z/xml/xml.c"
static XmlTag next_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag * parent)                                                          // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent->xml, node: parent->node.proto->next(&parent->node), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 313 "/home/phil/c/z/xml/xml.c"
static XmlTag prev_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag * parent)                                                          // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent->xml, node: parent->node.proto->prev(&parent->node), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 313 "/home/phil/c/z/xml/xml.c"
static XmlTag parent_XmlTag                                                          // Return the first child tag under the specified parent tag.
 (const XmlTag * parent)                                                          // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: parent->xml, node: parent->node.proto->parent(&parent->node), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 313 "/home/phil/c/z/xml/xml.c"

static XmlTag first_XmlParse                                                        // Return the first child tag in the specified Xml parse tree.
 (const XmlParse * xml)                                                           // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: *xml, node: xml->tree.proto->first(&xml->tree), proto: &ProtoTypes_XmlTag}; t;}));
 }
static XmlTag last_XmlParse                                                        // Return the first child tag in the specified Xml parse tree.
 (const XmlParse * xml)                                                           // Parent tag
 {return newXmlTag(({struct XmlTag t = {xml: *xml, node: xml->tree.proto->last(&xml->tree), proto: &ProtoTypes_XmlTag}; t;}));
 }
#line 319 "/home/phil/c/z/xml/xml.c"

//D1 Location                                                                   // Check the current location in the Xml parse tre

static int depth_int_XmlTag                                                       // The depth of a tag
 (const XmlTag * Tag)                                                             // Tag
 {typeof(0) i = 0;
  const typeof(*Tag) tag = *Tag;
  for(XmlTag t = tag; t.proto->valid(&t); t.node.offset = t.proto->parent(&t).node.offset) ++i;     // Count up to root
  return i - 1;                                                                 // We have included the root node
 }

static int isRoot_int_XmlTag                                                      // Check whether the specified tag is the root tag
 (const XmlTag * tag)                                                             // Tag
 {const typeof(tag->proto->parent(tag)) p = tag->proto->parent(tag);                                                             // Possibly the root of the ArenaList
  return !p.proto->valid(&p);                                                            // The root of the xml tree is one level below the root of the ArenaList
 }

static int isFirst_int_XmlTag                                                     // Check that the specified tag is first under its parent
 (const XmlTag * Tag)                                                             // Tag
 {const typeof(*Tag) tag = *Tag;
  if (tag.proto->isRoot(&tag)) return 1;                                                   // The root tag is always first
      const typeof(tag.proto->parent(&tag)) parent = tag.proto->parent(&tag);
  const typeof(parent.proto->first(&parent)) f = parent.proto->first(&parent);
  return f.proto->equals(&f, tag);
 }
static int isLast_int_XmlTag                                                     // Check that the specified tag is last under its parent
 (const XmlTag * Tag)                                                             // Tag
 {const typeof(*Tag) tag = *Tag;
  if (tag.proto->isRoot(&tag)) return 1;                                                   // The root tag is always last
      const typeof(tag.proto->parent(&tag)) parent = tag.proto->parent(&tag);
  const typeof(parent.proto->last(&parent)) f = parent.proto->last(&parent);
  return f.proto->equals(&f, tag);
 }
#line 345 "/home/phil/c/z/xml/xml.c"

static size_t charPosition__XmlTag                                                // Character position of a tag assuming all tags are drawn as open/close not as singletons
 (const XmlTag * tag,                                                             // XmlTag
  const int    close)                                                           // 0 - start tag, 1 - close tag
 {typeof(0ul) n = 0ul;
  jmp_buf found;

  void children(ArenaListNode parent, int depth)                                // Process the children of the specified parent
   {if (parent.proto->invalid(&parent)) return;                                               // Empty child
    if (close == 0 && parent.offset == tag->node.offset) longjmp(found, 1);     // Found current tag open
    n += parent.proto->length(&parent);                                                       // Open
    ArenaListfe(child, parent) children(child, depth + 1);                      // Content
    const typeof(newXmlTag(({struct XmlTag t = {xml: tag->xml, node: parent, proto: &ProtoTypes_XmlTag}; t;}))) t = newXmlTag(({struct XmlTag t = {xml: tag->xml, node: parent, proto: &ProtoTypes_XmlTag}; t;}));                                  // Make tag from node
    if (!t.proto->isText(&t))                                                            // Text is always a singleton
     {if (close == 1 && parent.offset == tag->node.offset) longjmp(found, 1);   // Found current tag close
      makeLocalCopyOfArenaListKey(k, l, parent);                                // Close
      parseXmlTagName(k, l);
      n += strlen(XmltagName);
     }
    else if (parent.offset == tag->node.offset) longjmp(found, 1);              // Found current tag
   }

  if (close == 0 && tag->proto->isRoot(tag)) return 0;                                     // Xml root
  const typeof(tag->xml.proto->root(&tag->xml)) root = tag->xml.proto->root(&tag->xml);                                                       // Parse tree
  makeLocalCopyOfArenaListKey(k, l, root.node);                                 // Close
  parseXmlTagName(k, l);
  n += strlen(XmltagName);
  if (!setjmp(found)) Xmlfe(child, root) children(child.node, 1);                 // Start at the root node
  return n;
 }

//D1 Text methods                                                               // Methods that operate on text tags

static int isText_int_XmlTag                                                      // Check whether we are on a text element
 (const XmlTag * tag)                                                             // Tag
 {XmlNodeExt e;                                                                   // Extension data for node
  tag->node.proto->getData(&tag->node, &e);                                                      // Get extension data from node
  return e.text;                                                                // Text or not
 }

static int onlyText_XmlTag                                                        // Return true if a tag contains just one text element and nothing else
 (const XmlTag * parent)                                                          // Tag
 {const typeof(parent->proto->countChildren(parent)) n = parent->proto->countChildren(parent);
  if (n != 1) return 0;                                                         // If there is more than one tag than there must be a non text tag present to separate the text
  const typeof(parent->proto->first(parent)) f = parent->proto->first(parent);
  return f.proto->isText(&f);
 }

//D1 Statistics                                                                 // Counts on the tag

static int empty_int_XmlTag                                                       // Check whether the specified parent tag is empty
 (const XmlTag * parent)                                                          // Parent tag
 {return !parent->proto->countChildren(parent);
 }

static int isTag_int_tag                                                        // Check that the tag is a tag not text
 (const XmlTag * tag)                                                             // Parent tag
 {return !tag->proto->isText(tag);
 }

static int hasText_int_XmlTag                                                     // Check whether the tag contains a text element
 (const XmlTag * Parent)                                                          // Parent tag
 {const typeof(*Parent) parent = *Parent;
  Xmlfe(child, parent) if (child.proto->isText(&child)) return 1;
  return 0;
 }

static int stayInLine_int_XmlTag                                                  // Check whether an opening or singleton tag can stay in line: if it is text or is preceded or followed by text.
 (const XmlTag * Tag)                                                             // Tag
 {const typeof(*Tag) tag = *Tag;
  if ( tag.proto->isText(&tag))                                   return 1;
//const typeof(tag.proto->countChildren(&tag)) n = tag.proto->countChildren(&tag); if (n == 0)                 return 1;                // Need to differentiate between tags that can never have any content and tags that can have content.
  if (!tag.proto->isFirst(&tag)) {const typeof(tag.proto->prev(&tag)) p = tag.proto->prev(&tag); if (p.proto->isText(&p)) return 2;}
  if (!tag.proto->isLast(&tag))  {const typeof(tag.proto->next(&tag)) n = tag.proto->next(&tag); if (n.proto->isText(&n)) return 3;}
  return 0;
 }

static int stayInLineClose_int_XmlTag                                             // Check whether a closing tag can stay inline: if it is preceded or followed by text or is empty, or its last child was text
 (const XmlTag * Tag)                                                             // Tag
 {const typeof(*Tag) tag = *Tag;
  if (tag.proto->stayInLine(&tag)) return 1;
  const typeof(tag.proto->last(&tag)) l = tag.proto->last(&tag);
  return l.proto->valid(&l) && l.proto->isText(&l);
 }

static size_t length_size__XmlTag                                                 // Length of a tag string
 (const XmlTag * tag)                                                             // XmlTag
 {return tag->node.proto->content(&tag->node)->length;
 }

//D1 Search                                                                     // Search the Xml parse tree

static int valid_int_XmlTag                                                       // Check that a tag is valid.
 (const XmlTag * tag)                                                             // Tag
 {return tag->node.offset;                                                      // A tag is valid unless it is the root node of the arena tree backing the Xml parse tree.
 }

static XmlTag findFirstChild_XmlTag_XmlTag_string                                     // Find the first child tag with the specified name under the specified parent tag.
 (const XmlTag *       Parent,                                                    // Parent
  const char * const key)                                                       // Name of the tag to find
 {const typeof(*Parent) parent = *Parent;
  Xmlfe(child, parent)                                                            // Each child of the parent
   {if (child.proto->tagNameEquals(&child, key)) return child;                               // First child with matching tag
   }
  return newXmlTag(({struct XmlTag t = {proto: &ProtoTypes_XmlTag};   t;}));
 }

static XmlTag findFirstChild_XmlTag_XmlParse_string                                   // Find the first child tag with the specified name under the root tag of the specified parse tree.
 (const XmlParse * xml,                                                           // Parent
  const char   * const key)                                                     // Name of the tag to find
 {       const typeof(xml->proto->root(xml)) root = xml->proto->root(xml);                                                     // Root tag
  return root.proto->findFirstChild(&root, key);                                            // Each child of the parent
 }

static XmlTag findFirstTag_XmlTag_XmlTag_string                                       // Find the first tag with the specified name in the Xml parse tree starting at the specified tag.
 (const XmlTag *       parent,                                                    // Parent tag
  const char * const key)                                                       // Name of the tag to find
 {jmp_buf found;
  XmlTag T = newXmlTag(({struct XmlTag t = {xml: parent->xml, proto: &ProtoTypes_XmlTag}; t;}));                                          // Tag found if any

  void find(const XmlTag tag)                                                     // Check whether the name of the tag matches the specified key
   {if (tag.proto->tagNameEquals(&tag, key))
     {T.node = tag.node;                                                        // Found matching node
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) parent->proto->by(parent, find);                                        // Search the Xml parse tree
  return T;
 }

static XmlTag findFirstTag_XmlTag_XmlParse_string                                     // Find the first tag in an Xml parse tree with the specified name.
 (const XmlParse *       xml,                                                     // Xml parse tree
  const char   * const key)                                                     // Name of the tag to find
 {jmp_buf found;
  XmlTag T = newXmlTag(({struct XmlTag t = {xml: *xml, proto: &ProtoTypes_XmlTag}; t;}));                                                 // Tag found if any

  void find(const XmlTag tag)                                                     // Check whether the name of the tag matches the specified key
   {if (tag.proto->tagNameEquals(&tag, key))
     {T.node = tag.node;                                                        // Found matching node
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) xml->proto->by(xml, find);                                           // Search the Xml parse tree
  return T;
 }

static XmlTag root_XmlParse                                                         // Return the root tag of the specified Xml parse tree
 (const XmlParse * xml)                                                           // Xml parse tree
 {const typeof(xml->tree.proto->root(&xml->tree)) root = xml->tree.proto->root(&xml->tree);
  return newXmlTag(({struct XmlTag t = {xml: *xml, node: root.proto->first(&root), proto: &ProtoTypes_XmlTag}; t;}));
 }

static XmlTag root_XmlTag                                                           // Return the root tsg of the Xml parse tree containing the specified tag.
 (const XmlTag * tag)                                                             // Tag
 {return tag->xml.proto->root(&tag->xml);
 }

static int equals_XmlTag_XmlTag                                                     // Confirm that two tags refer to the same text location in the source document.
 (const XmlTag * a,                                                               // First tag
  const XmlTag b)                                                                 // Second tag
 {return a->node.offset == b.node.offset;
 }

//D1 Traverse                                                                   // Traverse the Xml parse tree.

static void by_XmlTag_sub                                                         // Traverse the Xml parse tree rooted at the specified tag in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const XmlTag * tag,                                                             // Starting tag
  void (* const function) (const XmlTag tag))                                     // Function to call on each tag
 {void f(const ArenaListNode node)
   {function(newXmlTag(({struct XmlTag t = {xml: tag->xml, node: node, proto: &ProtoTypes_XmlTag}; t;})));
   }
  tag->node.proto->by(&tag->node, f);
 }

static void by_XmlParse_sub                                                       // Traverse the Xml parse tree in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const XmlParse * xml,                                                           // Xml parse tree
  void (* const function) (const XmlTag tag))                                     // Function to call on each tag
 {void f(const ArenaListNode node)
   {function(newXmlTag(({struct XmlTag t = {xml: *xml, node: node, proto: &ProtoTypes_XmlTag}; t;})));
   }
  const typeof(xml->tree.proto->first(&xml->tree)) F = xml->tree.proto->first(&xml->tree);
  F.proto->by(&F, f);
 }

static void scan_XmlTag_sub                                                       // Traverse the Xml parse tree rooted at the specified tag calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The Xml is buffered allowing changes to be made to the structure of the Xml without disruption as long as each child checks its context.
 (const XmlTag * tag,                                                             // Starting tag
  void (* const function) (XmlTag tag, int start, int depth))                     // Function to call on each tag: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
 {void f(const ArenaListNode node,   int start, int depth)
   {function(newXmlTag(({struct XmlTag t = {xml: tag->xml, node: node, proto: &ProtoTypes_XmlTag}; t;})), start, depth);
   }
  tag->node.proto->scan(&tag->node, f);
 }

static void scan_XmlParse_sub                                                     // Traverse the Xml parse tree calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The Xml is buffered allowing changes to be made to the structure of the Xml without disruption as long as each child checks its context.
 (const XmlParse * xml,                                                           // Xml parse tree
  void (* const function) (XmlTag tag, int start, int depth))                     // Function to call on each tag: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
 {void f(const ArenaListNode node,   int start, int depth)
   {function(newXmlTag(({struct XmlTag t = {xml: *xml, node: node, proto: &ProtoTypes_XmlTag}; t;})), start, depth);
   }
  xml->tree.proto->scan(&xml->tree, f);
 }

static  size_t countChildren_size_Xml                                             // Count the number of tags at the first level in an xml parse tree.
 (const XmlParse * xml)                                                           // Xml parse
 {       const typeof(xml->tree.proto->root(&xml->tree)) root = xml->tree.proto->root(&xml->tree);                                               // Root node
  return root.proto->countChildren(&root) - 1;                                              // Count the level immediately below the root tag of which there is always just one
 }

static  size_t countChildren_size_XmlTag                                          // Count the number of child tags directly under a parent tag
 (const XmlTag * parent)                                                          // Parent tag
 {return parent->node.proto->countChildren(&parent->node);
 }

static  size_t count_size_Xml                                                     // Count the number of tags in an Xml parse tree
 (const XmlParse * x)                                                             // Xml parse tree
 {return x->tree.proto->count(&x->tree);
 }

static  size_t count_size_XmlTag                                                  // Count the number of tags under the specified tag in an Xml parse tree
 (const XmlTag * t)                                                               // Parent tag
 {return t->node.proto->count(&t->node);
 }

//D1 Edit                                                                       // Change the structure of an Xml parse tree.

static void changeName_XmlTag                                                     // Change the name of the specified tag.
 (const XmlTag * Tag,                                                             // Tag
  const char * const newName)                                                   // New name                                                                               //vconst XmlTag t)                                                                 // Tag
 {const typeof(*Tag) tag = *Tag;
  const typeof(tag.proto->tagName(&tag)) oldName = tag.proto->tagName(&tag);                                                      // Old name
  const typeof(tag.proto->tagString(&tag)) string = tag.proto->tagString(&tag);                                                    // Tag string
  const typeof(strlen(oldName)) o = strlen(oldName); const typeof(strlen(newName)) n = strlen(newName); const typeof(strlen(string)) l = strlen(string);
  if (n > o) printStackBackTraceAndExit(1, "New code needed to deal with the case where the new name is longer than the old name\n");

  char key[l + n + 1], * p = key, * const q = strstr(string, oldName);
  p = stpncpy(p, string, q - string);
  p = stpcpy (p, newName);
  p = stpcpy (p, q + o);
  ArenaListNode t = *(ArenaListNode *)(&tag.node);
  t.proto->setKey(&t, key, strlen(key));
 }

static void insertChar__XmlTag_char_size                                          // Insert the specified character into the string of a tag at the specified position.
 (XmlTag  * tag,                                                                  // XmlTag
  const char  ins,                                                              // Character to insert
  size_t      pos)                                                              // Position in key. 0 prepends the char, while >= length appends the char.
 {tag->node.proto->insertChar(&tag->node, ins, pos);
 }

static void replaceChar__XmlTag_char_size                                         // Replace the character at the specified position in the key string of a tag with the specified character.
 (const XmlTag  * tag,                                                            // XmlTag
  const char  repl,                                                             // Replacement character
  size_t      pos)                                                              // Position in key. 0 replaces the first character.  No replacement occurs if the requested character is beyond the end of the key string
 {tag->node.proto->replaceChar(&tag->node, repl, pos);
 }

static void swapChars__XmlTag_size                                                // Delete the character at the specified position in the string of a tag.
 (const XmlTag  * tag,                                                            // XmlTag
  size_t      pos)                                                              // Position in key. 0 deletes the first character.  No deletion occurs if the requested character is beyond the end of the key string
 {tag->node.proto->swapChars(&tag->node, pos);
 }

static void deleteChar__XmlTag_size                                               // Delete the character at the specified position in the string of a tag.
 (const XmlTag  * tag,                                                            // XmlTag
  size_t      pos)                                                              // Position in key. 0 deletes the first character.  No deletion occurs if the requested character is beyond the end of the key string
 {tag->node.proto->deleteChar(&tag->node, pos);
 }

//D1 Wrap and Unwrap                                                            // Wrap and unwrap nodes

static XmlTag wrap_XmlTag_string                                                    // Wrap a specified tag with a new tag and return the newly createdf wraping tag.
 (const XmlTag *       Tag,                                                       // Tag
  const char * const string)                                                    // Wrapper without the leading < or trailing or >
 {const typeof(*Tag) tag = *Tag;                                                                   // Tag
  if (tag.proto->isRoot(&tag)) return tag;
  char s[strlen(string)+4], *p = stpcpy(s, string); *p = 0;
  const ArenaListNode n =  tag.node.proto->wrap(&tag.node, s);
  return newXmlTag(({struct XmlTag t = {xml: tag.xml, node: n, proto: &ProtoTypes_XmlTag}; t;}));
 }

static XmlTag unwrap_XmlTag                                                         // Unwrap the specified tag.
 (const XmlTag  *      tag)                                                       // Tag
 {if (!tag->proto->isRoot(tag)) tag->node.proto->unwrap(&tag->node);                                        // Cannot unwrap the root tag
  return *tag;
 }

//D1 Print                                                                      // Print an Xml parse tree starting at the specified tag.

static StringBuffer prettyPrint_stringBuffer_XmlTag                               // Pretty print the Xml parse tree starting at the specified tag with additional spacing between tags to make the tree easier to read.
 (const XmlTag * tag)                                                             // Starting tag
 {typeof(makeStringBuffer()) p = makeStringBuffer();

  void print(const XmlTag parent, const int depth)                                // Print the specified parent and its children
   {void indent()                                                               // Indent to the current depth
     {const typeof(p.proto->lengthOfLastLine(&p)) l = p.proto->lengthOfLastLine(&p);
      const typeof(depth * 2ul) n = depth * 2ul;
      if (l < n)                 p.proto->addSpaces(&p, n - l);
      else      {p.proto->addNewLine(&p); p.proto->addSpaces(&p, n);}
     }
    void open()                                                                 // Add open tag
     {if (parent.proto->isText(&parent))
       {p.proto->addn(&p, parent.proto->tagString(&parent), parent.proto->tagStringLength(&parent));
       }
      else if (parent.proto->empty(&parent))                                                  // Write tag with no children as a singleton
       {makeLocalCopyOfXmlTagString(s, l, parent);
        p.proto->addChar(&p, XmlOpen);
        p.proto->addn(&p, s, l);
        p.proto->addFormat(&p, "%c%c", XmlSlash, XmlClose);
       }
      else if (parent.proto->stayInLine(&parent) || parent.proto->isRoot(&parent))                          // Opener preceded or followed by text
       {p.proto->addChar(&p, XmlOpen);
        p.proto->addn(&p, parent.proto->tagString(&parent), parent.proto->tagStringLength(&parent));
        p.proto->addChar(&p, XmlClose);
       }
      else                                                                      // Opener
       {indent();
        p.proto->addChar(&p, XmlOpen);
        p.proto->addn(&p, parent.proto->tagString(&parent), parent.proto->tagStringLength(&parent));
        p.proto->addChar(&p, XmlClose);
       }
     }
    void close()                                                                // Add close tag unless we are on text
     {if (parent.proto->isTag(&parent) && !parent.proto->empty(&parent))
       {if (!parent.proto->stayInLineClose(&parent)) indent();
        p.proto->addFormat(&p, "%c%c%s%c", XmlOpen, XmlSlash, parent.proto->tagName(&parent), XmlClose);
       }
     }

    open();
    Xmlfe(child, parent) print(child, depth+1);
    close();
   }

  print(*tag, 0);
  p.proto->addNewLine(&p);                                                               // Terminating new line
  p.proto->join(&p);
  return p;
 } // prettyPrint

static StringBuffer prettyPrint_stringBuffer__Xml                                 // Pretty print the Xml parse tree with additional spacing between tags to make the tree easier to read.
 (const XmlParse * xml)                                                           // Xml parse tree
 {       const typeof(xml->proto->root(xml)) root = xml->proto->root(xml);
  return root.proto->prettyPrint(&root);
 } // prettyPrint

static StringBuffer print_stringBuffer__XmlTag                                    // Print the Xml parse tree starting at the specified tag with additional spacing between tags to make the tree easier to read.
 (const XmlTag * tag)                                                             // Starting tag
 {const StringBuffer p = makeStringBuffer();

  void print(const XmlTag parent, const int depth)                                // Print the specified parent and its children
   {void open()                                                                 // Add open tag
     {if (parent.proto->isText(&parent))
       {p.proto->addn(&p, parent.proto->tagString(&parent), parent.proto->tagStringLength(&parent));
       }
      else if (parent.proto->empty(&parent))                                                  // Write tag with no children as a singleton
       {makeLocalCopyOfXmlTagString(s, l, parent);
        p.proto->addChar(&p, XmlOpen);
        p.proto->addn(&p, s, l);
        p.proto->addFormat(&p, "%c%c", XmlSlash, XmlClose);
       }
      else                                                                      // Opener
       {p.proto->addChar(&p, XmlOpen);
        p.proto->addn(&p, parent.proto->tagString(&parent), parent.proto->tagStringLength(&parent));
        p.proto->addChar(&p, XmlClose);
       }
     }
    void close()                                                                // Add close tag unless we are on text
     {if (parent.proto->isTag(&parent) && !parent.proto->empty(&parent))
       {p.proto->addFormat(&p, "%c%c%s%c", XmlOpen, XmlSlash, parent.proto->tagName(&parent), XmlClose);
       }
     }

    open();
    Xmlfe(child, parent) print(child, depth+1);
    close();
   }

  print(*tag, 0);
  return p;
 }

static StringBuffer print_stringBuffer__Xml                                       // Print the Xml parse tree with additional spacing between tags to make the tree easier to read.
 (const XmlParse * xml)                                                           // Xml parse tree
 {       const typeof(xml->proto->root(xml)) root = xml->proto->root(xml);
  return root.proto->print(&root);
 }

static int printsAs_int__Xml_string                                               // Check that the Xml parse tree prints as expected
 (const XmlParse *       xml,                                                     // Xml parse tree
  const char   * const expected)                                                // Expected print
 {const typeof(xml->proto->print(xml)) s = xml->proto->print(xml);
  const typeof(s.proto->printsAs(&s, expected)) r = s.proto->printsAs(&s, expected);
  s.proto->free(&s);
  return r;
 }

static int printsAs_int__XmlTag_string                                            // Check that the Xml parse tree prints as expected
 (const XmlTag * tag,                                                             // Xml parse tree
  const char * const expected)                                                  // Expected print
 {const typeof(tag->proto->print(tag)) s = tag->proto->print(tag);
  const typeof(s.proto->printsAs(&s, expected)) r = s.proto->printsAs(&s, expected);
  s.proto->free(&s);
  return r;
 }

static void printAssert__XmlTag                                                   //P Print the Xml parse tree starting at the specified tag as an assert statement
 (const XmlTag *       tag,                                                       // Starting tag
  const char * const variable)                                                  // The name of the variable preceding this call
 {typeof(tag->proto->print(tag)) t = tag->proto->print(tag);
  t.proto->splitLines(&t);
  const typeof(makeStringBuffer()) s = makeStringBuffer();
  s.proto->addFormat(&s, "assert(printsAs_int_Xml_string(%s,\n", variable);
  ArenaListfe(line, t.string)
   {makeLocalCopyOfArenaListKey(l, n, line);
    if (l[n-1] == '\n') l[n-1] = 0;                                             // Remove any end of line
    s.proto->addDoubleQuote(&s);
    s.proto->add(&s, l);
    s.proto->addQuotedNewLine(&s);
    s.proto->addDoubleQuote(&s);
    s.proto->addNewLine(&s);
   }
  s.proto->add(&s, "));\n\n");
  s.proto->writeFile(&s, "/home/phil/c/z/z/zzz.txt");
  t.proto->free(&t); s.proto->free(&s);
 }

static void printAssert_Xml_string                                                //P Pretty print the Xml parse tree as an assert statement
 (const XmlParse *     xml,                                                       // Xml
  const char * const variable)                                                  // The name of the variable preceding this call
 {const XmlTag t = xml->proto->root(xml);
  t.proto->printAssert(&t, variable);
 }

static void dump_Xml                                                              //P Print the Xml parse tree starting to stderr
 (const XmlParse * x)                                                             // Xml
 {typeof(x->proto->prettyPrint(x)) t = x->proto->prettyPrint(x);
  t.proto->join(&t);
  t.proto->dump(&t);
  t.proto->free(&t);
 }

static void dump_XmlTag                                                           //P Dump the specified tag
 (const XmlTag * Tag)                                                             // Xml
 {const typeof(*Tag) tag = *Tag;
  makeLocalCopyOfXmlTagString(t, l, tag);
  say("%s\n", t);
 }

//D1 Samples                                                                    // Samples of Xml

const char * XmlsampleDita1()                                                     // Dita
 {return "<?xml version=\"1.0\"?>\n" "<!DOCTYPE concept PUBLIC \"-//OASIS//DTD DITA Concept//EN\" \"concept.dtd\">\n" "<!-- Created with XMetaL (http://www.xmetal.com) -->\n" "<concept id=\"concept_6FD9560EEEED4FB0A70086250EE15F88\">\n" "  <title>Foreword</title>\n" "  <shortdesc>Short Description\n" "  </shortdesc>\n" "  <conbody>\n" "   <p id=\"0\"> DITA <b>represents</b> a fundamental shift in the approach to writing and\n" "    publishing, and to a certain extent, the carefully accumulated practice of\n" "    hundreds of years of publication projects has to be reviewed and redefined.\n" "   </p>\n" "   <section id=\"1\" audience=\"contributor\">\n" "    <title>Rationale</title>\n" "    <p id=\"2\"> I <b>would</b> suggest that users who are <b>interested</b>\n" "      in a <b>particular</b> <i>feature</i> get together and\n" "      define their own best practices for said feature. Define the markup that\n" "      authors should use, and define the processing expectations of that markup. If\n" "      this unofficial spec is reasonable, then <b>vendors</b> will find it in their\n" "      interests to support it rather than striking out on their own. (It's this kind\n" "      of distributed, grassroots power-to-the-users ideology that the DITA TC wants\n" "      to instill in the community.) I've found that vendors (both commercial and\n" "      non-commercial) are usually very willing to accommodate users' requirements,\n" "      once they know what those requirements are.\n" "    </p>\n" "    <p  id=\"3\">The problem is\n" "      that the vast <b>majority</b> of new users are misusing DITA to a point that they are\n" "      often worse off than they were in Word or FrameMaker. Most companies choose not\n" "      to hire an expert to help them migrate to DITA, and they get what they pay for.\n" "      I'd like us as a TC (and moving forward via the Adoption SC) to put tutorials,\n" "      white papers, etc. in place to educate new users to help them to do the right\n" "      thing from the beginning. At the same time, we must develop tutorials, white\n" "      papers, and other guides that cover as many use cases and user groups as we're\n" "      aware of and address their needs, suggesting best practices and, where\n" "      relevant, providing unofficial packages of DTDs and supporting code (e.g.\n" "      DITA-OT plugins).\n" "    </p>\n" "   </section>\n" "  </conbody>\n" "</concept>\n";
//◉<?xml version="1.0"?>
//◉<!DOCTYPE concept PUBLIC "-//OASIS//DTD DITA Concept//EN" "concept.dtd">
//◉<!-- Created with XMetaL (http://www.xmetal.com) -->
//◉<concept id="concept_6FD9560EEEED4FB0A70086250EE15F88">
//◉  <title>Foreword</title>
//◉  <shortdesc>Short Description
//◉  </shortdesc>
//◉  <conbody>
//◉   <p id="0"> DITA <b>represents</b> a fundamental shift in the approach to writing and
//◉    publishing, and to a certain extent, the carefully accumulated practice of
//◉    hundreds of years of publication projects has to be reviewed and redefined.
//◉   </p>
//◉   <section id="1" audience="contributor">
//◉    <title>Rationale</title>
//◉    <p id="2"> I <b>would</b> suggest that users who are <b>interested</b>
//◉      in a <b>particular</b> <i>feature</i> get together and
//◉      define their own best practices for said feature. Define the markup that
//◉      authors should use, and define the processing expectations of that markup. If
//◉      this unofficial spec is reasonable, then <b>vendors</b> will find it in their
//◉      interests to support it rather than striking out on their own. (It's this kind
//◉      of distributed, grassroots power-to-the-users ideology that the DITA TC wants
//◉      to instill in the community.) I've found that vendors (both commercial and
//◉      non-commercial) are usually very willing to accommodate users' requirements,
//◉      once they know what those requirements are.
//◉    </p>
//◉    <p  id="3">The problem is
//◉      that the vast <b>majority</b> of new users are misusing DITA to a point that they are
//◉      often worse off than they were in Word or FrameMaker. Most companies choose not
//◉      to hire an expert to help them migrate to DITA, and they get what they pay for.
//◉      I'd like us as a TC (and moving forward via the Adoption SC) to put tutorials,
//◉      white papers, etc. in place to educate new users to help them to do the right
//◉      thing from the beginning. At the same time, we must develop tutorials, white
//◉      papers, and other guides that cover as many use cases and user groups as we're
//◉      aware of and address their needs, suggesting best practices and, where
//◉      relevant, providing unofficial packages of DTDs and supporting code (e.g.
//◉      DITA-OT plugins).
//◉    </p>
//◉   </section>
//◉  </conbody>
//◉</concept>
//◉
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TnewArenaList //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer //Terrors //TmakeXmlParseFromFile //Tempty //TisTag //tText
 {const typeof(parseXmlFromString("<a>aa<b>bb<c/>cc</b>dd<d><e/><f>F</f><g/></d><h/><i/></A>h", 0)) x = parseXmlFromString("<a>aa<b>bb<c/>cc</b>dd<d><e/><f>F</f><g/></d><h/><i/></A>h", 0);
  assert( x.proto->printsAs(&x, "<a>aa<b>bb<c/>cc</b>dd<d><e/><f>F</f><g/></d><h/><i/></a>"));

  if (1)
   {ArenaList e = x.errors;
    assert( x.proto->errors(&x) == 2);
    assert( e.proto->printContains(&e, "End tag: A does not match start tag: a"));
    assert( e.proto->printContains(&e, "Ignoring text at end"));
   }

  const typeof(x.proto->findFirstTag(&x, "b")) b = x.proto->findFirstTag(&x, "b"); assert(b.proto->valid(&b)); assert(b.proto->tagNameEquals(&b, "b")); assert(!b.proto->tagNameEquals(&b, "B"));
  const typeof(x.proto->findFirstTag(&x, "c")) c = x.proto->findFirstTag(&x, "c"); assert(c.proto->valid(&c)); assert(c.proto->tagNameEquals(&c, "c")); assert(!c.proto->tagNameEquals(&c, "C"));
  const typeof(x.proto->findFirstTag(&x, "d")) d = x.proto->findFirstTag(&x, "d"); assert(d.proto->valid(&d)); assert(d.proto->tagNameEquals(&d, "d")); assert(!d.proto->tagNameEquals(&d, "D"));
  const typeof(d.proto->first(&d)) e = d.proto->first(&d);             assert(e.proto->valid(&e)); assert(e.proto->tagNameEquals(&e, "e")); assert(!e.proto->tagNameEquals(&e, "E"));
  const typeof(d.proto->last(&d)) g = d.proto->last(&d);              assert(g.proto->valid(&g)); assert(g.proto->tagNameEquals(&g, "g")); assert(!g.proto->tagNameEquals(&g, "G"));
  const typeof(g.proto->prev(&g)) f = g.proto->prev(&g);              assert(f.proto->valid(&f)); assert(f.proto->tagNameEquals(&f, "f")); assert(!f.proto->tagNameEquals(&f, "F"));
  const typeof(x.proto->findFirstTag(&x, "h")) h = x.proto->findFirstTag(&x, "h"); assert(h.proto->valid(&h)); assert(h.proto->tagNameEquals(&h, "h")); assert(!h.proto->tagNameEquals(&h, "H"));
  const typeof(x.proto->findFirstTag(&x, "i")) i = x.proto->findFirstTag(&x, "i"); assert(i.proto->valid(&i)); assert(i.proto->tagNameEquals(&i, "i")); assert(!i.proto->tagNameEquals(&i, "I"));

  assert(!b.proto->empty(&b));
  assert( b.proto->isTag(&b));
  assert( c.proto->empty(&c));
  assert( e.proto->equals(&e, f.proto->prev(&f)));

    const typeof(b.proto->first(&b)) B = b.proto->first(&b);
  assert( B.proto->isText(&B));
  assert(!B.proto->isTag(&B));
  assert( B.proto->tagNameEquals(&B, "text"));
  assert( B.proto->tagStringEquals(&B, "bb"));

  x.proto->free(&x);
 } // test0

void test1()                                                                    //Tfirst //Tlast //Tprev //Tnext //Tequals //Tcount //TcountChildren //TfindFirstTag //TfindFirstChild //TparseXmlFromString //TparseXmlTagName //TtagName //TtagNameEquals //Tvalid //TtagString //TtagStringEquals //Tparent //Troot //Twrap //Tunwrap //TchangeName //TtagStringLength //TisText //TisFirst //TisLast //TisRoot
 {char * xml = "<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j></j></a>";
  const typeof(parseXmlFromString(xml, 0)) x = parseXmlFromString(xml, 0);

  assert( !x.proto->errors(&x));
//        x.proto->printAssert(&x, "x");
  assert( x.proto->printsAs(&x, "<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j/></a>"));

  const typeof(x.proto->root(&x)) a = x.proto->root(&x); assert( a.proto->isRoot(&a));  assert( a.proto->valid(&a)); assert( a.proto->tagNameEquals(&a, "a"));
  const typeof(x.proto->findFirstTag(&x, "b")) b = x.proto->findFirstTag(&x, "b"); assert( b.proto->valid(&b)); assert( b.proto->tagNameEquals(&b, "b")); assert( a.proto->equals(&a, b.proto->parent(&b)));
  const typeof(x.proto->findFirstTag(&x, "c")) c = x.proto->findFirstTag(&x, "c"); assert( c.proto->valid(&c)); assert( c.proto->tagNameEquals(&c, "c")); assert( b.proto->equals(&b, c.proto->parent(&c)));
  const typeof(b.proto->findFirstChild(&b, "d")) d = b.proto->findFirstChild(&b, "d"); assert( d.proto->valid(&d)); assert( d.proto->tagNameEquals(&d, "d")); assert( b.proto->equals(&b, d.proto->parent(&d)));
  const typeof(d.proto->findFirstChild(&d, "e")) e = d.proto->findFirstChild(&d, "e"); assert( e.proto->valid(&e)); assert( e.proto->tagNameEquals(&e, "e")); assert( d.proto->equals(&d, e.proto->parent(&e)));
  const typeof(d.proto->findFirstChild(&d, "f")) f = d.proto->findFirstChild(&d, "f"); assert( f.proto->valid(&f)); assert( f.proto->tagNameEquals(&f, "f")); assert( d.proto->equals(&d, f.proto->parent(&f)));
  const typeof(d.proto->findFirstChild(&d, "g")) g = d.proto->findFirstChild(&d, "g"); assert( g.proto->valid(&g)); assert( g.proto->tagNameEquals(&g, "g")); assert( d.proto->equals(&d, g.proto->parent(&g)));
  const typeof(b.proto->findFirstChild(&b, "h")) h = b.proto->findFirstChild(&b, "h"); assert( h.proto->valid(&h)); assert( h.proto->tagNameEquals(&h, "h")); assert( b.proto->equals(&b, h.proto->parent(&h)));
  const typeof(x.proto->findFirstTag(&x, "i")) i = x.proto->findFirstTag(&x, "i"); assert( i.proto->valid(&i)); assert( i.proto->tagNameEquals(&i, "i")); assert( a.proto->equals(&a, i.proto->parent(&i)));
  const typeof(x.proto->findFirstTag(&x, "j")) j = x.proto->findFirstTag(&x, "j"); assert( j.proto->valid(&j)); assert( j.proto->tagNameEquals(&j, "j")); assert( a.proto->equals(&a, j.proto->parent(&j)));

  assert( b.proto->equals(&b, a.proto->first(&a)));  assert(  b.proto->isFirst(&b));   assert( !d.proto->isFirst(&d)); assert( !b.proto->isRoot(&b));
  assert( c.proto->equals(&c, b.proto->first(&b)));  assert(  c.proto->empty(&c));
  assert( h.proto->equals(&h, b.proto->last(&b)));   assert(  h.proto->isLast(&h));    assert( !d.proto->isLast(&d));
  assert( j.proto->equals(&j, a.proto->last(&a)));

  assert( h.proto->equals(&h, d.proto->next(&d)));
  assert( c.proto->equals(&c, d.proto->prev(&d)));

  assert( a.proto->count(&a)         == 14);
  assert( a.proto->countChildren(&a) ==  4);

  assert( b.proto->count(&b)         == 10);
  assert( b.proto->countChildren(&b) ==  3);

  assert( !strcmp(parseXmlTagName(b.node.proto->key(&b.node), b.node.proto->length(&b.node)), "b"));
  assert( !strcmp(b.proto->tagName(&b),                                  "b"));

  assert( !strncmp(b.proto->tagString(&b), "b", b.proto->tagStringLength(&b)));
  assert( !strncmp(c.proto->tagString(&c), "c", c.proto->tagStringLength(&c)));
  assert( b.proto->tagStringEquals(&b,     "b"));
  assert( c.proto->tagStringEquals(&c,     "c"));

  const typeof(h.proto->first(&h)) H = h.proto->first(&h);
  assert(          H.proto->isText(&H));
  assert(          H.node.proto->equalsString(&H.node, "h"));
  assert( !strcmp (H.proto->tagName(&H), "text"));

  assert( !strncmp(H.proto->tagString(&H),      "h", H.proto->tagStringLength(&H)));
  assert(          H.proto->tagStringEquals(&H, "h"));

  assert( a.proto->equals(&a, b.proto->parent(&b)));

  const typeof(f.proto->wrap(&f, "FFFF id='1'")) F = f.proto->wrap(&f, "FFFF id='1'");
//d.proto->printAssert(&d, "d");
  assert( d.proto->printsAs(&d, "<d><e/>e<FFFF id='1'><f/></FFFF>f<g>g</g></d>"));

  F.proto->changeName(&F, "NNNN");
//d.proto->printAssert(&d, "d");
  assert( d.proto->printsAs(&d, "<d><e/>e<NNNN id='1'><f/></NNNN>f<g>g</g></d>"));

  F.proto->unwrap(&F);
//d.proto->printAssert(&d, "d");
  assert( d.proto->printsAs(&d, "<d><e/>e<f/>f<g>g</g></d>"));

  x.proto->free(&x);
 } // test1

void test2()                                                                    //Tprint
 {const typeof(XmlsampleDita1()) string = XmlsampleDita1();

  const typeof(makeXmlParseFromString(string, strlen(string), 0)) x = makeXmlParseFromString(string, strlen(string), 0);

  const typeof(x.proto->findFirstTag(&x, "p")) p = x.proto->findFirstTag(&x, "p"); const typeof(x.proto->findFirstTag(&x, "conbody")) c = x.proto->findFirstTag(&x, "conbody");

  assert( p.proto->valid(&p));
  assert( p.proto->tagNameEquals(&p, "p"));

  assert( c.proto->valid(&c));
  assert( c.proto->tagNameEquals(&c, "conbody"));

  x.proto->free(&x);
 } // test2

void test3()                                                                    //TmakeXmlParseFromString //TprintsAs //TonlyText //Tby //Tdepth
 {const typeof("<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j/></a>") xml = "<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j/></a>";
    const typeof(makeXmlParseFromString(xml, strlen(xml), 0)) x = makeXmlParseFromString(xml, strlen(xml), 0);

  assert( x.proto->printsAs(&x, "<a><b><c/><d><e/>e<f/>f<g>g</g></d><h>h</h></b><i/>i<j/></a>"));

    const typeof(x.proto->findFirstTag(&x, "h")) h = x.proto->findFirstTag(&x, "h");
  assert( h.proto->valid(&h));
  assert( h.proto->onlyText(&h));
  assert( h.proto->depth(&h) == 2);

    typeof(makeStringBuffer()) s = makeStringBuffer();

    void action(const XmlTag tag)
     {if (tag.proto->isTag(&tag)) s.proto->add(&s, tag.proto->tagName(&tag));
     }

    x.proto->by(&x, action);
    s.proto->joinWith(&s, " ");
  assert( s.proto->equalsString(&s, "c e f g d h b i j a"));

    x.proto->free(&x); s.proto->free(&s);
 } // test3

void test4()                                                                    //TnewArenaList //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {const typeof("/home/phil/c/z/xml/validation/validation.xml") file = "/home/phil/c/z/xml/validation/validation.xml";
  if (!developmentMode()) return;                                               // Does not work on gitHub - possibly out of memory or Cpu?

  const typeof(makeXmlParseFromFile(file, 0)) xml = makeXmlParseFromFile(file, 0);
  typeof(makeXmlValidate()) validate = makeXmlValidate();

    const typeof(xml.proto->findFirstChild(&xml, "possibilities")) possibilities = xml.proto->findFirstChild(&xml, "possibilities");                      // The possibilities possibilitiesArray assigns a number to each set of possible tags
  assert( possibilities.proto->valid(&possibilities));

    const typeof(possibilities.proto->findFirstChild(&possibilities, "array")) possibilitiesArray = possibilities.proto->findFirstChild(&possibilities, "array");
  assert( possibilitiesArray.proto->valid(&possibilitiesArray));
    const typeof(possibilitiesArray.proto->countChildren(&possibilitiesArray)) possibilitiesCount = possibilitiesArray.proto->countChildren(&possibilitiesArray);
  assert( possibilitiesCount == 383);

    const typeof(xml.proto->findFirstChild(&xml, "first")) first = xml.proto->findFirstChild(&xml, "first");                                      // The tags that can come first under a specified parent tag
  assert( first.proto->valid(&first));

    const typeof(first.proto->findFirstChild(&first, "hash")) firstHash = first.proto->findFirstChild(&first, "hash");
  assert( firstHash.proto->valid(&firstHash));
  assert( firstHash.proto->countChildren(&firstHash) == 374);

    const typeof(xml.proto->findFirstChild(&xml, "next")) next = xml.proto->findFirstChild(&xml, "next");                                        // The tags that can come next under a specified parent tag
  assert( next.proto->valid(&next));

    const typeof(next.proto->findFirstChild(&next, "hash")) nextParentHash = next.proto->findFirstChild(&next, "hash");
  assert( nextParentHash.proto->valid(&nextParentHash));
  assert( nextParentHash.proto->countChildren(&nextParentHash) == 374);

  if (1)                                                                        // Possibilities possibilitiesArray
   {ArenaTreeNode pa[possibilitiesCount];
    assert( validate.possibilities.proto->count(&validate.possibilities) == 0);
    typeof(0ul) i = 0ul;

    Xmlfe(element, possibilitiesArray)                                            // Load possibilities
     {char buffer[256];
      sprintf(buffer, "%lu", i);                                                // Offset as string for use as a key
      const typeof(validate.possibilities.proto->add(&validate.possibilities, buffer, strlen(buffer))) f = validate.possibilities.proto->add(&validate.possibilities, buffer, strlen(buffer));                 // Possibility set number
      assert( f.proto->valid(&f));
      pa[i] = f;                                                                // Cache node offset

      Xmlfe(hash, element)                                                        // Array of hashes
       {Xmlfe(hashKey, hash)                                                      // Keys in each hash
         {const typeof(hashKey.proto->tagName(&hashKey)) name = hashKey.proto->tagName(&hashKey);
          if (strcmp(name, "text")) f.proto->add(&f, name, strlen(name));
         }
       }
      ++i;
     }

    Xmlfe(firstTag, firstHash)                                                    // Load first tags
     {const typeof(firstTag.proto->first(&firstTag)) value = firstTag.proto->first(&firstTag);                                                 // Value tag
      makeLocalCopyOfXmlTagString(t, n, value);
      const typeof(strtol(t, 0, 0)) a = strtol(t, 0, 0);                                                      // Possibility number
      const typeof(firstTag.proto->tagName(&firstTag)) N = firstTag.proto->tagName(&firstTag);                                                  // Name of tag
      const typeof(validate.first.proto->add(&validate.first, N, strlen(N))) f = validate.first.proto->add(&validate.first, N, strlen(N));                                   // First possibilities for this tag
      *(size_t *)(f.proto->data(&f)) = pa[a].offset;                                     // Save the offset of the possibilities set for the first possible tag
     }

    Xmlfe(nextParentTag, nextParentHash)                                          // Load next parents
     {const typeof(nextParentTag.proto->tagName(&nextParentTag)) tag = nextParentTag.proto->tagName(&nextParentTag);                                           // Next parent tag
        const typeof(validate.next.proto->add(&validate.next, tag, strlen(tag))) nt = validate.next.proto->add(&validate.next, tag, strlen(tag));                             // Add next parent tag to next tree
      assert( nt.proto->valid(&nt));

      const typeof(nextParentTag.proto->findFirstChild(&nextParentTag, "hash")) nextChildHash = nextParentTag.proto->findFirstChild(&nextParentTag, "hash");

      Xmlfe(nextChildTag, nextChildHash)                                          // Load next children
       {const typeof(nextChildTag.proto->first(&nextChildTag)) value = nextChildTag.proto->first(&nextChildTag);                                           // Value tag
        makeLocalCopyOfXmlTagString(t, n, value);
        const typeof(strtol(t, 0, 0)) a = strtol(t, 0, 0);                                                    // Possibility number
        const typeof(nextChildTag.proto->tagName(&nextChildTag)) name = nextChildTag.proto->tagName(&nextChildTag);                                          // Next possibilities for the child tag
          const typeof(nt.proto->add(&nt, name, strlen(name))) F = nt.proto->add(&nt, name, strlen(name));                                     // Next possibilities for the child tag
        assert( F.proto->valid(&F));
        *(size_t *)(F.proto->data(&F)) = pa[a].offset;                                   // Save the offset of the possibilities set for the next possible tag
       }
     }
   }

  if (1)                                                                        // Check that 'appendix' is a possible first element of 'appendices'.
   {  const typeof(validate.first.proto->locate(&validate.first, "appendices", 10)) f = validate.first.proto->locate(&validate.first, "appendices", 10);
    assert( f.proto->valid(&f));

      const typeof(validate.possibilities.proto->offset(&validate.possibilities, *(size_t *)(f.proto->data(&f)))) n = validate.possibilities.proto->offset(&validate.possibilities, *(size_t *)(f.proto->data(&f)));               // Create a node representing the Appendices Tag first possibilities set
    assert( n.proto->equalsString(&n, "8"));                                                    // Appendices is tag 8
      const typeof(n.proto->locate(&n, "appendix", 8)) p = n.proto->locate(&n, "appendix", 8);                                            // Check whether Appendix is in the set of first tag possibilities for Appendices
    assert( p.proto->valid(&p));                                                                // Appendix is in the first tag set of Appendices
   }

  if (1)                                                                        // Check that 'dd' can follow child 'dt' under parent 'dlentry'.
   {  const typeof(validate.next.proto->ll(&validate.next, "dlentry", "dt", 0)) F = validate.next.proto->ll(&validate.next, "dlentry", "dt", 0);                               // Parent + child
    assert( F.proto->valid(&F));
      const typeof(validate.possibilities.proto->offset(&validate.possibilities, *(size_t *)(F.proto->data(&F)))) n = validate.possibilities.proto->offset(&validate.possibilities, *(size_t *)(F.proto->data(&F)));               // Next child possibilities under parent
      const typeof(n.proto->locate(&n, "dd", 2)) p = n.proto->locate(&n, "dd", 2);
    assert( p.proto->valid(&p));                                                                // Check whether 'dd' can follow 'dt' under 'dlentry'
   }

  xml.proto->free(&xml); validate.proto->free(&validate);
 } // test4

void test5()                                                                    //ThasText //TstayInline //TprettyPrint
 {const typeof("<a><b>bb bb <c/> ccc</b><d> <i/> <j> jj <k/> kk</j> </d><l>lll</l><m>mmm</m></a>") xml = "<a><b>bb bb <c/> ccc</b><d> <i/> <j> jj <k/> kk</j> </d><l>lll</l><m>mmm</m></a>";

  const typeof(parseXmlFromString(xml, 0)) x = parseXmlFromString(xml, 0);  assert( ! x.proto->errors(&x));
  const typeof(x.proto->findFirstTag(&x, "b")) b = x.proto->findFirstTag(&x, "b");     assert(   b.proto->hasText(&b));
                                 assert( ! b.proto->stayInLine(&b));
  const typeof(x.proto->findFirstTag(&x, "c")) c = x.proto->findFirstTag(&x, "c");     assert(   c.proto->stayInLine(&c));
  const typeof(c.proto->next(&c)) C = c.proto->next(&c);                  assert(   C.proto->stayInLine(&C));
  const typeof(x.proto->findFirstTag(&x, "d")) d = x.proto->findFirstTag(&x, "d");     assert( ! d.proto->hasText(&d));
  const typeof(x.proto->findFirstTag(&x, "m")) m = x.proto->findFirstTag(&x, "m");     assert( ! m.proto->stayInLine(&m));

    const typeof(x.proto->prettyPrint(&x)) p = x.proto->prettyPrint(&x);
  assert( p.proto->printsAs(&p, "<a>\n" "  <b>bb bb <c/> ccc</b>\n" "  <d><i/>\n" "    <j> jj <k/> kk</j>\n" "  </d>\n" "  <l>lll</l>\n" "  <m>mmm</m>\n" "</a>\n"));
//◉<a>
//◉  <b>bb bb <c/> ccc</b>
//◉  <d><i/>
//◉    <j> jj <k/> kk</j>
//◉  </d>
//◉  <l>lll</l>
//◉  <m>mmm</m>
//◉</a>
//◉

  x.proto->free(&x); p.proto->free(&p);
 }

void test6()                                                                    //Tscan
 {const typeof("<a><b><c/><d><e/>ee<f/>ff<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>") xml = "<a><b><c/><d><e/>ee<f/>ff<g>ggg</g></d><h>hh hh</h></b><i/>i<j></j></a>";
     const typeof(parseXmlFromString(xml, 0)) x = parseXmlFromString(xml, 0);
  assert( !x.proto->errors(&x));

  const typeof(makeStringBuffer()) s = makeStringBuffer();

  void action(const XmlTag tag, int start, int depth)
   {makeLocalCopyOfXmlTagString(t, l, tag);
    s.proto->addFormat(&s, "%s(%d,%d) ", t, start, depth);
   }

  x.proto->scan(&x, action);
  s.proto->equalsString(&s, "(1,0) <a>(1,1) <b>(1,2) <c>(0,3) <d>(1,3) <e>(0,4) ee(0,4) <f>(0,4) ff(0,4) <g>(1,4) ggg(0,5) <g>(-1,4) <d>(-1,3) <h>(1,3) hh hh(0,4) <h>(-1,3) <b>(-1,2) <i>(0,2) i(0,2) <j>(0,2) <a>(-1,1) (-1,0)");
  x.proto->free(&x); s.proto->free(&s);
 }

void test7()                                                                    //Tlength //TinsertChar //TdeleteChar //TreplaceChar //TswapChars
 {const typeof("<a><abcd>1234</abcd></a>") xml = "<a><abcd>1234</abcd></a>";
     const typeof(parseXmlFromString(xml, 0)) x = parseXmlFromString(xml, 0);
  assert( !x.proto->errors(&x));

  typeof(x.proto->findFirstChild(&x, "abcd")) a = x.proto->findFirstChild(&x, "abcd");
  typeof(a.proto->first(&a)) t = a.proto->first(&a);

  a.proto->insertChar(&a, 'D', 3);  assert( x.proto->printsAs(&x, "<a><abcDd>1234</abcDd></a>"));
  t.proto->insertChar(&t, 'T', 3);  assert( x.proto->printsAs(&x, "<a><abcDd>123T4</abcDd></a>"));
  assert( a.proto->length(&a) == 5);       assert( t.proto->length(&t) == 5);

  a.proto->replaceChar(&a, 'e', 3); assert( x.proto->printsAs(&x, "<a><abced>123T4</abced></a>"));
  t.proto->replaceChar(&t, 't', 3); assert( x.proto->printsAs(&x, "<a><abced>123t4</abced></a>"));
  assert( a.proto->length(&a) == 5);       assert( t.proto->length(&t) == 5);

  a.proto->deleteChar(&a, 3);       assert( x.proto->printsAs(&x, "<a><abcd>123t4</abcd></a>"));
  t.proto->deleteChar(&t, 3);       assert( x.proto->printsAs(&x, "<a><abcd>1234</abcd></a>"));
  assert( a.proto->length(&a) == 4);       assert( t.proto->length(&t) == 4);

  a.proto->swapChars(&a, 3);       assert( x.proto->printsAs(&x, "<a><abdc>1234</abdc></a>"));
  t.proto->swapChars(&t, 1);       assert( x.proto->printsAs(&x, "<a><abdc>2134</abdc></a>"));
  assert( a.proto->length(&a) == 4);       assert( t.proto->length(&t) == 4);

  x.proto->free(&x);
 }

void test8()                                                                    //TcharPos
 {const typeof("<a><abcd>1234</abcd><efg></efg></a>") xml = "<a><abcd>1234</abcd><efg></efg></a>";
     const typeof(parseXmlFromString(xml, 0)) x = parseXmlFromString(xml, 0);
  assert( !x.proto->errors(&x));
     const typeof(x.proto->root(&x)) X = x.proto->root(&x);                 assert( X.proto->charPosition(&X, 0) ==  0);  assert( X.proto->charPosition(&X, 1) == 19);
  const typeof(x.proto->findFirstChild(&x, "abcd")) a = x.proto->findFirstChild(&x, "abcd");  assert( a.proto->charPosition(&a, 0) ==  1);  assert( a.proto->charPosition(&a, 1) == 9);
  const typeof(a.proto->first(&a)) b = a.proto->first(&a);                   assert( b.proto->charPosition(&b, 0) ==  5);  assert( b.proto->charPosition(&b, 1) == 9);
  const typeof(a.proto->next(&a)) c = a.proto->next(&a);                    assert( c.proto->charPosition(&c, 0) == 13);  assert( c.proto->charPosition(&c, 1) == 16);

  x.proto->free(&x);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4,
                           test5, test6, test7, test8, 0};
  run_tests("Xml", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/xml/xml
