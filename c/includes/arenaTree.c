//------------------------------------------------------------------------------
// Trees held in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef ArenaTree_included
#define ArenaTree_included
#include <assert.h>
#include <fcntl.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <utilities.c>

#define ArenaTreeStartString "phi"                                                      // String that starts the arena

//D1 Structures                                                                 // Structures describing the tree. ArenaTree and TreeAllocator are not held inside the arena and so are pointers not offsets.

typedef char * ArenaTreeString;                                                         // Arena Tree string

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
  void  *data[0];                                                               // Location of any data following the node in the arena.
 } ArenaTreeContent;

typedef struct ArenaTreeOffset                                                          // Offset to any item in the tree.  The caller is responsible for interpreting the content of the memory so addressed.
 {ArenaTree      tree;                                                                  // ArenaTree containing the item
  size_t offset;                                                                // Offset
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
 {size_t version;                                                               // Version of arena tree
  size_t littleEndian;                                                          // Little endian if true
  size_t s64b;                                                                  // 64 bit address if true, else 32 bit
  size_t used;                                                                  // The number of bytes currently used by the arena.
  size_t root;                                                                  // Offset to the root node
  time_t created;                                                               // Time in seconds since the epoch when this tree was created
 } ArenaTreeDescription;

#include <arenaTree_prototypes.h>                                                      // Arena tree prototypes now that the relevant structures have been declared

//D1 Constructor                                                                // Construct a new Arena tree.

ArenaTree newArenaTree                                                                          // Create a new arena tree
 (const struct ArenaTree allocator)                                                     // ArenaTree allocator
 {ArenaTree t;                                                                          // Arena tree we are creating
  ArenaTreeArena *a = t.arena = alloc(sizeof(ArenaTreeArena));                                  // Allocate Arena description
  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  strcpy(a->data,    ArenaTreeStartString);                                             // Occupy offset 0 in the rena with some junk so that we can use a delta of 0 as a null offset
  a->used   = strlen(ArenaTreeStartString) + 1;                                         // Length used so far
  a->root   = 0;                                                                // Root not set
  t.proto   = allocator.proto;                                                  // Initialize tree prototype
  t.proto->new(t, "");                                                                  // Initialize root node
  return t;
 }

//D1 Pointers and Offsets                                                       // Operations on pointers and offsets

static void * pointer_tree_offset                                               //V Return a temporary pointer to an offset in a tree.
 (const ArenaTree      tree,                                                            // Tree
  const size_t delta)                                                           // Delta
 {if (!delta) return NULL;                                                      // A zero delta is an unset delta
  if ( delta > tree.arena->used) return NULL;                                   // An delta outside the arena is an unset delta
  return (void *)(tree.arena->data + delta);                                    // Convert a non zero delta that is within the arena to a valid pointer
 }

static void * pointer_offset                                                    //V Convert a node  describing an offset into an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const ArenaTreeOffset o)                                                              // Offset
 {return pointer_tree_offset(o.tree, o.offset);                                 // Return a temporary pointer to an offset in a tree.
 }

static ArenaTreeContent * content_ArenaTreeNode                                                 //V Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const ArenaTreeNode n)                                                                // NodeContent ArenaTreeOffset
 {return (ArenaTreeContent *)pointer_tree_offset(n.tree, n.offset);
 }

static int equals_int_ArenaTreeNode_ArenaTreeNode                                               // Confirm two offsets are equal
 (const ArenaTreeNode a,                                                                // First offset
  const ArenaTreeNode b)                                                                // Second offset
 {return a.tree.arena == b.tree.arena && a.offset == b.offset;
 }

static ArenaTreeNode root_ArenaTreeNodeOffset_ArenaTree                                                 // Return the root node of a tree.
 (const ArenaTree tree)                                                                 // Tree
 {const ArenaTreeNode n = {tree  : tree,                                                // Tree containing root node
                   offset: strlen(ArenaTreeStartString) + 1,                            // Offset of root node
                   proto : &ProtoTypes_ArenaTreeNode};                                  // Prototype
  return n;
 }

static ArenaTreeNode root_ArenaTreeNodeOffset_ArenaTreeNodeOffset                                       // Return the root node of the tree containing the specified node.
 (const ArenaTreeNode node)                                                             // NodeContent
 {const ArenaTree t = node.tree;
  return  t.proto->root(t);
 }

static ArenaTreeString key_string_ArenaTreeNode                                                 //V Get a temporary pointer to the offset containing the key of a node.
 (const ArenaTreeNode node)                                                             // NodeContent
 {ArenaTree t = node.tree;
  ArenaTreeOffset k = t.proto->offset(t, (node.proto->content(node))->key.delta);
  return  k.proto->pointer(k);
 }

//D1 Allocation                                                                 // Allocating memory in the tree

static ArenaTreeNode setKey_ArenaTreeNode_ArenaTreeNode_string                                          // Copy a string into the key field of a node
 (const ArenaTreeNode   node,                                                           // NodeContent
  const ArenaTreeString key)                                                            // Key - it will be copied into the tree
 {const ArenaTree   t  = node.tree;
  ArenaTreeOffset   o  = t.proto->saveString(t, key);                                           // Offset to key
  ArenaTreeContent *c  = (ArenaTreeContent *)(node.proto->content(node));
  c->key.delta = o.offset;
  return node;
 };

static ArenaTreeOffset  offset_tree_delta                                               // Create an offset to locate an item within the tree.
 (const ArenaTree       tree,                                                           // Tree
  const size_t  delta)                                                          // Delta within arena
 {const ArenaTreeOffset o = {tree, delta, &ProtoTypes_ArenaTreeOffset};                         // Create offset locally
  return        o;
 }

static ArenaTreeNode   node_tree_delta                                                  //P Create a node to locate an allocation within the arena of a tree.
 (const ArenaTree      tree,                                                            // Tree
  const size_t delta)                                                           // Delta within arena. A delta of zero represents no such node.
 {const ArenaTreeNode  n = {tree, delta, &ProtoTypes_ArenaTreeNode};                            // Create node
  return       n;
 }

static ArenaTreeOffset allocate_offset_tree_delta                                       //P Allocate memory within the arena of a tree and clear the allocated memory
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
        return allocate_offset_tree_delta(tree, size);                          // Allocate within arena
       }
     }
   }
  printStackBackTrace("Arena too large\n");                                     // The arena has become too large for the chosen size of offsets.
 }

