ArenaTree newArenaTree
 (const struct ArenaTree allocator);
static void * pointer_ArenaTree_size
 (const ArenaTree      tree,
  const size_t delta);
static void * pointer_ArenaTreeOffset
 (const ArenaTreeOffset o);
static ArenaTreeContent * content_ArenaTreeNode
 (const ArenaTreeNode n);
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
static ArenaTreeOffset  offset_ArenaTree_size
 (const ArenaTree       tree,
  const size_t  delta);
static ArenaTreeNode   node_ArenaTree_size
 (const ArenaTree      tree,
  const size_t delta);
static ArenaTreeOffset allocate_offset_ArenaTree_size
 (const ArenaTree      tree,
  const size_t size);
static ArenaTreeNode newn_ArenaTreeNode_ArenaTree_ArenaTreeString
 (const ArenaTree       tree,
  const char * const key,
  const size_t  length);
static ArenaTreeNode new_ArenaTreeNode_ArenaTree_ArenaTreeString
 (const ArenaTree            tree,
  const char * const key);
static ArenaTreeOffset saveString_ArenaTreeOffset_ArenaTree_ArenaTreeString
 (const ArenaTree       tree,
  const char * const str,
  const size_t  length);
static void get_ArenaTree_data_size
 (const ArenaTree      tree,
  const size_t offset,
  void * const data,
  const size_t length);
static ArenaTreeOffset save_offset_ArenaTree_data_size
 (const ArenaTree       tree,
  const void *  const data,
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
static void fromLetters_ArenaTree_ArenaTreeString
 (const ArenaTree tree,
  ArenaTreeString str);
static void free_ArenaTree
 (const ArenaTree tree);
static  ArenaTreeNode parent_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode child);
static  ArenaTreeNode first_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent);
static  ArenaTreeNode last_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent);
static  ArenaTreeNode next_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode child);
static  ArenaTreeNode prev_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode child);
static int findFirstKey_int_tree_string_ArenaTreeNodePointer
 (const ArenaTree            tree,
  const char * const key,
  ArenaTreeNode  * const     result);
static int context_ArenaTreeNode
 (const ArenaTreeNode        child,
  ArenaTreeNode * const      parent,
  const char * const key);
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
static  ArenaTreeNode putFirstLast_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode
 (const int   first,
  const ArenaTreeNode parent,
  const ArenaTreeNode child);
static  ArenaTreeNode putFirst_ArenaTreeNode_tree_ArenaTreeNode
 (const ArenaTree     tree,
  const ArenaTreeNode child);
static  ArenaTreeNode putLast_ArenaTreeNode_tree_ArenaTreeNode
 (const ArenaTree     tree,
  const ArenaTreeNode child);
static  ArenaTreeNode putFirst_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent,
  const ArenaTreeNode child);
static  ArenaTreeNode putLast_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent,
  const ArenaTreeNode child);
static  ArenaTreeNode putNextPrev_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode
 (const int   next,
  const ArenaTreeNode sibling,
  const ArenaTreeNode child);
static  ArenaTreeNode putNext_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode sibling,
  const ArenaTreeNode child);
static  ArenaTreeNode putPrev_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode sibling,
  const ArenaTreeNode child);
static void by_ArenaTree_sub
 (ArenaTree t,
  void (* const function) (const ArenaTreeNode node));
static  size_t count_size_ArenaTreeNodeOffset
 (const ArenaTreeNode parent);
static  size_t count_size_ArenaTree
 (const ArenaTree tree);
static  ArenaTreeString printWithBrackets_string_ArenaTreeNode
 (const ArenaTreeNode   node);
static  ArenaTreeString printWithBrackets_ArenaTreeString_ArenaTree
 (const ArenaTree t);
static  ArenaTreeString print_string_ArenaTreeNode
 (const ArenaTreeNode   node);
static  ArenaTreeString print_ArenaTreeString_ArenaTree
 (const ArenaTree t);
static  ArenaTreeNode cut_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode child);
static  ArenaTreeNode unwrap_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent);
static  ArenaTreeNode wrap_ArenaTreeNode_ArenaTreeString
 (const ArenaTreeNode   child,
  const char * const key);
static size_t used_ArenaTree
 (const ArenaTree tree);
static void write_void_ArenaTree_ArenaTreeString
 (const ArenaTree       tree,
  const char * const file);
