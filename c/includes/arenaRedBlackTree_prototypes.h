ArenaRedBlackTree newArenaRedBlackTree
 (const struct ArenaRedBlackTree allocator);
static void * pointer_ArenaRedBlackTree_size
 (const ArenaRedBlackTree      tree,
  const size_t delta);
static void * pointer_ArenaRedBlackTreeOffset
 (const ArenaRedBlackTreeOffset o);
static ArenaRedBlackTreeContent * content_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode n);
static ArenaRedBlackTreeOffset  offset_ArenaRedBlackTree_size
 (const ArenaRedBlackTree       tree,
  const size_t  delta);
static ArenaRedBlackTreeNode   node_ArenaRedBlackTree_size
 (const ArenaRedBlackTree      tree,
  const size_t delta);
static ArenaRedBlackTreeOffset allocate_offset_ArenaRedBlackTree_size
 (const ArenaRedBlackTree      tree,
  const size_t size);
static ArenaRedBlackTreeNode newn_ArenaRedBlackTreeNode_ArenaRedBlackTree_ArenaRedBlackTreeString
 (const ArenaRedBlackTree       tree,
  const char * const key,
  const size_t  length);
static ArenaRedBlackTreeNode new_ArenaRedBlackTreeNode_ArenaRedBlackTree_ArenaRedBlackTreeString
 (const ArenaRedBlackTree            tree,
  const char * const key);
static ArenaRedBlackTreeOffset saveString_ArenaRedBlackTreeOffset_ArenaRedBlackTree_ArenaRedBlackTreeString
 (const ArenaRedBlackTree       tree,
  const char * const str,
  const size_t  length);
ArenaRedBlackTreeFound newArenaRedBlackTreeFound
 (ArenaRedBlackTreeFound  allocator);
ArenaRedBlackTreeFindList newArenaRedBlackTreeFindList
 (ArenaRedBlackTreeFindList allocator);
static void free_ArenaRedBlackTree
 (const ArenaRedBlackTree tree);
static ArenaRedBlackTreeNode root_ArenaRedBlackTree
 (const ArenaRedBlackTree tree);
static size_t height_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode node);
static char *key_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode node);
static ArenaRedBlackTreeNode up_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode node);
static ArenaRedBlackTreeNode left_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode     parent);
static ArenaRedBlackTreeNode right_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode     parent);
static ArenaRedBlackTreeNode ownedTreeRoot_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode     parent);
static void setHeight_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode  parent,
  const size_t height);
static void setUp_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode child,
  const ArenaRedBlackTreeNode parent);
static void setLeft_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode parent,
  const ArenaRedBlackTreeNode left);
static void setRight_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode parent,
  const ArenaRedBlackTreeNode right);
static void setTree_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode parent,
  const ArenaRedBlackTreeNode tree);
static ArenaRedBlackTreeFound  find_ArenaRedBlackTreeFound_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string
 (ArenaRedBlackTreeFound       found,
  const ArenaRedBlackTreeNode  root);
static  ArenaRedBlackTreeFound find_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string
 (const ArenaRedBlackTreeNode  node,
  char * const key);
static  ArenaRedBlackTreeFound find_ArenaRedBlackTreeFound_ArenaRedBlackTree_string
 (const ArenaRedBlackTree      tree,
  char * const key);
static ArenaRedBlackTreeFindList find_ArenaRedBlackTreeFindList_ArenaRedBlackTree
 (ArenaRedBlackTreeFindList findList,
  const ArenaRedBlackTree   tree);
static ArenaRedBlackTreeFound add_ArenaRedBlackTreeFound_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string
 (ArenaRedBlackTreeFound f);
static ArenaRedBlackTreeFound add_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode_string
 (const ArenaRedBlackTreeNode  node,
  char * const key);
static ArenaRedBlackTreeFound add_ArenaRedBlackTreeNode_ArenaRedBlackTree_string
 (const ArenaRedBlackTree     tree,
  char * const key);
static char * sprintRoot_string_rootArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode root);
static char * sprint_string_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode node);
static char * sprint_string_ArenaRedBlackTree
 (const ArenaRedBlackTree tree);
static void print_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode node);
static void print_ArenaRedBlackTree
 (const ArenaRedBlackTree tree);
struct ProtoTypes_ArenaRedBlackTree {
  ArenaRedBlackTreeFound  (*add)(                                               // Add a new key if not already present in the specified base tree.
    const ArenaRedBlackTree tree,                                               // The tree to search
    char * const key);                                                          // The key to add
  ArenaRedBlackTreeOffset  (*allocate)(                                         // Allocate memory within the arena of a tree and clear the allocated memory
    const ArenaRedBlackTree tree,                                               // ArenaTree in which to allocate
    const size_t size);                                                         // Amount of memory required
  ArenaRedBlackTreeFound  (*find)(                                              // Find a key if it exists within the base tree.
    const ArenaRedBlackTree tree,                                               // The base tree to search
    char * const key);                                                          // The key to find
  void  (*free)(                                                                // Free a tree in its entirety
    const ArenaRedBlackTree tree);                                              // Tree
  ArenaRedBlackTreeNode  (*new)(                                                // Create a new tree node keyed by a zero terminated string.
    const ArenaRedBlackTree tree,                                               // Arena tree in which to create the node
    const char * const key);                                                    // Key for this node.  Note: we do not order nodes automatically.
  ArenaRedBlackTreeNode  (*newn)(                                               // Create a new tree node keyed by a string of the specified length to which a terminating zero will be appended.
    const ArenaRedBlackTree tree,                                               // Arena tree in which to create the node
    const char * const key,                                                     // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the tree is determined solely by the user.
    const size_t length);                                                       // Length of the key, or if zero, I will use strlen
  ArenaRedBlackTreeNode    (*node)(                                             // Create a node to locate an allocation within the arena of a tree.
    const ArenaRedBlackTree tree,                                               // Tree
    const size_t delta);                                                        // Delta within arena. A delta of zero represents no such node.
  ArenaRedBlackTreeOffset   (*offset)(                                          // Create an offset to locate an item within the tree.
    const ArenaRedBlackTree tree,                                               // Tree
    const size_t delta);                                                        // Delta within arena
  void *  (*pointer)(                                                           // Return a temporary pointer to an offset in a tree.
    const ArenaRedBlackTree tree,                                               // Tree
    const size_t delta);                                                        // Delta
  void  (*print)(                                                               // Print the specified base tree to stderr.
    const ArenaRedBlackTree tree);                                              // Tree
  ArenaRedBlackTreeNode  (*root)(                                               // The root node in a red black tree
    const ArenaRedBlackTree tree);                                              // Tree
  ArenaRedBlackTreeOffset  (*saveString)(                                       // Save a copy of a zero terminated string in a tree and return the offset of the string.
    const ArenaRedBlackTree tree,                                               // Arena tree in which to create the node
    const char * const str,                                                     // String
    const size_t length);                                                       // String, or if zero I will call strlen
  char *  (*sprint)(                                                            // Print a tree in a string. We allocate a string large enough to hold the print. Freeing the allocated string containing the print is the responsibility fo the caller.
    const ArenaRedBlackTree tree);                                              // Tree to print
 } const ProtoTypes_ArenaRedBlackTree =
{add_ArenaRedBlackTreeNode_ArenaRedBlackTree_string, allocate_offset_ArenaRedBlackTree_size, find_ArenaRedBlackTreeFound_ArenaRedBlackTree_string, free_ArenaRedBlackTree, new_ArenaRedBlackTreeNode_ArenaRedBlackTree_ArenaRedBlackTreeString, newn_ArenaRedBlackTreeNode_ArenaRedBlackTree_ArenaRedBlackTreeString, node_ArenaRedBlackTree_size, offset_ArenaRedBlackTree_size, pointer_ArenaRedBlackTree_size, print_ArenaRedBlackTree, root_ArenaRedBlackTree, saveString_ArenaRedBlackTreeOffset_ArenaRedBlackTree_ArenaRedBlackTreeString, sprint_string_ArenaRedBlackTree};
struct ProtoTypes_ArenaRedBlackTreeFindList {
  ArenaRedBlackTreeFindList  (*find)(                                           // Find a list of keys starting at the base tree. Return true if all the keys were found.
    ArenaRedBlackTreeFindList findList,                                         // The list of keys to search for
    const ArenaRedBlackTree tree);                                              // The base tree to search
  ArenaRedBlackTreeFindList  (*newArenaRedBlackTreeFindList)(                   // New find list
    ArenaRedBlackTreeFindList allocator);                                       // Allocator
 } const ProtoTypes_ArenaRedBlackTreeFindList =
{find_ArenaRedBlackTreeFindList_ArenaRedBlackTree, newArenaRedBlackTreeFindList};
struct ProtoTypes_ArenaRedBlackTreeFound {
  ArenaRedBlackTreeFound  (*add)(                                               // Add a new key if not already present in the tree root at the specified node.
    ArenaRedBlackTreeFound f);                                                  // Found status for the tree to which the specified key is being added.
  ArenaRedBlackTreeFound   (*find)(                                             // Find a key if it exists within the tree starting at this node.
    ArenaRedBlackTreeFound found,                                               // Found definition
    const ArenaRedBlackTreeNode root);                                          // The root node at which the tree starts
  ArenaRedBlackTreeFound  (*newArenaRedBlackTreeFound)(                         // New found status
    ArenaRedBlackTreeFound allocator);                                          // Allocator
 } const ProtoTypes_ArenaRedBlackTreeFound =
{add_ArenaRedBlackTreeFound_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string, find_ArenaRedBlackTreeFound_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string, newArenaRedBlackTreeFound};
struct ProtoTypes_ArenaRedBlackTreeNode {
  ArenaRedBlackTreeFound  (*add)(                                               // Add a new key if not already present in the tree owned by the specified node.
    const ArenaRedBlackTreeNode node,                                           // The tree to search
    char * const key);                                                          // The key to add
  ArenaRedBlackTreeContent *  (*content)(                                       // Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
    const ArenaRedBlackTreeNode n);                                             // NodeContent ArenaRedBlackTreeOffset
  ArenaRedBlackTreeFound  (*find)(                                              // Find a key if it exists within the tree owned by the specified node.
    const ArenaRedBlackTreeNode node,                                           // The node owning the tree to search
    char * const key);                                                          // The key to find
  size_t  (*height)(                                                            // Height of a sub tree starting at the specified node
    const ArenaRedBlackTreeNode node);                                          // Node
  char * (*key)(                                                                // Key of the specified node as a zero terminated string
    const ArenaRedBlackTreeNode node);                                          // Node
  ArenaRedBlackTreeNode  (*left)(                                               // Left child node below the specified parent node.
    const ArenaRedBlackTreeNode parent);                                        // Parent
  ArenaRedBlackTreeNode  (*ownedTreeRoot)(                                      // Root of tree owned by this node if there is  one, else the returned node has an offset of zero.
    const ArenaRedBlackTreeNode parent);                                        // Parent
  void  (*print)(                                                               // Print the tree owned by the specified node to stderr.
    const ArenaRedBlackTreeNode node);                                          // Node owning tree to be printed
  ArenaRedBlackTreeNode  (*right)(                                              // Right child node below the specified parent node.
    const ArenaRedBlackTreeNode parent);                                        // Parent
  void  (*setHeight)(                                                           // Save the height of the sub tree starting at the specified node
    const ArenaRedBlackTreeNode parent,                                         // Parent
    const size_t height);                                                       // Height
  void  (*setLeft)(                                                             // Set left child of specified parent.
    const ArenaRedBlackTreeNode parent,                                         // Parent
    const ArenaRedBlackTreeNode left);                                          // Left child
  void  (*setRight)(                                                            // Parent parent for a specified parent in a red black tree
    const ArenaRedBlackTreeNode parent,                                         // Parent
    const ArenaRedBlackTreeNode right);                                         // Right child
  void  (*setTree)(                                                             // Set the other tree located by this node.
    const ArenaRedBlackTreeNode parent,                                         // Parent
    const ArenaRedBlackTreeNode tree);                                          // Right child
  void  (*setUp)(                                                               // Set parent node of specified child node
    const ArenaRedBlackTreeNode child,                                          // Parent
    const ArenaRedBlackTreeNode parent);                                        // Child
  char *  (*sprintRoot)(                                                        // Print a tree, starting at the specified root node, as a string. We allocate a string large enough to hold the print. Freeing the allocated string containing the print is the responsibility fo the caller.
    const ArenaRedBlackTreeNode root);                                          // Root node of tree to be printed
  char *  (*sprint)(                                                            // Print the tree owned by the specified node in a string. We allocate a string large enough to hold the print. Freeing the allocated string containing the print is the responsibility fo the caller.
    const ArenaRedBlackTreeNode node);                                          // Node owning tree to be printed
  ArenaRedBlackTreeNode  (*up)(                                                 // Parent node for the specified node.
    const ArenaRedBlackTreeNode node);                                          // Node
 } const ProtoTypes_ArenaRedBlackTreeNode =
{add_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode_string, content_ArenaRedBlackTreeNode, find_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string, height_ArenaRedBlackTreeNode, key_ArenaRedBlackTreeNode, left_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, ownedTreeRoot_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, print_ArenaRedBlackTreeNode, right_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, setHeight_ArenaRedBlackTreeNode, setLeft_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, setRight_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, setTree_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, setUp_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, sprintRoot_string_rootArenaRedBlackTreeNode, sprint_string_ArenaRedBlackTreeNode, up_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode};
struct ProtoTypes_ArenaRedBlackTreeOffset {
  void *  (*pointer)(                                                           // Convert a node describing an offset into an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
    const ArenaRedBlackTreeOffset o);                                           // Offset
 } const ProtoTypes_ArenaRedBlackTreeOffset =
{pointer_ArenaRedBlackTreeOffset};
