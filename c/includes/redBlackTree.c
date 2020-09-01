#line 1 "/home/phil/c/z/redBlackTree/redBlackTree.c"
//------------------------------------------------------------------------------
// Red black tree.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef RedBlackTree_included
#define RedBlackTree_included
#include <utilities.c>

//D1 Structures                                                                 // Structures describing the tree. ArenaTree and TreeAllocator are not held inside the arena and so are pointers not offsets.

typedef struct RedBlackTree     *RedBlackTree;                                                        // Red black tree
typedef struct RedBlackTreeNode *RedBlackTreeNode;                                                    // A node in a red black tree

struct RedBlackTreeNode                                                                    // A node in the tree
 {const struct ProtoTypes_RedBlackTreeNode *proto;                                         // Methods associated with a node
  RedBlackTreeNode  left;                                                                  // Left child node
  RedBlackTreeNode  right;                                                                 // Right child node
  RedBlackTreeNode  up;                                                                    // Parent node
  size_t height;                                                                // Height of the tree below this node
  char  *key;                                                                   // The key of the node - a zero terminated character string
 };

typedef struct RedBlackTreeFound                                                           // The result of trying to find a node in the tree.
 {RedBlackTreeNode  last;                                                                  // Last parent node
  RedBlackTreeNode  added;                                                                 // Last node added
  int    different;                                                             // The compare result on the last node
 } RedBlackTreeFound;

struct RedBlackTree                                                                        // Arena Tree.
 {const struct ProtoTypes_RedBlackTree *proto;                                             // Methods associated with a red black tree
  RedBlackTreeNode root;                                                                   // Root element of the tree
 };

#include <redBlackTree_prototypes.h>                                                      // Arena tree prototypes now that the relevant structures have been declared

//D1 Constructor                                                                // Construct a new Arena tree.

RedBlackTree newRedBlackTree                                                                          // Create a new red black tree
 (const struct RedBlackTree     allocator)                                                 // Tree allocator
 {RedBlackTree t = alloc(sizeof(allocator));                                               // Allocate tree
   *t = allocator;                                                              // Initialize tree
  return t;
 }

static void free_RedBlackTree                                                              // Free a red black tree
 (RedBlackTree tree)                                                                       // Tree to free
 {void freeNode                                                                 // Free a node
   (RedBlackTreeNode node)                                                                 // Node
   {if (!node) return;
    freeNode(node->left);
    freeNode(node->right);
    free(node->key);
    free(node);
   }

  freeNode(tree->root);

  free(tree);
 }

RedBlackTreeNode newRedBlackTreeNode                                                                  // Create a new node
 (const struct RedBlackTreeNode allocator)                                                 // Node allocator
 {RedBlackTreeNode  n = alloc(sizeof(allocator));                                          // Allocate node
        *n = allocator;                                                         // Initialize node
  return n;
 }

//D1 Find                                                                       // Find a key if it exists with the tree.

static RedBlackTreeFound find_node_tree_string                                             // Find a key if it exists with the tree
 (RedBlackTree           tree,                                                             // The tree to search
  const char *key)                                                              // The key to find
 {RedBlackTreeFound f = {};
  if (!tree->root) return f;

  for(RedBlackTreeNode p = tree->root; p;)                                                 // Search through tree
   {f.last = p;                                                                 // Record last parent
    const int i = f.different = strcmp(key, p->key);                            // Compare key sought with current key

    if (!i) return f;                                                           // Parent and child must be in same tree
    p = i < 0 ? p->left : p->right;                                             // Delta of parent and child
   }

  return f;
 }

//D1 Add                                                                        // Add a new key if not already present in the tree

static RedBlackTreeFound add_node_tree_string                                              // Add a new key if not already present in the tree
 (RedBlackTree     tree,                                                                   // The tree to search
  char *key)                                                                    // The key to find
 {RedBlackTreeFound f = tree->proto->find(tree, key);                                                  // Try to find the key
  if (!f.last)                                                                  // Empty tree
   {char * const k = strdup(key);
    f.added = tree->root = newRedBlackTreeNode(({struct RedBlackTreeNode t = {key: k, proto: &ProtoTypes_RedBlackTreeNode}; t;}));
    return f;
   }
  if (!f.different) return f;                                                   // Key already present

  char * const k = strdup(key);
  RedBlackTreeNode n = newRedBlackTreeNode(({struct RedBlackTreeNode t = {key: k, proto: &ProtoTypes_RedBlackTreeNode}; t;}));                                                   // Create new node
  if (f.different < 0) f.last->left = n; else f.last->right = n;                // Insert node
  n->up   = f.last;                                                             // Connect to parent
  f.added = n;                                                                  // Node added

  for(RedBlackTreeNode p = n; p; p = p->up)                                                // Balance nodes along path back to root
   {size_t height(RedBlackTreeNode p) {return p ? p->height : 0;}                          // Height of a node

    void setHeight(RedBlackTreeNode p)                                                     // Height of a node
     {const int L = height(p->left), R = height(p->right);
      p->height = 1 + (L > R ? L : R);
     }

    RedBlackTreeNode balance                                                               // Balance node
     (const int d)                                                              // Balance left if true
     {const RedBlackTreeNode q = p->up, l = p->left, r = p->right;

      if (!q)                                                                   // Root node
       {const RedBlackTreeNode n = tree->root = d ? r : l; n->up = NULL;
       }
      else                                                                      // Not the root node
       {if (q->right == p) (q->right = d ? r : l)->up = q;
        else               (q->left  = d ? r : l)->up = q;
       }

      if (d) {p->right = r->left;  r->left  = p;}                               // Balance left
      else   {p->left  = l->right; l->right = p;}                               // Balance right

      setHeight(p->up = d ? r : l);                                             // Set height
      setHeight(p);
      return p->up;                                                             // The node above the input node is now balanced
     }

    const int L = height(p->left), R = height(p->right);
    if      (2*L + 1 < R) p = balance(1);                                       // Balance left or right if necessary
    else if (2*R + 1 < L) p = balance(0);
    else     setHeight(p);                                                      // Balanced: propogate height
   }

  return f;
 }

//D1 Print                                                                      // Print a tree

static void print_tree                                                          // Print a tree to stderr with suitable indentation
 (RedBlackTree tree)                                                                       // Tree
 {void print                                                                    // Print a node
   (RedBlackTreeNode node)                                                                 // Node
   {if (!node) return;
    print(node->left);
    say("%04lu", node->height);
    for(size_t i = 0; i < node->height; ++i) say("  ");
    say("%s\n", node->key);
    print(node->right);
   }

  print(tree->root);
 }

static char *key                                                                // Get the key of a node
 (RedBlackTreeNode node)                                                                   // Node
 {return node->key;
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

#define TEST_TREE_SIZE 10

void test0()                                                                    //Tadd //Tprint //Tkey //Tfind
 {RedBlackTree t = newRedBlackTree(({struct RedBlackTree t = {proto: &ProtoTypes_RedBlackTree};   t;}));

  for  (size_t i = 0; i < TEST_TREE_SIZE; ++i)
   {for(size_t j = 0; j < TEST_TREE_SIZE; ++j)
     {char c[4]; memset(c, 0, 4); c[0] = '0' + i; c[1] = '0' + j;
      t->proto->add(t, c);
     }
   }
  RedBlackTreeNode r = t->root;
  assert(!strcmp(r->proto->key(r), "31"));
  assert(r->height == 9);

  RedBlackTreeFound f = t->proto->find(t, "85");
  assert(!f.different && f.last->height == 2);

  t->proto->free(t);
 }

void test1()
 {RedBlackTree t = newRedBlackTree(({struct RedBlackTree t = {proto: &ProtoTypes_RedBlackTree};   t;}));
  for  (size_t i = TEST_TREE_SIZE; i > 0; --i)
   {for(size_t j = TEST_TREE_SIZE; j > 0; --j)
     {char c[4]; memset(c, 0, 4); c[0] = '0' + i - 1; c[1] = '0' + j - 1;
      t->proto->add(t, c);
     }
   }
  RedBlackTreeNode r = t->root;
  assert(!strcmp(r->proto->key(r), "68"));
  assert(r->height == 9);

  RedBlackTreeFound f = t->proto->find(t, "86");
  assert(!f.different && f.last->height == 2);

  t->proto->free(t);
 }

void test2()
 {RedBlackTree t = newRedBlackTree(({struct RedBlackTree t = {proto: &ProtoTypes_RedBlackTree};   t;}));
  for  (size_t i = 0; i < TEST_TREE_SIZE;        ++i)
   {for(size_t j =        TEST_TREE_SIZE; j > 0; --j)
     {char c[4]; memset(c, 0, 4); c[0] = '0' + i; c[1] = '0' + j - 1;
      t->proto->add(t, c);
     }
   }
  RedBlackTreeNode r = t->root;
  assert(!strcmp(r->proto->key(r), "17"));
  assert(r->height == 13);

  RedBlackTreeFound f = t->proto->find(t, "86");
  assert(!f.different && f.last->height == 8);

  t->proto->free(t);
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0, test1, test2, 0};
  run_tests("RedBlackTree", repetitions, tests);
  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/redBlackTree/redBlackTree
