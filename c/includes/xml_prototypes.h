static XmlTag makeXmlTag_XmlParse_ArenaListNode
 (const XmlParse        xml,
  const ArenaListNode node);
static XmlParse makeXmlParseFromFile
 (const char * const sourceFileName);
static XmlParse makeXmlParseFromString
 (const char * const sourceFileName,
  char       * const source,
  const size_t       length);
static XmlParse parseXmlFromString
 (char * string);
static void free_XmlParse
 (const XmlParse x);
static void free_XmlValidate
 (const XmlValidate x);
static size_t errors_XmlParse
 (const XmlParse x);
static char  * parseXmlTagName
 (const char * const tagString);
static char * tagName_XmlTag
 (const XmlTag tag);
static int tagNameEquals_XmlTag_string
 (const XmlTag         tag,
  const char * const expected);
static char * tagString_XmlTag
 (const XmlTag tag);
static size_t tagStringLength_XmlTag
 (const XmlTag tag);
static int tagStringEquals_XmlTag_string
 (const XmlTag         tag,
  const char * const expected);
static XmlTag first_XmlTag
 (const XmlTag parent);
static XmlTag last_XmlTag
 (const XmlTag parent);
static XmlTag next_XmlTag
 (const XmlTag parent);
static XmlTag prev_XmlTag
 (const XmlTag parent);
static XmlTag parent_XmlTag
 (const XmlTag parent);
static XmlTag first_XmlParse
 (const XmlParse xml);
static XmlTag last_XmlParse
 (const XmlParse xml);
static int isRoot_XmlTag
 (const XmlTag tag);
static int isFirst_XmlTag
 (const XmlTag tag);
static int isLast_XmlTag
 (const XmlTag tag);
static int isText_XmlTag
 (const XmlTag tag);
static int onlyText_XmlTag
 (const XmlTag parent);
static int empty_XmlTag
 (const XmlTag parent);
static int isTag
 (const XmlTag tag);
static int singleton_XmlTag
 (const XmlTag tag);
static int valid_XmlTag
 (const XmlTag tag);
static XmlTag findFirstChild_XmlTag_XmlTag_string
 (const XmlTag   parent,
  const char * const key);
static XmlTag findFirstChild_XmlTag_XmlParse_string
 (const XmlParse xml,
  const char * const key);
static XmlTag findFirstTag_XmlTag_XmlTag_string
 (const XmlTag   parent,
  const char * const key);
static XmlTag findFirstTag_XmlTag_XmlParse_string
 (const XmlParse       xml,
  const char * const key);
static XmlTag root_XmlParse
 (const XmlParse xml);
static XmlTag root_XmlTag
 (const XmlTag tag);
static int equals_XmlTag_XmlTag
 (const XmlTag a,
  const XmlTag b);
static void by_XmlTag_sub
 (const XmlTag tag,
  void (* const function) (const XmlTag tag));
static void by_XmlParse_sub
 (const XmlParse xml,
  void (* const function) (const XmlTag tag));
static  size_t countChildren_size_Xml
 (const XmlParse xml);
static  size_t countChildren_size_XmlTag
 (const XmlTag parent);
static  size_t count_size_Xml
 (const XmlParse x);
static  size_t count_size_XmlTag
 (const XmlTag t);
static void changeName_XmlTag
 (const XmlTag         tag,
  const char * const newName);
static XmlTag wrap_XmlTag_string
 (const XmlTag         tag,
  const char * const string);
static XmlTag unwrap_XmlTag
 (const XmlTag         tag);
static StringBuffer print_stringBuffer_XmlTag
 (const XmlTag tag);
static StringBuffer print_stringBuffer_Xml
 (const XmlParse xml);
static int printsAs_int_Xml_string
 (const XmlParse xml,
  const char * const expected);
static int printsAs_int_XmlTag_string
 (const XmlTag tag,
  const char * const expected);
static void printAssert_XmlTag
 (const XmlTag         tag,
  const char * const variable);
static void printAssert_Xml_string
 (const XmlParse       xml,
  const char * const variable);
