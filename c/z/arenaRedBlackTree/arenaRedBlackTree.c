//------------------------------------------------------------------------------
// Red black tree in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------

/* Each node of the base red black tree can, in turn, have a separate red black
tree dependent from it within the same arena*/

#define _GNU_SOURCE
#ifndef $_included
#define $_included
#include <stringBuffer.c>
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
 {for($Node p = root; p ▷ valid;)                                               // Search down through tree
   {found.last.offset  = p.offset;                                              // Record last parent compared with key sought
    k ◁ p ▷ key;                                                                // Key of current parent node
    i ◁ found.different = strcmp(found.key, k);                                 // Compare key sought with current key

    if (!i) return found;                                                       // Found

    p.offset = i < 0 ? p ▷ content->left.delta : p ▷ content->right.delta;      // Continue left or right
   }

  return found;                                                                 // Found
 }

static $Node locate_$Node_string                                                // Locate the node with the specified key if it exists within the tree owned by the specified node.
 (const $Node        node,                                                      // Node
  const char * const key)                                                       // Key to find
 {$Node p = node ▷ ownedTreeRoot;                                               // Root node
  if (!p ▷ valid) return p;                                                     // Empty tree
  for(;;)                                                                       // Search down through tree
   {i ◁ strcmp(key, p ▷ key);                                                   // Compare key sought with current key

    if (!i) return p;                                                           // Found

    p.offset = i < 0 ? p ▷ content->left.delta : p ▷ content->right.delta;      // Continue left or right
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
   {     i ◁ strcmp(key, p ▷ key);                                              // Compare key sought with current key
    if (!i) return p;                                                           // Found
    p.offset = i < 0 ? p ▷ content->left.delta : p ▷ content->right.delta;      // Continue left or right
    if (!p ▷ valid) return p;                                                   // Not found
   }

  printStackBackTrace("Should not be able to reach here\n");
 }

static void check_$                                                             // Check the integrity of the tree
 (const $ tree)                                                                 // Tree to check
 {void check($Node p)
   {if (!p ▷ valid) return;
    l ◁ p ▷ left; r ◁ p ▷ right;
    L ◁ l ▷ up;   R ◁ r ▷ up;
    if (l ▷ valid && !p ▷ equals(l ▷ up)) printStackBackTrace("Bad left  up %s to %s should be %s\n", l ▷ key, L ▷ key, p ▷ key);
    if (r ▷ valid && !p ▷ equals(r ▷ up)) printStackBackTrace("Bad right up %s to %s should be %s\n", r ▷ key, R ▷ key, p ▷ key);
    if (l ▷ valid && r ▷ valid)
     {lh ◁ l ▷ height; rh ◁ r ▷ height;
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
 {f ◁ make$Found(node.tree, key);                                               // Status of find
  r ◁ node ▷ ownedTreeRoot;                                                     // Root node of owned tree
  if (!r ▷ valid) return f;                                                     // Empty owned tree
  return f ▷ find(r);                                                           // Search the non empty owned tree starting at the specified node.
 }

static  $Found find_$Found_$_string                                             //P Find a key if it exists within the base tree.
 (const $      tree,                                                            // The base tree to search
  char * const key)                                                             // The key to find
 {f ◁ make$Found(tree, key);                                                    // Status of find
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
   {k ◁ va_arg(va, char *);
    if (!k) break;
    p.offset = p ▷ locate(k).offset;
   }
  va_end(va);
  return p;
 }

//D1 Add                                                                        // Add a new key if not already present in a base tree or a tree owned by a node.

static $Node add_$Node_$Found_$Node_string                                      // Add a new key if not already present in the tree root at the specified node.
 ($Found f,                                                                     // Found status for the key being added.
  $Node owner)                                                                  // Invalid - the base tree. Valid - the node that owns the tree being added to.
 {const $Node invalid = new $Node;                                                          // Return an invalid  node if the node already exists in the tree
  if (!f.different) return invalid;                                             // Key already present

   key ◁ f.key;                                                                 // The key to add
  tree ◁ f.last.tree;                                                           // The base tree
     n ◁ tree ▷ node(key); p ◁ f.last;                                          // Create new node under parent

  if (f.different < 0) p ▷ setLeft(n); else p ▷ setRight(n);                    // Insert node

  n ▷ setUp(p);                                                                 // Set parent of inserted node

  for($Node p = new $Node(tree: n.tree, offset: n.offset); p ▷ valid; p.offset = p ▷ up.offset)                                       // Balance nodes along path back to root
   {size_t height($Node p)                                                      // Height of a node
     {return p ▷ valid ? p ▷ height : 0;
     }

    void setHeight($Node p)                                                     // Set the height of a node
     {L ◁ height(p ▷ left);
      R ◁ height(p ▷ right);
      p ▷ setHeight(1 + (L > R ? L : R));
     }

    $Node balance                                                               // Balance node
     (const int d)                                                              // Rotate left if true
     {q ◁ p ▷ up; l ◁ p ▷ left; r ◁ p ▷ right; rl ◁ d ? r : l;

      if (!q ▷ valid)                                                           // Balance the root node
       {if (!owner ▷ valid) tree.arena->root = rl.offset;                       // New root node in base tree
        else owner ▷ setTree(rl);                                               // New root node in owned tree
        if (d)
         {p ▷ setRight   (r ▷ left);     r ▷ setLeft         (p);
          R ◁ p ▷ right; if (R ▷ valid) R ▷ setUp(p);
         }
        else
         {p ▷ setLeft    (l ▷ right);    l ▷ setRight        (p);
          L ◁ p ▷ left;  if (L ▷ valid) L ▷ setUp(p);
         }

        rl ▷ content->up.delta = 0;                                             // Nothing above this node as it is now the new root node
       }

      else                                                                      // Balance a node that is not the root node
       {rl ▷ setUp(q);
        if (p ▷ equals(q ▷ right)) q ▷ setRight(rl); else q ▷ setLeft(rl);
        if (d)
         {p ▷ setRight   (r ▷ left);
          R ◁ p ▷ right; if (R ▷ valid) R ▷ setUp(p);
          r ▷ setLeft(p);
         }
        else
         {p ▷ setLeft    (l ▷ right);
          L ◁ p ▷ left;  if (L ▷ valid) L ▷ setUp(p);
          l ▷ setRight(p);
         }
        setHeight(q);
       }

      p ▷ setUp(rl);
      setHeight(p);
      setHeight(rl);
      return rl;                                                                // The node now above the input node is now balanced
     }

    L ◁ height(p ▷ left); R ◁ height(p ▷ right);                                // Left and right depths
    if      (2*L + 1 < R) p.offset = balance(1).offset;                         // Balance left or right if necessary
    else if (2*R + 1 < L) p.offset = balance(0).offset;
    else     setHeight(p);                                                      // Balanced: propogate height
   }

  return n;                                                                     // The node added
 }

static  $Node   add_$Node_$_string                                              // Add a new key if not already present in the specified base tree.
 (const $       tree,                                                           // The tree to search
  char  * const key)                                                            // The key to add
 {f ◁ tree ▷ find(key);                                                         // Try to find the key

  if (!f.last ▷ valid)                                                          // Empty tree
   {root ◁ tree ▷ node(key);                                                    // Add new key in a node
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
       f ◁ node ▷ find(key);                                                    // Try to find the key
  if (!f.last ▷ valid)                                                          // Empty tree
   {root ◁ tree ▷ node(key);                                                    // Add new key in a node
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

static StringBuffer sprintRoot_string_root$Node                                 //P Print a tree, starting at the specified root node, as a stringBuffer.
 (const $Node root)                                                             // Root node of tree to be printed
 {const int W = 4;                                                              // Width of height field
  s ◁ makeStringBuffer();                                                       // Create the string buffer
  if (!root ▷ valid) return s;                                                  // The tree is empty, so the string buffer is too

  void print(const $Node node, size_t depth)                                    // Print to allocated string
   {if (!node ▷ valid) return;                                                  // No such node
    print(node ▷ left, depth+1);                                                // Print left
    for(size_t i = 0; i < depth * W; ++i) s ▷ add(" ");                         // Spacer
    s ▷ add(node ▷ key);                                                        // Key
    s ▷ addNewLine;                                                             // End of line
    print(node ▷ right, depth+1);                                               // Print right
   }

  print(root, 0);                                                               // Print to buffer
  return s;
 }

static StringBuffer sprint_string_$Node                                         // Print the tree owned by the specified node as a stringBuffer.
 (const $Node node)                                                             // Node owning tree to be printed
 {const $Node  root = node ▷ ownedTreeRoot;                                     // Root of tree to be printed
  return root ▷ sprintRoot;
 }

static StringBuffer sprint_string_$                                             // Print a tree as a stringBuffer;
 (const $ tree)                                                                 // Tree to print
 {root ◁ tree ▷ root;                                                           // Root of tree to print
  return root ▷ sprintRoot;
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

#define TEST_TREE_SIZE 10

void test0()                                                                    //Tnew$ //Tadd //Tleft //Tright //Tkey //Tup //Tfree
 {t ◁ make$();
  t ▷ add("b");
  t ▷ add("a");
  t ▷ add("c");
  t ▷ add("bb");
  t ▷ add("cc");

  b ◁ t ▷ root; a ◁ b ▷ left; c ◁ b ▷ right; bb ◁ c ▷ left; cc ◁ c ▷ right;

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
 {t ◁ make$();
  t ▷ add("a"); t ▷ add("b");
  a ◁ t ▷ root; b ◁ a ▷ right;

  b ▷ add("4"); b ▷ add("2"); b ▷ add("6");
  b ▷ add("1"); b ▷ add("3"); b ▷ add("5"); b ▷ add("7");

  b4 ◁ b  ▷ ownedTreeRoot; b2 ◁ b4 ▷ left; b6 ◁ b4 ▷ right;
  b1 ◁ b2 ▷ left; b3 ◁ b2 ▷ right; b5 ◁ b6 ▷ left; b7 ◁ b6 ▷ right;
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

         f ◁ t ▷ ll("b", "7", 0);
  assert(f.offset == b7.offset);

  t ▷ free;
 }

void test2()                                                                    //Tcount
 {size_t N = 100;
  t ◁ make$();

  for(size_t i = 0; i < N; ++i)
   {char c[256]; sprintf(c, "%lu", i);
    t ▷ add(c);
   }
  assert(t ▷ count == N);

  r   ◁ t ▷ root;
  n28 ◁ t ▷ locate("28"); n27 ◁ n28 ▷ up; n25 ◁ n27 ▷ up; n23 ◁ n25 ▷ up;

  assert(r   ▷ equalsString("3"));
  assert(r   ▷ height == 11);
  assert(n23 ▷ equalsString("23"));

  t ▷ check;
  t ▷ free;
 }

void test3()
 {int N = 256*256;
  t ◁ make$();

  for(int i = 0; i < N; ++i)
   {char c[256]; sprintf(c, "%x", i); t ▷ add(c);
   }
  assert(t ▷ count == (size_t)N);

  t ▷ check;

         r ◁ t ▷ root;
  assert(r ▷ equalsString("3"));
  assert(r ▷ height == 33);

  t ▷ free;
 }

void test4()                                                                    //Theight //Tlocate //Troot
 {t ◁ make$();
  n ◁ t ▷ locate("0"); assert(!n ▷ valid);                                      // Empty tree

  char c[4]; memset(c, 0, sizeof(c));

  for  (size_t i = 0; i < TEST_TREE_SIZE; ++i)
   {for(size_t j = 0; j < TEST_TREE_SIZE; ++j)
     {c[0] = '0' + i; c[1] = '0' + j;
      t ▷ add(c);
     }
   }

         r ◁ t ▷ root;
  assert(r ▷ equalsString("31"));
  assert(r ▷ height == 11);

         f ◁ t ▷ locate("85");
  assert(f ▷ valid);
  assert(f ▷ height == 2);

  t ▷ free;
 }

void test5()                                                                    //Tb2SumW8
 {size_t N = 20;
  t ◁ make$();
  char c[16]; memset(c, 0, sizeof(c));

  for(size_t i = 0; i < N; ++i)
   {sprintf(c, "%lu", i);
    n ◁ t ▷ add(c); e ◁ n ▷ locate("0"); assert(!e ▷ valid);                    // Empty owned tree
    for(size_t j = 0; j < N; ++j)
     {sprintf(c, "%lu-%lu", i, j);
      n ▷ add(c);
     }
   }

  const char *a[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 0};
  for(const char * const *p = a; *p; ++p)
   {n ◁ t ▷ locate(*p); assert(n ▷ equalsString(*p));
   }

  t ▷ free;
 }

void test6()
 {t ◁ make$();
  char c[4]; memset(c, 0, sizeof(c));

  for  (size_t i = 0; i < TEST_TREE_SIZE;        ++i)
   {for(size_t j =        TEST_TREE_SIZE; j > 0; --j)
     {c[0] = '0' + i; c[1] = '0' + j - 1;
      t ▷ add(c);
     }
   }
  r ◁ t ▷ root;
  assert(!strcmp(r ▷ key, "16"));
  assert(r ▷ height == 13);

   n86 ◁ t   ▷ locate("86");
   n76 ◁ n86 ▷ up; n66 ◁ n76 ▷ up;
   n56 ◁ n66 ▷ up; n46 ◁ n56 ▷ up;
  assert(n46 ▷ equalsString("46"));

  t ▷ free;
 }

void test7()                                                                    //Tcheck //Tsprint
 {t ◁ make$();
  const size_t N = 10;
  char c[128]; memset(c, 0, sizeof(c));

  for  (size_t i = 0; i < N; ++i)
   {sprintf(c, "%lu", i);
    t ▷ add(c);
    sprintf(c, "%lu", 2*N - i);
    t ▷ add(c);
   }
  t ▷ check;

  r ◁ t ▷ root;
  assert(!strcmp(r ▷ key, "19"));
  assert(r ▷ height == 6);

  t ▷ free;
 }

void test8()                                                                    //Tvalid
 {$Node n = new $Node; assert(!n ▷ valid);
 }

void test9()
 {t ◁ make$();
  char c[4]; memset(c, 0, sizeof(c));
  for  (size_t i = 0; i < 2; ++i)
   {sprintf(c, "%lu", i);
           n ◁ t ▷ add(c);
    assert(n ▷ valid);
    assert(n ▷ equalsString(c));
   }
  assert(t ▷ count == 2);
  t ▷ check;
  t ▷ free;
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0, test1, test2, test3, test4,
                           test5, test6, test7, test8, test9, 0};
//void (*tests[])(void) = {test5, 0};
  run_tests("$", repetitions, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree
