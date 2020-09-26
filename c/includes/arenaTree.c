#line 1 "/home/phil/c/z/arenaTree/arenaTree.c"
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
#ifndef ArenaTree_included
#define ArenaTree_included
#include <stringBuffer.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an ArenaTree.
struct ArenaTreeFound;                                                                  // Forward declare some of the structures used
struct ArenaTreeNode;

typedef struct ArenaTree                                                                // ArenaTree
 {const struct ProtoTypes_ArenaTree *proto;                                             // Methods associated with an ArenaTree
  struct ArenaTreeArena *arena;                                                         // The arena for the ArenaTree
//  int (*different)(const struct ArenaTreeFound *f, const struct ArenaTreeNode *n);
 } ArenaTree;

#line 37 "/home/phil/c/z/arenaList/arenaList.c"
typedef struct ArenaTreeNode                                                            //I Offset to the content of a node in the ArenaTree.
 {ArenaTree      list;                                                                  // ArenaTree containing the item
  size_t offset;                                                                // Offset
  const struct ProtoTypes_ArenaTreeNode *proto;                                         // Methods associated with nodes
 } ArenaTreeNode;
#line 43 "/home/phil/c/z/arenaList/arenaList.c"
typedef struct ArenaTreeArena                                                           //I Description of the block of memory that holds an ArenaTree.  The individual nodes of the ArenaTree cannot be cannot be freed individually, but the entire ArenaTree can be freed. As the ArenaTree uses offsets to store addresses the ArenaTree can be moved to a different location in memory while retaining its structure.
 {size_t size;                                                                  // The total size of the arena used to hold data associated with the ArenaTree.
  size_t used;                                                                  // The number of bytes currently used.
  size_t root;                                                                  // Offset to the root node
  size_t width;                                                                 // Width of a fixed data area added to each node measured in bytes
  void * data;                                                                  // The arena containing the data associated with the ArenaTree
 } ArenaTreeArena;
#line 51 "/home/phil/c/z/arenaList/arenaList.c"
typedef struct ArenaTreeDescription                                                     //I The description of an ArenaTree which is written as the header record for dump files.
 {const size_t version;                                                         // Version of ArenaTree
  const size_t littleEndian;                                                    // Little endian if true
  const size_t s64b;                                                            // 64 bit address if true, else 32 bit
  const size_t used;                                                            // The number of bytes currently used by the arena.
  const size_t root;                                                            // Offset to the root node
  const size_t width;                                                           // Width of user data associated with each node
  const time_t created;                                                         // Time in seconds since the epoch when this ArenaTree was created
 } ArenaTreeDescription;
#line 28 "/home/phil/c/z/arenaTree/arenaTree.c"

typedef struct ArenaTreeContent                                                         // Content of a node in a ArenaTree
 {unsigned int left, right, up, height, tree, length;                           // Location of left, right, parent trees, sub tree height, owned sub tree, length of key.
  char key[0];
 } ArenaTreeContent;

typedef struct ArenaTreeFound                                                           // The result of trying to find a node in the ArenaTree.
 {const struct ProtoTypes_ArenaTreeFound *proto;                                        // Methods
  char  * key;                                                                  // Key to find
  size_t  length;                                                               // Length of key to find
  ArenaTree       tree;                                                                 // ArenaTree being processed
  ArenaTreeNode   last;                                                                 // Last node found during find.
  int     different;                                                            // The compare result on the last node found. If zero, the last node found was a match
 } ArenaTreeFound;

#include <arenaTree_prototypes.h>                                                      // ArenaTree prototypes now that the relevant structures have been declared
#define makeLocalCopyOfArenaTreeKey(string,stringLength,node) const size_t stringLength = content_ArenaTreeNode(node)->length; char string[stringLength+1]; string[stringLength] = 0; memcpy(string, key_pointer_ArenaTreeNode(node), stringLength); // Copy the key and the length of the key of the specified node to the stack.

//D1 Pointers, offsets and allocations                                          // Locate items allocated in the arena

#line 68 "/home/phil/c/z/arenaList/arenaList.c"
static ArenaTree makeArenaTreeWithWidth                                                         //I Create a new ArenaTree with the specified width for the optional user data associated with each node.
 (const size_t width)                                                           // Width
 {ArenaTreeArena * const a = alloc(sizeof(ArenaTreeArena));                                     // Allocate arena description
  const typeof(newArenaTree(({struct ArenaTree t = {arena: a, proto: &ProtoTypes_ArenaTree}; t;}))) t = newArenaTree(({struct ArenaTree t = {arena: a, proto: &ProtoTypes_ArenaTree}; t;}));                                                          // ArenaTree we are creating

  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  a->used   = 0;                                                                // Length used so far
  a->root   = 0;                                                                // Root not set in ArenaTree
  a->width  = width;                                                            // Width of user data associated with each node
  t.proto->node(t, "", 0);                                                              // Initialize root node
  return t;
 }
#line 83 "/home/phil/c/z/arenaList/arenaList.c"
static ArenaTree makeArenaTree                                                                  //I Create a new ArenaTree
 ()                                                                             // ArenaTreeallocator
 {return makeArenaTreeWithWidth(0);                                                     // Allocate arena description
 }
#line 102 "/home/phil/c/z/arenaList/arenaList.c"
static void * pointer_ArenaTree_size                                                    //IPV Return a temporary pointer to data in the arena of the specified ArenaTree
 (const ArenaTree      list,                                                            // ArenaTree
  const size_t offset)                                                          // Offset
 {if (offset > list.arena->used)                                                // Check that the offset is valid
   {printStackBackTrace("Accessing area outside arena");
   }
  return (void *)(list.arena->data + offset);                                   // Convert a non zero delta that is within the arena to a valid pointer
 }
