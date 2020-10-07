static ArenaList makeArenaListWithWidth
 (const size_t width);
static ArenaList makeArenaList
 ();
static void swap_ArenaList_ArenaList
 (ArenaList source,
  ArenaList target);
static char * check_string_ArenaList
 (const ArenaList list);
static void * pointer_ArenaList_size
 (const ArenaList      list,
  const size_t offset);
static ArenaListContent * content_ArenaListNode
 (const ArenaListNode node);
static size_t width_size_ArenaList
 (const ArenaList list);
static size_t width_size_ArenaListNode
 (const ArenaListNode node);
static void * key_pointer_ArenaListNode
 (const ArenaListNode node);
static void * data_pointer_ArenaListNode
 (const ArenaListNode node);
static size_t length_size_ArenaListNode
 (const ArenaListNode node);
static  ArenaListNode  nodeFromOffset_ArenaList_size
 (const ArenaList      list,
  const size_t delta);
static int keyEquals_int_ArenaListNode_pointer_size
 (const ArenaListNode        node,
  const char * const key,
  const size_t       length);
static int equals_int_ArenaListNode_ArenaListNode
 (const ArenaListNode a,
  const ArenaListNode b);
static  ArenaListNode root_ArenaListNode_ArenaList
 (const ArenaList list);
static  ArenaListNode root_ArenaListNodeOffset_ArenaListNodeOffset
 (const ArenaListNode node);
static  ArenaListNode  allocate_offset_ArenaList_size
 (const ArenaList      list,
  const size_t size);
static size_t used_ArenaList
 (const ArenaList list);
static ArenaListNode node_ArenaListNode_ArenaList_string_size
 (const ArenaList            list,
  const void * const key,
  const size_t       length);
static void setKey_ArenaListNode_string_size
 (const ArenaListNode        node,
  const void * const key,
  const size_t       length);
static void setData_ArenaListNode_pointer
 (const ArenaListNode        node,
  const void * const data);
static void getData_ArenaListNode_pointer
 (const ArenaListNode  node,
   void * const data);
static void copyData_ArenaListNode_ArenaListNode
 (const ArenaListNode target,
  const ArenaListNode source);
static ArenaListNode  copy_ArenaList_ArenaListNode
 (const ArenaListNode source,
  const ArenaList     target);
static void fromLetters_ArenaList_string
 (const ArenaList      list,
  const char * str);
static void free_ArenaList
 (const ArenaList list);
static void free_ArenaListNode
 (ArenaListNode node);
static int valid_ArenaListNode
 (const ArenaListNode child);
static  ArenaListNode parent_ArenaListNode_ArenaListNode
 (const ArenaListNode child);
static  ArenaListNode first_ArenaListNode_ArenaListNode
 (const ArenaListNode parent);
static  ArenaListNode last_ArenaListNode_ArenaListNode
 (const ArenaListNode parent);
static  ArenaListNode next_ArenaListNode_ArenaListNode
 (const ArenaListNode parent);
static  ArenaListNode prev_ArenaListNode_ArenaListNode
 (const ArenaListNode parent);
static  ArenaListNode first_ArenaListNode_ArenaList
 (const ArenaList list);
static  ArenaListNode last_ArenaListNode_ArenaList
 (const ArenaList list);
static  ArenaListNode next_ArenaListNode_ArenaList
 (const ArenaList list);
static  ArenaListNode prev_ArenaListNode_ArenaList
 (const ArenaList list);
static int equalsString_ArenaListNode_string
 (const ArenaListNode        node,
  const char * const key);
static  ArenaListNode findFirst_ArenaListNode_string
 (const ArenaListNode        node,
  const char * const key);
static  ArenaListNode findFirst_ArenaList_string
 (const ArenaList            list,
  const char * const key);
static  ArenaListNode findFirstChild_ArenaListNode_string
 (const ArenaListNode        parent,
  const char * const key);
static  ArenaListNode findFirstChild_ArenaList_string
 (const ArenaList            list,
  const char * const key);
static int context_ArenaListNode
 (const ArenaListNode         child,
        ArenaListNode * const parent,
  const char  * const key);
static int isFirst_ArenaListNode
 (const ArenaListNode child);
static int isLast_ArenaListNode
 (const ArenaListNode child);
static int isEmpty_ArenaListNode
 (const ArenaListNode node);
static int isOnlyChild_ArenaListNode
 (const ArenaListNode child);
static int isRoot_ArenaListNode
 (const ArenaListNode node);
static  ArenaListNode putFL_ArenaListNode_ArenaListNode_ArenaListNode
 (const int   first,
  const ArenaListNode parent,
  const ArenaListNode child);
static  ArenaListNode putTreeFirst_ArenaListNode_ArenaListNode
 (const ArenaListNode child);
static  ArenaListNode putTreeLast_ArenaListNode_ArenaListNode
 (const ArenaListNode child);
static  ArenaListNode putFirst_ArenaListNode_ArenaListNode_ArenaListNode
 (const ArenaListNode parent,
  const ArenaListNode child);
static  ArenaListNode putLast_ArenaListNode_ArenaListNode_ArenaListNode
 (const ArenaListNode parent,
  const ArenaListNode child);
static  ArenaListNode putNP_ArenaListNode_ArenaListNode_ArenaListNode
 (const int   next,
  const ArenaListNode sibling,
  const ArenaListNode child);
static void setUp_ArenaListNode_ArenaListNode
 (const ArenaListNode child,
  const ArenaListNode parent);
static  ArenaListNode putNext_ArenaListNode_ArenaListNode_ArenaListNode
 (const ArenaListNode sibling,
  const ArenaListNode child);
static  ArenaListNode putPrev_ArenaListNode_ArenaListNode_ArenaListNode
 (const ArenaListNode sibling,
  const ArenaListNode child);
static  void replace__ArenaListNode_ArenaListNode
 (const ArenaListNode with,
  const ArenaListNode over);
static void by_ArenaListNode_sub
 (ArenaListNode node,
  void (* const function) (const ArenaListNode node));
static void by_ArenaList_sub
 (const ArenaList list,
  void (* const function) (const ArenaListNode node));
static void scan_ArenaListNode_sub
 (ArenaListNode node,
  void (* const function) (ArenaListNode node, int start, int depth));
static void scan_ArenaList_sub
 (const ArenaList list,
  void (* const function) (ArenaListNode node, int start, int depth));
static  size_t countChildren_size_ArenaList
 (const ArenaList list);
static  size_t countChildren_size_ArenaListNode
 (const ArenaListNode parent);
static  size_t count_size_ArenaListNode
 (const ArenaListNode node);
static  size_t count_size_ArenaList
 (const ArenaList list);
static ArenaList copyAndCompact_ArenaList_ArenaList
 (const ArenaList source);
static void printWithBrackets_ArenaListNode_function
 (const ArenaListNode   node,
  void (*printer)(char * string, size_t length));
static void printWithBrackets_string_ArenaList
 (const ArenaList list,
  void (*printer)(char * string, size_t length));
static int printsWithBracketsAs_int_ArenaListNode_string
 (const ArenaListNode        node,
  const char * const expected);
static void dumpWithBrackets_ArenaList
 (const ArenaList list);
static int printsWithBracketsAs_int_ArenaList_string
 (const ArenaList            list,
  const char * const expected);
static void dump_ArenaList
 (const ArenaList list);
static void dump_ArenaListNode
 (const ArenaListNode node);
static void print_ArenaListNode_function
 (const ArenaListNode   node,
  void (*printer)(char * string, size_t length));
static int printsAs_int_ArenaListNode_string
 (const ArenaListNode        node,
  const char * const expected);
static int printsAs_int_ArenaList_string
 (const ArenaList            list,
  const char * const expected);
static int printContains_ArenaListNode
 (const ArenaListNode   node,
  const char *  expected);
static int printContains_ArenaList
 (const ArenaList       list,
  const char *  expected);
static  ArenaListNode cut_ArenaListNode_ArenaListNode
 (const ArenaListNode child);
static  ArenaListNode unwrap_ArenaListNode_ArenaListNode
 (const ArenaListNode parent);
static  ArenaListNode wrap_ArenaListNode_string
 (const ArenaListNode   child,
  const char * const key);
static void write_void_ArenaList_string
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
{allocate_offset_ArenaList_size, by_ArenaList_sub, check_string_ArenaList, copyAndCompact_ArenaList_ArenaList, countChildren_size_ArenaList, count_size_ArenaList, dumpWithBrackets_ArenaList, dump_ArenaList, findFirstChild_ArenaList_string, findFirst_ArenaList_string, first_ArenaListNode_ArenaList, free_ArenaList, fromLetters_ArenaList_string, last_ArenaListNode_ArenaList, next_ArenaListNode_ArenaList, nodeFromOffset_ArenaList_size, node_ArenaListNode_ArenaList_string_size, pointer_ArenaList_size, prev_ArenaListNode_ArenaList, printContains_ArenaList, printWithBrackets_string_ArenaList, printsAs_int_ArenaList_string, printsWithBracketsAs_int_ArenaList_string, root_ArenaListNode_ArenaList, scan_ArenaList_sub, swap_ArenaList_ArenaList, used_ArenaList, width_size_ArenaList, write_void_ArenaList_string};
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
  void *  (*key)(                                                               // Get a temporary pointer to the key of a node.
    const ArenaListNode node);                                                  // ArenaListNode
  ArenaListNode  (*last)(                                                       // Get the last child under a parent.
    const ArenaListNode parent);                                                // Parent
  size_t  (*length)(                                                            // Get the length of the key associated with a node
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
  void  (*print)(                                                               // Apply a function to the print of a ArenaList.
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
{by_ArenaListNode_sub, content_ArenaListNode, context_ArenaListNode, copyData_ArenaListNode_ArenaListNode, copy_ArenaList_ArenaListNode, countChildren_size_ArenaListNode, count_size_ArenaListNode, cut_ArenaListNode_ArenaListNode, data_pointer_ArenaListNode, dump_ArenaListNode, equalsString_ArenaListNode_string, equals_int_ArenaListNode_ArenaListNode, findFirstChild_ArenaListNode_string, findFirst_ArenaListNode_string, first_ArenaListNode_ArenaListNode, free_ArenaListNode, getData_ArenaListNode_pointer, isEmpty_ArenaListNode, isFirst_ArenaListNode, isLast_ArenaListNode, isOnlyChild_ArenaListNode, isRoot_ArenaListNode, keyEquals_int_ArenaListNode_pointer_size, key_pointer_ArenaListNode, last_ArenaListNode_ArenaListNode, length_size_ArenaListNode, next_ArenaListNode_ArenaListNode, parent_ArenaListNode_ArenaListNode, prev_ArenaListNode_ArenaListNode, printContains_ArenaListNode, printWithBrackets_ArenaListNode_function, print_ArenaListNode_function, printsAs_int_ArenaListNode_string, printsWithBracketsAs_int_ArenaListNode_string, putFirst_ArenaListNode_ArenaListNode_ArenaListNode, putLast_ArenaListNode_ArenaListNode_ArenaListNode, putNext_ArenaListNode_ArenaListNode_ArenaListNode, putPrev_ArenaListNode_ArenaListNode_ArenaListNode, putTreeFirst_ArenaListNode_ArenaListNode, putTreeLast_ArenaListNode_ArenaListNode, replace__ArenaListNode_ArenaListNode, root_ArenaListNodeOffset_ArenaListNodeOffset, scan_ArenaListNode_sub, setData_ArenaListNode_pointer, setKey_ArenaListNode_string_size, setUp_ArenaListNode_ArenaListNode, unwrap_ArenaListNode_ArenaListNode, valid_ArenaListNode, width_size_ArenaListNode, wrap_ArenaListNode_string};
ArenaListNode newArenaListNode(ArenaListNode allocator) {return allocator;}

