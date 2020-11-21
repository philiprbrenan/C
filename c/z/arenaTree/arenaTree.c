//------------------------------------------------------------------------------
// Trees of trees held in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
/*
 Trees of trees. Each tree is represented by a red black tree. The ordering of
 the nodes in each tree is determined by a binary comparison of the key fields.
 The length of each key field is specified by the length field. Each node of
 each tree can, in turn, be the root of another tree held in the same arena.
*/
#define _GNU_SOURCE
#ifndef $_included
#define $_included
#include <stringBuffer.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an $.

typedef struct $                                                                // $
 {const struct ProtoTypes_$ *proto;                                             // Methods associated with an $
  struct $Arena *arena;                                                         // The arena for the $
 } $;

include ../arenaList/arenaList.c :structs !$ !$Content

typedef struct $Content                                                         // Content of a node in a $
 {unsigned int left, right, up, height, tree, length;                           // Location of left, right, parent trees, sub tree height, owned sub tree, length of key.
  char key [0];                                                                 // Variable length key whose current length is in the length field
 } $Content;

typedef struct $Found                                                           // The result of trying to find a node in the $.
 {const struct ProtoTypes_$Found *proto;                                        // Methods
  char  * key;                                                                  // Key to find
  size_t  length;                                                               // Length of key to find
  $       tree;                                                                 // $ being processed
  $Node   last;                                                                 // Last node found during find.
  int     different;                                                            // The compare result on the last node found. If zero, the last node found was a match
 } $Found;

#include <$$_prototypes.h>                                                      // $ prototypes now that the relevant structures have been declared
#define makeLocalCopyOf$Key(string,stringLength,node) stringLength ◁ content__ArenaTreeNode(&node)->length; char string[stringLength+1]; string[stringLength] = 0; memcpy(string, key_pointer__ArenaTreeNode(&node), stringLength); // Copy the key and the length of the key of the specified node to the stack.

//D1 Pointers, offsets and allocations                                          // Locate items allocated in the arena

include ../arenaList/arenaList.c :arena :nodeData

static void free_$                                                              // Free a $ in its entirety
 ($ * tree)                                                                     // $
 {free(tree->arena->data);
  free(tree->arena);
 }

static  $Found make$Found                                                       //P Create a new found description
 (const $      tree,                                                            // The $ being searched
  char * const key,                                                             // The key to find
  const size_t length)                                                          // Length of the key to find
 {const $Node n = new $Node(list: tree);                                        // Sample node within $
  return new$Found(tree: tree, key: key, length: length, last: n);
 }

//D1 Attributes                                                                 // Get and set the attributes of a node

static $Node root_$                                                             // The root node in a $
 (const $ * tree)                                                               // $
 {return tree ▶ offset(tree->arena->root);
 }

static size_t height_$Node                                                      // Height of a sub $ starting at the specified node
 (const $Node * node)                                                           // Node
 {return node ▶ content->height;
 }

static int keyEquals_$Node_string_size                                          // Key of the specified node as a zero terminated string
 (const $Node *       node,                                                     // Node
  const char  * const key,                                                      // Expected key value
  const size_t       length)                                                    // Length of expected key value
 {l ◁ node ▶ length;                                                            // Recorded length of node
  if (l != length) return 0;                                                    // Different lengths means the keys is unequal
  return !memcmp(node ▶ key, key, l);                                           // Compare memory
 }

static $Node up_$Node_$Node                                                     // Parent node for the specified node.
 (const $Node * node)                                                           // Node
 {return node->list ▷ offset(node ▶ content->up);
 }

static $Node left_$Node_$Node                                                   // Left child node below the specified parent node.
 (const $Node * parent)                                                         // Parent
 {return parent->list ▷ offset(parent ▶ content->left);
 }

static $Node right_$Node_$Node                                                  // Right child node below the specified parent node.
 (const $Node * parent)                                                         // Parent
 {return parent->list ▷ offset(parent ▶ content->right);
 }

static $Node ownedTreeRoot_$Node_$Node                                          //P Root of $ owned by this node if there is  one, else the returned node has an offset of zero.
 (const $Node * parent)                                                         // Parent
 {return parent->list ▷ offset(parent ▶ content->tree);
 }

static void setHeight_$Node                                                     //P Save the height of the sub $ starting at the specified node
 (const $Node * parent,                                                         // Parent
  const size_t height)                                                          // Height
 {parent ▶ content->height = height;
 }

static void setUp_$Node_$Node                                                   //P Set parent node of specified child node
 (const $Node * child,                                                          // Parent
  const $Node parent)                                                           // Child
 {child ▶ content->up = parent.offset;
 }

static void setLeft_$Node_$Node                                                 //P Set left child of specified parent.
 (const $Node * parent,                                                         // Parent
  const $Node left)                                                             // Left child
 {parent ▶ content->left = left.offset;
 }

static void setRight_$Node_$Node                                                //P Parent parent for a specified parent in a $
 (const $Node * parent,                                                         // Parent
  const $Node right)                                                            // Right child
 {parent ▶ content->right = right.offset;
 }

static void setTree_$Node_$Node                                                 //P Set the other $ located by this node.
 (const $Node * parent,                                                         // Parent
  const $Node tree)                                                             // $ to be added to parent
 {parent ▶ content->tree = tree.offset;
 }

static int valid_$Node                                                          // Check that the specified node is valid.
 (const $Node * node)                                                           // Node
 {return node->offset;                                                          // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

static int isRoot_$Node                                                         // Check that the specified node is valid.
 (const $Node * node)                                                           // Node
 {return !(node ▶ up).offset;                                                   // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

//D1 Find                                                                       // Find a key if it exists within the base $ or an owned $.

static int cmp$                                                                 // Compare key sought with current key
 (const char * const K,                                                         // Key sought
  const size_t       L,                                                         // key length
  const char * const k,                                                         // Current key
  const size_t       l)                                                         // Current key length
 {if (l < L)                                                                    // Current key shorter than sought key
   {const int i = strncmp(K, k, l);
    return i ? i : +1;
   }
  if (L < l)                                                                    // Current key longer than sought key
   {const int i = strncmp(K, k, L);
    return i ? i : -1;
   }
  return strncmp(K, k, L);                                                      // Equal length keys
 }

static $Found find_$Found_$Found_$Node_string                                   //P Find a key if it exists within the $ starting at this node.
 ($Found    * found,                                                            // Found definition
  const $Node root)                                                             // The root node at which the $ starts
 {for($Node p = root; p ▷ valid;)                                               // Search down through $
   {found->last.offset  = p.offset;                                             // Record last parent compared with key sought
    makeLocalCopyOf$Key(k, l, p);                                               // Local copy of key

    L ◁ found->length; K ◁ found->key;
    i ◁ found->different = cmp$(K, L, k, l);                                    // Compare key sought with current key

    if (!i) return *found;                                                      // Found

    p.offset = i < 0 ? p ▷ content->left : p ▷ content->right;                  // Continue left or right
   }

  return *found;                                                                // Found
 }

static $Node locate_$Node_string                                                // Locate the node with the specified key if it exists within the $ owned by the specified node.
 (const $Node *       node,                                                     // Node
  const char  * const key,                                                      // Key to find
  const size_t        length)                                                   // Length of the key to find
 {$Node p = node ▶ ownedTreeRoot;                                               // Root node
  if (!p ▷ valid) return p;                                                     // Empty $
  for(;;)                                                                       // Search down through $
   {makeLocalCopyOf$Key(k, l, p);
    i ◁ cmp$(key, length, k, l);                                                // Compare key sought with current key

    if (!i) return p;                                                           // Found

    p.offset = i < 0 ? p ▷ content->left : p ▷ content->right;                  // Continue left or right
    if (!p ▷ valid) return p;                                                   // Not found
   }

  printStackBackTrace("Should not be able to reach here\n");
 }

static $Node locate_$_string                                                    // Locate the node with the specified key if it exists within the specified $.
 (const $    *       tree,                                                      // $
  const char * const key,                                                       // Key to find
  const size_t       length)                                                    // Length of the key to find
 {$Node p = tree ▶ root;                                                        // Root node
  if (!p ▷ valid) return p;                                                     // Empty $
  for(;;)                                                                       // Search down through $
   {makeLocalCopyOf$Key (k, l, p);
    i ◁ cmp$(key, length, k, l);                                                // Compare key sought with current key
    if (!i) return p;                                                           // Found
    p.offset = i < 0 ? p ▷ content->left : p ▷ content->right;                  // Continue left or right
    if (!p ▷ valid) return p;                                                   // Not found
   }

  printStackBackTrace("Should not be able to reach here\n");
 }

static int check_$                                                              // Check the integrity of the $
 (const $ * tree)                                                               // $ to check
 {void check($Node p)                                                           // Check a node
   {if (!p ▷ valid) return;
                   makeLocalCopyOf$Key(pk, pl, p);
    l ◁ p ▷ left;  makeLocalCopyOf$Key(lk, ll, l);
    r ◁ p ▷ right; makeLocalCopyOf$Key(rk, rl, r);
    L ◁ l ▷ up;    makeLocalCopyOf$Key(Lk, Ll, L);
    R ◁ r ▷ up;    makeLocalCopyOf$Key(Rk, Rl, R);
    if (l ▷ valid && !p ▷ equals(l ▷ up)) printStackBackTraceAndExit
        (1, "Bad left  up %s to %s should be %s\n", lk, Lk, pk);
    if (r ▷ valid && !p ▷ equals(r ▷ up)) printStackBackTraceAndExit
        (2, "Bad right up %s to %s should be %s\n", rk, Rk, pk);
    if (l ▷ valid && r ▷ valid)
     {lh ◁ l ▷ height; rh ◁ r ▷ height;
      if (lh > 2 * rh + 1 || rh > 2 * lh + 1 ) printStackBackTraceAndExit
        (3, "Unbalanced $ at %s with left %lu versus right %lu\n", pk, lh, rh);
     }
    check(l);                                                                   // Check left
    check(r);                                                                   // Check right
   }
  check(tree ▶ root);                                                           // Check from root
  return 1;                                                                     // If we get this far the tree is ok
 }

static  $Found find_$Found_$Node_string                                         //P Find a key if it exists within the $ owned by the specified node.
 (const $Node * Node,                                                           // The node owning the $ to search
  char * const  key,                                                            // The key to find
  const size_t  length)                                                         // Length of the key to find
 {node ◁ *Node;
  f ◀ make$Found(node.list, key, length);                                       // Status of find
  r ◁ node ▷ ownedTreeRoot;                                                     // Root node of owned $
  if (!r ▷ valid) return f;                                                     // Empty owned $
  return f ▷ find(r);                                                           // Search the non empty owned $ starting at the specified node.
 }

static  $Found find_$Found_$_string                                             //P Find a key if it exists within the base $.
 ($    *       tree,                                                            // The base $ to search
  char * const key,                                                             // The key to find
  const size_t length)                                                          // Length of the key to find
 {f ◀ make$Found(*tree, key, length);                                           // Status of find
  if (!tree->arena->root) return f;                                             // Empty $
  return f ▷ find(tree ▶ offset(tree->arena->root));                            // Search the non empty base $ starting at the specified node.
 }

static $Node ll_$Node_$_strings                                                 // Search through a series of owned trees starting at the base $ as directed by the specified keys
 (const $    *       tree,                                                      // The base $ to search from
  const char * const keys,                                                      // Zero terminated list of keys
  ...)                                                                          // Following keys
 {va_list va;
  va_start(va, keys);
  $Node p = tree ▶ locate(keys, strlen(keys));
  for(;p ▷ valid;)
   {k ◁ va_arg(va, char *);
    if (!k) break;
    p.offset = p ▷ locate(k, strlen(k)).offset;
   }
  va_end(va);
  return p;
 }

//D1 Add                                                                        // Add a new key if not already present in a base $ or a $ owned by a node.

static $Node add_$Node_$Found_$Node_string                                      // Add a new key if not already present in the $ root at the specified node.
 ($Found * f,                                                                   // Found status for the key being added.
  $Node    owner)                                                               // Invalid - the base $. Valid - the node that owns the $ being added to.
 {const $Node invalid = new $Node;                                              // Return an invalid  node if the node already exists in the $
  if (!f->different) return invalid;                                            // Key already present

   key ◁ f->key;                                                                // The key to add
  tree ◁ f->last.list;                                                          // The base $
     n ◁ tree ▷ node(key, strlen(key)); p ◁ f->last;                            // Create new node under parent

  if (f->different < 0) p ▷ setLeft(n); else p ▷ setRight(n);                   // Insert node

  n ▷ setUp(p);                                                                 // Set parent of inserted node

  for($Node p = new $Node(list: n.list, offset: n.offset);                      // Balance nodes along path back to root
            p ▷ valid;
            p.offset = p ▷ up.offset)
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
       {if (!owner ▷ valid) tree.arena->root = rl.offset;                       // New root node in base $
        else owner ▷ setTree(rl);                                               // New root node in owned $
        if (d)
         {p ▷ setRight      (r ▷ left);  r ▷ setLeft (p);
          R ◁ p ▷ right; if (R ▷ valid)  R ▷ setUp   (p);
         }
        else
         {p ▷ setLeft       (l ▷ right); l ▷ setRight(p);
          L ◁ p ▷ left;  if (L ▷ valid)  L ▷ setUp   (p);
         }

        rl ▷ content->up = 0;                                                   // Nothing above this node as it is now the new root node
       }

      else                                                                      // Balance a node that is not the root node
       {rl ▷ setUp(q);
        if (p ▷ equals(q ▷ right)) q ▷ setRight(rl); else q ▷ setLeft(rl);
        if (d)
         {p ▷ setRight      (r ▷ left);
          R ◁ p ▷ right; if (R ▷ valid) R ▷ setUp(p);
          r ▷ setLeft(p);
         }
        else
         {p ▷ setLeft       (l ▷ right);
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

static  $Node   add_$Node_$_string                                              // Add a new key if not already present in the specified base $.
 ($     *       tree,                                                           // The $ to search
  char  * const key,                                                            // The key to add
  const size_t  length)                                                         // Length of the key to find
 {f ◀ tree ▶ find(key, length);                                                 // Try to find the key

  if (!f.last ▷ valid)                                                          // Empty $
   {root ◁ tree ▶ node(key, length);                                            // Add new key in a node
    tree->arena->root = root.offset;                                            // Make the new node the root node
    return root;                                                                // Create node
   }
  const $Node invalid = new $Node;
  return f ▷ add(invalid);                                                      // Non empty $ - add the node below the root node
 }

static  $Node   add_$Node_$Node_string                                          // Add a new key if not already present in the $ owned by the specified node.
 (const $Node * node,                                                           // The $ to search
  char  * const key,                                                            // The key to add
  const size_t  length)                                                         // Length of the key to find
 {tree  ◁ node->list;                                                           // $ to search
      f ◀ node ▶ find(key, length);                                             // Try to find the key
  if (!f.last ▷ valid)                                                          // Empty $
   {root ◁ tree ▷ node(key, length);                                            // Add new key in a node
    node ▶ setTree(root);                                                       // Make the new node the root node
    return root;                                                                // Create node
   }
  return f ▷ add(*node);                                                        // Non empty $ - add the node below the root node
 }

//D1 Traverse                                                                   // Traverse the $

static  size_t count_size_$                                                     // Count the number of nodes in a $
 (const $ * tree)                                                               // $
 {size_t l = 0;
  void count(const $Node parent)                                                // Count the nodes in a sub $
   {if (!parent ▷ valid) return;
    l++; count(parent ▷ left); count(parent ▷ right);
   }
  count(tree ▶ root);                                                           // Start at the root
  return l;                                                                     // Return count
 }

//D1 Print                                                                      // Print a $.

static StringBuffer print_$Node                                                 // Print the tree below the specified node as a string.
 (const $Node * node)                                                           // Node
 {height ◀ 0ul;                                                                 // Height of root
  p ◀ makeStringBuffer();                                                       // Print to this string buffer

  void print(const $Node node, size_t depth)                                    // Print to allocated string
   {if (!node  ▷ valid) return;                                                 // No such node
    print(node ▷ left, depth+1);                                                // Print left
    p ▷ addFormat("%lu ", depth);                                               // Depth
    p ▷ addSpaces(2 * depth);                                                   // Spacer
    makeLocalCopyOf$Key(k, l, node);                                            // Local copy of key
    p ▷ add(k);                                                                 // Print key
    for(size_t i = 0; i < height - depth - 1; ++i) p ▷ add("..");               // Separator
    p ▷ addNewLine;
    print(node ▷ right, depth+1);                                               // Print right
   }

  height = node ▶ height;                                                       // Height of node
  print(*node, 0);                                                              // Print to buffer
  p ▷ join;
  return p;
 }

static StringBuffer print_$                                                     // Print the specified $ as a string.
 (const $ * tree)                                                               // $
 {root   ◁ tree ▶ root;                                                         // Start at root
  return root ▷ print;                                                          // Print to buffer
 }

static void dump_$                                                              //P Print a $ on stderr
 (const  $ tree)                                                                // $
 {height ◀ 0ul;                                                                 // Height of root

  void print(const $Node node, size_t depth)                                    // Print to allocated string
   {if (!node  ▷ valid) return;                                                 // No such node
    print(node ▷ left, depth+1);                                                // Print left
    say("%lu ", depth);                                                         // Depth
    for(size_t i = 0; i < depth; ++i) say("  ");                                // Spacer
    makeLocalCopyOf$Key(k, l, node);                                            // Local copy of key
    say("%s", k);
    for(size_t i = 0; i < height - depth; ++i) say("..");                       // Separator
    say("\n");
    print(node ▷ right, depth+1);                                               // Print right
   }

  root   ◁ tree ▷ root;                                                         // Start at root
  height = root ▷ height;
  print(root, 0);                                                               // Print to buffer
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //Tnew$ //Tadd //Tleft //Tright //Tkey //Tup //Tfree
 {   t ◀ make$();
     t ▷ add("b",  1);
     t ▷ add("a",  1);
     t ▷ add("c",  1);
     t ▷ add("bb", 2);
     t ▷ add("cc", 2);

     b ◁ t ▷ root; a ◁ b ▷ left; c ◁ b ▷ right; B ◁ c ▷ left; C ◁ c ▷ right;

  ✓  b ▷ keyEquals("b",  1);
  ✓  a ▷ keyEquals("a",  1);
  ✓  c ▷ keyEquals("c",  1);
  ✓  B ▷ keyEquals("bb", 2);
  ✓  C ▷ keyEquals("cc", 2);
  ✓  b ▷ isRoot;
  ✓  b ▷ equals(a ▷ up);
  ✓  b ▷ equals(c ▷ up);
  ✓  c ▷ equals(B ▷ up);
  ✓  c ▷ equals(C ▷ up);

  ✓ !t ▷ find("cc", 2).different;
  ✓  t ▷ find("dd", 2).different;

     t ▷ free;
 }

void test1()                                                                    //Tll
 {  t  ◀ make$();
    t  ▷ add("a", 1);  t ▷ add("b", 1);
    a  ◁ t ▷ root; b ◁ a ▷ right;

    b  ▷ add("4", 1); b ▷ add("2", 1); b ▷ add("6", 1);
    b  ▷ add("1", 1); b ▷ add("3", 1); b ▷ add("5", 1); b ▷ add("7", 1);

    b4 ◁ b  ▷ ownedTreeRoot;
    b2 ◁ b4 ▷ left;  b6 ◁ b4 ▷ right;
    b1 ◁ b2 ▷ left;  b3 ◁ b2 ▷ right;
    b5 ◁ b6 ▷ left;  b7 ◁ b6 ▷ right;

  ✓ b4 ▷ equalsString("4");
  ✓ b2 ▷ equalsString("2");
  ✓ b6 ▷ equalsString("6");
  ✓ b1 ▷ equalsString("1");
  ✓ b3 ▷ equalsString("3");
  ✓ b5 ▷ equalsString("5");
  ✓ b7 ▷ equalsString("7");

  ✓ b4 ▷ up.offset == 0;
  ✓ b4 ▷ equals(b2 ▷ up);
  ✓ b4 ▷ equals(b6 ▷ up);
  ✓ b2 ▷ equals(b1 ▷ up);
  ✓ b2 ▷ equals(b3 ▷ up);
  ✓ b6 ▷ equals(b5 ▷ up);
  ✓ b6 ▷ equals(b7 ▷ up);

  ✓ !b ▷ find("7", 1).different;
  ✓  b ▷ find("8", 1).different;

    f  ◁ t ▷ ll("b", "7", 0);  ✓ f.offset == b7.offset;

    t  ▷ free;
 }

void test2()                                                                    //Tcount
 {t ◀ make$(); N ◁ 100ul; char c[256];

  for(i ◀ 0ul; i < N; ++i) t ▷ add(c, sprintf(c, "%lu", i));

  ✓ t   ▷ count == N;

    r   ◁ t ▷ root;
    n28 ◁ t ▷ locate("28", 2);
    n27 ◁ n28 ▷ up;
    n25 ◁ n27 ▷ up;
    n23 ◁ n25 ▷ up;

  ✓ r   ▷ equalsString("3");
  ✓ r   ▷ height == 11;
  ✓ n23 ▷ equalsString("23");

  ✓   t ▷ check;
      t ▷ free;
 }

void test3()
 {t ◀ make$(); N ◁ 256*256ul; char c[256];

  for(i ◀ 0ul; i < N; ++i) t ▷ add(c, sprintf(c, "%lx", i));

  ✓ t ▷ count == N;
  ✓ t ▷ check;

    r ◁ t ▷ root;
  ✓ r ▷ equalsString("3");
  ✓ r ▷ height == 33;

    t ▷ free;
 }

void test4()                                                                    //Theight //Tlocate //Troot
 {t ◀ make$(); char c[16]; N ◁ 10;
  n ◁ t ▷ locate("0", 1); ✓ !n ▷ valid;

  for  (i ◀ 0; i < N; ++i)
   {for(j ◀ 0; j < N; ++j)
     {t ▷ add(c, sprintf(c, "%d%d", i, j));
     }
   }

    r ◁ t ▷ root;
  ✓ r ▷ equalsString("31");
  ✓ r ▷ height == 11;

    f ◁ t ▷ locate("85", 2);
  ✓ f ▷ valid;
  ✓ f ▷ height == 2;

    t ▷ free;
 }

void test5()
 {t ◀ make$(); char c[16]; N ◁ 20;

  for(i ◀ 0; i < N; ++i)
   {   n ◁ t ▷ add(c, sprintf(c, "%d", i));
       e ◁ n ▷ locate("0", 1);
    ✓ !e ▷ valid;                                                               // Not present in sub tree

    for(j ◀ 0; j < N; ++j) n ▷ add(c, sprintf(c, "%d-%d", i, j));
   }

  const char *a[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 0};
  for(const char * const *p = a; *p; ++p)
   {  n ◁ t ▷ locate(*p, 1);
    ✓ n ▷ equalsString(*p);
   }

  t ▷ free;
 }

void test6()                                                                    //Tcheck //Tsprint //TisRoot //TkeyEquals
 {t ◀ make$(); char c[128]; N ◁ 10;

  for(i ◀ 0; i < N; ++i)
   {t ▷ add(c, sprintf(c, "%d", i));
    t ▷ add(c, sprintf(c, "%d", 2*N - i));
   }
  ✓ t ▷ check;

    r ◁ t ▷ root;

  ✓ r ▷ isRoot;
  ✓ r ▷ keyEquals("19", 2);
  ✓ r ▷ height == 6;

    t ▷ free;
 }

void test7()                                                                    //Tvalid //Tcmp$
 {$Node n = new $Node; ✓ !n ▷ valid;

  ✓ cmp$("aa", 2, "aa", 1) ==  1;
  ✓ cmp$("aa", 2, "aa", 2) ==  0;
  ✓ cmp$("aa", 1, "aa", 2) == -1;
 }

void test8()                                                                    //Tprint
 {t ◀ make$();  char s[256];  N ◀ 10;

  for  (i ◀ 0; i < N; ++i)
   {for(j ◀ N; j > 0; --j)
     {t ▷ add(s, sprintf(s, "%d%d", i, j));
     }
   }

  a ◁ t ▷ locate("86", 2);
               ✓ a ▷ valid;  ✓ a ▷ height ==  6;  ✓ a ▷ equalsString("86");
  b ◁ a ▷ up;  ✓ b ▷ valid;  ✓ b ▷ height ==  7;  ✓ b ▷ equalsString("810");
  c ◁ b ▷ up;  ✓ c ▷ valid;  ✓ c ▷ height ==  8;  ✓ c ▷ equalsString("76");
  d ◁ c ▷ up;  ✓ d ▷ valid;  ✓ d ▷ height ==  9;  ✓ d ▷ equalsString("66");
  e ◁ d ▷ up;  ✓ e ▷ valid;  ✓ e ▷ height == 10;  ✓ e ▷ equalsString("56");
  f ◁ e ▷ up;  ✓ f ▷ valid;  ✓ f ▷ height == 11;  ✓ f ▷ equalsString("36");
  g ◁ f ▷ up;  ✓ g ▷ valid;  ✓ g ▷ height == 12;  ✓ g ▷ equalsString("16");

  ✓ g ▷ equals(t ▷ root);    ✓ g ▷ isRoot;

    h ◁ t ▷ locate("98", 2);
    H ◁ h ▷ print;
  ✓ H ▷ printsAs(◉);
1   97
0 98..
1   99
◉
  ✓ t ▷ check;
    t ▷ free; H ▷ free;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4,
                           test5, test6, test7, test8, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaTree/arenaTree