#line 111 "/home/phil/c/z/arenaList/arenaList.c"
static ArenaTreeContent * content_ArenaTreeNode                                                 //IPV Convert a node offset to an address so that the content of a node can be updated in situ as long as the ArenaTree is not reallocated to a different position.
 (const ArenaTreeNode node)                                                             // Node
 {return (ArenaTreeContent *)pointer_ArenaTree_size(node.list, node.offset);
 }
#line 116 "/home/phil/c/z/arenaList/arenaList.c"
static size_t width_size_ArenaTree                                                      //I Get the width of the data area for a ArenaTree
 (const ArenaTree list)                                                                 // Node
 {return list.arena->width;
 }
#line 121 "/home/phil/c/z/arenaList/arenaList.c"
static size_t width_size_ArenaTreeNode                                                  //I Get the width of the data area for a node
 (const ArenaTreeNode node)                                                             // Node
 {return node.list.arena->width;
 }
#line 126 "/home/phil/c/z/arenaList/arenaList.c"
static void * key_pointer_ArenaTreeNode                                                 //IV Get a temporary pointer to the key of a node.
 (const ArenaTreeNode node)                                                             // Node
 {const typeof(node.proto->width(node)) width = node.proto->width(node);                                                         // Width of node
  return pointer_ArenaTree_size(node.list, node.offset + sizeof(ArenaTreeContent) + width);     // The key is stored after the node and optional user data in the arena.
 }
#line 132 "/home/phil/c/z/arenaList/arenaList.c"
static void * data_pointer_ArenaTreeNode                                                //IV Get a temporary pointer to the user data of a node.
 (const ArenaTreeNode node)                                                             // Node
 {return pointer_ArenaTree_size(node.list, node.offset + sizeof(ArenaTreeContent));             // The optional user data is stored immediately after the node in the arena.
 }
#line 137 "/home/phil/c/z/arenaList/arenaList.c"
static size_t length_size_ArenaTreeNode                                                 //I Get the length of the key associated with a node
 (const ArenaTreeNode node)                                                             // Node
 {return node.proto->content(node)->length;
 }
#line 142 "/home/phil/c/z/arenaList/arenaList.c"
static  ArenaTreeNode  nodeFromOffset_ArenaTree_size                                            //IP Create a node to locate an allocation within the arena of a ArenaTree.
 (const ArenaTree      list,                                                            // ArenaTree
  const size_t delta)                                                           // Delta within arena. A delta of zero represents no such node.
 {return newArenaTreeNode(({struct ArenaTreeNode t = {list: list, offset: delta, proto: &ProtoTypes_ArenaTreeNode}; t;}));
 }
#line 159 "/home/phil/c/z/arenaList/arenaList.c"
static int equals_int_ArenaTreeNode_ArenaTreeNode                                               //I Confirm two Nodes are equal
 (const ArenaTreeNode a,                                                                // First offset
  const ArenaTreeNode b)                                                                // Second offset
 {return a.list.arena == b.list.arena && a.offset == b.offset;
 }
#line 179 "/home/phil/c/z/arenaList/arenaList.c"
static  ArenaTreeNode  allocate_offset_ArenaTree_size                                           //IP Allocate memory within the arena of a ArenaTree and clear the allocated memory
 (const ArenaTree      list,                                                            // ArenaTree in which to allocate
  const size_t size)                                                            // Amount of memory required
 {if (list.arena->used + size < list.arena->size)                               // Allocate within existing arena
   {const typeof(list.proto->nodeFromOffset(list, list.arena->used)) n = list.proto->nodeFromOffset(list, list.arena->used);                                // Current offset to open memory
    list.arena->used += size;                                                   // Allocate
    return n;                                                                   // Return allocation
   }
  else                                                                          // Reallocate arena
   {const typeof(nextPowerOfTwo(list.arena->size + size)) S = nextPowerOfTwo(list.arena->size + size);                                // Round up memory required
    if (S < ((size_t)((1l<<(sizeof(((ArenaTreeContent *)0)->length)*8)) - 1)))          // The maximum size of a delta if there are 8 bits in a byte
     {const typeof(realloc(list.arena->data, S)) m = realloc(list.arena->data, S);                                         // Reallocate arena - the old one will be copied into it if it is moved.
      if (m)                                                                    // Retry the memory allocation
       {list.arena->data = m;                                                   // New arena
        list.arena->size = S;                                                   // Arena size
        const typeof(list.arena->used) u = list.arena->used;                                                   // Length of free space
        memset(list.arena->data + u, 0, S - u);                                 // Clear free space
        return list.proto->allocate(list, size);                                           // Allocate within arena
       }
     }
   }
  printStackBackTraceAndExit(2, "Requested arena too large\n");                 // The arena has become too large for the chosen size of offsets.
 }
#line 203 "/home/phil/c/z/arenaList/arenaList.c"
static size_t used_ArenaTree                                                            //I Amount of space currently being used within the arena of a ArenaTree.
 (const ArenaTree list)                                                                 // ArenaTree
 {return list.arena->used;
 }