struct ProtoTypes_XmlParse {
  void  (*by)(                                                                  // Traverse the Xml parse tree in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    const XmlParse xml,                                                         // Xml parse tree
    void (* const function) (const XmlTag tag));                                // Function to call on each tag
  size_t  (*countChildren)(                                                     // Count the number of tags at the first level in an xml parse tree.
    const XmlParse xml);                                                        // Xml parse
  size_t  (*count)(                                                             // Count the number of tags in an Xml parse tree
    const XmlParse x);                                                          // Xml parse tree
  size_t  (*errors)(                                                            // Number of errors encountered while creating an Xml parse tree.
    const XmlParse x);                                                          // Xml descriptor
  XmlTag  (*findFirstChild)(                                                    // Find the first child tag with the specified name under the root tag of the specified parse tree.
    const XmlParse xml,                                                         // Parent
    const char * const key);                                                    // Name of the tag to find
  XmlTag  (*findFirstTag)(                                                      // Find the first tag in an Xml parse tree with the specified name.
    const XmlParse xml,                                                         // Xml parse tree
    const char * const key);                                                    // Name of the tag to find
  XmlTag  (*first)(                                                             // Return the first child tag in the specified Xml parse tree.
    const XmlParse xml);                                                        // Parent tag
  void  (*free)(                                                                // Free an Xml parse
    const XmlParse x);                                                          // Xml descriptor
  XmlTag  (*last)(                                                              // Return the first child tag in the specified Xml parse tree.
    const XmlParse xml);                                                        // Parent tag
  XmlTag  (*makeXmlTag)(                                                        // Make a tag descriptor from a parse tree node holding the tag
    const XmlParse xml,                                                         // Xml parse tree
    const ArenaListNode node);                                                  // Node holding tag
  void  (*printAssert)(                                                         // Pretty print the Xml parse tree as an assert statement
    const XmlParse xml,                                                         // Xml
    const char * const variable);                                               // The name of the variable preceding this call
  StringBuffer  (*print)(                                                       // Print the Xml parse tree with additional spacing between tags to make the tree easier to read.
    const XmlParse xml);                                                        // Xml parse tree
  int  (*printsAs)(                                                             // Check that the Xml parse tree prints as expected
    const XmlParse xml,                                                         // Xml parse tree
    const char * const expected);                                               // Expected print
  XmlTag  (*root)(                                                              // Return the root tag of the specified Xml parse tree
    const XmlParse xml);                                                        // Xml parse tree
 } const ProtoTypes_XmlParse =
{by_XmlParse_sub, countChildren_size_Xml, count_size_Xml, errors_XmlParse, findFirstChild_XmlTag_XmlParse_string, findFirstTag_XmlTag_XmlParse_string, first_XmlParse, free_XmlParse, last_XmlParse, makeXmlTag_XmlParse_ArenaListNode, printAssert_Xml_string, print_stringBuffer_Xml, printsAs_int_Xml_string, root_XmlParse};
XmlParse newXmlParse(XmlParse allocator) {return allocator;}

