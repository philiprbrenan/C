#line 1 "/home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree.c"
//------------------------------------------------------------------------------
// Red black tree in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
/*
 Each node of the base red black tree can, in turn, be the root of its own red
 black tree in the same arena.
*/
#define _GNU_SOURCE
#ifndef ArenaRedBlackTree_included
#define ArenaRedBlackTree_included
#include <stringBuffer.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Red Black Tree.
struct ArenaRedBlackTreeFound;                                                                  // Forward declare some of the structures used
struct ArenaRedBlackTreeNode;

typedef struct ArenaRedBlackTree                                                                // Arena Tree.
 {const struct ProtoTypes_ArenaRedBlackTree *proto;                                             // Methods associated with an arena tree
  struct ArenaRedBlackTreeArena *arena;                                                         // The arena for the arena tree
  int (*different)(const struct ArenaRedBlackTreeFound *f, const struct ArenaRedBlackTreeNode *n);
 } ArenaRedBlackTree;

#line 37 "/home/phil/c/z/arenaTree/arenaTree.c"
typedef struct ArenaRedBlackTreeNode                                                            //I Offset to the content of a node in the ArenaRedBlackTree.
 {ArenaRedBlackTree      tree;                                                                  // ArenaTree containing the item
  size_t offset;                                                                // Offset
  const struct ProtoTypes_ArenaRedBlackTreeNode *proto;                                         // Methods associated with nodes
 } ArenaRedBlackTreeNode;
#line 43 "/home/phil/c/z/arenaTree/arenaTree.c"
typedef struct ArenaRedBlackTreeArena                                                           //I Description of the block of memory that holds an ArenaRedBlackTree.  The individual nodes of the ArenaRedBlackTree cannot be cannot be freed individually, but the entire ArenaRedBlackTree can be freed. As the ArenaRedBlackTree uses offsets to store addresses the ArenaRedBlackTree can be moved to a different location in memory while retaining its structure.
 {size_t  size;                                                                 // The total size of the arena used to hold data associated with the tree.
  size_t  used;                                                                 // The number of bytes currently used.
  size_t  root;                                                                 // Offset to the root node
  void *  data;                                                                 // The arena containing the data associated with the tree
 } ArenaRedBlackTreeArena;
#line 50 "/home/phil/c/z/arenaTree/arenaTree.c"
typedef struct ArenaRedBlackTreeDescription                                                     //I The description of an arena tree which is written as the header record for dump files.
 {const size_t version;                                                         // Version of arena tree
  const size_t littleEndian;                                                    // Little endian if true
  const size_t s64b;                                                            // 64 bit address if true, else 32 bit
  const size_t used;                                                            // The number of bytes currently used by the arena.
  const size_t root;                                                            // Offset to the root node
  const time_t created;                                                         // Time in seconds since the epoch when this tree was created
 } ArenaRedBlackTreeDescription;
#line 26 "/home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree.c"

typedef struct ArenaRedBlackTreeContent                                                         // Content of a node in a red black tree
 {unsigned int left, right, up, height, tree, length;                           // Location of left, right, parent trees, sub tree height, owned sub tree, length of key.
  char key[0];
 } ArenaRedBlackTreeContent;

typedef struct ArenaRedBlackTreeFound                                                           // The result of trying to find a node in the tree.
 {const struct ProtoTypes_ArenaRedBlackTreeFound *proto;                                        // Methods
  char  * key;                                                                  // Key to find
  size_t  length;                                                               // Length of key to find
  ArenaRedBlackTree       tree;                                                                 // Tree being processed
  ArenaRedBlackTreeNode   last;                                                                 // Last node found during find.
  int     different;                                                            // The compare result on the last node found. If zero, the last node found was a match
 } ArenaRedBlackTreeFound;

#include <arenaRedBlackTree_prototypes.h>                                                      // Arena tree prototypes now that the relevant structures have been declared
#define makeLocalCopyOfArenaRedBlackTreeKey(string,stringLength,node) const size_t stringLength = content_ArenaRedBlackTreeNode(node)->length; char string[stringLength+1]; string[stringLength] = 0; memcpy(string, content_ArenaRedBlackTreeNode(node)->key, stringLength); // Copy the key and the length of the key of the specified node to the stack.

//D1 Pointers, offsets and allocations                                          // Locate items allocated in the arena

#line 66 "/home/phil/c/z/arenaTree/arenaTree.c"
static ArenaRedBlackTree makeArenaRedBlackTree                                                                  //I Create a new arena tree
 ()                                                                             // ArenaTree allocator
 {ArenaRedBlackTreeArena * const a = alloc(sizeof(ArenaRedBlackTreeArena));                                     // Allocate arena description
  const typeof(newArenaRedBlackTree(({struct ArenaRedBlackTree t = {arena: a, proto: &ProtoTypes_ArenaRedBlackTree}; t;}))) t = newArenaRedBlackTree(({struct ArenaRedBlackTree t = {arena: a, proto: &ProtoTypes_ArenaRedBlackTree}; t;}));                                                          // Arena tree we are creating

  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  a->used   = 0;                                                                // Length used so far
  a->root   = 0;                                                                // Root not set in ArenaRedBlackTree
  t.proto->node(t, "", 0);                                                                 // Initialize root node
  return t;
 }
