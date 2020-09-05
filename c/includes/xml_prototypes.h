static XmlTag makeXmlTag_XmlParse_ArenaTreeNode
 (XmlParse        xml,
  ArenaTreeNode node);
static ArenaTreeNode node_XmlTag
 (XmlTag tag);
static XmlParse makeXmlParseFromFile
 (FileName      fileName);
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
static int findFirstTag_int_tree_string_XmlNodePointer
 (XmlParse       xml,
  char * const key,
  XmlTag * const result);
static void by_Xml_sub
 (XmlParse xml,
  void (* const function) (const ArenaTreeNode node));
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
  void  (*by)(                                                                  // Traverse an xml parse tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    XmlParse xml,                                                               // Xml parse tree
    void (* const function) (const ArenaTreeNode node));                        // Function
  size_t  (*errors)(                                                            // Number of errors encountered while creating an Xml parse tree.
    XmlParse x);                                                                // Xml descriptor
  int  (*findFirstTag)(                                                         // Find the first node in an Xml parse tree with the specified tag.
    XmlParse xml,                                                               // Xml parse tree
    char * const key,                                                           // Name of the tag to find
    XmlTag * const result);                                                     // Output area for tag if found
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
 } const ProtoTypes_XmlParse =
{by_Xml_sub, errors_XmlParse, findFirstTag_int_tree_string_XmlNodePointer, free_XmlParse, makeXmlTag_XmlParse_ArenaTreeNode, printContains_XmlParse, print_ReadOnlyBytes_XmlParse, printsAs_XmlParse};
XmlParse newXmlParse(XmlParse allocator) {return allocator;}

struct ProtoTypes_XmlTag {
  char *  (*getTag)(                                                            // Get the tag name from a node in the Xml parse tree. The tag name is returned in a reused static buffer that the caller should not free.
    XmlTag tag);                                                                // Tag
  ArenaTreeNode  (*node)(                                                       // Get a node from a tag
    XmlTag tag);                                                                // Tag
  int  (*printContains)(                                                        // Check the print of an Xml parse tree starting at the specified tag contains the expected string
    const XmlTag tag,                                                           // Starting tag
    const char * expected);                                                     // Expected string
  ReadOnlyBytes  (*print)(                                                      // Print the parse tree as a string starting at the specified tag.
    const XmlTag tag);                                                          // Starting tag
  int  (*printsAs)(                                                             // Check the print of an Xml parse tree starting at the specified tag is as expected.
    const XmlTag tag,                                                           // Starting tag
    const char * expected);                                                     // Expected string
  int  (*tagNameIs)(                                                            // Check the name of a tag.
    XmlTag tag,                                                                 // Tag
    char * const expected);                                                     // Expected name of tag
 } const ProtoTypes_XmlTag =
{getTag_XmlTag, node_XmlTag, printContains_XmlTag, print_readOnlyBytes_XmlTag, printsAs_XmlTag, tagNameIs_XmlTag_string};
XmlTag newXmlTag(XmlTag allocator) {return allocator;}