struct ProtoTypes_XmlTag {
  void  (*by)(                                                                  // Traverse the Xml parse tree rooted at the specified tag in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    const XmlTag tag,                                                           // Starting tag
    void (* const function) (const XmlTag tag));                                // Function to call on each tag
  void  (*changeName)(                                                          // Change the name of the specified tag.
    const XmlTag tag,                                                           // Tag
    const char * const newName);                                                // New name                                                                               //vconst XmlTag t)                                                                 // Tag
  size_t  (*countChildren)(                                                     // Count the number of child tags directly under a parent tag
    const XmlTag parent);                                                       // Parent tag
  size_t  (*count)(                                                             // Count the number of tags under the specified tag in an Xml parse tree
    const XmlTag t);                                                            // Parent tag
  int  (*empty)(                                                                // Check whether the specified parent tag is empty
    const XmlTag parent);                                                       // Parent tag
  int  (*equals)(                                                               // Confirm that two tags refer to the same text location in the source document.
    const XmlTag a,                                                             // First tag
    const XmlTag b);                                                            // Second tag
  XmlTag  (*findFirstChild)(                                                    // Find the first child tag with the specified name under the specified parent tag.
    const XmlTag parent,                                                        // Parent
    const char * const key);                                                    // Name of the tag to find
  XmlTag  (*findFirstTag)(                                                      // Find the first tag with the specified name in the Xml parse tree starting at the specified tag.
    const XmlTag parent,                                                        // Parent tag
    const char * const key);                                                    // Name of the tag to find
  XmlTag  (*first)(                                                             // Return the first child tag under the specified parent tag.
    const XmlTag parent);                                                       // Parent tag
  int  (*isFirst)(                                                              // Check that the specified tag is first under its parent
    const XmlTag tag);                                                          // Tag
  int  (*isLast)(                                                               // Check that the specified tag is last under its parent
    const XmlTag tag);                                                          // Tag
  int  (*isRoot)(                                                               // Check whether the specified tag is the root tag
    const XmlTag tag);                                                          // Tag
  int  (*isTag)(                                                                // Check that the tag is a tag rather than text
    const XmlTag tag);                                                          // Parent tag
  int  (*isText)(                                                               // Check whether we are on a text element
    const XmlTag tag);                                                          // Tag
  XmlTag  (*last)(                                                              // Return the first child tag under the specified parent tag.
    const XmlTag parent);                                                       // Parent tag
  XmlTag  (*next)(                                                              // Return the first child tag under the specified parent tag.
    const XmlTag parent);                                                       // Parent tag
  int  (*onlyText)(                                                             // Return true if a tag contains just one text element and nothing else
    const XmlTag parent);                                                       // Tag
  XmlTag  (*parent)(                                                            // Return the first child tag under the specified parent tag.
    const XmlTag parent);                                                       // Parent tag
  XmlTag  (*prev)(                                                              // Return the first child tag under the specified parent tag.
    const XmlTag parent);                                                       // Parent tag
  void  (*printAssert)(                                                         // Print the Xml parse tree starting at the specified tag as an assert statement
    const XmlTag tag,                                                           // Starting tag
    const char * const variable);                                               // The name of the variable preceding this call
  StringBuffer  (*print)(                                                       // Print the Xml parse tree starting at the specified tag with additional spacing between tags to make the tree easier to read.
    const XmlTag tag);                                                          // Starting tag
  int  (*printsAs)(                                                             // Check that the Xml parse tree prints as expected
    const XmlTag tag,                                                           // Xml parse tree
    const char * const expected);                                               // Expected print
  XmlTag  (*root)(                                                              // Return the root tsg of the Xml parse tree containing the specified tag.
    const XmlTag tag);                                                          // Tag
  int  (*singleton)(                                                            // Check that the tag has no children
    const XmlTag tag);                                                          // Parent tag
  int  (*tagNameEquals)(                                                        // Check the name of a tag.
    const XmlTag tag,                                                           // Tag
    const char * const expected);                                               // Expected name of tag
  char *  (*tagName)(                                                           // Get the tag name from a node in the Xml parse tree. The tag name is returned in a reused static buffer that the caller should not free.
    const XmlTag tag);                                                          // Tag
  int  (*tagStringEquals)(                                                      // Check the string of a tag.
    const XmlTag tag,                                                           // Tag
    const char * const expected);                                               // Expected name of tag
  size_t  (*tagStringLength)(                                                   // Get the length of the entire tag
    const XmlTag tag);                                                          // Tag
  char *  (*tagString)(                                                         // Get the entire tag from a node in the Xml parse tree.
    const XmlTag tag);                                                          // Tag
  XmlTag  (*unwrap)(                                                            // Unwrap the specified tag.
    const XmlTag tag);                                                          // Tag
  int  (*valid)(                                                                // Check that a tag is valid.
    const XmlTag tag);                                                          // Tag
  XmlTag  (*wrap)(                                                              // Wrap a specified tag with a new tag and return the newly createdf wraping tag.
    const XmlTag tag,                                                           // Tag
    const char * const string);                                                 // Wrapper without the leading < or trailing or >
 } const ProtoTypes_XmlTag =
{by_XmlTag_sub, changeName_XmlTag, countChildren_size_XmlTag, count_size_XmlTag, empty_XmlTag, equals_XmlTag_XmlTag, findFirstChild_XmlTag_XmlTag_string, findFirstTag_XmlTag_XmlTag_string, first_XmlTag, isFirst_XmlTag, isLast_XmlTag, isRoot_XmlTag, isTag, isText_XmlTag, last_XmlTag, next_XmlTag, onlyText_XmlTag, parent_XmlTag, prev_XmlTag, printAssert_XmlTag, print_stringBuffer_XmlTag, printsAs_int_XmlTag_string, root_XmlTag, singleton_XmlTag, tagNameEquals_XmlTag_string, tagName_XmlTag, tagStringEquals_XmlTag_string, tagStringLength_XmlTag, tagString_XmlTag, unwrap_XmlTag, valid_XmlTag, wrap_XmlTag_string};
XmlTag newXmlTag(XmlTag allocator) {return allocator;}

struct ProtoTypes_XmlValidate {
  void  (*free)(                                                                // Free an Xml validator
    const XmlValidate x);                                                       // Xml validator
 } const ProtoTypes_XmlValidate =
{free_XmlValidate};
XmlValidate newXmlValidate(XmlValidate allocator) {return allocator;}

