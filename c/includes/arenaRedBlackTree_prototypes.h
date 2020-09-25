static ArenaRedBlackTree makeArenaRedBlackTree
 ();
static void * pointer_ArenaRedBlackTree_size
 (const ArenaRedBlackTree      tree,
  const size_t offset);
static ArenaRedBlackTreeContent * content_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode node);
static void * key_pointer_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode node);
static size_t length_size_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode node);
static  ArenaRedBlackTreeNode  nodeFromOffset_ArenaRedBlackTree_size
 (const ArenaRedBlackTree      tree,
  const size_t delta);
static int equals_int_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode a,
  const ArenaRedBlackTreeNode b);
static  ArenaRedBlackTreeNode  allocate_offset_ArenaRedBlackTree_size
 (const ArenaRedBlackTree      tree,
  const size_t size);
static size_t used_ArenaRedBlackTree
 (const ArenaRedBlackTree tree);
static ArenaRedBlackTreeNode node_ArenaRedBlackTreeNode_ArenaRedBlackTree_string
 (const ArenaRedBlackTree            tree,
  const void * const key,
  const size_t       length);
static int equalsString_ArenaRedBlackTreeNode_string
 (const ArenaRedBlackTreeNode        node,
  const char * const key);
static void free_ArenaRedBlackTree
 (const ArenaRedBlackTree tree);
static  ArenaRedBlackTreeFound makeArenaRedBlackTreeFound
 (const ArenaRedBlackTree      tree,
  char * const key,
  const size_t length);
static ArenaRedBlackTreeNode root_ArenaRedBlackTree
 (const ArenaRedBlackTree tree);
static size_t height_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode node);
static int keyEquals_ArenaRedBlackTreeNode_string_size
 (const ArenaRedBlackTreeNode        node,
  const char * const key,
  const size_t       length);
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
static int isRoot_ArenaRedBlackTreeNode
 (const ArenaRedBlackTreeNode node);
static int cmpArenaRedBlackTree
 (const char *const K,
  const size_t      L,
  const char *const k,
  const size_t      l);
static ArenaRedBlackTreeFound find_ArenaRedBlackTreeFound_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string
 (ArenaRedBlackTreeFound      found,
  const ArenaRedBlackTreeNode root);
static ArenaRedBlackTreeNode locate_ArenaRedBlackTreeNode_string
 (const ArenaRedBlackTreeNode        node,
  const char * const key,
  const size_t       length);
static ArenaRedBlackTreeNode locate_ArenaRedBlackTree_string
 (const ArenaRedBlackTree            tree,
  const char * const key,
  const size_t       length);
static void check_ArenaRedBlackTree
 (const ArenaRedBlackTree tree);
static  ArenaRedBlackTreeFound find_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string
 (const ArenaRedBlackTreeNode  node,
  char * const key,
  const size_t length);
static  ArenaRedBlackTreeFound find_ArenaRedBlackTreeFound_ArenaRedBlackTree_string
 (const ArenaRedBlackTree      tree,
  char * const key,
  const size_t length);
static ArenaRedBlackTreeNode ll_ArenaRedBlackTreeNode_ArenaRedBlackTree_strings
 (const ArenaRedBlackTree   tree,
  const char * const keys,
  ...);
static ArenaRedBlackTreeNode add_ArenaRedBlackTreeNode_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string
 (ArenaRedBlackTreeFound f,
  ArenaRedBlackTreeNode owner);
static  ArenaRedBlackTreeNode   add_ArenaRedBlackTreeNode_ArenaRedBlackTree_string
 (const ArenaRedBlackTree       tree,
  char  * const key,
  const size_t  length);
static  ArenaRedBlackTreeNode   add_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode_string
 (const ArenaRedBlackTreeNode   node,
  char  * const key,
  const size_t  length);
static  size_t count_size_ArenaRedBlackTree
 (const ArenaRedBlackTree tree);
static void dumpStderr_ArenaRedBlackTree
 (const ArenaRedBlackTree tree);
