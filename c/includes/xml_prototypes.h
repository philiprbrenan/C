static void free_Xml
 (Xml x);
static char * print_Xml
 (Xml x);
static int printsAs_Xml
 (const Xml      x,
  const char * expected);
static ReadOnlyBytes getTag_Xml
 (XmlNode node);
static void by_Xml_sub
 (Xml xml,
  void (* const function) (const XmlNode node));
struct ProtoTypes_Xml {
  void  (*by)(                                                                  // Traverse an xml parse tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    Xml xml,                                                                    // Xml parse tree
    void (* const function) (const XmlNode node));                              // Function
  void  (*free)(                                                                // Free an xml tree
    Xml x);                                                                     // Xml descriptor
  char *  (*print)(                                                             // Print an Xml parse tree
    Xml x);                                                                     // Xml descriptor
  int  (*printsAs)(                                                             // Check the print of a parse tree is as expected.
    const Xml x,                                                                // Xml descriptor
    const char * expected);                                                     // Expected print string
 } const ProtoTypes_Xml =
{by_Xml_sub, free_Xml, print_Xml, printsAs_Xml};
struct ProtoTypes_XmlNode {
  ReadOnlyBytes  (*getTag)(                                                     // Get the tag name.
    XmlNode node);                                                              // Node in parse tree
 } const ProtoTypes_XmlNode =
{getTag_Xml};