struct ProtoTypes_ArenaTree {
  ArenaTreeOffset  (*allocate)(                                                 // Allocate memory within the arena of a tree and clear the allocated memory
    const ArenaTree tree,                                                       // ArenaTree in which to allocate
    const size_t size);                                                         // Amount of memory required
  void  (*by)(                                                                  // Traverse a tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
    ArenaTree t,                                                                // Tree
    void (* const function) (const ArenaTreeNode node));                        // Function
  size_t  (*count)(                                                             // Count the number of nodes in a tree
    const ArenaTree tree);                                                      // Tree
  int  (*findFirstKey)(                                                         // Find the first node with the specified key in the tree. Returns true and sets found if a matching key is found else returns false
    const ArenaTree tree,                                                       // Tree
    const char * const key,                                                     // Key to find
    ArenaTreeNode  * const result);                                             // Output area for node if found
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
  ArenaTreeNode  (*new)(                                                        // Create a new tree node keyed by a zero terminated string.
    const ArenaTree tree,                                                       // Arena tree in which to create the node
    const char * const key);                                                    // Key for this node.  Note: we do not order nodes automatically.
  ArenaTreeNode  (*newn)(                                                       // Create a new tree node keyed by a string of the specified length to which a terminating zero will be appended.
    const ArenaTree tree,                                                       // Arena tree in which to create the node
    const char * const key,                                                     // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the tree is determined solely by the user.
    const size_t length);                                                       // Length of the key, or if zero, I will use strlen
  ArenaTreeNode    (*node)(                                                     // Create a node to locate an allocation within the arena of a tree.
    const ArenaTree tree,                                                       // Tree
    const size_t delta);                                                        // Delta within arena. A delta of zero represents no such node.
  ArenaTreeOffset   (*offset)(                                                  // Create an offset to locate an item within the tree.
    const ArenaTree tree,                                                       // Tree
    const size_t delta);                                                        // Delta within arena
  void *  (*pointer)(                                                           // Return a temporary pointer to an offset in a tree.
    const ArenaTree tree,                                                       // Tree
    const size_t delta);                                                        // Delta
  ArenaTreeString  (*printWithBrackets)(                                        // Print an entire tree in preorder as a string with brackets around each sub tree
    const ArenaTree t);                                                         // Tree
  ArenaTreeString  (*print)(                                                    // Print an entire tree in preorder as a string.
    const ArenaTree t);                                                         // Tree
  ArenaTreeNode  (*putFirst)(                                                   // Put a child first in a tree
    const ArenaTree tree,                                                       // Tree
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*putLast)(                                                    // Put a child last in a tree
    const ArenaTree tree,                                                       // Tree
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*root)(                                                       // Return the root node of a tree.
    const ArenaTree tree);                                                      // Tree
  ArenaTreeOffset  (*saveString)(                                               // Save a copy of a zero terminated string in a tree and return the offset of the string.
    const ArenaTree tree,                                                       // Arena tree in which to create the node
    const char * const str,                                                     // String
    const size_t length);                                                       // String, or if zero I will call strlen
  ArenaTreeOffset  (*save)(                                                     // Save a copy of the specified data in the arena of the tree and return the offset of the data.
    const ArenaTree tree,                                                       // Arena tree in which to create the node
    const void *  const data,                                                   // Pointer to the data to be saved
    const size_t length);                                                       // Length of the data to be saved
  size_t  (*used)(                                                              // Amount of space currently being used within the arena of a tree.
    const ArenaTree tree);                                                      // Tree
  void  (*write)(                                                               // Write a tree to a named file or abort
    const ArenaTree tree,                                                       // Tree
    const char * const file);                                                   // File
 } const ProtoTypes_ArenaTree =
{allocate_offset_ArenaTree_size, by_ArenaTree_sub, count_size_ArenaTree, findFirstKey_int_tree_string_ArenaTreeNodePointer, free_ArenaTree, fromLetters_ArenaTree_ArenaTreeString, get_ArenaTree_data_size, new_ArenaTreeNode_ArenaTree_ArenaTreeString, newn_ArenaTreeNode_ArenaTree_ArenaTreeString, node_ArenaTree_size, offset_ArenaTree_size, pointer_ArenaTree_size, printWithBrackets_ArenaTreeString_ArenaTree, print_ArenaTreeString_ArenaTree, putFirst_ArenaTreeNode_tree_ArenaTreeNode, putLast_ArenaTreeNode_tree_ArenaTreeNode, root_ArenaTreeNodeOffset_ArenaTree, saveString_ArenaTreeOffset_ArenaTree_ArenaTreeString, save_offset_ArenaTree_data_size, used_ArenaTree, write_void_ArenaTree_ArenaTreeString};
struct ProtoTypes_ArenaTreeNode {
  ArenaTreeContent *  (*content)(                                               // Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
    const ArenaTreeNode n);                                                     // NodeContent ArenaTreeOffset
  int  (*context)(                                                              // Return true if the parent of the specified child has the specified key.
    const ArenaTreeNode child,                                                  // Child
    ArenaTreeNode * const parent,                                               // Parent container
    const char * const key);                                                    // Key
  void  (*copyData)(                                                            // Copy the data associated with the sourtce node to the target node by copying the offset to the data held in the source node to the target node.
    const ArenaTreeNode target,                                                 // Target node
    const ArenaTreeNode source);                                                // Source node
  size_t  (*count)(                                                             // Count the number of children in a parent
    const ArenaTreeNode parent);                                                // Parent
  ArenaTreeNode  (*cut)(                                                        // Cut out a child.
    const ArenaTreeNode child);                                                 // Child to cut out
  int  (*equals)(                                                               // Confirm two offsets are equal
    const ArenaTreeNode a,                                                      // First offset
    const ArenaTreeNode b);                                                     // Second offset
  ArenaTreeNode  (*first)(                                                      // Get the first child under a parent.
    const ArenaTreeNode parent);                                                // Parent
  void  (*get)(                                                                 // Get a copy of the data addressed by a node.
    const ArenaTreeNode node,                                                   // Node in an arena tree associated with the data
    void * const data,                                                          // Pointer to the area into which the data is to be copied
    const size_t length);                                                       // Length of the data to be retrieved
  int  (*isEmpty)(                                                              // Confirm a node has no children.
    const ArenaTreeNode node);                                                  // NodeContent
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
  ArenaTreeNode  (*next)(                                                       // Get the next sibling of the specified child.
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*parent)(                                                     // Get the parent of a child
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*prev)(                                                       // Get the previous sibling of the specified child.
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeString  (*printWithBrackets)(                                        // Print a node and the tree below it in preorder as a string with each sub tree enclosed in brackets.
    const ArenaTreeNode node);                                                  // Node
  ArenaTreeString  (*print)(                                                    // Print a node and the tree below it in preorder as a string.
    const ArenaTreeNode node);                                                  // Node
  ArenaTreeNode  (*putFirst)(                                                   // Put a child first under its parent
    const ArenaTreeNode parent,                                                 // Parent
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*putLast)(                                                    // Put a child last under its parent
    const ArenaTreeNode parent,                                                 // Parent
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*putNext)(                                                    // Put a child next after the specified sibling
    const ArenaTreeNode sibling,                                                // Sibling
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*putPrev)(                                                    // Put a child before the specified sibling
    const ArenaTreeNode sibling,                                                // Sibling
    const ArenaTreeNode child);                                                 // Child
  ArenaTreeNode  (*root)(                                                       // Return the root node of the tree containing the specified node.
    const ArenaTreeNode node);                                                  // NodeContent
  ArenaTreeNode  (*setKey)(                                                     // Copy a string into the key field of a node
    const ArenaTreeNode node,                                                   // Node
    const char * const key);                                                    // Key - it will be copied into the tree
  void  (*set)(                                                                 // Save a copy of the specified data in the arena of the tree and return the offset of the data.
    const ArenaTreeNode node,                                                   // Node in an arena tree to associate with the data
    const void * const data,                                                    // Pointer to the data to be saved
    const size_t length);                                                       // Length of the data to be saved
  ArenaTreeNode  (*unwrap)(                                                     // Unwrap the specified parent and return it.
    const ArenaTreeNode parent);                                                // Parent to unwrap
  ArenaTreeNode  (*wrap)(                                                       // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
    const ArenaTreeNode child,                                                  // Child to wrap
    const char * const key);                                                    // Key for new parent
 } const ProtoTypes_ArenaTreeNode =
{content_ArenaTreeNode, context_ArenaTreeNode, copyData_ArenaTreeNode_ArenaTreeNode, count_size_ArenaTreeNodeOffset, cut_ArenaTreeNode_ArenaTreeNode, equals_int_ArenaTreeNode_ArenaTreeNode, first_ArenaTreeNode_ArenaTreeNode, get_ArenaTreeNode_data_size, isEmpty_ArenaTreeNode, isFirst_ArenaTreeNode, isLast_ArenaTreeNode, isOnlyChild_ArenaTreeNode, isRoot_ArenaTreeNode, key_string_ArenaTreeNode, last_ArenaTreeNode_ArenaTreeNode, next_ArenaTreeNode_ArenaTreeNode, parent_ArenaTreeNode_ArenaTreeNode, prev_ArenaTreeNode_ArenaTreeNode, printWithBrackets_string_ArenaTreeNode, print_string_ArenaTreeNode, putFirst_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode, putLast_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode, putNext_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode, putPrev_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode, root_ArenaTreeNodeOffset_ArenaTreeNodeOffset, setKey_ArenaTreeNode_ArenaTreeNode_ArenaTreeString, set_ArenaTreeNode_data_size, unwrap_ArenaTreeNode_ArenaTreeNode, wrap_ArenaTreeNode_ArenaTreeString};
struct ProtoTypes_ArenaTreeOffset {
  void *  (*pointer)(                                                           // Convert a node describing an offset into an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
    const ArenaTreeOffset o);                                                   // Offset
 } const ProtoTypes_ArenaTreeOffset =
{pointer_ArenaTreeOffset};
