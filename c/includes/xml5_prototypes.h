static Xml5Tag makeXml5Tag_Xml5Parse_ArenaTreeNode
 (const Xml5Parse        xml,
  const ArenaTreeNode node);
static ArenaTreeNode node_Xml5Tag
 (const Xml5Tag tag);
static Xml5Parse makeXml5ParseFromFile
 (FileName            fileName);
static void free_Xml5Parse
 (const Xml5Parse x);
static size_t errors_Xml5Parse
 (const Xml5Parse x);
static char  * parseXml5TagName
 (const char * const tagString);
static char * tagName_Xml5Tag
 (const Xml5Tag tag);
static int tagNameEquals_Xml5Tag_string
 (const Xml5Tag         tag,
  const char * const expected);
static char * tagString_Xml5Tag
 (const Xml5Tag tag);
static int tagStringEquals_Xml5Tag_string
 (const Xml5Tag         tag,
  const char * const expected);
static Xml5Tag first_Xml5Tag
 (const Xml5Tag parent);
static Xml5Tag last_Xml5Tag
 (const Xml5Tag parent);
static Xml5Tag next_Xml5Tag
 (const Xml5Tag sibling);
static Xml5Tag prev_Xml5Tag
 (const Xml5Tag sibling);
static int valid_Xml5Tag
 (const Xml5Tag tag);
static Xml5Tag findFirstChild_Xml5Tag_Xml5Tag_string
 (const Xml5Tag   parent,
  const char * const key);
static Xml5Tag findFirstChild_Xml5Tag_Xml5Parse_string
 (const Xml5Parse xml,
  const char * const key);
static Xml5Tag findFirstTag_Xml5Tag_Xml5Tag_string
 (const Xml5Tag   parent,
  const char * const key);
static Xml5Tag findFirstTag_Xml5Tag_Xml5Parse_string
 (const Xml5Parse       xml,
  const char * const key);
static Xml5Tag root_Xml5Parse
 (const Xml5Parse xml);
static Xml5Tag root_Xml5Tag
 (const Xml5Tag tag);
static int equals_Xml5Tag_Xml5Tag
 (const Xml5Tag a,
  const Xml5Tag b);
static void by_Xml5Tag_sub
 (const Xml5Tag tag,
  void (* const function) (const Xml5Tag tag));
static void by_Xml5Parse_sub
 (const Xml5Parse xml,
  void (* const function) (const Xml5Tag tag));
static  size_t countChildren_size_Xml5Tag
 (const Xml5Tag parent);
static  size_t count_size_Xml5
 (const Xml5Parse x);
static ReadOnlyBytes print_readOnlyBytes_Xml5Tag
 (const Xml5Tag tag);
static ReadOnlyBytes print_ReadOnlyBytes_Xml5Parse
 (const Xml5Parse        xml);
static int printsAs_Xml5Tag
 (const Xml5Tag          tag,
  const char *  const expected);
static int printsAs_Xml5Parse
 (const Xml5Parse  xml,
  const char *  expected);
static int printContains_Xml5Tag
 (const Xml5Tag    tag,
  const char *  expected);
static int printContains_Xml5Parse
 (const Xml5Parse  xml,
  const char *  expected);
struct ProtoTypes_Xml5Parse {
  void  (*by)(                                                                  // Traverse the Xml5 parse tree in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    const Xml5Parse xml,                                                        // Xml5 parse tree
    void (* const function) (const Xml5Tag tag));                               // Function to call on each tag
  size_t  (*count)(                                                             // Count the number of tags in an Xml5 parse tree
    const Xml5Parse x);                                                         // Xml5 parse tree
  size_t  (*errors)(                                                            // Number of errors encountered while creating an Xml5 parse tree.
    const Xml5Parse x);                                                         // Xml5 descriptor
  Xml5Tag  (*findFirstChild)(                                                   // Find the first child tag with the specified name under the root tag of the specified parse tree.
    const Xml5Parse xml,                                                        // Parent
    const char * const key);                                                    // Name of the tag to find
  Xml5Tag  (*findFirstTag)(                                                     // Find the first tag in an Xml5 parse tree with the specified name.
    const Xml5Parse xml,                                                        // Xml parse tree
    const char * const key);                                                    // Name of the tag to find
  void  (*free)(                                                                // Free an xml parse
    const Xml5Parse x);                                                         // Xml5 descriptor
  Xml5Tag  (*makeXml5Tag)(                                                      // Make a tag descriptor from a parse tree node holding the tag
    const Xml5Parse xml,                                                        // Xml parse tree
    const ArenaTreeNode node);                                                  // Node holding tag
  int  (*printContains)(                                                        // Check the print of an Xml5 parse tree contains the expected string.
    const Xml5Parse xml,                                                        // Xml5 parse tree
    const char * expected);                                                     // Expected string
  ReadOnlyBytes  (*print)(                                                      // Print an entire Xml5 parse tree as a string.
    const Xml5Parse xml);                                                       // Xml5 parse tree
  int  (*printsAs)(                                                             // Check the print of an Xml5 parse tree is as expected.
    const Xml5Parse xml,                                                        // Xml5 parse tree
    const char * expected);                                                     // Expected string
  Xml5Tag  (*root)(                                                             // Return the root tag of the specified Xml5 parse tree
    const Xml5Parse xml);                                                       // Xml5 parse tree
 } const ProtoTypes_Xml5Parse =
{by_Xml5Parse_sub, count_size_Xml5, errors_Xml5Parse, findFirstChild_Xml5Tag_Xml5Parse_string, findFirstTag_Xml5Tag_Xml5Parse_string, free_Xml5Parse, makeXml5Tag_Xml5Parse_ArenaTreeNode, printContains_Xml5Parse, print_ReadOnlyBytes_Xml5Parse, printsAs_Xml5Parse, root_Xml5Parse};
Xml5Parse newXml5Parse(Xml5Parse allocator) {return allocator;}

