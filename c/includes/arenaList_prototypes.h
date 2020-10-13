static ArenaList makeArenaListWithWidth
 (const size_t width);
static ArenaList makeArenaList
 ();
static void swap_ArenaList_ArenaList
 (ArenaList source,
  ArenaList target);
static ArenaList makeArenaListFromLines
 (const char * str);
static ArenaList makeArenaListFromWords
 (const char * str);
static ArenaList makeArenaListFromLinesOfWords
 (const char * str);
static char * check_string_ArenaList
 (const ArenaList list);
static void * pointer__ArenaList_size
 (const ArenaList      list,
  const size_t offset);
static ArenaListContent * content__ArenaListNode
 (const ArenaListNode node);
static size_t width_size__ArenaList
 (const ArenaList list);
static size_t width_size__ArenaListNode
 (const ArenaListNode node);
static char * key_pointer__ArenaListNode
 (const ArenaListNode node);
static void * data_pointer__ArenaListNode
 (const ArenaListNode node);
static size_t length_size__ArenaListNode
 (const ArenaListNode node);
static size_t maxLength_size__ArenaListNode
 (const ArenaListNode node);
static  ArenaListNode  nodeFromOffset__ArenaList_size
 (const ArenaList      list,
  const size_t delta);
static int keyEquals_int__ArenaListNode_pointer_size
 (const ArenaListNode        node,
  const char * const key,
  const size_t       length);
static int equals_int__ArenaListNode_ArenaListNode
 (const ArenaListNode a,
  const ArenaListNode b);
static  ArenaListNode root_ArenaListNode__ArenaList
 (const ArenaList list);
static  ArenaListNode root_ArenaListNode__ArenaListNode
 (const ArenaListNode node);
static  ArenaListNode  allocate_ArenaListNode__ArenaList_size
 (const ArenaList      list,
  const size_t size);
static size_t used_size__ArenaList
 (const ArenaList list);
static ArenaListNode node_ArenaListNode__ArenaList_string_size
 (const ArenaList            list,
  const void * const key,
  const size_t       length);
static void setKey__ArenaListNode_string_size
 (const ArenaListNode        node,
  const void * const key,
  const size_t       length);
static size_t freedSpace_size__ArenaList
 (const ArenaList     list);
static void setData_ArenaListNode_pointer
 (const ArenaListNode        node,
  const void * const data);
static void getData_ArenaListNode_pointer
 (const ArenaListNode  node,
   void * const data);
static void copyData_ArenaListNode_ArenaListNode
 (const ArenaListNode target,
  const ArenaListNode source);
static ArenaListNode  copy_ArenaListNode__ArenaListNode_ArenaList
 (const ArenaListNode source,
  const ArenaList     target);
static void fromLetters__ArenaList_string
 (const ArenaList      list,
  const char * str);
static void free__ArenaList
 (const ArenaList list);
static void free__ArenaListNode
 (ArenaListNode node);
static void insertChar__ArenaListNode_char_size
 (const ArenaListNode node,
  const char  ins,
  size_t      pos);
static void replaceChar__ArenaListNode_size
 (const ArenaListNode node,
  const char  repl,
  size_t      pos);
static void swapChars__ArenaListNode_size
 (const ArenaListNode node,
  size_t      pos);
static void deleteChar__ArenaListNode_size
 (const ArenaListNode node,
  size_t      pos);
static int valid_int__ArenaListNode
 (const ArenaListNode child);
static  ArenaListNode parent_ArenaListNode__ArenaListNode
 (const ArenaListNode child);
static  ArenaListNode first_ArenaListNode__ArenaListNode
 (const ArenaListNode parent);
static  ArenaListNode last_ArenaListNode__ArenaListNode
 (const ArenaListNode parent);
static  ArenaListNode next_ArenaListNode__ArenaListNode
 (const ArenaListNode parent);
static  ArenaListNode prev_ArenaListNode__ArenaListNode
 (const ArenaListNode parent);
static  ArenaListNode first_ArenaListNode__ArenaList
 (const ArenaList list);
static  ArenaListNode last_ArenaListNode__ArenaList
 (const ArenaList list);
static  ArenaListNode next_ArenaListNode__ArenaList
 (const ArenaList list);
static  ArenaListNode prev_ArenaListNode__ArenaList
 (const ArenaList list);
