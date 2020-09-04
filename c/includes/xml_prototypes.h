static XmlTag makeXmlTag_XmlParse_ArenaTreeNode
 (XmlParse        xml,
  ArenaTreeNode node);
static ArenaTreeNode node_XmlTag
 (XmlTag tag);
static XmlParse makeXmlParseFromFile
 (FileName      fileName);
static void free_XmlParse
 (XmlParse x);
static char * print_XmlParse
 (XmlParse x);
static int printsAs_XmlParse
 (const XmlParse x,
  const char * expected);
static char * getTag_Xml
 (XmlTag tag);
static void by_Xml_sub
 (XmlParse xml,
  void (* const function) (const ArenaTreeNode node));
struct ProtoTypes_XmlParse {
  void  (*by)(                                                                  // Traverse an xml parse tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    XmlParse xml,                                                               // Xml parse tree
    void (* const function) (const ArenaTreeNode node));                        // Function
  void  (*free)(                                                                // Free an xml parse
    XmlParse x);                                                                // Xml descriptor
  XmlTag  (*makeXmlTag)(                                                        // Make a tag descriptor from a parse tree node holding the tag
    XmlParse xml,                                                               // Xml parse tree
    ArenaTreeNode node);                                                        // Node holding tag
  char *  (*print)(                                                             // Print an Xml parse tree
    XmlParse x);                                                                // Xml parse tree
  int  (*printsAs)(                                                             // Check the print of a parse tree is as expected.
    const XmlParse x,                                                           // Xml descriptor
    const char * expected);                                                     // Expected print string
 } const ProtoTypes_XmlParse =
{by_Xml_sub, free_XmlParse, makeXmlTag_XmlParse_ArenaTreeNode, print_XmlParse, printsAs_XmlParse};
XmlParse newXmlParse(XmlParse allocator) {return allocator;}

struct ProtoTypes_XmlTag {
  char *  (*getTag)(                                                            // Get the tag name from a node in the Xml parse tree. The tag name is returned in a reused static buffer that the caller should not free.
    XmlTag tag);                                                                // Tag
  ArenaTreeNode  (*node)(                                                       // Get a node from a tag
    XmlTag tag);                                                                // Tag
 } const ProtoTypes_XmlTag =
{getTag_Xml, node_XmlTag};
XmlTag newXmlTag(XmlTag allocator) {return allocator;}

