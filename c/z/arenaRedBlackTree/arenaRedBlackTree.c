//------------------------------------------------------------------------------
// Red black tree in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------

/* Each node of the base red black tree can, in turn, have a separate red black
tree dependent from it with the same arena*/

#define _GNU_SOURCE
#ifndef $_included
#define $_included
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Red Black Tree.
#define $StartString "rbt"                                                      // String that starts the arena
typedef char * $String;                                                         // Arena Tree string

include ../arenaTree/arenaTree.c :structs !$Content

typedef struct $Content                                                         // Content of a node in a red black tree
 {$Delta left;                                                                  // Left child node offset
  $Delta right;                                                                 // Right child node offset
  $Delta up;                                                                    // Parent node offset
  $Delta height;                                                                // Height of the tree below this node
  $Delta key;                                                                   // Offset of the key of the node - a zero terminated character string
  $Delta data;                                                                  // User data associated with this node.
  $Delta tree;                                                                  // Location of another tree
 } $Content;

typedef struct $Found                                                           // The result of trying to find a node in the tree.
 {const struct ProtoTypes_$Found *proto;                                        // Methods
  char *key;                                                                    // Key to find
  $     tree;                                                                   // Tree being processed
  $Node last;                                                                   // Last node found during find(). last.offset will be zero if the tree to be searched is empty.
  $Node added;                                                                  // Last node added during add()
  int   different;                                                              // The compare result on the last node found. If zero, the last node found was a match
 } $Found;

typedef struct $FindList                                                        // Find a concatenated key in a base tree
 {const struct ProtoTypes_$FindList *proto;                                     // Methods
  char *(keys[17]);                                                             // Zero terminated array of keys to find
  $Found found;                                                                 // Found results on last tree searched.
  int    count;                                                                 // The number of levels at which we were able to find a matching node.
  int    all;                                                                   // Found all the keys if true
 } $FindList;

#include <$$_prototypes.h>                                                      // Arena tree prototypes now that the relevant structures have been declared

//D1 Pointers, offsets and allocations                                          // Locate items allocated in the arena

include ../arenaTree/arenaTree.c :arena

static void free_$                                                              // Free a tree in its entirety
 (const $ tree)                                                                 // Tree
 {free(tree.arena->data);
  free(tree.arena);
 }

//D1 Attributes                                                                 // Get and set the attributes of a node

static $Node root_$                                                             // The root node in a red black tree
 (const $ tree)                                                                 // Tree
 {return tree ▷ nodeFromOffset(tree.arena->root);
 }

static size_t height_$Node                                                      // Height of a sub tree starting at the specified node
 (const $Node node)                                                             // Node
 {if (node.offset == 0) return 0;
  return node ▷ content->height.delta;
 }

static char *key_$Node                                                          // Key of the specified node as a zero terminated string
 (const $Node node)                                                             // Node
 {const $         t = node.tree;
  return          t ▷ pointer(node ▷ content->key.delta);
 }

static $Node up_$Node_$Node                                                     // Parent node for the specified node.
 (const $Node node)                                                             // Node
 {const $         t = node.tree;
  return t ▷ nodeFromOffset(node ▷ content->up.delta);                          // $Node.offset will be zero if we are at the top of the tree
 }

static $Node left_$Node_$Node                                                   // Left child node below the specified parent node.
 (const $Node     parent)                                                       // Parent
 {const $         t = parent.tree;
  return t ▷ nodeFromOffset(parent ▷ content->left.delta);
 }

static $Node right_$Node_$Node                                                  // Right child node below the specified parent node.
 (const $Node     parent)                                                       // Parent
 {const $         t = parent.tree;
  return t ▷ nodeFromOffset(parent ▷ content->right.delta);
 }

static $Node ownedTreeRoot_$Node_$Node                                          //P Root of tree owned by this node if there is  one, else the returned node has an offset of zero.
 (const $Node     parent)                                                       // Parent
 {const $         t = parent.tree;
  return t ▷ nodeFromOffset(parent ▷ content->tree.delta);
 }

static void setHeight_$Node                                                     //P Save the height of the sub tree starting at the specified node
 (const $Node  parent,                                                          // Parent
  const size_t height)                                                          // Height
 {parent ▷ content->height.delta = height;
 }

