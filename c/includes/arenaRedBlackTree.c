//------------------------------------------------------------------------------
// Red black tree in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------

/* Each node of the base red black tree can, in turn, have a separate red black
tree dependent from it with the same arena*/

#define _GNU_SOURCE
#ifndef ArenaRedBlackTree_included
#define ArenaRedBlackTree_included
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <readOnlyBytes.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Red Black Tree.
#define ArenaRedBlackTreeStartString "rbt"                                                      // String that starts the arena
typedef char * ArenaRedBlackTreeString;                                                         // Arena Tree string

typedef struct ArenaRedBlackTree                                                                // Arena Tree.
 {const struct ProtoTypes_ArenaRedBlackTree *proto;                                             // Methods associated with an arena tree
  struct ArenaRedBlackTreeArena *arena;                                                         // The arena for the arena tree
 } ArenaRedBlackTree;
typedef struct ArenaRedBlackTreeDelta                                                           // The amount an item is offset within a tree.
 {unsigned int delta;
 } ArenaRedBlackTreeDelta;
typedef struct ArenaRedBlackTreeOffset                                                          // Offset to any item in the tree.  The caller is responsible for interpreting the content of the memory so addressed.
 {ArenaRedBlackTree      tree;                                                                  // ArenaTree containing the item
  size_t offset;                                                                // Offset
  const struct ProtoTypes_ArenaRedBlackTreeOffset *proto;                                       // Methods associated with offsets
 } ArenaRedBlackTreeOffset;
typedef struct ArenaRedBlackTreeNode                                                            // Offset to a node in the tree. As we know what is being addressed we have a more specific set of methods available than those available to a generic Offset.
 {ArenaRedBlackTree      tree;                                                                  // ArenaTree containing the item
  size_t offset;                                                                // Offset
  const struct ProtoTypes_ArenaRedBlackTreeNode *proto;                                         // Methods associated with nodes
 } ArenaRedBlackTreeNode;
typedef struct ArenaRedBlackTreeArena                                                           // Description of the block of memory that holds a tree.  The individual nodes of the tree cannot be cannot be freed individually, but of course the entire arena can be. As the arena uses offsets to store addresses, the arena can be resized by copying it into a new, larger arena.
 {size_t  size;                                                                 // The total size of the arena used to hold data associated with the tree.
  size_t  used;                                                                 // The number of bytes currently used.
  size_t  root;                                                                 // Offset to the root node
  ArenaRedBlackTreeString data;                                                                 // The arena containing the data associated with the tree
 } ArenaRedBlackTreeArena;
typedef struct ArenaRedBlackTreeDescription                                                     // The description of an arena tree which is written as the header record for dump files.
 {size_t version;                                                               // Version of arena tree
  size_t littleEndian;                                                          // Little endian if true
  size_t s64b;                                                                  // 64 bit address if true, else 32 bit
  size_t used;                                                                  // The number of bytes currently used by the arena.
  size_t root;                                                                  // Offset to the root node
  time_t created;                                                               // Time in seconds since the epoch when this tree was created
 } ArenaRedBlackTreeDescription;
#line 23 "/home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree.c"

typedef struct ArenaRedBlackTreeContent                                                         // Content of a node in a red black tree
 {ArenaRedBlackTreeDelta left;                                                                  // Left child node offset
  ArenaRedBlackTreeDelta right;                                                                 // Right child node offset
  ArenaRedBlackTreeDelta up;                                                                    // Parent node offset
  ArenaRedBlackTreeDelta height;                                                                // Height of the tree below this node
  ArenaRedBlackTreeDelta key;                                                                   // Offset of the key of the node - a zero terminated character string
  ArenaRedBlackTreeDelta tree;                                                                  // Location of another tree
 } ArenaRedBlackTreeContent;

typedef struct ArenaRedBlackTreeFound                                                           // The result of trying to find a node in the tree.
 {const struct ProtoTypes_ArenaRedBlackTreeFound *proto;                                        // Methods
  char *key;                                                                    // Key to find
  ArenaRedBlackTree     tree;                                                                   // Tree being processed
  ArenaRedBlackTreeNode last;                                                                   // Last node found during find(). last.offset will be zero if the tree to be searched is empty.
  ArenaRedBlackTreeNode added;                                                                  // Last node added during add()
  int   different;                                                              // The compare result on the last node found. If zero, the last node found was a match
 } ArenaRedBlackTreeFound;

