static ArenaTree makeArenaTreeWithWidth
 (const size_t width);
static ArenaTree makeArenaTree
 ();
static void * pointer_ArenaTree_size
 (const ArenaTree      list,
  const size_t offset);
static ArenaTreeContent * content_ArenaTreeNode
 (const ArenaTreeNode node);
static size_t width_size_ArenaTree
 (const ArenaTree list);
static size_t width_size_ArenaTreeNode
 (const ArenaTreeNode node);
static void * key_pointer_ArenaTreeNode
 (const ArenaTreeNode node);
static void * data_pointer_ArenaTreeNode
 (const ArenaTreeNode node);
static size_t length_size_ArenaTreeNode
 (const ArenaTreeNode node);
static  ArenaTreeNode  nodeFromOffset_ArenaTree_size
 (const ArenaTree      list,
  const size_t delta);
static int equals_int_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode a,
  const ArenaTreeNode b);
static  ArenaTreeNode  allocate_offset_ArenaTree_size
 (const ArenaTree      list,
  const size_t size);
static size_t used_ArenaTree
 (const ArenaTree list);
static ArenaTreeNode node_ArenaTreeNode_ArenaTree_string
 (const ArenaTree            list,
  const void * const key,
  const size_t       length);
static int equalsString_ArenaTreeNode_string
 (const ArenaTreeNode        node,
  const char * const key);
static void dump_ArenaTreeNode
 (const ArenaTreeNode node);
static void free_ArenaTree
 (const ArenaTree tree);
static  ArenaTreeFound makeArenaTreeFound
 (const ArenaTree      tree,
  char * const key,
  const size_t length);
static ArenaTreeNode root_ArenaTree
 (const ArenaTree tree);
static size_t height_ArenaTreeNode
 (const ArenaTreeNode node);
static int keyEquals_ArenaTreeNode_string_size
 (const ArenaTreeNode        node,
  const char * const key,
  const size_t       length);
static ArenaTreeNode up_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode node);
static ArenaTreeNode left_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode     parent);
static ArenaTreeNode right_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode     parent);
static ArenaTreeNode ownedTreeRoot_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode     parent);
static void setHeight_ArenaTreeNode
 (const ArenaTreeNode  parent,
  const size_t height);
static void setUp_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode child,
  const ArenaTreeNode parent);
static void setLeft_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent,
  const ArenaTreeNode left);
static void setRight_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent,
  const ArenaTreeNode right);
static void setTree_ArenaTreeNode_ArenaTreeNode
 (const ArenaTreeNode parent,
  const ArenaTreeNode tree);
static int valid_ArenaTreeNode
 (const ArenaTreeNode node);
static int isRoot_ArenaTreeNode
 (const ArenaTreeNode node);
static int cmpArenaTree
 (const char *const K,
  const size_t      L,
  const char *const k,
  const size_t      l);
static ArenaTreeFound find_ArenaTreeFound_ArenaTreeFound_ArenaTreeNode_string
 (ArenaTreeFound      found,
  const ArenaTreeNode root);
static ArenaTreeNode locate_ArenaTreeNode_string
 (const ArenaTreeNode        node,
  const char * const key,
  const size_t       length);
static ArenaTreeNode locate_ArenaTree_string
 (const ArenaTree            tree,
  const char * const key,
  const size_t       length);
static int check_ArenaTree
 (const ArenaTree tree);
static  ArenaTreeFound find_ArenaTreeFound_ArenaTreeNode_string
 (const ArenaTreeNode  node,
  char * const key,
  const size_t length);
static  ArenaTreeFound find_ArenaTreeFound_ArenaTree_string
 (const ArenaTree      tree,
  char * const key,
  const size_t length);
static ArenaTreeNode ll_ArenaTreeNode_ArenaTree_strings
 (const ArenaTree   tree,
  const char * const keys,
  ...);
static ArenaTreeNode add_ArenaTreeNode_ArenaTreeFound_ArenaTreeNode_string
 (ArenaTreeFound f,
  ArenaTreeNode owner);
static  ArenaTreeNode   add_ArenaTreeNode_ArenaTree_string
 (const ArenaTree       tree,
  char  * const key,
  const size_t  length);
static  ArenaTreeNode   add_ArenaTreeNode_ArenaTreeNode_string
 (const ArenaTreeNode   node,
  char  * const key,
  const size_t  length);
static  size_t count_size_ArenaTree
 (const ArenaTree tree);
static StringBuffer print_ArenaTreeNode
 (const ArenaTreeNode node);
static StringBuffer print_ArenaTree
 (const ArenaTree tree);
static void dump_ArenaTree
 (const ArenaTree tree);
struct ProtoTypes_ArenaTree {
  ArenaTreeNode    (*add)(                                                      // Add a new key if not already present in the specified base ArenaTree.
    const ArenaTree tree,                                                       // The ArenaTree to search
    char  * const key,                                                          // The key to add
    const size_t length);                                                       // Length of the key to find
  ArenaTreeNode   (*allocate)(                                                  // Allocate memory within the arena of a ArenaTree and clear the allocated memory
    const ArenaTree list,                                                       // ArenaTree in which to allocate
    const size_t size);                                                         // Amount of memory required
  int  (*check)(                                                                // Check the integrity of the ArenaTree
    const ArenaTree tree);                                                      // ArenaTree to check
  size_t  (*count)(                                                             // Count the number of nodes in a ArenaTree
    const ArenaTree tree);                                                      // ArenaTree
  void  (*dump)(                                                                // Print a ArenaTree on stderr
    const ArenaTree tree);                                                      // ArenaTree
  ArenaTreeFound  (*find)(                                                      // Find a key if it exists within the base ArenaTree.
    const ArenaTree tree,                                                       // The base ArenaTree to search
    char * const key,                                                           // The key to find
    const size_t length);                                                       // Length of the key to find
  void  (*free)(                                                                // Free a ArenaTree in its entirety
    const ArenaTree tree);                                                      // ArenaTree
  ArenaTreeNode  (*ll)(                                                         // Search through a series of owned trees starting at the base ArenaTree as directed by the specified keys
    const ArenaTree tree,                                                       // The base ArenaTree to search from
    const char * const keys,                                                    // Zero terminated list of keys
     ...);                                                                      // Following keys
  ArenaTreeNode  (*locate)(                                                     // Locate the node with the specified key if it exists within the specified ArenaTree.
    const ArenaTree tree,                                                       // ArenaTree
    const char * const key,                                                     // Key to find
    const size_t length);                                                       // Length of the key to find
  ArenaTreeFound  (*makeArenaTreeFound)(                                        // Create a new found description
    const ArenaTree tree,                                                       // The ArenaTree being searched
    char * const key,                                                           // The key to find
    const size_t length);                                                       // Length of the key to find
  ArenaTreeNode   (*nodeFromOffset)(                                            // Create a node to locate an allocation within the arena of a ArenaTree.
    const ArenaTree list,                                                       // ArenaTree
    const size_t delta);                                                        // Delta within arena. A delta of zero represents no such node.
  ArenaTreeNode  (*node)(                                                       // Create a new ArenaTree node with the specified key.
    const ArenaTree list,                                                       // ArenaTree in which to create the node
    const void * const key,                                                     // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the ArenaTree is determined solely by the user.
    const size_t length);                                                       // Length of the key, or if zero, I will use strlen
  void *  (*pointer)(                                                           // Return a temporary pointer to data in the arena of the specified ArenaTree
    const ArenaTree list,                                                       // ArenaTree
    const size_t offset);                                                       // Offset
  StringBuffer  (*print)(                                                       // Print the specified ArenaTree as a string.
    const ArenaTree tree);                                                      // ArenaTree
  ArenaTreeNode  (*root)(                                                       // The root node in a ArenaTree
    const ArenaTree tree);                                                      // ArenaTree
  size_t  (*used)(                                                              // Amount of space currently being used within the arena of a ArenaTree.
    const ArenaTree list);                                                      // ArenaTree
  size_t  (*width)(                                                             // Get the width of the data area for a ArenaTree
    const ArenaTree list);                                                      // Node
 } const ProtoTypes_ArenaTree =
{add_ArenaTreeNode_ArenaTree_string, allocate_offset_ArenaTree_size, check_ArenaTree, count_size_ArenaTree, dump_ArenaTree, find_ArenaTreeFound_ArenaTree_string, free_ArenaTree, ll_ArenaTreeNode_ArenaTree_strings, locate_ArenaTree_string, makeArenaTreeFound, nodeFromOffset_ArenaTree_size, node_ArenaTreeNode_ArenaTree_string, pointer_ArenaTree_size, print_ArenaTree, root_ArenaTree, used_ArenaTree, width_size_ArenaTree};
ArenaTree newArenaTree(ArenaTree allocator) {return allocator;}

struct ProtoTypes_ArenaTreeFound {
  ArenaTreeNode  (*add)(                                                        // Add a new key if not already present in the ArenaTree root at the specified node.
    ArenaTreeFound f,                                                           // Found status for the key being added.
    ArenaTreeNode owner);                                                       // Invalid - the base ArenaTree. Valid - the node that owns the ArenaTree being added to.
  ArenaTreeFound  (*find)(                                                      // Find a key if it exists within the ArenaTree starting at this node.
    ArenaTreeFound found,                                                       // Found definition
    const ArenaTreeNode root);                                                  // The root node at which the ArenaTree starts
 } const ProtoTypes_ArenaTreeFound =
{add_ArenaTreeNode_ArenaTreeFound_ArenaTreeNode_string, find_ArenaTreeFound_ArenaTreeFound_ArenaTreeNode_string};
ArenaTreeFound newArenaTreeFound(ArenaTreeFound allocator) {return allocator;}

struct ProtoTypes_ArenaTreeNode {
  ArenaTreeNode    (*add)(                                                      // Add a new key if not already present in the ArenaTree owned by the specified node.
    const ArenaTreeNode node,                                                   // The ArenaTree to search
    char  * const key,                                                          // The key to add
    const size_t length);                                                       // Length of the key to find
  ArenaTreeContent *  (*content)(                                               // Convert a node offset to an address so that the content of a node can be updated in situ as long as the ArenaTree is not reallocated to a different position.
    const ArenaTreeNode node);                                                  // Node
  void *  (*data)(                                                              // Get a temporary pointer to the user data of a node.
    const ArenaTreeNode node);                                                  // Node
  void  (*dump)(                                                                // Dump a ArenaTreeNode on stderr
    const ArenaTreeNode node);                                                  // ArenaTreeNode
  int  (*equalsString)(                                                         // Check that the key of a node equals a string
    const ArenaTreeNode node,                                                   // Node
    const char * const key);                                                    // Key
  int  (*equals)(                                                               // Confirm two Nodes are equal
    const ArenaTreeNode a,                                                      // First offset
    const ArenaTreeNode b);                                                     // Second offset
  ArenaTreeFound  (*find)(                                                      // Find a key if it exists within the ArenaTree owned by the specified node.
    const ArenaTreeNode node,                                                   // The node owning the ArenaTree to search
    char * const key,                                                           // The key to find
    const size_t length);                                                       // Length of the key to find
  size_t  (*height)(                                                            // Height of a sub ArenaTree starting at the specified node
    const ArenaTreeNode node);                                                  // Node
  int  (*isRoot)(                                                               // Check that the specified node is valid.
    const ArenaTreeNode node);                                                  // Node
  int  (*keyEquals)(                                                            // Key of the specified node as a zero terminated string
    const ArenaTreeNode node,                                                   // Node
    const char * const key,                                                     // Expected key value
    const size_t length);                                                       // Length of expected key value
  void *  (*key)(                                                               // Get a temporary pointer to the key of a node.
    const ArenaTreeNode node);                                                  // Node
  ArenaTreeNode  (*left)(                                                       // Left child node below the specified parent node.
    const ArenaTreeNode parent);                                                // Parent
  size_t  (*length)(                                                            // Get the length of the key associated with a node
    const ArenaTreeNode node);                                                  // Node
  ArenaTreeNode  (*locate)(                                                     // Locate the node with the specified key if it exists within the ArenaTree owned by the specified node.
    const ArenaTreeNode node,                                                   // Node
    const char * const key,                                                     // Key to find
    const size_t length);                                                       // Length of the key to find
  ArenaTreeNode  (*ownedTreeRoot)(                                              // Root of ArenaTree owned by this node if there is  one, else the returned node has an offset of zero.
    const ArenaTreeNode parent);                                                // Parent
  StringBuffer  (*print)(                                                       // Print the tree below the specified node as a string.
    const ArenaTreeNode node);                                                  // Node
  ArenaTreeNode  (*right)(                                                      // Right child node below the specified parent node.
    const ArenaTreeNode parent);                                                // Parent
  void  (*setHeight)(                                                           // Save the height of the sub ArenaTree starting at the specified node
    const ArenaTreeNode parent,                                                 // Parent
    const size_t height);                                                       // Height
  void  (*setLeft)(                                                             // Set left child of specified parent.
    const ArenaTreeNode parent,                                                 // Parent
    const ArenaTreeNode left);                                                  // Left child
  void  (*setRight)(                                                            // Parent parent for a specified parent in a ArenaTree
    const ArenaTreeNode parent,                                                 // Parent
    const ArenaTreeNode right);                                                 // Right child
  void  (*setTree)(                                                             // Set the other ArenaTree located by this node.
    const ArenaTreeNode parent,                                                 // Parent
    const ArenaTreeNode tree);                                                  // ArenaTree to be added to parent
  void  (*setUp)(                                                               // Set parent node of specified child node
    const ArenaTreeNode child,                                                  // Parent
    const ArenaTreeNode parent);                                                // Child
  ArenaTreeNode  (*up)(                                                         // Parent node for the specified node.
    const ArenaTreeNode node);                                                  // Node
  int  (*valid)(                                                                // Check that the specified node is valid.
    const ArenaTreeNode node);                                                  // Node
  size_t  (*width)(                                                             // Get the width of the data area for a node
    const ArenaTreeNode node);                                                  // Node
 } const ProtoTypes_ArenaTreeNode =
{add_ArenaTreeNode_ArenaTreeNode_string, content_ArenaTreeNode, data_pointer_ArenaTreeNode, dump_ArenaTreeNode, equalsString_ArenaTreeNode_string, equals_int_ArenaTreeNode_ArenaTreeNode, find_ArenaTreeFound_ArenaTreeNode_string, height_ArenaTreeNode, isRoot_ArenaTreeNode, keyEquals_ArenaTreeNode_string_size, key_pointer_ArenaTreeNode, left_ArenaTreeNode_ArenaTreeNode, length_size_ArenaTreeNode, locate_ArenaTreeNode_string, ownedTreeRoot_ArenaTreeNode_ArenaTreeNode, print_ArenaTreeNode, right_ArenaTreeNode_ArenaTreeNode, setHeight_ArenaTreeNode, setLeft_ArenaTreeNode_ArenaTreeNode, setRight_ArenaTreeNode_ArenaTreeNode, setTree_ArenaTreeNode_ArenaTreeNode, setUp_ArenaTreeNode_ArenaTreeNode, up_ArenaTreeNode_ArenaTreeNode, valid_ArenaTreeNode, width_size_ArenaTreeNode};
ArenaTreeNode newArenaTreeNode(ArenaTreeNode allocator) {return allocator;}