static ArenaTreeOffset saveString_offset_tree_string                                    // Save a copy of a zero terminated string in a tree and return the offset of the string.
 (const ArenaTree tree,                                                                 // Arena tree in which to create the node
  const ArenaTreeString str)                                                            // String
 {const ArenaTreeOffset o = tree.proto->allocate(tree, strlen(str) + 1);
  const ArenaTreeString t = o.proto->pointer(o);
  strcpy(t, str);
  return o;
 }

static ArenaTreeOffset saveNString_offset_tree_string_delta                             // Save a copy of the specified number of characters from a string as a zero terminated string in a tree and return the offset of the string. (An extra byte is allocated to hold the terminating zero).
 (const ArenaTree       tree,                                                           // Arena tree in which to create the node
  const ArenaTreeString str,                                                            // str for this node.  Note: we do not order nodes automatically.
  const size_t  length)                                                         // str for this node.  Note: we do not order nodes automatically.
 {const size_t  l = length + 1;
  const ArenaTreeOffset o = tree.proto->allocate(tree, l);
  const ArenaTreeString t = o.proto->pointer(o);
  strncpy(t, str, length); t[l] = 0;
  return o;
 }

static ArenaTreeNode new_ArenaTreeNode_tree_string                                              // Create a new tree node and return its offset. The key of the node is zero terminated.
 (const ArenaTree          tree,                                                        // Arena tree in which to create the node
  const ArenaTreeString    key)                                                         // Key for this node.  Note: we do not order nodes automatically.
 {const ArenaTreeOffset    c = tree.proto->allocate(tree, sizeof(ArenaTreeContent));                       // Space required for a tree node
  const ArenaTreeOffset    k = tree.proto->saveString(tree, key);                                  // Save key string in tree arena
  ArenaTreeContent * const n = c.proto->pointer(c);                                             // Temporary pointer to node
  n->key.delta       = k.offset;                                                // Save key offset
  return tree.proto->node(tree, c.offset);                                                 // Return node
 }

static ArenaTreeNode newNStr_ArenaTreeNode_tree_string_length                                   // Create a new tree node from a string specified by its location and length
 (const ArenaTree      tree,                                                            // Arena tree in which to create the node
  char * const str,                                                             // String location
  const size_t length)                                                          // String length
 {return tree.proto->newNStrExtra(tree, str, length, 0, 0);                                // Return node with no extension
 }

