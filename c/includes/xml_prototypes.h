static XmlTag makeXmlTag_XmlParse_ArenaTreeNode
 (XmlParse        xml,
  ArenaTreeNode node);
static ArenaTreeNode node_XmlTag
 (XmlTag tag);
static XmlParse makeXmlParseFromFile
 (FileName          fileName);
static void free_XmlParse
 (XmlParse x);
static size_t errors_XmlParse
 (XmlParse x);
static char  * parseXmlTagName
 (const char * const tagString);
static char * getTag_XmlTag
 (XmlTag tag);
static int tagNameIs_XmlTag_string
 (XmlTag         tag,
  char * const expected);
static XmlTag first_XmlTag
 (XmlTag parent);
static XmlTag last_XmlTag
 (XmlTag parent);
static XmlTag next_XmlTag
 (XmlTag sibling);
static XmlTag prev_XmlTag
 (XmlTag sibling);
static int valid_XmlTag
 (const XmlTag tag);
static XmlTag findFirstChild_XmlTag_XmlTag_string
 (XmlTag         parent,
  char * const key);
static XmlTag findFirstChild_XmlTag_XmlParse_string
 (XmlParse       xml,
  char * const key);
static XmlTag findFirstTag_XmlTag_XmlTag_string
 (XmlTag         parent,
  char * const key);
static XmlTag findFirstTag_XmlTag_XmlParse_string
 (XmlParse       xml,
  char * const key);
static XmlTag root_XmlParse
 (XmlParse xml);
static XmlTag root_XmlTag
 (XmlTag tag);
static void by_XmlTag_sub
 (XmlTag tag,
  void (* const function) (const XmlTag tag));
static void by_XmlParse_sub
 (XmlParse xml,
  void (* const function) (const XmlTag tag));
static ReadOnlyBytes print_readOnlyBytes_XmlTag
 (const XmlTag tag);
static ReadOnlyBytes print_ReadOnlyBytes_XmlParse
 (XmlParse xml);
static int printsAs_XmlTag
 (const XmlTag    tag,
  const char *  expected);
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
    XmlParse xml,                                                               // Xml parse tree
    void (* const function) (const XmlTag tag));                                // Function to call on each tag
  size_t  (*errors)(                                                            // Number of errors encountered while creating an Xml parse tree.
    XmlParse x);                                                                // Xml descriptor
  XmlTag  (*findFirstChild)(                                                    // Find the first child tag with the specified name under the root tag of the specified parse tree.
    XmlParse xml,                                                               // Parent
    char * const key);                                                          // Name of the tag to find
  XmlTag  (*findFirstTag)(                                                      // Find the first tag in an Xml parse tree with the specified name.
    XmlParse xml,                                                               // Xml parse tree
    char * const key);                                                          // Name of the tag to find
  void  (*free)(                                                                // Free an xml parse
    XmlParse x);                                                                // Xml descriptor
  XmlTag  (*makeXmlTag)(                                                        // Make a tag descriptor from a parse tree node holding the tag
    XmlParse xml,                                                               // Xml parse tree
    ArenaTreeNode node);                                                        // Node holding tag
  int  (*printContains)(                                                        // Check the print of an Xml parse tree contains the expected string.
    const XmlParse xml,                                                         // Xml parse tree
    const char * expected);                                                     // Expected string
  ReadOnlyBytes  (*print)(                                                      // Print an entire Xml parse tree as a string.
    XmlParse xml);                                                              // Xml parse tree
  int  (*printsAs)(                                                             // Check the print of an Xml parse tree is as expected.
    const XmlParse xml,                                                         // Xml parse tree
    const char * expected);                                                     // Expected string
  XmlTag  (*root)(                                                              // Return the root tag of the specified Xml parse tree
    XmlParse xml);                                                              // Xml parse tree
 } const ProtoTypes_XmlParse =
{by_XmlParse_sub, errors_XmlParse, findFirstChild_XmlTag_XmlParse_string, findFirstTag_XmlTag_XmlParse_string, free_XmlParse, makeXmlTag_XmlParse_ArenaTreeNode, printContains_XmlParse, print_ReadOnlyBytes_XmlParse, printsAs_XmlParse, root_XmlParse};
XmlParse newXmlParse(XmlParse allocator) {return allocator;}

struct ProtoTypes_XmlTag {
  void  (*by)(                                                                  // Traverse the Xml parse tree rooted at the specified tag in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    XmlTag tag,                                                                 // Starting tag
    void (* const function) (const XmlTag tag));                                // Function to call on each tag
  XmlTag  (*findFirstChild)(                                                    // Find the first child tag with the specified name under the specified parent tag.
    XmlTag parent,                                                              // Parent
    char * const key);                                                          // Name of the tag to find
  XmlTag  (*findFirstTag)(                                                      // Find the first tag with the specified name in the Xml parse tree starting at the specified tag.
    XmlTag parent,                                                              // Parent tag
    char * const key);                                                          // Name of the tag to find
  XmlTag  (*first)(                                                             // Return the first child tag under the specified parent tag.
    XmlTag parent);                                                             // Parent tag
  char *  (*getTag)(                                                            // Get the tag name from a node in the Xml parse tree. The tag name is returned in a reused static buffer that the caller should not free.
    XmlTag tag);                                                                // Tag
  XmlTag  (*last)(                                                              // Return the last child tag under the specified parent tag.
    XmlTag parent);                                                             // Parent tag
  XmlTag  (*next)(                                                              // Return the next sibling tag after this one.
    XmlTag sibling);                                                            // Sibling tag
  ArenaTreeNode  (*node)(                                                       // Get a node from a tag
    XmlTag tag);                                                                // Tag
  XmlTag  (*prev)(                                                              // Return the previous sibling tag before this one.
    XmlTag sibling);                                                            // Sibling tag
  int  (*printContains)(                                                        // Check the print of an Xml parse tree starting at the specified tag contains the expected string
    const XmlTag tag,                                                           // Starting tag
    const char * expected);                                                     // Expected string
  ReadOnlyBytes  (*print)(                                                      // Print the parse tree as a string starting at the specified tag.
    const XmlTag tag);                                                          // Starting tag
  int  (*printsAs)(                                                             // Check the print of an Xml parse tree starting at the specified tag is as expected.
    const XmlTag tag,                                                           // Starting tag
    const char * expected);                                                     // Expected string
  XmlTag  (*root)(                                                              // Return the root tsg of the Xml parse tree containing the specified tag.
    XmlTag tag);                                                                // Tag
  int  (*tagNameIs)(                                                            // Check the name of a tag.
    XmlTag tag,                                                                 // Tag
    char * const expected);                                                     // Expected name of tag
  int  (*valid)(                                                                // Check that a tag is valid.
    const XmlTag tag);                                                          // Tag
 } const ProtoTypes_XmlTag =
{by_XmlTag_sub, findFirstChild_XmlTag_XmlTag_string, findFirstTag_XmlTag_XmlTag_string, first_XmlTag, getTag_XmlTag, last_XmlTag, next_XmlTag, node_XmlTag, prev_XmlTag, printContains_XmlTag, print_readOnlyBytes_XmlTag, printsAs_XmlTag, root_XmlTag, tagNameIs_XmlTag_string, valid_XmlTag};
XmlTag newXmlTag(XmlTag allocator) {return allocator;}

