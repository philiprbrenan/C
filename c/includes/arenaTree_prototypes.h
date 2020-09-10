static ArenaTree makeArenaTree
 ();
static char * check_ArenaTree
 (const ArenaTree tree);
static void * pointer_ArenaTree_size
 (const ArenaTree      tree,
  const size_t delta);
static void * pointer_ArenaTreeOffset
 (const ArenaTreeOffset o);
static ArenaTreeOffset offsetTo_ArenaTree_pointer
 (const ArenaTree            tree,
  const void * const pointer);
static ArenaTreeContent * content_ArenaTreeNode
 (const ArenaTreeNode n);
static int checkKey_int_ArenaTreeNode_string_size
 (const ArenaTreeNode        node,
  const char * const key,
  const size_t       length);
static int equals_int_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode a,
  const ArenaTreeNode b);
static ArenaTreeNode root_ArenaTreeNodeOffset_ArenaTree
 (const ArenaTree tree);
static ArenaTreeNode root_ArenaTreeNodeOffset_ArenaTreeNodeOffset
 (const ArenaTreeNode node);
static ArenaTreeString key_string_ArenaTreeNode
 (const ArenaTreeNode node);
static ArenaTreeNode setKey_ArenaTreeNode_ArenaTreeNode_ArenaTreeString
 (const ArenaTreeNode        node,
  const char * const key);
static size_t used_ArenaTree
 (const ArenaTree tree);
static ArenaTreeOffset  offset_ArenaTree_size
 (const ArenaTree       tree,
  const size_t  delta);
static ArenaTreeNode   nodeFromOffset_ArenaTree_size
 (const ArenaTree      tree,
  const size_t delta);
static ArenaTreeOffset allocate_offset_ArenaTree_size
 (const ArenaTree      tree,
  const size_t size);
static ArenaTreeNode noden_ArenaTreeNode_ArenaTree_ArenaTreeString
 (const ArenaTree       tree,
  const char * const key,
  const size_t  length);
static ArenaTreeNode node_ArenaTreeNode_ArenaTree_ArenaTreeString
 (const ArenaTree             tree,
  const char * const  key);
static ArenaTreeOffset saveString_ArenaTreeOffset_ArenaTree_ArenaTreeString
 (const ArenaTree       tree,
  const char * const str,
  const size_t  length);
static void get_ArenaTree_data_size
 (const ArenaTree      tree,
  const size_t offset,
  void * const data,
  const size_t length);
static ArenaTreeOffset store_offset_ArenaTree_data_size
 (const ArenaTree       tree,
  const void *  const data,
  const size_t  length);
static void retrieve_ArenaTree_ArenaTreeOffset_data_size
 (const ArenaTree       tree,
  const ArenaTreeOffset offset,
  void * const  data,
  const size_t  length);
static void get_ArenaTreeNode_data_size
 (const ArenaTreeNode  node,
  void * const data,
  const size_t length);
static void set_ArenaTreeNode_data_size
 (const ArenaTreeNode  node,
  const void * const data,
  const size_t length);
static void copyData_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode target,
  const ArenaTreeNode source);
static size_t getData_size_ArenaTreeNode
 (const ArenaTreeNode  node);
static void setData_ArenaTreeNode_size
 (const ArenaTreeNode  node,
  const size_t offset);
static void fromLetters_ArenaTree_ArenaTreeString
 (const ArenaTree tree,
  ArenaTreeString str);
static void free_ArenaTree
 (const ArenaTree tree);
static int valid_ArenaTreeNode
 (const ArenaTreeNode child);
static  ArenaTreeNode parent_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode child);
static  ArenaTreeNode first_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent);
static  ArenaTreeNode last_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent);
static  ArenaTreeNode next_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent);
static  ArenaTreeNode prev_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent);
static int equalsString_ArenaTreeNode_string
 (const ArenaTreeNode        node,
  const char * const key);
static  ArenaTreeNode findFirst_ArenaTreeNode_string
 (const ArenaTreeNode        node,
  const char * const key);
static  ArenaTreeNode findFirst_ArenaTree_string
 (const ArenaTree            tree,
  const char * const key);
static  ArenaTreeNode findFirstChild_ArenaTreeNode_string
 (const ArenaTreeNode        parent,
  const char * const key);
static  ArenaTreeNode findFirstChild_ArenaTree_string
 (const ArenaTree            tree,
  const char * const key);
static int context_ArenaTreeNode
 (const ArenaTreeNode         child,
        ArenaTreeNode * const parent,
  const char  * const key);
static int isFirst_ArenaTreeNode
 (const ArenaTreeNode child);
static int isLast_ArenaTreeNode
 (const ArenaTreeNode child);
static int isEmpty_ArenaTreeNode
 (const ArenaTreeNode node);
static int isOnlyChild_ArenaTreeNode
 (const ArenaTreeNode child);
static int isRoot_ArenaTreeNode
 (const ArenaTreeNode node);
static  ArenaTreeNode putFL_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode
 (const int   first,
  const ArenaTreeNode parent,
  const ArenaTreeNode child);
static  ArenaTreeNode putTreeFirst_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode child);
static  ArenaTreeNode putTreeLast_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode child);
static  ArenaTreeNode putFirst_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent,
  const ArenaTreeNode child);
static  ArenaTreeNode putLast_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent,
  const ArenaTreeNode child);
static  ArenaTreeNode putNP_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode
 (const int   next,
  const ArenaTreeNode sibling,
  const ArenaTreeNode child);
static  ArenaTreeNode putNext_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode sibling,
  const ArenaTreeNode child);
static  ArenaTreeNode putPrev_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode sibling,
  const ArenaTreeNode child);
static void by_ArenaTreeNode_sub
 (ArenaTreeNode node,
  void (* const function) (const ArenaTreeNode node));
static void by_ArenaTree_sub
 (const ArenaTree tree,
  void (* const function) (const ArenaTreeNode node));
static  size_t countChildren_size_ArenaTreeNodeOffset
 (const ArenaTreeNode parent);
static  size_t count_size_ArenaTree
 (const ArenaTree tree);
static  ArenaTreeString printWithBrackets_string_ArenaTreeNode
 (const ArenaTreeNode   node);
static  ArenaTreeString printWithBrackets_ArenaTreeString_ArenaTree
 (const ArenaTree t);
static int printsWithBracketsAs_int_ArenaTreeNode_string
 (const ArenaTreeNode        node,
  const char * const expected);
static int printsWithBracketsAs_int_ArenaTree_string
 (const ArenaTree            tree,
  const char * const expected);
static ReadOnlyBytes print_string_ArenaTreeNode
 (const ArenaTreeNode   node);
static ReadOnlyBytes print_string_ArenaTree
 (const ArenaTree t);
static int printsAs_int_ArenaTreeNode_string
 (const ArenaTreeNode        node,
  const char * const expected);
static int printsAs_int_ArenaTree_string
 (const ArenaTree            tree,
  const char * const expected);
static int printContains_ArenaTreeNode
 (const ArenaTreeNode   node,
  const char *  expected);
static int printContains_ArenaTree
 (const ArenaTree       tree,
  const char *  expected);
static  ArenaTreeNode cut_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode child);
static  ArenaTreeNode unwrap_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent);
static  ArenaTreeNode wrap_ArenaTreeNode_ArenaTreeString
 (const ArenaTreeNode   child,
  const char * const key);
static void write_void_ArenaTree_ArenaTreeString
 (const ArenaTree       tree,
  const char * const file);
struct ProtoTypes_ArenaTree {
  ArenaTreeOffset  (*allocate)(                                                 // Allocate memory within the arena of a tree and clear the allocated memory
    const ArenaTree tree,                                                       // ArenaTree in which to allocate
    const size_t size);                                                         // Amount of memory required
  void  (*by)(                                                                  // Traverse a tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    const ArenaTree tree,                                                       // Tree
    void (* const function) (const ArenaTreeNode node));                        // Function
  char *  (*check)(                                                             // Return a string describing the structure
    const ArenaTree tree);                                                      // Tree
  size_t  (*count)(                                                             // Count the number of nodes in a tree
    const ArenaTree tree);                                                      // Tree
  ArenaTreeNode  (*findFirstChild)(                                             // Find the first child immediately under the root with the specified key.
    const ArenaTree tree,                                                       // Tree
    const char * const key);                                                    // Key to find
  ArenaTreeNode  (*findFirst)(                                                  // Find the first node with the specified key in a post-order traversal of the tree.
    const ArenaTree tree,                                                       // Tree
    const char * const key);                                                    // Key to find
  void  (*free)(                                                                // Free an entire arena tree.
    const ArenaTree tree);                                                      // Arena tree to free
  void  (*fromLetters)(                                                         // Load tree from a string of letters and brackets.  The root node of the tree so constructed is always given the letter 'a'.
    const ArenaTree tree,                                                       // Tree
    ArenaTreeString str);                                                       // String of letters and brackets describing desired tree structure
  void  (*get)(                                                                 // Get a copy of the data at the specified offset in the arena of a tree.
    const ArenaTree tree,                                                       // Arena tree
    const size_t offset,                                                        // Offset to data in arena
    void * const data,                                                          // Pointer to the area into which the data is to be copied
    const size_t length);                                                       // Length of the data to be retrieved
  ArenaTreeNode    (*nodeFromOffset)(                                           // Create a node to locate an allocation within the arena of a tree.
    const ArenaTree tree,                                                       // Tree
    const size_t delta);                                                        // Delta within arena. A delta of zero represents no such node.
  ArenaTreeNode  (*node)(                                                       // Create a new tree node keyed by a zero terminated string.
    const ArenaTree tree,                                                       // Arena tree in which to create the node
    const char * const key);                                                    // Key for this node.  Note: we do not order nodes automatically.
  ArenaTreeNode  (*noden)(                                                      // Create a new tree node keyed by a string of the specified length to which a terminating zero will be appended.
    const ArenaTree tree,                                                       // Arena tree in which to create the node
    const char * const key,                                                     // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the tree is determined solely by the user.
    const size_t length);                                                       // Length of the key, or if zero, I will use strlen
  ArenaTreeOffset  (*offsetTo)(                                                 // Create an offset from a pointer into the arena of a tree.
    const ArenaTree tree,                                                       // Offset
    const void * const pointer);                                                // Pointer into arena
  ArenaTreeOffset   (*offset)(                                                  // Create an offset to locate an item within the tree.
    const ArenaTree tree,                                                       // Tree
    const size_t delta);                                                        // Delta within arena
  void *  (*pointer)(                                                           // Return a temporary pointer to an offset in a tree.
    const ArenaTree tree,                                                       // Tree
    const size_t delta);                                                        // Delta
  int  (*printContains)(                                                        // Check the print of an ArenaTree contains the expected string.
    const ArenaTree tree,                                                       // ArenaTree parse tree
    const char * expected);                                                     // Expected string
  ArenaTreeString  (*printWithBrackets)(                                        // Print an entire tree in preorder as a string with brackets around each sub tree
    const ArenaTree t);                                                         // Tree
  ReadOnlyBytes  (*print)(                                                      // Print an entire tree in preorder as a string.
    const ArenaTree t);                                                         // ArenaTree
  int  (*printsAs)(                                                             // Check that the specified ArenaTree prints as expected.
    const ArenaTree tree,                                                       // ArenaTree
    const char * const expected);                                               // Expected string when printed
  int  (*printsWithBracketsAs)(                                                 // Check that the specified ArenaTree prints with brackets as expected.
    const ArenaTree tree,                                                       // ArenaTree
    const char * const expected);                                               // Expected string when printed
  void  (*retrieve)(                                                            // Retrieve a copy of the data stored at the specified offset in the arena of the specified tree.
    const ArenaTree tree,                                                       // Tree in whose arena the data is stored.
    const ArenaTreeOffset offset,                                               // Offset in the arena at which the data to be retrieved is stored.
    void * const data,                                                          // Area into which the retrieved data is to be copied.
    const size_t length);                                                       // Length of the data to be retrieved
  ArenaTreeNode  (*root)(                                                       // Return the root node of a tree.
    const ArenaTree tree);                                                      // Tree
  ArenaTreeOffset  (*saveString)(                                               // Save a copy of a zero terminated string in a tree and return the offset of the string.
    const ArenaTree tree,                                                       // Arena tree in which to create the node
    const char * const str,                                                     // String
    const size_t length);                                                       // String, or if zero I will call strlen
  ArenaTreeOffset  (*store)(                                                    // Store a copy of the specified data in the arena of the tree and return the offset of the data.
    const ArenaTree tree,                                                       // Tree in whose arena the data will be stored
    const void *  const data,                                                   // Pointer to the data to be stored
    const size_t length);                                                       // Length of the data to be stored
  size_t  (*used)(                                                              // Amount of space currently being used within the arena of a tree.
    const ArenaTree tree);                                                      // Tree
  void  (*write)(                                                               // Write a tree to a named file or abort
    const ArenaTree tree,                                                       // Tree
    const char * const file);                                                   // File
 } const ProtoTypes_ArenaTree =
{allocate_offset_ArenaTree_size, by_ArenaTree_sub, check_ArenaTree, count_size_ArenaTree, findFirstChild_ArenaTree_string, findFirst_ArenaTree_string, free_ArenaTree, fromLetters_ArenaTree_ArenaTreeString, get_ArenaTree_data_size, nodeFromOffset_ArenaTree_size, node_ArenaTreeNode_ArenaTree_ArenaTreeString, noden_ArenaTreeNode_ArenaTree_ArenaTreeString, offsetTo_ArenaTree_pointer, offset_ArenaTree_size, pointer_ArenaTree_size, printContains_ArenaTree, printWithBrackets_ArenaTreeString_ArenaTree, print_string_ArenaTree, printsAs_int_ArenaTree_string, printsWithBracketsAs_int_ArenaTree_string, retrieve_ArenaTree_ArenaTreeOffset_data_size, root_ArenaTreeNodeOffset_ArenaTree, saveString_ArenaTreeOffset_ArenaTree_ArenaTreeString, store_offset_ArenaTree_data_size, used_ArenaTree, write_void_ArenaTree_ArenaTreeString};
ArenaTree newArenaTree(ArenaTree allocator) {return allocator;}

struct ProtoTypes_ArenaTreeNode {
  void  (*by)(                                                                  // Traverse the tree rooted at the specified node in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    ArenaTreeNode node,                                                         // Node
    void (* const function) (const ArenaTreeNode node));                        // Function
  int  (*checkKey)(                                                             // Confirm that the key of a node is equal to the specified string
    const ArenaTreeNode node,                                                   // Node whose key is to be checked
    const char * const key,                                                     // Expected key value
    const size_t length);                                                       // Length of expected key value or zero to request strlen
  ArenaTreeContent *  (*content)(                                               // Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
    const ArenaTreeNode n);                                                     // NodeContent ArenaTreeOffset
  int  (*context)(                                                              // Return true if the parent of the specified child has the specified key.
    const ArenaTreeNode child,                                                  // Child
    ArenaTreeNode * const parent,                                               // Parent container
    const char  * const key);                                                   // Key
  void  (*copyData)(                                                            // Copy the data associated with the sourtce node to the target node by copying the offset to the data held in the source node to the target node.
    const ArenaTreeNode target,                                                 // Target node
    const ArenaTreeNode source);                                                // Source node
  size_t  (*countChildren)(                                                     // Count the number of children in a parent
    const ArenaTreeNode parent);                                                // Parent
  ArenaTreeNode  (*cut)(                                                        // Cut out a child.
    const ArenaTreeNode child);                                                 // Child to cut out
  int  (*equalsString)(                                                         // Check that the key of a node
    const ArenaTreeNode node,                                                   // Node
    const char * const key);                                                    // Key
  int  (*equals)(                                                               // Confirm two offsets are equal
    const ArenaTreeNode a,                                                      // First offset
    const ArenaTreeNode b);                                                     // Second offset
  ArenaTreeNode  (*findFirstChild)(                                             // Find the first child of the specified parent with the specified key.
    const ArenaTreeNode parent,                                                 // Parent node
    const char * const key);                                                    // Key to find immediately under parent
  ArenaTreeNode  (*findFirst)(                                                  // Find the first node with the specified key in a post-order traversal of the tree starting at the specified node.
    const ArenaTreeNode node,                                                   // Node at the start of the tree to be searched
    const char * const key);                                                    // Key to find
  ArenaTreeNode  (*first)(                                                      // Get the first child under a parent.
    const ArenaTreeNode parent);                                                // Parent
  size_t  (*getData)(                                                           // Get the value of the data offset associated with a node.
    const ArenaTreeNode node);                                                  // Node in an arena tree associated with the data
  void  (*get)(                                                                 // Get a copy of the data addressed by a node.
    const ArenaTreeNode node,                                                   // Node in an arena tree associated with the data
    void * const data,                                                          // Pointer to the area into which the data is to be copied
    const size_t length);                                                       // Length of the data to be retrieved
  int  (*isEmpty)(                                                              // Confirm a node has no children.
    const ArenaTreeNode node);                                                  // Node
  int  (*isFirst)(                                                              // Confirm a child is first under its parent
    const ArenaTreeNode child);                                                 // Child
  int  (*isLast)(                                                               // Confirm a child is last under its parent
    const ArenaTreeNode child);                                                 // Child
  int  (*isOnlyChild)(                                                          // Confirm that this child is the only child of its parent
    const ArenaTreeNode child);                                                 // Child
  int  (*isRoot)(                                                               // Confirm a node is the root
    const ArenaTreeNode node);                                                  // NodeContent
  ArenaTreeString  (*key)(                                                      // Get a temporary pointer to the offset containing the key of a node.
    const ArenaTreeNode node);                                                  // NodeContent
  ArenaTreeNode  (*last)(                                                       // Get the last child under a parent.
    const ArenaTreeNode parent);                                                // Parent
  ArenaTreeNode  (*next)(                                                       // Get the next child under a parent.
    const ArenaTreeNode parent);                                                // Parent
  ArenaTreeNode  (*parent)(                                                     // Get the parent of a child
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*prev)(                                                       // Get the prev child under a parent.
    const ArenaTreeNode parent);                                                // Parent
  int  (*printContains)(                                                        // Check the print of an ArenaTree starting at the specified tag contains the expected string.
    const ArenaTreeNode node,                                                   // Starting node
    const char * expected);                                                     // Expected string
  ArenaTreeString  (*printWithBrackets)(                                        // Print a node and the tree below it in preorder as a string with each sub tree enclosed in brackets.
    const ArenaTreeNode node);                                                  // Node
  ReadOnlyBytes  (*print)(                                                      // Print a node and the ArenaTree below it in preorder as a string.
    const ArenaTreeNode node);                                                  // Node
  int  (*printsAs)(                                                             // Check that the ArenaTree starting at the specified node prints as expected.
    const ArenaTreeNode node,                                                   // Node
    const char * const expected);                                               // Expected string when printed
  int  (*printsWithBracketsAs)(                                                 // Check that the ArenaTree starting at the specified node prints with brackets as expected.
    const ArenaTreeNode node,                                                   // Node
    const char * const expected);                                               // Expected string when printed
  ArenaTreeNode  (*putFirst)(                                                   // Put a child first under its parent
    const ArenaTreeNode parent,                                                 // Parent
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*putLast)(                                                    // Put a child last under its parent
    const ArenaTreeNode parent,                                                 // Parent
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*putNext)(                                                    // Put a child next after the specified sibling
    const ArenaTreeNode sibling,                                                // Sibling
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*putPrev)(                                                    // Put a child previous after the specified sibling
    const ArenaTreeNode sibling,                                                // Sibling
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*putTreeFirst)(                                               // Put a child first in the tree containing the arena in which the child was allocated.
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*putTreeLast)(                                                // Put a child last in the tree containing the arena in which the child was allocated.
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*root)(                                                       // Return the root node of the tree containing the specified node.
    const ArenaTreeNode node);                                                  // NodeContent
  void  (*setData)(                                                             // Set the value of the data offset associated with a node.
    const ArenaTreeNode node,                                                   // Node in an arena tree to associate with the data
    const size_t offset);                                                       // Value the data offset is to be set to
  ArenaTreeNode  (*setKey)(                                                     // Copy a string into the key field of a node
    const ArenaTreeNode node,                                                   // Node
    const char * const key);                                                    // Key - it will be copied into the tree
  void  (*set)(                                                                 // Save a copy of the specified data in the arena of the tree and return the offset of the data.
    const ArenaTreeNode node,                                                   // Node in an arena tree to associate with the data
    const void * const data,                                                    // Pointer to the data to be saved
    const size_t length);                                                       // Length of the data to be saved
  ArenaTreeNode  (*unwrap)(                                                     // Unwrap the specified parent and return it.
    const ArenaTreeNode parent);                                                // Parent to unwrap
  int  (*valid)(                                                                // Check that a node is valid.
    const ArenaTreeNode child);                                                 // Node
  ArenaTreeNode  (*wrap)(                                                       // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
    const ArenaTreeNode child,                                                  // Child to wrap
    const char * const key);                                                    // Key for new parent
 } const ProtoTypes_ArenaTreeNode =
{by_ArenaTreeNode_sub, checkKey_int_ArenaTreeNode_string_size, content_ArenaTreeNode, context_ArenaTreeNode, copyData_ArenaTreeNode_ArenaTreeNode, countChildren_size_ArenaTreeNodeOffset, cut_ArenaTreeNode_ArenaTreeNode, equalsString_ArenaTreeNode_string, equals_int_ArenaTreeNode_ArenaTreeNode, findFirstChild_ArenaTreeNode_string, findFirst_ArenaTreeNode_string, first_ArenaTreeNode_ArenaTreeNode, getData_size_ArenaTreeNode, get_ArenaTreeNode_data_size, isEmpty_ArenaTreeNode, isFirst_ArenaTreeNode, isLast_ArenaTreeNode, isOnlyChild_ArenaTreeNode, isRoot_ArenaTreeNode, key_string_ArenaTreeNode, last_ArenaTreeNode_ArenaTreeNode, next_ArenaTreeNode_ArenaTreeNode, parent_ArenaTreeNode_ArenaTreeNode, prev_ArenaTreeNode_ArenaTreeNode, printContains_ArenaTreeNode, printWithBrackets_string_ArenaTreeNode, print_string_ArenaTreeNode, printsAs_int_ArenaTreeNode_string, printsWithBracketsAs_int_ArenaTreeNode_string, putFirst_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode, putLast_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode, putNext_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode, putPrev_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode, putTreeFirst_ArenaTreeNode_ArenaTreeNode, putTreeLast_ArenaTreeNode_ArenaTreeNode, root_ArenaTreeNodeOffset_ArenaTreeNodeOffset, setData_ArenaTreeNode_size, setKey_ArenaTreeNode_ArenaTreeNode_ArenaTreeString, set_ArenaTreeNode_data_size, unwrap_ArenaTreeNode_ArenaTreeNode, valid_ArenaTreeNode, wrap_ArenaTreeNode_ArenaTreeString};
ArenaTreeNode newArenaTreeNode(ArenaTreeNode allocator) {return allocator;}

struct ProtoTypes_ArenaTreeOffset {
  void *  (*pointer)(                                                           // Convert a node describing an offset into an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
    const ArenaTreeOffset o);                                                   // Offset
 } const ProtoTypes_ArenaTreeOffset =
{pointer_ArenaTreeOffset};
ArenaTreeOffset newArenaTreeOffset(ArenaTreeOffset allocator) {return allocator;}