static void setUp_$Node_$Node                                                   //P Set parent node of specified child node
 (const $Node child,                                                            // Parent
  const $Node parent)                                                           // Child
 {child ▷ content->up.delta = parent.offset;
 }

static void setLeft_$Node_$Node                                                 //P Set left child of specified parent.
 (const $Node parent,                                                           // Parent
  const $Node left)                                                             // Left child
 {parent ▷ content->left.delta = left.offset;
 }

static void setRight_$Node_$Node                                                //P Parent parent for a specified parent in a red black tree
 (const $Node parent,                                                           // Parent
  const $Node right)                                                            // Right child
 {parent ▷ content->right.delta = right.offset;
 }

static void setTree_$Node_$Node                                                 //P Set the other tree located by this node.
 (const $Node parent,                                                           // Parent
  const $Node tree)                                                             // Right child
 {parent ▷ content->tree.delta = tree.offset;
 }

//D1 Find                                                                       // Find a key if it exists within the base tree or an owned tree.

static $Found  find_$Found_$Found_$Node_string                                  //P Find a key if it exists within the tree starting at this node.
 ($Found       found,                                                           // Found definition
  const $Node  root)                                                            // The root node at which the tree starts
 {$ tree = root.tree;                                                           // Tree containing node
  char *key = found.key;                                                        // The key to find

  for($Node p     = root; p.offset;)                                            // Search down through tree
   {found.last    = p;                                                          // Record last parent compared with key sought
    const char *k = p ▷ key;                                                    // Key of current parent node
    const int i   = found.different = strcmp(key, k);                           // Compare key sought with current key

    if (!i) return found;                                                       // Found
    $Content * const c = p ▷ content;                                           // Continue
    p = tree ▷ nodeFromOffset(i < 0 ? c->left.delta : c->right.delta);          // Continue left or right
   }

  return found;                                                                 // Found
 }

static  $Found find_$Found_$Node_string                                         // Find a key if it exists within the tree owned by the specified node.
 (const $Node  node,                                                            // The node owning the tree to search
  char * const key)                                                             // The key to find
 {$Found f = new$Found(tree: node.tree, key: key);                              // Status of find
  $Node  r = node ▷ ownedTreeRoot;                                              // Root node of owned tree
  if (!r.offset) return f;                                                      // Empty owned tree
  return f ▷ find(r);                                                           // Search the non empty owned tree starting at the specified node.
 }

static  $Found find_$Found_$_string                                             // Find a key if it exists within the base tree.
 (const $      tree,                                                            // The base tree to search
  char * const key)                                                             // The key to find
 {$Found f = new$Found(tree: tree,      key: key);                              // Status of find
  if (!tree.arena->root) return f;                                              // Empty tree
  return f ▷ find(tree ▷ nodeFromOffset(tree.arena->root));                     // Search the non empty base tree starting at the specified node.
 }

static $FindList find_$FindList_$                                               // Find a list of keys starting at the base tree. Return true if all the keys were found.
 ($FindList findList,                                                           // The list of keys to search for
  const $   tree)                                                               // The base tree to search
 {findList.count = 0;                                                           // Number of keys found
  char **keys = findList.keys;                                                  // Zero terminated array of keys to find.
  if (!*keys) {findList.all = 1; return findList;}                              // No keys to find - so we found all of them because we cannot show a key that we did not find.
  $Found f = findList.found = tree ▷ find(*keys);                               // Status of find of first key in base tree
  if (f.different) return findList;                                             // Cannot find first key in base tree
  findList.count++;                                                             // Found the first key
  for(++keys; *keys; ++keys, ++findList.count)                                  // Each following find request for a following tree owned by the last found node
   {$Node node = f.last;                                                        // Last node found
    f = findList.found = node ▷ find(*keys);                                    // Status of find of current key in tree owned by last found node
    if (f.different) return findList;                                           // Failed to find the next key
   }
  findList.all = 1;                                                             // Successfully found all keys
  return findList;
 }

//D1 Add                                                                        // Add a new key if not already present in a base tree or a tree owned by a node.

static $Found add_$Found_$Found_$Node_string                                    // Add a new key if not already present in the tree root at the specified node.
 ($Found f)                                                                     // Found status for the tree to which the specified key is being added.
 {if (!f.different) return f;                                                   // Key already present

  char * const key = f.key;                                                     // The key to add
  const $ tree = f.last.tree;                                                   // The base tree
  const $Node          n = f.added = tree ▷ node(key), p = f.last;              // Create new node under parent
  $Content * const N = n ▷ content,       * const P = p ▷ content;

  if (f.different < 0) P->left .delta = n.offset;                               // Insert node left
  else                 P->right.delta = n.offset;                               // Insert node right

  N->up.delta = p.offset;                                                       // Set parent of inserted node

  for($Node p = n; p.offset; p = p ▷ up)                                        // Balance nodes along path back to root
   {size_t height($Node p)                                                      // Height of a node
     {return p.offset ? p ▷ height : 0;
     }

    void setHeight($Node p)                                                     // Set the height of a node
     {const int L = height(p ▷ left), R = height(p ▷ right);
      p ▷ setHeight(1 + (L > R ? L : R));
     }

    $Node balance                                                               // Balance node
     (const int d)                                                              // Balance left if true
     {const $Node q = p ▷ up, l = p ▷ left, r = p ▷ right, rl = d ? r : l;
      if (!q.offset)                                                            // Root node
       {const $Node z = {};
        tree.arena->root = rl.offset;
        rl ▷ setUp(z);
       }
      else                                                                      // Not the root node
       {if ((q ▷ right).offset == p.offset)
         {q ▷ setRight(rl); rl ▷ setUp(q);
         }
        else
         {q ▷ setLeft(rl);  rl ▷ setUp(q);
         }
       }

      if (d) {p ▷ setRight(r ▷ left);  r ▷ setLeft (p);}                        // Balance left
      else   {p ▷ setLeft (l ▷ right); l ▷ setRight(p);}                        // Balance right

      p ▷ setUp(rl);
      setHeight(rl);
      setHeight(p);
      return p ▷ up;                                                            // The node above the input node is now balanced
     }

    const int L = height(p ▷ left), R = height(p ▷ right);                      // Left and right depths
    if      (2*L + 1 < R) p = balance(1);                                       // Balance left or right if necessary
    else if (2*R + 1 < L) p = balance(0);
    else     setHeight(p);                                                      // Balanced: propogate height
   }

  return f;
 }

static $Found add_$Node_$Node_string                                            // Add a new key if not already present in the tree owned by the specified node.
 (const $Node  node,                                                            // The tree to search
  char * const key)                                                             // The key to add
 {$ tree   = node.tree;                                                         // Tree to search
  $Found f = node ▷ find(key);                                                  // Try to find the key

  if (!f.last.offset)                                                           // Empty tree
   {const $Node root = f.added = tree ▷ node(key);                              // Add new key in a node
    node ▷ setTree(root);                                                       // Make the new node the root node
    return f;                                                                   // Find status
   }
  return f ▷ add;                                                               // Non empty tree - add the node below the root node
 }

static $Found add_$Node_$_string                                                // Add a new key if not already present in the specified base tree.
 (const $     tree,                                                             // The tree to search
  char * const key)                                                             // The key to add
 {$Found f = tree ▷ find(key);                                                  // Try to find the key

  if (!f.last.offset)                                                           // Empty tree
   {const $Node root = f.added = tree ▷ node(key);                              // Add new key in a node
    tree.arena->root = root.offset;                                             // Make the new node the root node
    return f;                                                                   // Find status
   }
  return f ▷ add;                                                               // Non empty tree - add the node below the root node
 }

//D1 Print                                                                      // Print a tree.

