#line 1 "/home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree.c"
//------------------------------------------------------------------------------
// Red black tree in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------

/* Each node of the base red black tree can, in turn, have a separate red black
tree dependent from it with the same arena*/

#define _GNU_SOURCE
#ifndef ArenaRedBlackTree_included
#define ArenaRedBlackTree_included
#include <readOnlyBytes.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Red Black Tree.
typedef char * ArenaRedBlackTreeString;                                                         // Arena Tree string

struct ArenaRedBlackTreeFound;                                                                  // Forward declare some of the structures used
struct ArenaRedBlackTreeNode;

typedef struct ArenaRedBlackTree                                                                // Arena Tree.
 {const struct ProtoTypes_ArenaRedBlackTree *proto;                                             // Methods associated with an arena tree
  struct ArenaRedBlackTreeArena *arena;                                                         // The arena for the arena tree
  int (*different)(const struct ArenaRedBlackTreeFound *f, const struct ArenaRedBlackTreeNode *n);
 } ArenaRedBlackTree;

#line 25 "/home/phil/c/z/arenaTree/arenaTree.c"
typedef struct ArenaRedBlackTreeDelta                                                           //I The amount an item is offset within a tree.
 {unsigned int delta;
 } ArenaRedBlackTreeDelta;
#line 36 "/home/phil/c/z/arenaTree/arenaTree.c"
typedef struct ArenaRedBlackTreeOffset                                                          //I Offset to any item in the tree.  The caller is responsible for interpreting the content of the memory so addressed.
 {const ArenaRedBlackTree      tree;                                                            // ArenaTree containing the item
  const size_t offset;                                                          // Offset
  const struct ProtoTypes_ArenaRedBlackTreeOffset *proto;                                       // Methods associated with offsets
 } ArenaRedBlackTreeOffset;
#line 42 "/home/phil/c/z/arenaTree/arenaTree.c"
typedef struct ArenaRedBlackTreeNode                                                            //I Offset to a node in the tree. As we know what is being addressed we have a more specific set of methods available than those available to a generic Offset.
 {ArenaRedBlackTree      tree;                                                                  // ArenaTree containing the item
  size_t offset;                                                                // Offset
  const struct ProtoTypes_ArenaRedBlackTreeNode *proto;                                         // Methods associated with nodes
 } ArenaRedBlackTreeNode;
#line 48 "/home/phil/c/z/arenaTree/arenaTree.c"
typedef struct ArenaRedBlackTreeArena                                                           //I Description of the block of memory that holds a tree.  The individual nodes of the tree cannot be cannot be freed individually, but of course the entire arena can be. As the arena uses offsets to store addresses, the arena can be resized by copying it into a new, larger arena.
 {size_t  size;                                                                 // The total size of the arena used to hold data associated with the tree.
  size_t  used;                                                                 // The number of bytes currently used.
  size_t  root;                                                                 // Offset to the root node
  ArenaRedBlackTreeString data;                                                                 // The arena containing the data associated with the tree
 } ArenaRedBlackTreeArena;
#line 55 "/home/phil/c/z/arenaTree/arenaTree.c"
typedef struct ArenaRedBlackTreeDescription                                                     //I The description of an arena tree which is written as the header record for dump files.
 {const size_t version;                                                         // Version of arena tree
  const size_t littleEndian;                                                    // Little endian if true
  const size_t s64b;                                                            // 64 bit address if true, else 32 bit
  const size_t used;                                                            // The number of bytes currently used by the arena.
  const size_t root;                                                            // Offset to the root node
  const time_t created;                                                         // Time in seconds since the epoch when this tree was created
 } ArenaRedBlackTreeDescription;
#line 28 "/home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree.c"

typedef struct ArenaRedBlackTreeContent                                                         // Content of a node in a red black tree
 {ArenaRedBlackTreeDelta left;                                                                  // Left child node offset
  ArenaRedBlackTreeDelta right;                                                                 // Right child node offset
  ArenaRedBlackTreeDelta up;                                                                    // Parent node offset
  ArenaRedBlackTreeDelta height;                                                                // Height of the tree below this node
  ArenaRedBlackTreeDelta key;                                                                   // Offset of the key of the node - a zero terminated character string
  ArenaRedBlackTreeDelta data;                                                                  // User data associated with this node.
  ArenaRedBlackTreeDelta tree;                                                                  // Location of another tree
 } ArenaRedBlackTreeContent;

typedef struct ArenaRedBlackTreeFound                                                           // The result of trying to find a node in the tree.
 {const struct ProtoTypes_ArenaRedBlackTreeFound *proto;                                        // Methods
  char *key;                                                                    // Key to find
  ArenaRedBlackTree     tree;                                                                   // Tree being processed
  ArenaRedBlackTreeNode last;                                                                   // Last node found during find.
  int   different;                                                              // The compare result on the last node found. If zero, the last node found was a match
 } ArenaRedBlackTreeFound;

#include <arenaRedBlackTree_prototypes.h>                                                      // Arena tree prototypes now that the relevant structures have been declared

//D1 Pointers, offsets and allocations                                          // Locate items allocated in the arena