typedef struct ArenaRedBlackTreeFindList                                                        // Find a concatenated key in a base tree
 {const struct ProtoTypes_ArenaRedBlackTreeFindList *proto;                                     // Methods
  char *(keys[17]);                                                             // Zero terminated array of keys to find
  ArenaRedBlackTreeFound found;                                                                 // Found results on last tree searched.
  int    count;                                                                 // The number of levels at which we were able to find a matching node.
  int    all;                                                                   // Found all the keys if true
 } ArenaRedBlackTreeFindList;

#include <arenaRedBlackTree_prototypes.h>                                                      // Arena tree prototypes now that the relevant structures have been declared

//D1 Pointers, offsets and allocations                                          // Locate items allocated in the arena

ArenaRedBlackTree newArenaRedBlackTree                                                                          // Create a new arena tree
 (const struct ArenaRedBlackTree allocator)                                                     // ArenaTree allocator
 {ArenaRedBlackTree t;                                                                          // Arena tree we are creating
  ArenaRedBlackTreeArena *a = t.arena = alloc(sizeof(ArenaRedBlackTreeArena));                                  // Allocate Arena description
  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  strcpy(a->data,    ArenaRedBlackTreeStartString);                                             // Occupy offset 0 in the rena with some junk so that we can use a delta of 0 as a null offset
  a->used   = strlen(ArenaRedBlackTreeStartString) + 1;                                         // Length used so far
  a->root   = 0;                                                                // Root not set
  t.proto   = allocator.proto;                                                  // Initialize tree prototype
  t.proto->new(t, "");                                                                  // Initialize root node
  return t;
 }
static void * pointer_tree_offset                                               //V Return a temporary pointer to an offset in a tree.
 (const ArenaRedBlackTree      tree,                                                            // Tree
  const size_t delta)                                                           // Delta
 {if (!delta) return NULL;                                                      // A zero delta is an unset delta
  if ( delta > tree.arena->used) return NULL;                                   // An delta outside the arena is an unset delta
  return (void *)(tree.arena->data + delta);                                    // Convert a non zero delta that is within the arena to a valid pointer
 }
static void * pointer_offset                                                    //V Convert a node  describing an offset into an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const ArenaRedBlackTreeOffset o)                                                              // Offset
 {return pointer_tree_offset(o.tree, o.offset);                                 // Return a temporary pointer to an offset in a tree.
 }
static ArenaRedBlackTreeContent * content_ArenaRedBlackTreeNode                                                 //V Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const ArenaRedBlackTreeNode n)                                                                // NodeContent ArenaRedBlackTreeOffset
 {return (ArenaRedBlackTreeContent *)pointer_tree_offset(n.tree, n.offset);
 }
static ArenaRedBlackTreeOffset  offset_tree_delta                                               // Create an offset to locate an item within the tree.
 (const ArenaRedBlackTree       tree,                                                           // Tree
  const size_t  delta)                                                          // Delta within arena
 {const ArenaRedBlackTreeOffset o = {tree, delta, &ProtoTypes_ArenaRedBlackTreeOffset};                         // Create offset locally
  return        o;
 }
static ArenaRedBlackTreeNode   node_tree_delta                                                  //P Create a node to locate an allocation within the arena of a tree.
 (const ArenaRedBlackTree      tree,                                                            // Tree
  const size_t delta)                                                           // Delta within arena. A delta of zero represents no such node.
 {const ArenaRedBlackTreeNode  n = {tree, delta, &ProtoTypes_ArenaRedBlackTreeNode};                            // Create node
  return       n;
 }
static ArenaRedBlackTreeOffset allocate_offset_tree_delta                                       //P Allocate memory within the arena of a tree and clear the allocated memory
 (const ArenaRedBlackTree      tree,                                                            // ArenaTree in which to allocate
  const size_t size)                                                            // Amount of memory required
 {if (tree.arena->used + size < tree.arena->size)                               // Allocate within existing arena
   {const size_t delta = tree.arena->used;                                      // Current delta to open memory
    const ArenaRedBlackTreeOffset O = tree.proto->offset(tree, delta);                                     // Current offset to open memory
    const ArenaRedBlackTreeString o = O.proto->pointer(O);                                              // Address of memory to be allocated
    memset(o, 0, size);                                                         // Clear new memory
    tree.arena->used += size;                                                   // Allocate
    return O;                                                                   // Return allocation
   }
  else                                                                          // Reallocate arena
   {const size_t S = nextPowerOfTwo(tree.arena->size + size);                   // Round up memory required
    if (S < ((size_t)((1l<<(sizeof(ArenaRedBlackTreeDelta)*8)) - 1)))                           // The maximum size of a delta if there are 8 bits in a byte
     {void * const m = realloc(tree.arena->data, S);                            // Reallocate arena - the old one will be copied into it if it is moved.
      if (m)                                                                    // Retry the memory allocation
       {tree.arena->data = m;                                                   // New arena
        tree.arena->size = S;                                                   // Arena size
        const size_t u = tree.arena->used;                                      // Length of free space
        memset(tree.arena->data + u, 0, S - u);                                 // Clear free space
        return allocate_offset_tree_delta(tree, size);                          // Allocate within arena
       }
     }
   }
  printStackBackTrace("Arena too large\n");                                     // The arena has become too large for the chosen size of offsets.
 }
