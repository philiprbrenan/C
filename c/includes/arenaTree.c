#line 1 "/home/phil/c/z/arenaTree/arenaTree.c"
//------------------------------------------------------------------------------
// Trees held in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <setjmp.h>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Tree.
#ifndef ArenaTree_included
#define ArenaTree_included




typedef char * ArenaTreeString;                                                         // Arena Tree string
typedef struct StringBuffer  StringBuffer;

typedef struct ArenaTree                                                                // Arena Tree.
 {const struct ProtoTypes_ArenaTree *proto;                                             // Methods associated with an arena tree
  struct ArenaTreeArena *arena;                                                         // The arena for the arena tree
 } ArenaTree;

typedef struct ArenaTreeDelta                                                           // The amount an item is offset within a tree.
 {unsigned int offset;
 } ArenaTreeDelta;

typedef struct ArenaTreeContent                                                         // A tree node in the arena
 {ArenaTreeDelta parent;                                                                // Offset to the parent of this node or zero if this is the root of the tree
  ArenaTreeDelta first, last, next, prev;                                               // Offsets to the first, last children of this node, amd the next and previous siblings of this node.
  ArenaTreeDelta key;                                                                   // The name of this node represented by a zero terminated string at the specified offset in the arena. Nodes are not ordered by their keys, their order is determined solely by the user. Keys can be reused between nodes.
  ArenaTreeDelta data;                                                                  // The data carried by this node.
 } ArenaTreeContent;

typedef struct ArenaTreeOffset                                                          // Offset to any item in the tree.  The caller is responsible for interpreting the content of the memory so addressed.
 {const ArenaTree      tree;                                                            // ArenaTree containing the item
  const size_t offset;                                                          // Offset
  const struct ProtoTypes_ArenaTreeOffset *proto;                                       // Methods associated with offsets
 } ArenaTreeOffset;

typedef struct ArenaTreeNode                                                            // Offset to a node in the tree. As we know what is being addressed we have a more specific set of methods available than those available to a generic Offset.
 {ArenaTree      tree;                                                                  // ArenaTree containing the item
  size_t offset;                                                                // Offset
  const struct ProtoTypes_ArenaTreeNode *proto;                                         // Methods associated with nodes
 } ArenaTreeNode;

typedef struct ArenaTreeArena                                                           // Description of the block of memory that holds a tree.  The individual nodes of the tree cannot be cannot be freed individually, but of course the entire arena can be. As the arena uses offsets to store addresses, the arena can be resized by copying it into a new, larger arena.
 {size_t  size;                                                                 // The total size of the arena used to hold data associated with the tree.
  size_t  used;                                                                 // The number of bytes currently used.
  size_t  root;                                                                 // Offset to the root node
  ArenaTreeString data;                                                                 // The arena containing the data associated with the tree
 } ArenaTreeArena;

typedef struct ArenaTreeDescription                                                     // The description of an arena tree which is written as the header record for dump files.
 {const size_t version;                                                         // Version of arena tree
  const size_t littleEndian;                                                    // Little endian if true
  const size_t s64b;                                                            // 64 bit address if true, else 32 bit
  const size_t used;                                                            // The number of bytes currently used by the arena.
  const size_t root;                                                            // Offset to the root node
  const time_t created;                                                         // Time in seconds since the epoch when this tree was created
 } ArenaTreeDescription;

#include <arenaTree_prototypes.h>                                                      // Arena tree prototypes now that the relevant structures have been declared
#define ArenaTreefe( child, parent) for(ArenaTreeNode child = parent.proto->first(parent); child.proto->valid(child); child = child.proto->next(child)) // Each child in a parent from first to last
#define ArenaTreefer(child, parent) for(ArenaTreeNode child = parent.proto->last(parent) ; child.proto->valid(child); child = child.proto->prev(child)) // Each child in a parent from last to first
#include <stringBuffer.c>

//D1 Constructors                                                               // Construct a new Arena tree.

static ArenaTree makeArenaTree                                                                  // Create a new arena tree
 ()                                                                             // ArenaTree allocator
 {ArenaTreeArena * const a = alloc(sizeof(ArenaTreeArena));                                     // Allocate arena description
  const typeof(newArenaTree(({struct ArenaTree t = {arena: a, proto: &ProtoTypes_ArenaTree}; t;}))) t = newArenaTree(({struct ArenaTree t = {arena: a, proto: &ProtoTypes_ArenaTree}; t;}));                                                          // Arena tree we are creating

  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  a->used   = 0;                                                                // Length used so far
  a->root   = 0;                                                                // Root not set in ArenaTree
  t.proto->node(t, "");                                                                 // Initialize root node
  return t;
 }

static void swap_ArenaTree_ArenaTree                                                            // Swap the arenas of two trees.
 (ArenaTree source,                                                                     // Source tree
  ArenaTree target)                                                                     // Target tree
 {const typeof(source.arena) s = source.arena; const typeof(target.arena) t = target.arena;                                           // Arenas
  ArenaTreeArena u = *t; *t = *s; *s = u;
 }

static char * check_ArenaTree                                                           //P Return a string describing the structure
 (const ArenaTree tree)                                                                 // Tree
 {if (tree.arena->data) {} return "tree";
 }

//D1 Pointers and Offsets                                                       // Operations on pointers and offsets

static void * pointer_ArenaTree_size                                                    //PV Return a temporary pointer to an offset in a tree.
 (const ArenaTree      tree,                                                            // Tree
  const size_t delta)                                                           // Delta
 {if (delta > tree.arena->used)                                                 // An delta outside the arena is an unset delta
   {printStackBackTrace("Accessing area outside arena");
   }
  return (void *)(tree.arena->data + delta);                                    // Convert a non zero delta that is within the arena to a valid pointer
 }

static void * pointer_ArenaTreeOffset                                                   //PV Convert a node describing an offset into an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const ArenaTreeOffset o)                                                              // Offset
 {return pointer_ArenaTree_size(o.tree, o.offset);                                      // Return a temporary pointer to an offset in a tree.
 }

static ArenaTreeOffset offsetTo_ArenaTree_pointer                                               //P Create an offset from a pointer into the arena of a tree.
 (const ArenaTree            tree,                                                      // Offset
  const void * const pointer)                                                   // Pointer into arena
 {const typeof(*tree.arena) a = *tree.arena;
  const typeof((char const *)pointer) p = (char const *)pointer;
  if (p > a.data && p < a.data + a.used)
   {return newArenaTreeOffset(({struct ArenaTreeOffset t = {tree: tree, offset: p - a.data, proto: &ProtoTypes_ArenaTreeOffset}; t;}));
   }
  printStackBackTraceAndExit(1, "Arena does not contain pointer %p\n", pointer);
 }

static ArenaTreeContent * content_ArenaTreeNode                                                 //PV Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const ArenaTreeNode n)                                                                // NodeContent ArenaTreeOffset
 {return (ArenaTreeContent *)pointer_ArenaTree_size(n.tree, n.offset);
 }

static int checkKey_int_ArenaTreeNode_string_size                                       // Confirm that the key of a node is equal to the specified string
 (const ArenaTreeNode        node,                                                      // Node whose key is to be checked
  const char * const key,                                                       // Expected key value
  const size_t       length)                                                    // Length of expected key value or zero to request strlen
 {return !strncmp(node.proto->key(node), key, length ? : strlen(key));
 }

static int equals_int_ArenaTreeNode_ArenaTreeNode                                               // Confirm two offsets are equal
 (const ArenaTreeNode a,                                                                // First offset
  const ArenaTreeNode b)                                                                // Second offset
 {return a.tree.arena == b.tree.arena && a.offset == b.offset;
 }

static  ArenaTreeNode root_ArenaTreeNodeOffset_ArenaTree                                                // Return the root node of a tree.
 (const ArenaTree tree)                                                                 // Tree
 {return newArenaTreeNode(({struct ArenaTreeNode t = {tree: tree, proto: &ProtoTypes_ArenaTreeNode}; t;}));                                                 // Tree containing root node
 }

static  ArenaTreeNode root_ArenaTreeNodeOffset_ArenaTreeNodeOffset                                      // Return the root node of the tree containing the specified node.
 (const ArenaTreeNode node)                                                             // NodeContent
 {return node.tree.proto->root(node.tree);
 }

static ArenaTreeString key_string_ArenaTreeNode                                                 //V Get a temporary pointer to the offset containing the key of a node.
 (const ArenaTreeNode node)                                                             // NodeContent
 {const ArenaTree t = node.tree;
  const typeof(t.proto->offset(t, (node.proto->content(node))->key.offset)) k = t.proto->offset(t, (node.proto->content(node))->key.offset);
  return  k.proto->pointer(k);
 }

//D1 Allocation                                                                 // Allocating memory in the tree

static void setKey_ArenaTreeNode_ArenaTreeString                                                // Copy a string into the key field of a node
 (const ArenaTreeNode        node,                                                      // Node
  const char * const key)                                                       // Key - it will be copied into the tree
 {const typeof(node.proto->key(node)) k = node.proto->key(node);                                                               // Existing key                                                                               //
  const size_t  l = strlen(k), m = strlen(key);
  if (k && m <= l)                                                              // There is enough room in the existing key for the new key
   {strcpy(k, key);
    return;
   }
  node.proto->content(node)->key.offset = node.tree.proto->saveString(node.tree, key, m).offset;           // Allocate new (longer) key
 };

static size_t used_ArenaTree                                                            // Amount of space currently being used within the arena of a tree.
 (const ArenaTree tree)                                                                 // Tree
 {return tree.arena->used;
 }

static ArenaTreeOffset  offset_ArenaTree_size                                                   //P Create an offset to locate an item within the tree.
 (const ArenaTree       tree,                                                           // Tree
  const size_t  delta)                                                          // Delta within arena
 {const ArenaTreeOffset o = {tree, delta, &ProtoTypes_ArenaTreeOffset};                         // Create offset locally
  return        o;
 }

static ArenaTreeNode   nodeFromOffset_ArenaTree_size                                            //P Create a node to locate an allocation within the arena of a tree.
 (const ArenaTree      tree,                                                            // Tree
  const size_t delta)                                                           // Delta within arena. A delta of zero represents no such node.
 {return newArenaTreeNode(({struct ArenaTreeNode t = {tree: tree, offset: delta, proto: &ProtoTypes_ArenaTreeNode}; t;}));
 }

static ArenaTreeOffset allocate_offset_ArenaTree_size                                           //P Allocate memory within the arena of a tree and clear the allocated memory
 (const ArenaTree      tree,                                                            // ArenaTree in which to allocate
  const size_t size)                                                            // Amount of memory required
 {if (tree.arena->used + size < tree.arena->size)                               // Allocate within existing arena
   {const typeof(tree.proto->offset(tree, tree.arena->used)) o = tree.proto->offset(tree, tree.arena->used);                                        // Current offset to open memory
    memset(o.proto->pointer(o), 0, size);                                               // Clear new memory
    tree.arena->used += size;                                                   // Allocate
    return o;                                                                   // Return allocation
   }
  else                                                                          // Reallocate arena
   {const typeof(nextPowerOfTwo(tree.arena->size + size)) S = nextPowerOfTwo(tree.arena->size + size);                                // Round up memory required
    if (S < ((size_t)((1l<<(sizeof(ArenaTreeDelta)*8)) - 1)))                           // The maximum size of a delta if there are 8 bits in a byte
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
  printStackBackTraceAndExit(2, "Arena too large\n");                           // The arena has become too large for the chosen size of offsets.
 }

static ArenaTreeNode noden_ArenaTreeNode_ArenaTree_ArenaTreeString                                              // Create a new tree node keyed by a string of the specified length to which a terminating zero will be appended.
 (const ArenaTree       tree,                                                           // Arena tree in which to create the node
  const char * const key,                                                       // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the tree is determined solely by the user.
  const size_t  length)                                                         // Length of the key, or if zero, I will use strlen
 {const typeof(tree.proto->allocate(tree, sizeof(ArenaTreeContent))) c = tree.proto->allocate(tree, sizeof(ArenaTreeContent));                                        // Space required for a tree node
 ((ArenaTreeContent *)(c.proto->pointer(c)))->key.offset = tree.proto->saveString(tree, key,length).offset;// Save key                                                      // Save key offset
  return tree.proto->nodeFromOffset(tree, c.offset);                                       // Return node
 }

static ArenaTreeNode node_ArenaTreeNode_ArenaTree_ArenaTreeString                                               // Create a new tree node keyed by a zero terminated string.
 (const ArenaTree             tree,                                                     // Arena tree in which to create the node
  const char * const  key)                                                      // Key for this node.  Note: we do not order nodes automatically.
 {return tree.proto->noden(tree, key, 0);
 }

static  ArenaTreeNode nodeFromStringBuffer_ArenaTree_ArenaTreeNode_ArenaTree_StringBuffer                       // Create a new tree node from a String Buffer
 (const ArenaTree             tree,                                                     // Arena tree in which to create the node
  const StringBuffer  string)                                                   // Key for this node as a string buffer
 {makeLocalCopyOfStringBuffer(b, l, string);                                    // Local copy
  const typeof(tree.proto->noden(tree, b, l)) n = tree.proto->noden(tree, b, l);                                                       // Node from string of known length
  return n;
 }

static ArenaTreeOffset saveString_ArenaTreeOffset_ArenaTree_ArenaTreeString                                     //P Save a copy of a zero terminated string in a tree and return the offset of the string.
 (const ArenaTree       tree,                                                           // Arena tree in which to create the node
  const char * const str,                                                       // String
  const size_t  length)                                                         // String, or if zero I will call strlen
 {const typeof(length ? : strlen(str)) l = length ? : strlen(str);                                                   // Length of string
  const typeof(tree.proto->allocate(tree, l + 1)) o = tree.proto->allocate(tree, l + 1);                                                   // Allocate space for the string plus a terminating zero
  const typeof(stpncpy(o.proto->pointer(o), str, l)) T = stpncpy(o.proto->pointer(o), str, l); *T = 0;
  return o;
 }

static void get_ArenaTree_data_size                                                     // Get a copy of the data at the specified offset in the arena of a tree.
 (const ArenaTree      tree,                                                            // Arena tree
  const size_t offset,                                                          // Offset to data in arena
  void * const data,                                                            // Pointer to the area into which the data is to be copied
  const size_t length)                                                          // Length of the data to be retrieved
 {memcpy(data, tree.proto->pointer(tree, offset), length);                                 // Copy out data
 }

static ArenaTreeOffset store_offset_ArenaTree_data_size                                         // Store a copy of the specified data in the arena of the tree and return the offset of the data.
 (const ArenaTree       tree,                                                           // Tree in whose arena the data will be stored
  const void *  const data,                                                     // Pointer to the data to be stored
  const size_t  length)                                                         // Length of the data to be stored
 {       const typeof(tree.proto->allocate(tree, length)) o = tree.proto->allocate(tree, length);                                           // Allocate space for data
  memcpy(o.proto->pointer(o), data, length);                                            // Copy in data
  return o;
 }

static void retrieve_ArenaTree_ArenaTreeOffset_data_size                                        // Retrieve a copy of the data stored at the specified offset in the arena of the specified tree.
 (const ArenaTree       tree,                                                           // Tree in whose arena the data is stored.
  const ArenaTreeOffset offset,                                                         // Offset in the arena at which the data to be retrieved is stored.
  void * const  data,                                                           // Area into which the retrieved data is to be copied.
  const size_t  length)                                                         // Length of the data to be retrieved
 {memcpy(data, tree.proto->pointer(tree, offset.offset), length);                          // Copy out data
 }

static void get_ArenaTreeNode_data_size                                                 // Get a copy of the data addressed by a node.
 (const ArenaTreeNode  node,                                                            // Node in an arena tree associated with the data
  void * const data,                                                            // Pointer to the area into which the data is to be copied
  const size_t length)                                                          // Length of the data to be retrieved
 {node.tree.proto->get(node.tree, node.proto->content(node)->data.offset, data, length);                   // Locate data
 }

static void set_ArenaTreeNode_data_size                                                 // Save the specified data in the arena of the tree and return the offset of the data.
 (const ArenaTreeNode  node,                                                            // Node in an arena tree to associate with the data
  const void * const data,                                                      // Pointer to the data to be saved
  const size_t length)                                                          // Length of the data to be saved
 {node.proto->content(node)->data.offset = node.tree.proto->store(node.tree, data, length).offset;         // Record offset of saved data
 }

static void copyData_ArenaTreeNode_ArenaTreeNode                                                // Copy the data offset associated with the source node to the target node by copying the offset to the data held in the source node to the target node.
 (const ArenaTreeNode target,                                                           // Target node
  const ArenaTreeNode source)                                                           // Source node
 {target.proto->content(target)->data = source.proto->content(source)->data;                              // Copy data offset
 }

static size_t getData_size_ArenaTreeNode                                                // Get the value of the data offset associated with a node.
 (const ArenaTreeNode  node)                                                            // Node in an arena tree associated with the data
 {return node.proto->content(node)->data.offset;                                           // Value of the data offset field
 }

static void setData_ArenaTreeNode_size                                                  // Set the value of the data offset associated with a node.
 (const ArenaTreeNode  node,                                                            // Node in an arena tree to associate with the data
  const size_t offset)                                                          // Value the data offset is to be set to
 {node.proto->content(node)->data.offset = offset;                                         // Record offset
 }

static void fromLetters_ArenaTree_ArenaTreeString                                               // Load tree from a string of letters and brackets.  The root node of the tree so constructed is always given the letter 'a'.
 (const ArenaTree tree,                                                                 // Tree
  ArenaTreeString str)                                                                  // String of letters and brackets describing desired tree structure
 {const ArenaTreeNode n = tree.proto->root(tree);                                                  // The node we are currently  adding to
  n.proto->setKey(n, "a");                                                              // The root is always called "a"

  void parse(ArenaTreeNode current)                                                     // Parse the string of letters and brackets to construct the desired tree
   {while(*str)
     {const typeof(*str) c = *str;
      switch(c)
       {case '(':                                                               // Enter new parent scope
         {++str;
          parse(current.proto->last(current));
          break;
         }
        case ')':                                                               // Exit current parent scope
         {++str; return;
         }
        default:                                                                // Add to current parent
         {char s[2] = {c, 0};
          current.proto->putLast(current, tree.proto->node(tree, s));
          ++str; continue;
         }
       }
     }
   }

  parse(n);                                                                     // Parse the string of letters and brackets to construct the desired tree
 };

static void free_ArenaTree                                                              // Free an entire arena tree.
 (const ArenaTree tree)                                                                 // Arena tree to free
 {const typeof(tree.arena) a = tree.arena;
  if  (a->data) free(a->data);
  free(a);
 }

//D1 Navigation                                                                 // Navigate through a tree.

static int valid_ArenaTreeNode                                                          // Check that a node is valid.
 (const ArenaTreeNode child)                                                            // Node
 {return child.offset;                                                          // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

static  ArenaTreeNode parent_ArenaTreeNode_ArenaTreeNode                                                // Get the parent of a child
 (const ArenaTreeNode child)                                                            // Child
 {if (child.proto->isRoot(child)) return child;
  return child.tree.proto->nodeFromOffset(child.tree, child.proto->content(child)->parent.offset);
 }

static  ArenaTreeNode first_ArenaTreeNode_ArenaTreeNode                                                 // Get the first child under a parent.
 (const ArenaTreeNode parent)                                                           // Parent
 {return  parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->first.offset);
 }
static  ArenaTreeNode last_ArenaTreeNode_ArenaTreeNode                                                 // Get the last child under a parent.
 (const ArenaTreeNode parent)                                                           // Parent
 {return  parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->last.offset);
 }
#line 358 "/home/phil/c/z/arenaTree/arenaTree.c"
static  ArenaTreeNode next_ArenaTreeNode_ArenaTreeNode                                                 // Get the next child under a parent.
 (const ArenaTreeNode parent)                                                           // Parent
 {return  parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->next.offset);
 }
#line 358 "/home/phil/c/z/arenaTree/arenaTree.c"
static  ArenaTreeNode prev_ArenaTreeNode_ArenaTreeNode                                                 // Get the prev child under a parent.
 (const ArenaTreeNode parent)                                                           // Parent
 {return  parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->prev.offset);
 }
#line 358 "/home/phil/c/z/arenaTree/arenaTree.c"

static  ArenaTreeNode first_ArenaTreeNode_ArenaTree                                                     // Get the first child in the specified ArenaTree.
 (const ArenaTree tree)                                                                 // Parent
 {const ArenaTreeNode root = tree.proto->root(tree);
  return root.proto->first(root);
 }
static  ArenaTreeNode last_ArenaTreeNode_ArenaTree                                                     // Get the last child in the specified ArenaTree.
 (const ArenaTree tree)                                                                 // Parent
 {const ArenaTreeNode root = tree.proto->root(tree);
  return root.proto->last(root);
 }
#line 365 "/home/phil/c/z/arenaTree/arenaTree.c"
static  ArenaTreeNode next_ArenaTreeNode_ArenaTree                                                     // Get the next child in the specified ArenaTree.
 (const ArenaTree tree)                                                                 // Parent
 {const ArenaTreeNode root = tree.proto->root(tree);
  return root.proto->next(root);
 }
#line 365 "/home/phil/c/z/arenaTree/arenaTree.c"
static  ArenaTreeNode prev_ArenaTreeNode_ArenaTree                                                     // Get the prev child in the specified ArenaTree.
 (const ArenaTree tree)                                                                 // Parent
 {const ArenaTreeNode root = tree.proto->root(tree);
  return root.proto->prev(root);
 }
#line 365 "/home/phil/c/z/arenaTree/arenaTree.c"

//D1 Search                                                                     // Search for nodes.

static int equalsString_ArenaTreeNode_string                                            // Check that the key of a node
 (const ArenaTreeNode        node,                                                      // Node
  const char * const key)                                                       // Key
 {return !strcmp(node.proto->key(node), key);
 }