#line 71 "/home/phil/c/z/arenaTree/arenaTree.c"
static ArenaRedBlackTree makeArenaRedBlackTree                                                                  //I Create a new arena tree
 ()                                                                             // ArenaTree allocator
 {ArenaRedBlackTreeArena * const a = alloc(sizeof(ArenaRedBlackTreeArena));                                     // Allocate arena description
  const ArenaRedBlackTree t = newArenaRedBlackTree(({struct ArenaRedBlackTree t = {arena: a, proto: &ProtoTypes_ArenaRedBlackTree}; t;}));                                                  // Arena tree we are creating
  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  a->used   = 0;                                                                // Length used so far
  a->root   = 0;                                                                // Root not set in ArenaRedBlackTree
  t.proto->node(t, "");                                                                 // Initialize root node
  return t;
 }
#line 91 "/home/phil/c/z/arenaTree/arenaTree.c"
static void * pointer_ArenaRedBlackTree_size                                                    //IPV Return a temporary pointer to an offset in a tree.
 (const ArenaRedBlackTree      tree,                                                            // Tree
  const size_t delta)                                                           // Delta
 {if (delta > tree.arena->used)                                                 // An delta outside the arena is an unset delta
   {printStackBackTrace("Accessing area outside arena");
   }
  return (void *)(tree.arena->data + delta);                                    // Convert a non zero delta that is within the arena to a valid pointer
 }
#line 100 "/home/phil/c/z/arenaTree/arenaTree.c"
static void * pointer_ArenaRedBlackTreeOffset                                                   //IPV Convert a node describing an offset into an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const ArenaRedBlackTreeOffset o)                                                              // Offset
 {return pointer_ArenaRedBlackTree_size(o.tree, o.offset);                                      // Return a temporary pointer to an offset in a tree.
 }
#line 116 "/home/phil/c/z/arenaTree/arenaTree.c"
static ArenaRedBlackTreeContent * content_ArenaRedBlackTreeNode                                                 //IPV Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const ArenaRedBlackTreeNode n)                                                                // NodeContent ArenaRedBlackTreeOffset
 {return (ArenaRedBlackTreeContent *)pointer_ArenaRedBlackTree_size(n.tree, n.offset);
 }
#line 129 "/home/phil/c/z/arenaTree/arenaTree.c"
static int equals_int_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                               //I Confirm two offsets are equal
 (const ArenaRedBlackTreeNode a,                                                                // First offset
  const ArenaRedBlackTreeNode b)                                                                // Second offset
 {return a.tree.arena == b.tree.arena && a.offset == b.offset;
 }
#line 164 "/home/phil/c/z/arenaTree/arenaTree.c"
static size_t used_ArenaRedBlackTree                                                            //I Amount of space currently being used within the arena of a tree.
 (const ArenaRedBlackTree tree)                                                                 // Tree
 {return tree.arena->used;
 }
#line 169 "/home/phil/c/z/arenaTree/arenaTree.c"
static ArenaRedBlackTreeOffset  offset_ArenaRedBlackTree_size                                                   //IP Create an offset to locate an item within the tree.
 (const ArenaRedBlackTree       tree,                                                           // Tree
  const size_t  delta)                                                          // Delta within arena
 {const ArenaRedBlackTreeOffset o = {tree, delta, &ProtoTypes_ArenaRedBlackTreeOffset};                         // Create offset locally
  return        o;
 }
#line 176 "/home/phil/c/z/arenaTree/arenaTree.c"
static ArenaRedBlackTreeNode   nodeFromOffset_ArenaRedBlackTree_size                                            //IP Create a node to locate an allocation within the arena of a tree.
 (const ArenaRedBlackTree      tree,                                                            // Tree
  const size_t delta)                                                           // Delta within arena. A delta of zero represents no such node.
 {const ArenaRedBlackTreeNode  n = {tree, delta, &ProtoTypes_ArenaRedBlackTreeNode};                            // Create node
  return       n;
 }
#line 183 "/home/phil/c/z/arenaTree/arenaTree.c"
static ArenaRedBlackTreeOffset allocate_offset_ArenaRedBlackTree_size                                           //IP Allocate memory within the arena of a tree and clear the allocated memory
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
        return tree.proto->allocate(tree, size);                                           // Allocate within arena
       }
     }
   }
  printStackBackTrace("Arena too large\n");                                     // The arena has become too large for the chosen size of offsets.
 }
#line 210 "/home/phil/c/z/arenaTree/arenaTree.c"
static ArenaRedBlackTreeNode noden_ArenaRedBlackTreeNode_ArenaRedBlackTree_ArenaRedBlackTreeString                                              //I Create a new tree node keyed by a string of the specified length to which a terminating zero will be appended.
 (const ArenaRedBlackTree       tree,                                                           // Arena tree in which to create the node
  const char * const key,                                                       // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the tree is determined solely by the user.
  const size_t  length)                                                         // Length of the key, or if zero, I will use strlen
 {const ArenaRedBlackTreeOffset c = tree.proto->allocate(tree, sizeof(ArenaRedBlackTreeContent));                          // Space required for a tree node
  const ArenaRedBlackTreeOffset k = tree.proto->saveString(tree, key, length);                             // Save key string in tree arena
  ArenaRedBlackTreeContent * const a = c.proto->pointer(c);
  a->key.delta = k.offset;                                                      // Save key offset
  return tree.proto->nodeFromOffset(tree, c.offset);                                       // Return node
 }