#line 208 "/home/phil/c/z/arenaList/arenaList.c"
static ArenaTreeNode node_ArenaTreeNode_ArenaTree_string                                                //I Create a new ArenaTree node with the specified key.
 (const ArenaTree            list,                                                      // ArenaTree in which to create the node
  const void * const key,                                                       // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the ArenaTree is determined solely by the user.
  const size_t       length)                                                    // Length of the key, or if zero, I will use strlen
 {const typeof(list.proto->allocate(list, sizeof(ArenaTreeContent) + length + list.proto->width(list))) n = list.proto->allocate(list, sizeof(ArenaTreeContent) + length + list.proto->width(list));                // Offset of space allocated for a node and key
  const typeof(n.proto->content(n)) c = n.proto->content(n);                                                              // Address content
  c->length = length;                                                           // Save key length
  memcpy(n.proto->key(n), key, length);                                                 // Copy in key
  return n;                                                                     // Return node
 }
#line 307 "/home/phil/c/z/arenaList/arenaList.c"
static int equalsString_ArenaTreeNode_string                                            //I Check that the key of a node equals a string
 (const ArenaTreeNode        node,                                                      // Node
  const char * const key)                                                       // Key
 {return node.proto->keyEquals(node, key, strlen(key));
 }
#line 666 "/home/phil/c/z/arenaList/arenaList.c"
static void dump_ArenaTreeNode                                                          //IP Dump a ArenaTreeNode on stderr
 (const ArenaTreeNode node)                                                             // ArenaTreeNode
 {makeLocalCopyOfArenaTreeKey(k, l, node);                                              // Local copy of key
  say("%lu %s\n", l, k);                                                        // Print key number
 }
#line 49 "/home/phil/c/z/arenaTree/arenaTree.c"

static void free_ArenaTree                                                              // Free a ArenaTree in its entirety
 (const ArenaTree tree)                                                                 // ArenaTree
 {free(tree.arena->data);
  free(tree.arena);
 }

static  ArenaTreeFound makeArenaTreeFound                                                       //P Create a new found description
 (const ArenaTree      tree,                                                            // The ArenaTree being searched
  char * const key,                                                             // The key to find
  const size_t length)                                                          // Length of the key to find
 {const ArenaTreeNode n = newArenaTreeNode(({struct ArenaTreeNode t = {list: tree, proto: &ProtoTypes_ArenaTreeNode}; t;}));                                        // Sample node within ArenaTree
  return newArenaTreeFound(({struct ArenaTreeFound t = {tree: tree, key: key, length: length, last: n, proto: &ProtoTypes_ArenaTreeFound}; t;}));
 }

//D1 Attributes                                                                 // Get and set the attributes of a node

static ArenaTreeNode root_ArenaTree                                                             // The root node in a ArenaTree
 (const ArenaTree tree)                                                                 // ArenaTree
 {return tree.proto->nodeFromOffset(tree, tree.arena->root);
 }

static size_t height_ArenaTreeNode                                                      // Height of a sub ArenaTree starting at the specified node
 (const ArenaTreeNode node)                                                             // Node
 {return node.proto->content(node)->height;
 }

static int keyEquals_ArenaTreeNode_string_size                                          // Key of the specified node as a zero terminated string
 (const ArenaTreeNode        node,                                                      // Node
  const char * const key,                                                       // Expected key value
  const size_t       length)                                                    // Length of expected key value
 {const typeof(node.proto->length(node)) l = node.proto->length(node);                                                            // Recorded length of node
  if (l != length) return 0;                                                    // Different lengths means the keys is unequal
  return !memcmp(node.proto->key(node), key, l);                                           // Compare memory
 }

static ArenaTreeNode up_ArenaTreeNode_ArenaTreeNode                                                     // Parent node for the specified node.
 (const ArenaTreeNode node)                                                             // Node
 {return node.list.proto->nodeFromOffset(node.list, node.proto->content(node)->up);
 }

static ArenaTreeNode left_ArenaTreeNode_ArenaTreeNode                                                   // Left child node below the specified parent node.
 (const ArenaTreeNode     parent)                                                       // Parent
 {return parent.list.proto->nodeFromOffset(parent.list, parent.proto->content(parent)->left);
 }

static ArenaTreeNode right_ArenaTreeNode_ArenaTreeNode                                                  // Right child node below the specified parent node.
 (const ArenaTreeNode     parent)                                                       // Parent
 {return  parent.list.proto->nodeFromOffset(parent.list, parent.proto->content(parent)->right);
 }

static ArenaTreeNode ownedTreeRoot_ArenaTreeNode_ArenaTreeNode                                          //P Root of ArenaTree owned by this node if there is  one, else the returned node has an offset of zero.
 (const ArenaTreeNode     parent)                                                       // Parent
 {return parent.list.proto->nodeFromOffset(parent.list, parent.proto->content(parent)->tree);
 }

static void setHeight_ArenaTreeNode                                                     //P Save the height of the sub ArenaTree starting at the specified node
 (const ArenaTreeNode  parent,                                                          // Parent
  const size_t height)                                                          // Height
 {parent.proto->content(parent)->height = height;
 }

static void setUp_ArenaTreeNode_ArenaTreeNode                                                   //P Set parent node of specified child node
 (const ArenaTreeNode child,                                                            // Parent
  const ArenaTreeNode parent)                                                           // Child
 {child.proto->content(child)->up = parent.offset;
 }

static void setLeft_ArenaTreeNode_ArenaTreeNode                                                 //P Set left child of specified parent.
 (const ArenaTreeNode parent,                                                           // Parent
  const ArenaTreeNode left)                                                             // Left child
 {parent.proto->content(parent)->left = left.offset;
 }

static void setRight_ArenaTreeNode_ArenaTreeNode                                                //P Parent parent for a specified parent in a ArenaTree
 (const ArenaTreeNode parent,                                                           // Parent
  const ArenaTreeNode right)                                                            // Right child
 {parent.proto->content(parent)->right = right.offset;
 }

static void setTree_ArenaTreeNode_ArenaTreeNode                                                 //P Set the other ArenaTree located by this node.
 (const ArenaTreeNode parent,                                                           // Parent
  const ArenaTreeNode tree)                                                             // ArenaTree to be added to parent
 {parent.proto->content(parent)->tree = tree.offset;
 }

static int valid_ArenaTreeNode                                                          // Check that the specified node is valid.
 (const ArenaTreeNode node)                                                             // Node
 {return node.offset;                                                           // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

static int isRoot_ArenaTreeNode                                                         // Check that the specified node is valid.
 (const ArenaTreeNode node)                                                             // Node
 {return !(node.proto->up(node)).offset;                                                   // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

//D1 Find                                                                       // Find a key if it exists within the base ArenaTree or an owned ArenaTree.

static int cmpArenaTree                                                                 // Compare key sought with current key
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

static ArenaTreeFound find_ArenaTreeFound_ArenaTreeFound_ArenaTreeNode_string                                   //P Find a key if it exists within the ArenaTree starting at this node.
 (ArenaTreeFound      found,                                                            // Found definition
  const ArenaTreeNode root)                                                             // The root node at which the ArenaTree starts
 {for(ArenaTreeNode p = root; p.proto->valid(p);)                                               // Search down through ArenaTree
   {found.last.offset  = p.offset;                                              // Record last parent compared with key sought
    makeLocalCopyOfArenaTreeKey(k, l, p);                                               // Local copy of key

    const typeof(found.length) L = found.length; const typeof(found.key) K = found.key;
    const typeof(found.different = cmpArenaTree(K, L, k, l)) i = found.different = cmpArenaTree(K, L, k, l);                                     // Compare key sought with current key

    if (!i) return found;                                                       // Found

    p.offset = i < 0 ? p.proto->content(p)->left : p.proto->content(p)->right;                  // Continue left or right
   }

  return found;                                                                 // Found
 }

static ArenaTreeNode locate_ArenaTreeNode_string                                                // Locate the node with the specified key if it exists within the ArenaTree owned by the specified node.
 (const ArenaTreeNode        node,                                                      // Node
  const char * const key,                                                       // Key to find
  const size_t       length)                                                    // Length of the key to find
 {ArenaTreeNode p = node.proto->ownedTreeRoot(node);                                               // Root node
  if (!p.proto->valid(p)) return p;                                                     // Empty ArenaTree
  for(;;)                                                                       // Search down through ArenaTree
   {makeLocalCopyOfArenaTreeKey(k, l, p);
    const typeof(cmpArenaTree(key, length, k, l)) i = cmpArenaTree(key, length, k, l);                                                // Compare key sought with current key

    if (!i) return p;                                                           // Found

    p.offset = i < 0 ? p.proto->content(p)->left : p.proto->content(p)->right;                  // Continue left or right
    if (!p.proto->valid(p)) return p;                                                   // Not found
   }

  printStackBackTrace("Should not be able to reach here\n");
 }

static ArenaTreeNode locate_ArenaTree_string                                                    // Locate the node with the specified key if it exists within the specified ArenaTree.
 (const ArenaTree            tree,                                                      // ArenaTree
  const char * const key,                                                       // Key to find
  const size_t       length)                                                    // Length of the key to find
 {ArenaTreeNode p = tree.proto->root(tree);                                                        // Root node
  if (!p.proto->valid(p)) return p;                                                     // Empty ArenaTree
  for(;;)                                                                       // Search down through ArenaTree
   {makeLocalCopyOfArenaTreeKey (k, l, p);
    const typeof(cmpArenaTree(key, length, k, l)) i = cmpArenaTree(key, length, k, l);                                                // Compare key sought with current key
    if (!i) return p;                                                           // Found
    p.offset = i < 0 ? p.proto->content(p)->left : p.proto->content(p)->right;                  // Continue left or right
    if (!p.proto->valid(p)) return p;                                                   // Not found
   }

  printStackBackTrace("Should not be able to reach here\n");
 }

static int check_ArenaTree                                                              // Check the integrity of the ArenaTree
 (const ArenaTree tree)                                                                 // ArenaTree to check
 {void check(ArenaTreeNode p)                                                           // Check a node
   {if (!p.proto->valid(p)) return;
                   makeLocalCopyOfArenaTreeKey(pk, pl, p);
    const typeof(p.proto->left(p)) l = p.proto->left(p);  makeLocalCopyOfArenaTreeKey(lk, ll, l);
    const typeof(p.proto->right(p)) r = p.proto->right(p); makeLocalCopyOfArenaTreeKey(rk, rl, r);
    const typeof(l.proto->up(l)) L = l.proto->up(l);    makeLocalCopyOfArenaTreeKey(Lk, Ll, L);
    const typeof(r.proto->up(r)) R = r.proto->up(r);    makeLocalCopyOfArenaTreeKey(Rk, Rl, R);
    if (l.proto->valid(l) && !p.proto->equals(p, l.proto->up(l))) printStackBackTraceAndExit
        (1, "Bad left  up %s to %s should be %s\n", lk, Lk, pk);
    if (r.proto->valid(r) && !p.proto->equals(p, r.proto->up(r))) printStackBackTraceAndExit
        (2, "Bad right up %s to %s should be %s\n", rk, Rk, pk);
    if (l.proto->valid(l) && r.proto->valid(r))
     {const typeof(l.proto->height(l)) lh = l.proto->height(l); const typeof(r.proto->height(r)) rh = r.proto->height(r);
      if (lh > 2 * rh + 1 || rh > 2 * lh + 1 ) printStackBackTraceAndExit
        (3, "Unbalanced ArenaTree at %s with left %lu versus right %lu\n", pk, lh, rh);
     }
    check(l);                                                                   // Check left
    check(r);                                                                   // Check right
   }
  check(tree.proto->root(tree));                                                           // Check from root
  return 1;                                                                     // If we get this far the tree is ok
 }

static  ArenaTreeFound find_ArenaTreeFound_ArenaTreeNode_string                                         //P Find a key if it exists within the ArenaTree owned by the specified node.
 (const ArenaTreeNode  node,                                                            // The node owning the ArenaTree to search
  char * const key,                                                             // The key to find
  const size_t length)                                                          // Length of the key to find
 {const typeof(makeArenaTreeFound(node.list, key, length)) f = makeArenaTreeFound(node.list, key, length);                                       // Status of find
  const typeof(node.proto->ownedTreeRoot(node)) r = node.proto->ownedTreeRoot(node);                                                     // Root node of owned ArenaTree
  if (!r.proto->valid(r)) return f;                                                     // Empty owned ArenaTree
  return f.proto->find(f, r);                                                           // Search the non empty owned ArenaTree starting at the specified node.
 }

static  ArenaTreeFound find_ArenaTreeFound_ArenaTree_string                                             //P Find a key if it exists within the base ArenaTree.
 (const ArenaTree      tree,                                                            // The base ArenaTree to search
  char * const key,                                                             // The key to find
  const size_t length)                                                          // Length of the key to find
 {const typeof(makeArenaTreeFound(tree, key, length)) f = makeArenaTreeFound(tree, key, length);                                            // Status of find
  if (!tree.arena->root) return f;                                              // Empty ArenaTree
  return f.proto->find(f, tree.proto->nodeFromOffset(tree, tree.arena->root));                     // Search the non empty base ArenaTree starting at the specified node.
 }

static ArenaTreeNode ll_ArenaTreeNode_ArenaTree_strings                                                 // Search through a series of owned trees starting at the base ArenaTree as directed by the specified keys
 (const ArenaTree   tree,                                                               // The base ArenaTree to search from
  const char * const keys,                                                      // Zero terminated list of keys
  ...)                                                                          // Following keys
 {va_list va;
  va_start(va, keys);
  ArenaTreeNode p = tree.proto->locate(tree, keys, strlen(keys));
  for(;p.proto->valid(p);)
   {const typeof(va_arg(va, char *)) k = va_arg(va, char *);
    if (!k) break;
    p.offset = p.proto->locate(p, k, strlen(k)).offset;
   }
  va_end(va);
  return p;
 }

//D1 Add                                                                        // Add a new key if not already present in a base ArenaTree or a ArenaTree owned by a node.

static ArenaTreeNode add_ArenaTreeNode_ArenaTreeFound_ArenaTreeNode_string                                      // Add a new key if not already present in the ArenaTree root at the specified node.
 (ArenaTreeFound f,                                                                     // Found status for the key being added.
  ArenaTreeNode owner)                                                                  // Invalid - the base ArenaTree. Valid - the node that owns the ArenaTree being added to.
 {const ArenaTreeNode invalid = newArenaTreeNode(({struct ArenaTreeNode t = {proto: &ProtoTypes_ArenaTreeNode};   t;}));                                              // Return an invalid  node if the node already exists in the ArenaTree
  if (!f.different) return invalid;                                             // Key already present

   const typeof(f.key) key = f.key;                                                                 // The key to add
  const typeof(f.last.list) tree = f.last.list;                                                           // The base ArenaTree
     const typeof(tree.proto->node(tree, key, strlen(key))) n = tree.proto->node(tree, key, strlen(key)); const typeof(f.last) p = f.last;                             // Create new node under parent

  if (f.different < 0) p.proto->setLeft(p, n); else p.proto->setRight(p, n);                    // Insert node

  n.proto->setUp(n, p);                                                                 // Set parent of inserted node

  for(ArenaTreeNode p = newArenaTreeNode(({struct ArenaTreeNode t = {list: n.list, offset: n.offset, proto: &ProtoTypes_ArenaTreeNode}; t;}));                      // Balance nodes along path back to root
            p.proto->valid(p);
            p.offset = p.proto->up(p).offset)
   {size_t height(ArenaTreeNode p)                                                      // Height of a node
     {return p.proto->valid(p) ? p.proto->height(p) : 0;
     }

    void setHeight(ArenaTreeNode p)                                                     // Set the height of a node
     {const typeof(height(p.proto->left(p))) L = height(p.proto->left(p));
      const typeof(height(p.proto->right(p))) R = height(p.proto->right(p));
      p.proto->setHeight(p, 1 + (L > R ? L : R));
     }

    ArenaTreeNode balance                                                               // Balance node
     (const int d)                                                              // Rotate left if true
     {const typeof(p.proto->up(p)) q = p.proto->up(p); const typeof(p.proto->left(p)) l = p.proto->left(p); const typeof(p.proto->right(p)) r = p.proto->right(p); const typeof(d ? r : l) rl = d ? r : l;

      if (!q.proto->valid(q))                                                           // Balance the root node
       {if (!owner.proto->valid(owner)) tree.arena->root = rl.offset;                       // New root node in base ArenaTree
        else owner.proto->setTree(owner, rl);                                               // New root node in owned ArenaTree
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

static  ArenaTreeNode   add_ArenaTreeNode_ArenaTree_string                                              // Add a new key if not already present in the specified base ArenaTree.
 (const ArenaTree       tree,                                                           // The ArenaTree to search
  char  * const key,                                                            // The key to add
  const size_t  length)                                                         // Length of the key to find
 {const typeof(tree.proto->find(tree, key, length)) f = tree.proto->find(tree, key, length);                                                 // Try to find the key

  if (!f.last.proto->valid(f.last))                                                          // Empty ArenaTree
   {const typeof(tree.proto->node(tree, key, length)) root = tree.proto->node(tree, key, length);                                            // Add new key in a node
    tree.arena->root = root.offset;                                             // Make the new node the root node
    return root;                                                                // Create node
   }
  const ArenaTreeNode invalid = newArenaTreeNode(({struct ArenaTreeNode t = {proto: &ProtoTypes_ArenaTreeNode};   t;}));
  return f.proto->add(f, invalid);                                                      // Non empty ArenaTree - add the node below the root node
 }

static  ArenaTreeNode   add_ArenaTreeNode_ArenaTreeNode_string                                          // Add a new key if not already present in the ArenaTree owned by the specified node.
 (const ArenaTreeNode   node,                                                           // The ArenaTree to search
  char  * const key,                                                            // The key to add
  const size_t  length)                                                         // Length of the key to find
 {const ArenaTree tree  = node.list;                                                    // ArenaTree to search
       const typeof(node.proto->find(node, key, length)) f = node.proto->find(node, key, length);                                            // Try to find the key
  if (!f.last.proto->valid(f.last))                                                          // Empty ArenaTree
   {const typeof(tree.proto->node(tree, key, length)) root = tree.proto->node(tree, key, length);                                            // Add new key in a node
    node.proto->setTree(node, root);                                                       // Make the new node the root node
    return root;                                                                // Create node
   }
  return f.proto->add(f, node);                                                         // Non empty ArenaTree - add the node below the root node
 }

//D1 Traverse                                                                   // Traverse the ArenaTree

static  size_t count_size_ArenaTree                                                     // Count the number of nodes in a ArenaTree
 (const ArenaTree tree)                                                                 // ArenaTree
 {size_t l = 0;
  void count(const ArenaTreeNode parent)                                                // Count the nodes in a sub ArenaTree
   {if (!parent.proto->valid(parent)) return;
    l++; count(parent.proto->left(parent)); count(parent.proto->right(parent));
   }
  count(tree.proto->root(tree));                                                           // Start at the root
  return l;                                                                     // Return count
 }

//D1 Print                                                                      // Print a ArenaTree.

static StringBuffer print_ArenaTreeNode                                                 // Print the tree below the specified node as a string.
 (const ArenaTreeNode node)                                                             // Node
 {typeof(0ul) height = 0ul;                                                                 // Height of root
  const typeof(makeStringBuffer()) p = makeStringBuffer();                                                       // Print to this string buffer

  void print(const ArenaTreeNode node, size_t depth)                                    // Print to allocated string
   {if (!node.proto->valid(node)) return;                                                 // No such node
    print(node.proto->left(node), depth+1);                                                // Print left
    p.proto->addFormat(p, "%lu ", depth);                                               // Depth
    p.proto->addSpaces(p, 2 * depth);                                                   // Spacer
    makeLocalCopyOfArenaTreeKey(k, l, node);                                            // Local copy of key
    p.proto->add(p, k);                                                                 // Print key
    for(size_t i = 0; i < height - depth; ++i) p.proto->add(p, "..");                   // Separator
    p.proto->addNewLine(p);
    print(node.proto->right(node), depth+1);                                               // Print right
   }

  height = node.proto->height(node);                                                       // Height of node
  print(node, 0);                                                               // Print to buffer
  p.proto->join(p);
  return p;
 }

static StringBuffer print_ArenaTree                                                     // Print the specified ArenaTree as a string.
 (const ArenaTree tree)                                                                 // ArenaTree
 {const typeof(tree.proto->root(tree)) root = tree.proto->root(tree);                                                         // Start at root
  return root.proto->print(root);                                                          // Print to buffer
 }

static void dump_ArenaTree                                                              //P Print a ArenaTree on stderr
 (const ArenaTree tree)                                                                 // ArenaTree
 {typeof(0ul) height = 0ul;                                                                 // Height of root

  void print(const ArenaTreeNode node, size_t depth)                                    // Print to allocated string
   {if (!node.proto->valid(node)) return;                                                 // No such node
    print(node.proto->left(node), depth+1);                                                // Print left
    say("%lu ", depth);                                                         // Depth
    for(size_t i = 0; i < depth; ++i) say("  ");                                // Spacer
    makeLocalCopyOfArenaTreeKey(k, l, node);                                            // Local copy of key
    say("%s", k);
    for(size_t i = 0; i < height - depth; ++i) say("..");                       // Separator
    say("\n");
    print(node.proto->right(node), depth+1);                                               // Print right
   }

  const typeof(tree.proto->root(tree)) root = tree.proto->root(tree);                                                         // Start at root
  height = root.proto->height(root);
  print(root, 0);                                                               // Print to buffer
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TnewArenaTree //Tadd //Tleft //Tright //Tkey //Tup //Tfree
 {   const typeof(makeArenaTree()) t = makeArenaTree();
     t.proto->add(t, "b",  1);
     t.proto->add(t, "a",  1);
     t.proto->add(t, "c",  1);
     t.proto->add(t, "bb", 2);
     t.proto->add(t, "cc", 2);

     const typeof(t.proto->root(t)) b = t.proto->root(t); const typeof(b.proto->left(b)) a = b.proto->left(b); const typeof(b.proto->right(b)) c = b.proto->right(b); const typeof(c.proto->left(c)) bb = c.proto->left(c); const typeof(c.proto->right(c)) cc = c.proto->right(c);

  assert(  b.proto->keyEquals(b, "b",  1));
  assert(  a.proto->keyEquals(a, "a",  1));
  assert(  c.proto->keyEquals(c, "c",  1));
  assert(  bb.proto->keyEquals(bb, "bb", 2));
  assert(  cc.proto->keyEquals(cc, "cc", 2));
  assert(  b.proto->isRoot(b));
  assert(  b.proto->equals(b, a.proto->up(a)));
  assert(  b.proto->equals(b, c.proto->up(c)));
  assert(  c.proto->equals(c, bb.proto->up(bb)));
  assert(  c.proto->equals(c, cc.proto->up(cc)));

  assert( !t.proto->find(t, "cc", 2).different);
  assert(  t.proto->find(t, "dd", 2).different);

     t.proto->free(t);
 }

void test1()                                                                    //Tll
 {  const typeof(makeArenaTree()) t = makeArenaTree();
    t.proto->add(t, "a", 1);  t.proto->add(t, "b", 1);
    const typeof(t.proto->root(t)) a = t.proto->root(t); const typeof(a.proto->right(a)) b = a.proto->right(a);

    b.proto->add(b, "4", 1); b.proto->add(b, "2", 1); b.proto->add(b, "6", 1);
    b.proto->add(b, "1", 1); b.proto->add(b, "3", 1); b.proto->add(b, "5", 1); b.proto->add(b, "7", 1);

    const typeof(b.proto->ownedTreeRoot(b)) b4 = b.proto->ownedTreeRoot(b);
    const typeof(b4.proto->left(b4)) b2 = b4.proto->left(b4);  const typeof(b4.proto->right(b4)) b6 = b4.proto->right(b4);
    const typeof(b2.proto->left(b2)) b1 = b2.proto->left(b2);  const typeof(b2.proto->right(b2)) b3 = b2.proto->right(b2);
    const typeof(b6.proto->left(b6)) b5 = b6.proto->left(b6);  const typeof(b6.proto->right(b6)) b7 = b6.proto->right(b6);

  assert( b4.proto->equalsString(b4, "4"));
  assert( b2.proto->equalsString(b2, "2"));
  assert( b6.proto->equalsString(b6, "6"));
  assert( b1.proto->equalsString(b1, "1"));
  assert( b3.proto->equalsString(b3, "3"));
  assert( b5.proto->equalsString(b5, "5"));
  assert( b7.proto->equalsString(b7, "7"));

  assert( b4.proto->up(b4).offset == 0);
  assert( b4.proto->equals(b4, b2.proto->up(b2)));
  assert( b4.proto->equals(b4, b6.proto->up(b6)));
  assert( b2.proto->equals(b2, b1.proto->up(b1)));
  assert( b2.proto->equals(b2, b3.proto->up(b3)));
  assert( b6.proto->equals(b6, b5.proto->up(b5)));
  assert( b6.proto->equals(b6, b7.proto->up(b7)));

  assert( !b.proto->find(b, "7", 1).different);
  assert(  b.proto->find(b, "8", 1).different);

    const typeof(t.proto->ll(t, "b", "7", 0)) f = t.proto->ll(t, "b", "7", 0);  assert( f.offset == b7.offset);

    t.proto->free(t);
 }

void test2()                                                                    //Tcount
 {const typeof(makeArenaTree()) t = makeArenaTree(); const typeof(100ul) N = 100ul; char c[256];

  for(typeof(0ul) i = 0ul; i < N; ++i) t.proto->add(t, c, sprintf(c, "%lu", i));

  assert( t.proto->count(t) == N);

    const typeof(t.proto->root(t)) r = t.proto->root(t);
    const typeof(t.proto->locate(t, "28", 2)) n28 = t.proto->locate(t, "28", 2);
    const typeof(n28.proto->up(n28)) n27 = n28.proto->up(n28);
    const typeof(n27.proto->up(n27)) n25 = n27.proto->up(n27);
    const typeof(n25.proto->up(n25)) n23 = n25.proto->up(n25);

  assert( r.proto->equalsString(r, "3"));
  assert( r.proto->height(r) == 11);
  assert( n23.proto->equalsString(n23, "23"));

  assert(   t.proto->check(t));
      t.proto->free(t);
 }

void test3()
 {const typeof(makeArenaTree()) t = makeArenaTree(); const typeof(256*256ul) N = 256*256ul; char c[256];

  for(typeof(0ul) i = 0ul; i < N; ++i) t.proto->add(t, c, sprintf(c, "%lx", i));

  assert( t.proto->count(t) == N);
  assert( t.proto->check(t));

    const typeof(t.proto->root(t)) r = t.proto->root(t);
  assert( r.proto->equalsString(r, "3"));
  assert( r.proto->height(r) == 33);

    t.proto->free(t);
 }

void test4()                                                                    //Theight //Tlocate //Troot
 {const typeof(makeArenaTree()) t = makeArenaTree(); char c[16]; const typeof(10) N = 10;
  const typeof(t.proto->locate(t, "0", 1)) n = t.proto->locate(t, "0", 1); assert( !n.proto->valid(n));

  for  (typeof(0) i = 0; i < N; ++i)
   {for(typeof(0) j = 0; j < N; ++j)
     {t.proto->add(t, c, sprintf(c, "%d%d", i, j));
     }
   }

    const typeof(t.proto->root(t)) r = t.proto->root(t);
  assert( r.proto->equalsString(r, "31"));
  assert( r.proto->height(r) == 11);

    const typeof(t.proto->locate(t, "85", 2)) f = t.proto->locate(t, "85", 2);
  assert( f.proto->valid(f));
  assert( f.proto->height(f) == 2);

    t.proto->free(t);
 }

void test5()
 {const typeof(makeArenaTree()) t = makeArenaTree(); char c[16]; const typeof(20) N = 20;

  for(typeof(0) i = 0; i < N; ++i)
   {   const typeof(t.proto->add(t, c, sprintf(c, "%d", i))) n = t.proto->add(t, c, sprintf(c, "%d", i));
       const typeof(n.proto->locate(n, "0", 1)) e = n.proto->locate(n, "0", 1);
    assert( !e.proto->valid(e));                                                               // Not present in sub tree

    for(typeof(0) j = 0; j < N; ++j) n.proto->add(n, c, sprintf(c, "%d-%d", i, j));
   }

  const char *a[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 0};
  for(const char * const *p = a; *p; ++p)
   {  const typeof(t.proto->locate(t, *p, 1)) n = t.proto->locate(t, *p, 1);
    assert( n.proto->equalsString(n, *p));
   }

  t.proto->free(t);
 }

void test6()                                                                    //Tcheck //Tsprint //TisRoot //TkeyEquals
 {const typeof(makeArenaTree()) t = makeArenaTree(); char c[128]; const typeof(10) N = 10;

  for(typeof(0) i = 0; i < N; ++i)
   {t.proto->add(t, c, sprintf(c, "%d", i));
    t.proto->add(t, c, sprintf(c, "%d", 2*N - i));
   }
  assert( t.proto->check(t));

    const typeof(t.proto->root(t)) r = t.proto->root(t);

  assert( r.proto->isRoot(r));
  assert( r.proto->keyEquals(r, "19", 2));
  assert( r.proto->height(r) == 6);

    t.proto->free(t);
 }

void test7()                                                                    //Tvalid //TcmpArenaTree
 {ArenaTreeNode n = newArenaTreeNode(({struct ArenaTreeNode t = {proto: &ProtoTypes_ArenaTreeNode};   t;})); assert( !n.proto->valid(n));

  assert( cmpArenaTree("aa", 2, "aa", 1) ==  1);
  assert( cmpArenaTree("aa", 2, "aa", 2) ==  0);
  assert( cmpArenaTree("aa", 1, "aa", 2) == -1);
 }

void test8()                                                                    //Tprint
 {const typeof(makeArenaTree()) t = makeArenaTree();  char s[256];  typeof(10) N = 10;

  for  (typeof(0) i = 0; i < N; ++i)
   {for(typeof(N) j = N; j > 0; --j)
     {t.proto->add(t, s, sprintf(s, "%d%d", i, j));
     }
   }

  const typeof(t.proto->locate(t, "86", 2)) a = t.proto->locate(t, "86", 2);
               assert( a.proto->valid(a));  assert( a.proto->height(a) ==  6);  assert( a.proto->equalsString(a, "86"));
  const typeof(a.proto->up(a)) b = a.proto->up(a);  assert( b.proto->valid(b));  assert( b.proto->height(b) ==  7);  assert( b.proto->equalsString(b, "810"));
  const typeof(b.proto->up(b)) c = b.proto->up(b);  assert( c.proto->valid(c));  assert( c.proto->height(c) ==  8);  assert( c.proto->equalsString(c, "76"));
  const typeof(c.proto->up(c)) d = c.proto->up(c);  assert( d.proto->valid(d));  assert( d.proto->height(d) ==  9);  assert( d.proto->equalsString(d, "66"));
  const typeof(d.proto->up(d)) e = d.proto->up(d);  assert( e.proto->valid(e));  assert( e.proto->height(e) == 10);  assert( e.proto->equalsString(e, "56"));
  const typeof(e.proto->up(e)) f = e.proto->up(e);  assert( f.proto->valid(f));  assert( f.proto->height(f) == 11);  assert( f.proto->equalsString(f, "36"));
  const typeof(f.proto->up(f)) g = f.proto->up(f);  assert( g.proto->valid(g));  assert( g.proto->height(g) == 12);  assert( g.proto->equalsString(g, "16"));

  assert( g.proto->equals(g, t.proto->root(t)));    assert( g.proto->isRoot(g));

    const typeof(t.proto->locate(t, "98", 2)) h = t.proto->locate(t, "98", 2);
    const typeof(h.proto->print(h)) H = h.proto->print(h);
  assert( H.proto->printsAs(H, 
"1   97..\n"
"0 98....\n"
"1   99..\n"
));
  assert( t.proto->check(t));
    t.proto->free(t); H.proto->free(H);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4,
                           test5, test6, test7, test8, 0};
  run_tests("ArenaTree", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaTree/arenaTree
