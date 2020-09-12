static XmlTag makeXmlTag_XmlParse_ArenaTreeNode
 (const XmlParse        xml,
  const ArenaTreeNode node);
static ArenaTreeNode node_XmlTag
 (const XmlTag tag);
static XmlParse makeXmlParseFromFile
 (FileName            fileName);
static XmlParse makeXmlParseFromString
 (const char * const string);
static void free_XmlParse
 (const XmlParse x);
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
static ReadOnlyBytes print_readOnlyBytes_XmlTag
 (const XmlTag tag);
static ReadOnlyBytes print_ReadOnlyBytes_XmlParse
 (const XmlParse        xml);
static int printsAs_XmlTag
 (const XmlTag          tag,
  const char *  const expected);
static int printsAs_XmlParse
 (const XmlParse  xml,
  const char *  expected);
static int printContains_XmlTag
 (const XmlTag    tag,
  const char *  expected);
static int printContains_XmlParse
 (const XmlParse  xml,
  const char *  expected);
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
  void  (*free)(                                                                // Free an xml parse
    const XmlParse x);                                                          // Xml descriptor
  XmlTag  (*last)(                                                              // Return the first child tag in the specified Xml parse tree.
    const XmlParse xml);                                                        // Parent tag
  XmlTag  (*makeXmlTag)(                                                        // Make a tag descriptor from a parse tree node holding the tag
    const XmlParse xml,                                                         // Xml parse tree
    const ArenaTreeNode node);                                                  // Node holding tag
  int  (*printContains)(                                                        // Check the print of an Xml parse tree contains the expected string.
    const XmlParse xml,                                                         // Xml parse tree
    const char * expected);                                                     // Expected string
  ReadOnlyBytes  (*print)(                                                      // Print an entire Xml parse tree as a string.
    const XmlParse xml);                                                        // Xml parse tree
  int  (*printsAs)(                                                             // Check the print of an Xml parse tree is as expected.
    const XmlParse xml,                                                         // Xml parse tree
    const char * expected);                                                     // Expected string
  XmlTag  (*root)(                                                              // Return the root tag of the specified Xml parse tree
    const XmlParse xml);                                                        // Xml parse tree
 } const ProtoTypes_XmlParse =
{by_XmlParse_sub, countChildren_size_Xml, count_size_Xml, errors_XmlParse, findFirstChild_XmlTag_XmlParse_string, findFirstTag_XmlTag_XmlParse_string, first_XmlParse, free_XmlParse, last_XmlParse, makeXmlTag_XmlParse_ArenaTreeNode, printContains_XmlParse, print_ReadOnlyBytes_XmlParse, printsAs_XmlParse, root_XmlParse};
XmlParse newXmlParse(XmlParse allocator) {return allocator;}

struct ProtoTypes_XmlTag {
  void  (*by)(                                                                  // Traverse the Xml parse tree rooted at the specified tag in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    const XmlTag tag,                                                           // Starting tag
    void (* const function) (const XmlTag tag));                                // Function to call on each tag
  size_t  (*countChildren)(                                                     // Count the number of child tags directly under a parent tag
    const XmlTag parent);                                                       // Parent tag
  size_t  (*count)(                                                             // Count the number of tags under the specified tag in an Xml parse tree
    const XmlTag t);                                                            // Parent tag
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
  XmlTag  (*last)(                                                              // Return the first child tag under the specified parent tag.
    const XmlTag parent);                                                       // Parent tag
  XmlTag  (*next)(                                                              // Return the first child tag under the specified parent tag.
    const XmlTag parent);                                                       // Parent tag
  ArenaTreeNode  (*node)(                                                       // Get a node from a tag
    const XmlTag tag);                                                          // Tag
  XmlTag  (*parent)(                                                            // Return the first child tag under the specified parent tag.
    const XmlTag parent);                                                       // Parent tag
  XmlTag  (*prev)(                                                              // Return the first child tag under the specified parent tag.
    const XmlTag parent);                                                       // Parent tag
  int  (*printContains)(                                                        // Check the print of an Xml parse tree starting at the specified tag contains the expected string
    const XmlTag tag,                                                           // Starting tag
    const char * expected);                                                     // Expected string
  ReadOnlyBytes  (*print)(                                                      // Print the parse tree as a string starting at the specified tag.
    const XmlTag tag);                                                          // Starting tag
  int  (*printsAs)(                                                             // Check the print of an Xml parse tree starting at the specified tag is as expected.
    const XmlTag tag,                                                           // Starting tag
    const char *  const expected);                                              // Expected string
  XmlTag  (*root)(                                                              // Return the root tsg of the Xml parse tree containing the specified tag.
    const XmlTag tag);                                                          // Tag
  int  (*tagNameEquals)(                                                        // Check the name of a tag.
    const XmlTag tag,                                                           // Tag
    const char * const expected);                                               // Expected name of tag
  char *  (*tagName)(                                                           // Get the tag name from a node in the Xml parse tree. The tag name is returned in a reused static buffer that the caller should not free.
    const XmlTag tag);                                                          // Tag
  int  (*tagStringEquals)(                                                      // Check the string of a tag.
    const XmlTag tag,                                                           // Tag
    const char * const expected);                                               // Expected name of tag
  char *  (*tagString)(                                                         // Get the entire tag from a node in the Xml parse tree.
    const XmlTag tag);                                                          // Tag
  int  (*valid)(                                                                // Check that a tag is valid.
    const XmlTag tag);                                                          // Tag
 } const ProtoTypes_XmlTag =
{by_XmlTag_sub, countChildren_size_XmlTag, count_size_XmlTag, equals_XmlTag_XmlTag, findFirstChild_XmlTag_XmlTag_string, findFirstTag_XmlTag_XmlTag_string, first_XmlTag, last_XmlTag, next_XmlTag, node_XmlTag, parent_XmlTag, prev_XmlTag, printContains_XmlTag, print_readOnlyBytes_XmlTag, printsAs_XmlTag, root_XmlTag, tagNameEquals_XmlTag_string, tagName_XmlTag, tagStringEquals_XmlTag_string, tagString_XmlTag, valid_XmlTag};
XmlTag newXmlTag(XmlTag allocator) {return allocator;}