static  ArenaTreeNode findFirst_ArenaTreeNode_string                                            // Find the first node with the specified key in a post-order traversal of the tree starting at the specified node.
 (const ArenaTreeNode        node,                                                      // Node at the start of the tree to be searched
  const char * const key)                                                       // Key to find
 {jmp_buf found;
  ArenaTreeNode F = newArenaTreeNode(({struct ArenaTreeNode t = {tree: node.tree, proto: &ProtoTypes_ArenaTreeNode}; t;}));                                         // An invalid node

  void find(ArenaTreeNode node)                                                         // Check whether the key of the current node matches the specified key
   {if (node.proto->equalsString(node, key))                                               // Found
     {F.offset = node.offset;
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) node.proto->by(node, find);                                          // Search the tree

  return F;                                                                     // Return an invalid node
 }

static  ArenaTreeNode findFirst_ArenaTree_string                                                // Find the first node with the specified key in a post-order traversal of the tree.
 (const ArenaTree            tree,                                                      // Tree
  const char * const key)                                                       // Key to find
 {jmp_buf found;
  ArenaTreeNode F = newArenaTreeNode(({struct ArenaTreeNode t = {tree: tree, proto: &ProtoTypes_ArenaTreeNode}; t;}));                                              // An invalid node

  void find(ArenaTreeNode node)                                                         // Check whether the key of the current node matches the specified key
   {if (node.proto->equalsString(node, key))                                               // Found
     {F.offset = node.offset;
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) tree.proto->by(tree, find);                                          // Search the tree

  return F;                                                                     // Return an invalid node
 }

static  ArenaTreeNode findFirstChild_ArenaTreeNode_string                                       // Find the first child of the specified parent with the specified key.
 (const ArenaTreeNode        parent,                                                    // Parent node
  const char * const key)                                                       // Key to find immediately under parent
 {ArenaTreefe(child, parent) if (child.proto->equalsString(child, key)) return child;               // Found matching child

  const ArenaTreeNode invalid = {};                                                     // An invalid node
  return invalid;                                                               // Failed - return an invalid node
 }

static  ArenaTreeNode findFirstChild_ArenaTree_string                                           // Find the first child immediately under the root with the specified key.
 (const ArenaTree            tree,                                                      // Tree
  const char * const key)                                                       // Key to find
 {const ArenaTreeNode  r = tree.proto->root(tree);                                                 // Root node of the tree
  return r.proto->findFirstChild(r, key);                                               // Search the tree
 }

//D1 Location                                                                   // Verify the current location.

static int context_ArenaTreeNode                                                        // Return true if the parent of the specified child has the specified key.
 (const ArenaTreeNode         child,                                                    // Child
        ArenaTreeNode * const parent,                                                   // Parent container
  const char  * const key)                                                      // Key
 {if (child.proto->isRoot(child)) return 0;                                                 // The root node has no context
  const typeof(*parent = child.proto->parent(child)) p = *parent = child.proto->parent(child);
  return p.proto->valid(p) && !strcmp(p.proto->key(p), key);                                    // Check key
 }

static int isFirst_ArenaTreeNode                                                        // Confirm a child is first under its parent
 (const ArenaTreeNode child)                                                            // Child
 {const ArenaTreeNode parent = child.proto->parent(child);
  return child.proto->equals(child, parent.proto->first(parent));
 }
static int isLast_ArenaTreeNode                                                        // Confirm a child is last under its parent
 (const ArenaTreeNode child)                                                            // Child
 {const ArenaTreeNode parent = child.proto->parent(child);
  return child.proto->equals(child, parent.proto->last(parent));
 }
#line 443 "/home/phil/c/z/arenaTree/arenaTree.c"

static int isEmpty_ArenaTreeNode                                                        // Confirm a node has no children.
 (const ArenaTreeNode node)                                                             // Node
 {const ArenaTreeNode child = node.proto->first(node);
  return !child.proto->valid(child);                                                        // No first child so the node is empty
 }

static int isOnlyChild_ArenaTreeNode                                                    // Confirm that this child is the only child of its parent
 (const ArenaTreeNode child)                                                            // Child
 {const typeof(child.proto->parent(child)) parent = child.proto->parent(child);                                                      // Parent
  return parent.proto->valid(parent) && child.proto->isFirst(child) && child.proto->isLast(child);                   // Child is first and last and not the root so it is an only child
 }

static int isRoot_ArenaTreeNode                                                         // Confirm a node is the root
 (const ArenaTreeNode node)                                                             // NodeContent
 {return !node.offset;
 }

//D1 Put                                                                        // Insert children into a tree.

static  ArenaTreeNode putFL_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                           //P Put a child first or last under its parent
 (const int   first,                                                            // Put first if true, else last
  const ArenaTreeNode parent,                                                           // Parent
  const ArenaTreeNode child)                                                            // Child
 {const typeof(parent.tree) tree = parent.tree;
  if (child.tree.arena != tree.arena) printStackBackTrace("Wrong tree\n");      // Parent and child must be in same tree
  const typeof(parent.offset) P = parent.offset;    const typeof(child.offset) C = child.offset;                                       // Delta of parent and child
  const typeof(parent.proto->content(parent)) p = parent.proto->content(parent); const typeof(child.proto->content(child)) c = child.proto->content(child);                                   // Pointers to parent and child
  const typeof(first ? p->first.offset : p->last.offset) L = first ? p->first.offset : p->last.offset;                                 // Delta of child currently first/last
  if (!L)                                                                       // No children yet
   {if (first) p->last.offset = C; else p->first.offset = C;                    // Delta to last/first child
   }
  else                                                                          // Existing children
   {ArenaTreeContent * const l = tree.proto->pointer(tree, L);                                     // Link first/last two children together
    if (first) l->prev.offset = C; else l->next.offset = C;
    if (first) c->next.offset = L; else c->prev.offset = L;
   }
  c->parent.offset = P;                                                         // Parent new first/last child
  if (first)  p->first.offset = C; else p->last.offset = C;                     // Parent points to new first/last child
  return child;
 }

static  ArenaTreeNode putTreeFirst_ArenaTreeNode_ArenaTreeNode                                          // Put a child first in the tree containing the arena in which the child was allocated.
 (const ArenaTreeNode child)                                                            // Child
 {const typeof(child.tree) t = child.tree;                                                               // Tree containing arena containing child
  const typeof(t.proto->root(t)) r = t.proto->root(t);
  return r.proto->putFirst(r, child);                                                   // Put the child first
 }
static  ArenaTreeNode putTreeLast_ArenaTreeNode_ArenaTreeNode                                          // Put a child last in the tree containing the arena in which the child was allocated.
 (const ArenaTreeNode child)                                                            // Child
 {const typeof(child.tree) t = child.tree;                                                               // Tree containing arena containing child
  const typeof(t.proto->root(t)) r = t.proto->root(t);
  return r.proto->putLast(r, child);                                                   // Put the child last
 }
#line 492 "/home/phil/c/z/arenaTree/arenaTree.c"

static  ArenaTreeNode putFirst_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                        // Put a child first under its parent
 (const ArenaTreeNode parent,                                                           // Parent
  const ArenaTreeNode child)                                                            // Child
 {return putFL_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode(1, parent, child);                             // Put a child first under its parent
 }
static  ArenaTreeNode putLast_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                        // Put a child last under its parent
 (const ArenaTreeNode parent,                                                           // Parent
  const ArenaTreeNode child)                                                            // Child
 {return putFL_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode(0, parent, child);                             // Put a child last under its parent
 }
#line 499 "/home/phil/c/z/arenaTree/arenaTree.c"

static  ArenaTreeNode putNP_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                           //P Put a child next or previous to the specified sibling
 (const int   next,                                                             // Put next if true, else previous
  const ArenaTreeNode sibling,                                                          // Sibling
  const ArenaTreeNode child)                                                            // Child
 {const typeof(sibling.tree) tree = sibling.tree;                                                        // Tree
  const typeof(sibling.proto->parent(sibling)) parent = sibling.proto->parent(sibling);                                                    // Parent node
  const typeof(next ? sibling.proto->next(sibling) : sibling.proto->prev(sibling)) twin = next ? sibling.proto->next(sibling) : sibling.proto->prev(sibling);                              // Next or prev sibling to sibling is such a node exists
  assert(child.tree.arena == tree.arena);                                       // Parent and child must be in same tree
  const typeof(sibling.proto->content(sibling)) s = sibling.proto->content(sibling);                                                        // Pointer to sibling
  const typeof(child.proto->content(child)) c = child.proto->content(child);                                                        // Pointer to child

  if (!twin.proto->valid(twin))                                                            // Sibling is last/first under parent
   {if (next) parent.proto->putLast(parent, child); else parent.proto->putFirst(parent, child);           // Place child
   }
  else                                                                          // Not last/first
   {const typeof(twin.proto->content(twin)) w = twin.proto->content(twin);                                                         // Pointer to twin of sibling
    if (next)
     {s->next.offset = w->prev.offset = child.offset;
      c->prev.offset = sibling.offset; c->next.offset = twin.offset;
     }
    else
     {s->prev.offset = w->next.offset = child.offset;
      c->next.offset = sibling.offset; c->prev.offset = twin.offset;
     }
   }
  c->parent.offset = parent.offset;                                             // Parent new next/prev child
  return child;
 }

static void setUp_ArenaTreeNode_ArenaTreeNode                                                   //P Make the specified parent node the parent of the specified child node
 (const ArenaTreeNode child,                                                            // Child
  const ArenaTreeNode parent)                                                           // Child
 {child.proto->content(child)->parent.offset = parent.offset;                               // Set parent of child
 }

static  ArenaTreeNode putNext_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                         // Put a child next after the specified sibling
 (const ArenaTreeNode sibling,                                                          // Sibling
  const ArenaTreeNode child)                                                            // Child
 {return putNP_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode(1, sibling, child);                            // Put child next
 }
static  ArenaTreeNode putPrev_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                         // Put a child previous after the specified sibling
 (const ArenaTreeNode sibling,                                                          // Sibling
  const ArenaTreeNode child)                                                            // Child
 {return putNP_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode(0, sibling, child);                            // Put child previous
 }
#line 541 "/home/phil/c/z/arenaTree/arenaTree.c"

//D1 Traverse                                                                   // Traverse a tree.

static void by_ArenaTreeNode_sub                                                        // Traverse the tree rooted at the specified node in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (ArenaTreeNode node,                                                                   // Node
  void (* const function) (const ArenaTreeNode node))                                   // Function
 {void children(const ArenaTreeNode parent)                                             // Process the children of the specified parent
   {if (!parent.proto->valid(parent)) return;                                                // Empty child
    const typeof(parent.proto->countChildren(parent)) N = parent.proto->countChildren(parent);                                                 // Number of children
    size_t c[N+1];                                                              // Array of children terminated by a trailing zero
    size_t n = 0; ArenaTreefe(child, parent) c[n++] = child.offset;                     // Load each child into an array
    for(size_t i = 0; i < N; ++i)                                               // Process each child allowing it to change its position without changing the traversal
     {children(newArenaTreeNode(({struct ArenaTreeNode t = {tree: parent.tree, offset: c[i], proto: &ProtoTypes_ArenaTreeNode}; t;})));
     }
    function(parent);                                                           // Process the parent
   }
  children(node);                                                               // Start at the specified root node
 }

static void by_ArenaTree_sub                                                            // Traverse a tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const ArenaTree tree,                                                                 // Tree
  void (* const function) (const ArenaTreeNode node))                                   // Function
 {const typeof(tree.proto->root(tree)) root = tree.proto->root(tree);                                                           // Root node
  const typeof(tree.proto->countChildren(tree)) N = tree.proto->countChildren(tree);                                                  // Number of children under root
  size_t c[N+1];                                                                // Array of children terminated by a trailing zero
  size_t n = 0; ArenaTreefe(child, root) c[n++] = child.offset;                         // Load each child into an array
  for(size_t i = 0; i < N; ++i)
   {ArenaTreeNode n = newArenaTreeNode(({struct ArenaTreeNode t = {tree: tree, offset: c[i], proto: &ProtoTypes_ArenaTreeNode}; t;}));                              // Process each child allowing it to change position
    n.proto->by(n, function);
   }
  function(root);
 }