static ArenaTreeNode newNStrExtra_ArenaTreeNode_tree_string_sizet_sizet_pointer                 // Create a new tree node from a string specified by its location and length with following space for a structure of specified size loaded from the specified pointer.
 (const ArenaTree      tree,                                                            // Arena tree in which to create the node
  char * const str,                                                             // String location
  const size_t length,                                                          // String length
  const size_t extraLength,                                                     // Length of extra data
  const void  *extra)                                                           // Extra data
 {const ArenaTreeOffset N = tree.proto->allocate(tree, sizeof(ArenaTreeContent) + extraLength);            // Space required for a tree node plus extra
  const ArenaTreeOffset k = tree.proto->saveNString(tree, str, length);                            // Save key string in tree arena
  ArenaTreeContent * const n = N.proto->pointer(N);                                             // Temporary pointer to node
  n->key.delta       = k.offset;                                                // Save key offset
  if (extraLength > 0) memcpy(&n->data, extra, extraLength);                    // Copy in extra data if provided
  return tree.proto->node(tree, N.offset);                                                 // Return node
 }

static void getExtra_ArenaTreeNode_sizet_pointer                                        // Get the extra data after a node - the caller must provide the size of the data and a structure into which to copy it.
 (const ArenaTreeNode  node,                                                            // Node
  const size_t extraLength,                                                     // Length of extra data
  void        *extra)                                                           // Extra data area
 {ArenaTreeContent * const n = node.proto->content(node);                                          // Temporary pointer to node
  memcpy(extra, &n->data, extraLength);                                         // Copy out extra data
 }

static void fromLetters_tree_str                                                // Load tree from a string of letters and brackets.  The root node of the tree so constructed is always given the letter 'a'.
 (ArenaTree       tree,                                                                 // Tree
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
          current.proto->putLast(current, tree.proto->new(tree, s));
          ++str; continue;
         }
       }
     }
   }

  parse(n);                                                                     // Parse the string of letters and brackets to construct the desired tree
 };

static void free_tree                                                           // Free an entire arena tree.
 (const ArenaTree tree)                                                                 // Arena tree to free
 {if (tree.arena->data) free(tree.arena->data);
  free(tree.arena);
 }

//D1 Navigation                                                                 // Navigate through a tree.

static ArenaTreeNode parent_ArenaTreeNode_ArenaTreeNode                                                 // Get the parent of a child
 (const ArenaTreeNode  child)                                                           // Child
 {if (child.proto->isRoot(child)) printStackBackTrace("The root node of a tree has no parent\n");
  const ArenaTreeDelta p = (child.proto->content(child))->parent;
  const ArenaTree t = child.tree;
  return  t.proto->node(t, p.delta);
 }

static ArenaTreeNode first_ArenaTreeNode_ArenaTreeNode                                                  // Get the first child under a parent.
 (const ArenaTreeNode parent)                                                           // Parent
 {const ArenaTree t = parent.tree;
  return t.proto->node(t, (parent.proto->content(parent))->first.delta);
 }

static ArenaTreeNode last_ArenaTreeNode_ArenaTreeNode                                                   // Get the last child under a parent.
 (const ArenaTreeNode parent)                                                           // Parent
 {const ArenaTree t = parent.tree;
  return t.proto->node(t, (parent.proto->content(parent))->last.delta);
 }

static ArenaTreeNode next_ArenaTreeNode_ArenaTreeNode                                                   // Get the next sibling of the specified child.
 (const ArenaTreeNode child)                                                            // Child
 {const ArenaTree t = child.tree;
  return t.proto->node(t, (child.proto->content(child))->next.delta);
 }

static ArenaTreeNode prev_ArenaTreeNode_ArenaTreeNode                                                   // Get the previous sibling of the specified child.
 (const ArenaTreeNode child)                                                            // Child
 {const ArenaTree t = child.tree;
  return t.proto->node(t, (child.proto->content(child))->prev.delta);
 }

static int findFirstKey_int_tree_string_ArenaTreeNodePointer                            // Find the first node with the specified key in the tree. Returns true and sets found if a matching key is found else returns false
 (const ArenaTree        tree,                                                          // Tree
  const ArenaTreeString  key,                                                           // Key to find
  ArenaTreeNode  * const result)                                                        // Output area for node if found
 {jmp_buf found;
  ArenaTreeNode F;

  void find(ArenaTreeNode node)                                                         // Check whether the key of the current node matches the specified key
   {if (strcmp(node.proto->key(node), key)) return;                                        // Not found
    F = node;                                                                   // Found
    longjmp(found, 1);
   }

  if (!setjmp(found))                                                           // Search the tree
   {tree.proto->by(tree, find);
    return 0;                                                                   // No matching key
   }
  else
   {*result = F;
    return 1;
   }
 }

//D1 Location                                                                   // Verify the current location.

static int context_ArenaTreeNode                                                        // Return true if the parent of the specified child has the specified key.
 (ArenaTreeNode  child,                                                                 // Child
  ArenaTreeNode *parent,                                                                // Parent container
  char  *key)                                                                   // Key
 {if (child.proto->isRoot(child)) return 0;                                                 // The root node has no context
  const ArenaTreeNode p = *parent = child.proto->parent(child);                                     // Get parent if it exists
  return(p.offset && strcmp(p.proto->key(p), key) == 0);                                // Check key
 }

static int isFirst_ArenaTreeNode                                                        // Confirm a child is first under its parent
 (const ArenaTreeNode child)                                                            // Child
 {const ArenaTreeNode parent = child.proto->parent(child);
  return child.proto->equals(child, parent.proto->first(parent));
 }

static int isLast_ArenaTreeNode                                                         // Confirm a child is last under its parent
 (const ArenaTreeNode child)                                                            // Child
 {const ArenaTreeNode parent = child.proto->parent(child);
  return child.proto->equals(child, parent.proto->last(parent));
 }

static int isEmpty                                                              // Confirm a node has no children.
 (const ArenaTreeNode node)                                                             // NodeContent
 {const ArenaTreeNode child = node.proto->first(node);
  return child.offset == 0;                                                     // No first child so the node is empty
 }

static int isOnlyChild                                                          // Confirm that this child is the only child of its parent
 (const ArenaTreeNode child)                                                            // Child
 {const ArenaTreeNode parent = child.proto->parent(child);                                          // Parent
  return parent.offset != 0 && child.proto->isFirst(child) && child.proto->isLast(child);               // Child is first and last and not the root so it is an only child
 }

static int isRoot                                                               // Confirm a node is the root
 (const ArenaTreeNode node)                                                             // NodeContent
 {const ArenaTreeDelta p = (node.proto->content(node))->parent;
  return p.delta == 0;                                                          // No parent so the node is a root node
 }

//D1 Put                                                                        // Insert children into a tree.

static  ArenaTreeNode putFirstLast_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                    //P Put a child last under its parent
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

static  ArenaTreeNode putFirst_ArenaTreeNode_tree_ArenaTreeNode                                         // Put a child first in a tree
 (const ArenaTree     tree,                                                             // Tree
  const ArenaTreeNode child)                                                            // Child
 {const ArenaTreeNode r = tree.proto->root(tree);
  return r.proto->putFirst(r, child);                                                   // Put the child first
 }

static  ArenaTreeNode putLast_ArenaTreeNode_tree_ArenaTreeNode                                          // Put a child last in a tree
 (const ArenaTree     tree,                                                             // Tree
  const ArenaTreeNode child)                                                            // Child
 {const ArenaTreeNode r = tree.proto->root(tree);
  return r.proto->putLast(r, child);                                                    // Put the child last
 }

static  ArenaTreeNode putFirst_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                        // Put a child first under its parent
 (const ArenaTreeNode parent,                                                           // Parent
  const ArenaTreeNode child)                                                            // Child
 {return putFirstLast_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode(1, parent, child);                      // Put a child first under its parent
 }

static  ArenaTreeNode putLast_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                         // Put a child last under its parent
 (const ArenaTreeNode parent,                                                           // Parent
  const ArenaTreeNode child)                                                            // Child
 {return putFirstLast_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode(0, parent, child);                      // Put a child last under its parent
 }

static  ArenaTreeNode putNextPrev_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                     //P Put a child next or previous to the specified sibling
 (const int   next,                                                             // Put next if true, else previous
  const ArenaTreeNode sibling,                                                          // Sibling
  const ArenaTreeNode child)                                                            // Child
 {const ArenaTree     tree   = sibling.tree;                                            // Tree
  const ArenaTreeNode parent = sibling.proto->parent(sibling);                                        // Parent node
  const ArenaTreeNode twin   = next ? sibling.proto->next(sibling) : sibling.proto->prev(sibling);                  // Next or prev sibling to sibling is such a node exists
  assert(child.tree.arena == tree.arena);                                       // Parent and child must be in same tree
  ArenaTreeContent * const s = sibling.proto->content(sibling);                                       // Pointer to sibling
  ArenaTreeContent * const c = child.proto->content(child);                                       // Pointer to child

  if (!twin.offset)                                                             // Sibling is last/first under parent
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

static  ArenaTreeNode putNext_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                         // Put a child next after the specified sibling
 (const ArenaTreeNode sibling,                                                          // Sibling
  const ArenaTreeNode child)                                                            // Child
 {return putNextPrev_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode(1, sibling, child);                      // Put child next
 }

static  ArenaTreeNode putPrev_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode                                         // Put a child before the specified sibling
 (const ArenaTreeNode sibling,                                                          // Sibling
  const ArenaTreeNode child)                                                            // Child
 {return putNextPrev_ArenaTreeNode_ArenaTreeNode_ArenaTreeNode(0, sibling, child);                      // Put a child last under its parent
 }

//D1 Traverse                                                                   // Traverse a tree.

#define ArenaTreefe( child, parent) for(ArenaTreeNode child = parent.proto->first(parent); child.offset; child = next_ArenaTreeNode_ArenaTreeNode(child)) // Each child in a parent from first to last
#define ArenaTreefer(child, parent) for(ArenaTreeNode child = parent.proto->last(parent);  child.offset; child = prev_ArenaTreeNode_ArenaTreeNode(child)) // Each child in a parent from last to first

static void by_ArenaTree_sub                                                            // Traverse a tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (ArenaTree t,                                                                          // Tree
  void (* const function) (const ArenaTreeNode node))                                   // Function
 {void children(const ArenaTreeNode parent)                                             // Process the children of the specified parent
   {const size_t N = parent.proto->count(parent);                                            // Children plus trailing zero
    ArenaTreeNode c[N];                                                                 // Array of children
    size_t n = 0; ArenaTreefe(child, parent) c[n++] = child;                             // Load each child into an array
    for(size_t i = 0; i < N; ++i) children(c[i]);                               // Process each child allowing it to change position
    function(parent);                                                           // Process the parent
   }
  children(t.proto->root(t));                                                           // Start at the root
 }

static  size_t count_sizet_ArenaTreeNodeOffset                                          // Count the number of children in a parent
 (const ArenaTreeNode parent)                                                           // Parent
 {size_t l = 0;
  ArenaTreefe(child, parent) ++l;
  return l;                                                                     // Return count
 }

static  size_t count_size_t_tree                                                // Count the number of nodes in a tree
 (const ArenaTree tree)                                                                 // Tree
 {size_t l = 0;
  void count(const ArenaTreeNode parent)                                                // Process the children of the specified parent
   {l++; ArenaTreefe(child, parent) count(child);                                        // Each child
    if (l > 99) printStackBackTrace("Too many\n");
   }
  count(tree.proto->root(tree));                                                           // Start at the root
  return l;                                                                     // Return count
 }

static  ArenaTreeString printWithBrackets_string_ArenaTreeNode                                  // Print a node and the tree below it in preorder as a string with each sub tree enclosed in brackets.
 (const ArenaTreeNode   node)                                                           // Node
 {size_t  l = 0;                                                                // Length of output string
  void len(const ArenaTreeNode parent)                                                  // Find the size of buffer we will need
   {l += strlen(parent.proto->key(parent));                                                  // Parent key
    if (!parent.proto->isEmpty(parent))                                                      // Parent has children
     {l += 2;                                                                   // Space for brackets
      ArenaTreefe(child, parent) len(child);                                             // Each child
     }
   }

  ArenaTreeString p;
  void print(const ArenaTreeNode parent)                                                // Print the children of the specified parent
   {const ArenaTreeString k = parent.proto->key(parent);
    p = stpcpy(p, k);
    if (!parent.proto->isEmpty(parent))                                                      // Parent has children
     {*p++ = '(';                                                               // Start children
      ArenaTreefe(child, parent) print(child);                                           // Each child
      *p++ = ')';                                                               // Start children
     }
    *p = 0;                                                                     // End the string
   }

  len(node);
  const ArenaTreeString s = p = alloc(l+1); *s = 0;
  print(node);
  return s;
 }

static  ArenaTreeString printWithBrackets_string_tree                                   // Print an entire tree in preorder as a string with brackets around each sub tree
 (const ArenaTree t)                                                                    // Tree
 {const ArenaTreeNode node = t.proto->root(t);                                                  // Root
  return node.proto->printWithBrackets(node);
 }