static int equalsString_int__ArenaListNode_string
 (const ArenaListNode        node,
  const char * const key);
static  ArenaListNode findFirst_ArenaListNode__string
 (const ArenaListNode        node,
  const char * const key);
static  ArenaListNode findFirst_ArenaListNode__ArenaList_string
 (const ArenaList            list,
  const char * const key);
static  ArenaListNode findFirstChild_ArenaListNode__ArenaListNode_string
 (const ArenaListNode        parent,
  const char * const key);
static  ArenaListNode findFirstChild_ArenaListNode__ArenaList_string
 (const ArenaList            list,
  const char * const key);
static int context_int__ArenaListNode_ArenaListNode_string
 (const ArenaListNode         child,
        ArenaListNode * const parent,
  const char  * const key);
static int isFirst_int__ArenaListNode
 (const ArenaListNode child);
static int isLast_int__ArenaListNode
 (const ArenaListNode child);
static int isEmpty_int__ArenaListNode
 (const ArenaListNode node);
static int isOnlyChild_int__ArenaListNode
 (const ArenaListNode child);
static int isRoot_int__ArenaListNode
 (const ArenaListNode node);
static  ArenaListNode putFL_ArenaListNode__int_ArenaListNode_ArenaListNode
 (const int   first,
  const ArenaListNode parent,
  const ArenaListNode child);
static  ArenaListNode putTreeFirst_ArenaListNode__ArenaListNode
 (const ArenaListNode child);
static  ArenaListNode putTreeLast_ArenaListNode__ArenaListNode
 (const ArenaListNode child);
static  ArenaListNode putFirst_ArenaListNode__ArenaListNode_ArenaListNode
 (const ArenaListNode parent,
  const ArenaListNode child);
static  ArenaListNode putLast_ArenaListNode__ArenaListNode_ArenaListNode
 (const ArenaListNode parent,
  const ArenaListNode child);
static  ArenaListNode putNP_ArenaListNode__int_ArenaListNode_ArenaListNode
 (const int   next,
  const ArenaListNode sibling,
  const ArenaListNode child);
static void setUp__ArenaListNode_ArenaListNode
 (const ArenaListNode child,
  const ArenaListNode parent);
static  ArenaListNode putNext_ArenaListNode__ArenaListNode_ArenaListNode
 (const ArenaListNode sibling,
  const ArenaListNode child);
static  ArenaListNode putPrev_ArenaListNode__ArenaListNode_ArenaListNode
 (const ArenaListNode sibling,
  const ArenaListNode child);
static  void replace__ArenaListNode_ArenaListNode
 (const ArenaListNode with,
  const ArenaListNode over);
static void splitKey_ArenaListNode_ArenaListNode
 (const ArenaListNode node,
  size_t      pos);
static void by__ArenaListNode_sub
 (ArenaListNode node,
  void (* const function) (const ArenaListNode node));
static void by__ArenaList_sub
 (const ArenaList list,
  void (* const function) (const ArenaListNode node));
static void scan__ArenaListNode_sub
 (ArenaListNode node,
  void (* const function) (ArenaListNode node, int start, int depth));
static void scan__ArenaList_sub
 (const ArenaList list,
  void (* const function) (ArenaListNode node, int start, int depth));
static  size_t countChildren_size__ArenaList
 (const ArenaList list);
static  size_t countChildren_size__ArenaListNode
 (const ArenaListNode parent);
static  size_t count_size__ArenaListNode
 (const ArenaListNode node);
static  size_t count_size__ArenaList
 (const ArenaList list);
static ArenaList copyAndCompact_ArenaList__ArenaList
 (const ArenaList source);
static void printWithBrackets__ArenaListNode_function
 (const ArenaListNode   node,
  void (*printer)(char * string, size_t length));
static void printWithBrackets__string_ArenaList
 (const ArenaList list,
  void (*printer)(char * string, size_t length));
static int printsWithBracketsAs_int__ArenaListNode_string
 (const ArenaListNode        node,
  const char * const expected);
static void dumpWithBrackets__ArenaList
 (const ArenaList list);
static int printsWithBracketsAs_int__ArenaList_string
 (const ArenaList            list,
  const char * const expected);
static void dump__ArenaList
 (const ArenaList list);
static void dump__ArenaListNode
 (const ArenaListNode node);
static void print__ArenaListNode_function
 (const ArenaListNode   node,
  void (*printer)(char * string, size_t length));
static int printsAs_int__ArenaListNode_string
 (const ArenaListNode        node,
  const char * const expected);
static int printsAs_int__ArenaList_string
 (const ArenaList            list,
  const char * const expected);
static int printContains_int__ArenaListNode
 (const ArenaListNode   node,
  const char *  expected);
static int printContains_int__ArenaList
 (const ArenaList       list,
  const char *  expected);
static  ArenaListNode cut_ArenaListNode__ArenaListNode
 (const ArenaListNode child);
static  ArenaListNode unwrap_ArenaListNode__ArenaListNode
 (const ArenaListNode parent);
static  ArenaListNode wrap_ArenaListNode__string
 (const ArenaListNode   child,
  const char * const key);
static void write__ArenaList_string
 (const ArenaList       list,
  const char * const file);
struct ProtoTypes_ArenaList {
  ArenaListNode   (*allocate)(                                                  // Allocate a node within the arena of a ArenaList
    const ArenaList list,                                                       // ArenaList in which to allocate
    const size_t size);                                                         // Amount of memory required
  void  (*by)(                                                                  // Traverse a ArenaList in post-order calling the specified function to process each child node.  The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
    const ArenaList list,                                                       // ArenaList
    void (* const function) (const ArenaListNode node));                        // Function
  char *  (*check)(                                                             // Return a string describing the structure
    const ArenaList list);                                                      // ArenaList
  ArenaList  (*copyAndCompact)(                                                 // Copy a ArenaList compacting any free space.  This method assumes that there are no direct external references to the nodes in the list as this process might change the location of one or more nodes in the arena.
    const ArenaList source);                                                    // Source ArenaList
  size_t  (*countChildren)(                                                     // Count the number of children directly under a parent.
    const ArenaList list);                                                      // ArenaList
  size_t  (*count)(                                                             // Count the number of nodes in a ArenaList
    const ArenaList list);                                                      // ArenaList
  void  (*dumpWithBrackets)(                                                    // Dump a ArenaList printed with brackets to stderr
    const ArenaList list);                                                      // ArenaList
  void  (*dump)(                                                                // Dump a ArenaList on stderr
    const ArenaList list);                                                      // ArenaList
  ArenaListNode  (*findFirstChild)(                                             // Find the first child immediately under the root with the specified key.
    const ArenaList list,                                                       // ArenaList
    const char * const key);                                                    // Key to find
  ArenaListNode  (*findFirst)(                                                  // Find the first node with the specified key in a post-order traversal of the ArenaList.
    const ArenaList list,                                                       // ArenaList
    const char * const key);                                                    // Key to find
  ArenaListNode  (*first)(                                                      // Get the first child in the specified ArenaList.
    const ArenaList list);                                                      // Parent
  void  (*free)(                                                                // Free an entire ArenaList.
    const ArenaList list);                                                      // ArenaList to free
  size_t  (*freedSpace)(                                                        // Amount of space on the free chains
    const ArenaList list);                                                      // ArenaList
  void  (*fromLetters)(                                                         // Load ArenaList from a string of letters and brackets.  The root node of the ArenaList so constructed is always given the letter 'a'.
    const ArenaList list,                                                       // ArenaList
    const char * str);                                                          // String of letters and brackets describing desired list structure
  ArenaListNode  (*last)(                                                       // Get the last child in the specified ArenaList.
    const ArenaList list);                                                      // Parent
  ArenaListNode  (*next)(                                                       // Get the next child in the specified ArenaList.
    const ArenaList list);                                                      // Parent
  ArenaListNode   (*nodeFromOffset)(                                            // Create a node to locate an allocation within the arena of a ArenaList.
    const ArenaList list,                                                       // ArenaList
    const size_t delta);                                                        // Delta within arena. A delta of zero represents no such node.
  ArenaListNode  (*node)(                                                       // Create a new ArenaList node with the specified key.
    const ArenaList list,                                                       // ArenaList in which to create the node
    const void * const key,                                                     // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the ArenaList is determined solely by the user.
    const size_t length);                                                       // Length of the key.
  void *  (*pointer)(                                                           // Return a temporary pointer to data in the arena of the specified ArenaList
    const ArenaList list,                                                       // ArenaList
    const size_t offset);                                                       // Offset
  ArenaListNode  (*prev)(                                                       // Get the prev child in the specified ArenaList.
    const ArenaList list);                                                      // Parent
  int  (*printContains)(                                                        // Check the print of an ArenaList contains the expected string.
    const ArenaList list,                                                       // ArenaList parse ArenaList
    const char * expected);                                                     // Expected string
  void  (*printWithBrackets)(                                                   // Print an entire ArenaList in preorder as a string with brackets around each sub ArenaList
    const ArenaList list,                                                       // ArenaList
    void (*printer) (char * string, size_t length));                            // Function to apply to printed ArenaList
  int  (*printsAs)(                                                             // Check that the specified ArenaList prints as expected.
    const ArenaList list,                                                       // ArenaList
    const char * const expected);                                               // Expected string when printed
  int  (*printsWithBracketsAs)(                                                 // Check that the specified ArenaList prints with brackets as expected.
    const ArenaList list,                                                       // ArenaList
    const char * const expected);                                               // Expected string when printed
  ArenaListNode  (*root)(                                                       // Return the root node of a ArenaList.
    const ArenaList list);                                                      // ArenaList
  void  (*scan)(                                                                // Traverse a ArenaList calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
    const ArenaList list,                                                       // ArenaList
    void (* const function) (ArenaListNode node, int start, int depth));        // Function: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
  void  (*swap)(                                                                // Swap the arenas of two ArenaList
    ArenaList source,                                                           // Source ArenaList
    ArenaList target);                                                          // Target ArenaList
  size_t  (*used)(                                                              // Amount of space currently being used within the arena of a ArenaList.
    const ArenaList list);                                                      // ArenaList
  size_t  (*width)(                                                             // Get the width of the data area for a ArenaList
    const ArenaList list);                                                      // ArenaListNode
  void  (*write)(                                                               // Write a ArenaList to a named file or abort
    const ArenaList list,                                                       // ArenaList
    const char * const file);                                                   // File
 } const ProtoTypes_ArenaList =
{allocate_ArenaListNode__ArenaList_size, by__ArenaList_sub, check_string_ArenaList, copyAndCompact_ArenaList__ArenaList, countChildren_size__ArenaList, count_size__ArenaList, dumpWithBrackets__ArenaList, dump__ArenaList, findFirstChild_ArenaListNode__ArenaList_string, findFirst_ArenaListNode__ArenaList_string, first_ArenaListNode__ArenaList, free__ArenaList, freedSpace_size__ArenaList, fromLetters__ArenaList_string, last_ArenaListNode__ArenaList, next_ArenaListNode__ArenaList, nodeFromOffset__ArenaList_size, node_ArenaListNode__ArenaList_string_size, pointer__ArenaList_size, prev_ArenaListNode__ArenaList, printContains_int__ArenaList, printWithBrackets__string_ArenaList, printsAs_int__ArenaList_string, printsWithBracketsAs_int__ArenaList_string, root_ArenaListNode__ArenaList, scan__ArenaList_sub, swap_ArenaList_ArenaList, used_size__ArenaList, width_size__ArenaList, write__ArenaList_string};
ArenaList newArenaList(ArenaList allocator) {return allocator;}

struct ProtoTypes_ArenaListArena {
 } const ProtoTypes_ArenaListArena =
{};
ArenaListArena newArenaListArena(ArenaListArena allocator) {return allocator;}

struct ProtoTypes_ArenaListContent {
 } const ProtoTypes_ArenaListContent =
{};
ArenaListContent newArenaListContent(ArenaListContent allocator) {return allocator;}

struct ProtoTypes_ArenaListDescription {
 } const ProtoTypes_ArenaListDescription =
{};
ArenaListDescription newArenaListDescription(ArenaListDescription allocator) {return allocator;}

struct ProtoTypes_ArenaListNode {
  void  (*by)(                                                                  // Traverse the ArenaList rooted at the specified node in post-order calling the specified function to process each child node.  The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
    ArenaListNode node,                                                         // ArenaListNode
    void (* const function) (const ArenaListNode node));                        // Function
  ArenaListContent *  (*content)(                                               // Convert a node offset to an address so that the content of a node can be updated in situ as long as the ArenaList is not reallocated to a different position.
    const ArenaListNode node);                                                  // ArenaListNode
  int  (*context)(                                                              // Return true if the parent of the specified child has the specified key.
    const ArenaListNode child,                                                  // Child
    ArenaListNode * const parent,                                               // Parent container
    const char  * const key);                                                   // Key
  void  (*copyData)(                                                            // Copy the data from the source node to the target node.
    const ArenaListNode target,                                                 // Target ArenaListNode
    const ArenaListNode source);                                                // Source ArenaListNode
  ArenaListNode   (*copy)(                                                      // Copy a node from one ArenaList to another ArenaList
    const ArenaListNode source,                                                 // Source ArenaListNode
    const ArenaList target);                                                    // Target ArenaList
  size_t  (*countChildren)(                                                     // Count the number of children directly under a node.
    const ArenaListNode parent);                                                // Parent
  size_t  (*count)(                                                             // Count the number of nodes under a specified node but not including the specified node.
    const ArenaListNode node);                                                  // ArenaListNode
  ArenaListNode  (*cut)(                                                        // Cut out a child.
    const ArenaListNode child);                                                 // Child to cut out
  void *  (*data)(                                                              // Get a temporary pointer to the user data of a node.
    const ArenaListNode node);                                                  // ArenaListNode
  void  (*deleteChar)(                                                          // Delete the character at the specified position in the key string of a node.
    const ArenaListNode node,                                                   // ArenaListNode
    size_t pos);                                                                // Position in key. 0 deletes the first character.  No deletion occurs if the requested character is beyond the end of the key string
  void  (*dump)(                                                                // Dump a ArenaListNode on stderr
    const ArenaListNode node);                                                  // ArenaListNode
  int  (*equalsString)(                                                         // Check that the key of a node equals a string
    const ArenaListNode node,                                                   // ArenaListNode
    const char * const key);                                                    // Key
  int  (*equals)(                                                               // Confirm two nodes are equal
    const ArenaListNode a,                                                      // First offset
    const ArenaListNode b);                                                     // Second offset
  ArenaListNode  (*findFirstChild)(                                             // Find the first child of the specified parent with the specified key.
    const ArenaListNode parent,                                                 // Parent node
    const char * const key);                                                    // Key to find immediately under parent
  ArenaListNode  (*findFirst)(                                                  // Find the first node with the specified key in a post-order traversal of the ArenaList starting at the specified node.
    const ArenaListNode node,                                                   // ArenaListNode at the start of the list to be searched
    const char * const key);                                                    // Key to find
  ArenaListNode  (*first)(                                                      // Get the first child under a parent.
    const ArenaListNode parent);                                                // Parent
  void  (*free)(                                                                // Free a node. If the ArenaList is editable the node is made available for reuse otherwise the node wastes space. A new copy of the ArenaList without wasted space can be made with copyAndCompact_ArenaList .
    ArenaListNode node);                                                        // ArenaListNode to free
  void  (*getData)(                                                             // Get the optional user data associated with a node in an ArenaList
    const ArenaListNode node,                                                   // ArenaListNode
    void * const data);                                                         // Data
  void  (*insertChar)(                                                          // Insert the specified character into the key string of a node at the specified position.
    const ArenaListNode node,                                                   // ArenaListNode
    const char ins,                                                             // Character to insert
    size_t pos);                                                                // Position in key. 0 prepends the char, while >= length appends the char.
  int  (*isEmpty)(                                                              // Confirm a node has no children.
    const ArenaListNode node);                                                  // ArenaListNode
  int  (*isFirst)(                                                              // Confirm a child is first under its parent
    const ArenaListNode child);                                                 // Child
  int  (*isLast)(                                                               // Confirm a child is last under its parent
    const ArenaListNode child);                                                 // Child
  int  (*isOnlyChild)(                                                          // Confirm that this child is the only child of its parent
    const ArenaListNode child);                                                 // Child
  int  (*isRoot)(                                                               // Confirm a node is the root
    const ArenaListNode node);                                                  // ArenaListNode
  int  (*keyEquals)(                                                            // Confirm that the key of a node is equal to the specified memory
    const ArenaListNode node,                                                   // ArenaListNode whose key is to be checked
    const char * const key,                                                     // Expected key value
    const size_t length);                                                       // Length of expected key value
  char *  (*key)(                                                               // Get a temporary pointer to the key of a node.
    const ArenaListNode node);                                                  // ArenaListNode
  ArenaListNode  (*last)(                                                       // Get the last child under a parent.
    const ArenaListNode parent);                                                // Parent
  size_t  (*length)(                                                            // Get the length of the key associated with a node
    const ArenaListNode node);                                                  // ArenaListNode
  size_t  (*maxLength)(                                                         // Get the maximum allowed length of the key associated with a node.
    const ArenaListNode node);                                                  // ArenaListNode
  ArenaListNode  (*next)(                                                       // Get the next child under a parent.
    const ArenaListNode parent);                                                // Parent
  ArenaListNode  (*parent)(                                                     // Get the parent of a child
    const ArenaListNode child);                                                 // Child
  ArenaListNode  (*prev)(                                                       // Get the prev child under a parent.
    const ArenaListNode parent);                                                // Parent
  int  (*printContains)(                                                        // Check the print of an ArenaList starting at the specified tag contains the expected string.
    const ArenaListNode node,                                                   // Starting node
    const char * expected);                                                     // Expected string
  void  (*printWithBrackets)(                                                   // Apply a function to a string containing a print of the specified  node and the ArenaList below it in preorder as a string with each sub ArenaList enclosed in brackets.
    const ArenaListNode node,                                                   // ArenaListNode
    void (*printer) (char * string, size_t length));                            // Function to apply to printed ArenaList
  void  (*print)(                                                               // Apply a function to the print of a ArenaListNode and the tree below it.
    const ArenaListNode node,                                                   // ArenaListNode
    void (*printer) (char * string, size_t length));                            // Function to apply to printed ArenaList
  int  (*printsAs)(                                                             // Check that the specified node prints as expected.
    const ArenaListNode node,                                                   // ArenaList
    const char * const expected);                                               // Expected string when printed
  int  (*printsWithBracketsAs)(                                                 // Check that the ArenaList starting at the specified node prints with brackets as expected.
    const ArenaListNode node,                                                   // ArenaListNode
    const char * const expected);                                               // Expected string when printed
  ArenaListNode  (*putFirst)(                                                   // Put a child first under its parent
    const ArenaListNode parent,                                                 // Parent
    const ArenaListNode child);                                                 // Child
  ArenaListNode  (*putLast)(                                                    // Put a child last under its parent
    const ArenaListNode parent,                                                 // Parent
    const ArenaListNode child);                                                 // Child
  ArenaListNode  (*putNext)(                                                    // Put a child next after the specified sibling
    const ArenaListNode sibling,                                                // Sibling
    const ArenaListNode child);                                                 // Child
  ArenaListNode  (*putPrev)(                                                    // Put a child previous after the specified sibling
    const ArenaListNode sibling,                                                // Sibling
    const ArenaListNode child);                                                 // Child
  ArenaListNode  (*putTreeFirst)(                                               // Put a child first in the ArenaList containing the arena in which the child was allocated.
    const ArenaListNode child);                                                 // Child
  ArenaListNode  (*putTreeLast)(                                                // Put a child last in the ArenaList containing the arena in which the child was allocated.
    const ArenaListNode child);                                                 // Child
  void  (*replaceChar)(                                                         // Replace the character at the specified position in the key string of a node with the specified character.
    const ArenaListNode node,                                                   // ArenaListNode
    const char repl,                                                            // Replacement character
    size_t pos);                                                                // Position in key. 0 replaces the first character.  No replacement occurs if the requested character is beyond the end of the key string
  void  (*replace)(                                                             // Replace the specified node with this node
    const ArenaListNode with,                                                   // Replace with this node
    const ArenaListNode over);                                                  // Node to be replaced
  ArenaListNode  (*root)(                                                       // Return the root node of the ArenaList containing the specified node.
    const ArenaListNode node);                                                  // ArenaListNode
  void  (*scan)(                                                                // Traverse the ArenaList rooted at the specified node calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
    ArenaListNode node,                                                         // ArenaListNode
    void (* const function) (ArenaListNode node, int start, int depth));        // Function: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
  void  (*setData)(                                                             // Set the optional user data associated with a node in an ArenaList
    const ArenaListNode node,                                                   // ArenaListNode
    const void * const data);                                                   // Data
  void  (*setKey)(                                                              // Set the key of a node to a different value as long as there is enough room from the last setting.
    const ArenaListNode node,                                                   // ArenaListNode
    const void * const key,                                                     // Key
    const size_t length);                                                       // Length of key
  void  (*setUp)(                                                               // Make the specified parent node the parent of the specified child node
    const ArenaListNode child,                                                  // Child
    const ArenaListNode parent);                                                // Child
  void  (*splitKey)(                                                            // Split the specified node at the specified position.
    const ArenaListNode node,                                                   // ArenaListNode
    size_t pos);                                                                // Position in key at which to split.
  void  (*swapChars)(                                                           // Swap the characters on either side of the specified position if it is between 1 and length - 1.
    const ArenaListNode node,                                                   // ArenaListNode
    size_t pos);                                                                // Position in key. 1 swaps the first two characters.  length - 1 swaps the last two characters.
  ArenaListNode  (*unwrap)(                                                     // Unwrap the specified parent and return it.
    const ArenaListNode parent);                                                // Parent to unwrap
  int  (*valid)(                                                                // Check that a node is valid.
    const ArenaListNode child);                                                 // ArenaListNode
  size_t  (*width)(                                                             // Get the width of the data area for a node
    const ArenaListNode node);                                                  // ArenaListNode
  ArenaListNode  (*wrap)(                                                       // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
    const ArenaListNode child,                                                  // Child to wrap
    const char * const key);                                                    // Key for new parent
 } const ProtoTypes_ArenaListNode =
{by__ArenaListNode_sub, content__ArenaListNode, context_int__ArenaListNode_ArenaListNode_string, copyData_ArenaListNode_ArenaListNode, copy_ArenaListNode__ArenaListNode_ArenaList, countChildren_size__ArenaListNode, count_size__ArenaListNode, cut_ArenaListNode__ArenaListNode, data_pointer__ArenaListNode, deleteChar__ArenaListNode_size, dump__ArenaListNode, equalsString_int__ArenaListNode_string, equals_int__ArenaListNode_ArenaListNode, findFirstChild_ArenaListNode__ArenaListNode_string, findFirst_ArenaListNode__string, first_ArenaListNode__ArenaListNode, free__ArenaListNode, getData_ArenaListNode_pointer, insertChar__ArenaListNode_char_size, isEmpty_int__ArenaListNode, isFirst_int__ArenaListNode, isLast_int__ArenaListNode, isOnlyChild_int__ArenaListNode, isRoot_int__ArenaListNode, keyEquals_int__ArenaListNode_pointer_size, key_pointer__ArenaListNode, last_ArenaListNode__ArenaListNode, length_size__ArenaListNode, maxLength_size__ArenaListNode, next_ArenaListNode__ArenaListNode, parent_ArenaListNode__ArenaListNode, prev_ArenaListNode__ArenaListNode, printContains_int__ArenaListNode, printWithBrackets__ArenaListNode_function, print__ArenaListNode_function, printsAs_int__ArenaListNode_string, printsWithBracketsAs_int__ArenaListNode_string, putFirst_ArenaListNode__ArenaListNode_ArenaListNode, putLast_ArenaListNode__ArenaListNode_ArenaListNode, putNext_ArenaListNode__ArenaListNode_ArenaListNode, putPrev_ArenaListNode__ArenaListNode_ArenaListNode, putTreeFirst_ArenaListNode__ArenaListNode, putTreeLast_ArenaListNode__ArenaListNode, replaceChar__ArenaListNode_size, replace__ArenaListNode_ArenaListNode, root_ArenaListNode__ArenaListNode, scan__ArenaListNode_sub, setData_ArenaListNode_pointer, setKey__ArenaListNode_string_size, setUp__ArenaListNode_ArenaListNode, splitKey_ArenaListNode_ArenaListNode, swapChars__ArenaListNode_size, unwrap_ArenaListNode__ArenaListNode, valid_int__ArenaListNode, width_size__ArenaListNode, wrap_ArenaListNode__string};
ArenaListNode newArenaListNode(ArenaListNode allocator) {return allocator;}

