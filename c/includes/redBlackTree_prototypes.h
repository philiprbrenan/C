static void free_RedBlackTree
 (RedBlackTree tree);
static RedBlackTreeFound find_node_tree_string
 (RedBlackTree           tree,
  const char *key);
static RedBlackTreeFound add_node_tree_string
 (RedBlackTree     tree,
  char *key);
static void print_tree
 (RedBlackTree tree);
static char *key
 (RedBlackTreeNode node);
struct ProtoTypes_RedBlackTree {
  RedBlackTreeFound  (*add)(                                                    // Add a new key if not already present in the tree
    RedBlackTree tree,                                                          // The tree to search
    char * key);                                                                // The key to find
  RedBlackTreeFound  (*find)(                                                   // Find a key if it exists with the tree
    RedBlackTree tree,                                                          // The tree to search
    const char * key);                                                          // The key to find
  void  (*free)(                                                                // Free a red black tree
    RedBlackTree tree);                                                         // Tree to free
  void  (*print)(                                                               // Print a tree to stderr with suitable indentation
    RedBlackTree tree);                                                         // Tree
 } const ProtoTypes_RedBlackTree =
{add_node_tree_string, find_node_tree_string, free_RedBlackTree, print_tree};
RedBlackTree newRedBlackTree(RedBlackTree allocator) {return allocator;}

struct ProtoTypes_RedBlackTreeNode {
  char * (*key)(                                                                // Get the key of a node
    RedBlackTreeNode node);                                                     // Node
 } const ProtoTypes_RedBlackTreeNode =
{key};
RedBlackTreeNode newRedBlackTreeNode(RedBlackTreeNode allocator) {return allocator;}