static ArenaRedBlackTreeOffset saveString_offset_tree_string                                    // Save a copy of a zero terminated string in a tree and return the offset of the string.
 (const ArenaRedBlackTree tree,                                                                 // Arena tree in which to create the node
  const ArenaRedBlackTreeString str)                                                            // String
 {const ArenaRedBlackTreeOffset o = tree.proto->allocate(tree, strlen(str) + 1);
  const ArenaRedBlackTreeString t = o.proto->pointer(o);
  strcpy(t, str);
  return o;
 }
static ArenaRedBlackTreeNode new_ArenaRedBlackTreeNode_tree_string                                              // Create a new tree node and return its offset. The key of the node is zero terminated.
 (const ArenaRedBlackTree          tree,                                                        // Arena tree in which to create the node
  const ArenaRedBlackTreeString    key)                                                         // Key for this node.  Note: we do not order nodes automatically.
 {const ArenaRedBlackTreeOffset    c = tree.proto->allocate(tree, sizeof(ArenaRedBlackTreeContent));                       // Space required for a tree node
  const ArenaRedBlackTreeOffset    k = tree.proto->saveString(tree, key);                                  // Save key string in tree arena
  ArenaRedBlackTreeContent * const n = c.proto->pointer(c);                                             // Temporary pointer to node
  n->key.delta       = k.offset;                                                // Save key offset
  return tree.proto->node(tree, c.offset);                                                 // Return node
 }
static size_t used_ArenaRedBlackTree                                                            // Amount of space currently being used within the arena of a tree.
 (const ArenaRedBlackTree tree)                                                                 // Tree
 {return tree.arena->used;
 }
static void write_void_ArenaRedBlackTree_string                                                 // Write a tree to a named file or abort
 (const ArenaRedBlackTree       tree,                                                           // Tree
  const ArenaRedBlackTreeString file)                                                           // File
 {const int o = open(file, O_CREAT| O_WRONLY, S_IRWXU);                         // Open for output creating if needed
  if (o < 0) printStackBackTrace("Cannot open file: %s for write\n", file);

  time_t current; time(&current);
//const ArenaRedBlackTreeDescription h = {1, 1, 0, tree.arena->used, (root_ArenaRedBlackTreeNodeOffset_ArenaRedBlackTree(tree)).offset, current}; // Create arena tree header describing the tree
  const ArenaRedBlackTreeDescription h = {1, 1, 0, tree.proto->used(tree), tree.proto->root(tree).offset, current};   // Create arena tree header describing the tree

  if (sizeof(h) != write(o, &h, sizeof(h)))                                     // Write header
   {printStackBackTrace("Cannot write ArenaRedBlackTree header to file: %s\n", file);
   }

  const ssize_t w = write(o, tree.arena->data, tree.arena->used);               // Write arena
  if (w < 0 || tree.arena->used != (size_t)w)
   {printStackBackTrace("Cannot write ArenaRedBlackTree arena to file: %s\n", file);
   }

  close(o);
 }
#line 55 "/home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree.c"

ArenaRedBlackTreeFound newArenaRedBlackTreeFound                                                                //P New found status
 (ArenaRedBlackTreeFound  allocator)                                                            // Allocator
 {return allocator;
 }

ArenaRedBlackTreeFindList newArenaRedBlackTreeFindList                                                          //P New find list
 (ArenaRedBlackTreeFindList allocator)                                                          // Allocator
 {return allocator;
 }

static void free_ArenaRedBlackTree                                                              // Free a tree in its entirety
 (const ArenaRedBlackTree tree)                                                                 // Tree
 {free(tree.arena->data);
  free(tree.arena);
 }

//D1 Attributes                                                                 // Get and set the attributes of a node

static ArenaRedBlackTreeNode root_ArenaRedBlackTree                                                             // The root node in a red black tree
 (const ArenaRedBlackTree tree)                                                                 // Tree
 {return tree.proto->node(tree, tree.arena->root);
 }

