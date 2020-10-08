static ArenaListEditable makeArenaListEditableWithWidth
 (const size_t width);
static ArenaListEditable makeArenaListEditable
 ();
static void swap_ArenaListEditable_ArenaListEditable
 (ArenaListEditable source,
  ArenaListEditable target);
static char * check_string_ArenaListEditable
 (const ArenaListEditable list);
static void * pointer_ArenaListEditable_size
 (const ArenaListEditable      list,
  const size_t offset);
static ArenaListEditableContent * content_ArenaListEditableNode
 (const ArenaListEditableNode node);
static size_t width_size_ArenaListEditable
 (const ArenaListEditable list);
static size_t width_size_ArenaListEditableNode
 (const ArenaListEditableNode node);
static void * key_pointer_ArenaListEditableNode
 (const ArenaListEditableNode node);
static void * data_pointer_ArenaListEditableNode
 (const ArenaListEditableNode node);
static size_t length_size_ArenaListEditableNode
 (const ArenaListEditableNode node);
static  ArenaListEditableNode  nodeFromOffset_ArenaListEditable_size
 (const ArenaListEditable      list,
  const size_t delta);
static int keyEquals_int_ArenaListEditableNode_pointer_size
 (const ArenaListEditableNode        node,
  const char * const key,
  const size_t       length);
static int equals_int_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode a,
  const ArenaListEditableNode b);
static  ArenaListEditableNode root_ArenaListEditableNode_ArenaListEditable
 (const ArenaListEditable list);
static  ArenaListEditableNode root_ArenaListEditableNodeOffset_ArenaListEditableNodeOffset
 (const ArenaListEditableNode node);
static  ArenaListEditableNode  allocate_offset_ArenaListEditable_size
 (const ArenaListEditable      list,
  const size_t size);
static size_t used_ArenaListEditable
 (const ArenaListEditable list);
static ArenaListEditableNode node_ArenaListEditableNode_ArenaListEditable_string_size
 (const ArenaListEditable            list,
  const void * const key,
  const size_t       length);
static void setKey_ArenaListEditableNode_string_size
 (const ArenaListEditableNode        node,
  const void * const key,
  const size_t       length);
static void setData_ArenaListEditableNode_pointer
 (const ArenaListEditableNode        node,
  const void * const data);
static void getData_ArenaListEditableNode_pointer
 (const ArenaListEditableNode  node,
   void * const data);
static void copyData_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode target,
  const ArenaListEditableNode source);
static ArenaListEditableNode  copy_ArenaListEditable_ArenaListEditableNode
 (const ArenaListEditableNode source,
  const ArenaListEditable     target);
static void fromLetters_ArenaListEditable_string
 (const ArenaListEditable      list,
  const char * str);
static void free_ArenaListEditable
 (const ArenaListEditable list);
static void free_ArenaListEditableNode
 (ArenaListEditableNode node);
static int valid_ArenaListEditableNode
 (const ArenaListEditableNode child);
static  ArenaListEditableNode parent_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode child);
static  ArenaListEditableNode first_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode parent);
static  ArenaListEditableNode last_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode parent);
static  ArenaListEditableNode next_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode parent);
static  ArenaListEditableNode prev_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode parent);
static  ArenaListEditableNode first_ArenaListEditableNode_ArenaListEditable
 (const ArenaListEditable list);
static  ArenaListEditableNode last_ArenaListEditableNode_ArenaListEditable
 (const ArenaListEditable list);
static  ArenaListEditableNode next_ArenaListEditableNode_ArenaListEditable
 (const ArenaListEditable list);
static  ArenaListEditableNode prev_ArenaListEditableNode_ArenaListEditable
 (const ArenaListEditable list);
static int equalsString_ArenaListEditableNode_string
 (const ArenaListEditableNode        node,
  const char * const key);
static  ArenaListEditableNode findFirst_ArenaListEditableNode_string
 (const ArenaListEditableNode        node,
  const char * const key);
static  ArenaListEditableNode findFirst_ArenaListEditable_string
 (const ArenaListEditable            list,
  const char * const key);
static  ArenaListEditableNode findFirstChild_ArenaListEditableNode_string
 (const ArenaListEditableNode        parent,
  const char * const key);
static  ArenaListEditableNode findFirstChild_ArenaListEditable_string
 (const ArenaListEditable            list,
  const char * const key);
static int context_ArenaListEditableNode
 (const ArenaListEditableNode         child,
        ArenaListEditableNode * const parent,
  const char  * const key);
static int isFirst_ArenaListEditableNode
 (const ArenaListEditableNode child);
static int isLast_ArenaListEditableNode
 (const ArenaListEditableNode child);
static int isEmpty_ArenaListEditableNode
 (const ArenaListEditableNode node);
static int isOnlyChild_ArenaListEditableNode
 (const ArenaListEditableNode child);
static int isRoot_ArenaListEditableNode
 (const ArenaListEditableNode node);
static  ArenaListEditableNode putFL_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode
 (const int   first,
  const ArenaListEditableNode parent,
  const ArenaListEditableNode child);
static  ArenaListEditableNode putTreeFirst_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode child);
static  ArenaListEditableNode putTreeLast_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode child);
static  ArenaListEditableNode putFirst_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode parent,
  const ArenaListEditableNode child);
static  ArenaListEditableNode putLast_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode parent,
  const ArenaListEditableNode child);
static  ArenaListEditableNode putNP_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode
 (const int   next,
  const ArenaListEditableNode sibling,
  const ArenaListEditableNode child);
static void setUp_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode child,
  const ArenaListEditableNode parent);
static  ArenaListEditableNode putNext_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode sibling,
  const ArenaListEditableNode child);
static  ArenaListEditableNode putPrev_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode sibling,
  const ArenaListEditableNode child);
static  void replace__ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode with,
  const ArenaListEditableNode over);
static void by_ArenaListEditableNode_sub
 (ArenaListEditableNode node,
  void (* const function) (const ArenaListEditableNode node));
static void by_ArenaListEditable_sub
 (const ArenaListEditable list,
  void (* const function) (const ArenaListEditableNode node));
static void scan_ArenaListEditableNode_sub
 (ArenaListEditableNode node,
  void (* const function) (ArenaListEditableNode node, int start, int depth));
static void scan_ArenaListEditable_sub
 (const ArenaListEditable list,
  void (* const function) (ArenaListEditableNode node, int start, int depth));
static  size_t countChildren_size_ArenaListEditable
 (const ArenaListEditable list);
static  size_t countChildren_size_ArenaListEditableNode
 (const ArenaListEditableNode parent);
static  size_t count_size_ArenaListEditableNode
 (const ArenaListEditableNode node);
static  size_t count_size_ArenaListEditable
 (const ArenaListEditable list);
static ArenaListEditable copyAndCompact_ArenaListEditable_ArenaListEditable
 (const ArenaListEditable source);
static void printWithBrackets_ArenaListEditableNode_function
 (const ArenaListEditableNode   node,
  void (*printer)(char * string, size_t length));
static void printWithBrackets_string_ArenaListEditable
 (const ArenaListEditable list,
  void (*printer)(char * string, size_t length));
static int printsWithBracketsAs_int_ArenaListEditableNode_string
 (const ArenaListEditableNode        node,
  const char * const expected);
static void dumpWithBrackets_ArenaListEditable
 (const ArenaListEditable list);
static int printsWithBracketsAs_int_ArenaListEditable_string
 (const ArenaListEditable            list,
  const char * const expected);
static void dump_ArenaListEditable
 (const ArenaListEditable list);
static void dump_ArenaListEditableNode
 (const ArenaListEditableNode node);
static void print_ArenaListEditableNode_function
 (const ArenaListEditableNode   node,
  void (*printer)(char * string, size_t length));
static int printsAs_int_ArenaListEditableNode_string
 (const ArenaListEditableNode        node,
  const char * const expected);
static int printsAs_int_ArenaListEditable_string
 (const ArenaListEditable            list,
  const char * const expected);
static int printContains_ArenaListEditableNode
 (const ArenaListEditableNode   node,
  const char *  expected);
static int printContains_ArenaListEditable
 (const ArenaListEditable       list,
  const char *  expected);
static  ArenaListEditableNode cut_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode child);
static  ArenaListEditableNode unwrap_ArenaListEditableNode_ArenaListEditableNode
 (const ArenaListEditableNode parent);
static  ArenaListEditableNode wrap_ArenaListEditableNode_string
 (const ArenaListEditableNode   child,
  const char * const key);
static void write_void_ArenaListEditable_string
 (const ArenaListEditable       list,
  const char * const file);
struct ProtoTypes_ArenaListEditable {
  ArenaListEditableNode   (*allocate)(                                          // Allocate a node within the arena of a ArenaListEditable
    const ArenaListEditable list,                                               // ArenaListEditable in which to allocate
    const size_t size);                                                         // Amount of memory required
  void  (*by)(                                                                  // Traverse a ArenaListEditable in post-order calling the specified function to process each child node.  The ArenaListEditable is buffered allowing changes to be made to the structure of the ArenaListEditable without disruption as long as each child checks its context.
    const ArenaListEditable list,                                               // ArenaListEditable
    void (* const function) (const ArenaListEditableNode node));                // Function
  char *  (*check)(                                                             // Return a string describing the structure
    const ArenaListEditable list);                                              // ArenaListEditable
  ArenaListEditable  (*copyAndCompact)(                                         // Copy a ArenaListEditable compacting any free space.  This method assumes that there are no direct external references to the nodes in the list as this process might change the location of one or more nodes in the arena.
    const ArenaListEditable source);                                            // Source ArenaListEditable
  size_t  (*countChildren)(                                                     // Count the number of children directly under a parent.
    const ArenaListEditable list);                                              // ArenaListEditable
  size_t  (*count)(                                                             // Count the number of nodes in a ArenaListEditable
    const ArenaListEditable list);                                              // ArenaListEditable
  void  (*dumpWithBrackets)(                                                    // Dump a ArenaListEditable printed with brackets to stderr
    const ArenaListEditable list);                                              // ArenaListEditable
  void  (*dump)(                                                                // Dump a ArenaListEditable on stderr
    const ArenaListEditable list);                                              // ArenaListEditable
  ArenaListEditableNode  (*findFirstChild)(                                     // Find the first child immediately under the root with the specified key.
    const ArenaListEditable list,                                               // ArenaListEditable
    const char * const key);                                                    // Key to find
  ArenaListEditableNode  (*findFirst)(                                          // Find the first node with the specified key in a post-order traversal of the ArenaListEditable.
    const ArenaListEditable list,                                               // ArenaListEditable
    const char * const key);                                                    // Key to find
  ArenaListEditableNode  (*first)(                                              // Get the first child in the specified ArenaListEditable.
    const ArenaListEditable list);                                              // Parent
  void  (*free)(                                                                // Free an entire ArenaListEditable.
    const ArenaListEditable list);                                              // ArenaListEditable to free
  void  (*fromLetters)(                                                         // Load ArenaListEditable from a string of letters and brackets.  The root node of the ArenaListEditable so constructed is always given the letter 'a'.
    const ArenaListEditable list,                                               // ArenaListEditable
    const char * str);                                                          // String of letters and brackets describing desired list structure
  ArenaListEditableNode  (*last)(                                               // Get the last child in the specified ArenaListEditable.
    const ArenaListEditable list);                                              // Parent
  ArenaListEditableNode  (*next)(                                               // Get the next child in the specified ArenaListEditable.
    const ArenaListEditable list);                                              // Parent
  ArenaListEditableNode   (*nodeFromOffset)(                                    // Create a node to locate an allocation within the arena of a ArenaListEditable.
    const ArenaListEditable list,                                               // ArenaListEditable
    const size_t delta);                                                        // Delta within arena. A delta of zero represents no such node.
  ArenaListEditableNode  (*node)(                                               // Create a new ArenaListEditable node with the specified key.
    const ArenaListEditable list,                                               // ArenaListEditable in which to create the node
    const void * const key,                                                     // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the ArenaListEditable is determined solely by the user.
    const size_t length);                                                       // Length of the key.
  void *  (*pointer)(                                                           // Return a temporary pointer to data in the arena of the specified ArenaListEditable
    const ArenaListEditable list,                                               // ArenaListEditable
    const size_t offset);                                                       // Offset
  ArenaListEditableNode  (*prev)(                                               // Get the prev child in the specified ArenaListEditable.
    const ArenaListEditable list);                                              // Parent
  int  (*printContains)(                                                        // Check the print of an ArenaListEditable contains the expected string.
    const ArenaListEditable list,                                               // ArenaListEditable parse ArenaListEditable
    const char * expected);                                                     // Expected string
  void  (*printWithBrackets)(                                                   // Print an entire ArenaListEditable in preorder as a string with brackets around each sub ArenaListEditable
    const ArenaListEditable list,                                               // ArenaListEditable
    void (*printer) (char * string, size_t length));                            // Function to apply to printed ArenaListEditable
  int  (*printsAs)(                                                             // Check that the specified ArenaListEditable prints as expected.
    const ArenaListEditable list,                                               // ArenaListEditable
    const char * const expected);                                               // Expected string when printed
  int  (*printsWithBracketsAs)(                                                 // Check that the specified ArenaListEditable prints with brackets as expected.
    const ArenaListEditable list,                                               // ArenaListEditable
    const char * const expected);                                               // Expected string when printed
  ArenaListEditableNode  (*root)(                                               // Return the root node of a ArenaListEditable.
    const ArenaListEditable list);                                              // ArenaListEditable
  void  (*scan)(                                                                // Traverse a ArenaListEditable calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The ArenaListEditable is buffered allowing changes to be made to the structure of the ArenaListEditable without disruption as long as each child checks its context.
    const ArenaListEditable list,                                               // ArenaListEditable
    void (* const function) (ArenaListEditableNode node, int start, int depth));                                                                               // Function: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
  void  (*swap)(                                                                // Swap the arenas of two ArenaListEditable
    ArenaListEditable source,                                                   // Source ArenaListEditable
    ArenaListEditable target);                                                  // Target ArenaListEditable
  size_t  (*used)(                                                              // Amount of space currently being used within the arena of a ArenaListEditable.
    const ArenaListEditable list);                                              // ArenaListEditable
  size_t  (*width)(                                                             // Get the width of the data area for a ArenaListEditable
    const ArenaListEditable list);                                              // ArenaListEditableNode
  void  (*write)(                                                               // Write a ArenaListEditable to a named file or abort
    const ArenaListEditable list,                                               // ArenaListEditable
    const char * const file);                                                   // File
 } const ProtoTypes_ArenaListEditable =
{allocate_offset_ArenaListEditable_size, by_ArenaListEditable_sub, check_string_ArenaListEditable, copyAndCompact_ArenaListEditable_ArenaListEditable, countChildren_size_ArenaListEditable, count_size_ArenaListEditable, dumpWithBrackets_ArenaListEditable, dump_ArenaListEditable, findFirstChild_ArenaListEditable_string, findFirst_ArenaListEditable_string, first_ArenaListEditableNode_ArenaListEditable, free_ArenaListEditable, fromLetters_ArenaListEditable_string, last_ArenaListEditableNode_ArenaListEditable, next_ArenaListEditableNode_ArenaListEditable, nodeFromOffset_ArenaListEditable_size, node_ArenaListEditableNode_ArenaListEditable_string_size, pointer_ArenaListEditable_size, prev_ArenaListEditableNode_ArenaListEditable, printContains_ArenaListEditable, printWithBrackets_string_ArenaListEditable, printsAs_int_ArenaListEditable_string, printsWithBracketsAs_int_ArenaListEditable_string, root_ArenaListEditableNode_ArenaListEditable, scan_ArenaListEditable_sub, swap_ArenaListEditable_ArenaListEditable, used_ArenaListEditable, width_size_ArenaListEditable, write_void_ArenaListEditable_string};
ArenaListEditable newArenaListEditable(ArenaListEditable allocator) {return allocator;}

struct ProtoTypes_ArenaListEditableArena {
 } const ProtoTypes_ArenaListEditableArena =
{};
ArenaListEditableArena newArenaListEditableArena(ArenaListEditableArena allocator) {return allocator;}

struct ProtoTypes_ArenaListEditableContent {
 } const ProtoTypes_ArenaListEditableContent =
{};
ArenaListEditableContent newArenaListEditableContent(ArenaListEditableContent allocator) {return allocator;}

struct ProtoTypes_ArenaListEditableDescription {
 } const ProtoTypes_ArenaListEditableDescription =
{};
ArenaListEditableDescription newArenaListEditableDescription(ArenaListEditableDescription allocator) {return allocator;}

struct ProtoTypes_ArenaListEditableNode {
  void  (*by)(                                                                  // Traverse the ArenaListEditable rooted at the specified node in post-order calling the specified function to process each child node.  The ArenaListEditable is buffered allowing changes to be made to the structure of the ArenaListEditable without disruption as long as each child checks its context.
    ArenaListEditableNode node,                                                 // ArenaListEditableNode
    void (* const function) (const ArenaListEditableNode node));                // Function
  ArenaListEditableContent *  (*content)(                                       // Convert a node offset to an address so that the content of a node can be updated in situ as long as the ArenaListEditable is not reallocated to a different position.
    const ArenaListEditableNode node);                                          // ArenaListEditableNode
  int  (*context)(                                                              // Return true if the parent of the specified child has the specified key.
    const ArenaListEditableNode child,                                          // Child
    ArenaListEditableNode * const parent,                                       // Parent container
    const char  * const key);                                                   // Key
  void  (*copyData)(                                                            // Copy the data from the source node to the target node.
    const ArenaListEditableNode target,                                         // Target ArenaListEditableNode
    const ArenaListEditableNode source);                                        // Source ArenaListEditableNode
  ArenaListEditableNode   (*copy)(                                              // Copy a node from one ArenaListEditable to another ArenaListEditable
    const ArenaListEditableNode source,                                         // Source ArenaListEditableNode
    const ArenaListEditable target);                                            // Target ArenaListEditable
  size_t  (*countChildren)(                                                     // Count the number of children directly under a node.
    const ArenaListEditableNode parent);                                        // Parent
  size_t  (*count)(                                                             // Count the number of nodes under a specified node but not including the specified node.
    const ArenaListEditableNode node);                                          // ArenaListEditableNode
  ArenaListEditableNode  (*cut)(                                                // Cut out a child.
    const ArenaListEditableNode child);                                         // Child to cut out
  void *  (*data)(                                                              // Get a temporary pointer to the user data of a node.
    const ArenaListEditableNode node);                                          // ArenaListEditableNode
  void  (*dump)(                                                                // Dump a ArenaListEditableNode on stderr
    const ArenaListEditableNode node);                                          // ArenaListEditableNode
  int  (*equalsString)(                                                         // Check that the key of a node equals a string
    const ArenaListEditableNode node,                                           // ArenaListEditableNode
    const char * const key);                                                    // Key
  int  (*equals)(                                                               // Confirm two nodes are equal
    const ArenaListEditableNode a,                                              // First offset
    const ArenaListEditableNode b);                                             // Second offset
  ArenaListEditableNode  (*findFirstChild)(                                     // Find the first child of the specified parent with the specified key.
    const ArenaListEditableNode parent,                                         // Parent node
    const char * const key);                                                    // Key to find immediately under parent
  ArenaListEditableNode  (*findFirst)(                                          // Find the first node with the specified key in a post-order traversal of the ArenaListEditable starting at the specified node.
    const ArenaListEditableNode node,                                           // ArenaListEditableNode at the start of the list to be searched
    const char * const key);                                                    // Key to find
  ArenaListEditableNode  (*first)(                                              // Get the first child under a parent.
    const ArenaListEditableNode parent);                                        // Parent
  void  (*free)(                                                                // Free a node. If the ArenaListEditable is editable the node is made available for reuse otherwise the node wastes space. A new copy of the ArenaListEditable without wasted space can be made with copyAndCompact_ArenaListEditable .
    ArenaListEditableNode node);                                                // ArenaListEditableNode to free
  void  (*getData)(                                                             // Get the optional user data associated with a node in an ArenaListEditable
    const ArenaListEditableNode node,                                           // ArenaListEditableNode
    void * const data);                                                         // Data
  int  (*isEmpty)(                                                              // Confirm a node has no children.
    const ArenaListEditableNode node);                                          // ArenaListEditableNode
  int  (*isFirst)(                                                              // Confirm a child is first under its parent
    const ArenaListEditableNode child);                                         // Child
  int  (*isLast)(                                                               // Confirm a child is last under its parent
    const ArenaListEditableNode child);                                         // Child
  int  (*isOnlyChild)(                                                          // Confirm that this child is the only child of its parent
    const ArenaListEditableNode child);                                         // Child
  int  (*isRoot)(                                                               // Confirm a node is the root
    const ArenaListEditableNode node);                                          // ArenaListEditableNode
  int  (*keyEquals)(                                                            // Confirm that the key of a node is equal to the specified memory
    const ArenaListEditableNode node,                                           // ArenaListEditableNode whose key is to be checked
    const char * const key,                                                     // Expected key value
    const size_t length);                                                       // Length of expected key value
  void *  (*key)(                                                               // Get a temporary pointer to the key of a node.
    const ArenaListEditableNode node);                                          // ArenaListEditableNode
  ArenaListEditableNode  (*last)(                                               // Get the last child under a parent.
    const ArenaListEditableNode parent);                                        // Parent
  size_t  (*length)(                                                            // Get the length of the key associated with a node
    const ArenaListEditableNode node);                                          // ArenaListEditableNode
  ArenaListEditableNode  (*next)(                                               // Get the next child under a parent.
    const ArenaListEditableNode parent);                                        // Parent
  ArenaListEditableNode  (*parent)(                                             // Get the parent of a child
    const ArenaListEditableNode child);                                         // Child
  ArenaListEditableNode  (*prev)(                                               // Get the prev child under a parent.
    const ArenaListEditableNode parent);                                        // Parent
  int  (*printContains)(                                                        // Check the print of an ArenaListEditable starting at the specified tag contains the expected string.
    const ArenaListEditableNode node,                                           // Starting node
    const char * expected);                                                     // Expected string
  void  (*printWithBrackets)(                                                   // Apply a function to a string containing a print of the specified  node and the ArenaListEditable below it in preorder as a string with each sub ArenaListEditable enclosed in brackets.
    const ArenaListEditableNode node,                                           // ArenaListEditableNode
    void (*printer) (char * string, size_t length));                            // Function to apply to printed ArenaListEditable
  void  (*print)(                                                               // Apply a function to the print of a ArenaListEditable.
    const ArenaListEditableNode node,                                           // ArenaListEditableNode
    void (*printer) (char * string, size_t length));                            // Function to apply to printed ArenaListEditable
  int  (*printsAs)(                                                             // Check that the specified node prints as expected.
    const ArenaListEditableNode node,                                           // ArenaListEditable
    const char * const expected);                                               // Expected string when printed
  int  (*printsWithBracketsAs)(                                                 // Check that the ArenaListEditable starting at the specified node prints with brackets as expected.
    const ArenaListEditableNode node,                                           // ArenaListEditableNode
    const char * const expected);                                               // Expected string when printed
  ArenaListEditableNode  (*putFirst)(                                           // Put a child first under its parent
    const ArenaListEditableNode parent,                                         // Parent
    const ArenaListEditableNode child);                                         // Child
  ArenaListEditableNode  (*putLast)(                                            // Put a child last under its parent
    const ArenaListEditableNode parent,                                         // Parent
    const ArenaListEditableNode child);                                         // Child
  ArenaListEditableNode  (*putNext)(                                            // Put a child next after the specified sibling
    const ArenaListEditableNode sibling,                                        // Sibling
    const ArenaListEditableNode child);                                         // Child
  ArenaListEditableNode  (*putPrev)(                                            // Put a child previous after the specified sibling
    const ArenaListEditableNode sibling,                                        // Sibling
    const ArenaListEditableNode child);                                         // Child
  ArenaListEditableNode  (*putTreeFirst)(                                       // Put a child first in the ArenaListEditable containing the arena in which the child was allocated.
    const ArenaListEditableNode child);                                         // Child
  ArenaListEditableNode  (*putTreeLast)(                                        // Put a child last in the ArenaListEditable containing the arena in which the child was allocated.
    const ArenaListEditableNode child);                                         // Child
  void  (*replace)(                                                             // Replace the specified node with this node
    const ArenaListEditableNode with,                                           // Replace with this node
    const ArenaListEditableNode over);                                          // Node to be replaced
  ArenaListEditableNode  (*root)(                                               // Return the root node of the ArenaListEditable containing the specified node.
    const ArenaListEditableNode node);                                          // ArenaListEditableNode
  void  (*scan)(                                                                // Traverse the ArenaListEditable rooted at the specified node calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The ArenaListEditable is buffered allowing changes to be made to the structure of the ArenaListEditable without disruption as long as each child checks its context.
    ArenaListEditableNode node,                                                 // ArenaListEditableNode
    void (* const function) (ArenaListEditableNode node, int start, int depth));                                                                               // Function: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
  void  (*setData)(                                                             // Set the optional user data associated with a node in an ArenaListEditable
    const ArenaListEditableNode node,                                           // ArenaListEditableNode
    const void * const data);                                                   // Data
  void  (*setKey)(                                                              // Set the key of a node to a different value as long as there is enough room from the last setting.
    const ArenaListEditableNode node,                                           // ArenaListEditableNode
    const void * const key,                                                     // Key
    const size_t length);                                                       // Length of key
  void  (*setUp)(                                                               // Make the specified parent node the parent of the specified child node
    const ArenaListEditableNode child,                                          // Child
    const ArenaListEditableNode parent);                                        // Child
  ArenaListEditableNode  (*unwrap)(                                             // Unwrap the specified parent and return it.
    const ArenaListEditableNode parent);                                        // Parent to unwrap
  int  (*valid)(                                                                // Check that a node is valid.
    const ArenaListEditableNode child);                                         // ArenaListEditableNode
  size_t  (*width)(                                                             // Get the width of the data area for a node
    const ArenaListEditableNode node);                                          // ArenaListEditableNode
  ArenaListEditableNode  (*wrap)(                                               // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
    const ArenaListEditableNode child,                                          // Child to wrap
    const char * const key);                                                    // Key for new parent
 } const ProtoTypes_ArenaListEditableNode =
{by_ArenaListEditableNode_sub, content_ArenaListEditableNode, context_ArenaListEditableNode, copyData_ArenaListEditableNode_ArenaListEditableNode, copy_ArenaListEditable_ArenaListEditableNode, countChildren_size_ArenaListEditableNode, count_size_ArenaListEditableNode, cut_ArenaListEditableNode_ArenaListEditableNode, data_pointer_ArenaListEditableNode, dump_ArenaListEditableNode, equalsString_ArenaListEditableNode_string, equals_int_ArenaListEditableNode_ArenaListEditableNode, findFirstChild_ArenaListEditableNode_string, findFirst_ArenaListEditableNode_string, first_ArenaListEditableNode_ArenaListEditableNode, free_ArenaListEditableNode, getData_ArenaListEditableNode_pointer, isEmpty_ArenaListEditableNode, isFirst_ArenaListEditableNode, isLast_ArenaListEditableNode, isOnlyChild_ArenaListEditableNode, isRoot_ArenaListEditableNode, keyEquals_int_ArenaListEditableNode_pointer_size, key_pointer_ArenaListEditableNode, last_ArenaListEditableNode_ArenaListEditableNode, length_size_ArenaListEditableNode, next_ArenaListEditableNode_ArenaListEditableNode, parent_ArenaListEditableNode_ArenaListEditableNode, prev_ArenaListEditableNode_ArenaListEditableNode, printContains_ArenaListEditableNode, printWithBrackets_ArenaListEditableNode_function, print_ArenaListEditableNode_function, printsAs_int_ArenaListEditableNode_string, printsWithBracketsAs_int_ArenaListEditableNode_string, putFirst_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode, putLast_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode, putNext_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode, putPrev_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode, putTreeFirst_ArenaListEditableNode_ArenaListEditableNode, putTreeLast_ArenaListEditableNode_ArenaListEditableNode, replace__ArenaListEditableNode_ArenaListEditableNode, root_ArenaListEditableNodeOffset_ArenaListEditableNodeOffset, scan_ArenaListEditableNode_sub, setData_ArenaListEditableNode_pointer, setKey_ArenaListEditableNode_string_size, setUp_ArenaListEditableNode_ArenaListEditableNode, unwrap_ArenaListEditableNode_ArenaListEditableNode, valid_ArenaListEditableNode, width_size_ArenaListEditableNode, wrap_ArenaListEditableNode_string};
ArenaListEditableNode newArenaListEditableNode(ArenaListEditableNode allocator) {return allocator;}