static  ArenaTreeString print_string_ArenaTreeNode                                              // Print a node and the tree below it in preorder as a string.
 (const ArenaTreeNode   node)                                                           // Node
 {size_t  l = 0;                                                                // Length of output string
  void len(const ArenaTreeNode parent)                                                  // Find the size of buffer we will need
   {l += strlen(parent.proto->key(parent));                                                  // Parent key
    ArenaTreefe(child, parent) len(child);                                               // Each child
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
  return s;
 }

static  ArenaTreeString print_string_tree                                               // Print an entire tree in preorder as a string.
 (const ArenaTree t)                                                                    // Tree
 {const ArenaTreeNode node = t.proto->root(t);                                                  // Root
  return node.proto->print(node);
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
 {for  (ArenaTreeNode child = parent.proto->last(parent); child.offset; child = parent.proto->last(parent))      // Each child in a parent from first to last
   {parent.proto->putNext(parent, child.proto->cut(child));                                              // Place each child after the parent
   }
  return parent.proto->cut(parent);                                                          // Remove and return empty parent
 }

static  ArenaTreeNode wrap_ArenaTreeNode_string_pointer                                         // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
 (const ArenaTreeNode   child,                                                          // Child to wrap
  const ArenaTreeString key)                                                            // Key for new parent
 {const ArenaTree       tree = child.tree;                                              // Tree
  const ArenaTreeNode   parent = tree.proto->new(tree, key);                                       // New parent
  child.proto->putNext(child, parent);                                                     // Place parent after child
  parent.proto->putLast(parent, child.proto->cut(child));                                                // Place child under parent
  return parent;
 }

//D1 Input and Output                                                           // Read and write a tree from/to a file

static size_t used_ArenaTree                                                            // Amount of space currently being used within the arena of a tree.
 (const ArenaTree tree)                                                                 // Tree
 {return tree.arena->used;
 }

static void write_void_ArenaTree_string                                                 // Write a tree to a named file or abort
 (const ArenaTree       tree,                                                           // Tree
  const ArenaTreeString file)                                                           // File
 {const int o = open(file, O_CREAT| O_WRONLY, S_IRWXU);                         // Open for output creating if needed
  if (o < 0) printStackBackTrace("Cannot open file: %s for write\n", file);

  time_t current; time(&current);
//const ArenaTreeDescription h = {1, 1, 0, tree.arena->used, (root_ArenaTreeNodeOffset_ArenaTree(tree)).offset, current}; // Create arena tree header describing the tree
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
 (const ArenaTreeString file)                                                           // File
 {ArenaTree tree;
  tree.arena = alloc(sizeof(ArenaTreeArena));
  tree.proto = &ProtoTypes_ArenaTree;                                                   // Initialize tree prototype

  const int i = open(file, 0, O_RDONLY);                                        // Open for input
  if (i < 0) printStackBackTrace("Cannot open file: %s for read\n", file);

  struct ArenaTreeDescription h;
  if (sizeof(h) != read(i, &h, sizeof(h)))                                      // Read header
   {printStackBackTrace("Cannot read header from file: %s\n", file);
   }

  tree.arena->data = alloc(tree.arena->size = tree.arena->used = h.used);       // Allocate arena

  ssize_t r = read(i, tree.arena->data, tree.arena->used);                      // Read arena
  if (r < 0 || tree.arena->used != (size_t)r)
   {printStackBackTrace("Cannot read ArenaTree from file: %s\n", file);
   }

  close(i);
  tree.proto = &ProtoTypes_ArenaTree;
  tree.arena->root = h.root;                                                    // Offset to root in arena
  return tree;
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0
#define treeIs(n, s) {ArenaTreeString p = n.proto->printWithBrackets(n); assert(strcmp(p,s) == 0); free(p);}// Check the content of a tree

void test0()                                                                    //TnewArenaTree //Tnew //TnewNStr //Tfree //TputFirst //TputLast //Tfe //Tfer
 {ArenaTree        t = newArenaTree(({struct ArenaTree t = {proto: &ProtoTypes_ArenaTree};   t;}));                                                            // Create a tree
  ArenaTreeNode root = t.proto->root(t);

  for(int i = 0; i < 10; ++i)                                                   // Load tree
   {char s[2] = {'a'+i, 0};
    ArenaTreeNode         child = t.proto->new(t, s);    root.proto->putLast(root, child);
                  child = t.proto->newNStr(t, s, 1); root.proto->putFirst(root, child);
    s[0] = 'A'+i; child.proto->setKey(child,  s);
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

void test1()                                                                    //Troot //Tfirst //Tlast //Tnext //Tprev //Tparent //Tequals //Tprint //TprintWithBrackets //TfromLetters
 {ArenaTree t = newArenaTree(({struct ArenaTree t = {proto: &ProtoTypes_ArenaTree};   t;})); t.proto->fromLetters(t, "b(c(de)f)g(hi)j");
       treeIs(t.proto->root(t),     "a(b(c(de)f)g(hi)j)");

  {ArenaTreeString p = t.proto->printWithBrackets(t); assert(!strcmp(p, "a(b(c(de)f)g(hi)j)")); free(p);}
  {ArenaTreeString p = t.proto->print(t);             assert(!strcmp(p, "abcdefghij"));         free(p);}

  ArenaTreeNode a = t.proto->root(t),
        b = a.proto->first(a),
        c = b.proto->first(b),
        e = c.proto->last(c),
        d = e.proto->prev(e);

  ArenaTreeString     k = d.proto->key(d);
  assert(k[0] == 'd');
  treeIs(b, "b(c(de)f)");
  treeIs(c,   "c(de)");

  assert(c.proto->equals(c, d.proto->parent(d)));
  assert(b.proto->equals(b, c.proto->parent(c)));
  assert(a.proto->equals(a, b.proto->parent(b)));
  assert(a.proto->equals(a, t.proto->root(t)));

  t.proto->free(t);
 }

void test2()                                                                    //Tby
 {ArenaTree t = newArenaTree(({struct ArenaTree t = {proto: &ProtoTypes_ArenaTree};   t;})); t.proto->fromLetters(t, "b(c(de)f)g(hi)j");
       treeIs(t.proto->root(t),     "a(b(c(de)f)g(hi)j)");

  char l[t.proto->count(t) + 1]; *l = 0;
  void process(ArenaTreeNode n) {strcat(l, n.proto->key(n));}
  t.proto->by(t, process);
  assert(strcmp(l, "decfbhigja") == 0);

  t.proto->free(t);
 }

void test3()                                                                    //TisFirst //TisLast //TisEmpty //TisRoot //TisOnlyChild //Tcontext
 {ArenaTree t = newArenaTree (({struct ArenaTree  t = {proto: &ProtoTypes_ArenaTree };   t;})); t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
        treeIs(t.proto->root(t),     "a(b(c(de(f)gh)i)j)");

  ArenaTreeNode a = t.proto->root(t);  assert((a.proto->key(a))[0] == 'a');                                 treeIs(a, "a(b(c(de(f)gh)i)j)");
  ArenaTreeNode b = a.proto->first(a); assert((b.proto->key(b))[0] == 'b'); assert(a.proto->equals(a, b.proto->parent(b))); treeIs(b, "b(c(de(f)gh)i)");
  ArenaTreeNode c = b.proto->first(b); assert((c.proto->key(c))[0] == 'c'); assert(b.proto->equals(b, c.proto->parent(c))); treeIs(c, "c(de(f)gh)");
  ArenaTreeNode d = c.proto->first(c); assert((d.proto->key(d))[0] == 'd'); assert(c.proto->equals(c, d.proto->parent(d))); treeIs(d, "d");
  ArenaTreeNode e = d.proto->next(d);  assert((e.proto->key(e))[0] == 'e'); assert(c.proto->equals(c, e.proto->parent(e))); treeIs(e, "e(f)");
  ArenaTreeNode f = e.proto->last(e);  assert((f.proto->key(f))[0] == 'f'); assert(e.proto->equals(e, f.proto->parent(f))); treeIs(f, "f");
  ArenaTreeNode g = e.proto->next(e);  assert((g.proto->key(g))[0] == 'g'); assert(c.proto->equals(c, g.proto->parent(g))); treeIs(g, "g");
  ArenaTreeNode h = g.proto->next(g);  assert((h.proto->key(h))[0] == 'h'); assert(c.proto->equals(c, h.proto->parent(h))); treeIs(h, "h");
  ArenaTreeNode i = c.proto->next(c);  assert((i.proto->key(i))[0] == 'i'); assert(b.proto->equals(b, i.proto->parent(i))); treeIs(i, "i");
  ArenaTreeNode j = b.proto->next(b);  assert((j.proto->key(j))[0] == 'j'); assert(a.proto->equals(a, j.proto->parent(j))); treeIs(j, "j");

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

void test4()                                                                    //Tcut //TfindFirstKey
 {ArenaTree t = newArenaTree (({struct ArenaTree  t = {proto: &ProtoTypes_ArenaTree };   t;})); t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
        treeIs(t.proto->root(t),     "a(b(c(de(f)gh)i)j)");

  ArenaTreeNode f, g;

  assert(t.proto->findFirstKey(t, "f", &f) && t.proto->findFirstKey(t, "g", &g));

  g.proto->putNext(g, f.proto->cut(f)); treeIs(t.proto->root(t), "a(b(c(degfh)i)j)");

  t.proto->free(t);
 }

void test5()                                                                    //TreadArenaTree //Twrite
 {ArenaTree t = newArenaTree (({struct ArenaTree  t = {proto: &ProtoTypes_ArenaTree };   t;})); t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
        treeIs(t.proto->root(t),     "a(b(c(de(f)gh)i)j)");

  ArenaTreeString f = "/tmp/arenaTreeTest.data";
        treeIs(t.proto->root(t),     "a(b(c(de(f)gh)i)j)");
               t.proto->write(t, f);

  ArenaTree      u = readArenaTree      (f);
  treeIs(u.proto->root(u),           "a(b(c(de(f)gh)i)j)");

  t.proto->free(t);
  u.proto->free(u);
  unlink(f);
 }

void test6()                                                                    //Tunwrap //Twrap //Tcount
 {ArenaTree t = newArenaTree(({struct ArenaTree t = {proto: &ProtoTypes_ArenaTree};   t;})); t.proto->fromLetters(t, "bce");
  treeIs(t.proto->root(t),          "a(bce)");
  assert(t.proto->count(t) == 4);

  ArenaTreeNode c;  assert(t.proto->findFirstKey(t, "c", &c));

  ArenaTreeNode d = c.proto->wrap(c, "d");
  treeIs (t.proto->root(t), "a(bd(c)e)");
  assert (t.proto->count(t) == 5);

  d.proto->unwrap(d);
  treeIs(t.proto->root(t),  "a(bce)");
  assert(t.proto->count(t) == 4);
         t.proto->free(t);
 }

void test7()                                                                    //Toffset //Tallocate //TsetKey //Tkey //TsaveString //TsaveNString //Tpointer //Tused //Tsize
 {ArenaTree t = newArenaTree (({struct ArenaTree  t = {proto: &ProtoTypes_ArenaTree };   t;})); t.proto->fromLetters(t, "");
  ArenaTreeDelta   d = {delta : 1};
  assert(sizeof(d) == 4);

  ArenaTreeOffset  o = t.proto->offset(t, 1);
  assert (o.tree.arena == t.arena && o.offset == 1);

  ArenaTreeOffset a = t.proto->saveString(t, "aaaa");
  assert(strcmp(a.proto->pointer(a), "aaaa") == 0);

  ArenaTreeOffset b = t.proto->saveNString(t, "bbbb", 2);
  assert(strcmp(b.proto->pointer(b), "bb") == 0);

  ArenaTreeNode      n = t.proto->new(t, "a");
                 n.proto->setKey(n, "A");
  assert(strcmp (n.proto->key(n),   "A") == 0);

  assert(t.proto->used(t) == 67);

  t.proto->free(t);
 }

void test8()                                                                    //TputNext //TputPrev
 {ArenaTree t = newArenaTree(({struct ArenaTree t = {proto: &ProtoTypes_ArenaTree};   t;})); t.proto->fromLetters(t, "A");
  ArenaTreeNode a = t.proto->root(t);  assert((a.proto->key(a))[0] == 'a');
  ArenaTreeNode A = a.proto->first(a); assert((A.proto->key(A))[0] == 'A');

  for(int i = 0; i < 10; ++i)
   {char c[2] = {'0'+i, 0};
    A.proto->putNext(A, t.proto->new(t, c));
    A.proto->putPrev(A, t.proto->new(t, c));
   }

  treeIs(t.proto->root(t), "a(0123456789A9876543210)");

  t.proto->free(t);
 }

void test9()                                                                    //TgetExtra //TnewNStrExtra //Tcontent
 {ArenaTree t = newArenaTree(({struct ArenaTree t = {proto: &ProtoTypes_ArenaTree};   t;}));
  struct S {int a, b;} s = {1, 2};
  ArenaTreeNode r = t.proto->root(t); r.proto->setKey(r, "a");

  ArenaTreeNode b = t.proto->newNStrExtra(t, "b", 1, sizeof(s), &s);

  r.proto->putLast(r, b); r.proto->putLast(r, t.proto->new(t, "c")); r.proto->putLast(r, t.proto->new(t, "d"));

  treeIs(t.proto->root(t), "a(bcd)");
  b.proto->getExtra(b, sizeof(s), &s);
  assert(s.a == 1 && s.b == 2);
  ArenaTreeNode R = b.proto->root(b);
  assert(memcmp(&R, &r, sizeof(ArenaTreeNode)) == 0);

  const ArenaTreeContent *c = R.proto->content(R);
  assert(strcmp((char *)(t.arena->data + c->key.delta), b.proto->key(b)));

  t.proto->free(t);
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 10;                                                   // Number of times to test
  void (*tests[])(void) = {test0, test1, test2, test3, test4, test5,
                           test6, test7, test8, test9, 0};
  run_tests("ArenaTree", repetitions, tests);

  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaTree/arenaTree
