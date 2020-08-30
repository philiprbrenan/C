static void * pointer_tree_offset
 (const ArenaTree      tree,
  const size_t delta);
static void * pointer_offset
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
static ArenaTreeNode setKey_ArenaTreeNode_ArenaTreeNode_string
 (const ArenaTreeNode   node,
  const ArenaTreeString key);
static ArenaTreeOffset  offset_tree_delta
 (const ArenaTree       tree,
  const size_t  delta);
static ArenaTreeNode   node_tree_delta
 (const ArenaTree      tree,
  const size_t delta);
static ArenaTreeOffset allocate_offset_tree_delta
 (const ArenaTree      tree,
  const size_t size);
static ArenaTreeOffset saveString_offset_tree_string
 (const ArenaTree tree,
  const ArenaTreeString str);
static ArenaTreeOffset saveNString_offset_tree_string_delta
 (const ArenaTree       tree,
  const ArenaTreeString str,
  const size_t  length);
static ArenaTreeNode new_ArenaTreeNode_tree_string
 (const ArenaTree          tree,
  const ArenaTreeString    key);
static ArenaTreeNode newNStr_ArenaTreeNode_tree_string_length
 (const ArenaTree      tree,
  char * const str,
  const size_t length);
static ArenaTreeNode newNStrExtra_ArenaTreeNode_tree_string_sizet_sizet_pointer
 (const ArenaTree      tree,
  char * const str,
  const size_t length,
  const size_t extraLength,
  const void  *extra);
static void getExtra_ArenaTreeNode_sizet_pointer
 (const ArenaTreeNode  node,
  const size_t extraLength,
  void        *extra);
static void fromLetters_tree_str
 (ArenaTree       tree,
  ArenaTreeString str);
static void free_tree
 (const ArenaTree tree);
static ArenaTreeNode parent_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode  child);
static ArenaTreeNode first_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent);
static ArenaTreeNode last_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent);
static ArenaTreeNode next_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode child);
static ArenaTreeNode prev_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode child);
static int findFirstKey_int_tree_string_ArenaTreeNodePointer
 (const ArenaTree        tree,
  const ArenaTreeString  key,
  ArenaTreeNode  * const result);
static int context_ArenaTreeNode
 (ArenaTreeNode  child,
  ArenaTreeNode *parent,
  char  *key);
static int isFirst_ArenaTreeNode
 (const ArenaTreeNode child);
static int isLast_ArenaTreeNode
 (const ArenaTreeNode child);
static int isEmpty
 (const ArenaTreeNode node);
static int isOnlyChild
 (const ArenaTreeNode child);
static int isRoot
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
static  size_t count_sizet_ArenaTreeNodeOffset
 (const ArenaTreeNode parent);
static  size_t count_size_t_tree
 (const ArenaTree tree);
static  ArenaTreeString printWithBrackets_string_ArenaTreeNode
 (const ArenaTreeNode   node);
static  ArenaTreeString printWithBrackets_string_tree
 (const ArenaTree t);
static  ArenaTreeString print_string_ArenaTreeNode
 (const ArenaTreeNode   node);
static  ArenaTreeString print_string_tree
 (const ArenaTree t);
static  ArenaTreeNode cut_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode child);
static  ArenaTreeNode unwrap_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent);
static  ArenaTreeNode wrap_ArenaTreeNode_string_pointer
 (const ArenaTreeNode   child,
  const ArenaTreeString key);
static size_t used_ArenaTree
 (const ArenaTree tree);
static void write_void_ArenaTree_string
 (const ArenaTree       tree,
  const ArenaTreeString file);
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
    const ArenaTreeString key,                                                  // Key to find
    ArenaTreeNode  * const result);                                             // Output area for node if found
  void  (*free)(                                                                // Free an entire arena tree.
    const ArenaTree tree);                                                      // Arena tree to free
  void  (*fromLetters)(                                                         // Load tree from a string of letters and brackets.  The root node of the tree so constructed is always given the letter 'a'.
    ArenaTree tree,                                                             // Tree
    ArenaTreeString str);                                                       // String of letters and brackets describing desired tree structure
  ArenaTreeNode  (*newNStrExtra)(                                               // Create a new tree node from a string specified by its location and length with following space for a structure of specified size loaded from the specified pointer.
    const ArenaTree tree,                                                       // Arena tree in which to create the node
    char * const str,                                                           // String location
    const size_t length,                                                        // String length
    const size_t extraLength,                                                   // Length of extra data
    const void  * extra);                                                       // Extra data
  ArenaTreeNode  (*newNStr)(                                                    // Create a new tree node from a string specified by its location and length
    const ArenaTree tree,                                                       // Arena tree in which to create the node
    char * const str,                                                           // String location
    const size_t length);                                                       // String length
  ArenaTreeNode  (*new)(                                                        // Create a new tree node and return its offset. The key of the node is zero terminated.
    const ArenaTree tree,                                                       // Arena tree in which to create the node
    const ArenaTreeString key);                                                 // Key for this node.  Note: we do not order nodes automatically.
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
  ArenaTreeOffset  (*saveNString)(                                              // Save a copy of the specified number of characters from a string as a zero terminated string in a tree and return the offset of the string. (An extra byte is allocated to hold the terminating zero).
    const ArenaTree tree,                                                       // Arena tree in which to create the node
    const ArenaTreeString str,                                                  // str for this node.  Note: we do not order nodes automatically.
    const size_t length);                                                       // str for this node.  Note: we do not order nodes automatically.
  ArenaTreeOffset  (*saveString)(                                               // Save a copy of a zero terminated string in a tree and return the offset of the string.
    const ArenaTree tree,                                                       // Arena tree in which to create the node
    const ArenaTreeString str);                                                 // String
  size_t  (*used)(                                                              // Amount of space currently being used within the arena of a tree.
    const ArenaTree tree);                                                      // Tree
  void  (*write)(                                                               // Write a tree to a named file or abort
    const ArenaTree tree,                                                       // Tree
    const ArenaTreeString file);                                                // File
 } const ProtoTypes_ArenaTree =
{allocate_offset_tree_delta, by_ArenaTree_sub, count_size_t_tree, findFirstKey_int_tree_string_ArenaTreeNodePointer, free_tree, fromLetters_tree_str, newNStrExtra_ArenaTreeNode_tree_string_sizet_sizet_pointer, newNStr_ArenaTreeNode_tree_string_length, new_ArenaTreeNode_tree_string, node_tree_delta, offset_tree_delta, pointer_tree_offset, printWithBrackets_string_tree, print_string_tree, putFirst_ArenaTreeNode_tree_ArenaTreeNode, putLast_ArenaTreeNode_tree_ArenaTreeNode, root_ArenaTreeNodeOffset_ArenaTree, saveNString_offset_tree_string_delta, saveString_offset_tree_string, used_ArenaTree, write_void_ArenaTree_string};
struct ProtoTypes_ArenaTreeNode {
  ArenaTreeContent *  (*content)(                                               // Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
    const ArenaTreeNode n);                                                     // NodeContent ArenaTreeOffset
  int  (*context)(                                                              // Return true if the parent of the specified child has the specified key.
    ArenaTreeNode child,                                                        // Child
    ArenaTreeNode * parent,                                                     // Parent container
    char  * key);                                                               // Key
  size_t  (*count)(                                                             // Count the number of children in a parent
    const ArenaTreeNode parent);                                                // Parent
  ArenaTreeNode  (*cut)(                                                        // Cut out a child.
    const ArenaTreeNode child);                                                 // Child to cut out
  int  (*equals)(                                                               // Confirm two offsets are equal
    const ArenaTreeNode a,                                                      // First offset
    const ArenaTreeNode b);                                                     // Second offset
  ArenaTreeNode  (*first)(                                                      // Get the first child under a parent.
    const ArenaTreeNode parent);                                                // Parent
  void  (*getExtra)(                                                            // Get the extra data after a node - the caller must provide the size of the data and a structure into which to copy it.
    const ArenaTreeNode node,                                                   // Node
    const size_t extraLength,                                                   // Length of extra data
    void        * extra);                                                       // Extra data area
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
    const ArenaTreeNode node,                                                   // NodeContent
    const ArenaTreeString key);                                                 // Key - it will be copied into the tree
  ArenaTreeNode  (*unwrap)(                                                     // Unwrap the specified parent and return it.
    const ArenaTreeNode parent);                                                // Parent to unwrap
  ArenaTreeNode  (*wrap)(                                                       // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
    const ArenaTreeNode child,                                                  // Child to wrap
    const ArenaTreeString key);                                                 // Key for new parent
 } const ProtoTypes_ArenaTreeNode =
{content_ArenaTreeNode, context_ArenaTreeNode, count_sizet_ArenaTreeNodeOffset, cut_ArenaTreeNode_ArenaTreeNode, equals_int_ArenaTreeNode_ArenaTreeNode, first_ArenaTreeNode_ArenaTreeNode, getExtra_ArenaTreeNode_sizet_pointer, isEmpty, isFirst_ArenaTreeNode, isLast_ArenaTreeNode, isOnlyChild, isRoot, key_string_ArenaTreeNode, last_ArenaTreeNode_ArenaTreeNode, next_ArenaTreeNode_ArenaTreeNode, parent_ArenaTreeNode_ArenaTreeNode, prev_ArenaTreeNode_ArenaTreeNode, printWithBrackets_string_ArenaTreeNode, print_string_ArenaTreeNode, putFirst_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode, putLast_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode, putNext_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode, putPrev_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode, root_ArenaTreeNodeOffset_ArenaTreeNodeOffset, setKey_ArenaTreeNode_ArenaTreeNode_string, unwrap_ArenaTreeNode_ArenaTreeNode, wrap_ArenaTreeNode_string_pointer};
struct ProtoTypes_ArenaTreeOffset {
  void *  (*pointer)(                                                           // Convert a node  describing an offset into an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
    const ArenaTreeOffset o);                                                   // Offset
 } const ProtoTypes_ArenaTreeOffset =
{pointer_offset};