#line 94 "/home/phil/c/z/arenaTree/arenaTree.c"
static void * pointer_ArenaRedBlackTree_size                                                    //IPV Return a temporary pointer to data in the arena of the specified ArenaRedBlackTree
 (const ArenaRedBlackTree      tree,                                                            // ArenaRedBlackTree
  const size_t offset)                                                          // Offset
 {if (offset > tree.arena->used)                                                // Check that the offset is valid
   {printStackBackTrace("Accessing area outside arena");
   }
  return (void *)(tree.arena->data + offset);                                   // Convert a non zero delta that is within the arena to a valid pointer
 }
#line 103 "/home/phil/c/z/arenaTree/arenaTree.c"
static ArenaRedBlackTreeContent * content_ArenaRedBlackTreeNode                                                 //IPV Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {return (ArenaRedBlackTreeContent *)pointer_ArenaRedBlackTree_size(node.tree, node.offset);
 }
#line 108 "/home/phil/c/z/arenaTree/arenaTree.c"
static void * key_pointer_ArenaRedBlackTreeNode                                                 //IV Get a temporary pointer to the key of a node.
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {return pointer_ArenaRedBlackTree_size(node.tree, node.offset + sizeof(ArenaRedBlackTreeContent));             // The key is stored immediately after the node in teh arena.
 }
#line 113 "/home/phil/c/z/arenaTree/arenaTree.c"
static size_t length_size_ArenaRedBlackTreeNode                                                 //I Get the length of a node
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {return node.proto->content(node)->length;
 }
#line 118 "/home/phil/c/z/arenaTree/arenaTree.c"
static  ArenaRedBlackTreeNode  nodeFromOffset_ArenaRedBlackTree_size                                            //IP Create a node to locate an allocation within the arena of a tree.
 (const ArenaRedBlackTree      tree,                                                            // Tree
  const size_t delta)                                                           // Delta within arena. A delta of zero represents no such node.
 {return newArenaRedBlackTreeNode(({struct ArenaRedBlackTreeNode t = {tree: tree, offset: delta, proto: &ProtoTypes_ArenaRedBlackTreeNode}; t;}));
 }
#line 135 "/home/phil/c/z/arenaTree/arenaTree.c"
static int equals_int_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                               //I Confirm two Nodes are equal
 (const ArenaRedBlackTreeNode a,                                                                // First offset
  const ArenaRedBlackTreeNode b)                                                                // Second offset
 {return a.tree.arena == b.tree.arena && a.offset == b.offset;
 }
#line 155 "/home/phil/c/z/arenaTree/arenaTree.c"
static  ArenaRedBlackTreeNode  allocate_offset_ArenaRedBlackTree_size                                           //IP Allocate memory within the arena of a tree and clear the allocated memory
 (const ArenaRedBlackTree      tree,                                                            // ArenaTree in which to allocate
  const size_t size)                                                            // Amount of memory required
 {if (tree.arena->used + size < tree.arena->size)                               // Allocate within existing arena
   {const typeof(tree.proto->nodeFromOffset(tree, tree.arena->used)) n = tree.proto->nodeFromOffset(tree, tree.arena->used);                                // Current offset to open memory
    memset(tree.arena->data+tree.arena->used, 0, size);                         // Clear new memory
    tree.arena->used += size;                                                   // Allocate
    return n;                                                                   // Return allocation
   }
  else                                                                          // Reallocate arena
   {const typeof(nextPowerOfTwo(tree.arena->size + size)) S = nextPowerOfTwo(tree.arena->size + size);                                // Round up memory required
    if (S < ((size_t)((1l<<(sizeof(((ArenaRedBlackTreeContent *)0)->length)*8)) - 1)))          // The maximum size of a delta if there are 8 bits in a byte
     {const typeof(realloc(tree.arena->data, S)) m = realloc(tree.arena->data, S);                                         // Reallocate arena - the old one will be copied into it if it is moved.
      if (m)                                                                    // Retry the memory allocation
       {tree.arena->data = m;                                                   // New arena
        tree.arena->size = S;                                                   // Arena size
        const typeof(tree.arena->used) u = tree.arena->used;                                                   // Length of free space
        memset(tree.arena->data + u, 0, S - u);                                 // Clear free space
        return tree.proto->allocate(tree, size);                                           // Allocate within arena
       }
     }
   }
  printStackBackTraceAndExit(2, "Requested arena too large\n");                 // The arena has become too large for the chosen size of offsets.
 }
#line 180 "/home/phil/c/z/arenaTree/arenaTree.c"
static size_t used_ArenaRedBlackTree                                                            //I Amount of space currently being used within the arena of a tree.
 (const ArenaRedBlackTree tree)                                                                 // Tree
 {return tree.arena->used;
 }
#line 185 "/home/phil/c/z/arenaTree/arenaTree.c"
static ArenaRedBlackTreeNode node_ArenaRedBlackTreeNode_ArenaRedBlackTree_string                                                //I Create a new ArenaRedBlackTree node with the specified key.
 (const ArenaRedBlackTree            tree,                                                      // Arena tree in which to create the node
  const void * const key,                                                       // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the tree is determined solely by the user.
  const size_t       length)                                                    // Length of the key, or if zero, I will use strlen
 {const typeof(tree.proto->allocate(tree, sizeof(ArenaRedBlackTreeContent) + length)) n = tree.proto->allocate(tree, sizeof(ArenaRedBlackTreeContent) + length);                               // Offset of space allocated for a node and key
  memcpy(n.proto->key(n), key, n.proto->content(n)->length = length);                           // Save length of key
  return tree.proto->nodeFromOffset(tree, n.offset);                                       // Return node
 }
#line 271 "/home/phil/c/z/arenaTree/arenaTree.c"
static int equalsString_ArenaRedBlackTreeNode_string                                            //I Check that the key of a node equals a string
 (const ArenaRedBlackTreeNode        node,                                                      // Node
  const char * const key)                                                       // Key
 {return node.proto->keyEquals(node, key, strlen(key));
 }
#line 47 "/home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree.c"

static void free_ArenaRedBlackTree                                                              // Free a tree in its entirety
 (const ArenaRedBlackTree tree)                                                                 // Tree
 {free(tree.arena->data);
  free(tree.arena);
 }

static  ArenaRedBlackTreeFound makeArenaRedBlackTreeFound                                                       //P Create a new found description
 (const ArenaRedBlackTree      tree,                                                            // The tree being searched
  char * const key,                                                             // The key to find
  const size_t length)                                                          // Length of the key to find
 {const ArenaRedBlackTreeNode n = newArenaRedBlackTreeNode(({struct ArenaRedBlackTreeNode t = {tree: tree, proto: &ProtoTypes_ArenaRedBlackTreeNode}; t;}));                                        // Sample node within tree
  return newArenaRedBlackTreeFound(({struct ArenaRedBlackTreeFound t = {tree: tree, key: key, length: length, last: n, proto: &ProtoTypes_ArenaRedBlackTreeFound}; t;}));
 }

//D1 Attributes                                                                 // Get and set the attributes of a node

static ArenaRedBlackTreeNode root_ArenaRedBlackTree                                                             // The root node in a red black tree
 (const ArenaRedBlackTree tree)                                                                 // Tree
 {return tree.proto->nodeFromOffset(tree, tree.arena->root);
 }

static size_t height_ArenaRedBlackTreeNode                                                      // Height of a sub tree starting at the specified node
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {return node.proto->content(node)->height;
 }

static int keyEquals_ArenaRedBlackTreeNode_string_size                                          // Key of the specified node as a zero terminated string
 (const ArenaRedBlackTreeNode        node,                                                      // Node
  const char * const key,                                                       // Expected key value
  const size_t       length)                                                    // Length of expected key value
 {const typeof(node.proto->length(node)) l = node.proto->length(node);                                                            // Recorded length of node
  if (l != length) return 0;                                                    // Different lengths means the keys is unequal
  return !memcmp(node.proto->key(node), key, l);                                           // Compare memory
 }

static ArenaRedBlackTreeNode up_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                     // Parent node for the specified node.
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {return node.tree.proto->nodeFromOffset(node.tree, node.proto->content(node)->up);
 }

static ArenaRedBlackTreeNode left_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                   // Left child node below the specified parent node.
 (const ArenaRedBlackTreeNode     parent)                                                       // Parent
 {return parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->left);
 }

static ArenaRedBlackTreeNode right_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                  // Right child node below the specified parent node.
 (const ArenaRedBlackTreeNode     parent)                                                       // Parent
 {return  parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->right);
 }

static ArenaRedBlackTreeNode ownedTreeRoot_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                          //P Root of tree owned by this node if there is  one, else the returned node has an offset of zero.
 (const ArenaRedBlackTreeNode     parent)                                                       // Parent
 {return parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->tree);
 }

static void setHeight_ArenaRedBlackTreeNode                                                     //P Save the height of the sub tree starting at the specified node
 (const ArenaRedBlackTreeNode  parent,                                                          // Parent
  const size_t height)                                                          // Height
 {parent.proto->content(parent)->height = height;
 }

static void setUp_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                   //P Set parent node of specified child node
 (const ArenaRedBlackTreeNode child,                                                            // Parent
  const ArenaRedBlackTreeNode parent)                                                           // Child
 {child.proto->content(child)->up = parent.offset;
 }

static void setLeft_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                 //P Set left child of specified parent.
 (const ArenaRedBlackTreeNode parent,                                                           // Parent
  const ArenaRedBlackTreeNode left)                                                             // Left child
 {parent.proto->content(parent)->left = left.offset;
 }

static void setRight_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                //P Parent parent for a specified parent in a red black tree
 (const ArenaRedBlackTreeNode parent,                                                           // Parent
  const ArenaRedBlackTreeNode right)                                                            // Right child
 {parent.proto->content(parent)->right = right.offset;
 }

static void setTree_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode                                                 //P Set the other tree located by this node.
 (const ArenaRedBlackTreeNode parent,                                                           // Parent
  const ArenaRedBlackTreeNode tree)                                                             // Tree to be added to parent
 {parent.proto->content(parent)->tree = tree.offset;
 }

static int valid_ArenaRedBlackTreeNode                                                          // Check that the specified node is valid.
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {return node.offset;                                                           // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

static int isRoot_ArenaRedBlackTreeNode                                                         // Check that the specified node is valid.
 (const ArenaRedBlackTreeNode node)                                                             // Node
 {return !(node.proto->up(node)).offset;                                                   // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

//D1 Find                                                                       // Find a key if it exists within the base tree or an owned tree.

static int cmpArenaRedBlackTree                                                                 // Compare key sought with current key
 (const char *const K,                                                          // Key sought
  const size_t      L,                                                          // key length
  const char *const k,                                                          // Current key
  const size_t      l)                                                          // Current key length
 {if (l < L)                                                                    // Current key shorter than sought key
   {const int i = strncmp(K, k, l);
    return i ? i : +1;
   }
  else if (L < l)                                                               // Current key longer than sought key
   {const int i = strncmp(K, k, L);
    return i ? i : -1;
   }
  else return strncmp(K, k, L);                                                 // Equal length keys
 }

static ArenaRedBlackTreeFound find_ArenaRedBlackTreeFound_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string                                   //P Find a key if it exists within the tree starting at this node.
 (ArenaRedBlackTreeFound      found,                                                            // Found definition
  const ArenaRedBlackTreeNode root)                                                             // The root node at which the tree starts
 {for(ArenaRedBlackTreeNode p = root; p.proto->valid(p);)                                               // Search down through tree
   {found.last.offset  = p.offset;                                              // Record last parent compared with key sought
    makeLocalCopyOfArenaRedBlackTreeKey(k, l, p);                                               // Local copy of key

    const typeof(found.length) L = found.length; const typeof(found.key) K = found.key;
    const typeof(found.different = cmpArenaRedBlackTree(K, L, k, l)) i = found.different = cmpArenaRedBlackTree(K, L, k, l);                                      // Compare key sought with current key

    if (!i) return found;                                                       // Found

    p.offset = i < 0 ? p.proto->content(p)->left : p.proto->content(p)->right;                  // Continue left or right
   }

  return found;                                                                 // Found
 }

static ArenaRedBlackTreeNode locate_ArenaRedBlackTreeNode_string                                                // Locate the node with the specified key if it exists within the tree owned by the specified node.
 (const ArenaRedBlackTreeNode        node,                                                      // Node
  const char * const key,                                                       // Key to find
  const size_t       length)                                                    // Length of the key to find
 {ArenaRedBlackTreeNode p = node.proto->ownedTreeRoot(node);                                               // Root node
  if (!p.proto->valid(p)) return p;                                                     // Empty tree
  for(;;)                                                                       // Search down through tree
   {makeLocalCopyOfArenaRedBlackTreeKey(k, l, p);
    const typeof(cmpArenaRedBlackTree(key, length, k, l)) i = cmpArenaRedBlackTree(key, length, k, l);                                                // Compare key sought with current key

    if (!i) return p;                                                           // Found

    p.offset = i < 0 ? p.proto->content(p)->left : p.proto->content(p)->right;                  // Continue left or right
    if (!p.proto->valid(p)) return p;                                                   // Not found
   }

  printStackBackTrace("Should not be able to reach here\n");
 }

static ArenaRedBlackTreeNode locate_ArenaRedBlackTree_string                                                    // Locate the node with the specified key if it exists within the specified tree.
 (const ArenaRedBlackTree            tree,                                                      // ArenaRedBlackTree
  const char * const key,                                                       // Key to find
  const size_t       length)                                                    // Length of the key to find
 {ArenaRedBlackTreeNode p = tree.proto->root(tree);                                                        // Root node
  if (!p.proto->valid(p)) return p;                                                     // Empty tree
  for(;;)                                                                       // Search down through tree
   {makeLocalCopyOfArenaRedBlackTreeKey (k, l, p);
    const typeof(cmpArenaRedBlackTree(key, length, k, l)) i = cmpArenaRedBlackTree(key, length, k, l);                                                // Compare key sought with current key
    if (!i) return p;                                                           // Found
    p.offset = i < 0 ? p.proto->content(p)->left : p.proto->content(p)->right;                  // Continue left or right
    if (!p.proto->valid(p)) return p;                                                   // Not found
   }

  printStackBackTrace("Should not be able to reach here\n");
 }

static void check_ArenaRedBlackTree                                                             // Check the integrity of the tree
 (const ArenaRedBlackTree tree)                                                                 // Tree to check
 {void check(ArenaRedBlackTreeNode p)                                                           // Check a node
   {if (!p.proto->valid(p)) return;
                   makeLocalCopyOfArenaRedBlackTreeKey(pk, pl, p);
    const typeof(p.proto->left(p)) l = p.proto->left(p);  makeLocalCopyOfArenaRedBlackTreeKey(lk, ll, l);
    const typeof(p.proto->right(p)) r = p.proto->right(p); makeLocalCopyOfArenaRedBlackTreeKey(rk, rl, r);
    const typeof(l.proto->up(l)) L = l.proto->up(l);    makeLocalCopyOfArenaRedBlackTreeKey(Lk, Ll, L);
    const typeof(r.proto->up(r)) R = r.proto->up(r);    makeLocalCopyOfArenaRedBlackTreeKey(Rk, Rl, R);
    if (l.proto->valid(l) && !p.proto->equals(p, l.proto->up(l))) printStackBackTraceAndExit
        (1, "Bad left  up %s to %s should be %s\n", lk, Lk, pk);
    if (r.proto->valid(r) && !p.proto->equals(p, r.proto->up(r))) printStackBackTraceAndExit
        (2, "Bad right up %s to %s should be %s\n", rk, Rk, pk);
    if (l.proto->valid(l) && r.proto->valid(r))
     {const typeof(l.proto->height(l)) lh = l.proto->height(l); const typeof(r.proto->height(r)) rh = r.proto->height(r);
      if (lh > 2 * rh + 1 || rh > 2 * lh + 1 ) printStackBackTraceAndExit
        (3, "Unbalanced tree at %s with left %lu versus right %lu\n", pk,lh,rh);
     }
    check(l);                                                                   // Check left
    check(r);                                                                   // Check right
   }
  check(tree.proto->root(tree));                                                           // Check from root
 }

static  ArenaRedBlackTreeFound find_ArenaRedBlackTreeFound_ArenaRedBlackTreeNode_string                                         //P Find a key if it exists within the tree owned by the specified node.
 (const ArenaRedBlackTreeNode  node,                                                            // The node owning the tree to search
  char * const key,                                                             // The key to find
  const size_t length)                                                          // Length of the key to find
 {const typeof(makeArenaRedBlackTreeFound(node.tree, key, length)) f = makeArenaRedBlackTreeFound(node.tree, key, length);                                       // Status of find
  const typeof(node.proto->ownedTreeRoot(node)) r = node.proto->ownedTreeRoot(node);                                                     // Root node of owned tree
  if (!r.proto->valid(r)) return f;                                                     // Empty owned tree
  return f.proto->find(f, r);                                                           // Search the non empty owned tree starting at the specified node.
 }

static  ArenaRedBlackTreeFound find_ArenaRedBlackTreeFound_ArenaRedBlackTree_string                                             //P Find a key if it exists within the base tree.
 (const ArenaRedBlackTree      tree,                                                            // The base tree to search
  char * const key,                                                             // The key to find
  const size_t length)                                                          // Length of the key to find
 {const typeof(makeArenaRedBlackTreeFound(tree, key, length)) f = makeArenaRedBlackTreeFound(tree, key, length);                                            // Status of find
  if (!tree.arena->root) return f;                                              // Empty tree
  return f.proto->find(f, tree.proto->nodeFromOffset(tree, tree.arena->root));                     // Search the non empty base tree starting at the specified node.
 }

static ArenaRedBlackTreeNode ll_ArenaRedBlackTreeNode_ArenaRedBlackTree_strings                                                 // Search through a series of owned trees starting at the base tree as directed by the specified keys
 (const ArenaRedBlackTree   tree,                                                               // The base tree to search from
  const char * const keys,                                                      // Zero terminated list of keys
  ...)                                                                          // Following keys
 {va_list va;
  va_start(va, keys);
  ArenaRedBlackTreeNode p = tree.proto->locate(tree, keys, strlen(keys));
  for(;p.proto->valid(p);)
   {const typeof(va_arg(va, char *)) k = va_arg(va, char *);
    if (!k) break;
    p.offset = p.proto->locate(p, k, strlen(k)).offset;
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

   const typeof(f.key) key = f.key;                                                                 // The key to add
  const typeof(f.last.tree) tree = f.last.tree;                                                           // The base tree
     const typeof(tree.proto->node(tree, key, strlen(key))) n = tree.proto->node(tree, key, strlen(key)); const typeof(f.last) p = f.last;                             // Create new node under parent

  if (f.different < 0) p.proto->setLeft(p, n); else p.proto->setRight(p, n);                    // Insert node

  n.proto->setUp(n, p);                                                                 // Set parent of inserted node

  for(ArenaRedBlackTreeNode p = newArenaRedBlackTreeNode(({struct ArenaRedBlackTreeNode t = {tree: n.tree, offset: n.offset, proto: &ProtoTypes_ArenaRedBlackTreeNode}; t;}));                      // Balance nodes along path back to root
            p.proto->valid(p);
            p.offset = p.proto->up(p).offset)
   {size_t height(ArenaRedBlackTreeNode p)                                                      // Height of a node
     {return p.proto->valid(p) ? p.proto->height(p) : 0;
     }

    void setHeight(ArenaRedBlackTreeNode p)                                                     // Set the height of a node
     {const typeof(height(p.proto->left(p))) L = height(p.proto->left(p));
      const typeof(height(p.proto->right(p))) R = height(p.proto->right(p));
      p.proto->setHeight(p, 1 + (L > R ? L : R));
     }

    ArenaRedBlackTreeNode balance                                                               // Balance node
     (const int d)                                                              // Rotate left if true
     {const typeof(p.proto->up(p)) q = p.proto->up(p); const typeof(p.proto->left(p)) l = p.proto->left(p); const typeof(p.proto->right(p)) r = p.proto->right(p); const typeof(d ? r : l) rl = d ? r : l;

      if (!q.proto->valid(q))                                                           // Balance the root node
       {if (!owner.proto->valid(owner)) tree.arena->root = rl.offset;                       // New root node in base tree
        else owner.proto->setTree(owner, rl);                                               // New root node in owned tree
        if (d)
         {p.proto->setRight(p, r.proto->left(r));  r.proto->setLeft(r, p);
          const typeof(p.proto->right(p)) R = p.proto->right(p); if (R.proto->valid(R))  R.proto->setUp(R, p);
         }
        else
         {p.proto->setLeft(p, l.proto->right(l)); l.proto->setRight(l, p);
          const typeof(p.proto->left(p)) L = p.proto->left(p);  if (L.proto->valid(L))  L.proto->setUp(L, p);
         }

        rl.proto->content(rl)->up = 0;                                                   // Nothing above this node as it is now the new root node
       }

      else                                                                      // Balance a node that is not the root node
       {rl.proto->setUp(rl, q);
        if (p.proto->equals(p, q.proto->right(q))) q.proto->setRight(q, rl); else q.proto->setLeft(q, rl);
        if (d)
         {p.proto->setRight(p, r.proto->left(r));
          const typeof(p.proto->right(p)) R = p.proto->right(p); if (R.proto->valid(R)) R.proto->setUp(R, p);
          r.proto->setLeft(r, p);
         }
        else
         {p.proto->setLeft(p, l.proto->right(l));
          const typeof(p.proto->left(p)) L = p.proto->left(p);  if (L.proto->valid(L)) L.proto->setUp(L, p);
          l.proto->setRight(l, p);
         }
        setHeight(q);
       }

      p.proto->setUp(p, rl);
      setHeight(p);
      setHeight(rl);
      return rl;                                                                // The node now above the input node is now balanced
     }

    const typeof(height(p.proto->left(p))) L = height(p.proto->left(p)); const typeof(height(p.proto->right(p))) R = height(p.proto->right(p));                                // Left and right depths
    if      (2*L + 1 < R) p.offset = balance(1).offset;                         // Balance left or right if necessary
    else if (2*R + 1 < L) p.offset = balance(0).offset;
    else     setHeight(p);                                                      // Balanced: propogate height
   }

  return n;                                                                     // The node added
 }

static  ArenaRedBlackTreeNode   add_ArenaRedBlackTreeNode_ArenaRedBlackTree_string                                              // Add a new key if not already present in the specified base tree.
 (const ArenaRedBlackTree       tree,                                                           // The tree to search
  char  * const key,                                                            // The key to add
  const size_t  length)                                                         // Length of the key to find
 {const typeof(tree.proto->find(tree, key, length)) f = tree.proto->find(tree, key, length);                                                 // Try to find the key

  if (!f.last.proto->valid(f.last))                                                          // Empty tree
   {const typeof(tree.proto->node(tree, key, length)) root = tree.proto->node(tree, key, length);                                            // Add new key in a node
    tree.arena->root = root.offset;                                             // Make the new node the root node
    return root;                                                                // Create node
   }
  const ArenaRedBlackTreeNode invalid = newArenaRedBlackTreeNode(({struct ArenaRedBlackTreeNode t = {proto: &ProtoTypes_ArenaRedBlackTreeNode};   t;}));
  return f.proto->add(f, invalid);                                                      // Non empty tree - add the node below the root node
 }

static  ArenaRedBlackTreeNode   add_ArenaRedBlackTreeNode_ArenaRedBlackTreeNode_string                                          // Add a new key if not already present in the tree owned by the specified node.
 (const ArenaRedBlackTreeNode   node,                                                           // The tree to search
  char  * const key,                                                            // The key to add
  const size_t  length)                                                         // Length of the key to find
 {const ArenaRedBlackTree tree  = node.tree;                                                    // Tree to search
       const typeof(node.proto->find(node, key, length)) f = node.proto->find(node, key, length);                                            // Try to find the key
  if (!f.last.proto->valid(f.last))                                                          // Empty tree
   {const typeof(tree.proto->node(tree, key, length)) root = tree.proto->node(tree, key, length);                                            // Add new key in a node
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

static void dumpStderr_ArenaRedBlackTree                                                        //P Print a tree on stderr
 (const ArenaRedBlackTree tree)                                                                 // ArenaRedBlackTree
 {void print(const ArenaRedBlackTreeNode node, size_t depth)                                    // Print to allocated string
   {if (!node.proto->valid(node)) return;                                                 // No such node
    print(node.proto->left(node), depth+1);                                                // Print left
    fprintf(stderr, "%lu ", depth);                                             // Depth
    for(size_t i = 0; i < depth; ++i) fprintf(stderr, "  ");                    // Spacer
    makeLocalCopyOfArenaRedBlackTreeKey(k, l, node);                                            // Local copy of key
    fprintf(stderr, "%s", k);
    const size_t N = depth > 80 ? 80 : 80 - depth;                              // Separator size
    for(size_t i = 0; i < N; ++i) fprintf(stderr, "..");                        // Separator
    fprintf(stderr, "\n");
    print(node.proto->right(node), depth+1);                                               // Print right
   }

  const typeof(tree.proto->root(tree)) root = tree.proto->root(tree);                                                           // Start at root
  print(root, 0);                                                               // Print to buffer
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

#define TEST_TREE_SIZE 10

void test0()                                                                    //TnewArenaRedBlackTree //Tadd //Tleft //Tright //Tkey //Tup //Tfree
 {const typeof(makeArenaRedBlackTree()) t = makeArenaRedBlackTree();
  t.proto->add(t, "b",  1);
  t.proto->add(t, "a",  1);
  t.proto->add(t, "c",  1);
  t.proto->add(t, "bb", 2);
  t.proto->add(t, "cc", 2);

  const typeof(t.proto->root(t)) b = t.proto->root(t); const typeof(b.proto->left(b)) a = b.proto->left(b); const typeof(b.proto->right(b)) c = b.proto->right(b); const typeof(c.proto->left(c)) bb = c.proto->left(c); const typeof(c.proto->right(c)) cc = c.proto->right(c);

  assert( b.proto->keyEquals(b, "b",  1));
  assert( a.proto->keyEquals(a, "a",  1));
  assert( c.proto->keyEquals(c, "c",  1));
  assert( bb.proto->keyEquals(bb, "bb", 2));
  assert( cc.proto->keyEquals(cc, "cc", 2));
  assert( b.proto->isRoot(b));
  assert( b.proto->equals(b, a.proto->up(a)));
  assert( b.proto->equals(b, c.proto->up(c)));
  assert( c.proto->equals(c, bb.proto->up(bb)));
  assert( c.proto->equals(c, cc.proto->up(cc)));

  assert(!t.proto->find(t, "cc", 2).different);
  assert( t.proto->find(t, "dd", 2).different);

  t.proto->free(t);
 }

void test1()                                                                    //Tll
 {const typeof(makeArenaRedBlackTree()) t = makeArenaRedBlackTree();
  t.proto->add(t, "a", 1); t.proto->add(t, "b", 1);
  const typeof(t.proto->root(t)) a = t.proto->root(t); const typeof(a.proto->right(a)) b = a.proto->right(a);

  b.proto->add(b, "4", 1); b.proto->add(b, "2", 1); b.proto->add(b, "6", 1);
  b.proto->add(b, "1", 1); b.proto->add(b, "3", 1); b.proto->add(b, "5", 1); b.proto->add(b, "7", 1);

  const typeof(b.proto->ownedTreeRoot(b)) b4 = b.proto->ownedTreeRoot(b); const typeof(b4.proto->left(b4)) b2 = b4.proto->left(b4); const typeof(b4.proto->right(b4)) b6 = b4.proto->right(b4);
  const typeof(b2.proto->left(b2)) b1 = b2.proto->left(b2); const typeof(b2.proto->right(b2)) b3 = b2.proto->right(b2); const typeof(b6.proto->left(b6)) b5 = b6.proto->left(b6); const typeof(b6.proto->right(b6)) b7 = b6.proto->right(b6);
  assert(b4.proto->equalsString(b4, "4"));
  assert(b2.proto->equalsString(b2, "2"));
  assert(b6.proto->equalsString(b6, "6"));
  assert(b1.proto->equalsString(b1, "1"));
  assert(b3.proto->equalsString(b3, "3"));
  assert(b5.proto->equalsString(b5, "5"));
  assert(b7.proto->equalsString(b7, "7"));
  assert(b4.proto->up(b4).offset == 0);
  assert(b4.proto->equals(b4, b2.proto->up(b2)));
  assert(b4.proto->equals(b4, b6.proto->up(b6)));
  assert(b2.proto->equals(b2, b1.proto->up(b1)));
  assert(b2.proto->equals(b2, b3.proto->up(b3)));
  assert(b6.proto->equals(b6, b5.proto->up(b5)));
  assert(b6.proto->equals(b6, b7.proto->up(b7)));

  assert(!b.proto->find(b, "7", 1).different);
  assert( b.proto->find(b, "8", 1).different);

  const typeof(t.proto->ll(t, "b", "7", 0)) f = t.proto->ll(t, "b", "7", 0);
  assert(f.offset == b7.offset);

  t.proto->free(t);
 }

void test2()                                                                    //Tcount
 {size_t N = 100;
  const typeof(makeArenaRedBlackTree()) t = makeArenaRedBlackTree();

  for(size_t i = 0; i < N; ++i)
   {char c[256]; sprintf(c, "%lu", i);
    t.proto->add(t, c, strlen(c));
   }

  assert(t.proto->count(t) == N);

  const typeof(t.proto->root(t)) r = t.proto->root(t);
  const typeof(t.proto->locate(t, "28", 2)) n28 = t.proto->locate(t, "28", 2); const typeof(n28.proto->up(n28)) n27 = n28.proto->up(n28); const typeof(n27.proto->up(n27)) n25 = n27.proto->up(n27); const typeof(n25.proto->up(n25)) n23 = n25.proto->up(n25);

  assert(r.proto->equalsString(r, "3"));
  assert(r.proto->height(r) == 11);
  assert(n23.proto->equalsString(n23, "23"));

  t.proto->check(t);
  t.proto->free(t);
 }

void test3()
 {int N = 256*256;
  const typeof(makeArenaRedBlackTree()) t = makeArenaRedBlackTree();

  for(int i = 0; i < N; ++i)
   {char c[256]; sprintf(c, "%x", i); t.proto->add(t, c, strlen(c));
   }
  assert(t.proto->count(t) == (size_t)N);
  t.proto->check(t);
         const typeof(t.proto->root(t)) r = t.proto->root(t);
  assert(r.proto->equalsString(r, "3"));
  assert(r.proto->height(r) == 33);

  t.proto->free(t);
 }

void test4()                                                                    //Theight //Tlocate //Troot
 {const typeof(makeArenaRedBlackTree()) t = makeArenaRedBlackTree();
  const typeof(t.proto->locate(t, "0", 1)) n = t.proto->locate(t, "0", 1); assert(!n.proto->valid(n));

  char c[4]; memset(c, 0, sizeof(c));

  for  (size_t i = 0; i < TEST_TREE_SIZE; ++i)
   {for(size_t j = 0; j < TEST_TREE_SIZE; ++j)
     {c[0] = '0' + i; c[1] = '0' + j;
      t.proto->add(t, c, strlen(c));
     }
   }

         const typeof(t.proto->root(t)) r = t.proto->root(t);
  assert(r.proto->equalsString(r, "31"));
  assert(r.proto->height(r) == 11);

         const typeof(t.proto->locate(t, "85", 2)) f = t.proto->locate(t, "85", 2);
  assert(f.proto->valid(f));
  assert(f.proto->height(f) == 2);

  t.proto->free(t);
 }

void test5()                                                                    //Tb2SumW8
 {size_t N = 20;
  const typeof(makeArenaRedBlackTree()) t = makeArenaRedBlackTree();
  char c[16]; memset(c, 0, sizeof(c));

  for(size_t i = 0; i < N; ++i)
   {sprintf(c, "%lu", i);
    const typeof(t.proto->add(t, c, strlen(c))) n = t.proto->add(t, c, strlen(c)); const typeof(n.proto->locate(n, "0", 1)) e = n.proto->locate(n, "0", 1); assert(!e.proto->valid(e));
    for(size_t j = 0; j < N; ++j)
     {sprintf(c, "%lu-%lu", i, j);
      n.proto->add(n, c, strlen(c));
     }
   }

  const char *a[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 0};
  for(const char * const *p = a; *p; ++p)
   {const typeof(t.proto->locate(t, *p, 1)) n = t.proto->locate(t, *p, 1); assert(n.proto->equalsString(n, *p));
   }

  t.proto->free(t);
 }

void test6()
 {const typeof(makeArenaRedBlackTree()) t = makeArenaRedBlackTree();
  for  (size_t i = 0; i < TEST_TREE_SIZE;        ++i)
   {for(size_t j =        TEST_TREE_SIZE; j > 0; --j)
     {char c[256]; const size_t l = sprintf(c, "%lu%lu", i, j);
      t.proto->add(t, c, l);
     }
   }
  const typeof(t.proto->root(t)) r = t.proto->root(t);

  assert(r.proto->keyEquals(r, "16", 2));
  assert(r.proto->height(r) == 12);

  const typeof(t.proto->locate(t, "86", 2)) n86 = t.proto->locate(t, "86", 2);
  const typeof(n86.proto->up(n86)) n810 = n86.proto->up(n86);
  const typeof(n810.proto->up(n810)) n76 = n810.proto->up(n810); const typeof(n76.proto->up(n76)) n66 = n76.proto->up(n76);
  const typeof(n66.proto->up(n66)) n56 = n66.proto->up(n66); const typeof(n56.proto->up(n56)) n36 = n56.proto->up(n56);
  assert(n36.proto->equalsString(n36, "36"));

  t.proto->free(t);
 }

void test7()                                                                    //Tcheck //Tsprint //TisRoot //TkeyEquals
 {const typeof(makeArenaRedBlackTree()) t = makeArenaRedBlackTree();
  const size_t N = 10;
  char c[128]; memset(c, 0, sizeof(c));

  for  (size_t i = 0; i < N; ++i)
   {sprintf(c, "%lu", i);
    t.proto->add(t, c, strlen(c));
    sprintf(c, "%lu", 2*N - i);
    t.proto->add(t, c, strlen(c));
   }
  t.proto->check(t);

  const typeof(t.proto->root(t)) r = t.proto->root(t);

  assert(r.proto->isRoot(r));
  assert(r.proto->keyEquals(r, "19", 2));
  assert(r.proto->height(r) == 6);

  t.proto->free(t);
 }

void test8()                                                                    //Tvalid //TcmpArenaRedBlackTree
 {ArenaRedBlackTreeNode n = newArenaRedBlackTreeNode(({struct ArenaRedBlackTreeNode t = {proto: &ProtoTypes_ArenaRedBlackTreeNode};   t;})); assert(!n.proto->valid(n));

  assert(cmpArenaRedBlackTree("aa", 2, "aa", 1) ==  1);
  assert(cmpArenaRedBlackTree("aa", 2, "aa", 2) ==  0);
  assert(cmpArenaRedBlackTree("aa", 1, "aa", 2) == -1);
 }

void test9()
 {const typeof(makeArenaRedBlackTree()) t = makeArenaRedBlackTree();
  char c[4]; memset(c, 0, sizeof(c));
  for  (size_t i = 0; i < 2; ++i)
   {sprintf(c, "%lu", i);
           const typeof(t.proto->add(t, c, strlen(c))) n = t.proto->add(t, c, strlen(c));
    assert(n.proto->valid(n));
    assert(n.proto->equalsString(n, c));
   }
  assert(t.proto->count(t) == 2);
  t.proto->check(t);
  t.proto->free(t);
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0, test1, test2, test3, test4,
                           test5, test6, test7, test8, test9, 0};
//  void (*tests[])(void) = {test6, 0};
  run_tests("ArenaRedBlackTree", repetitions, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaRedBlackTree/arenaRedBlackTree