static size_t height_ArenaRedBlackTreeNode                                                      // Height of a sub tree starting at the specified node
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {if (node.offset == 0) return 0;
  return node.proto->content(node)->height.delta;
 }

static char *key_ArenaRedBlackTreeNode                                                          // Key of the specified node as a zero terminated string
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {const ArenaRedBlackTree         t = node.tree;
  return          t.proto->pointer(t, node.proto->content(node)->key.delta);
 }

static ArenaRedBlackTreeNode up_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                     // Parent node for the specified node.
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {const ArenaRedBlackTree         t = node.tree;
  return t.proto->node(t, node.proto->content(node)->up.delta);                                    // ArenaRedBlackTreeNode.offset will be zero if we are at the top of the tree
 }

static ArenaRedBlackTreeNode left_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                   // Left child node below the specified parent node.
 (const ArenaRedBlackTreeNode     parent)                                                       // Parent
 {const ArenaRedBlackTree         t = parent.tree;
  return t.proto->node(t, parent.proto->content(parent)->left.delta);
 }

static ArenaRedBlackTreeNode right_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                  // Right child node below the specified parent node.
 (const ArenaRedBlackTreeNode     parent)                                                       // Parent
 {const ArenaRedBlackTree         t = parent.tree;
  return t.proto->node(t, parent.proto->content(parent)->right.delta);
 }

static ArenaRedBlackTreeNode ownedTreeRoot_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                          //P Root of tree owned by this node if there is  one, else the returned node has an offset of zero.
 (const ArenaRedBlackTreeNode     parent)                                                       // Parent
 {const ArenaRedBlackTree         t = parent.tree;
  return t.proto->node(t, parent.proto->content(parent)->tree.delta);
 }

static void setHeight_ArenaRedBlackTreeNode                                                     //P Save the height of the sub tree starting at the specified node
 (const ArenaRedBlackTreeNode  parent,                                                          // Parent
  const size_t height)                                                          // Height
 {parent.proto->content(parent)->height.delta = height;
 }

static void setUp_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                   //P Set parent node of specified child node
 (const ArenaRedBlackTreeNode child,                                                            // Parent
  const ArenaRedBlackTreeNode parent)                                                           // Child
 {child.proto->content(child)->up.delta = parent.offset;
 }

static void setLeft_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                 //P Set left child of specified parent.
 (const ArenaRedBlackTreeNode parent,                                                           // Parent
  const ArenaRedBlackTreeNode left)                                                             // Left child
 {parent.proto->content(parent)->left.delta = left.offset;
 }

static void setRight_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                //P Parent parent for a specified parent in a red black tree
 (const ArenaRedBlackTreeNode parent,                                                           // Parent
  const ArenaRedBlackTreeNode right)                                                            // Right child
 {parent.proto->content(parent)->right.delta = right.offset;
 }

static void setTree_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                 //P Set the other tree located by this node.
 (const ArenaRedBlackTreeNode parent,                                                           // Parent
  const ArenaRedBlackTreeNode tree)                                                             // Right child
 {parent.proto->content(parent)->tree.delta = tree.offset;
 }

//D1 Find                                                                       // Find a key if it exists within the base tree or an owned tree.

static ArenaRedBlackTreeFound  find_ArenaRedBlackTreeFound_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string                                  //P Find a key if it exists within the tree starting at this node.
 (ArenaRedBlackTreeFound       found,                                                           // Found definition
  const ArenaRedBlackTreeNode  root)                                                            // The root node at which the tree starts
 {ArenaRedBlackTree tree = root.tree;                                                           // Tree containing node
  char *key = found.key;                                                        // The key to find

  for(ArenaRedBlackTreeNode p     = root; p.offset;)                                            // Search down through tree
   {found.last    = p;                                                          // Record last parent compared with key sought
    const char *k = p.proto->key(p);                                                    // Key of current parent node
    const int i   = found.different = strcmp(key, k);                           // Compare key sought with current key

    if (!i) return found;                                                       // Found
    ArenaRedBlackTreeContent * const c = p.proto->content(p);                                           // Continue
    p = tree.proto->node(tree, i < 0 ? c->left.delta : c->right.delta);                    // Continue left or right
   }

  return found;                                                                 // Found
 }