struct ProtoTypes_ArenaRedBlackTree {
  ArenaRedBlackTreeNode    (*add)(                                              // Add a new key if not already present in the specified base tree.
    const ArenaRedBlackTree tree,                                               // The tree to search
    char  * const key,                                                          // The key to add
    const size_t length);                                                       // Length of the key to find
  ArenaRedBlackTreeNode   (*allocate)(                                          // Allocate memory within the arena of a tree and clear the allocated memory
    const ArenaRedBlackTree tree,                                               // ArenaTree in which to allocate
    const size_t size);                                                         // Amount of memory required
  void  (*check)(                                                               // Check the integrity of the tree
    const ArenaRedBlackTree tree);                                              // Tree to check
  size_t  (*count)(                                                             // Count the number of nodes in a ArenaRedBlackTree
    const ArenaRedBlackTree tree);                                              // ArenaRedBlackTree
  void  (*dumpStderr)(                                                          // Print a tree on stderr
    const ArenaRedBlackTree tree);                                              // ArenaRedBlackTree
  ArenaRedBlackTreeFound  (*find)(                                              // Find a key if it exists within the base tree.
    const ArenaRedBlackTree tree,                                               // The base tree to search
    char * const key,                                                           // The key to find
    const size_t length);                                                       // Length of the key to find
  void  (*free)(                                                                // Free a tree in its entirety
    const ArenaRedBlackTree tree);                                              // Tree
  ArenaRedBlackTreeNode  (*ll)(                                                 // Search through a series of owned trees starting at the base tree as directed by the specified keys
    const ArenaRedBlackTree tree,                                               // The base tree to search from
    const char * const keys,                                                    // Zero terminated list of keys
     ...);                                                                      // Following keys
  ArenaRedBlackTreeNode  (*locate)(                                             // Locate the node with the specified key if it exists within the specified tree.
    const ArenaRedBlackTree tree,                                               // ArenaRedBlackTree
    const char * const key,                                                     // Key to find
    const size_t length);                                                       // Length of the key to find
  ArenaRedBlackTreeFound  (*makeArenaRedBlackTreeFound)(                        // Create a new found description
    const ArenaRedBlackTree tree,                                               // The tree being searched
    char * const key,                                                           // The key to find
    const size_t length);                                                       // Length of the key to find
  ArenaRedBlackTreeNode   (*nodeFromOffset)(                                    // Create a node to locate an allocation within the arena of a tree.
    const ArenaRedBlackTree tree,                                               // Tree
    const size_t delta);                                                        // Delta within arena. A delta of zero represents no such node.
  ArenaRedBlackTreeNode  (*node)(                                               // Create a new ArenaRedBlackTree node with the specified key.
    const ArenaRedBlackTree tree,                                               // Arena tree in which to create the node
    const void * const key,                                                     // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the tree is determined solely by the user.
    const size_t length);                                                       // Length of the key, or if zero, I will use strlen
  void *  (*pointer)(                                                           // Return a temporary pointer to data in the arena of the specified ArenaRedBlackTree
    const ArenaRedBlackTree tree,                                               // ArenaRedBlackTree
    const size_t offset);                                                       // Offset
  ArenaRedBlackTreeNode  (*root)(                                               // The root node in a red black tree
    const ArenaRedBlackTree tree);                                              // Tree
  size_t  (*used)(                                                              // Amount of space currently being used within the arena of a tree.
    const ArenaRedBlackTree tree);                                              // Tree
 } const ProtoTypes_ArenaRedBlackTree =
{add_ArenaRedBlackTreeNode_ArenaRedBlackTree_string, allocate_offset_ArenaRedBlackTree_size, check_ArenaRedBlackTree, count_size_ArenaRedBlackTree, dumpStderr_ArenaRedBlackTree, find_ArenaRedBlackTreeFound_ArenaRedBlackTree_string, free_ArenaRedBlackTree, ll_ArenaRedBlackTreeNode_ArenaRedBlackTree_strings, locate_ArenaRedBlackTree_string, makeArenaRedBlackTreeFound, nodeFromOffset_ArenaRedBlackTree_size, node_ArenaRedBlackTreeNode_ArenaRedBlackTree_string, pointer_ArenaRedBlackTree_size, root_ArenaRedBlackTree, used_ArenaRedBlackTree};
ArenaRedBlackTree newArenaRedBlackTree(ArenaRedBlackTree allocator) {return allocator;}

struct ProtoTypes_ArenaRedBlackTreeFound {
  ArenaRedBlackTreeNode  (*add)(                                                // Add a new key if not already present in the tree root at the specified node.
    ArenaRedBlackTreeFound f,                                                   // Found status for the key being added.
    ArenaRedBlackTreeNode owner);                                               // Invalid - the base tree. Valid - the node that owns the tree being added to.
  ArenaRedBlackTreeFound  (*find)(                                              // Find a key if it exists within the tree starting at this node.
    ArenaRedBlackTreeFound found,                                               // Found definition
    const ArenaRedBlackTreeNode root);                                          // The root node at which the tree starts
 } const ProtoTypes_ArenaRedBlackTreeFound =
{add_ArenaRedBlackTreeNode_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string, find_ArenaRedBlackTreeFound_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string};
ArenaRedBlackTreeFound newArenaRedBlackTreeFound(ArenaRedBlackTreeFound allocator) {return allocator;}

struct ProtoTypes_ArenaRedBlackTreeNode {
  ArenaRedBlackTreeNode    (*add)(                                              // Add a new key if not already present in the tree owned by the specified node.
    const ArenaRedBlackTreeNode node,                                           // The tree to search
    char  * const key,                                                          // The key to add
    const size_t length);                                                       // Length of the key to find
  ArenaRedBlackTreeContent *  (*content)(                                       // Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
    const ArenaRedBlackTreeNode node);                                          // Node
  int  (*equalsString)(                                                         // Check that the key of a node equals a string
    const ArenaRedBlackTreeNode node,                                           // Node
    const char * const key);                                                    // Key
  int  (*equals)(                                                               // Confirm two Nodes are equal
    const ArenaRedBlackTreeNode a,                                              // First offset
    const ArenaRedBlackTreeNode b);                                             // Second offset
  ArenaRedBlackTreeFound  (*find)(                                              // Find a key if it exists within the tree owned by the specified node.
    const ArenaRedBlackTreeNode node,                                           // The node owning the tree to search
    char * const key,                                                           // The key to find
    const size_t length);                                                       // Length of the key to find
  size_t  (*height)(                                                            // Height of a sub tree starting at the specified node
    const ArenaRedBlackTreeNode node);                                          // Node
  int  (*isRoot)(                                                               // Check that the specified node is valid.
    const ArenaRedBlackTreeNode node);                                          // Node
  int  (*keyEquals)(                                                            // Key of the specified node as a zero terminated string
    const ArenaRedBlackTreeNode node,                                           // Node
    const char * const key,                                                     // Expected key value
    const size_t length);                                                       // Length of expected key value
  void *  (*key)(                                                               // Get a temporary pointer to the key of a node.
    const ArenaRedBlackTreeNode node);                                          // Node
  ArenaRedBlackTreeNode  (*left)(                                               // Left child node below the specified parent node.
    const ArenaRedBlackTreeNode parent);                                        // Parent
  size_t  (*length)(                                                            // Get the length of a node
    const ArenaRedBlackTreeNode node);                                          // Node
  ArenaRedBlackTreeNode  (*locate)(                                             // Locate the node with the specified key if it exists within the tree owned by the specified node.
    const ArenaRedBlackTreeNode node,                                           // Node
    const char * const key,                                                     // Key to find
    const size_t length);                                                       // Length of the key to find
  ArenaRedBlackTreeNode  (*ownedTreeRoot)(                                      // Root of tree owned by this node if there is  one, else the returned node has an offset of zero.
    const ArenaRedBlackTreeNode parent);                                        // Parent
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
    const ArenaRedBlackTreeNode tree);                                          // Tree to be added to parent
  void  (*setUp)(                                                               // Set parent node of specified child node
    const ArenaRedBlackTreeNode child,                                          // Parent
    const ArenaRedBlackTreeNode parent);                                        // Child
  ArenaRedBlackTreeNode  (*up)(                                                 // Parent node for the specified node.
    const ArenaRedBlackTreeNode node);                                          // Node
  int  (*valid)(                                                                // Check that the specified node is valid.
    const ArenaRedBlackTreeNode node);                                          // Node
 } const ProtoTypes_ArenaRedBlackTreeNode =
{add_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode_string, content_ArenaRedBlackTreeNode, equalsString_ArenaRedBlackTreeNode_string, equals_int_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, find_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string, height_ArenaRedBlackTreeNode, isRoot_ArenaRedBlackTreeNode, keyEquals_ArenaRedBlackTreeNode_string_size, key_pointer_ArenaRedBlackTreeNode, left_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, length_size_ArenaRedBlackTreeNode, locate_ArenaRedBlackTreeNode_string, ownedTreeRoot_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, right_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, setHeight_ArenaRedBlackTreeNode, setLeft_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, setRight_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, setTree_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, setUp_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, up_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode, valid_ArenaRedBlackTreeNode};
ArenaRedBlackTreeNode newArenaRedBlackTreeNode(ArenaRedBlackTreeNode allocator) {return allocator;}