static char * sprintRoot_string_root$Node                                       //P Print a tree, starting at the specified root node, as a string. We allocate a string large enough to hold the print. Freeing the allocated string containing the print is the responsibility fo the caller.
 (const $Node root)                                                             // Root node of tree to be printed
 {const int W = 4;                                                              // Width of height field
  if (!root.offset) return strdup("");                                          // The tree is empty - return a freeable empty string

  size_t l = 0;                                                                 // Length of buffer needed
  void len($Node node)                                                          // Get length of print
   {if (node.offset)
     {len(node ▷ left); l += 2 + W + 1 + 4 * node ▷ height; len(node ▷ right);  // With room for white space
     }
   }
  len(root);                                                                    // Set size of print

  char *p;
  void print(const $Node node)                                                  // Print to allocated string
   {if (!node.offset) return;                                                   // No such node
    print(node ▷ left);                                                         // Print left

    const size_t H = node ▷ height, N = H > 0 ? H - 1 : 0;                      // Height
    char w[W+2]; sprintf(w, "%04lu ", N);
    p = stpcpy(p, w);

    for(size_t i = 0; i < N; ++i) p = stpcpy(p, "    ");                        // Spacer
    p = stpcpy(p, node ▷ key);                                                  // Key
    p = stpcpy(p, "\n");                                                        // End of line
    print(node ▷ right);                                                        // Print right
   }

  char *P = p = malloc(l); *p = 0;                                              // Create buffer
  print(root);                                                                  // Print to buffer

  return P;                                                                     // Please free the returned string when done.
 }

static char * sprint_string_$Node                                               // Print the tree owned by the specified node in a string. We allocate a string large enough to hold the print. Freeing the allocated string containing the print is the responsibility fo the caller.
 (const $Node node)                                                             // Node owning tree to be printed
 {$Node  root = node ▷ ownedTreeRoot;                                           // Root of tree to be printed
  return root ▷ sprintRoot;
 }

static char * sprint_string_$                                                   // Print a tree in a string. We allocate a string large enough to hold the print. Freeing the allocated string containing the print is the responsibility fo the caller.
 (const $ tree)                                                                 // Tree to print
 {const $Node root = tree ▷ root;                                               // Root of tree to print
  return root ▷ sprintRoot;
 }

static void print_$Node                                                         // Print the tree owned by the specified node to stderr.
 (const $Node node)                                                             // Node owning tree to be printed
 {char * const p = node ▷ sprint; fprintf(stderr, "%s\n", p); free(p);
 }

static void print_$                                                             // Print the specified base tree to stderr.
 (const $ tree)                                                                 // Tree
 {char * const p = tree ▷ sprint; fprintf(stderr, "%s\n", p); free(p);
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

#define TEST_TREE_SIZE 10

void test0()                                                                    //Tnew$ //Tadd //Tleft //Tright //Tkey //Tsprint //Tup
 {$ t = make$();
    t ▷ add("b");
    t ▷ add("a");
    t ▷ add("c");
    t ▷ add("bb");
    t ▷ add("cc");
  $Node b = t ▷ root, a = b ▷ left, c = b ▷ right, bb = c ▷ left, cc = c ▷ right;
  assert(!strcmp("b",  b  ▷ key));
  assert(!strcmp("a",  a  ▷ key));
  assert(!strcmp("c",  c  ▷ key));
  assert(!strcmp("bb", bb ▷ key));
  assert(!strcmp("cc", cc ▷ key));
  assert((b  ▷ up).offset == 0);
  assert((a  ▷ up).offset == b.offset);
  assert((c  ▷ up).offset == b.offset);
  assert((bb ▷ up).offset == c.offset);
  assert((cc ▷ up).offset == c.offset);

  char * const p = t ▷ sprint;
  assert(strstr(p, "0000 a"));
  assert(strstr(p, "0002         b"));
  assert(strstr(p, "0000 bb"));
  assert(strstr(p, "0001     c"));
  assert(strstr(p, "0000 cc"));
  free(p);

  assert(!t ▷ find("cc").different);
  assert( t ▷ find("dd").different);

  t ▷ free;
 }

void test1()
 {$ t = make$();
    t ▷ add("a"); t ▷ add("b");
  $Node a = t ▷ root, b = a ▷ right;

  b ▷ add("4"); b ▷ add("2"); b ▷ add("6");
  b ▷ add("1"); b ▷ add("3"); b ▷ add("5"); b ▷ add("7");

  $Node b4 = b  ▷ ownedTreeRoot, b2 = b4 ▷ left, b6 = b4 ▷ right,
        b1 = b2 ▷ left, b3 = b2 ▷ right, b5 = b6 ▷ left, b7 = b6 ▷ right;
  assert(!strcmp("4",  b4 ▷ key));
  assert(!strcmp("2",  b2 ▷ key));
  assert(!strcmp("6",  b6 ▷ key));
  assert(!strcmp("1",  b1 ▷ key));
  assert(!strcmp("3",  b3 ▷ key));
  assert(!strcmp("5",  b5 ▷ key));
  assert(!strcmp("7",  b7 ▷ key));
  assert((b4 ▷ up).offset == 0);
  assert((b2 ▷ up).offset == b4.offset);
  assert((b6 ▷ up).offset == b4.offset);
  assert((b1 ▷ up).offset == b2.offset);
  assert((b3 ▷ up).offset == b2.offset);
  assert((b5 ▷ up).offset == b6.offset);
  assert((b7 ▷ up).offset == b6.offset);

  char * const p = b ▷ sprint;
  assert(strstr(p, "0000 1"));
  assert(strstr(p, "0001     2"));
  assert(strstr(p, "0000 3"));
  assert(strstr(p, "0002         4"));
  assert(strstr(p, "0000 5"));
  assert(strstr(p, "0001     6"));
  assert(strstr(p, "0000 7"));
  free(p);

  char * const q = t ▷ sprint;
  assert(strstr(q, "0001     a"));
  assert(strstr(q, "0000 b"));
  free(q);

  assert(!b ▷ find("7").different);
  assert( b ▷ find("8").different);

  $FindList l = new$FindList(keys: {"b", "7", 0});
  $FindList L = l ▷ find(t);
  assert(L.count == 2);
  assert(L.found.last.offset == b7.offset);

  t ▷ free;
 }

void test2()                                                                    //Tprint //Tfree
 {$ t = make$();

  for  (size_t i = 0; i < TEST_TREE_SIZE; ++i)
   {char c[2]; c[0] = 'a' + i;
    t ▷ add(c);
   }
  //t ▷ print;
  t ▷ free;
 }

void test3()                                                                    //Theight //Tfind //Troot
 {$ t = make$();

  for  (size_t i = 0; i < TEST_TREE_SIZE; ++i)
   {for(size_t j = 0; j < TEST_TREE_SIZE; ++j)
     {char c[4]; c[0] = '0' + i; c[1] = '0' + j;
      t ▷ add(c);
     }
   }

  $Node r = t ▷ root;
  assert(!strcmp(r ▷ key, "31"));
  assert(r ▷ height == 9);

  $Found f = t ▷ find("85");
  assert(!f.different && ({$Node F = f.last; F ▷ height;}) == 2);

  //t ▷ print;
  t ▷ free;
 }

void test4()
 {$ t = make$();

  for  (size_t i = TEST_TREE_SIZE; i > 0; --i)
   {for(size_t j = TEST_TREE_SIZE; j > 0; --j)
     {char c[4]; c[0] = '0' + i - 1; c[1] = '0' + j - 1;
      t ▷ add(c);
     }
   }
  $Node r = t ▷ root;
  assert(!strcmp(r ▷ key, "68"));
  assert(r ▷ height == 9);

  $Found f = t ▷ find("86");
  assert(!f.different && ({$Node F = f.last; F ▷ height;}) == 2);

//t ▷ print;
  t ▷ free;
 }

void test5()
 {$ t = make$();

  for  (size_t i = 0; i < TEST_TREE_SIZE;        ++i)
   {for(size_t j =        TEST_TREE_SIZE; j > 0; --j)
     {char c[4]; c[0] = '0' + i; c[1] = '0' + j - 1;
      t ▷ add(c);
     }
   }
  $Node r = t ▷ root;
  assert(!strcmp(r ▷ key, "17"));
  assert(r ▷ height == 13);

  $Found f = t ▷ find("86");
  assert(!f.different && ({$Node F = f.last; F ▷ height;}) == 8);

// t ▷ print;
  t ▷ free;
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0, test1, test2, test3, test4, test5, 0};
  run_tests("$", repetitions, tests);
  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree
