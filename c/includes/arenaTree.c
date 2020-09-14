#line 1 "/home/phil/c/z/arenaTree/arenaTree.c"
//------------------------------------------------------------------------------
// Trees held in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef ArenaTree_included
#define ArenaTree_included
#include <setjmp.h>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Tree.




typedef char * ArenaTreeString;                                                         // Arena Tree string
typedef struct ReadOnlyBytes ReadOnlyBytes;
typedef struct StringBuffer  StringBuffer;

typedef struct ArenaTree                                                                // Arena Tree.
 {const struct ProtoTypes_ArenaTree *proto;                                             // Methods associated with an arena tree
  struct ArenaTreeArena *arena;                                                         // The arena for the arena tree
 } ArenaTree;

typedef struct ArenaTreeDelta                                                           // The amount an item is offset within a tree.
 {unsigned int delta;
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
#define ArenaTreefer(child, parent) for(ArenaTreeNode child = parent.proto->last(parent);  child.proto->valid(child); child = child.proto->prev(child)) // Each child in a parent from last to first
#include <readOnlyBytes.c>
#include <stringBuffer.c>

//D1 Constructors                                                               // Construct a new Arena tree.

static ArenaTree makeArenaTree                                                                  // Create a new arena tree
 ()                                                                             // ArenaTree allocator
 {ArenaTreeArena * const a = alloc(sizeof(ArenaTreeArena));                                     // Allocate arena description
  const ArenaTree t = newArenaTree(({struct ArenaTree t = {arena: a, proto: &ProtoTypes_ArenaTree}; t;}));                                                  // Arena tree we are creating
  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  a->used   = 0;                                                                // Length used so far
  a->root   = 0;                                                                // Root not set in ArenaTree
  t.proto->node(t, "");                                                                 // Initialize root node
  return t;
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
 {const ArenaTreeArena a = *tree.arena;
  const char * const p = (char *)pointer;
  if (p > a.data && p < a.data + a.used)
   {return newArenaTreeOffset(({struct ArenaTreeOffset t = {tree: tree, offset: p - a.data, proto: &ProtoTypes_ArenaTreeOffset}; t;}));
   }
  printStackBackTrace("Arena does not contain pointer %p\n", pointer);
 }

static ArenaTreeContent * content_ArenaTreeNode                                                 //PV Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const ArenaTreeNode n)                                                                // NodeContent ArenaTreeOffset
 {return (ArenaTreeContent *)pointer_ArenaTree_size(n.tree, n.offset);
 }

static int checkKey_int_ArenaTreeNode_string_size                                       // Confirm that the key of a node is equal to the specified string
 (const ArenaTreeNode        node,                                                      // Node whose key is to be checked
  const char * const key,                                                       // Expected key value
  const size_t       length)                                                    // Length of expected key value or zero to request strlen
 {const char * const k = node.proto->key(node);
  return !strncmp(k, key, length ? : strlen(key));
 }

static int equals_int_ArenaTreeNode_ArenaTreeNode                                               // Confirm two offsets are equal
 (const ArenaTreeNode a,                                                                // First offset
  const ArenaTreeNode b)                                                                // Second offset
 {return a.tree.arena == b.tree.arena && a.offset == b.offset;
 }

static ArenaTreeNode root_ArenaTreeNodeOffset_ArenaTree                                                 // Return the root node of a tree.
 (const ArenaTree tree)                                                                 // Tree
 {return newArenaTreeNode(({struct ArenaTreeNode t = {tree: tree, proto: &ProtoTypes_ArenaTreeNode}; t;}));                                                 // Tree containing root node
 }

static ArenaTreeNode root_ArenaTreeNodeOffset_ArenaTreeNodeOffset                                       // Return the root node of the tree containing the specified node.
 (const ArenaTreeNode node)                                                             // NodeContent
 {const ArenaTree t = node.tree;
  return  t.proto->root(t);
 }

static ArenaTreeString key_string_ArenaTreeNode                                                 //V Get a temporary pointer to the offset containing the key of a node.
 (const ArenaTreeNode node)                                                             // NodeContent
 {const ArenaTree t = node.tree;
  const ArenaTreeOffset k = t.proto->offset(t, (node.proto->content(node))->key.delta);
  return  k.proto->pointer(k);
 }

//D1 Allocation                                                                 // Allocating memory in the tree

static void setKey_ArenaTreeNode_ArenaTreeString                                                // Copy a string into the key field of a node
 (const ArenaTreeNode        node,                                                      // Node
  const char * const key)                                                       // Key - it will be copied into the tree
 {      char * const k = node.proto->key(node);                                            // Existing key                                                                               //
  const size_t       l = strlen(k), m = strlen(key);
  if (k && m <= l)                                                              // There is enough room in the existing key for the new key
   {strcpy(k, key);
    return;
   }
  ArenaTreeContent   * const c = node.proto->content(node);                                        // Node content
  c->key.delta         = node.tree.proto->saveString(node.tree, key, m).offset;                 // Allocate new (longer) key
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
 {const ArenaTreeNode  n = {tree, delta, &ProtoTypes_ArenaTreeNode};                            // Create node
  return       n;
 }

static ArenaTreeOffset allocate_offset_ArenaTree_size                                           //P Allocate memory within the arena of a tree and clear the allocated memory
 (const ArenaTree      tree,                                                            // ArenaTree in which to allocate
  const size_t size)                                                            // Amount of memory required
 {if (tree.arena->used + size < tree.arena->size)                               // Allocate within existing arena
   {const size_t delta = tree.arena->used;                                      // Current delta to open memory
    const ArenaTreeOffset O = tree.proto->offset(tree, delta);                                     // Current offset to open memory
    const ArenaTreeString o = O.proto->pointer(O);                                              // Address of memory to be allocated
    memset(o, 0, size);                                                         // Clear new memory
    tree.arena->used += size;                                                   // Allocate
    return O;                                                                   // Return allocation
   }
  else                                                                          // Reallocate arena
   {const size_t S = nextPowerOfTwo(tree.arena->size + size);                   // Round up memory required
    if (S < ((size_t)((1l<<(sizeof(ArenaTreeDelta)*8)) - 1)))                           // The maximum size of a delta if there are 8 bits in a byte
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

static ArenaTreeNode noden_ArenaTreeNode_ArenaTree_ArenaTreeString                                              // Create a new tree node keyed by a string of the specified length to which a terminating zero will be appended.
 (const ArenaTree       tree,                                                           // Arena tree in which to create the node
  const char * const key,                                                       // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the tree is determined solely by the user.
  const size_t  length)                                                         // Length of the key, or if zero, I will use strlen
 {const ArenaTreeOffset c = tree.proto->allocate(tree, sizeof(ArenaTreeContent));                          // Space required for a tree node
  const ArenaTreeOffset k = tree.proto->saveString(tree, key, length);                             // Save key string in tree arena
  ArenaTreeContent * const a = c.proto->pointer(c);
  a->key.delta = k.offset;                                                      // Save key offset
  return tree.proto->nodeFromOffset(tree, c.offset);                                       // Return node
 }

static ArenaTreeNode node_ArenaTreeNode_ArenaTree_ArenaTreeString                                               // Create a new tree node keyed by a zero terminated string.
 (const ArenaTree             tree,                                                     // Arena tree in which to create the node
  const char * const  key)                                                      // Key for this node.  Note: we do not order nodes automatically.
 {return tree.proto->noden(tree, key, 0);
 }

static  ArenaTreeNode nodeFromReadOnlyBytes_ArenaTreeNode_ArenaTree_ReadOnlyBytes                       // Create a new tree node from a ReadOnlyBytes String
 (const ArenaTree             tree,                                                     // Arena tree in which to create the node
  const ReadOnlyBytes string)                                                   // Key for this node as a read only bytes string.
 {return tree.proto->noden(tree, string.data, string.length);
 }

static  ArenaTreeNode nodeFromStringBuffer_ArenaTree_ArenaTreeNode_ArenaTree_StringBuffer                       // Create a new tree node from a String Buffer
 (const ArenaTree             tree,                                                     // Arena tree in which to create the node
  const StringBuffer  string)                                                   // Key for this node as a string buffer
 {const ReadOnlyBytes r = string.proto->readOnlyBytes(string);                               // Read only bytes from string buffer
  const ArenaTreeNode n = tree.proto->nodeFromReadOnlyBytes(tree, r);
  r.proto->free(r);
  return n;
 }

static ArenaTreeOffset saveString_ArenaTreeOffset_ArenaTree_ArenaTreeString                                     //P Save a copy of a zero terminated string in a tree and return the offset of the string.
 (const ArenaTree       tree,                                                           // Arena tree in which to create the node
  const char * const str,                                                       // String
  const size_t  length)                                                         // String, or if zero I will call strlen
 {const size_t l = length ? : strlen(str);                                      // Length of string
  const ArenaTreeOffset o = tree.proto->allocate(tree, l + 1);                                     // Allocate space for the string plus a terminating zero
  char * const t = o.proto->pointer(o);
  char * const T = stpncpy(t, str, l); *T = 0;
  return o;
 }

static void get_ArenaTree_data_size                                                     // Get a copy of the data at the specified offset in the arena of a tree.
 (const ArenaTree      tree,                                                            // Arena tree
  const size_t offset,                                                          // Offset to data in arena
  void * const data,                                                            // Pointer to the area into which the data is to be copied
  const size_t length)                                                          // Length of the data to be retrieved
 {const void * const s = tree.proto->pointer(tree, offset);                                // Locate data
  memcpy(data, s, length);                                                      // Copy out data
 }

static ArenaTreeOffset store_offset_ArenaTree_data_size                                         // Store a copy of the specified data in the arena of the tree and return the offset of the data.
 (const ArenaTree       tree,                                                           // Tree in whose arena the data will be stored
  const void *  const data,                                                     // Pointer to the data to be stored
  const size_t  length)                                                         // Length of the data to be stored
 {const ArenaTreeOffset o = tree.proto->allocate(tree, length);                                    // Allocate space for data
  void * const  t = o.proto->pointer(o);                                                // Address allocation
  memcpy(t, data, length);                                                      // Copy in data
  return o;
 }

static void retrieve_ArenaTree_ArenaTreeOffset_data_size                                        // Retrieve a copy of the data stored at the specified offset in the arena of the specified tree.
 (const ArenaTree       tree,                                                           // Tree in whose arena the data is stored.
  const ArenaTreeOffset offset,                                                         // Offset in the arena at which the data to be retrieved is stored.
  void * const  data,                                                           // Area into which the retrieved data is to be copied.
  const size_t  length)                                                         // Length of the data to be retrieved
 {const void * const  s = tree.proto->pointer(tree, offset.offset);                        // Address data
  memcpy(data, s, length);                                                      // Copy out data
 }

static void get_ArenaTreeNode_data_size                                                 // Get a copy of the data addressed by a node.
 (const ArenaTreeNode  node,                                                            // Node in an arena tree associated with the data
  void * const data,                                                            // Pointer to the area into which the data is to be copied
  const size_t length)                                                          // Length of the data to be retrieved
 {const ArenaTree tree  = node.tree;                                                    // Tree containing node
  tree.proto->get(tree, node.proto->content(node)->data.delta, data, length);                         // Locate data
 }

static void set_ArenaTreeNode_data_size                                                 // Save a copy of the specified data in the arena of the tree and return the offset of the data.
 (const ArenaTreeNode  node,                                                            // Node in an arena tree to associate with the data
  const void * const data,                                                      // Pointer to the data to be saved
  const size_t length)                                                          // Length of the data to be saved
 {const ArenaTree    tree = node.tree;                                                  // Tree containing node
  node.proto->content(node)->data.delta = tree.proto->store(tree, data, length).offset;               // Record offset of saved data
 }

static void copyData_ArenaTreeNode_ArenaTreeNode                                                // Copy the data associated with the sourtce node to the target node by copying the offset to the data held in the source node to the target node.
 (const ArenaTreeNode target,                                                           // Target node
  const ArenaTreeNode source)                                                           // Source node
 {      ArenaTreeContent * const t = target.proto->content(target);                                  // Target content
  const ArenaTreeContent * const s = source.proto->content(source);                                  // Source content
  t->data = s->data;                                                            // Copy data offset
 }

static size_t getData_size_ArenaTreeNode                                                // Get the value of the data offset associated with a node.
 (const ArenaTreeNode  node)                                                            // Node in an arena tree associated with the data
 {return node.proto->content(node)->data.delta;                                            // Value of the data offset field
 }

static void setData_ArenaTreeNode_size                                                  // Set the value of the data offset associated with a node.
 (const ArenaTreeNode  node,                                                            // Node in an arena tree to associate with the data
  const size_t offset)                                                          // Value the data offset is to be set to
 {node.proto->content(node)->data.delta = offset;                                          // Record offset
 }

static void fromLetters_ArenaTree_ArenaTreeString                                               // Load tree from a string of letters and brackets.  The root node of the tree so constructed is always given the letter 'a'.
 (const ArenaTree tree,                                                                 // Tree
  ArenaTreeString str)                                                                  // String of letters and brackets describing desired tree structure
 {const ArenaTreeNode n = tree.proto->root(tree);                                                  // The node we are currently  adding to
  n.proto->setKey(n, "a");                                                              // The root is always called "a"

  void parse(ArenaTreeNode current)                                                     // Parse the string of letters and brackets to construct the desired tree
   {while(*str)
     {const char c = *str;
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
 {ArenaTreeArena *a = tree.arena;
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
 {if (child.proto->isRoot(child)) printStackBackTrace("The root node of a tree has no parent\n");
  return child.tree.proto->nodeFromOffset(child.tree, child.proto->content(child)->parent.delta);
 }

static  ArenaTreeNode first_ArenaTreeNode_ArenaTreeNode                                                 // Get the first child under a parent.
 (const ArenaTreeNode parent)                                                           // Parent
 {return  parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->first.delta);
 }
static  ArenaTreeNode last_ArenaTreeNode_ArenaTreeNode                                                 // Get the last child under a parent.
 (const ArenaTreeNode parent)                                                           // Parent
 {return  parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->last.delta);
 }
#line 375 "/home/phil/c/z/arenaTree/arenaTree.c"
static  ArenaTreeNode next_ArenaTreeNode_ArenaTreeNode                                                 // Get the next child under a parent.
 (const ArenaTreeNode parent)                                                           // Parent
 {return  parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->next.delta);
 }
#line 375 "/home/phil/c/z/arenaTree/arenaTree.c"
static  ArenaTreeNode prev_ArenaTreeNode_ArenaTreeNode                                                 // Get the prev child under a parent.
 (const ArenaTreeNode parent)                                                           // Parent
 {return  parent.tree.proto->nodeFromOffset(parent.tree, parent.proto->content(parent)->prev.delta);
 }
#line 375 "/home/phil/c/z/arenaTree/arenaTree.c"

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
#line 382 "/home/phil/c/z/arenaTree/arenaTree.c"
static  ArenaTreeNode next_ArenaTreeNode_ArenaTree                                                     // Get the next child in the specified ArenaTree.
 (const ArenaTree tree)                                                                 // Parent
 {const ArenaTreeNode root = tree.proto->root(tree);
  return root.proto->next(root);
 }
#line 382 "/home/phil/c/z/arenaTree/arenaTree.c"
static  ArenaTreeNode prev_ArenaTreeNode_ArenaTree                                                     // Get the prev child in the specified ArenaTree.
 (const ArenaTree tree)                                                                 // Parent
 {const ArenaTreeNode root = tree.proto->root(tree);
  return root.proto->prev(root);
 }
#line 382 "/home/phil/c/z/arenaTree/arenaTree.c"

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
  ArenaTreeNode F;                                                                      // An invalid node

  void find(ArenaTreeNode node)                                                         // Check whether the key of the current node matches the specified key
   {if (node.proto->equalsString(node, key))                                               // Found
     {F = node;
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) node.proto->by(node, find);                                          // Search the tree

  return F;                                                                     // Return an invalid node
 }

static  ArenaTreeNode findFirst_ArenaTree_string                                                // Find the first node with the specified key in a post-order traversal of the tree.
 (const ArenaTree            tree,                                                      // Tree
  const char * const key)                                                       // Key to find
 {const ArenaTreeNode r = tree.proto->root(tree);                                                  // Root node of the tree
  return r.proto->findFirst(r, key);                                                    // Search the tree
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
  const ArenaTreeNode p = *parent = child.proto->parent(child);                                     // Get parent if it exists
  return(p.proto->valid(p) && strcmp(p.proto->key(p), key) == 0);                               // Check key
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
#line 449 "/home/phil/c/z/arenaTree/arenaTree.c"

static int isEmpty_ArenaTreeNode                                                        // Confirm a node has no children.
 (const ArenaTreeNode node)                                                             // Node
 {const ArenaTreeNode child = node.proto->first(node);
  return !child.proto->valid(child);                                                        // No first child so the node is empty
 }

static int isOnlyChild_ArenaTreeNode                                                    // Confirm that this child is the only child of its parent
 (const ArenaTreeNode child)                                                            // Child
 {const ArenaTreeNode parent = child.proto->parent(child);                                          // Parent
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
 {const ArenaTree tree = parent.tree;
  if (child.tree.arena != tree.arena) printStackBackTrace("Wrong tree\n");      // Parent and child must be in same tree
  const size_t     P = parent.offset,                     C = child.offset;     // Delta of parent and child
  ArenaTreeContent * const p = parent.proto->content(parent); ArenaTreeContent * const c = child.proto->content(child); // Pointers to parent and child
  const size_t     L = first ? p->first.delta : p->last.delta;                  // Delta of child currently first/last
  if (!L)                                                                       // No children yet
   {if (first) p->last.delta = C; else p->first.delta = C;                      // Delta to last/first child
   }
  else                                                                          // Existing children
   {ArenaTreeContent * const l = tree.proto->pointer(tree, L);                                     // Link first/last two children together
    if (first) l->prev.delta = C; else l->next.delta = C;
    if (first) c->next.delta = L; else c->prev.delta = L;
   }
  c->parent.delta = P;                                                          // Parent new first/last child
  if (first)  p->first.delta = C; else p->last.delta = C;                       // Parent points to new first/last child
  return child;
 }

static  ArenaTreeNode putTreeFirst_ArenaTreeNode_ArenaTreeNode                                          // Put a child first in the tree containing the arena in which the child was allocated.
 (const ArenaTreeNode child)                                                            // Child
 {const ArenaTree     t = child.tree;                                                   // Tree containing arena containing child
  const ArenaTreeNode r = t.proto->root(t);
  return r.proto->putFirst(r, child);                                                   // Put the child first
 }
static  ArenaTreeNode putTreeLast_ArenaTreeNode_ArenaTreeNode                                          // Put a child last in the tree containing the arena in which the child was allocated.
 (const ArenaTreeNode child)                                                            // Child
 {const ArenaTree     t = child.tree;                                                   // Tree containing arena containing child
  const ArenaTreeNode r = t.proto->root(t);
  return r.proto->putLast(r, child);                                                   // Put the child last
 }
#line 498 "/home/phil/c/z/arenaTree/arenaTree.c"

static  ArenaTreeNode putFirst_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                        // Put a child first under its parent
 (const ArenaTreeNode parent,                                                           // Parent
  const ArenaTreeNode child)                                                            // Child
 {return         putFL_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode(1, parent, child);                     // Put a child first under its parent
 }
static  ArenaTreeNode putLast_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                        // Put a child last under its parent
 (const ArenaTreeNode parent,                                                           // Parent
  const ArenaTreeNode child)                                                            // Child
 {return         putFL_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode(0, parent, child);                     // Put a child last under its parent
 }
#line 505 "/home/phil/c/z/arenaTree/arenaTree.c"

static  ArenaTreeNode putNP_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                           //P Put a child next or previous to the specified sibling
 (const int   next,                                                             // Put next if true, else previous
  const ArenaTreeNode sibling,                                                          // Sibling
  const ArenaTreeNode child)                                                            // Child
 {const ArenaTree     tree   = sibling.tree;                                            // Tree
  const ArenaTreeNode parent = sibling.proto->parent(sibling);                                        // Parent node
  const ArenaTreeNode twin   = next ? sibling.proto->next(sibling) : sibling.proto->prev(sibling);                  // Next or prev sibling to sibling is such a node exists
  assert(child.tree.arena == tree.arena);                                       // Parent and child must be in same tree
  ArenaTreeContent * const s = sibling.proto->content(sibling);                                       // Pointer to sibling
  ArenaTreeContent * const c = child.proto->content(child);                                       // Pointer to child

  if (!twin.proto->valid(twin))                                                            // Sibling is last/first under parent
   {if (next) parent.proto->putLast(parent, child); else parent.proto->putFirst(parent, child);           // Place child
   }
  else                                                                          // Not last/first
   {ArenaTreeContent * const w = twin.proto->content(twin);                                        // Pointer to twin of sibling
    if (next)
     {s->next.delta = w->prev.delta = child.offset;
      c->prev.delta = sibling.offset; c->next.delta = twin.offset;
     }
    else
     {s->prev.delta = w->next.delta = child.offset;
      c->next.delta = sibling.offset; c->prev.delta = twin.offset;
     }
   }
  c->parent.delta = parent.offset;                                              // Parent new next/prev child
  return child;
 }

static void setUp_ArenaTreeNode_ArenaTreeNode                                                   //P Make the specified parent node the parent of the specified child node
 (const ArenaTreeNode child,                                                            // Child
  const ArenaTreeNode parent)                                                           // Child
 {child.proto->content(child)->parent.delta = parent.offset;                                // Set parent of child
 }

static  ArenaTreeNode putNext_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                         // Put a child next after the specified sibling
 (const ArenaTreeNode sibling,                                                          // Sibling
  const ArenaTreeNode child)                                                            // Child
 {return        putNP_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode(1, sibling, child);                     // Put child next
 }
static  ArenaTreeNode putPrev_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                         // Put a child previous after the specified sibling
 (const ArenaTreeNode sibling,                                                          // Sibling
  const ArenaTreeNode child)                                                            // Child
 {return        putNP_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode(0, sibling, child);                     // Put child previous
 }
#line 547 "/home/phil/c/z/arenaTree/arenaTree.c"

//D1 Traverse                                                                   // Traverse a tree.

static void by_ArenaTreeNode_sub                                                        // Traverse the tree rooted at the specified node in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (ArenaTreeNode node,                                                                   // Node
  void (* const function) (const ArenaTreeNode node))                                   // Function
 {void children(const ArenaTreeNode parent)                                             // Process the children of the specified parent
   {const size_t N = parent.proto->countChildren(parent);                                    // Number of children
    ArenaTreeNode c[N+1];                                                               // Array of children terminated by a trailing zero
    size_t n = 0; ArenaTreefe(child, parent) c[n++] = child;                            // Load each child into an array
    for(size_t i = 0; i < N; ++i) children(c[i]);                               // Process each child allowing it to change position
    function(parent);                                                           // Process the parent
   }
  children(node);                                                               // Start at the specified root node
 }

static void by_ArenaTree_sub                                                            // Traverse a tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const ArenaTree tree,                                                                 // Tree
  void (* const function) (const ArenaTreeNode node))                                   // Function
 {const ArenaTreeNode n = tree.proto->root(tree);                                                  // Start at the root
  n.proto->by(n, function);
 }

static  size_t countChildren_size_ArenaTree                                             // Count the number of children directly under a parent.
 (const ArenaTree tree)                                                                 // ArenaTree
 {size_t l = 0;
  const ArenaTreeNode root = tree.proto->root(tree);
  if (!root.proto->valid(root)) return 0;
  ArenaTreefe(child, root) ++l;
  return l;                                                                     // Return count
 }

static  size_t countChildren_size_ArenaTreeNode                                         // Count the number of children directly under a node.
 (const ArenaTreeNode parent)                                                           // Parent
 {size_t l = 0;
  ArenaTreefe(child, parent) ++l;
  return l;                                                                     // Return count
 }

static  size_t count_size_ArenaTreeNode                                                 // Count the number of nodes under a specified node.
 (const ArenaTreeNode node)                                                             // Node
 {size_t l = 0;

  void count(const ArenaTreeNode parent)                                                // Process the children of the specified parent
   {l++; ArenaTreefe(child, parent) count(child);                                       // Each child
   }

  count(node);                                                                  // Start at the specified node
  return l - 1;                                                                 // Return count without counting the root node
 }


static  size_t count_size_ArenaTree                                                     // Count the number of nodes in a tree
 (const ArenaTree tree)                                                                 // Tree
 {const ArenaTreeNode root = tree.proto->root(tree);
  return root.proto->count(root);
 }

//D1 Print                                                                      // Print Arena Trees in various ways

static  ArenaTreeString printWithBrackets_string_ArenaTreeNode                                  // Print a node and the tree below it in preorder as a string with each sub tree enclosed in brackets.
 (const ArenaTreeNode   node)                                                           // Node
 {size_t  l = 0;                                                                // Length of output string
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

  len(node);
  const ArenaTreeString s = p = alloc(l+1); *s = 0;
  print(node);
  return s;
 }

static  ArenaTreeString printWithBrackets_ArenaTreeString_ArenaTree                                     // Print an entire tree in preorder as a string with brackets around each sub tree
 (const ArenaTree t)                                                                    // Tree
 {const ArenaTreeNode node = t.proto->root(t);                                                  // Root
  return node.proto->printWithBrackets(node);
 }

static int printsWithBracketsAs_int_ArenaTreeNode_string                                // Check that the ArenaTree starting at the specified node prints with brackets as expected.
 (const ArenaTreeNode        node,                                                      // Node
  const char * const expected)                                                  // Expected string when printed
 {char * const s = node.proto->printWithBrackets(node);
  const int r = !strcmp(s, expected);
  free(s);
  return r;
 }

static int printsWithBracketsAs_int_ArenaTree_string                                    // Check that the specified ArenaTree prints with brackets as expected.
 (const ArenaTree            tree,                                                      // ArenaTree
  const char * const expected)                                                  // Expected string when printed
 {const ArenaTreeNode root = tree.proto->root(tree);                                               // Root
  return root.proto->printsWithBracketsAs(root, expected);
 }

static ReadOnlyBytes print_string_ArenaTreeNode                                         // Print a node and the ArenaTree below it in preorder as a string.
 (const ArenaTreeNode   node)                                                           // Node
 {size_t  l = 0;                                                                // Length of output string
  void len(const ArenaTreeNode parent)                                                  // Find the size of buffer we will need
   {l += strlen(parent.proto->key(parent));                                                  // Parent key
    ArenaTreefe(child, parent) len(child);                                              // Each child
   }

  ArenaTreeString p;
  void print(const ArenaTreeNode parent)                                                // Print the children of the specified parent
   {const ArenaTreeString k = parent.proto->key(parent);
    p = stpcpy(p, k);
    ArenaTreefe(child, parent) print(child);                                            // Each child
    *p = 0;                                                                     // End the string so far
   }

  len(node);
  const ArenaTreeString s = p = alloc(l+1); *s = 0;
  print(node);
  return newReadOnlyBytes(({struct ReadOnlyBytes t = {data:s, length: l, ReadOnlyBytesAllocator_malloc, proto: &ProtoTypes_ReadOnlyBytes}; t;}));
 }

static ReadOnlyBytes print_string_ArenaTree                                             // Print an entire tree in preorder as a string.
 (const ArenaTree t)                                                                    // ArenaTree
 {const ArenaTreeNode node = t.proto->root(t);                                                  // Root
  return node.proto->print(node);
 }

static int printsAs_int_ArenaTreeNode_string                                            // Check that the ArenaTree starting at the specified node prints as expected.
 (const ArenaTreeNode        node,                                                      // Node
  const char * const expected)                                                  // Expected string when printed
 {const ReadOnlyBytes s = node.proto->print(node);
  const int r = s.proto->equalsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printsAs_int_ArenaTree_string                                                // Check that the specified ArenaTree prints as expected.
 (const ArenaTree            tree,                                                      // ArenaTree
  const char * const expected)                                                  // Expected string when printed
 {const ArenaTreeNode root = tree.proto->root(tree);                                               // Root
  return root.proto->printsAs(root, expected);
 }

static int printContains_ArenaTreeNode                                                  // Check the print of an ArenaTree starting at the specified tag contains the expected string.
 (const ArenaTreeNode   node,                                                           // Starting node
  const char *  expected)                                                       // Expected string
 {ReadOnlyBytes s = node.proto->print(node);
  const int     r = s.proto->containsString(s, expected);
  s.proto->free(s);
  return r;
 }

static int printContains_ArenaTree                                                      // Check the print of an ArenaTree contains the expected string.
 (const ArenaTree       tree,                                                            // ArenaTree parse tree
  const char *  expected)                                                       // Expected string
 {ReadOnlyBytes s = tree.proto->print(tree);
  const int     r = s.proto->containsString(s, expected);
  s.proto->free(s);
  return r;
 }

//D1 Edit                                                                       // Edit a tree in situ.

static  ArenaTreeNode cut_ArenaTreeNode_ArenaTreeNode                                                   // Cut out a child.
 (const ArenaTreeNode child)                                                            // Child to cut out
 {const ArenaTreeNode parent = child.proto->parent(child);                                         // Parent
  ArenaTreeContent * const p = parent.proto->content(parent), * const c = child.proto->content(child);           // Parent pointer

  if (child.proto->isOnlyChild(child))                                                      // Only child
   {p->first.delta = p->last.delta = 0;                                         // Remove child
   }
  else if (child.proto->isLast(child))                                                      // Last child
   {const ArenaTreeNode L = child.proto->prev(child);
    ArenaTreeContent * const l = L.proto->content(L);
    p->last.delta =  L.offset; l->next.delta = 0;                               // Last child under parent
   }
  else if (child.proto->isFirst(child))                                                     // First child
   {const ArenaTreeNode F = child.proto->next(child);
    ArenaTreeContent * const f =  F.proto->content(F);
    p->first.delta = F.offset; f->prev.delta = 0;                               // Last child under parent
   }
  else                                                                          // Some where in the middle
   {const ArenaTreeNode N = child.proto->next(child),  P = child.proto->prev(child);
    ArenaTreeContent * const n =  N.proto->content(N), * const p =  P.proto->content(P);
    p->next.delta = N.offset; n->prev.delta = P.offset;
   }

  c->next.delta = c->prev.delta = 0;                                            // Remove child

  return child;
 }

static  ArenaTreeNode unwrap_ArenaTreeNode_ArenaTreeNode                                                // Unwrap the specified parent and return it.
 (const ArenaTreeNode parent)                                                           // Parent to unwrap
 {for  (ArenaTreeNode child = parent.proto->last(parent); child.proto->valid(child); child = parent.proto->last(parent))     // Each child in a parent from first to last
   {parent.proto->putNext(parent, child.proto->cut(child));                                              // Place each child after the parent
   }
  return parent.proto->cut(parent);                                                          // Remove and return empty parent
 }

static  ArenaTreeNode wrap_ArenaTreeNode_ArenaTreeString                                                // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
 (const ArenaTreeNode   child,                                                          // Child to wrap
  const char * const key)                                                       // Key for new parent
 {const ArenaTree       tree = child.tree;                                              // Tree
  const ArenaTreeNode   parent = tree.proto->node(tree, key);                                      // New parent
  child.proto->putNext(child, parent);                                                     // Place parent after child
  parent.proto->putLast(parent, child.proto->cut(child));                                                // Place child under parent
  return parent;
 }

//D1 Input and Output                                                           // Read and write a tree from/to a file

static void write_void_ArenaTree_ArenaTreeString                                                // Write a tree to a named file or abort
 (const ArenaTree       tree,                                                           // Tree
  const char * const file)                                                      // File
 {const int o = open(file, O_CREAT| O_WRONLY, S_IRWXU);                         // Open for output creating if needed
  if (o < 0) printStackBackTrace("Cannot open file: %s for write\n", file);

  time_t current; time(&current);
  const ArenaTreeDescription h = {1, 1, 0, tree.proto->used(tree), tree.proto->root(tree).offset, current};   // Create arena tree header describing the tree

  if (sizeof(h) != write(o, &h, sizeof(h)))                                     // Write header
   {printStackBackTrace("Cannot write ArenaTree header to file: %s\n", file);
   }

  const ssize_t w = write(o, tree.arena->data, tree.arena->used);               // Write arena
  if (w < 0 || tree.arena->used != (size_t)w)
   {printStackBackTrace("Cannot write ArenaTree arena to file: %s\n", file);
   }

  close(o);
 }

ArenaTree readArenaTree                                                                         // Read a tree from a file
 (const char * const file)                                                      // File
 {ArenaTreeArena * const arena = alloc(sizeof(ArenaTreeArena));                                 // Create arena
  ArenaTree tree = newArenaTree(({struct ArenaTree t = {arena: arena, proto: &ProtoTypes_ArenaTree}; t;}));                                                 // Initialize tree

  const int i = open(file, 0, O_RDONLY);                                        // Open for input
  if (i < 0) printStackBackTrace("Cannot open file: %s for read\n", file);

  struct ArenaTreeDescription h;
  if (sizeof(h) != read(i, &h, sizeof(h)))                                      // Read header
   {printStackBackTrace("Cannot read header from file: %s\n", file);
   }

  tree.arena->data = alloc(arena->size = arena->used = h.used);                 // Allocate arena

  const ssize_t r = read(i, arena->data, arena->used);                          // Read arena
  if (r < 0 || arena->used != (size_t)r)
   {printStackBackTrace("Cannot read ArenaTree from file: %s\n", file);
   }

  close(i);
  arena->root = h.root;                                                         // Offset to root in arena
  return tree;
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TmakeArenaTree //Tnode //Tfree //TputFirst //TputLast //Tfe //Tfer
 {ArenaTree        t = makeArenaTree();                                                         // Create a tree
  ArenaTreeNode root = t.proto->root(t);

  for(int i = 0; i < 10; ++i)                                                   // Load tree
   {char s[2] = {'a'+i, 0};
    ArenaTreeNode         child = t.proto->node(t, s); root.proto->putLast(root, child);
                  child = t.proto->node(t, s); root.proto->putFirst(root, child);
    s[0] = 'A'+i; child.proto->setKey(child, s);
    assert(strcmp(child.proto->key(child), s) == 0);
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
 {ArenaTree t = makeArenaTree();    t.proto->fromLetters(t, "b(c(de)f)g(hi)j");
  assert(t.proto->printsWithBracketsAs(t, "a(b(c(de)f)g(hi)j)"));
  assert(t.proto->printsAs(t, "abcdefghij"));

  ArenaTreeNode a = t.proto->root(t),
        b = a.proto->first(a),
        c = b.proto->first(b),
        e = c.proto->last(c),
        d = e.proto->prev(e);

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
 {ArenaTree t = makeArenaTree();    t.proto->fromLetters(t, "b(c(de)f)g(hi)j");
  assert(t.proto->printsWithBracketsAs(t, "a(b(c(de)f)g(hi)j)"));
  assert(t.proto->printContains(t, "def"));

  char l[t.proto->count(t) + 1]; *l = 0;

  void process(ArenaTreeNode n) {strcat(l, n.proto->key(n));}
  t.proto->by(t, process);

  assert(strcmp(l, "decfbhigja") == 0);

  t.proto->free(t);
 }

void test3()                                                                    //TisFirst //TisLast //TisEmpty //TisRoot //TisOnlyChild //Tcontext //TcheckKey //Tvalid
 {ArenaTree t = makeArenaTree();    t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
  assert(t.proto->printsWithBracketsAs(t, "a(b(c(de(f)gh)i)j)"));

  ArenaTreeNode a = t.proto->root(t);  assert(a.proto->checkKey(a, "a", 0));                                 assert(a.proto->printsWithBracketsAs(a,  "a(b(c(de(f)gh)i)j)"));
  ArenaTreeNode b = a.proto->first(a); assert(b.proto->checkKey(b, "b", 0)); assert(a.proto->equals(a, b.proto->parent(b))); assert(b.proto->printsWithBracketsAs(b,    "b(c(de(f)gh)i)"));
  ArenaTreeNode c = b.proto->first(b); assert(c.proto->checkKey(c, "c", 0)); assert(b.proto->equals(b, c.proto->parent(c))); assert(c.proto->printsWithBracketsAs(c,      "c(de(f)gh)"));
  ArenaTreeNode d = c.proto->first(c); assert(d.proto->checkKey(d, "d", 0)); assert(c.proto->equals(c, d.proto->parent(d))); assert(d.proto->printsWithBracketsAs(d,        "d"));
  ArenaTreeNode e = d.proto->next(d);  assert(e.proto->checkKey(e, "e", 0)); assert(c.proto->equals(c, e.proto->parent(e))); assert(e.proto->printsWithBracketsAs(e,         "e(f)"));
  ArenaTreeNode f = e.proto->last(e);  assert(f.proto->checkKey(f, "f", 0)); assert(e.proto->equals(e, f.proto->parent(f))); assert(f.proto->printsWithBracketsAs(f,           "f"));
  ArenaTreeNode g = e.proto->next(e);  assert(g.proto->checkKey(g, "g", 0)); assert(c.proto->equals(c, g.proto->parent(g))); assert(g.proto->printsWithBracketsAs(g,             "g"));
  ArenaTreeNode h = g.proto->next(g);  assert(h.proto->checkKey(h, "h", 0)); assert(c.proto->equals(c, h.proto->parent(h))); assert(h.proto->printsWithBracketsAs(h,              "h"));
  ArenaTreeNode i = c.proto->next(c);  assert(i.proto->checkKey(i, "i", 0)); assert(b.proto->equals(b, i.proto->parent(i))); assert(i.proto->printsWithBracketsAs(i,                "i"));
  ArenaTreeNode j = b.proto->next(b);  assert(j.proto->checkKey(j, "j", 0)); assert(a.proto->equals(a, j.proto->parent(j))); assert(j.proto->printsWithBracketsAs(j,                  "j"));

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
 {ArenaTree t = makeArenaTree();    t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
  assert(t.proto->printsWithBracketsAs(t, "a(b(c(de(f)gh)i)j)"));

  ArenaTreeNode c = t.proto->findFirst(t, "c");
  ArenaTreeNode d = t.proto->findFirst(t, "d");
  assert(d.proto->equalsString(d, "d"));
  ArenaTreeNode e = t.proto->findFirst(t, "e"); assert(e.proto->equalsString(e, "e"));
  ArenaTreeNode f = t.proto->findFirst(t, "f"); assert(f.proto->equalsString(f, "f"));
  ArenaTreeNode g = t.proto->findFirst(t, "g"); assert(g.proto->equalsString(g, "g"));
  ArenaTreeNode h = t.proto->findFirst(t, "h"); assert(h.proto->equalsString(h, "h"));

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
 {ArenaTree t = makeArenaTree();    t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
  assert(t.proto->printsWithBracketsAs(t, "a(b(c(de(f)gh)i)j)"));

  ArenaTreeString f = "/tmp/arenaTreeTest.data";
  assert(t.proto->printsWithBracketsAs(t, "a(b(c(de(f)gh)i)j)"));
         t.proto->write(t, f);

  ArenaTree      u = readArenaTree(f);
  assert(u.proto->printsWithBracketsAs(u, "a(b(c(de(f)gh)i)j)"));

  t.proto->free(t);
  u.proto->free(u);
  unlink(f);
 }

void test6()                                                                    //Tunwrap //Twrap //Tcount
 {ArenaTree t = makeArenaTree();    t.proto->fromLetters(t, "bce");
  assert(t.proto->printsWithBracketsAs(t, "a(bce)"));
  assert(t.proto->count(t) == 3);

  ArenaTreeNode c = t.proto->findFirst(t, "c");
  assert(c.proto->valid(c));

  ArenaTreeNode d = c.proto->wrap(c, "d");
  assert(t.proto->printsWithBracketsAs(t, "a(bd(c)e)"));
  assert(t.proto->count(t) == 4);

  d.proto->unwrap(d);
  assert(t.proto->printsWithBracketsAs(t, "a(bce)"));
  assert(t.proto->count(t) == 3);
         t.proto->free(t);
 }

void test7()                                                                    //TsetKey //Tkey //TsaveString //Tused //Tsize
 {ArenaTree t = makeArenaTree(); t.proto->fromLetters(t, "");
  ArenaTreeDelta   d = {delta : 1};
  assert(sizeof(d) == 4);

  ArenaTreeOffset a = t.proto->saveString(t, "aaaa", 0);
  assert(strcmp(a.proto->pointer(a), "aaaa") == 0);

  ArenaTreeNode      n = t.proto->node(t, "a");
                 n.proto->setKey(n, "A");
  assert(strcmp (n.proto->key(n),   "A") == 0);

  t.proto->free(t);
 }

void test8()                                                                    //TputNext //TputPrev //Tnoden
 {ArenaTree t = makeArenaTree(); t.proto->fromLetters(t, "A");
  ArenaTreeNode a = t.proto->root(t);  assert((a.proto->key(a))[0] == 'a');
  ArenaTreeNode A = a.proto->first(a); assert((A.proto->key(A))[0] == 'A');

  for(int i = 0; i < 10; ++i)
   {char c[2] = {'0'+i};
    A.proto->putNext(A, t.proto->noden(t, c, 1));
    A.proto->putPrev(A, t.proto->noden(t, c, 1));
   }

  assert(t.proto->printsWithBracketsAs(t, "a(0123456789A9876543210)"));

  t.proto->free(t);
 }

void test9()                                                                    //Tget //Tset //TcopyData
 {ArenaTree t = makeArenaTree();
    t.proto->fromLetters(t, "a");
  struct S
   {int a;
   } S = {1}, T;

  ArenaTreeNode b = t.proto->node(t, "b");
        b.proto->set(b, &S, sizeof(S));
  T.a = 0;
  b.proto->get(b, &T, sizeof(T));
  assert (T.a == 1);

  ArenaTreeNode c = t.proto->node(t, "c");
  c.proto->copyData(c, b);
  T.a = 0;
  c.proto->get(c, &T, sizeof(T));
  assert (T.a == 1);

  t.proto->free(t);
 }

void test10()                                                                   //TgetData //TsetData
 {ArenaTree t = makeArenaTree();
    t.proto->fromLetters(t, "a");

  ArenaTreeNode   b = t.proto->node(t, "b");
          b.proto->setData(b, 42);
  assert (b.proto->getData(b) == 42);

  t.proto->free(t);
 }

void test11()                                                                   //Tstore //Tretrieve //TnewArenaTreeOffset
 {ArenaTree t = makeArenaTree();

  ArenaTreeOffset a = t.proto->store(t, "abcd", 4);
  char c[4];
  for(size_t i = 0; i < sizeof(c); ++i)
   {t.proto->retrieve(t, a, c, i);
    assert(!strncmp(c, "abcd", i));
   }

  if (1)                                                                        // Create an offset explicitly
   {void * p = a.proto->pointer(a);
    ArenaTreeOffset A = t.proto->offsetTo(t, p);
    assert(a.offset == A.offset && a.tree.arena->data == A.tree.arena->data);
   }

  t.proto->free(t);
 }

void test12()                                                                   //TputTreeFirst //TputTreeLast
 {ArenaTree t = makeArenaTree(); ArenaTreeNode r = t.proto->root(t); r.proto->setKey(r, "a");

  ArenaTreeNode c = t.proto->node(t, "c"); c.proto->putTreeFirst(c);
  ArenaTreeNode d = t.proto->node(t, "d"); d.proto->putTreeLast(d);
  ArenaTreeNode b = t.proto->node(t, "b"); b.proto->putTreeFirst(b);

  assert(t.proto->printsWithBracketsAs(t, "a(bcd)"));

  t.proto->free(t);
 }

void test13()                                                                   //TnodeFromReadOnlyBytes //TnodeFromStringBuffer
 {ArenaTree t = makeArenaTree();

  const ReadOnlyBytes r = makeReadOnlyBytesFromString("aaa");
  const StringBuffer  s = makeStringBuffer();
  s.proto->addReadOnlyBytes(s, r);

  const ArenaTreeNode a = t.proto->nodeFromReadOnlyBytes(t, r); a.proto->putTreeLast(a);
  const ArenaTreeNode A = t.proto->nodeFromStringBuffer(t, s); A.proto->putTreeLast(A);

  assert(t.proto->printsWithBracketsAs(t, "(aaaaaa)"));

  t.proto->free(t); s.proto->free(s); r.proto->free(r);
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0,  test1,  test2,  test3, test4,
                           test5,  test6,  test7,  test8, test9,
                           test10, test11, test12, test13, 0};
  run_tests("ArenaTree", repetitions, tests);

  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaTree/arenaTree