static  size_t countChildren_size_ArenaTree                                             // Count the number of children directly under a parent.
 (const ArenaTree tree)                                                                 // ArenaTree
 {size_t l = 0;
  const typeof(tree.proto->root(tree)) root = tree.proto->root(tree);
  ArenaTreefe(child, root) ++l;
  return l;                                                                     // Return count
 }

static  size_t countChildren_size_ArenaTreeNode                                         // Count the number of children directly under a node.
 (const ArenaTreeNode parent)                                                           // Parent
 {size_t l = 0;
  ArenaTreefe(child, parent) ++l;
  return l;                                                                     // Return count
 }

static  size_t count_size_ArenaTreeNode                                                 // Count the number of nodes under a specified node but not including the specified node.
 (const ArenaTreeNode node)                                                             // Node
 {size_t l = 0;

  void count(const ArenaTreeNode parent)                                                // Process the children of the specified parent
   {if (!parent.proto->valid(parent)) return;
    l++; ArenaTreefe(child, parent) count(child);                                       // Each child
   }

  count(node);                                                                  // Start at the specified node
  return l - 1;                                                                 // Return count without counting the root node
 }


static  size_t count_size_ArenaTree                                                     // Count the number of nodes in a tree
 (const ArenaTree tree)                                                                 // Tree
 {size_t l = 0;

  const typeof(tree.proto->root(tree)) root = tree.proto->root(tree);                                                           // Root is always invalid
  ArenaTreefe(child, root) l += 1 + child.proto->count(child);                                      // Child under root plus its children
  return l;                                                                     // Return count without counting the root node
 }

//D1 Print                                                                      // Print Arena Trees in various ways

static  ArenaTreeString printWithBrackets_string_ArenaTreeNode                                  // Print a node and the tree below it in preorder as a string with each sub tree enclosed in brackets.
 (const ArenaTreeNode   node)                                                           // Node
 {size_t l = 0;                                                                 // Length of output string
  void len(const ArenaTreeNode parent)                                                  // Find the size of buffer we will need
   {l += strlen(parent.proto->key(parent));                                                  // Parent key
    if (!parent.proto->isEmpty(parent))                                                      // Parent has children
     {l += 2;                                                                   // Space for brackets
      ArenaTreefe(child, parent) len(child);                                            // Each child
     }
   }

  ArenaTreeString p;
  void print(const ArenaTreeNode parent)                                                // Print the children of the specified parent
   {const ArenaTreeString k = parent.proto->key(parent);
    p = stpcpy(p, k);
    if (!parent.proto->isEmpty(parent))                                                      // Parent has children
     {*p++ = '(';                                                               // Start children
      ArenaTreefe(child, parent) print(child);                                          // Each child
      *p++ = ')';                                                               // Start children
     }
    *p = 0;                                                                     // End the string
   }

  len(node);                                                                    // Length of the print
  const typeof(p = alloc(l+1)) s = p = alloc(l+1); *s = 0;
  print(node);
  return s;
 }

static  ArenaTreeString printWithBrackets_ArenaTreeString_ArenaTree                                     // Print an entire tree in preorder as a string with brackets around each sub tree
 (const ArenaTree t)                                                                    // Tree
 {const typeof(t.proto->root(t)) node = t.proto->root(t);                                                             // Root
  return node.proto->printWithBrackets(node);
 }

static int printsWithBracketsAs_int_ArenaTreeNode_string                                // Check that the ArenaTree starting at the specified node prints with brackets as expected.
 (const ArenaTreeNode        node,                                                      // Node
  const char * const expected)                                                  // Expected string when printed
 {const typeof(node.proto->printWithBrackets(node)) s = node.proto->printWithBrackets(node);
  const typeof(!strcmp(s, expected)) r = !strcmp(s, expected);
  free(s);
  return r;
 }

static int printsWithBracketsAs_int_ArenaTree_string                                    // Check that the specified ArenaTree prints with brackets as expected.
 (const ArenaTree            tree,                                                      // ArenaTree
  const char * const expected)                                                  // Expected string when printed
 {       const typeof(tree.proto->root(tree)) root = tree.proto->root(tree);                                                    // Root
  return root.proto->printsWithBracketsAs(root, expected);
 }

static StringBuffer print_string_ArenaTreeNode                                          // Print a node and the ArenaTree below it in preorder as a string.
 (const ArenaTreeNode node)                                                             // Node to print from
 {const typeof(makeStringBuffer()) s = makeStringBuffer();                                                       // String buffer being constructed

  void print(const ArenaTreeNode parent)                                                // Print the children of the specified parent
   {s.proto->add(s, parent.proto->key(parent));
    ArenaTreefe(child, parent) print(child);                                            // Each child
   }

  print(node);
  return s;
 }

static StringBuffer print_string_ArenaTree                                              // Print an entire tree in preorder as a string.
 (const ArenaTree tree)                                                                 // ArenaTree
 {        const typeof(tree.proto->root(tree)) node = tree.proto->root(tree);                                                   // Root
  return  node.proto->print(node);
 }

static int printsAs_int_ArenaTreeNode_string                                            // Check that the ArenaTree starting at the specified node prints as expected.
 (const ArenaTreeNode        node,                                                      // Node
  const char * const expected)                                                  // Expected string when printed
 {const typeof(node.proto->print(node)) s = node.proto->print(node);
  const typeof(s.proto->equalsString(s, expected)) r = s.proto->equalsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printsAs_int_ArenaTree_string                                                // Check that the specified ArenaTree prints as expected.
 (const ArenaTree            tree,                                                      // ArenaTree
  const char * const expected)                                                  // Expected string when printed
 {       const typeof(tree.proto->root(tree)) root = tree.proto->root(tree);                                                    // Root
  return root.proto->printsAs(root, expected);
 }

static int printContains_ArenaTreeNode                                                  // Check the print of an ArenaTree starting at the specified tag contains the expected string.
 (const ArenaTreeNode   node,                                                           // Starting node
  const char *  expected)                                                       // Expected string
 {const typeof(node.proto->print(node)) s = node.proto->print(node);
  const typeof(s.proto->containsString(s, expected)) r = s.proto->containsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printContains_ArenaTree                                                      // Check the print of an ArenaTree contains the expected string.
 (const ArenaTree       tree,                                                           // ArenaTree parse tree
  const char *  expected)                                                       // Expected string
 {const typeof(tree.proto->print(tree)) s = tree.proto->print(tree);
  const typeof(s.proto->containsString(s, expected)) r = s.proto->containsString(s, expected);
  s.proto->free(s);
  return r;
 }

//D1 Edit                                                                       // Edit a tree in situ.

static  ArenaTreeNode cut_ArenaTreeNode_ArenaTreeNode                                                   // Cut out a child.
 (const ArenaTreeNode child)                                                            // Child to cut out
 {const ArenaTreeNode parent = child.proto->parent(child);                                         // Parent
  const typeof(parent.proto->content(parent)) p = parent.proto->content(parent); const typeof(child.proto->content(child)) c = child.proto->content(child);                                    // Parent pointer, child content

  if (child.proto->isOnlyChild(child))                                                      // Only child
   {p->first.offset = p->last.offset = 0;                                       // Remove child
   }
  else if (child.proto->isLast(child))                                                      // Last child
   {const typeof(child.proto->prev(child)) L = child.proto->prev(child);
    const typeof(L.proto->content(L)) l = L.proto->content(L);
    p->last.offset = L.offset; l->next.offset = 0;                              // Last child under parent
   }
  else if (child.proto->isFirst(child))                                                     // First child
   {const typeof(child.proto->next(child)) F = child.proto->next(child);
    const typeof(F.proto->content(F)) f = F.proto->content(F);
    p->first.offset = F.offset; f->prev.offset = 0;                             // Last child under parent
   }
  else                                                                          // Some where in the middle
   {const typeof(child.proto->next(child)) N = child.proto->next(child);    const typeof(child.proto->prev(child)) P = child.proto->prev(child);
    const typeof(N.proto->content(N)) n = N.proto->content(N); const typeof(P.proto->content(P)) p = P.proto->content(P);
    p->next.offset = N.offset; n->prev.offset = P.offset;
   }

  c->next.offset = c->prev.offset = 0;                                          // Remove child

  return child;
 }

static  ArenaTreeNode unwrap_ArenaTreeNode_ArenaTreeNode                                                // Unwrap the specified parent and return it.
 (const ArenaTreeNode parent)                                                           // Parent to unwrap
 {for(ArenaTreeNode child = newArenaTreeNode(({struct ArenaTreeNode t = {tree: parent.tree, offset: parent.proto->last(parent).offset, proto: &ProtoTypes_ArenaTreeNode}; t;})); // Remove last children
    child.proto->valid(child);
    child.offset = parent.proto->last(parent).offset)
   {parent.proto->putNext(parent, child.proto->cut(child));                                              // Place each child after the parent
   }
  return parent.proto->cut(parent);                                                          // Remove and return empty parent
 }

static  ArenaTreeNode wrap_ArenaTreeNode_ArenaTreeString                                                // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
 (const ArenaTreeNode   child,                                                          // Child to wrap
  const char * const key)                                                       // Key for new parent
 {const typeof(child.tree) tree = child.tree;                                                          // Tree
  const typeof(tree.proto->node(tree, key)) parent = tree.proto->node(tree, key);                                                    // New parent
  child.proto->putNext(child, parent);                                                     // Place parent after child
  parent.proto->putLast(parent, child.proto->cut(child));                                                // Place child under parent
  return parent;
 }

//D1 Input and Output                                                           // Read and write a tree from/to a file

static void write_void_ArenaTree_ArenaTreeString                                                // Write a tree to a named file or abort
 (const ArenaTree       tree,                                                           // Tree
  const char * const file)                                                      // File
 {    const typeof(open(file, O_CREAT| O_WRONLY, S_IRWXU)) o = open(file, O_CREAT| O_WRONLY, S_IRWXU);                               // Open for output creating if needed
  if (o < 0) printStackBackTrace("Cannot open file: %s for write\n", file);

  time_t current; time(&current);
  const ArenaTreeDescription h = {1, 1, 0, tree.proto->used(tree), tree.proto->root(tree).offset, current};   // Create arena tree header describing the tree

  if (sizeof(h) != write(o, &h, sizeof(h)))                                     // Write header
   {printStackBackTrace("Cannot write ArenaTree header to file: %s\n", file);
   }

  const typeof(write(o, tree.arena->data, tree.arena->used)) w = write(o, tree.arena->data, tree.arena->used);                             // Write arena
  if (w < 0 || tree.arena->used != (size_t)w)
   {printStackBackTrace("Cannot write ArenaTree arena to file: %s\n", file);
   }

  close(o);
 }

ArenaTree readArenaTree                                                                         // Read a tree from a file
 (const char * const file)                                                      // File
 {ArenaTreeArena * const arena = alloc(sizeof(ArenaTreeArena));                                 // Create arena
  const typeof(newArenaTree(({struct ArenaTree t = {arena: arena, proto: &ProtoTypes_ArenaTree}; t;}))) tree = newArenaTree(({struct ArenaTree t = {arena: arena, proto: &ProtoTypes_ArenaTree}; t;}));                                                   // Initialize tree

      const typeof(open(file, 0, O_RDONLY)) i = open(file, 0, O_RDONLY);                                              // Open for input
  if (i < 0) printStackBackTrace("Cannot open file: %s for read\n", file);

  struct ArenaTreeDescription h;
  if (sizeof(h) != read(i, &h, sizeof(h)))                                      // Read header
   {printStackBackTrace("Cannot read header from file: %s\n", file);
   }

  tree.arena->data = alloc(arena->size = arena->used = h.used);                 // Allocate arena

      const typeof(read(i, arena->data, arena->used)) r = read(i, arena->data, arena->used);                                    // Read arena
  if (r < 0 || arena->used != (size_t)r)
   {printStackBackTrace("Cannot read ArenaTree from file: %s\n", file);
   }

  close(i);
  arena->root = h.root;                                                         // Offset to root in arena
  return tree;
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TmakeArenaTree //Tnode //Tfree //TputFirst //TputLast //Tfe //Tfer
 {   const typeof(makeArenaTree()) t = makeArenaTree();                                                               // Create a tree
  const typeof(t.proto->root(t)) root = t.proto->root(t);

  for(int i = 0; i < 10; ++i)                                                   // Load tree
   {char s[2] = {'a'+i, 0};

    const typeof(t.proto->node(t, s)) c1 = t.proto->node(t, s); root.proto->putLast(root, c1);
    const typeof(t.proto->node(t, s)) c2 = t.proto->node(t, s); root.proto->putFirst(root, c2);
    s[0] = 'A'+i; c2.proto->setKey(c2, s);
    assert(strcmp(c2.proto->key(c2), s) == 0);
   }

  if (1)                                                                        // For each
   {char l[t.proto->count(t) + 1]; *l = 0;
    ArenaTreefe (child, root) strcat(l, child.proto->key(child));
    assert(strcmp(l, "JIHGFEDCBAabcdefghij") == 0);
   }

  if (1)                                                                        // For each in reverse
   {char l[t.proto->count(t) + 1]; *l = 0;
    ArenaTreefer(child, root) strcat(l, child.proto->key(child));
    assert(strcmp(l, "jihgfedcbaABCDEFGHIJ") == 0);
   }

  t.proto->free(t);
 }

void test1()                                                                    //Troot //Tfirst //Tlast //Tnext //Tprev //Tparent //Tequals //Tprint //TprintWithBrackets //TfromLetters //TprintsAs
 {const typeof(makeArenaTree()) t = makeArenaTree();    t.proto->fromLetters(t, "b(c(de)f)g(hi)j");
  assert(t.proto->printsWithBracketsAs(t, "a(b(c(de)f)g(hi)j)"));
  assert(t.proto->printsAs(t, "abcdefghij"));

  const typeof(t.proto->root(t)) a = t.proto->root(t);
  const typeof(a.proto->first(a)) b = a.proto->first(a);
  const typeof(b.proto->first(b)) c = b.proto->first(b);
  const typeof(c.proto->last(c)) e = c.proto->last(c);
  const typeof(e.proto->prev(e)) d = e.proto->prev(e);

  ArenaTreeString     k = d.proto->key(d);
  assert(k[0] == 'd');
  b.proto->printsWithBracketsAs(b, "b(c(de)f)");
  c.proto->printsWithBracketsAs(c, "c(de)");

  assert(c.proto->equals(c, d.proto->parent(d)));
  assert(b.proto->equals(b, c.proto->parent(c)));
  assert(a.proto->equals(a, b.proto->parent(b)));
  assert(a.proto->equals(a, t.proto->root(t)));

  assert(c.proto->printsAs(c, "cde"));
  assert(t.proto->printsAs(t, "abcdefghij"));

  t.proto->free(t);
 }

void test2()                                                                    //Tby //TprintsWithBracketsAs //TprintContains
 {const typeof(makeArenaTree()) t = makeArenaTree();      t.proto->fromLetters(t, "b(c(de)f)g(hi)j");
  assert(t.proto->printsWithBracketsAs(t, "a(b(c(de)f)g(hi)j)"));
  assert(t.proto->printContains(t, "def"));

  char l[t.proto->count(t) + 2], *p = l;

  void process(ArenaTreeNode n) {p = stpcpy(p, n.proto->key(n)); *p = 0;}
  t.proto->by(t, process);
  assert(strcmp(l, "decfbhigja") == 0);

  t.proto->free(t);
 }

void test3()                                                                    //TisFirst //TisLast //TisEmpty //TisRoot //TisOnlyChild //Tcontext //TcheckKey //Tvalid
 {const typeof(makeArenaTree()) t = makeArenaTree();      t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
  assert(t.proto->printsWithBracketsAs(t, "a(b(c(de(f)gh)i)j)"));

  const typeof(t.proto->root(t)) a = t.proto->root(t);  assert(a.proto->checkKey(a, "a", 0));                                 assert(a.proto->printsWithBracketsAs(a,  "a(b(c(de(f)gh)i)j)"));
  const typeof(a.proto->first(a)) b = a.proto->first(a); assert(b.proto->checkKey(b, "b", 0)); assert(a.proto->equals(a, b.proto->parent(b))); assert(b.proto->printsWithBracketsAs(b,    "b(c(de(f)gh)i)"));
  const typeof(b.proto->first(b)) c = b.proto->first(b); assert(c.proto->checkKey(c, "c", 0)); assert(b.proto->equals(b, c.proto->parent(c))); assert(c.proto->printsWithBracketsAs(c,      "c(de(f)gh)"));
  const typeof(c.proto->first(c)) d = c.proto->first(c); assert(d.proto->checkKey(d, "d", 0)); assert(c.proto->equals(c, d.proto->parent(d))); assert(d.proto->printsWithBracketsAs(d,        "d"));
  const typeof(d.proto->next(d)) e = d.proto->next(d);  assert(e.proto->checkKey(e, "e", 0)); assert(c.proto->equals(c, e.proto->parent(e))); assert(e.proto->printsWithBracketsAs(e,         "e(f)"));
  const typeof(e.proto->last(e)) f = e.proto->last(e);  assert(f.proto->checkKey(f, "f", 0)); assert(e.proto->equals(e, f.proto->parent(f))); assert(f.proto->printsWithBracketsAs(f,           "f"));
  const typeof(e.proto->next(e)) g = e.proto->next(e);  assert(g.proto->checkKey(g, "g", 0)); assert(c.proto->equals(c, g.proto->parent(g))); assert(g.proto->printsWithBracketsAs(g,             "g"));
  const typeof(g.proto->next(g)) h = g.proto->next(g);  assert(h.proto->checkKey(h, "h", 0)); assert(c.proto->equals(c, h.proto->parent(h))); assert(h.proto->printsWithBracketsAs(h,              "h"));
  const typeof(c.proto->next(c)) i = c.proto->next(c);  assert(i.proto->checkKey(i, "i", 0)); assert(b.proto->equals(b, i.proto->parent(i))); assert(i.proto->printsWithBracketsAs(i,                "i"));
  const typeof(b.proto->next(b)) j = b.proto->next(b);  assert(j.proto->checkKey(j, "j", 0)); assert(a.proto->equals(a, j.proto->parent(j))); assert(j.proto->printsWithBracketsAs(j,                  "j"));

  assert(!a.proto->valid(a));
  assert( b.proto->isFirst(b));
  assert( j.proto->isLast(j));
  assert( f.proto->isFirst(f));
  assert( f.proto->isLast(f));

  assert( f.proto->isEmpty(f));
  assert( f.proto->isOnlyChild(f));
  assert(!e.proto->isOnlyChild(e));
  assert( a.proto->isRoot(a));

  ArenaTreeNode A, C, E; assert(f.proto->context(f, &E, "e") && E.proto->context(E, &C, "c"));
  assert(!a.proto->context(a, &A, "a"));

  t.proto->free(t);
 }

void test4()                                                                    //Tcut //TfindFirst //TcountChildren //TequalsString //TfindFirstChild
 {const typeof(makeArenaTree()) t = makeArenaTree();      t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
  assert(t.proto->printsWithBracketsAs(t, "a(b(c(de(f)gh)i)j)"));

  const typeof(t.proto->findFirst(t, "c")) c = t.proto->findFirst(t, "c");

  assert(c.proto->equalsString(c, "c"));
  const typeof(t.proto->findFirst(t, "d")) d = t.proto->findFirst(t, "d"); assert(d.proto->equalsString(d, "d"));
  const typeof(t.proto->findFirst(t, "e")) e = t.proto->findFirst(t, "e"); assert(e.proto->equalsString(e, "e"));
  const typeof(t.proto->findFirst(t, "f")) f = t.proto->findFirst(t, "f"); assert(f.proto->equalsString(f, "f"));
  const typeof(t.proto->findFirst(t, "g")) g = t.proto->findFirst(t, "g"); assert(g.proto->equalsString(g, "g"));
  const typeof(t.proto->findFirst(t, "h")) h = t.proto->findFirst(t, "h"); assert(h.proto->equalsString(h, "h"));

  assert(g.proto->equals(g, c.proto->findFirstChild(c, "g")));
  assert(c.proto->countChildren(c) == 4);
  assert(e.proto->countChildren(e) == 1);

  f.proto->cut(f);           assert(t.proto->printsWithBracketsAs(t, "a(b(c(degh)i)j)"));
  e.proto->putFirst(e, f);   assert(t.proto->printsWithBracketsAs(t, "a(b(c(de(f)gh)i)j)"));

  e.proto->cut(e);           assert(t.proto->printsWithBracketsAs(t, "a(b(c(dgh)i)j)"));
  d.proto->putNext(d, e);    assert(t.proto->printsWithBracketsAs(t, "a(b(c(de(f)gh)i)j)"));

  d.proto->cut(d);           assert(t.proto->printsWithBracketsAs(t, "a(b(c(e(f)gh)i)j)"));
  e.proto->putPrev(e, d);    assert(t.proto->printsWithBracketsAs(t, "a(b(c(de(f)gh)i)j)"));

  h.proto->cut(h);           assert(t.proto->printsWithBracketsAs(t, "a(b(c(de(f)g)i)j)"));
  g.proto->putNext(g, h);    assert(t.proto->printsWithBracketsAs(t, "a(b(c(de(f)gh)i)j)"));

  t.proto->free(t);
 }

void test5()                                                                    //TreadArenaTree //Twrite
 {const typeof(makeArenaTree()) t = makeArenaTree();      t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
  assert(t.proto->printsWithBracketsAs(t, "a(b(c(de(f)gh)i)j)"));

         const typeof("/tmp/arenaTreeTest.data") f = "/tmp/arenaTreeTest.data";
  assert(t.proto->printsWithBracketsAs(t, "a(b(c(de(f)gh)i)j)"));
         t.proto->write(t, f);

         const typeof(readArenaTree(f)) u = readArenaTree(f);
  assert(u.proto->printsWithBracketsAs(u, "a(b(c(de(f)gh)i)j)"));

  t.proto->free(t);
  u.proto->free(u);
  unlink(f);
 }

void test6()                                                                    //Tunwrap //Twrap //Tcount
 {const typeof(makeArenaTree()) t = makeArenaTree();    t.proto->fromLetters(t, "bce");
  assert(t.proto->printsWithBracketsAs(t, "a(bce)"));
  assert(t.proto->count(t) == 3);

         const typeof(t.proto->findFirst(t, "c")) c = t.proto->findFirst(t, "c");
  assert(c.proto->valid(c));

     const typeof(c.proto->wrap(c, "d")) d = c.proto->wrap(c, "d");
  assert(t.proto->printsWithBracketsAs(t, "a(bd(c)e)"));
  assert(t.proto->count(t) == 4);

         d.proto->unwrap(d);
  assert(t.proto->printsWithBracketsAs(t, "a(bce)"));
  assert(t.proto->count(t) == 3);
         t.proto->free(t);
 }

void test7()                                                                    //TsetKey //Tkey //TsaveString //Tused //Tsize
 {const typeof(makeArenaTree()) t = makeArenaTree(); t.proto->fromLetters(t, "");
  ArenaTreeDelta d = {offset : 1};
  assert(sizeof(d) == 4);

  const typeof(t.proto->saveString(t, "aaaa", 0)) a = t.proto->saveString(t, "aaaa", 0);
  assert(strcmp(a.proto->pointer(a), "aaaa") == 0);

                 const typeof(t.proto->node(t, "a")) n = t.proto->node(t, "a");
                 n.proto->setKey(n, "A");
  assert(strcmp (n.proto->key(n),     "A") == 0);

  t.proto->free(t);
 }

void test8()                                                                    //TputNext //TputPrev //Tnoden
 {const typeof(makeArenaTree()) t = makeArenaTree(); t.proto->fromLetters(t, "A");
  const typeof(t.proto->root(t)) a = t.proto->root(t);  assert((a.proto->key(a))[0] == 'a');
  const typeof(a.proto->first(a)) A = a.proto->first(a); assert((A.proto->key(A))[0] == 'A');

  for(int i = 0; i < 10; ++i)
   {char c[2] = {'0'+i};
    A.proto->putNext(A, t.proto->noden(t, c, 1));
    A.proto->putPrev(A, t.proto->noden(t, c, 1));
   }

  assert(t.proto->printsWithBracketsAs(t, "a(0123456789A9876543210)"));

  t.proto->free(t);
 }

void test9()                                                                    //Tget //Tset //TcopyData
 {const typeof(makeArenaTree()) t = makeArenaTree(); t.proto->fromLetters(t, "a");
  struct S
   {int a;
   } S = {1}, T;

  const typeof(t.proto->node(t, "b")) b = t.proto->node(t, "b");
  b.proto->set(b, &S, sizeof(S));
  T.a = 0;
  b.proto->get(b, &T, sizeof(T));
  assert (T.a == 1);

  const typeof(t.proto->node(t, "c")) c = t.proto->node(t, "c");
  c.proto->copyData(c, b);
  T.a = 0;
  c.proto->get(c, &T, sizeof(T));
  assert (T.a == 1);

  t.proto->free(t);
 }

void test10()                                                                   //TgetData //TsetData
 {const typeof(makeArenaTree()) t = makeArenaTree(); t.proto->fromLetters(t, "a");

          const typeof(t.proto->node(t, "b")) b = t.proto->node(t, "b");
          b.proto->setData(b, 42);
  assert (b.proto->getData(b) == 42);

  t.proto->free(t);
 }

void test11()                                                                   //Tstore //Tretrieve //TnewArenaTreeOffset
 {const typeof(makeArenaTree() ) t = makeArenaTree() ;

  const typeof(t.proto->store(t, "abcd", 4)) a = t.proto->store(t, "abcd", 4);
  char c[4];
  for(size_t i = 0; i < sizeof(c); ++i)
   {t.proto->retrieve(t, a, c, i);
    assert(!strncmp(c, "abcd", i));
   }

  if (1)                                                                        // Create an offset explicitly
   {void * p = a.proto->pointer(a);
           const typeof(t.proto->offsetTo(t, p)) A = t.proto->offsetTo(t, p);
    assert(a.offset == A.offset && a.tree.arena->data == A.tree.arena->data);
   }

  t.proto->free(t);
 }

void test12()                                                                   //TputTreeFirst //TputTreeLast
 {const typeof(makeArenaTree()) t = makeArenaTree(); const typeof(t.proto->root(t)) r = t.proto->root(t); r.proto->setKey(r, "a");

  const typeof(t.proto->node(t, "c")) c = t.proto->node(t, "c"); c.proto->putTreeFirst(c);
  const typeof(t.proto->node(t, "d")) d = t.proto->node(t, "d"); d.proto->putTreeLast(d);
  const typeof(t.proto->node(t, "b")) b = t.proto->node(t, "b"); b.proto->putTreeFirst(b);

  assert(t.proto->printsWithBracketsAs(t, "a(bcd)"));

  t.proto->free(t);
 }

void test13()                                                                   //TnodeFromStringBuffer
 {const typeof(makeArenaTree()) t = makeArenaTree();
  const typeof(makeStringBuffer()) s = makeStringBuffer();
  s.proto->add(s, "b");
  const typeof(t.proto->nodeFromStringBuffer(t, s)) n1 = t.proto->nodeFromStringBuffer(t, s); n1.proto->putTreeLast(n1);
  const typeof(t.proto->nodeFromStringBuffer(t, s)) n2 = t.proto->nodeFromStringBuffer(t, s); n2.proto->putTreeLast(n2);

  assert(t.proto->printsWithBracketsAs(t, "(bb)"));

  s.proto->free(s); t.proto->free(t);
 }

void test14()                                                                   //Tswap
 {const typeof(makeArenaTree()) s = makeArenaTree(); s.proto->fromLetters(s, "s");
  const typeof(makeArenaTree()) t = makeArenaTree(); t.proto->fromLetters(t, "t");
  assert(s.proto->printsWithBracketsAs(s, "a(s)"));
  assert(t.proto->printsWithBracketsAs(t, "a(t)"));
  s.proto->swap(s, t);
  assert(t.proto->printsWithBracketsAs(t, "a(s)"));
  assert(s.proto->printsWithBracketsAs(s, "a(t)"));
  s.proto->free(s);
  t.proto->free(t);
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0,  test1,  test2,  test3,  test4,
                           test5,  test6,  test7,  test8,  test9,
                           test10, test11, test12, test13, test14, 0};
  run_tests("ArenaTree", repetitions, tests);

  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaTree/arenaTree
