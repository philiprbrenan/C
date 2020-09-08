static ArenaRedBlackTree makeArenaRedBlackTree
 ();
static void * pointer_ArenaRedBlackTree_size
 (const ArenaRedBlackTree      tree,
  const size_t delta);
static void * pointer_ArenaRedBlackTreeOffset
 (const ArenaRedBlackTreeOffset o);
static ArenaRedBlackTreeContent * content_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode n);
static int equals_int_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode a,
  const ArenaRedBlackTreeNode b);
static ArenaRedBlackTreeOffset  offset_ArenaRedBlackTree_size
 (const ArenaRedBlackTree       tree,
  const size_t  delta);
static ArenaRedBlackTreeNode   nodeFromOffset_ArenaRedBlackTree_size
 (const ArenaRedBlackTree      tree,
  const size_t delta);
static ArenaRedBlackTreeOffset allocate_offset_ArenaRedBlackTree_size
 (const ArenaRedBlackTree      tree,
  const size_t size);
static ArenaRedBlackTreeNode noden_ArenaRedBlackTreeNode_ArenaRedBlackTree_ArenaRedBlackTreeString
 (const ArenaRedBlackTree       tree,
  const char * const key,
  const size_t  length);
static ArenaRedBlackTreeNode node_ArenaRedBlackTreeNode_ArenaRedBlackTree_ArenaRedBlackTreeString
 (const ArenaRedBlackTree             tree,
  const char * const  key);
static ArenaRedBlackTreeOffset saveString_ArenaRedBlackTreeOffset_ArenaRedBlackTree_ArenaRedBlackTreeString
 (const ArenaRedBlackTree       tree,
  const char * const str,
  const size_t  length);
static size_t getData_size_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode  node);
static void setData_ArenaRedBlackTreeNode_size
 (const ArenaRedBlackTreeNode  node,
  const size_t offset);
static int equalsString_ArenaRedBlackTreeNode_string
 (const ArenaRedBlackTreeNode        node,
  const char * const key);
static void free_ArenaRedBlackTree
 (const ArenaRedBlackTree tree);
static  ArenaRedBlackTreeFound makeArenaRedBlackTreeFound
 (const ArenaRedBlackTree      tree,
  char * const key);
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
static int valid_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode node);
static ArenaRedBlackTreeFound  find_ArenaRedBlackTreeFound_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string
 (ArenaRedBlackTreeFound       found,
  const ArenaRedBlackTreeNode  root);
static void check_ArenaRedBlackTree
 (ArenaRedBlackTree tree);
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
static  ArenaRedBlackTreeFound  add_ArenaRedBlackTreeFound_ArenaRedBlackTree_string
 (const ArenaRedBlackTree       tree,
  char  * const key);
static  ArenaRedBlackTreeFound  add_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string
 (const ArenaRedBlackTreeNode   node,
  char  * const key);
static ReadOnlyBytes sprintRoot_string_rootArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode root);
static ReadOnlyBytes sprint_string_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode node);
static ReadOnlyBytes sprint_string_ArenaRedBlackTree
 (const ArenaRedBlackTree tree);
static void print_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode node);
static void print_ArenaRedBlackTree
 (const ArenaRedBlackTree tree);
struct ProtoTypes_ArenaRedBlackTree {
  ArenaRedBlackTreeFound   (*add)(                                              // Add a new key if not already present in the specified base tree.
    const ArenaRedBlackTree tree,                                               // The tree to search
    char  * const key);                                                         // The key to add
  ArenaRedBlackTreeOffset  (*allocate)(                                         // Allocate memory within the arena of a tree and clear the allocated memory
    const ArenaRedBlackTree tree,                                               // ArenaTree in which to allocate
    const size_t size);                                                         // Amount of memory required
  void  (*check)(                                                               // Check the integrity of the tree
    ArenaRedBlackTree tree);                                                    // Tree to check
  ArenaRedBlackTreeFound  (*find)(                                              // Find a key if it exists within the base tree.
    const ArenaRedBlackTree tree,                                               // The base tree to search
    char * const key);                                                          // The key to find
  void  (*free)(                                                                // Free a tree in its entirety
    const ArenaRedBlackTree tree);                                              // Tree
  ArenaRedBlackTreeFound  (*makeArenaRedBlackTreeFound)(                        // Create a new found description
    const ArenaRedBlackTree tree,                                               // The tree being searched
    char * const key);                                                          // The key to find
  ArenaRedBlackTreeNode    (*nodeFromOffset)(                                   // Create a node to locate an allocation within the arena of a tree.
    const ArenaRedBlackTree tree,                                               // Tree
    const size_t delta);                                                        // Delta within arena. A delta of zero represents no such node.
  ArenaRedBlackTreeNode  (*node)(                                               // Create a new tree node keyed by a zero terminated string.
    const ArenaRedBlackTree tree,                                               // Arena tree in which to create the node
    const char * const key);                                                    // Key for this node.  Note: we do not order nodes automatically.
  ArenaRedBlackTreeNode  (*noden)(                                              // Create a new tree node keyed by a string of the specified length to which a terminating zero will be appended.
    const ArenaRedBlackTree tree,                                               // Arena tree in which to create the node
    const char * const key,                                                     // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the tree is determined solely by the user.
    const size_t length);                                                       // Length of the key, or if zero, I will use strlen
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
  ReadOnlyBytes  (*sprint)(                                                     // Print a tree  as ReadOnlyBytes.
    const ArenaRedBlackTree tree);                                              // Tree to print
 } const ProtoTypes_ArenaRedBlackTree =
{add_ArenaRedBlackTreeFound_ArenaRedBlackTree_string, allocate_offset_ArenaRedBlackTree_size, check_ArenaRedBlackTree, find_ArenaRedBlackTreeFound_ArenaRedBlackTree_string, free_ArenaRedBlackTree, makeArenaRedBlackTreeFound, nodeFromOffset_ArenaRedBlackTree_size, node_ArenaRedBlackTreeNode_ArenaRedBlackTree_ArenaRedBlackTreeString, noden_ArenaRedBlackTreeNode_ArenaRedBlackTree_ArenaRedBlackTreeString, offset_ArenaRedBlackTree_size, pointer_ArenaRedBlackTree_size, print_ArenaRedBlackTree, root_ArenaRedBlackTree, saveString_ArenaRedBlackTreeOffset_ArenaRedBlackTree_ArenaRedBlackTreeString, sprint_string_ArenaRedBlackTree};
ArenaRedBlackTree newArenaRedBlackTree(ArenaRedBlackTree allocator) {return allocator;}

struct ProtoTypes_ArenaRedBlackTreeFindList {
  ArenaRedBlackTreeFindList  (*find)(                                           // Find a list of keys starting at the base tree. Return true if all the keys were found.
    ArenaRedBlackTreeFindList findList,                                         // The list of keys to search for
    const ArenaRedBlackTree tree);                                              // The base tree to search
 } const ProtoTypes_ArenaRedBlackTreeFindList =
{find_ArenaRedBlackTreeFindList_ArenaRedBlackTree};
ArenaRedBlackTreeFindList newArenaRedBlackTreeFindList(ArenaRedBlackTreeFindList allocator) {return allocator;}

struct ProtoTypes_ArenaRedBlackTreeFound {
  ArenaRedBlackTreeFound  (*add)(                                               // Add a new key if not already present in the tree root at the specified node.
    ArenaRedBlackTreeFound f);                                                  // Found status for the tree to which the specified key is being added.
  ArenaRedBlackTreeFound   (*find)(                                             // Find a key if it exists within the tree starting at this node.
    ArenaRedBlackTreeFound found,                                               // Found definition
    const ArenaRedBlackTreeNode root);                                          // The root node at which the tree starts
 } const ProtoTypes_ArenaRedBlackTreeFound =
{add_ArenaRedBlackTreeFound_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string, find_ArenaRedBlackTreeFound_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string};
ArenaRedBlackTreeFound newArenaRedBlackTreeFound(ArenaRedBlackTreeFound allocator) {return allocator;}

struct ProtoTypes_ArenaRedBlackTreeNode {
  ArenaRedBlackTreeFound   (*add)(                                              // Add a new key if not already present in the tree owned by the specified node.
    const ArenaRedBlackTreeNode node,                                           // The tree to search
    char  * const key);                                                         // The key to add
  ArenaRedBlackTreeContent *  (*content)(                                       // Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
    const ArenaRedBlackTreeNode n);                                             // NodeContent ArenaRedBlackTreeOffset
  int  (*equalsString)(                                                         // Check that the key of a node
    const ArenaRedBlackTreeNode node,                                           // Node
    const char * const key);                                                    // Key
  int  (*equals)(                                                               // Confirm two offsets are equal
    const ArenaRedBlackTreeNode a,                                              // First offset
    const ArenaRedBlackTreeNode b);                                             // Second offset
  ArenaRedBlackTreeFound  (*find)(                                              // Find a key if it exists within the tree owned by the specified node.
    const ArenaRedBlackTreeNode node,                                           // The node owning the tree to search
    char * const key);                                                          // The key to find
  size_t  (*getData)(                                                           // Get the value of the data offset associated with a node.
    const ArenaRedBlackTreeNode node);                                          // Node in an arena tree associated with the data
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
  void  (*setData)(                                                             // Set the value of the data offset associated with a node.
    const ArenaRedBlackTreeNode node,                                           // Node in an arena tree to associate with the data
    const size_t offset);                                                       // Value the data offset is to be set to
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
    const ArenaRedBlackTreeNode tree);                                          // Tree to be added to parent
  void  (*setUp)(                                                               // Set parent node of specified child node
    const ArenaRedBlackTreeNode child,                                          // Parent
    const ArenaRedBlackTreeNode parent);                                        // Child
  ReadOnlyBytes  (*sprintRoot)(                                                 // Print a tree, starting at the specified root node, as a string. We allocate a string large enough to hold the print. Freeing the allocated string containing the print is the responsibility fo the caller.
    const ArenaRedBlackTreeNode root);                                          // Root node of tree to be printed
  ReadOnlyBytes  (*sprint)(                                                     // Print the tree owned by the specified node as ReadOnlyBytes.
    const ArenaRedBlackTreeNode node);                                          // Node owning tree to be printed
  ArenaRedBlackTreeNode  (*up)(                                                 // Parent node for the specified node.
    const ArenaRedBlackTreeNode node);                                          // Node
  int  (*valid)(                                                                // Check that the specified node is valid.
    const ArenaRedBlackTreeNode node);                                          // Node
 } const ProtoTypes_ArenaRedBlackTreeNode =
{add_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string, content_ArenaRedBlackTreeNode, equalsString_ArenaRedBlackTreeNode_string, equals_int_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, find_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string, getData_size_ArenaRedBlackTreeNode, height_ArenaRedBlackTreeNode, key_ArenaRedBlackTreeNode, left_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, ownedTreeRoot_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, print_ArenaRedBlackTreeNode, right_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, setData_ArenaRedBlackTreeNode_size, setHeight_ArenaRedBlackTreeNode, setLeft_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, setRight_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, setTree_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, setUp_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, sprintRoot_string_rootArenaRedBlackTreeNode, sprint_string_ArenaRedBlackTreeNode, up_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, valid_ArenaRedBlackTreeNode};
ArenaRedBlackTreeNode newArenaRedBlackTreeNode(ArenaRedBlackTreeNode allocator) {return allocator;}

struct ProtoTypes_ArenaRedBlackTreeOffset {
  void *  (*pointer)(                                                           // Convert a node describing an offset into an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
    const ArenaRedBlackTreeOffset o);                                           // Offset
 } const ProtoTypes_ArenaRedBlackTreeOffset =
{pointer_ArenaRedBlackTreeOffset};
ArenaRedBlackTreeOffset newArenaRedBlackTreeOffset(ArenaRedBlackTreeOffset allocator) {return allocator;}