struct ProtoTypes_Xml5Tag {
  void  (*by)(                                                                  // Traverse the Xml5 parse tree rooted at the specified tag in post-order calling the specified function to process each tag.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    const Xml5Tag tag,                                                          // Starting tag
    void (* const function) (const Xml5Tag tag));                               // Function to call on each tag
  size_t  (*countChildren)(                                                     // Count the number of child tags directly under a parent tag
    const Xml5Tag parent);                                                      // Parent tag
  int  (*equals)(                                                               // Confirm that two tags refer to the same text location in the source document.
    const Xml5Tag a,                                                            // First tag
    const Xml5Tag b);                                                           // Second tag
  Xml5Tag  (*findFirstChild)(                                                   // Find the first child tag with the specified name under the specified parent tag.
    const Xml5Tag parent,                                                       // Parent
    const char * const key);                                                    // Name of the tag to find
  Xml5Tag  (*findFirstTag)(                                                     // Find the first tag with the specified name in the Xml5 parse tree starting at the specified tag.
    const Xml5Tag parent,                                                       // Parent tag
    const char * const key);                                                    // Name of the tag to find
  Xml5Tag  (*first)(                                                            // Return the first child tag under the specified parent tag.
    const Xml5Tag parent);                                                      // Parent tag
  Xml5Tag  (*last)(                                                             // Return the last child tag under the specified parent tag.
    const Xml5Tag parent);                                                      // Parent tag
  Xml5Tag  (*next)(                                                             // Return the next sibling tag after this one.
    const Xml5Tag sibling);                                                     // Sibling tag
  ArenaTreeNode  (*node)(                                                       // Get a node from a tag
    const Xml5Tag tag);                                                         // Tag
  Xml5Tag  (*prev)(                                                             // Return the previous sibling tag before this one.
    const Xml5Tag sibling);                                                     // Sibling tag
  int  (*printContains)(                                                        // Check the print of an Xml5 parse tree starting at the specified tag contains the expected string
    const Xml5Tag tag,                                                          // Starting tag
    const char * expected);                                                     // Expected string
  ReadOnlyBytes  (*print)(                                                      // Print the parse tree as a string starting at the specified tag.
    const Xml5Tag tag);                                                         // Starting tag
  int  (*printsAs)(                                                             // Check the print of an Xml5 parse tree starting at the specified tag is as expected.
    const Xml5Tag tag,                                                          // Starting tag
    const char *  const expected);                                              // Expected string
  Xml5Tag  (*root)(                                                             // Return the root tsg of the Xml5 parse tree containing the specified tag.
    const Xml5Tag tag);                                                         // Tag
  int  (*tagNameEquals)(                                                        // Check the name of a tag.
    const Xml5Tag tag,                                                          // Tag
    const char * const expected);                                               // Expected name of tag
  char *  (*tagName)(                                                           // Get the tag name from a node in the Xml5 parse tree. The tag name is returned in a reused static buffer that the caller should not free.
    const Xml5Tag tag);                                                         // Tag
  int  (*tagStringEquals)(                                                      // Check the string of a tag.
    const Xml5Tag tag,                                                          // Tag
    const char * const expected);                                               // Expected name of tag
  char *  (*tagString)(                                                         // Get the entire tag from a node in the Xml5 parse tree.
    const Xml5Tag tag);                                                         // Tag
  int  (*valid)(                                                                // Check that a tag is valid.
    const Xml5Tag tag);                                                         // Tag
 } const ProtoTypes_Xml5Tag =
{by_Xml5Tag_sub, countChildren_size_Xml5Tag, equals_Xml5Tag_Xml5Tag, findFirstChild_Xml5Tag_Xml5Tag_string, findFirstTag_Xml5Tag_Xml5Tag_string, first_Xml5Tag, last_Xml5Tag, next_Xml5Tag, node_Xml5Tag, prev_Xml5Tag, printContains_Xml5Tag, print_readOnlyBytes_Xml5Tag, printsAs_Xml5Tag, root_Xml5Tag, tagNameEquals_Xml5Tag_string, tagName_Xml5Tag, tagStringEquals_Xml5Tag_string, tagString_Xml5Tag, valid_Xml5Tag};
Xml5Tag newXml5Tag(Xml5Tag allocator) {return allocator;}