#line 221 "/home/phil/c/z/arenaTree/arenaTree.c"
static ArenaRedBlackTreeNode node_ArenaRedBlackTreeNode_ArenaRedBlackTree_ArenaRedBlackTreeString                                               //I Create a new tree node keyed by a zero terminated string.
 (const ArenaRedBlackTree             tree,                                                     // Arena tree in which to create the node
  const char * const  key)                                                      // Key for this node.  Note: we do not order nodes automatically.
 {return tree.proto->noden(tree, key, 0);
 }
#line 227 "/home/phil/c/z/arenaTree/arenaTree.c"
static ArenaRedBlackTreeOffset saveString_ArenaRedBlackTreeOffset_ArenaRedBlackTree_ArenaRedBlackTreeString                                     //IP Save a copy of a zero terminated string in a tree and return the offset of the string.
 (const ArenaRedBlackTree       tree,                                                           // Arena tree in which to create the node
  const char * const str,                                                       // String
  const size_t  length)                                                         // String, or if zero I will call strlen
 {const size_t l = length ? : strlen(str);                                      // Length of string
  const ArenaRedBlackTreeOffset o = tree.proto->allocate(tree, l + 1);                                     // Allocate space for the string plus a terminating zero
  char * const t = o.proto->pointer(o);
  char * const T = stpncpy(t, str, l); *T = 0;
  return o;
 }
#line 290 "/home/phil/c/z/arenaTree/arenaTree.c"
static size_t getData_size_ArenaRedBlackTreeNode                                                //I Get the value of the data offset associated with a node.
 (const ArenaRedBlackTreeNode  node)                                                            // Node in an arena tree associated with the data
 {return node.proto->content(node)->data.delta;                                            // Value of the data offset field
 }
#line 295 "/home/phil/c/z/arenaTree/arenaTree.c"
static void setData_ArenaRedBlackTreeNode_size                                                  //I Set the value of the data offset associated with a node.
 (const ArenaRedBlackTreeNode  node,                                                            // Node in an arena tree to associate with the data
  const size_t offset)                                                          // Value the data offset is to be set to
 {node.proto->content(node)->data.delta = offset;                                          // Record offset
 }
#line 366 "/home/phil/c/z/arenaTree/arenaTree.c"
static int equalsString_ArenaRedBlackTreeNode_string                                            //I Check that the key of a node
 (const ArenaRedBlackTreeNode        node,                                                      // Node
  const char * const key)                                                       // Key
 {return !strcmp(node.proto->key(node), key);
 }
#line 52 "/home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree.c"

static void free_ArenaRedBlackTree                                                              // Free a tree in its entirety
 (const ArenaRedBlackTree tree)                                                                 // Tree
 {free(tree.arena->data);
  free(tree.arena);
 }

static  ArenaRedBlackTreeFound makeArenaRedBlackTreeFound                                                       //P Create a new found description
 (const ArenaRedBlackTree      tree,                                                            // The tree being searched
  char * const key)                                                             // The key to find
 {const ArenaRedBlackTreeNode n = newArenaRedBlackTreeNode(({struct ArenaRedBlackTreeNode t = {tree: tree, proto: &ProtoTypes_ArenaRedBlackTreeNode}; t;}));                                        // Sample node within tree
  return newArenaRedBlackTreeFound(({struct ArenaRedBlackTreeFound t = {tree: tree, key: key, last: n, proto: &ProtoTypes_ArenaRedBlackTreeFound}; t;}));
 }

//D1 Attributes                                                                 // Get and set the attributes of a node

static ArenaRedBlackTreeNode root_ArenaRedBlackTree                                                             // The root node in a red black tree
 (const ArenaRedBlackTree tree)                                                                 // Tree
 {return tree.proto->nodeFromOffset(tree, tree.arena->root);
 }

static size_t height_ArenaRedBlackTreeNode                                                      // Height of a sub tree starting at the specified node
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {return node.proto->content(node)->height.delta;
 }

static char *key_ArenaRedBlackTreeNode                                                          // Key of the specified node as a zero terminated string
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {return node.tree.proto->pointer(node.tree, node.proto->content(node)->key.delta);
 }

static ArenaRedBlackTreeNode up_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                     // Parent node for the specified node.
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {return node.tree.proto->nodeFromOffset(node.tree, node.proto->content(node)->up.delta);
 }

static ArenaRedBlackTreeNode left_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                   // Left child node below the specified parent node.
 (const ArenaRedBlackTreeNode     parent)                                                       // Parent
 {return parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->left.delta);
 }

static ArenaRedBlackTreeNode right_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                  // Right child node below the specified parent node.
 (const ArenaRedBlackTreeNode     parent)                                                       // Parent
 {return  parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->right.delta);
 }

static ArenaRedBlackTreeNode ownedTreeRoot_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                          //P Root of tree owned by this node if there is  one, else the returned node has an offset of zero.
 (const ArenaRedBlackTreeNode     parent)                                                       // Parent
 {return parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->tree.delta);
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
  const ArenaRedBlackTreeNode tree)                                                             // Tree to be added to parent
 {parent.proto->content(parent)->tree.delta = tree.offset;
 }

static int valid_ArenaRedBlackTreeNode                                                          // Check that the specified node is valid.
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {return node.offset;                                                           // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

//D1 Find                                                                       // Find a key if it exists within the base tree or an owned tree.

static ArenaRedBlackTreeFound find_ArenaRedBlackTreeFound_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string                                   //P Find a key if it exists within the tree starting at this node.
 (ArenaRedBlackTreeFound      found,                                                            // Found definition
  const ArenaRedBlackTreeNode root)                                                             // The root node at which the tree starts
 {const ArenaRedBlackTree tree = root.tree;                                                     // Tree containing node
  const char * const key = found.key;                                           // The key to find

  for(ArenaRedBlackTreeNode p   = root; p.proto->valid(p);)                                             // Search down through tree
   {found.last  = p;                                                            // Record last parent compared with key sought
    const char * const k = p.proto->key(p);                                             // Key of current parent node
    const int i = found.different = strcmp(key, k);                             // Compare key sought with current key

    if (!i) return found;                                                       // Found

    ArenaRedBlackTreeContent * const c = p.proto->content(p);                                           // Continue
    p = tree.proto->nodeFromOffset(tree, i < 0 ? c->left.delta : c->right.delta);          // Continue left or right
   }

  return found;                                                                 // Found
 }

static ArenaRedBlackTreeNode locate_ArenaRedBlackTreeNode_string                                                // Locate the node with the specified key if it exists within the tree owned by the specified node.
 (const ArenaRedBlackTreeNode        node,                                                      // Node
  const char * const key)                                                       // Key to find
 {const ArenaRedBlackTree tree = node.tree;                                                     // ArenaRedBlackTree containing node
  ArenaRedBlackTreeNode p = node.proto->ownedTreeRoot(node);                                               // Root node
  if (!p.proto->valid(p)) return p;                                                     // Empty tree
  for(;;)                                                                       // Search down through tree
   {const char * const k = p.proto->key(p);                                             // Key of current parent node
    const int i = strcmp(key, k);                                               // Compare key sought with current key

    if (!i) return p;                                                           // Found

    ArenaRedBlackTreeContent * const c = p.proto->content(p);                                           // Continue
    p = tree.proto->nodeFromOffset(tree, i < 0 ? c->left.delta : c->right.delta);          // Continue left or right
    if (!p.proto->valid(p)) return p;                                                   // Not found
   }

  printStackBackTrace("Should not be able to reach here\n");
 }

static ArenaRedBlackTreeNode locate_ArenaRedBlackTree_string                                                    // Locate the node with the specified key if it exists within the specified tree.
 (const ArenaRedBlackTree            tree,                                                      // ArenaRedBlackTree
  const char * const key)                                                       // Key to find
 {ArenaRedBlackTreeNode p = tree.proto->root(tree);                                                        // Root node
  if (!p.proto->valid(p)) return p;                                                     // Empty tree
  for(;;)                                                                       // Search down through tree
   {const char * const k = p.proto->key(p);                                             // Key of current parent node
    const int i = strcmp(key, k);                                               // Compare key sought with current key

    if (!i) return p;                                                           // Found

    ArenaRedBlackTreeContent * const c = p.proto->content(p);                                           // Continue
    p = tree.proto->nodeFromOffset(tree, i < 0 ? c->left.delta : c->right.delta);          // Continue left or right
    if (!p.proto->valid(p)) return p;                                                   // Not found
   }

  printStackBackTrace("Should not be able to reach here\n");
 }

static void check_ArenaRedBlackTree                                                             // Check the integrity of the tree
 (const ArenaRedBlackTree tree)                                                                 // Tree to check
 {void check(ArenaRedBlackTreeNode p)
   {if (!p.proto->valid(p)) return;
    const ArenaRedBlackTreeNode l = p.proto->left(p), r = p.proto->right(p);
    const ArenaRedBlackTreeNode L = l.proto->up(l),   R = r.proto->up(r);
    if (l.proto->valid(l) && !p.proto->equals(p, l.proto->up(l))) printStackBackTrace("Bad left  up %s to %s should be %s\n", l.proto->key(l), L.proto->key(L), p.proto->key(p));
    if (r.proto->valid(r) && !p.proto->equals(p, r.proto->up(r))) printStackBackTrace("Bad right up %s to %s should be %s\n", r.proto->key(r), R.proto->key(R), p.proto->key(p));
    if (l.proto->valid(l) && r.proto->valid(r))
     {const size_t lh = l.proto->height(l), rh = r.proto->height(r);
      if (lh > 2 * rh + 1 || rh > 2 * lh + 1 ) printStackBackTrace("Unbalanced tree at %s with left %lu versus right %lu\n", p.proto->key(p), lh, rh);
     }
    check(l);
    check(r);
   }
  check(tree.proto->root(tree));
 }

static  ArenaRedBlackTreeFound find_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string                                         //P Find a key if it exists within the tree owned by the specified node.
 (const ArenaRedBlackTreeNode  node,                                                            // The node owning the tree to search
  char * const key)                                                             // The key to find
 {const ArenaRedBlackTreeFound f = makeArenaRedBlackTreeFound(node.tree, key);                                  // Status of find
  const ArenaRedBlackTreeNode  r = node.proto->ownedTreeRoot(node);                                        // Root node of owned tree
  if (!r.proto->valid(r)) return f;                                                     // Empty owned tree
  return f.proto->find(f, r);                                                           // Search the non empty owned tree starting at the specified node.
 }

static  ArenaRedBlackTreeFound find_ArenaRedBlackTreeFound_ArenaRedBlackTree_string                                             //P Find a key if it exists within the base tree.
 (const ArenaRedBlackTree      tree,                                                            // The base tree to search
  char * const key)                                                             // The key to find
 {const ArenaRedBlackTreeFound f = makeArenaRedBlackTreeFound(tree, key);                                       // Status of find
  if (!tree.arena->root) return f;                                              // Empty tree
  return f.proto->find(f, tree.proto->nodeFromOffset(tree, tree.arena->root));                     // Search the non empty base tree starting at the specified node.
 }

static ArenaRedBlackTreeNode ll_ArenaRedBlackTreeNode_ArenaRedBlackTree_strings                                                 // Search through a series of owned trees starting at the base tree as directed by the specified keys
 (const ArenaRedBlackTree   tree,                                                               // The base tree to search from
  const char * const keys,                                                      // Zero terminated list of keys
  ...)                                                                          // Following keys
 {va_list va;
  va_start(va, keys);
  ArenaRedBlackTreeNode p = tree.proto->locate(tree, keys);
  for(;p.proto->valid(p);)
   {const char * const k = va_arg(va, char *);
    if (!k) break;
    p = p.proto->locate(p, k);
   }
  va_end(va);
  return p;
 }

//D1 Add                                                                        // Add a new key if not already present in a base tree or a tree owned by a node.

static ArenaRedBlackTreeNode add_ArenaRedBlackTreeNode_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string                                      // Add a new key if not already present in the tree root at the specified node.
 (ArenaRedBlackTreeFound f,                                                                     // Found status for the key being added.
  ArenaRedBlackTreeNode owner)                                                                  // Invalid - the base tree. Valid - the node that owns the tree being added to.
 {const ArenaRedBlackTreeNode invalid = newArenaRedBlackTreeNode(({struct ArenaRedBlackTreeNode t = {proto: &ProtoTypes_ArenaRedBlackTreeNode};   t;}));                                              // Return an invalid  node if the node already exists in the tree
  if (!f.different) return invalid;                                             // Key already present

  const char * const key = f.key;                                               // The key to add
  const ArenaRedBlackTree           tree = f.last.tree;                                         // The base tree
  const ArenaRedBlackTreeNode          n = tree.proto->node(tree, key), p = f.last;                        // Create new node under parent

  if (f.different < 0) p.proto->setLeft(p, n); else p.proto->setRight(p, n);                    // Insert node

  n.proto->setUp(n, p);                                                                 // Set parent of inserted node

//?for(; p.proto->valid(p); p = p.proto->up(p))                                                 // Balance nodes along path back to root
  for(ArenaRedBlackTreeNode p = n; p.proto->valid(p); p = p.proto->up(p))                                       // Balance nodes along path back to root
   {size_t height(ArenaRedBlackTreeNode p)                                                      // Height of a node
     {return p.proto->valid(p) ? p.proto->height(p) : 0;
     }

    void setHeight(ArenaRedBlackTreeNode p)                                                     // Set the height of a node
     {const int L = height(p.proto->left(p)), R = height(p.proto->right(p));
      p.proto->setHeight(p, 1 + (L > R ? L : R));
     }

    ArenaRedBlackTreeNode balance                                                               // Balance node
     (const int d)                                                              // Rotate left if true
     {const ArenaRedBlackTreeNode q = p.proto->up(p), l = p.proto->left(p), r = p.proto->right(p), rl = d ? r : l;

      if (!q.proto->valid(q))                                                           // Balance the root node
       {if (!owner.proto->valid(owner)) tree.arena->root = rl.offset;                       // New root node in base tree
        else owner.proto->setTree(owner, rl);                                               // New root node in owned tree
        if (d)
         {p.proto->setRight(p, r.proto->left(r));     r.proto->setLeft(r, p);
          const ArenaRedBlackTreeNode R = p.proto->right(p); if (R.proto->valid(R)) R.proto->setUp(R, p);
         }
        else
         {p.proto->setLeft(p, l.proto->right(l));    l.proto->setRight(l, p);
          const ArenaRedBlackTreeNode L = p.proto->left(p);  if (L.proto->valid(L)) L.proto->setUp(L, p);
         }

        rl.proto->content(rl)->up.delta = 0;                                             // Nothing above this node as it is now the new root node
       }

      else                                                                      // Balance a node that is not the root node
       {rl.proto->setUp(rl, q);
        if (p.proto->equals(p, q.proto->right(q))) q.proto->setRight(q, rl); else q.proto->setLeft(q, rl);
        if (d)
         {p.proto->setRight(p, r.proto->left(r));
          const ArenaRedBlackTreeNode R = p.proto->right(p); if (R.proto->valid(R)) R.proto->setUp(R, p);
          r.proto->setLeft(r, p);
         }
        else
         {p.proto->setLeft(p, l.proto->right(l));
          const ArenaRedBlackTreeNode L = p.proto->left(p);  if (L.proto->valid(L)) L.proto->setUp(L, p);
          l.proto->setRight(l, p);
         }
        setHeight(q);
       }

      p.proto->setUp(p, rl);
      setHeight(p);
      setHeight(rl);
      return rl;                                                                // The node now above the input node is now balanced
     }

    const int L = height(p.proto->left(p)), R = height(p.proto->right(p));                      // Left and right depths
    if      (2*L + 1 < R) p = balance(1);                                       // Balance left or right if necessary
    else if (2*R + 1 < L) p = balance(0);
    else     setHeight(p);                                                      // Balanced: propogate height
   }

  return n;                                                                     // The node added
 }

static  ArenaRedBlackTreeNode   add_ArenaRedBlackTreeNode_ArenaRedBlackTree_string                                              // Add a new key if not already present in the specified base tree.
 (const ArenaRedBlackTree       tree,                                                           // The tree to search
  char  * const key)                                                            // The key to add
 {ArenaRedBlackTreeFound f =    tree.proto->find(tree, key);                                               // Try to find the key

  if (!f.last.proto->valid(f.last))                                                          // Empty tree
   {const ArenaRedBlackTreeNode root = tree.proto->node(tree, key);                                        // Add new key in a node
    tree.arena->root = root.offset;                                             // Make the new node the root node
    return root;                                                                // Create node
   }
  const ArenaRedBlackTreeNode invalid = newArenaRedBlackTreeNode(({struct ArenaRedBlackTreeNode t = {proto: &ProtoTypes_ArenaRedBlackTreeNode};   t;}));
  return f.proto->add(f, invalid);                                                      // Non empty tree - add the node below the root node
 }

static  ArenaRedBlackTreeNode   add_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode_string                                          // Add a new key if not already present in the tree owned by the specified node.
 (const ArenaRedBlackTreeNode   node,                                                           // The tree to search
  char  * const key)                                                            // The key to add
 {const ArenaRedBlackTree tree  = node.tree;                                                    // Tree to search
  ArenaRedBlackTreeFound f = node.proto->find(node, key);                                                  // Try to find the key
  if (!f.last.proto->valid(f.last))                                                          // Empty tree
   {const ArenaRedBlackTreeNode root = tree.proto->node(tree, key);                                        // Add new key in a node
    node.proto->setTree(node, root);                                                       // Make the new node the root node
    return root;                                                                // Create node
   }
  return f.proto->add(f, node);                                                         // Non empty tree - add the node below the root node
 }

//D1 Traverse                                                                   // Traverse the ArenaRedBlackTree

static  size_t count_size_ArenaRedBlackTree                                                     // Count the number of nodes in a ArenaRedBlackTree
 (const ArenaRedBlackTree tree)                                                                 // ArenaRedBlackTree
 {size_t l = 0;
  void count(const ArenaRedBlackTreeNode parent)                                                // Count the nodes in a sub tree
   {if (!parent.proto->valid(parent)) return;
    l++; count(parent.proto->left(parent)); count(parent.proto->right(parent));
   }
  count(tree.proto->root(tree));                                                           // Start at the root
  return l;                                                                     // Return count
 }

//D1 Print                                                                      // Print a tree.

static ReadOnlyBytes sprintRoot_string_rootArenaRedBlackTreeNode                                //P Print a tree, starting at the specified root node, as a string. We allocate a string large enough to hold the print. Freeing the allocated string containing the print is the responsibility fo the caller.
 (const ArenaRedBlackTreeNode root)                                                             // Root node of tree to be printed
 {const int W = 4;                                                              // Width of height field
  if (!root.proto->valid(root)) return makeReadOnlyBytesFromString("");                    // The tree is empty

  size_t l = 1;                                                                 // Length of buffer needed - terminating 0
  void len(ArenaRedBlackTreeNode node, size_t depth)                                            // Get length of print
   {if (node.proto->valid(node))
     {len(node.proto->left(node), depth+1);
      l += 1 + depth * W + strlen(node.proto->key(node));                                  // newline(({struct line t = {proto: &ProtoTypes_line};   t;})), spacing, key
      len(node.proto->right(node), depth+1);
     }
   }
  len(root, 0);                                                                 // Set size of print

  const ReadOnlyBytes r = makeReadOnlyBytesBuffer(l);                           // Output area
  char * p = r.data;

  void print(const ArenaRedBlackTreeNode node, size_t depth)                                    // Print to allocated string
   {if (!node.proto->valid(node)) return;                                                  // No such node
    print(node.proto->left(node), depth+1);                                                // Print left
    for(size_t i = 0; i < depth * W; ++i) p = stpcpy(p, " ");                   // Spacer
    p = stpcpy(p, node.proto->key(node));                                                  // Key
    p = stpcpy(p, "\n");                                                        // End of line
    print(node.proto->right(node), depth+1);                                               // Print right
   }

  print(root, 0);                                                               // Print to buffer

  return r;                                                                     // Please free the returned string when done.
 }

static ReadOnlyBytes sprint_string_ArenaRedBlackTreeNode                                        // Print the tree owned by the specified node as ReadOnlyBytes.
 (const ArenaRedBlackTreeNode node)                                                             // Node owning tree to be printed
 {const ArenaRedBlackTreeNode  root = node.proto->ownedTreeRoot(node);                                     // Root of tree to be printed
  return root.proto->sprintRoot(root);
 }

static ReadOnlyBytes sprint_string_ArenaRedBlackTree                                            // Print a tree  as ReadOnlyBytes.
 (const ArenaRedBlackTree tree)                                                                 // Tree to print
 {const ArenaRedBlackTreeNode root = tree.proto->root(tree);                                               // Root of tree to print
  return root.proto->sprintRoot(root);
 }
// Marked with I to prevent tests needed.
static void print_ArenaRedBlackTreeNode                                                         //I Print the tree owned by the specified node to stderr.
 (const ArenaRedBlackTreeNode node)                                                             // Node owning tree to be printed
 {const ReadOnlyBytes p = node.proto->sprint(node);
  fprintf(stderr, "%s\n", p.data);
  p.proto->free(p);
 }

static void print_ArenaRedBlackTree                                                             //I Print the specified base tree to stderr.
 (const ArenaRedBlackTree tree)                                                                 // Tree
 {const ReadOnlyBytes p = tree.proto->sprint(tree);
  fprintf(stderr, "%s\n", p.data);
  p.proto->free(p);
 }

static size_t b2SumW8_size_ArenaRedBlackTree                                                    // Digest for a tree
 (const ArenaRedBlackTree tree)                                                                 // Tree
 {const ReadOnlyBytes s = tree.proto->sprint(tree);
  const size_t d = s.proto->b2SumW8(s);
  s.proto->free(s);
  return d;
 }

static size_t b2SumW8_size_ArenaRedBlackTreeNode                                                // Digest for the tree owned by a node
 (const ArenaRedBlackTreeNode node)                                                             // Owning node
 {const ReadOnlyBytes s = node.proto->sprint(node);
  const size_t d = s.proto->b2SumW8(s);
  s.proto->free(s);
  return d;
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

#define TEST_TREE_SIZE 10

void test0()                                                                    //TnewArenaRedBlackTree //Tadd //Tleft //Tright //Tkey //Tup //Tfree
 {ArenaRedBlackTree t = makeArenaRedBlackTree();
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

  assert(!t.proto->find(t, "cc").different);
  assert( t.proto->find(t, "dd").different);

  t.proto->free(t);
 }

void test1()                                                                    //Tll
 {ArenaRedBlackTree t = makeArenaRedBlackTree();
    t.proto->add(t, "a"); t.proto->add(t, "b");
  ArenaRedBlackTreeNode a = t.proto->root(t), b = a.proto->right(a);

  b.proto->add(b, "4"); b.proto->add(b, "2"); b.proto->add(b, "6");
  b.proto->add(b, "1"); b.proto->add(b, "3"); b.proto->add(b, "5"); b.proto->add(b, "7");

  ArenaRedBlackTreeNode b4 = b.proto->ownedTreeRoot(b), b2 = b4.proto->left(b4), b6 = b4.proto->right(b4),
        b1 = b2.proto->left(b2), b3 = b2.proto->right(b2), b5 = b6.proto->left(b6), b7 = b6.proto->right(b6);
  assert(b4.proto->equalsString(b4, "4"));
  assert(b2.proto->equalsString(b2, "2"));
  assert(b6.proto->equalsString(b6, "6"));
  assert(b1.proto->equalsString(b1, "1"));
  assert(b3.proto->equalsString(b3, "3"));
  assert(b5.proto->equalsString(b5, "5"));
  assert(b7.proto->equalsString(b7, "7"));
  assert(b4.proto->up(b4).offset == 0);
  assert(b2.proto->up(b2).offset == b4.offset);
  assert(b6.proto->up(b6).offset == b4.offset);
  assert(b1.proto->up(b1).offset == b2.offset);
  assert(b3.proto->up(b3).offset == b2.offset);
  assert(b5.proto->up(b5).offset == b6.offset);
  assert(b7.proto->up(b7).offset == b6.offset);

  assert(!b.proto->find(b, "7").different);
  assert( b.proto->find(b, "8").different);

  ArenaRedBlackTreeNode f = t.proto->ll(t, "b", "7", 0);
  assert(f.offset == b7.offset);

  t.proto->free(t);
 }

void test2()                                                                    //Tcount
 {size_t N = 100;
  ArenaRedBlackTree t = makeArenaRedBlackTree();

  for(size_t i = 0; i < N; ++i)
   {char c[256]; sprintf(c, "%lu", i);
    t.proto->add(t, c);
   }
  assert(t.proto->count(t) == N);

  ArenaRedBlackTreeNode r = t.proto->root(t),
      n28 = t.proto->locate(t, "28"), n27 = n28.proto->up(n28), n25 = n27.proto->up(n27), n23 = n25.proto->up(n25);

  assert(r.proto->equalsString(r, "3"));
  assert(r.proto->height(r) == 11);
  assert(n23.proto->equalsString(n23, "23"));

  t.proto->check(t);
  t.proto->free(t);
 }

void test3()
 {int N = 256*256;
  ArenaRedBlackTree t = makeArenaRedBlackTree();

  for  (int i = 0; i < N; ++i)
   {char c[256]; sprintf(c, "%x", i);
    t.proto->add(t, c);
   }
  assert(t.proto->count(t) == (size_t)N);

  t.proto->check(t);

  ArenaRedBlackTreeNode r = t.proto->root(t);
  assert(r.proto->equalsString(r, "3"));
  assert(r.proto->height(r) == 33);

  t.proto->free(t);
 }

void test4()                                                                    //Theight //Tlocate //Troot
 {ArenaRedBlackTree t = makeArenaRedBlackTree();
  ArenaRedBlackTreeNode n = t.proto->locate(t, "0"); assert(!n.proto->valid(n));                                // Empty tree

  char c[4]; memset(c, 0, sizeof(c));

  for  (size_t i = 0; i < TEST_TREE_SIZE; ++i)
   {for(size_t j = 0; j < TEST_TREE_SIZE; ++j)
     {c[0] = '0' + i; c[1] = '0' + j;
      t.proto->add(t, c);
     }
   }

  ArenaRedBlackTreeNode r = t.proto->root(t);
  assert(r.proto->equalsString(r, "31"));
  assert(r.proto->height(r) == 11);

  ArenaRedBlackTreeNode f = t.proto->locate(t, "85");
  assert(f.proto->valid(f));
  assert(f.proto->height(f) == 2);

  t.proto->free(t);
 }

void test5()                                                                    //Tb2SumW8
 {size_t N = 20;
  ArenaRedBlackTree t = makeArenaRedBlackTree();
  char c[16]; memset(c, 0, sizeof(c));

  for(size_t i = 0; i < N; ++i)
   {sprintf(c, "%lu", i);
    const ArenaRedBlackTreeNode n = t.proto->add(t, c), e = n.proto->locate(n, "0"); assert(!e.proto->valid(e));        // Empty owned tree
    for(size_t j = 0; j < N; ++j)
     {sprintf(c, "%lu-%lu", i, j);
      n.proto->add(n, c);
     }
   }

  assert(t.proto->b2SumW8(t) == 9);
  ArenaRedBlackTreeNode n0 = t.proto->locate(t, "0"); assert(n0.proto->b2SumW8(n0) ==  70);
  ArenaRedBlackTreeNode n1 = t.proto->locate(t, "1"); assert(n1.proto->b2SumW8(n1) ==   9);
  ArenaRedBlackTreeNode n2 = t.proto->locate(t, "2"); assert(n2.proto->b2SumW8(n2) ==  90);
  ArenaRedBlackTreeNode n3 = t.proto->locate(t, "3"); assert(n3.proto->b2SumW8(n3) ==  56);
  ArenaRedBlackTreeNode n4 = t.proto->locate(t, "4"); assert(n4.proto->b2SumW8(n4) == 250);
  ArenaRedBlackTreeNode n5 = t.proto->locate(t, "5"); assert(n5.proto->b2SumW8(n5) == 172);
  ArenaRedBlackTreeNode n6 = t.proto->locate(t, "6"); assert(n6.proto->b2SumW8(n6) == 212);
  ArenaRedBlackTreeNode n7 = t.proto->locate(t, "7"); assert(n7.proto->b2SumW8(n7) == 115);
  ArenaRedBlackTreeNode n8 = t.proto->locate(t, "8"); assert(n8.proto->b2SumW8(n8) == 200);
  ArenaRedBlackTreeNode n9 = t.proto->locate(t, "9"); assert(n9.proto->b2SumW8(n9) ==  81);

  t.proto->free(t);
 }

void test6()
 {ArenaRedBlackTree t = makeArenaRedBlackTree();
  char c[4]; memset(c, 0, sizeof(c));

  for  (size_t i = 0; i < TEST_TREE_SIZE;        ++i)
   {for(size_t j =        TEST_TREE_SIZE; j > 0; --j)
     {c[0] = '0' + i; c[1] = '0' + j - 1;
      t.proto->add(t, c);
     }
   }
  ArenaRedBlackTreeNode r = t.proto->root(t);
  assert(!strcmp(r.proto->key(r), "16"));
  assert(r.proto->height(r) == 13);

  ArenaRedBlackTreeNode n86 = t.proto->locate(t, "86"),
        n76 = n86.proto->up(n86), n66 = n76.proto->up(n76), n56 = n66.proto->up(n66), n46 = n56.proto->up(n56);
  assert(n46.proto->equalsString(n46, "46"));

  t.proto->free(t);
 }

void test7()                                                                    //Tcheck //Tsprint
 {ArenaRedBlackTree t = makeArenaRedBlackTree();
  const size_t N = 10;
  char c[128]; memset(c, 0, sizeof(c));

  for  (size_t i = 0; i < N; ++i)
   {sprintf(c, "%lu", i);
    t.proto->add(t, c);
    sprintf(c, "%lu", 2*N - i);
    t.proto->add(t, c);
   }
  t.proto->check(t);

  ArenaRedBlackTreeNode r = t.proto->root(t);
  assert(!strcmp(r.proto->key(r), "19"));
  assert(r.proto->height(r) == 6);
  assert(t.proto->b2SumW8(t) == 223);

  t.proto->free(t);
 }

void test8()                                                                    //Tvalid
 {ArenaRedBlackTreeNode n = newArenaRedBlackTreeNode(({struct ArenaRedBlackTreeNode t = {proto: &ProtoTypes_ArenaRedBlackTreeNode};   t;})); assert(!n.proto->valid(n));
 }

void test9()
 {ArenaRedBlackTree t = makeArenaRedBlackTree();
  char c[4]; memset(c, 0, sizeof(c));
  for  (size_t i = 0; i < 2; ++i)
   {sprintf(c, "%lu", i);
    ArenaRedBlackTreeNode n = t.proto->add(t, c);
    assert(n.proto->valid(n));
    assert(n.proto->equalsString(n, c));
   }
  assert(t.proto->count(t) == 2);
  t.proto->check(t);
  t.proto->free(t);
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0, test1, test2, test3, test4, test5, test6, test7, test8, test9, 0};
//void (*tests[])(void) = {test5, 0};
  run_tests("ArenaRedBlackTree", repetitions, tests);
  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree
