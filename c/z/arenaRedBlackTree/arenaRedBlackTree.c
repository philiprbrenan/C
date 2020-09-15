//------------------------------------------------------------------------------
// Red black tree in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------

/* Each node of the base red black tree can, in turn, have a separate red black
tree dependent from it with the same arena*/

#define _GNU_SOURCE
#ifndef $_included
#define $_included
#include <readOnlyBytes.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Red Black Tree.
typedef char * $String;                                                         // Arena Tree string

struct $Found;                                                                  // Forward declare some of the structures used
struct $Node;

typedef struct $                                                                // Arena Tree.
 {const struct ProtoTypes_$ *proto;                                             // Methods associated with an arena tree
  struct $Arena *arena;                                                         // The arena for the arena tree
  int (*different)(const struct $Found *f, const struct $Node *n);
 } $;

include ../arenaTree/arenaTree.c :structs !$ !$Content

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
  $Node last;                                                                   // Last node found during find.
  int   different;                                                              // The compare result on the last node found. If zero, the last node found was a match
 } $Found;

#include <$$_prototypes.h>                                                      // Arena tree prototypes now that the relevant structures have been declared

//D1 Pointers, offsets and allocations                                          // Locate items allocated in the arena

include ../arenaTree/arenaTree.c :arena

static void free_$                                                              // Free a tree in its entirety
 (const $ tree)                                                                 // Tree
 {free(tree.arena->data);
  free(tree.arena);
 }

static  $Found make$Found                                                       //P Create a new found description
 (const $      tree,                                                            // The tree being searched
  char * const key)                                                             // The key to find
 {const $Node n = new $Node(tree: tree);                                        // Sample node within tree
  return new$Found(tree: tree, key: key, last: n);
 }

//D1 Attributes                                                                 // Get and set the attributes of a node

static $Node root_$                                                             // The root node in a red black tree
 (const $ tree)                                                                 // Tree
 {return tree ▷ nodeFromOffset(tree.arena->root);
 }

static size_t height_$Node                                                      // Height of a sub tree starting at the specified node
 (const $Node node)                                                             // Node
 {return node ▷ content->height.delta;
 }

static char *key_$Node                                                          // Key of the specified node as a zero terminated string
 (const $Node node)                                                             // Node
 {return node.tree ▷ pointer(node ▷ content->key.delta);
 }

static $Node up_$Node_$Node                                                     // Parent node for the specified node.
 (const $Node node)                                                             // Node
 {return node.tree ▷ nodeFromOffset(node ▷ content->up.delta);
 }

static $Node left_$Node_$Node                                                   // Left child node below the specified parent node.
 (const $Node     parent)                                                       // Parent
 {return parent.tree ▷ nodeFromOffset(parent ▷ content->left.delta);
 }

static $Node right_$Node_$Node                                                  // Right child node below the specified parent node.
 (const $Node     parent)                                                       // Parent
 {return  parent.tree ▷ nodeFromOffset(parent ▷ content->right.delta);
 }

static $Node ownedTreeRoot_$Node_$Node                                          //P Root of tree owned by this node if there is  one, else the returned node has an offset of zero.
 (const $Node     parent)                                                       // Parent
 {return parent.tree ▷ nodeFromOffset(parent ▷ content->tree.delta);
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
  const $Node tree)                                                             // Tree to be added to parent
 {parent ▷ content->tree.delta = tree.offset;
 }

static int valid_$Node                                                          // Check that the specified node is valid.
 (const $Node node)                                                             // Node
 {return node.offset;                                                           // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

//D1 Find                                                                       // Find a key if it exists within the base tree or an owned tree.

static $Found find_$Found_$Found_$Node_string                                   //P Find a key if it exists within the tree starting at this node.
 ($Found      found,                                                            // Found definition
  const $Node root)                                                             // The root node at which the tree starts
 {const $ tree = root.tree;                                                     // Tree containing node
  const char * const key = found.key;                                           // The key to find

  for($Node p   = root; p ▷ valid;)                                             // Search down through tree
   {found.last  = p;                                                            // Record last parent compared with key sought
    const char * const k = p ▷ key;                                             // Key of current parent node
    const int i = found.different = strcmp(key, k);                             // Compare key sought with current key

    if (!i) return found;                                                       // Found

    $Content * const c = p ▷ content;                                           // Continue
    p = tree ▷ nodeFromOffset(i < 0 ? c->left.delta : c->right.delta);          // Continue left or right
   }

  return found;                                                                 // Found
 }

static $Node locate_$Node_string                                                // Locate the node with the specified key if it exists within the tree owned by the specified node.
 (const $Node        node,                                                      // Node
  const char * const key)                                                       // Key to find
 {const $ tree = node.tree;                                                     // $ containing node
  $Node p = node ▷ ownedTreeRoot;                                               // Root node
  if (!p ▷ valid) return p;                                                     // Empty tree
  for(;;)                                                                       // Search down through tree
   {const char * const k = p ▷ key;                                             // Key of current parent node
    const int i = strcmp(key, k);                                               // Compare key sought with current key

    if (!i) return p;                                                           // Found

    $Content * const c = p ▷ content;                                           // Continue
    p = tree ▷ nodeFromOffset(i < 0 ? c->left.delta : c->right.delta);          // Continue left or right
    if (!p ▷ valid) return p;                                                   // Not found
   }

  printStackBackTrace("Should not be able to reach here\n");
 }

static $Node locate_$_string                                                    // Locate the node with the specified key if it exists within the specified tree.
 (const $            tree,                                                      // $
  const char * const key)                                                       // Key to find
 {$Node p = tree ▷ root;                                                        // Root node
  if (!p ▷ valid) return p;                                                     // Empty tree
  for(;;)                                                                       // Search down through tree
   {const char * const k = p ▷ key;                                             // Key of current parent node
    const int i = strcmp(key, k);                                               // Compare key sought with current key

    if (!i) return p;                                                           // Found

    $Content * const c = p ▷ content;                                           // Continue
    p = tree ▷ nodeFromOffset(i < 0 ? c->left.delta : c->right.delta);          // Continue left or right
    if (!p ▷ valid) return p;                                                   // Not found
   }

  printStackBackTrace("Should not be able to reach here\n");
 }

static void check_$                                                             // Check the integrity of the tree
 (const $ tree)                                                                 // Tree to check
 {void check($Node p)
   {if (!p ▷ valid) return;
    const $Node l = p ▷ left, r = p ▷ right;
    const $Node L = l ▷ up,   R = r ▷ up;
    if (l ▷ valid && !p ▷ equals(l ▷ up)) printStackBackTrace("Bad left  up %s to %s should be %s\n", l ▷ key, L ▷ key, p ▷ key);
    if (r ▷ valid && !p ▷ equals(r ▷ up)) printStackBackTrace("Bad right up %s to %s should be %s\n", r ▷ key, R ▷ key, p ▷ key);
    if (l ▷ valid && r ▷ valid)
     {const size_t lh = l ▷ height, rh = r ▷ height;
      if (lh > 2 * rh + 1 || rh > 2 * lh + 1 ) printStackBackTrace("Unbalanced tree at %s with left %lu versus right %lu\n", p ▷ key, lh, rh);
     }
    check(l);
    check(r);
   }
  check(tree ▷ root);
 }

static  $Found find_$Found_$Node_string                                         //P Find a key if it exists within the tree owned by the specified node.
 (const $Node  node,                                                            // The node owning the tree to search
  char * const key)                                                             // The key to find
 {const $Found f = make$Found(node.tree, key);                                  // Status of find
  const $Node  r = node ▷ ownedTreeRoot;                                        // Root node of owned tree
  if (!r ▷ valid) return f;                                                     // Empty owned tree
  return f ▷ find(r);                                                           // Search the non empty owned tree starting at the specified node.
 }

static  $Found find_$Found_$_string                                             //P Find a key if it exists within the base tree.
 (const $      tree,                                                            // The base tree to search
  char * const key)                                                             // The key to find
 {const $Found f = make$Found(tree, key);                                       // Status of find
  if (!tree.arena->root) return f;                                              // Empty tree
  return f ▷ find(tree ▷ nodeFromOffset(tree.arena->root));                     // Search the non empty base tree starting at the specified node.
 }

static $Node ll_$Node_$_strings                                                 // Search through a series of owned trees starting at the base tree as directed by the specified keys
 (const $   tree,                                                               // The base tree to search from
  const char * const keys,                                                      // Zero terminated list of keys
  ...)                                                                          // Following keys
 {va_list va;
  va_start(va, keys);
  $Node p = tree ▷ locate(keys);
  for(;p ▷ valid;)
   {const char * const k = va_arg(va, char *);
    if (!k) break;
    p = p ▷ locate(k);
   }
  va_end(va);
  return p;
 }

//D1 Add                                                                        // Add a new key if not already present in a base tree or a tree owned by a node.

static $Node add_$Node_$Found_$Node_string                                      // Add a new key if not already present in the tree root at the specified node.
 ($Found f,                                                                     // Found status for the key being added.
  $Node owner)                                                                  // Invalid - the base tree. Valid - the node that owns the tree being added to.
 {const $Node invalid = new $Node;                                              // Return an invalid  node if the node already exists in the tree
  if (!f.different) return invalid;                                             // Key already present

  const char * const key = f.key;                                               // The key to add
  const $           tree = f.last.tree;                                         // The base tree
  const $Node          n = tree ▷ node(key), p = f.last;                        // Create new node under parent

  if (f.different < 0) p ▷ setLeft(n); else p ▷ setRight(n);                    // Insert node

  n ▷ setUp(p);                                                                 // Set parent of inserted node

//?for(; p ▷ valid; p = p ▷ up)                                                 // Balance nodes along path back to root
  for($Node p = n; p ▷ valid; p = p ▷ up)                                       // Balance nodes along path back to root
   {size_t height($Node p)                                                      // Height of a node
     {return p ▷ valid ? p ▷ height : 0;
     }

    void setHeight($Node p)                                                     // Set the height of a node
     {const int L = height(p ▷ left), R = height(p ▷ right);
      p ▷ setHeight(1 + (L > R ? L : R));
     }

    $Node balance                                                               // Balance node
     (const int d)                                                              // Rotate left if true
     {const $Node q = p ▷ up, l = p ▷ left, r = p ▷ right, rl = d ? r : l;

      if (!q ▷ valid)                                                           // Balance the root node
       {if (!owner ▷ valid) tree.arena->root = rl.offset;                       // New root node in base tree
        else owner ▷ setTree(rl);                                               // New root node in owned tree
        if (d)
         {p ▷ setRight   (r ▷ left);     r ▷ setLeft         (p);
          const $Node R = p ▷ right; if (R ▷ valid) R ▷ setUp(p);
         }
        else
         {p ▷ setLeft    (l ▷ right);    l ▷ setRight        (p);
          const $Node L = p ▷ left;  if (L ▷ valid) L ▷ setUp(p);
         }

        rl ▷ content->up.delta = 0;                                             // Nothing above this node as it is now the new root node
       }

      else                                                                      // Balance a node that is not the root node
       {rl ▷ setUp(q);
        if (p ▷ equals(q ▷ right)) q ▷ setRight(rl); else q ▷ setLeft(rl);
        if (d)
         {p ▷ setRight   (r ▷ left);
          const $Node R = p ▷ right; if (R ▷ valid) R ▷ setUp(p);
          r ▷ setLeft(p);
         }
        else
         {p ▷ setLeft    (l ▷ right);
          const $Node L = p ▷ left;  if (L ▷ valid) L ▷ setUp(p);
          l ▷ setRight(p);
         }
        setHeight(q);
       }

      p ▷ setUp(rl);
      setHeight(p);
      setHeight(rl);
      return rl;                                                                // The node now above the input node is now balanced
     }

    const int L = height(p ▷ left), R = height(p ▷ right);                      // Left and right depths
    if      (2*L + 1 < R) p = balance(1);                                       // Balance left or right if necessary
    else if (2*R + 1 < L) p = balance(0);
    else     setHeight(p);                                                      // Balanced: propogate height
   }

  return n;                                                                     // The node added
 }

static  $Node   add_$Node_$_string                                              // Add a new key if not already present in the specified base tree.
 (const $       tree,                                                           // The tree to search
  char  * const key)                                                            // The key to add
 {$Found f =    tree ▷ find(key);                                               // Try to find the key

  if (!f.last ▷ valid)                                                          // Empty tree
   {const $Node root = tree ▷ node(key);                                        // Add new key in a node
    tree.arena->root = root.offset;                                             // Make the new node the root node
    return root;                                                                // Create node
   }
  const $Node invalid = new $Node;
  return f ▷ add(invalid);                                                      // Non empty tree - add the node below the root node
 }

static  $Node   add_$Node_$Node_string                                          // Add a new key if not already present in the tree owned by the specified node.
 (const $Node   node,                                                           // The tree to search
  char  * const key)                                                            // The key to add
 {const $ tree  = node.tree;                                                    // Tree to search
  $Found f = node ▷ find(key);                                                  // Try to find the key
  if (!f.last ▷ valid)                                                          // Empty tree
   {const $Node root = tree ▷ node(key);                                        // Add new key in a node
    node ▷ setTree(root);                                                       // Make the new node the root node
    return root;                                                                // Create node
   }
  return f ▷ add(node);                                                         // Non empty tree - add the node below the root node
 }

//D1 Traverse                                                                   // Traverse the $

static  size_t count_size_$                                                     // Count the number of nodes in a $
 (const $ tree)                                                                 // $
 {size_t l = 0;
  void count(const $Node parent)                                                // Count the nodes in a sub tree
   {if (!parent ▷ valid) return;
    l++; count(parent ▷ left); count(parent ▷ right);
   }
  count(tree ▷ root);                                                           // Start at the root
  return l;                                                                     // Return count
 }

//D1 Print                                                                      // Print a tree.

static ReadOnlyBytes sprintRoot_string_root$Node                                //P Print a tree, starting at the specified root node, as a string. We allocate a string large enough to hold the print. Freeing the allocated string containing the print is the responsibility fo the caller.
 (const $Node root)                                                             // Root node of tree to be printed
 {const int W = 4;                                                              // Width of height field
  if (!root ▷ valid) return makeReadOnlyBytesFromString("");                    // The tree is empty

  size_t l = 1;                                                                 // Length of buffer needed - terminating 0
  void len($Node node, size_t depth)                                            // Get length of print
   {if (node ▷ valid)
     {len(node ▷ left, depth+1);
      l += 1 + depth * W + strlen(node ▷ key);                                  // new line, spacing, key
      len(node ▷ right, depth+1);
     }
   }
  len(root, 0);                                                                 // Set size of print

  char data[l+1], *p = data;                                                    // Output area

  void print(const $Node node, size_t depth)                                    // Print to allocated string
   {if (!node ▷ valid) return;                                                  // No such node
    print(node ▷ left, depth+1);                                                // Print left
    for(size_t i = 0; i < depth * W; ++i) p = stpcpy(p, " ");                   // Spacer
    p = stpcpy(p, node ▷ key);                                                  // Key
    p = stpcpy(p, "\n");                                                        // End of line
    print(node ▷ right, depth+1);                                               // Print right
   }

  print(root, 0);                                                               // Print to buffer
  const ReadOnlyBytes r = makeReadOnlyBytesDupN(data, l);                       // Create read only bytes
  return r;                                                                     // Please free the returned string when done.
 }

static ReadOnlyBytes sprint_string_$Node                                        // Print the tree owned by the specified node as ReadOnlyBytes.
 (const $Node node)                                                             // Node owning tree to be printed
 {const $Node  root = node ▷ ownedTreeRoot;                                     // Root of tree to be printed
  return root ▷ sprintRoot;
 }

static ReadOnlyBytes sprint_string_$                                            // Print a tree  as ReadOnlyBytes.
 (const $ tree)                                                                 // Tree to print
 {const $Node root = tree ▷ root;                                               // Root of tree to print
  return root ▷ sprintRoot;
 }
// Marked with I to prevent tests needed.
static void print_$Node                                                         //I Print the tree owned by the specified node to stderr.
 (const $Node node)                                                             // Node owning tree to be printed
 {const ReadOnlyBytes p = node ▷ sprint;
  fprintf(stderr, "%s\n", p.data);
  p ▷ free;
 }

static void print_$                                                             //I Print the specified base tree to stderr.
 (const $ tree)                                                                 // Tree
 {const ReadOnlyBytes p = tree ▷ sprint;
  fprintf(stderr, "%s\n", p.data);
  p ▷ free;
 }

static size_t b2SumW8_size_$                                                    // Digest for a tree
 (const $ tree)                                                                 // Tree
 {const ReadOnlyBytes s = tree ▷ sprint;
  const size_t d = s ▷ b2SumW8;
  s ▷ free;
  return d;
 }

static size_t b2SumW8_size_$Node                                                // Digest for the tree owned by a node
 (const $Node node)                                                             // Owning node
 {const ReadOnlyBytes s = node ▷ sprint;
  const size_t d = s ▷ b2SumW8;
  s ▷ free;
  return d;
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

#define TEST_TREE_SIZE 10

void test0()                                                                    //Tnew$ //Tadd //Tleft //Tright //Tkey //Tup //Tfree
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

  assert(!t ▷ find("cc").different);
  assert( t ▷ find("dd").different);

  t ▷ free;
 }

void test1()                                                                    //Tll
 {$ t = make$();
    t ▷ add("a"); t ▷ add("b");
  $Node a = t ▷ root, b = a ▷ right;

  b ▷ add("4"); b ▷ add("2"); b ▷ add("6");
  b ▷ add("1"); b ▷ add("3"); b ▷ add("5"); b ▷ add("7");

  $Node b4 = b  ▷ ownedTreeRoot, b2 = b4 ▷ left, b6 = b4 ▷ right,
        b1 = b2 ▷ left, b3 = b2 ▷ right, b5 = b6 ▷ left, b7 = b6 ▷ right;
  assert(b4 ▷ equalsString("4"));
  assert(b2 ▷ equalsString("2"));
  assert(b6 ▷ equalsString("6"));
  assert(b1 ▷ equalsString("1"));
  assert(b3 ▷ equalsString("3"));
  assert(b5 ▷ equalsString("5"));
  assert(b7 ▷ equalsString("7"));
  assert(b4 ▷ up.offset == 0);
  assert(b2 ▷ up.offset == b4.offset);
  assert(b6 ▷ up.offset == b4.offset);
  assert(b1 ▷ up.offset == b2.offset);
  assert(b3 ▷ up.offset == b2.offset);
  assert(b5 ▷ up.offset == b6.offset);
  assert(b7 ▷ up.offset == b6.offset);

  assert(!b ▷ find("7").different);
  assert( b ▷ find("8").different);

  $Node f = t ▷ ll("b", "7", 0);
  assert(f.offset == b7.offset);

  t ▷ free;
 }

void test2()                                                                    //Tcount
 {size_t N = 100;
  $ t = make$();

  for(size_t i = 0; i < N; ++i)
   {char c[256]; sprintf(c, "%lu", i);
    t ▷ add(c);
   }
  assert(t ▷ count == N);

  $Node r = t ▷ root,
      n28 = t ▷ locate("28"), n27 = n28 ▷ up, n25 = n27 ▷ up, n23 = n25 ▷ up;

  assert(r   ▷ equalsString("3"));
  assert(r   ▷ height == 11);
  assert(n23 ▷ equalsString("23"));

  t ▷ check;
  t ▷ free;
 }

void test3()
 {int N = 256*256;
  $ t = make$();

  for  (int i = 0; i < N; ++i)
   {char c[256]; sprintf(c, "%x", i);
    t ▷ add(c);
   }
  assert(t ▷ count == (size_t)N);

  t ▷ check;

  $Node r = t ▷ root;
  assert(r   ▷ equalsString("3"));
  assert(r   ▷ height == 33);

  t ▷ free;
 }

void test4()                                                                    //Theight //Tlocate //Troot
 {$ t = make$();
  $Node n = t ▷ locate("0"); assert(!n ▷ valid);                                // Empty tree

  char c[4]; memset(c, 0, sizeof(c));

  for  (size_t i = 0; i < TEST_TREE_SIZE; ++i)
   {for(size_t j = 0; j < TEST_TREE_SIZE; ++j)
     {c[0] = '0' + i; c[1] = '0' + j;
      t ▷ add(c);
     }
   }

  $Node r = t ▷ root;
  assert(r ▷ equalsString("31"));
  assert(r ▷ height == 11);

  $Node f = t ▷ locate("85");
  assert(f ▷ valid);
  assert(f ▷ height == 2);

  t ▷ free;
 }

void test5()                                                                    //Tb2SumW8
 {size_t N = 20;
  $ t = make$();
  char c[16]; memset(c, 0, sizeof(c));

  for(size_t i = 0; i < N; ++i)
   {sprintf(c, "%lu", i);
    const $Node n = t ▷ add(c), e = n ▷ locate("0"); assert(!e ▷ valid);        // Empty owned tree
    for(size_t j = 0; j < N; ++j)
     {sprintf(c, "%lu-%lu", i, j);
      n ▷ add(c);
     }
   }

  assert(t ▷ b2SumW8 == 9);
  $Node n0 = t ▷ locate("0"); assert(n0 ▷ b2SumW8 ==  70);
  $Node n1 = t ▷ locate("1"); assert(n1 ▷ b2SumW8 ==   9);
  $Node n2 = t ▷ locate("2"); assert(n2 ▷ b2SumW8 ==  90);
  $Node n3 = t ▷ locate("3"); assert(n3 ▷ b2SumW8 ==  56);
  $Node n4 = t ▷ locate("4"); assert(n4 ▷ b2SumW8 == 250);
  $Node n5 = t ▷ locate("5"); assert(n5 ▷ b2SumW8 == 172);
  $Node n6 = t ▷ locate("6"); assert(n6 ▷ b2SumW8 == 212);
  $Node n7 = t ▷ locate("7"); assert(n7 ▷ b2SumW8 == 115);
  $Node n8 = t ▷ locate("8"); assert(n8 ▷ b2SumW8 == 200);
  $Node n9 = t ▷ locate("9"); assert(n9 ▷ b2SumW8 ==  81);

  t ▷ free;
 }

void test6()
 {$ t = make$();
  char c[4]; memset(c, 0, sizeof(c));

  for  (size_t i = 0; i < TEST_TREE_SIZE;        ++i)
   {for(size_t j =        TEST_TREE_SIZE; j > 0; --j)
     {c[0] = '0' + i; c[1] = '0' + j - 1;
      t ▷ add(c);
     }
   }
  $Node r = t ▷ root;
  assert(!strcmp(r ▷ key, "16"));
  assert(r ▷ height == 13);

  $Node n86 = t ▷ locate("86"),
        n76 = n86 ▷ up, n66 = n76 ▷ up, n56 = n66 ▷ up, n46 = n56 ▷ up;
  assert(n46 ▷ equalsString("46"));

  t ▷ free;
 }

void test7()                                                                    //Tcheck //Tsprint
 {$ t = make$();
  const size_t N = 10;
  char c[128]; memset(c, 0, sizeof(c));

  for  (size_t i = 0; i < N; ++i)
   {sprintf(c, "%lu", i);
    t ▷ add(c);
    sprintf(c, "%lu", 2*N - i);
    t ▷ add(c);
   }
  t ▷ check;

  $Node r = t ▷ root;
  assert(!strcmp(r ▷ key, "19"));
  assert(r ▷ height == 6);
  assert(t ▷ b2SumW8 == 223);

  t ▷ free;
 }

void test8()                                                                    //Tvalid
 {$Node n = new $Node; assert(!n ▷ valid);
 }

void test9()
 {$ t = make$();
  char c[4]; memset(c, 0, sizeof(c));
  for  (size_t i = 0; i < 2; ++i)
   {sprintf(c, "%lu", i);
    $Node n = t ▷ add(c);
    assert(n ▷ valid);
    assert(n ▷ equalsString(c));
   }
  assert(t ▷ count == 2);
  t ▷ check;
  t ▷ free;
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0, test1, test2, test3, test4, test5, test6, test7, test8, test9, 0};
//void (*tests[])(void) = {test5, 0};
  run_tests("$", repetitions, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree
