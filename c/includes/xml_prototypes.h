static XmlParse  parseXmlFromString
 (const char * const string,
  const size_t       width);
static void free_XmlParse
 (const XmlParse * x);
static void free_XmlValidate
 (XmlValidate * x);
static size_t errors_size_XmlParse
 (const XmlParse * x);
static char  * parseXmlTagNameInAngleBrackets
 (const char * const tagString);
static  char * parseXmlTagName
 (const char * tag,
  const size_t length);
static char * tagName_string_XmlTag
 (const XmlTag * Tag);
static int tagNameEquals_int_XmlTag_string
 (const XmlTag *       tag,
  const char * const expected);
static char * tagString_string_XmlTag
 (const XmlTag * tag);
static size_t tagStringLength_size_XmlTag
 (const XmlTag * tag);
static int tagStringEquals_int_XmlTag_string
 (const XmlTag *       tag,
  const char * const expected);
static XmlTag first_XmlTag
 (const XmlTag * parent);
static XmlTag last_XmlTag
 (const XmlTag * parent);
static XmlTag next_XmlTag
 (const XmlTag * parent);
static XmlTag prev_XmlTag
 (const XmlTag * parent);
static XmlTag parent_XmlTag
 (const XmlTag * parent);
static XmlTag first_XmlParse
 (const XmlParse * xml);
static XmlTag last_XmlParse
 (const XmlParse * xml);
static int depth_int_XmlTag
 (const XmlTag * Tag);
static int isRoot_int_XmlTag
 (const XmlTag * tag);
static int isFirst_int_XmlTag
 (const XmlTag * Tag);
static int isLast_int_XmlTag
 (const XmlTag * Tag);
static size_t charPosition__XmlTag
 (const XmlTag * tag,
  const int    close);
static int isText_int_XmlTag
 (const XmlTag * tag);
static int onlyText_XmlTag
 (const XmlTag * parent);
static int empty_int_XmlTag
 (const XmlTag * parent);
static int isTag_int_tag
 (const XmlTag * tag);
static int hasText_int_XmlTag
 (const XmlTag * Parent);
static int stayInLine_int_XmlTag
 (const XmlTag * Tag);
static int stayInLineClose_int_XmlTag
 (const XmlTag * Tag);
static size_t length_size__XmlTag
 (const XmlTag * tag);
static int valid_int_XmlTag
 (const XmlTag * tag);
static XmlTag findFirstChild_XmlTag_XmlTag_string
 (const XmlTag *       Parent,
  const char * const key);
static XmlTag findFirstChild_XmlTag_XmlParse_string
 (const XmlParse * xml,
  const char   * const key);
static XmlTag findFirstTag_XmlTag_XmlTag_string
 (const XmlTag *       parent,
  const char * const key);
static XmlTag findFirstTag_XmlTag_XmlParse_string
 (const XmlParse *       xml,
  const char   * const key);
static XmlTag root_XmlParse
 (const XmlParse * xml);
static XmlTag root_XmlTag
 (const XmlTag * tag);
static int equals_XmlTag_XmlTag
 (const XmlTag * a,
  const XmlTag b);
static void by_XmlTag_sub
 (const XmlTag * tag,
  void (* const function) (const XmlTag tag));
static void by_XmlParse_sub
 (const XmlParse * xml,
  void (* const function) (const XmlTag tag));
static void scan_XmlTag_sub
 (const XmlTag * tag,
  void (* const function) (XmlTag tag, int start, int depth));
static void scan_XmlParse_sub
 (const XmlParse * xml,
  void (* const function) (XmlTag tag, int start, int depth));
static  size_t countChildren_size_Xml
 (const XmlParse * xml);
static  size_t countChildren_size_XmlTag
 (const XmlTag * parent);
static  size_t count_size_Xml
 (const XmlParse * x);
static  size_t count_size_XmlTag
 (const XmlTag * t);
static void changeName_XmlTag
 (const XmlTag * Tag,
  const char * const newName);
static void insertChar__XmlTag_char_size
 (XmlTag  * tag,
  const char  ins,
  size_t      pos);
static void replaceChar__XmlTag_char_size
 (const XmlTag  * tag,
  const char  repl,
  size_t      pos);
static void swapChars__XmlTag_size
 (const XmlTag  * tag,
  size_t      pos);
static void deleteChar__XmlTag_size
 (const XmlTag  * tag,
  size_t      pos);
static XmlTag wrap_XmlTag_string
 (const XmlTag *       Tag,
  const char * const string);
static XmlTag unwrap_XmlTag
 (const XmlTag  *      tag);
static StringBuffer prettyPrint_stringBuffer_XmlTag
 (const XmlTag * tag);
static StringBuffer prettyPrint_stringBuffer__Xml
 (const XmlParse * xml);
static StringBuffer print_stringBuffer__XmlTag
 (const XmlTag * tag);
static StringBuffer print_stringBuffer__Xml
 (const XmlParse * xml);
static int printsAs_int__Xml_string
 (const XmlParse *       xml,
  const char   * const expected);
static int printsAs_int__XmlTag_string
 (const XmlTag * tag,
  const char * const expected);
static void printAssert__XmlTag
 (const XmlTag *       tag,
  const char * const variable);
static void printAssert_Xml_string
 (const XmlParse *     xml,
  const char * const variable);
static void dump_Xml
 (const XmlParse * x);
static void dump_XmlTag
 (const XmlTag * Tag);
struct ProtoTypes_XmlNodeExt {
 } const ProtoTypes_XmlNodeExt =
{};
XmlNodeExt newXmlNodeExt(XmlNodeExt allocator) {return allocator;}

struct ProtoTypes_XmlParse {
  void  (*by)(                                                                  // Traverse the Xml parse tree in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    const XmlParse * xml,                                                       // Xml parse tree
    void (* const function) (const XmlTag tag));                                // Function to call on each tag
  size_t  (*countChildren)(                                                     // Count the number of tags at the first level in an xml parse tree.
    const XmlParse * xml);                                                      // Xml parse
  size_t  (*count)(                                                             // Count the number of tags in an Xml parse tree
    const XmlParse * x);                                                        // Xml parse tree
  void  (*dump)(                                                                // Print the Xml parse tree starting to stderr
    const XmlParse * x);                                                        // Xml
  size_t  (*errors)(                                                            // Number of errors encountered while creating an Xml parse tree.
    const XmlParse * x);                                                        // Xml descriptor
  XmlTag  (*findFirstChild)(                                                    // Find the first child tag with the specified name under the root tag of the specified parse tree.
    const XmlParse * xml,                                                       // Parent
    const char   * const key);                                                  // Name of the tag to find
  XmlTag  (*findFirstTag)(                                                      // Find the first tag in an Xml parse tree with the specified name.
    const XmlParse * xml,                                                       // Xml parse tree
    const char   * const key);                                                  // Name of the tag to find
  XmlTag  (*first)(                                                             // Return the first child tag in the specified Xml parse tree.
    const XmlParse * xml);                                                      // Parent tag
  void  (*free)(                                                                // Free an Xml parse
    const XmlParse * x);                                                        // Xml descriptor
  XmlTag  (*last)(                                                              // Return the first child tag in the specified Xml parse tree.
    const XmlParse * xml);                                                      // Parent tag
  StringBuffer  (*prettyPrint)(                                                 // Pretty print the Xml parse tree with additional spacing between tags to make the tree easier to read.
    const XmlParse * xml);                                                      // Xml parse tree
  void  (*printAssert)(                                                         // Pretty print the Xml parse tree as an assert statement
    const XmlParse * xml,                                                       // Xml
    const char * const variable);                                               // The name of the variable preceding this call
  StringBuffer  (*print)(                                                       // Print the Xml parse tree with additional spacing between tags to make the tree easier to read.
    const XmlParse * xml);                                                      // Xml parse tree
  int  (*printsAs)(                                                             // Check that the Xml parse tree prints as expected
    const XmlParse * xml,                                                       // Xml parse tree
    const char   * const expected);                                             // Expected print
  XmlTag  (*root)(                                                              // Return the root tag of the specified Xml parse tree
    const XmlParse * xml);                                                      // Xml parse tree
  void  (*scan)(                                                                // Traverse the Xml parse tree calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The Xml is buffered allowing changes to be made to the structure of the Xml without disruption as long as each child checks its context.
    const XmlParse * xml,                                                       // Xml parse tree
    void (* const function) (XmlTag tag, int start, int depth));                // Function to call on each tag: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
 } const ProtoTypes_XmlParse =
{by_XmlParse_sub, countChildren_size_Xml, count_size_Xml, dump_Xml, errors_size_XmlParse, findFirstChild_XmlTag_XmlParse_string, findFirstTag_XmlTag_XmlParse_string, first_XmlParse, free_XmlParse, last_XmlParse, prettyPrint_stringBuffer__Xml, printAssert_Xml_string, print_stringBuffer__Xml, printsAs_int__Xml_string, root_XmlParse, scan_XmlParse_sub};
XmlParse newXmlParse(XmlParse allocator) {return allocator;}

struct ProtoTypes_XmlParseOptions {
 } const ProtoTypes_XmlParseOptions =
{};
XmlParseOptions newXmlParseOptions(XmlParseOptions allocator) {return allocator;}

struct ProtoTypes_XmlTag {
  void  (*by)(                                                                  // Traverse the Xml parse tree rooted at the specified tag in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    const XmlTag * tag,                                                         // Starting tag
    void (* const function) (const XmlTag tag));                                // Function to call on each tag
  void  (*changeName)(                                                          // Change the name of the specified tag.
    const XmlTag * Tag,                                                         // Tag
    const char * const newName);                                                // New name                                                                               //vconst XmlTag t)                                                                 // Tag
  size_t  (*charPosition)(                                                      // Character position of a tag assuming all tags are drawn as open/close not as singletons
    const XmlTag * tag,                                                         // XmlTag
    const int close);                                                           // 0 - start tag, 1 - close tag
  size_t  (*countChildren)(                                                     // Count the number of child tags directly under a parent tag
    const XmlTag * parent);                                                     // Parent tag
  size_t  (*count)(                                                             // Count the number of tags under the specified tag in an Xml parse tree
    const XmlTag * t);                                                          // Parent tag
  void  (*deleteChar)(                                                          // Delete the character at the specified position in the string of a tag.
    const XmlTag  * tag,                                                        // XmlTag
    size_t pos);                                                                // Position in key. 0 deletes the first character.  No deletion occurs if the requested character is beyond the end of the key string
  int  (*depth)(                                                                // The depth of a tag
    const XmlTag * Tag);                                                        // Tag
  void  (*dump)(                                                                // Dump the specified tag
    const XmlTag * Tag);                                                        // Xml
  int  (*empty)(                                                                // Check whether the specified parent tag is empty
    const XmlTag * parent);                                                     // Parent tag
  int  (*equals)(                                                               // Confirm that two tags refer to the same text location in the source document.
    const XmlTag * a,                                                           // First tag
    const XmlTag b);                                                            // Second tag
  XmlTag  (*findFirstChild)(                                                    // Find the first child tag with the specified name under the specified parent tag.
    const XmlTag * Parent,                                                      // Parent
    const char * const key);                                                    // Name of the tag to find
  XmlTag  (*findFirstTag)(                                                      // Find the first tag with the specified name in the Xml parse tree starting at the specified tag.
    const XmlTag * parent,                                                      // Parent tag
    const char * const key);                                                    // Name of the tag to find
  XmlTag  (*first)(                                                             // Return the first child tag under the specified parent tag.
    const XmlTag * parent);                                                     // Parent tag
  int  (*hasText)(                                                              // Check whether the tag contains a text element
    const XmlTag * Parent);                                                     // Parent tag
  void  (*insertChar)(                                                          // Insert the specified character into the string of a tag at the specified position.
    XmlTag  * tag,                                                              // XmlTag
    const char ins,                                                             // Character to insert
    size_t pos);                                                                // Position in key. 0 prepends the char, while >= length appends the char.
  int  (*isFirst)(                                                              // Check that the specified tag is first under its parent
    const XmlTag * Tag);                                                        // Tag
  int  (*isLast)(                                                               // Check that the specified tag is last under its parent
    const XmlTag * Tag);                                                        // Tag
  int  (*isRoot)(                                                               // Check whether the specified tag is the root tag
    const XmlTag * tag);                                                        // Tag
  int  (*isTag)(                                                                // Check that the tag is a tag not text
    const XmlTag * tag);                                                        // Parent tag
  int  (*isText)(                                                               // Check whether we are on a text element
    const XmlTag * tag);                                                        // Tag
  XmlTag  (*last)(                                                              // Return the first child tag under the specified parent tag.
    const XmlTag * parent);                                                     // Parent tag
  size_t  (*length)(                                                            // Length of a tag string
    const XmlTag * tag);                                                        // XmlTag
  XmlTag  (*next)(                                                              // Return the first child tag under the specified parent tag.
    const XmlTag * parent);                                                     // Parent tag
  int  (*onlyText)(                                                             // Return true if a tag contains just one text element and nothing else
    const XmlTag * parent);                                                     // Tag
  XmlTag  (*parent)(                                                            // Return the first child tag under the specified parent tag.
    const XmlTag * parent);                                                     // Parent tag
  StringBuffer  (*prettyPrint)(                                                 // Pretty print the Xml parse tree starting at the specified tag with additional spacing between tags to make the tree easier to read.
    const XmlTag * tag);                                                        // Starting tag
  XmlTag  (*prev)(                                                              // Return the first child tag under the specified parent tag.
    const XmlTag * parent);                                                     // Parent tag
  void  (*printAssert)(                                                         // Print the Xml parse tree starting at the specified tag as an assert statement
    const XmlTag * tag,                                                         // Starting tag
    const char * const variable);                                               // The name of the variable preceding this call
  StringBuffer  (*print)(                                                       // Print the Xml parse tree starting at the specified tag with additional spacing between tags to make the tree easier to read.
    const XmlTag * tag);                                                        // Starting tag
  int  (*printsAs)(                                                             // Check that the Xml parse tree prints as expected
    const XmlTag * tag,                                                         // Xml parse tree
    const char * const expected);                                               // Expected print
  void  (*replaceChar)(                                                         // Replace the character at the specified position in the key string of a tag with the specified character.
    const XmlTag  * tag,                                                        // XmlTag
    const char repl,                                                            // Replacement character
    size_t pos);                                                                // Position in key. 0 replaces the first character.  No replacement occurs if the requested character is beyond the end of the key string
  XmlTag  (*root)(                                                              // Return the root tsg of the Xml parse tree containing the specified tag.
    const XmlTag * tag);                                                        // Tag
  void  (*scan)(                                                                // Traverse the Xml parse tree rooted at the specified tag calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The Xml is buffered allowing changes to be made to the structure of the Xml without disruption as long as each child checks its context.
    const XmlTag * tag,                                                         // Starting tag
    void (* const function) (XmlTag tag, int start, int depth));                // Function to call on each tag: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
  int  (*stayInLineClose)(                                                      // Check whether a closing tag can stay inline: if it is preceded or followed by text or is empty, or its last child was text
    const XmlTag * Tag);                                                        // Tag
  int  (*stayInLine)(                                                           // Check whether an opening or singleton tag can stay in line: if it is text or is preceded or followed by text.
    const XmlTag * Tag);                                                        // Tag
  void  (*swapChars)(                                                           // Delete the character at the specified position in the string of a tag.
    const XmlTag  * tag,                                                        // XmlTag
    size_t pos);                                                                // Position in key. 0 deletes the first character.  No deletion occurs if the requested character is beyond the end of the key string
  int  (*tagNameEquals)(                                                        // Check the name of a tag.
    const XmlTag * tag,                                                         // Tag
    const char * const expected);                                               // Expected name of tag
  char *  (*tagName)(                                                           // Get the tag name from a node in the Xml parse tree. The tag name is returned in a reused static buffer that the caller should not free.
    const XmlTag * Tag);                                                        // Tag
  int  (*tagStringEquals)(                                                      // Check the string of a tag.
    const XmlTag * tag,                                                         // Tag
    const char * const expected);                                               // Expected name of tag
  size_t  (*tagStringLength)(                                                   // Get the length of the entire tag
    const XmlTag * tag);                                                        // Tag
  char *  (*tagString)(                                                         // Get the entire tag from a node in the Xml parse tree.
    const XmlTag * tag);                                                        // Tag
  XmlTag  (*unwrap)(                                                            // Unwrap the specified tag.
    const XmlTag  * tag);                                                       // Tag
  int  (*valid)(                                                                // Check that a tag is valid.
    const XmlTag * tag);                                                        // Tag
  XmlTag  (*wrap)(                                                              // Wrap a specified tag with a new tag and return the newly createdf wraping tag.
    const XmlTag * Tag,                                                         // Tag
    const char * const string);                                                 // Wrapper without the leading < or trailing or >
 } const ProtoTypes_XmlTag =
{by_XmlTag_sub, changeName_XmlTag, charPosition__XmlTag, countChildren_size_XmlTag, count_size_XmlTag, deleteChar__XmlTag_size, depth_int_XmlTag, dump_XmlTag, empty_int_XmlTag, equals_XmlTag_XmlTag, findFirstChild_XmlTag_XmlTag_string, findFirstTag_XmlTag_XmlTag_string, first_XmlTag, hasText_int_XmlTag, insertChar__XmlTag_char_size, isFirst_int_XmlTag, isLast_int_XmlTag, isRoot_int_XmlTag, isTag_int_tag, isText_int_XmlTag, last_XmlTag, length_size__XmlTag, next_XmlTag, onlyText_XmlTag, parent_XmlTag, prettyPrint_stringBuffer_XmlTag, prev_XmlTag, printAssert__XmlTag, print_stringBuffer__XmlTag, printsAs_int__XmlTag_string, replaceChar__XmlTag_char_size, root_XmlTag, scan_XmlTag_sub, stayInLineClose_int_XmlTag, stayInLine_int_XmlTag, swapChars__XmlTag_size, tagNameEquals_int_XmlTag_string, tagName_string_XmlTag, tagStringEquals_int_XmlTag_string, tagStringLength_size_XmlTag, tagString_string_XmlTag, unwrap_XmlTag, valid_int_XmlTag, wrap_XmlTag_string};
XmlTag newXmlTag(XmlTag allocator) {return allocator;}

struct ProtoTypes_XmlValidate {
  void  (*free)(                                                                // Free an Xml validator
    XmlValidate * x);                                                           // Xml validator
 } const ProtoTypes_XmlValidate =
{free_XmlValidate};
XmlValidate newXmlValidate(XmlValidate allocator) {return allocator;}