static  ArenaRedBlackTreeFound find_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string                                         // Find a key if it exists within the tree owned by the specified node.
 (const ArenaRedBlackTreeNode  node,                                                            // The node owning the tree to search
  char * const key)                                                             // The key to find
 {ArenaRedBlackTreeFound f = newArenaRedBlackTreeFound(({struct ArenaRedBlackTreeFound t = {tree: node.tree, key: key, proto: &ProtoTypes_ArenaRedBlackTreeFound}; t;}));                              // Status of find
  ArenaRedBlackTreeNode  r = node.proto->ownedTreeRoot(node);                                              // Root node of owned tree
  if (!r.offset) return f;                                                      // Empty owned tree
  return f.proto->find(f, r);                                                           // Search the non empty owned tree starting at the specified node.
 }

static  ArenaRedBlackTreeFound find_ArenaRedBlackTreeFound_ArenaRedBlackTree_string                                             // Find a key if it exists within the base tree.
 (const ArenaRedBlackTree      tree,                                                            // The base tree to search
  char * const key)                                                             // The key to find
 {ArenaRedBlackTreeFound f = newArenaRedBlackTreeFound(({struct ArenaRedBlackTreeFound t = {tree: tree,      key: key, proto: &ProtoTypes_ArenaRedBlackTreeFound}; t;}));                              // Status of find
  if (!tree.arena->root) return f;                                              // Empty tree
  return f.proto->find(f, tree.proto->node(tree, tree.arena->root));                               // Search the non empty base tree starting at the specified node.
 }

static ArenaRedBlackTreeFindList find_ArenaRedBlackTreeFindList_ArenaRedBlackTree                                               // Find a list of keys starting at the base tree. Return true if all the keys were found.
 (ArenaRedBlackTreeFindList findList,                                                           // The list of keys to search for
  const ArenaRedBlackTree   tree)                                                               // The base tree to search
 {findList.count = 0;                                                           // Number of keys found
  char **keys = findList.keys;                                                  // Zero terminated array of keys to find.
  if (!*keys) {findList.all = 1; return findList;}                              // No keys to find - so we found all of them because we cannot show a key that we did not find.
  ArenaRedBlackTreeFound f = findList.found = tree.proto->find(tree, *keys);                               // Status of find of first key in base tree
  if (f.different) return findList;                                             // Cannot find first key in base tree
  findList.count++;                                                             // Found the first key
  for(++keys; *keys; ++keys, ++findList.count)                                  // Each following find request for a following tree owned by the last found node
   {ArenaRedBlackTreeNode node = f.last;                                                        // Last node found
    f = findList.found = node.proto->find(node, *keys);                                    // Status of find of current key in tree owned by last found node
    if (f.different) return findList;                                           // Failed to find the next key
   }
  findList.all = 1;                                                             // Successfully found all keys
  return findList;
 }

//D1 Add                                                                        // Add a new key if not already present in a base tree or a tree owned by a node.

static ArenaRedBlackTreeFound add_ArenaRedBlackTreeFound_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string                                    // Add a new key if not already present in the tree root at the specified node.
 (ArenaRedBlackTreeFound f)                                                                     // Found status for the tree to which the specified key is being added.
 {if (!f.different) return f;                                                   // Key already present

  char * const key = f.key;                                                     // The key to add
  const ArenaRedBlackTree tree = f.last.tree;                                                   // The base tree
  const ArenaRedBlackTreeNode          n = f.added = tree.proto->new(tree, key), p = f.last;               // Create new node under parent
  ArenaRedBlackTreeContent * const N = n.proto->content(n),       * const P = p.proto->content(p);

  if (f.different < 0) P->left .delta = n.offset;                               // Insert node left
  else                 P->right.delta = n.offset;                               // Insert node right

  N->up.delta = p.offset;                                                       // Set parent of inserted node

  for(ArenaRedBlackTreeNode p = n; p.offset; p = p.proto->up(p))                                        // Balance nodes along path back to root
   {size_t height(ArenaRedBlackTreeNode p)                                                      // Height of a node
     {return p.offset ? p.proto->height(p) : 0;
     }

    void setHeight(ArenaRedBlackTreeNode p)                                                     // Set the height of a node
     {const int L = height(p.proto->left(p)), R = height(p.proto->right(p));
      p.proto->setHeight(p, 1 + (L > R ? L : R));
     }

    ArenaRedBlackTreeNode balance                                                               // Balance node
     (const int d)                                                              // Balance left if true
     {const ArenaRedBlackTreeNode q = p.proto->up(p), l = p.proto->left(p), r = p.proto->right(p), rl = d ? r : l;
      if (!q.offset)                                                            // Root node
       {const ArenaRedBlackTreeNode z = {};
        tree.arena->root = rl.offset;
        rl.proto->setUp(rl, z);
       }
      else                                                                      // Not the root node
       {if ((q.proto->right(q)).offset == p.offset)
         {q.proto->setRight(q, rl); rl.proto->setUp(rl, q);
         }
        else
         {q.proto->setLeft(q, rl);  rl.proto->setUp(rl, q);
         }
       }

      if (d) {p.proto->setRight(p, r.proto->left(r));  r.proto->setLeft(r, p);}                        // Balance left
      else   {p.proto->setLeft(p, l.proto->right(l)); l.proto->setRight(l, p);}                        // Balance right

      p.proto->setUp(p, rl);
      setHeight(rl);
      setHeight(p);
      return p.proto->up(p);                                                            // The node above the input node is now balanced
     }

    const int L = height(p.proto->left(p)), R = height(p.proto->right(p));                      // Left and right depths
    if      (2*L + 1 < R) p = balance(1);                                       // Balance left or right if necessary
    else if (2*R + 1 < L) p = balance(0);
    else     setHeight(p);                                                      // Balanced: propogate height
   }

  return f;
 }

static ArenaRedBlackTreeFound add_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode_string                                            // Add a new key if not already present in the tree owned by the specified node.
 (const ArenaRedBlackTreeNode  node,                                                            // The tree to search
  char * const key)                                                             // The key to add
 {ArenaRedBlackTree tree   = node.tree;                                                         // Tree to search
  ArenaRedBlackTreeFound f = node.proto->find(node, key);                                                  // Try to find the key

  if (!f.last.offset)                                                           // Empty tree
   {const ArenaRedBlackTreeNode root = f.added = tree.proto->new(tree, key);                               // Add new key in a node
    node.proto->setTree(node, root);                                                       // Make the new node the root node
    return f;                                                                   // Find status
   }
  return f.proto->add(f);                                                               // Non empty tree - add the node below the root node
 }

static ArenaRedBlackTreeFound add_ArenaRedBlackTreeNode_ArenaRedBlackTree_string                                                // Add a new key if not already present in the specified base tree.
 (const ArenaRedBlackTree     tree,                                                             // The tree to search
  char * const key)                                                             // The key to add
 {ArenaRedBlackTreeFound f = tree.proto->find(tree, key);                                                  // Try to find the key

  if (!f.last.offset)                                                           // Empty tree
   {const ArenaRedBlackTreeNode root = f.added = tree.proto->new(tree, key);                               // Add new key in a node
    tree.arena->root = root.offset;                                             // Make the new node the root node
    return f;                                                                   // Find status
   }
  return f.proto->add(f);                                                               // Non empty tree - add the node below the root node
 }

//D1 Print                                                                      // Print a tree.

static char * sprintRoot_string_rootArenaRedBlackTreeNode                                       //P Print a tree, starting at the specified root node, as a string. We allocate a string large enough to hold the print. Freeing the allocated string containing the print is the responsibility fo the caller.
 (const ArenaRedBlackTreeNode root)                                                             // Root node of tree to be printed
 {const int W = 4;                                                              // Width of height field
  if (!root.offset) return strdup("");                                          // The tree is empty - return a freeable empty string

  size_t l = 0;                                                                 // Length of buffer needed
  void len(ArenaRedBlackTreeNode node)                                                          // Get length of print
   {if (node.offset)
     {len(node.proto->left(node)); l += 2 + W + 1 + 4 * node.proto->height(node); len(node.proto->right(node));  // With room for white space
     }
   }
  len(root);                                                                    // Set size of print

  char *p;
  void print(const ArenaRedBlackTreeNode node)                                                  // Print to allocated string
   {if (!node.offset) return;                                                   // No such node
    print(node.proto->left(node));                                                         // Print left

    const size_t H = node.proto->height(node), N = H > 0 ? H - 1 : 0;                      // Height
    char w[W+2]; sprintf(w, "%04lu ", N);
    p = stpcpy(p, w);

    for(size_t i = 0; i < N; ++i) p = stpcpy(p, "    ");                        // Spacer
    p = stpcpy(p, node.proto->key(node));                                                  // Key
    p = stpcpy(p, "\n");                                                        // End of line
    print(node.proto->right(node));                                                        // Print right
   }

  char *P = p = malloc(l); *p = 0;                                              // Create buffer
  print(root);                                                                  // Print to buffer

  return P;                                                                     // Please free the returned string when done.
 }

static char * sprint_string_ArenaRedBlackTreeNode                                               // Print the tree owned by the specified node in a string. We allocate a string large enough to hold the print. Freeing the allocated string containing the print is the responsibility fo the caller.
 (const ArenaRedBlackTreeNode node)                                                             // Node owning tree to be printed
 {ArenaRedBlackTreeNode  root = node.proto->ownedTreeRoot(node);                                           // Root of tree to be printed
  return root.proto->sprintRoot(root);
 }

static char * sprint_string_ArenaRedBlackTree                                                   // Print a tree in a string. We allocate a string large enough to hold the print. Freeing the allocated string containing the print is the responsibility fo the caller.
 (const ArenaRedBlackTree tree)                                                                 // Tree to print
 {const ArenaRedBlackTreeNode root = tree.proto->root(tree);                                               // Root of tree to print
  return root.proto->sprintRoot(root);
 }

static void print_ArenaRedBlackTreeNode                                                         // Print the tree owned by the specified node to stderr.
 (const ArenaRedBlackTreeNode node)                                                             // Node owning tree to be printed
 {char * const p = node.proto->sprint(node); fprintf(stderr, "%s\n", p); free(p);
 }

static void print_ArenaRedBlackTree                                                             // Print the specified base tree to stderr.
 (const ArenaRedBlackTree tree)                                                                 // Tree
 {char * const p = tree.proto->sprint(tree); fprintf(stderr, "%s\n", p); free(p);
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

#define TEST_TREE_SIZE 10

void test0()                                                                    //Tnew //Tadd //Tleft //Tright //Tkey //Tsprint //Tup
 {ArenaRedBlackTree t = newArenaRedBlackTree(({struct ArenaRedBlackTree t = {proto: &ProtoTypes_ArenaRedBlackTree};   t;}));
    t.proto->add(t, "b");
    t.proto->add(t, "a");
    t.proto->add(t, "c");
    t.proto->add(t, "bb");
    t.proto->add(t, "cc");
  ArenaRedBlackTreeNode b = t.proto->root(t), a = b.proto->left(b), c = b.proto->right(b), bb = c.proto->left(c), cc = c.proto->right(c);
  assert(!strcmp("b",  b.proto->key(b)));
  assert(!strcmp("a",  a.proto->key(a)));
  assert(!strcmp("c",  c.proto->key(c)));
  assert(!strcmp("bb", bb.proto->key(bb)));
  assert(!strcmp("cc", cc.proto->key(cc)));
  assert((b.proto->up(b)).offset == 0);
  assert((a.proto->up(a)).offset == b.offset);
  assert((c.proto->up(c)).offset == b.offset);
  assert((bb.proto->up(bb)).offset == c.offset);
  assert((cc.proto->up(cc)).offset == c.offset);

  char * const p = t.proto->sprint(t);
  assert(strstr(p, "0000 a"));
  assert(strstr(p, "0002         b"));
  assert(strstr(p, "0000 bb"));
  assert(strstr(p, "0001     c"));
  assert(strstr(p, "0000 cc"));
  free(p);

  assert(!t.proto->find(t, "cc").different);
  assert( t.proto->find(t, "dd").different);

  t.proto->free(t);
 }

void test1()
 {ArenaRedBlackTree t = newArenaRedBlackTree(({struct ArenaRedBlackTree t = {proto: &ProtoTypes_ArenaRedBlackTree};   t;}));
    t.proto->add(t, "a"); t.proto->add(t, "b");
  ArenaRedBlackTreeNode a = t.proto->root(t), b = a.proto->right(a);

  b.proto->add(b, "4"); b.proto->add(b, "2"); b.proto->add(b, "6");
  b.proto->add(b, "1"); b.proto->add(b, "3"); b.proto->add(b, "5"); b.proto->add(b, "7");

  ArenaRedBlackTreeNode b4 = b.proto->ownedTreeRoot(b), b2 = b4.proto->left(b4), b6 = b4.proto->right(b4),
        b1 = b2.proto->left(b2), b3 = b2.proto->right(b2), b5 = b6.proto->left(b6), b7 = b6.proto->right(b6);
  assert(!strcmp("4",  b4.proto->key(b4)));
  assert(!strcmp("2",  b2.proto->key(b2)));
  assert(!strcmp("6",  b6.proto->key(b6)));
  assert(!strcmp("1",  b1.proto->key(b1)));
  assert(!strcmp("3",  b3.proto->key(b3)));
  assert(!strcmp("5",  b5.proto->key(b5)));
  assert(!strcmp("7",  b7.proto->key(b7)));
  assert((b4.proto->up(b4)).offset == 0);
  assert((b2.proto->up(b2)).offset == b4.offset);
  assert((b6.proto->up(b6)).offset == b4.offset);
  assert((b1.proto->up(b1)).offset == b2.offset);
  assert((b3.proto->up(b3)).offset == b2.offset);
  assert((b5.proto->up(b5)).offset == b6.offset);
  assert((b7.proto->up(b7)).offset == b6.offset);

  char * const p = b.proto->sprint(b);
  assert(strstr(p, "0000 1"));
  assert(strstr(p, "0001     2"));
  assert(strstr(p, "0000 3"));
  assert(strstr(p, "0002         4"));
  assert(strstr(p, "0000 5"));
  assert(strstr(p, "0001     6"));
  assert(strstr(p, "0000 7"));
  free(p);

  char * const q = t.proto->sprint(t);
  assert(strstr(q, "0001     a"));
  assert(strstr(q, "0000 b"));
  free(q);

  assert(!b.proto->find(b, "7").different);
  assert( b.proto->find(b, "8").different);

  ArenaRedBlackTreeFindList l = newArenaRedBlackTreeFindList(({struct ArenaRedBlackTreeFindList t = {keys: {"b", "7", 0}, proto: &ProtoTypes_ArenaRedBlackTreeFindList}; t;}));
  ArenaRedBlackTreeFindList L = l.proto->find(l, t);
  assert(L.count == 2);
  assert(L.found.last.offset == b7.offset);

  t.proto->free(t);
 }

void test2()                                                                    //Tprint //Tfree
 {ArenaRedBlackTree t = newArenaRedBlackTree(({struct ArenaRedBlackTree t = {proto: &ProtoTypes_ArenaRedBlackTree};   t;}));

  for  (size_t i = 0; i < TEST_TREE_SIZE; ++i)
   {char c[2]; c[0] = 'a' + i;
    t.proto->add(t, c);
   }
  //t.proto->print(t);
  t.proto->free(t);
 }

void test3()                                                                    //Theight //Tfind //Troot
 {ArenaRedBlackTree t = newArenaRedBlackTree(({struct ArenaRedBlackTree t = {proto: &ProtoTypes_ArenaRedBlackTree};   t;}));

  for  (size_t i = 0; i < TEST_TREE_SIZE; ++i)
   {for(size_t j = 0; j < TEST_TREE_SIZE; ++j)
     {char c[4]; c[0] = '0' + i; c[1] = '0' + j;
      t.proto->add(t, c);
     }
   }

  ArenaRedBlackTreeNode r = t.proto->root(t);
  assert(!strcmp(r.proto->key(r), "31"));
  assert(r.proto->height(r) == 9);

  ArenaRedBlackTreeFound f = t.proto->find(t, "85");
  assert(!f.different && ({ArenaRedBlackTreeNode F = f.last; F.proto->height(F);}) == 2);

  //t.proto->print(t);
  t.proto->free(t);
 }

void test4()
 {ArenaRedBlackTree t = newArenaRedBlackTree(({struct ArenaRedBlackTree t = {proto: &ProtoTypes_ArenaRedBlackTree};   t;}));

  for  (size_t i = TEST_TREE_SIZE; i > 0; --i)
   {for(size_t j = TEST_TREE_SIZE; j > 0; --j)
     {char c[4]; c[0] = '0' + i - 1; c[1] = '0' + j - 1;
      t.proto->add(t, c);
     }
   }
  ArenaRedBlackTreeNode r = t.proto->root(t);
  assert(!strcmp(r.proto->key(r), "68"));
  assert(r.proto->height(r) == 9);

  ArenaRedBlackTreeFound f = t.proto->find(t, "86");
  assert(!f.different && ({ArenaRedBlackTreeNode F = f.last; F.proto->height(F);}) == 2);

//t.proto->print(t);
  t.proto->free(t);
 }

void test5()
 {ArenaRedBlackTree t = newArenaRedBlackTree(({struct ArenaRedBlackTree t = {proto: &ProtoTypes_ArenaRedBlackTree};   t;}));

  for  (size_t i = 0; i < TEST_TREE_SIZE;        ++i)
   {for(size_t j =        TEST_TREE_SIZE; j > 0; --j)
     {char c[4]; c[0] = '0' + i; c[1] = '0' + j - 1;
      t.proto->add(t, c);
     }
   }
  ArenaRedBlackTreeNode r = t.proto->root(t);
  assert(!strcmp(r.proto->key(r), "17"));
  assert(r.proto->height(r) == 13);

  ArenaRedBlackTreeFound f = t.proto->find(t, "86");
  assert(!f.different && ({ArenaRedBlackTreeNode F = f.last; F.proto->height(F);}) == 8);

// t.proto->print(t);
  t.proto->free(t);
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0, test1, test2, test3, test4, test5, 0};
  run_tests("ArenaRedBlackTree", repetitions, tests);
  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree
