#line 1 "/home/phil/c/z/arenaListEditable/arenaListEditable.c"
//------------------------------------------------------------------------------
// Lists held in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
/*
 A ArenaListEditable contains nodes and keys. Each node can contain a list of sub nodes. Each
 node has a key and a length field which is used to store the data held by each
 node. The nodes are ordered solely by the user: the key field does not affect
 the ordering of the nodes. The key can contain \0 and other binary data as the
 length of the key field is determined solely by the length field.

 Put __ after the return type component in function names derived from signatures
*/
#define _GNU_SOURCE
#include <setjmp.h>
#include <utilities.c>
#define ArenaListEditableEditable

//D1 Structures                                                                 // Structures describing an ArenaListEditable.
#ifndef ArenaListEditable_included
#define ArenaListEditable_included





typedef struct ArenaListEditable                                                                // ArenaListEditable
 {const struct ProtoTypes_ArenaListEditable *proto;                                             // Methods associated with an ArenaListEditable
  struct ArenaListEditableArena *arena;                                                         // The arena for the ArenaListEditable
 } ArenaListEditable;

typedef struct ArenaListEditableContent                                                         // The content of a node as stored in the ArenaListEditable
 {unsigned int next, parent, first, last, prev, length;                         // Related nodes and the length of the key
#ifdef ArenaListEditableEditable
  unsigned char size;                                                           // The size of the node including the key as the exponent of a power of two
#endif
  char          data[0];                                                        // The fixed data content of the node - the actual width of this field is held in arena.width
  char          key [0];                                                        // The content of the key
 } ArenaListEditableContent;

typedef struct ArenaListEditableNode                                                            // Offset to the content of a node in the ArenaListEditable.
 {ArenaListEditable      list;                                                                  // ArenaListEditable containing the item
  size_t offset;                                                                // Offset
  const struct ProtoTypes_ArenaListEditableNode *proto;                                         // Methods associated with nodes
 } ArenaListEditableNode;

typedef struct ArenaListEditableArena                                                           // Description of the block of memory that holds an ArenaListEditable.  The individual nodes of the ArenaListEditable cannot be cannot be freed individually, but the entire ArenaListEditable can be freed. As the ArenaListEditable uses offsets to store addresses the ArenaListEditable can be moved to a different location in memory while retaining its structure.
 {size_t size;                                                                  // The total size of the arena used to hold data associated with the ArenaListEditable.
  size_t used;                                                                  // The number of bytes currently used.
  size_t root;                                                                  // Offset to the root node
  size_t width;                                                                 // Width of a fixed data area added to each node measured in bytes
  void * data;                                                                  // The arena containing the data associated with the ArenaListEditable
#ifdef ArenaListEditableEditable
  size_t freeSpace[8*sizeof(unsigned int)];                                     // Lists of freed nodes by exponent of power of two of node size.
#endif
 } ArenaListEditableArena;

typedef struct ArenaListEditableDescription                                                     // The description of an ArenaListEditable which is written as the header record for dump files.
 {const size_t version;                                                         // Version of ArenaListEditable
  const size_t littleEndian;                                                    // Little endian if true
  const size_t s64b;                                                            // 64 bit address if true, else 32 bit
  const size_t used;                                                            // The number of bytes currently used by the arena.
  const size_t root;                                                            // Offset to the root node
  const size_t width;                                                           // Width of user data associated with each node
  const time_t created;                                                         // Time in seconds since the epoch when this ArenaListEditable was created
 } ArenaListEditableDescription;

#include <arenaListEditable_prototypes.h>                                                      // ArenaListEditable prototypes now that the relevant structures have been declared
#define ArenaListEditablefe( child, parent) for(ArenaListEditableNode child = parent.proto->first(parent); child.proto->valid(child); child = child.proto->next(child)) // Each child in a parent from first to last
#define ArenaListEditablefer(child, parent) for(ArenaListEditableNode child = parent.proto->last(parent) ; child.proto->valid(child); child = child.proto->prev(child)) // Each child in a parent from last to first
#define makeLocalCopyOfArenaListEditableKey(string,stringLength,node) const size_t stringLength = content_ArenaListEditableNode(node)->length; char string[stringLength+1]; string[stringLength] = 0; memcpy(string, key_pointer_ArenaListEditableNode(node), stringLength); // Copy the key and the length of the key of the specified node to the stack.

//D1 Constructors                                                               // Construct a new ArenaListEditable.

static ArenaListEditable makeArenaListEditableWithWidth                                                         // Create a new ArenaListEditable with the specified width for the optional user data associated with each node.
 (const size_t width)                                                           // Width
 {ArenaListEditableArena * a = alloc(sizeof(ArenaListEditableArena));                                           // Allocate arena description
  memset(a, 0, sizeof(ArenaListEditableArena));                                                 // Clear arena description
  const typeof(newArenaListEditable(({struct ArenaListEditable t = {arena: a, proto: &ProtoTypes_ArenaListEditable}; t;}))) t = newArenaListEditable(({struct ArenaListEditable t = {arena: a, proto: &ProtoTypes_ArenaListEditable}; t;}));                                                          // ArenaListEditable we are creating

  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  a->used   = 0;                                                                // Length used so far
  a->root   = 0;                                                                // Root not set in ArenaListEditable
  a->width  = width;                                                            // Width of user data associated with each node
  t.proto->node(t, "", 0);                                                              // Initialize root node
  return t;
 }

static ArenaListEditable makeArenaListEditable                                                                  // Create a new ArenaListEditable
 ()                                                                             // ArenaListEditableallocator
 {return makeArenaListEditableWithWidth(0);                                                     // Allocate arena description
 }

static void swap_ArenaListEditable_ArenaListEditable                                                            // Swap the arenas of two ArenaListEditable
 (ArenaListEditable source,                                                                     // Source ArenaListEditable
  ArenaListEditable target)                                                                     // Target ArenaListEditable
 {const typeof(source.arena) s = source.arena; const typeof(target.arena) t = target.arena;                                           // Arenas
  ArenaListEditableArena u = *t; *t = *s; *s = u;
 }

static char * check_string_ArenaListEditable                                                    //P Return a string describing the structure
 (const ArenaListEditable list)                                                                 // ArenaListEditable
 {if (list.arena->data) {} return "ArenaListEditable";
 }

//D1 Pointers and Offsets                                                       // Operations on pointers and offsets

static void * pointer_ArenaListEditable_size                                                    //PV Return a temporary pointer to data in the arena of the specified ArenaListEditable
 (const ArenaListEditable      list,                                                            // ArenaListEditable
  const size_t offset)                                                          // Offset
 {if (offset > list.arena->used)                                                // Check that the offset is valid
   {printStackBackTraceAndExit(1, "Accessing area outside arena");
   }
  return (void *)(list.arena->data + offset);                                   // Convert a non zero delta that is within the arena to a valid pointer
 }

static ArenaListEditableContent * content_ArenaListEditableNode                                                 //PV Convert a node offset to an address so that the content of a node can be updated in situ as long as the ArenaListEditable is not reallocated to a different position.
 (const ArenaListEditableNode node)                                                             // ArenaListEditableNode
 {return (ArenaListEditableContent *)pointer_ArenaListEditable_size(node.list, node.offset);
 }

static size_t width_size_ArenaListEditable                                                      // Get the width of the data area for a ArenaListEditable
 (const ArenaListEditable list)                                                                 // ArenaListEditableNode
 {return list.arena->width;
 }

static size_t width_size_ArenaListEditableNode                                                  // Get the width of the data area for a node
 (const ArenaListEditableNode node)                                                             // ArenaListEditableNode
 {return node.list.arena->width;
 }

static void * key_pointer_ArenaListEditableNode                                                 //V Get a temporary pointer to the key of a node.
 (const ArenaListEditableNode node)                                                             // ArenaListEditableNode
 {const typeof(node.proto->width(node)) width = node.proto->width(node);                                                         // Width of node
  return pointer_ArenaListEditable_size(node.list, node.offset + sizeof(ArenaListEditableContent) + width);     // The key is stored after the node and optional user data in the arena.
 }

static void * data_pointer_ArenaListEditableNode                                                //V Get a temporary pointer to the user data of a node.
 (const ArenaListEditableNode node)                                                             // ArenaListEditableNode
 {return pointer_ArenaListEditable_size(node.list, node.offset + sizeof(ArenaListEditableContent));             // The optional user data is stored immediately after the node in the arena.
 }

static size_t length_size_ArenaListEditableNode                                                 // Get the length of the key associated with a node
 (const ArenaListEditableNode node)                                                             // ArenaListEditableNode
 {return node.proto->content(node)->length;
 }

static  ArenaListEditableNode  nodeFromOffset_ArenaListEditable_size                                            //P Create a node to locate an allocation within the arena of a ArenaListEditable.
 (const ArenaListEditable      list,                                                            // ArenaListEditable
  const size_t delta)                                                           // Delta within arena. A delta of zero represents no such node.
 {return newArenaListEditableNode(({struct ArenaListEditableNode t = {list: list, offset: delta, proto: &ProtoTypes_ArenaListEditableNode}; t;}));
 }

//D1 Checks                                                                     // Check the validity of a node

static int keyEquals_int_ArenaListEditableNode_pointer_size                                     // Confirm that the key of a node is equal to the specified memory
 (const ArenaListEditableNode        node,                                                      // ArenaListEditableNode whose key is to be checked
  const char * const key,                                                       // Expected key value
  const size_t       length)                                                    // Length of expected key value
 {const typeof(node.proto->length(node)) l = node.proto->length(node);                                                            // Recorded length of node
  if (l != length) return 0;                                                    // Different lengths means the keys is unequal
  return !memcmp(node.proto->key(node), key, l);                                           // Compare memory
 }

static int equals_int_ArenaListEditableNode_ArenaListEditableNode                                               // Confirm two nodes are equal
 (const ArenaListEditableNode a,                                                                // First offset
  const ArenaListEditableNode b)                                                                // Second offset
 {return a.list.arena == b.list.arena && a.offset == b.offset;
 }

//D1 Root node                                                                  // The root node of an ArenaListEditable is at offset 0

static  ArenaListEditableNode root_ArenaListEditableNode_ArenaListEditable                                                      // Return the root node of a ArenaListEditable.
 (const ArenaListEditable list)                                                                 // ArenaListEditable
 {return newArenaListEditableNode(({struct ArenaListEditableNode t = {list: list, proto: &ProtoTypes_ArenaListEditableNode}; t;}));                                                 // Root node
 }

static  ArenaListEditableNode root_ArenaListEditableNodeOffset_ArenaListEditableNodeOffset                                      // Return the root node of the ArenaListEditable containing the specified node.
 (const ArenaListEditableNode node)                                                             // ArenaListEditableNode
 {return node.list.proto->root(node.list);
 }

//D1 Allocation                                                                 // Allocating memory in the ArenaListEditable

static  ArenaListEditableNode  allocate_offset_ArenaListEditable_size                                           //P Allocate a node within the arena of a ArenaListEditable
 (const ArenaListEditable      list,                                                            // ArenaListEditable in which to allocate
  const size_t size)                                                            // Amount of memory required
 {
#ifdef ArenaListEditableEditable                                                                // Check the free space chains first to see if there is any free space we can reuse rather than allocating more space in the arena.
  const typeof(exponentOfNextPowerOfTwo(size)) e = exponentOfNextPowerOfTwo(size);                                           // Exponent of power of two of block size
  const typeof(list.arena ->freeSpace[e]) f = list.arena ->freeSpace[e];                                                // First element (if any) on the free space chain of this size
  if (f)                                                                        // Free chain has an element
   {ArenaListEditableContent * c = list.proto->pointer(list, f);                                           // Content of first free node
    list.arena->freeSpace[e] = c->next;                                         // Remove node from free chain
    return list.proto->nodeFromOffset(list, f);                                            // Return node - it was cleared when it was freed
   }
#endif

 if (list.arena->used + size < list.arena->size)                                // Allocate within existing arena
   {const typeof(list.proto->nodeFromOffset(list, list.arena->used)) n = list.proto->nodeFromOffset(list, list.arena->used);                                // Current offset to open memory
    list.arena->used += size;                                                   // Allocate
#ifdef ArenaListEditableEditable                                                                // Check the free space chains first to see if there is any free space we can reuse rather than allocating more space in the arena.
    ArenaListEditableContent * c = n.proto->content(n);                                                 // Content of node
    c->size = exponentOfNextPowerOfTwo(size);                                   // Node size
#endif
    return n;                                                                   // Return allocation
   }
  else                                                                          // Reallocate arena
   {const typeof(nextPowerOfTwo(list.arena->size + size)) S = nextPowerOfTwo(list.arena->size + size);                                // Round up memory required
    if (S < ((size_t)((1l<<(sizeof(((ArenaListEditableContent *)0)->length)*8)) - 1)))          // The maximum size of a delta if there are 8 bits in a byte
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

static size_t used_ArenaListEditable                                                            // Amount of space currently being used within the arena of a ArenaListEditable.
 (const ArenaListEditable list)                                                                 // ArenaListEditable
 {return list.arena->used;
 }

static ArenaListEditableNode node_ArenaListEditableNode_ArenaListEditable_string_size                                           // Create a new ArenaListEditable node with the specified key.
 (const ArenaListEditable            list,                                                      // ArenaListEditable in which to create the node
  const void * const key,                                                       // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the ArenaListEditable is determined solely by the user.
  const size_t       length)                                                    // Length of the key.
 {const typeof(sizeof(ArenaListEditableContent) + length + list.proto->width(list)) s = sizeof(ArenaListEditableContent) + length + list.proto->width(list);                                 // Minimum width of node
#ifndef ArenaListEditableEditable
  const typeof(list.proto->allocate(list, s)) n = list.proto->allocate(list, s);                                                       // Offset of space allocated for a node and key
#else
  const typeof(exponentOfNextPowerOfTwo(s)) t = exponentOfNextPowerOfTwo(s);                                              // Round size up to next power of two
  const typeof(list.proto->allocate(list, 1ul<<t)) n = list.proto->allocate(list, 1ul<<t);                                                  // Offset of space allocated for a node and key
#endif
  const typeof(n.proto->content(n)) c = n.proto->content(n);                                                              // Address content
  c->length = length;                                                           // Save key length
  memcpy(n.proto->key(n), key, length);                                                 // Copy in key
  return n;                                                                     // Return node
 }

static void setKey_ArenaListEditableNode_string_size                                            // Set the key of a node to a different value as long as there is enough room from the last setting.
 (const ArenaListEditableNode        node,                                                      // ArenaListEditableNode
  const void * const key,                                                       // Key
  const size_t       length)                                                    // Length of key
 {const typeof(node.proto->length(node)) l = node.proto->length(node);                                                            // Length of existing key
  if (length <= l)                                                              // There is enough room in the existing key for the new key
   {memcpy(node.proto->key(node), key, length);                                            // Copy in the key
    node.proto->content(node) -> length = length;                                          // Set key length
    return;
   }
#ifndef ArenaListEditableEditable
  printStackBackTraceAndExit(1, "Attempt to set longer key\n");                 // Error: no room for the key and not editable
#else
  const typeof(node.list.proto->node(node.list, key, length)) n = node.list.proto->node(node.list, key, length);                                            // Create a node capable of storing the key
  n.proto->replace(n, node);                                                            // Replace the existing node with the new node
  node.proto->free(node);                                                                  // Place the original node on the appropriate free chain
#endif
 }

static void setData_ArenaListEditableNode_pointer                                               // Set the optional user data associated with a node in an ArenaListEditable
 (const ArenaListEditableNode        node,                                                      // ArenaListEditableNode
  const void * const data)                                                      // Data
 {memcpy(node.proto->data(node), data, node.proto->width(node));                                      // Copy in the data
 }

static void getData_ArenaListEditableNode_pointer                                               // Get the optional user data associated with a node in an ArenaListEditable
 (const ArenaListEditableNode  node,                                                            // ArenaListEditableNode
   void * const data)                                                           // Data
 {memcpy(data, node.proto->data(node), node.proto->width(node));                                      // Copy out the data
 }

static void copyData_ArenaListEditableNode_ArenaListEditableNode                                                // Copy the data from the source node to the target node.
 (const ArenaListEditableNode target,                                                           // Target ArenaListEditableNode
  const ArenaListEditableNode source)                                                           // Source ArenaListEditableNode
 {memcpy(target.proto->data(target), source.proto->data(source), source.proto->width(source));                         // Copy the data if any
 }

static ArenaListEditableNode  copy_ArenaListEditable_ArenaListEditableNode                                                      // Copy a node from one ArenaListEditable to another ArenaListEditable
 (const ArenaListEditableNode source,                                                           // Source ArenaListEditableNode
  const ArenaListEditable     target)                                                           // Target ArenaListEditable
 {makeLocalCopyOfArenaListEditableKey(s, l, source);
  const typeof(target.proto->node(target, s, l)) n = target.proto->node(target, s, l);                                                    // Copy node to target
  if  (n.proto->width(n) != source.proto->width(source)) printStackBackTrace("Width of target different from width of source\n");
  else n.proto->copyData(n, source);                                                    // Copy data from source to target
  return n;                                                                     // Copy any data associated with the node
 }                                                                              // Return new node

static void fromLetters_ArenaListEditable_string                                                // Load ArenaListEditable from a string of letters and brackets.  The root node of the ArenaListEditable so constructed is always given the letter 'a'.
 (const ArenaListEditable      list,                                                            // ArenaListEditable
  const char * str)                                                             // String of letters and brackets describing desired list structure
 {const ArenaListEditableNode n = list.proto->root(list);                                                  // The node we are currently  adding to

  void parse(ArenaListEditableNode current)                                                     // Parse the string of letters and brackets to construct the desired ArenaListEditable
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
         {current.proto->putLast(current, list.proto->node(list, &c, 1));
          ++str; continue;
         }
       }
     }
   }

  parse(n);                                                                     // Parse the string of letters and brackets to construct the desired ArenaListEditable
 };

static void free_ArenaListEditable                                                              // Free an entire ArenaListEditable.
 (const ArenaListEditable list)                                                                 // ArenaListEditable to free
 {const typeof(list.arena) a = list.arena;
  if  (a->data) free(a->data);
  free(a);
 }

static void free_ArenaListEditableNode                                                          // Free a node. If the ArenaListEditable is editable the node is made available for reuse otherwise the node wastes space. A new copy of the ArenaListEditable without wasted space can be made with copyAndCompact_ArenaListEditable .
 (ArenaListEditableNode node)                                                                   // ArenaListEditableNode to free
#ifdef ArenaListEditableEditable
 {const typeof(node.proto->content(node)) c = node.proto->content(node);                                                           // Content of node
  const typeof(c->size) s = c->size;                                                                  // Size of node as a power of two
  memset(c, 0, 1ul<<s);                                                         // Clear node
  const typeof(node.list.arena->freeSpace[c->size  = s]) f = node.list.arena->freeSpace[c->size  = s];                                 // First element  of free space chain
  if (f) c->next  = f;                                                          // Place the incoming node at the front of the chain
      node.list.arena->freeSpace[s] = node.offset;                              // New first node on the free space chain
 }
#else
 {node=node;                                                                    // Free has no effect if we are not editable.
 }
#endif

//D1 Navigation                                                                 // Navigate through a ArenaListEditable.

static int valid_ArenaListEditableNode                                                          // Check that a node is valid.
 (const ArenaListEditableNode child)                                                            // ArenaListEditableNode
 {return child.offset;                                                          // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

static  ArenaListEditableNode parent_ArenaListEditableNode_ArenaListEditableNode                                                // Get the parent of a child
 (const ArenaListEditableNode child)                                                            // Child
 {if (child.proto->isRoot(child)) return child;
  return child.list.proto->nodeFromOffset(child.list, child.proto->content(child)->parent);
 }

static  ArenaListEditableNode first_ArenaListEditableNode_ArenaListEditableNode                                                 // Get the first child under a parent.
 (const ArenaListEditableNode parent)                                                           // Parent
 {return  parent.list.proto->nodeFromOffset(parent.list, parent.proto->content(parent)->first);
 }
static  ArenaListEditableNode last_ArenaListEditableNode_ArenaListEditableNode                                                 // Get the last child under a parent.
 (const ArenaListEditableNode parent)                                                           // Parent
 {return  parent.list.proto->nodeFromOffset(parent.list, parent.proto->content(parent)->last);
 }
#line 362 "/home/phil/c/z/arenaListEditable/arenaListEditable.c"
static  ArenaListEditableNode next_ArenaListEditableNode_ArenaListEditableNode                                                 // Get the next child under a parent.
 (const ArenaListEditableNode parent)                                                           // Parent
 {return  parent.list.proto->nodeFromOffset(parent.list, parent.proto->content(parent)->next);
 }
#line 362 "/home/phil/c/z/arenaListEditable/arenaListEditable.c"
static  ArenaListEditableNode prev_ArenaListEditableNode_ArenaListEditableNode                                                 // Get the prev child under a parent.
 (const ArenaListEditableNode parent)                                                           // Parent
 {return  parent.list.proto->nodeFromOffset(parent.list, parent.proto->content(parent)->prev);
 }
#line 362 "/home/phil/c/z/arenaListEditable/arenaListEditable.c"

static  ArenaListEditableNode first_ArenaListEditableNode_ArenaListEditable                                                     // Get the first child in the specified ArenaListEditable.
 (const ArenaListEditable list)                                                                 // Parent
 {const ArenaListEditableNode root = list.proto->root(list);
  return root.proto->first(root);
 }
static  ArenaListEditableNode last_ArenaListEditableNode_ArenaListEditable                                                     // Get the last child in the specified ArenaListEditable.
 (const ArenaListEditable list)                                                                 // Parent
 {const ArenaListEditableNode root = list.proto->root(list);
  return root.proto->last(root);
 }
#line 369 "/home/phil/c/z/arenaListEditable/arenaListEditable.c"
static  ArenaListEditableNode next_ArenaListEditableNode_ArenaListEditable                                                     // Get the next child in the specified ArenaListEditable.
 (const ArenaListEditable list)                                                                 // Parent
 {const ArenaListEditableNode root = list.proto->root(list);
  return root.proto->next(root);
 }
#line 369 "/home/phil/c/z/arenaListEditable/arenaListEditable.c"
static  ArenaListEditableNode prev_ArenaListEditableNode_ArenaListEditable                                                     // Get the prev child in the specified ArenaListEditable.
 (const ArenaListEditable list)                                                                 // Parent
 {const ArenaListEditableNode root = list.proto->root(list);
  return root.proto->prev(root);
 }
#line 369 "/home/phil/c/z/arenaListEditable/arenaListEditable.c"

//D1 Search                                                                     // Search for nodes.

static int equalsString_ArenaListEditableNode_string                                            // Check that the key of a node equals a string
 (const ArenaListEditableNode        node,                                                      // ArenaListEditableNode
  const char * const key)                                                       // Key
 {return node.proto->keyEquals(node, key, strlen(key));
 }

static  ArenaListEditableNode findFirst_ArenaListEditableNode_string                                            // Find the first node with the specified key in a post-order traversal of the ArenaListEditable starting at the specified node.
 (const ArenaListEditableNode        node,                                                      // ArenaListEditableNode at the start of the list to be searched
  const char * const key)                                                       // Key to find
 {jmp_buf found;
  ArenaListEditableNode F = newArenaListEditableNode(({struct ArenaListEditableNode t = {list: node.list, proto: &ProtoTypes_ArenaListEditableNode}; t;}));                                         // An invalid node

  void find(ArenaListEditableNode node)                                                         // Check whether the key of the current node matches the specified key
   {if (node.proto->equalsString(node, key))                                               // Found
     {F.offset = node.offset;
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) node.proto->by(node, find);                                          // Search the ArenaListEditable

  return F;                                                                     // Return an invalid node
 }

static  ArenaListEditableNode findFirst_ArenaListEditable_string                                                // Find the first node with the specified key in a post-order traversal of the ArenaListEditable.
 (const ArenaListEditable            list,                                                      // ArenaListEditable
  const char * const key)                                                       // Key to find
 {jmp_buf found;
  ArenaListEditableNode F = newArenaListEditableNode(({struct ArenaListEditableNode t = {list: list, proto: &ProtoTypes_ArenaListEditableNode}; t;}));                                              // An invalid node

  void find(ArenaListEditableNode node)                                                         // Check whether the key of the current node matches the specified key
   {if (node.proto->equalsString(node, key))                                               // Found
     {F.offset = node.offset;
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) list.proto->by(list, find);                                          // Search the ArenaListEditable

  return F;                                                                     // Return an invalid node
 }

static  ArenaListEditableNode findFirstChild_ArenaListEditableNode_string                                       // Find the first child of the specified parent with the specified key.
 (const ArenaListEditableNode        parent,                                                    // Parent node
  const char * const key)                                                       // Key to find immediately under parent
 {ArenaListEditablefe(child, parent) if (child.proto->equalsString(child, key)) return child;               // Found matching child

  const ArenaListEditableNode invalid = {};                                                     // An invalid node
  return invalid;                                                               // Failed - return an invalid node
 }

static  ArenaListEditableNode findFirstChild_ArenaListEditable_string                                           // Find the first child immediately under the root with the specified key.
 (const ArenaListEditable            list,                                                      // ArenaListEditable
  const char * const key)                                                       // Key to find
 {const ArenaListEditableNode  r = list.proto->root(list);                                                 // Root node of the ArenaListEditable
  return r.proto->findFirstChild(r, key);                                               // Search the ArenaListEditable
 }

//D1 Location                                                                   // Verify the current location.

static int context_ArenaListEditableNode                                                        // Return true if the parent of the specified child has the specified key.
 (const ArenaListEditableNode         child,                                                    // Child
        ArenaListEditableNode * const parent,                                                   // Parent container
  const char  * const key)                                                      // Key
 {if (child.proto->isRoot(child)) return 0;                                                 // The root node has no context
  const typeof(*parent = child.proto->parent(child)) p = *parent = child.proto->parent(child);
  makeLocalCopyOfArenaListEditableKey(k, l, p);
  return p.proto->valid(p) && !memcmp(k, key, l);                                       // Check key
 }

static int isFirst_ArenaListEditableNode                                                        // Confirm a child is first under its parent
 (const ArenaListEditableNode child)                                                            // Child
 {const ArenaListEditableNode parent = child.proto->parent(child);
  return child.proto->equals(child, parent.proto->first(parent));
 }
static int isLast_ArenaListEditableNode                                                        // Confirm a child is last under its parent
 (const ArenaListEditableNode child)                                                            // Child
 {const ArenaListEditableNode parent = child.proto->parent(child);
  return child.proto->equals(child, parent.proto->last(parent));
 }
#line 448 "/home/phil/c/z/arenaListEditable/arenaListEditable.c"

static int isEmpty_ArenaListEditableNode                                                        // Confirm a node has no children.
 (const ArenaListEditableNode node)                                                             // ArenaListEditableNode
 {const ArenaListEditableNode child = node.proto->first(node);
  return !child.proto->valid(child);                                                        // No first child so the node is empty
 }

static int isOnlyChild_ArenaListEditableNode                                                    // Confirm that this child is the only child of its parent
 (const ArenaListEditableNode child)                                                            // Child
 {const typeof(child.proto->parent(child)) parent = child.proto->parent(child);                                                      // Parent
  return parent.proto->valid(parent) && child.proto->isFirst(child) && child.proto->isLast(child);                   // Child is first and last and not the root so it is an only child
 }

static int isRoot_ArenaListEditableNode                                                         // Confirm a node is the root
 (const ArenaListEditableNode node)                                                             // ArenaListEditableNode
 {return !node.offset;
 }

//D1 Put                                                                        // Insert children into a ArenaListEditable.

static  ArenaListEditableNode putFL_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode                                           //P Put a child first or last under its parent
 (const int   first,                                                            // Put first if true, else last
  const ArenaListEditableNode parent,                                                           // Parent
  const ArenaListEditableNode child)                                                            // Child
 {const typeof(parent.list) list = parent.list;
  if (child.list.arena != list.arena) printStackBackTrace("Wrong list\n");      // Parent and child must be in same ArenaListEditable
  const typeof(parent.offset) P = parent.offset;    const typeof(child.offset) C = child.offset;                                       // Delta of parent and child
  const typeof(parent.proto->content(parent)) p = parent.proto->content(parent); const typeof(child.proto->content(child)) c = child.proto->content(child);                                   // Pointers to parent and child
  const typeof(first ? p->first : p->last) L = first ? p->first : p->last;                                               // Delta of child currently first/last
  if (!L)                                                                       // No children yet
   {if (first) p->last = C; else p->first = C;                                  // Delta to last/first child
   }
  else                                                                          // Existing children
   {ArenaListEditableContent * const l = list.proto->pointer(list, L);                                     // Link first/last two children together
    if (first) l->prev = C; else l->next = C;
    if (first) c->next = L; else c->prev = L;
   }
  c->parent = P;                                                                // Parent new first/last child
  if (first)  p->first = C; else p->last = C;                                   // Parent points to new first/last child
  return child;
 }

static  ArenaListEditableNode putTreeFirst_ArenaListEditableNode_ArenaListEditableNode                                          // Put a child first in the ArenaListEditable containing the arena in which the child was allocated.
 (const ArenaListEditableNode child)                                                            // Child
 {const typeof(child.list) t = child.list;                                                               // ArenaListEditable containing arena containing child
  const typeof(t.proto->root(t)) r = t.proto->root(t);
  return r.proto->putFirst(r, child);                                                   // Put the child first
 }
static  ArenaListEditableNode putTreeLast_ArenaListEditableNode_ArenaListEditableNode                                          // Put a child last in the ArenaListEditable containing the arena in which the child was allocated.
 (const ArenaListEditableNode child)                                                            // Child
 {const typeof(child.list) t = child.list;                                                               // ArenaListEditable containing arena containing child
  const typeof(t.proto->root(t)) r = t.proto->root(t);
  return r.proto->putLast(r, child);                                                   // Put the child last
 }
#line 497 "/home/phil/c/z/arenaListEditable/arenaListEditable.c"

static  ArenaListEditableNode putFirst_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode                                        // Put a child first under its parent
 (const ArenaListEditableNode parent,                                                           // Parent
  const ArenaListEditableNode child)                                                            // Child
 {return putFL_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode(1, parent, child);                             // Put a child first under its parent
 }
static  ArenaListEditableNode putLast_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode                                        // Put a child last under its parent
 (const ArenaListEditableNode parent,                                                           // Parent
  const ArenaListEditableNode child)                                                            // Child
 {return putFL_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode(0, parent, child);                             // Put a child last under its parent
 }
#line 504 "/home/phil/c/z/arenaListEditable/arenaListEditable.c"

static  ArenaListEditableNode putNP_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode                                           //P Put a child next or previous to the specified sibling
 (const int   next,                                                             // Put next if true, else previous
  const ArenaListEditableNode sibling,                                                          // Sibling
  const ArenaListEditableNode child)                                                            // Child
 {const typeof(sibling.list) list = sibling.list;                                                        // ArenaListEditable
  const typeof(sibling.proto->parent(sibling)) parent = sibling.proto->parent(sibling);                                                    // Parent node
  const typeof(next ? sibling.proto->next(sibling) : sibling.proto->prev(sibling)) twin = next ? sibling.proto->next(sibling) : sibling.proto->prev(sibling);                              // Next or prev sibling to sibling is such a node exists
  assert(child.list.arena == list.arena);                                       // Parent and child must be in same ArenaListEditable
  const typeof(sibling.proto->content(sibling)) s = sibling.proto->content(sibling);                                                        // Pointer to sibling
  const typeof(child.proto->content(child)) c = child.proto->content(child);                                                        // Pointer to child

  if (!twin.proto->valid(twin))                                                            // Sibling is last/first under parent
   {if (next) parent.proto->putLast(parent, child); else parent.proto->putFirst(parent, child);           // Place child
   }
  else                                                                          // Not last/first
   {const typeof(twin.proto->content(twin)) w = twin.proto->content(twin);                                                         // Pointer to twin of sibling
    if (next)
     {s->next = w->prev = child.offset;
      c->prev = sibling.offset;
      c->next = twin.offset;
     }
    else
     {s->prev = w->next = child.offset;
      c->next = sibling.offset;
      c->prev = twin.offset;
     }
   }
  c->parent = parent.offset;                                                    // Parent new next/prev child
  return child;
 }

static void setUp_ArenaListEditableNode_ArenaListEditableNode                                                   //P Make the specified parent node the parent of the specified child node
 (const ArenaListEditableNode child,                                                            // Child
  const ArenaListEditableNode parent)                                                           // Child
 {child.proto->content(child)->parent = parent.offset;                                      // Set parent of child
 }

static  ArenaListEditableNode putNext_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode                                         // Put a child next after the specified sibling
 (const ArenaListEditableNode sibling,                                                          // Sibling
  const ArenaListEditableNode child)                                                            // Child
 {return putNP_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode(1, sibling, child);                            // Put child next
 }
static  ArenaListEditableNode putPrev_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode                                         // Put a child previous after the specified sibling
 (const ArenaListEditableNode sibling,                                                          // Sibling
  const ArenaListEditableNode child)                                                            // Child
 {return putNP_ArenaListEditableNode_ArenaListEditableNode_ArenaListEditableNode(0, sibling, child);                            // Put child previous
 }
#line 548 "/home/phil/c/z/arenaListEditable/arenaListEditable.c"

static  void replace__ArenaListEditableNode_ArenaListEditableNode                                               // Replace the specified node with this node
 (const ArenaListEditableNode with,                                                             // Replace with this node
  const ArenaListEditableNode over)                                                             // Node to be replaced
 {over.proto->putPrev(over, with);                                                         // Place new node
  ArenaListEditableContent * w = with.proto->content(with);                                                // Transfer children
  ArenaListEditableContent * o = over.proto->content(over);
  w->first = o->first; w->last = o->last;
  over.proto->cut(over);                                                                   // Remove node being replaced
 }

//D1 Traverse                                                                   // Traverse a ArenaListEditable.

static void by_ArenaListEditableNode_sub                                                        // Traverse the ArenaListEditable rooted at the specified node in post-order calling the specified function to process each child node.  The ArenaListEditable is buffered allowing changes to be made to the structure of the ArenaListEditable without disruption as long as each child checks its context.
 (ArenaListEditableNode node,                                                                   // ArenaListEditableNode
  void (* const function) (const ArenaListEditableNode node))                                   // Function
 {void children(const ArenaListEditableNode parent)                                             // Process the children of the specified parent
   {if (!parent.proto->valid(parent)) return;                                                // Empty child
    const typeof(parent.proto->countChildren(parent)) N = parent.proto->countChildren(parent);                                                 // Number of children
    if (N)                                                                      // Process children if there are any
     {size_t c[N+1];                                                            // Array of children terminated by a trailing zero
      size_t n = 0; ArenaListEditablefe(child, parent) c[n++] = child.offset;                   // Load each child into an array
      for(size_t i = 0; i < N; ++i)                                             // Process each child allowing it to change its position without changing the traversal
       {children(newArenaListEditableNode(({struct ArenaListEditableNode t = {list: parent.list, offset: c[i], proto: &ProtoTypes_ArenaListEditableNode}; t;})));
       }
     }
    function(parent);                                                           // Process the parent
   }
  children(node);                                                               // Start at the specified node
 }

static void by_ArenaListEditable_sub                                                            // Traverse a ArenaListEditable in post-order calling the specified function to process each child node.  The ArenaListEditable is buffered allowing changes to be made to the structure of the ArenaListEditable without disruption as long as each child checks its context.
 (const ArenaListEditable list,                                                                 // ArenaListEditable
  void (* const function) (const ArenaListEditableNode node))                                   // Function
 {const typeof(list.proto->root(list)) root = list.proto->root(list);                                                           // Root node
  const typeof(list.proto->countChildren(list)) N = list.proto->countChildren(list);                                                  // Number of children under root
  if (N)                                                                        // Process children under root if there are any
   {size_t c[N+1];                                                              // Array of children terminated by a trailing zero
    size_t n = 0; ArenaListEditablefe(child, root) c[n++] = child.offset;                       // Load each child into an array
    for(size_t i = 0; i < N; ++i)
     {ArenaListEditableNode n = newArenaListEditableNode(({struct ArenaListEditableNode t = {list: list, offset: c[i], proto: &ProtoTypes_ArenaListEditableNode}; t;}));                            // Process each child allowing it to change position
      n.proto->by(n, function);
     }
   }
  function(root);                                                               // Start at the root
 }

static void scan_ArenaListEditableNode_sub                                                      // Traverse the ArenaListEditable rooted at the specified node calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The ArenaListEditable is buffered allowing changes to be made to the structure of the ArenaListEditable without disruption as long as each child checks its context.
 (ArenaListEditableNode node,                                                                   // ArenaListEditableNode
  void (* const function) (ArenaListEditableNode node, int start, int depth))                   // Function: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
 {void children(ArenaListEditableNode parent, int depth)                                        // Process the children of the specified parent
   {if (!parent.proto->valid(parent)) return;                                                // Empty child
    const typeof(parent.proto->countChildren(parent)) N = parent.proto->countChildren(parent);                                                // Number of children
    if (N)                                                                      // Process children if there are any
     {size_t c[N+1];                                                            // Array of children terminated by a trailing zero
      size_t n = 0; ArenaListEditablefe(child, parent) c[n++] = child.offset;                   // Load each child into an array
      function(parent, 1, depth);                                               // Call before
      for(size_t i = 0; i < N; ++i)                                             // Process each child allowing it to change its position without changing the traversal
       {children(newArenaListEditableNode(({struct ArenaListEditableNode t = {list: parent.list, offset: c[i], proto: &ProtoTypes_ArenaListEditableNode}; t;})), depth+1);
       }
      function(parent, -1, depth);                                              // Call after
     }
    else function(parent, 0, depth);                                            // Call once as there are no child nodes
   }
  children(node, 1);                                                            // Start at the root node
 }

static void scan_ArenaListEditable_sub                                                          // Traverse a ArenaListEditable calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The ArenaListEditable is buffered allowing changes to be made to the structure of the ArenaListEditable without disruption as long as each child checks its context.
 (const ArenaListEditable list,                                                                 // ArenaListEditable
  void (* const function) (ArenaListEditableNode node, int start, int depth))                   // Function: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
 {const typeof(list.proto->root(list)) root = list.proto->root(list);                                                           // Root node
  const typeof(list.proto->countChildren(list)) N = list.proto->countChildren(list);                                                  // Number of children under root
  if (N)                                                                        // Process children under root if there are any
   {size_t c[N+1];                                                              // Array of children terminated by a trailing zero
    size_t n = 0; ArenaListEditablefe(child, root) c[n++] = child.offset;                       // Load each child into an array
    function(root, 1, 0);                                                       // Call before
    for(size_t i = 0; i < N; ++i)
     {ArenaListEditableNode n = newArenaListEditableNode(({struct ArenaListEditableNode t = {list: list, offset: c[i], proto: &ProtoTypes_ArenaListEditableNode}; t;}));                            // Process each child allowing it to change position
      n.proto->scan(n, function);
     }
    function(root, -1, 0);                                                      // Call after
   }
  else function(root, 0, 0);                                                    // Call once as the root has no children
 }

static  size_t countChildren_size_ArenaListEditable                                             // Count the number of children directly under a parent.
 (const ArenaListEditable list)                                                                 // ArenaListEditable
 {size_t l = 0;
  const typeof(list.proto->root(list)) root = list.proto->root(list);
  ArenaListEditablefe(child, root) ++l;
  return l;                                                                     // Return count
 }

static  size_t countChildren_size_ArenaListEditableNode                                         // Count the number of children directly under a node.
 (const ArenaListEditableNode parent)                                                           // Parent
 {size_t l = 0;
  ArenaListEditablefe(child, parent) ++l;
  return l;                                                                     // Return count
 }

static  size_t count_size_ArenaListEditableNode                                                 // Count the number of nodes under a specified node but not including the specified node.
 (const ArenaListEditableNode node)                                                             // ArenaListEditableNode
 {size_t l = 0;

  void count(const ArenaListEditableNode parent)                                                // Process the children of the specified parent
   {if (!parent.proto->valid(parent)) return;
    l++; ArenaListEditablefe(child, parent) count(child);                                       // Each child
   }

  count(node);                                                                  // Start at the specified node
  return l - 1;                                                                 // Return count without counting the root node
 }


static  size_t count_size_ArenaListEditable                                                     // Count the number of nodes in a ArenaListEditable
 (const ArenaListEditable list)                                                                 // ArenaListEditable
 {size_t l = 0;

  const typeof(list.proto->root(list)) root = list.proto->root(list);                                                           // Root is always invalid
  ArenaListEditablefe(child, root) l += 1 + child.proto->count(child);                                      // Child under root plus its children
  return l;                                                                     // Return count without counting the root node
 }

static ArenaListEditable copyAndCompact_ArenaListEditable_ArenaListEditable                                                     // Copy a ArenaListEditable compacting any free space.  This method assumes that there are no direct external references to the nodes in the list as this process might change the location of one or more nodes in the arena.
 (const ArenaListEditable source)                                                               // Source ArenaListEditable
 {const typeof(makeArenaListEditableWithWidth(source.proto->width(source))) target = makeArenaListEditableWithWidth(source.proto->width(source));                                      // ArenaListEditable with same width
  void children(ArenaListEditableNode old, ArenaListEditableNode new)                                           // Make a new copy of each node in the source ArenaListEditable.
   {ArenaListEditablefe(child, old)                                                             // Each child of old parent
     {const typeof(child.proto->copy(child, target)) n = child.proto->copy(child, target);                                                 // Copy child to new ArenaListEditable
      new.proto->putLast(new, n);                                                         // Place new child
      children(child, n);                                                       // Copy grand children
     }
   }
  const typeof(source.proto->root(source)) rs = source.proto->root(source); const typeof(target.proto->root(target)) rt = target.proto->root(target);                                       // Start at root
  children(rs, rt);                                                             // Each child of the root node
  return target;                                                                // Return new ArenaListEditable
 }

//D1 Print                                                                      // Print ArenaListEditables in various ways

static void printWithBrackets_ArenaListEditableNode_function                                    // Apply a function to a string containing a print of the specified  node and the ArenaListEditable below it in preorder as a string with each sub ArenaListEditable enclosed in brackets.
 (const ArenaListEditableNode   node,                                                           // ArenaListEditableNode
  void (*printer)(char * string, size_t length))                                // Function to apply to printed ArenaListEditable
 {size_t l = 0;                                                                 // Length of output string
  void len(const ArenaListEditableNode parent)                                                  // Find the size of buffer we will need
   {l += parent.proto->length(parent);                                                       // Parent key
    if (!parent.proto->isEmpty(parent))                                                      // Parent has children
     {l += 2;                                                                   // Space for brackets
      ArenaListEditablefe(child, parent) len(child);                                            // Each child
     }
   }

  char * p;
  void print(const ArenaListEditableNode parent)                                                // Print the children of the specified parent
   {makeLocalCopyOfArenaListEditableKey(k, l, parent);                                          // Local copy of key
    memcpy(p, k, l); p += l;                                                    // Copy key into print string
    if (!parent.proto->isEmpty(parent))                                                      // Parent has children
     {*p++ = '(';                                                               // Start children
      ArenaListEditablefe(child, parent) print(child);                                          // Each child
      *p++ = ')';                                                               // Start children
     }
    *p = 0;                                                                     // End the string
   }

  len(node); char P[l+1]; P[0] = 0; p = P; print(node);                         // Print in local string
  printer(P, l);                                                                // Apply function
 }

static void printWithBrackets_string_ArenaListEditable                                          // Print an entire ArenaListEditable in preorder as a string with brackets around each sub ArenaListEditable
 (const ArenaListEditable list,                                                                 // ArenaListEditable
  void (*printer)(char * string, size_t length))                                // Function to apply to printed ArenaListEditable
 {const typeof(list.proto->root(list)) node = list.proto->root(list);                                                           // Root
  node.proto->printWithBrackets(node, printer);
 }

static int printsWithBracketsAs_int_ArenaListEditableNode_string                                // Check that the ArenaListEditable starting at the specified node prints with brackets as expected.
 (const ArenaListEditableNode        node,                                                      // ArenaListEditableNode
  const char * const expected)                                                  // Expected string when printed
 {int r;
  void printer(char * s, size_t l)
   {r = !strncmp(s, expected, l);
   }
  node.proto->printWithBrackets(node, printer);
  return r;
 }

static void dumpWithBrackets_ArenaListEditable                                                  //P Dump a ArenaListEditable printed with brackets to stderr
 (const ArenaListEditable list)                                                                 // ArenaListEditable
 {void printer(char * s, size_t l)
   {say("\n%s\n", s); l=l;
   }
  list.proto->printWithBrackets(list, printer);
 }

static int printsWithBracketsAs_int_ArenaListEditable_string                                    // Check that the specified ArenaListEditable prints with brackets as expected.
 (const ArenaListEditable            list,                                                      // ArenaListEditable
  const char * const expected)                                                  // Expected string when printed
 {       const typeof(list.proto->root(list)) root = list.proto->root(list);                                                    // Root
  return root.proto->printsWithBracketsAs(root, expected);
 }

static void dump_ArenaListEditable                                                              //P Dump a ArenaListEditable on stderr
 (const ArenaListEditable list)                                                                 // ArenaListEditable
 {size_t n = 0;
  void print(const ArenaListEditableNode parent, int depth)                                     // Print the children of the specified parent
   {makeLocalCopyOfArenaListEditableKey(k, l, parent);                                          // Local copy of key
    say("%lu ", ++n);                                                           // Print key number
    for(int i = 0; i < depth; ++i) say("  ");                                   // Spacer
    say("%s\n", k);                                                             // Print key
    if (!parent.proto->isEmpty(parent)) ArenaListEditablefe(child, parent) print(child, depth+1);            // Each child
   }

  const typeof(list.proto->root(list)) root = list.proto->root(list);                                                           // Root
  print(root, 0);
 }

static void dump_ArenaListEditableNode                                                          //P Dump a ArenaListEditableNode on stderr
 (const ArenaListEditableNode node)                                                             // ArenaListEditableNode
 {makeLocalCopyOfArenaListEditableKey(k, l, node);                                              // Local copy of key
  say("%lu %s\n", l, k);                                                        // Print key number
 }

static void print_ArenaListEditableNode_function                                                // Apply a function to the print of a ArenaListEditable.
 (const ArenaListEditableNode   node,                                                           // ArenaListEditableNode
  void (*printer)(char * string, size_t length))                                // Function to apply to printed ArenaListEditable
 {size_t l = 0;                                                                 // Length of output string
  void len(const ArenaListEditableNode parent)                                                  // Find the size of buffer we will need
   {l += parent.proto->length(parent);                                                       // Parent key
    if (!parent.proto->isEmpty(parent)) ArenaListEditablefe(child, parent) len(child);                       // Each child
   }

  char * p;
  void print(const ArenaListEditableNode parent)                                                // Print the children of the specified parent
   {makeLocalCopyOfArenaListEditableKey(k, l, parent);                                          // Local copy of key
    memcpy(p, k, l); p += l;
    if (!parent.proto->isEmpty(parent)) ArenaListEditablefe(child, parent) print(child);                     // Each child
    *p = 0;                                                                     // End the string
   }

  len(node); char P[l+1]; P[0] = 0; p = P; print(node);                         // Print in local string
  printer(P, l);                                                                // Apply function
 }

static int printsAs_int_ArenaListEditableNode_string                                            // Check that the specified node prints as expected.
 (const ArenaListEditableNode        node,                                                      // ArenaListEditable
  const char * const expected)                                                  // Expected string when printed
 {int r;
  void printer(char * s, size_t l)
   {r = !strncmp(s, expected, l);
   }
  node.proto->print(node, printer);
  return r;
 }

static int printsAs_int_ArenaListEditable_string                                                // Check that the specified ArenaListEditable prints as expected.
 (const ArenaListEditable            list,                                                      // ArenaListEditable
  const char * const expected)                                                  // Expected string when printed
 {int r;
  void printer(char * s, size_t l)
   {r = !strncmp(s, expected, l);
   }
  const typeof(list.proto->root(list)) root = list.proto->root(list);                                                           // Print from root of ArenaListEditable
  root.proto->print(root, printer);
  return r;
 }

static int printContains_ArenaListEditableNode                                                  // Check the print of an ArenaListEditable starting at the specified tag contains the expected string.
 (const ArenaListEditableNode   node,                                                           // Starting node
  const char *  expected)                                                       // Expected string
 {int r;
  void printer(char * s, size_t l)
   {r = !!strstr(s, expected); if(0)l=l;
   }
  node.proto->print(node, printer);
  return r;
 }

static int printContains_ArenaListEditable                                                      // Check the print of an ArenaListEditable contains the expected string.
 (const ArenaListEditable       list,                                                           // ArenaListEditable parse ArenaListEditable
  const char *  expected)                                                       // Expected string
 {int r;
  void printer(char * s, size_t l)
   {r = !!strstr(s, expected); if(0)l=l;
   }
  const typeof(list.proto->root(list)) root = list.proto->root(list);                                                           // Print from root of ArenaListEditable
  root.proto->print(root, printer);
  return r;
 }

//D1 Edit                                                                       // Edit a ArenaListEditable in situ.

static  ArenaListEditableNode cut_ArenaListEditableNode_ArenaListEditableNode                                                   // Cut out a child.
 (const ArenaListEditableNode child)                                                            // Child to cut out
 {const ArenaListEditableNode parent = child.proto->parent(child);                                         // Parent
  const typeof(parent.proto->content(parent)) p = parent.proto->content(parent); const typeof(child.proto->content(child)) c = child.proto->content(child);                                    // Parent pointer, child content

  if (child.proto->isOnlyChild(child))                                                      // Only child
   {p->first = p->last = 0;                                                     // Remove child
   }
  else if (child.proto->isLast(child))                                                      // Last child
   {const typeof(child.proto->prev(child)) L = child.proto->prev(child);
    const typeof(L.proto->content(L)) l = L.proto->content(L);
    p->last = L.offset; l->next = 0;                                            // Last child under parent
   }
  else if (child.proto->isFirst(child))                                                     // First child
   {const typeof(child.proto->next(child)) F = child.proto->next(child);
    const typeof(F.proto->content(F)) f = F.proto->content(F);
    p->first  = F.offset; f->prev = 0;                                          // Last child under parent
   }
  else                                                                          // Some where in the middle
   {const typeof(child.proto->next(child)) N = child.proto->next(child);    const typeof(child.proto->prev(child)) P = child.proto->prev(child);
    const typeof(N.proto->content(N)) n = N.proto->content(N); const typeof(P.proto->content(P)) p = P.proto->content(P);
    p->next   = N.offset; n->prev  = P.offset;
   }

  c->next = c->prev = 0;                                                        // Remove child

  return child;
 }

static  ArenaListEditableNode unwrap_ArenaListEditableNode_ArenaListEditableNode                                                // Unwrap the specified parent and return it.
 (const ArenaListEditableNode parent)                                                           // Parent to unwrap
 {for(ArenaListEditableNode child = newArenaListEditableNode(({struct ArenaListEditableNode t = {list: parent.list, offset: parent.proto->last(parent).offset, proto: &ProtoTypes_ArenaListEditableNode}; t;})); // Remove last children
    child.proto->valid(child);
    child.offset = parent.proto->last(parent).offset)
   {parent.proto->putNext(parent, child.proto->cut(child));                                              // Place each child after the parent
   }
  return parent.proto->cut(parent);                                                          // Remove and return empty parent
 }

static  ArenaListEditableNode wrap_ArenaListEditableNode_string                                                 // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
 (const ArenaListEditableNode   child,                                                          // Child to wrap
  const char * const key)                                                       // Key for new parent
 {const typeof(child.list) list = child.list;                                                          // Tree
  const typeof(list.proto->node(list, key, strlen(key))) parent = list.proto->node(list, key, strlen(key));                                       // New parent
  child.proto->putNext(child, parent);                                                     // Place parent after child
  parent.proto->putLast(parent, child.proto->cut(child));                                                // Place child under parent
  return parent;
 }

//D1 Input and Output                                                           // Read and write a ArenaListEditable from/to a file

static void write_void_ArenaListEditable_string                                                 // Write a ArenaListEditable to a named file or abort
 (const ArenaListEditable       list,                                                           // ArenaListEditable
  const char * const file)                                                      // File
 {    const typeof(open(file, O_CREAT| O_WRONLY, S_IRWXU)) o = open(file, O_CREAT| O_WRONLY, S_IRWXU);                               // Open for output creating if needed
  if (o < 0) printStackBackTrace("Cannot open file: %s for write\n", file);

  time_t current; time(&current);
  const ArenaListEditableDescription h =                                                        // Create ArenaListEditable header describing the ArenaListEditable
   {1, 1, 0, list.proto->used(list), list.proto->root(list).offset, list.proto->width(list), current};

  if (sizeof(h) != write(o, &h, sizeof(h)))                                     // Write header
   {printStackBackTrace("Cannot write ArenaListEditable header to file: %s\n", file);
   }

  const typeof(write(o, list.arena->data, list.arena->used)) w = write(o, list.arena->data, list.arena->used);                             // Write arena
  if (w < 0 || list.arena->used != (size_t)w)
   {printStackBackTrace("Cannot write ArenaListEditable arena to file: %s\n", file);
   }

  close(o);
 }

ArenaListEditable readArenaListEditable                                                                         // Read a ArenaListEditable from a file
 (const char * const file)                                                      // File
 {ArenaListEditableArena * const arena = alloc(sizeof(ArenaListEditableArena));                                 // Create arena
  const typeof(newArenaListEditable(({struct ArenaListEditable t = {arena: arena, proto: &ProtoTypes_ArenaListEditable}; t;}))) list = newArenaListEditable(({struct ArenaListEditable t = {arena: arena, proto: &ProtoTypes_ArenaListEditable}; t;}));                                                   // Initialize ArenaListEditable

      const typeof(open(file, 0, O_RDONLY)) i = open(file, 0, O_RDONLY);                                              // Open for input
  if (i < 0) printStackBackTrace("Cannot open file: %s for read\n", file);

  struct ArenaListEditableDescription h;
  if (sizeof(h) != read(i, &h, sizeof(h)))                                      // Read header
   {printStackBackTrace("Cannot read header from file: %s\n", file);
   }

  list.arena->data = alloc(arena->size = arena->used = h.used);                 // Allocate arena

      const typeof(read(i, arena->data, arena->used)) r = read(i, arena->data, arena->used);                                    // Read arena
  if (r < 0 || arena->used != (size_t)r)
   {printStackBackTrace("Cannot read ArenaListEditable from file: %s\n", file);
   }

  close(i);
  arena->root = h.root; arena->width = h.width;                                 // Offset to root in arena
  return list;
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TmakeArenaListEditable //Tnode //Tfree //TputFirst //TputLast //Tfe //Tfer
 {   const typeof(makeArenaListEditable()) t = makeArenaListEditable();                                                               // Create a ArenaListEditable
  const typeof(t.proto->root(t)) root = t.proto->root(t);

  for(int i = 0; i < 10; ++i)                                                   // Load ArenaListEditable
   {char c = 'a'+i;

    const typeof(t.proto->node(t, &c, 1)) c1 = t.proto->node(t, &c, 1); root.proto->putLast(root, c1);
    const typeof(t.proto->node(t, &c, 1)) c2 = t.proto->node(t, &c, 1); root.proto->putFirst(root, c2);
    c = 'A'+i; c2.proto->setKey(c2, &c, 1);
    makeLocalCopyOfArenaListEditableKey(k2, l2, c2);
    assert(k2[0] == c);
   }

  if (1)                                                                        // For each
   {char l[t.proto->count(t) + 1]; *l = 0;
    ArenaListEditablefe (child, root) strncat(l, child.proto->key(child), child.proto->length(child));

    assert( !strcmp(l, "JIHGFEDCBAabcdefghij"));
   }

  if (1)                                                                        // For each in reverse
   {char l[t.proto->count(t) + 1]; *l = 0;
    ArenaListEditablefer(child, root) strncat(l, child.proto->key(child), child.proto->length(child));
    assert(strcmp(l, "jihgfedcbaABCDEFGHIJ") == 0);
   }

  t.proto->free(t);
 }

void test1()                                                                    //Troot //Tfirst //Tlast //Tnext //Tprev //Tparent //Tequals //Tprint //TprintWithBrackets //TfromLetters //TprintsAs
 {const typeof(makeArenaListEditable()) t = makeArenaListEditable();    t.proto->fromLetters(t, "b(c(de)f)g(hi)j");
  assert(t.proto->printsWithBracketsAs(t, "(b(c(de)f)g(hi)j)"));
  assert(t.proto->printsAs(t, "bcdefghij"));

  const typeof(t.proto->root(t)) a = t.proto->root(t);
  const typeof(a.proto->first(a)) b = a.proto->first(a);
  const typeof(b.proto->first(b)) c = b.proto->first(b);
  const typeof(c.proto->last(c)) e = c.proto->last(c);
  const typeof(e.proto->prev(e)) d = e.proto->prev(e);

  char * k = d.proto->key(d);
  assert(k[0] == 'd');
  b.proto->printsWithBracketsAs(b, "b(c(de)f)");
  c.proto->printsWithBracketsAs(c,   "c(de)");

  assert(c.proto->equals(c, d.proto->parent(d)));
  assert(b.proto->equals(b, c.proto->parent(c)));
  assert(a.proto->equals(a, b.proto->parent(b)));
  assert(a.proto->equals(a, t.proto->root(t)));

  assert(c.proto->printsAs(c, "cde"));
  assert(t.proto->printsAs(t, "bcdefghij"));

  t.proto->free(t);
 }

void test2()                                                                    //Tby //TprintsWithBracketsAs //TprintContains //Tscan
 {  const typeof(makeArenaListEditable()) t = makeArenaListEditable(); t.proto->fromLetters(t, "b(c(de)f)g(hi)j");
  assert( t.proto->printsWithBracketsAs(t, "(b(c(de)f)g(hi)j)"));
  assert( t.proto->printContains(t, "def"));

  if (1)
   {char l[1024], *p = l;

    void process(ArenaListEditableNode n)
     {makeLocalCopyOfArenaListEditableKey(k, l, n);
      strncpy(p, k, l); p += l; *p = 0;
     }

    t.proto->by(t, process);
    assert( !strcmp(l, "decfbhigj"));
   }

  if (1)
   {char l[1024], *p = l;

    void process(ArenaListEditableNode n, int start, int depth)
     {makeLocalCopyOfArenaListEditableKey(k, l, n);
      p += sprintf(p, "%s(%d,%d)", k, start, depth);
     }

    t.proto->scan(t, process);
    assert( !strcmp(l, "(1,0)b(1,1)c(1,2)d(0,3)e(0,3)c(-1,2)f(0,2)b(-1,1)g(1,1)h(0,2)i(0,2)g(-1,1)j(0,1)(-1,0)"));
   }

  t.proto->free(t);
 }

void test3()                                                                    //TisFirst //TisLast //TisEmpty //TisRoot //TisOnlyChild //Tcontext //TcheckKey //Tvalid
 {const typeof(makeArenaListEditable()) t = makeArenaListEditable();     t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
  assert(t.proto->printsWithBracketsAs(t, "(b(c(de(f)gh)i)j)"));

  const typeof(t.proto->root(t)) a = t.proto->root(t);  assert(a.proto->equalsString(a, ""));                           assert(a.proto->printsWithBracketsAs(a, "(b(c(de(f)gh)i)j)"));
  const typeof(a.proto->first(a)) b = a.proto->first(a); assert(b.proto->equalsString(b, "b")); assert(a.proto->equals(a, b.proto->parent(b))); assert(b.proto->printsWithBracketsAs(b,  "b(c(de(f)gh)i)"));
  const typeof(b.proto->first(b)) c = b.proto->first(b); assert(c.proto->equalsString(c, "c")); assert(b.proto->equals(b, c.proto->parent(c))); assert(c.proto->printsWithBracketsAs(c,    "c(de(f)gh)"));
  const typeof(c.proto->first(c)) d = c.proto->first(c); assert(d.proto->equalsString(d, "d")); assert(c.proto->equals(c, d.proto->parent(d))); assert(d.proto->printsWithBracketsAs(d,      "d"));
  const typeof(d.proto->next(d)) e = d.proto->next(d);  assert(e.proto->equalsString(e, "e")); assert(c.proto->equals(c, e.proto->parent(e))); assert(e.proto->printsWithBracketsAs(e,       "e(f)"));
  const typeof(e.proto->last(e)) f = e.proto->last(e);  assert(f.proto->equalsString(f, "f")); assert(e.proto->equals(e, f.proto->parent(f))); assert(f.proto->printsWithBracketsAs(f,         "f"));
  const typeof(e.proto->next(e)) g = e.proto->next(e);  assert(g.proto->equalsString(g, "g")); assert(c.proto->equals(c, g.proto->parent(g))); assert(g.proto->printsWithBracketsAs(g,           "g"));
  const typeof(g.proto->next(g)) h = g.proto->next(g);  assert(h.proto->equalsString(h, "h")); assert(c.proto->equals(c, h.proto->parent(h))); assert(h.proto->printsWithBracketsAs(h,            "h"));
  const typeof(c.proto->next(c)) i = c.proto->next(c);  assert(i.proto->equalsString(i, "i")); assert(b.proto->equals(b, i.proto->parent(i))); assert(i.proto->printsWithBracketsAs(i,              "i"));
  const typeof(b.proto->next(b)) j = b.proto->next(b);  assert(j.proto->equalsString(j, "j")); assert(a.proto->equals(a, j.proto->parent(j))); assert(j.proto->printsWithBracketsAs(j,                "j"));

  assert( !a.proto->valid(a));
  assert(  b.proto->isFirst(b));
  assert(  j.proto->isLast(j));
  assert(  f.proto->isFirst(f));
  assert(  f.proto->isLast(f));

  assert(  f.proto->isEmpty(f));
  assert(  f.proto->isOnlyChild(f));
  assert( !e.proto->isOnlyChild(e));
  assert(  a.proto->isRoot(a));

  ArenaListEditableNode A, C, E;
  assert(  f.proto->context(f, &E, "e"));
  assert(  E.proto->context(E, &C, "c"));
  assert( !a.proto->context(a, &A, "a"));

  t.proto->free(t);
 }

void test4()                                                                    //Tcut //TfindFirst //TcountChildren //TequalsString //TfindFirstChild
 {const typeof(makeArenaListEditable()) t = makeArenaListEditable();     t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
  assert(t.proto->printsWithBracketsAs(t, "(b(c(de(f)gh)i)j)"));

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

  f.proto->cut(f);           assert(t.proto->printsWithBracketsAs(t, "(b(c(degh)i)j)"));
  e.proto->putFirst(e, f);   assert(t.proto->printsWithBracketsAs(t, "(b(c(de(f)gh)i)j)"));

  e.proto->cut(e);           assert(t.proto->printsWithBracketsAs(t, "(b(c(dgh)i)j)"));
  d.proto->putNext(d, e);    assert(t.proto->printsWithBracketsAs(t, "(b(c(de(f)gh)i)j)"));

  d.proto->cut(d);           assert(t.proto->printsWithBracketsAs(t, "(b(c(e(f)gh)i)j)"));
  e.proto->putPrev(e, d);    assert(t.proto->printsWithBracketsAs(t, "(b(c(de(f)gh)i)j)"));

  h.proto->cut(h);           assert(t.proto->printsWithBracketsAs(t, "(b(c(de(f)g)i)j)"));
  g.proto->putNext(g, h);    assert(t.proto->printsWithBracketsAs(t, "(b(c(de(f)gh)i)j)"));

  t.proto->free(t);
 }

void test5()                                                                    //TreadArenaTree //Twrite
 {const typeof(makeArenaListEditable()) t = makeArenaListEditable();     t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
  assert(t.proto->printsWithBracketsAs(t, "(b(c(de(f)gh)i)j)"));

   const typeof("/tmp/arenaTreeTest.data") f = "/tmp/arenaTreeTest.data";
  assert(t.proto->printsWithBracketsAs(t, "(b(c(de(f)gh)i)j)"));
   t.proto->write(t, f);

   const typeof(readArenaListEditable(f)) u = readArenaListEditable(f);
  assert(u.proto->printsWithBracketsAs(u, "(b(c(de(f)gh)i)j)"));

  t.proto->free(t);
  u.proto->free(u);
  unlink(f);
 }

void test6()                                                                    //Tunwrap //Twrap //Tcount
 {const typeof(makeArenaListEditable()) t = makeArenaListEditable();     t.proto->fromLetters(t, "bce");
  assert( t.proto->printsWithBracketsAs(t, "(bce)"));
  assert( t.proto->count(t) == 3);

         const typeof(t.proto->findFirst(t, "c")) c = t.proto->findFirst(t, "c");
  assert( c.proto->valid(c));

    const typeof(c.proto->wrap(c, "d")) d = c.proto->wrap(c, "d");
  assert( t.proto->printsWithBracketsAs(t, "(bd(c)e)"));
  assert( t.proto->count(t) == 4);

    d.proto->unwrap(d);
  assert( t.proto->printsWithBracketsAs(t, "(bce)"));
  assert( t.proto->count(t) == 3);
    t.proto->free(t);
 }

void test7()                                                                    //TputNext //TputPrev
 {const typeof(makeArenaListEditable()) t = makeArenaListEditable();    t.proto->fromLetters(t, "A");
              const typeof(t.proto->root(t)) a = t.proto->root(t);
          const typeof(a.proto->first(a)) A = a.proto->first(a);
  assert(A.proto->equalsString(A, "A"));

  for(int i = 0; i < 10; ++i)
   {char c = '0'+i;
    A.proto->putNext(A, t.proto->node(t, &c, 1));
    A.proto->putPrev(A, t.proto->node(t, &c, 1));
   }

  assert(t.proto->printsWithBracketsAs(t, "(0123456789A9876543210)"));

  t.proto->free(t);
 }

void test8()                                                                    //TputTreeFirst //TputTreeLast //TsetKey //Tkey //Tlength //Tused //TkeyEquals
 {const typeof(makeArenaListEditable()) t = makeArenaListEditable();

  const typeof(t.proto->node(t, "c", 1)) c = t.proto->node(t, "c", 1); c.proto->putTreeFirst(c);
  const typeof(t.proto->node(t, "d", 1)) d = t.proto->node(t, "d", 1); d.proto->putTreeLast(d);
  const typeof(t.proto->node(t, "b", 1)) b = t.proto->node(t, "b", 1); b.proto->putTreeFirst(b);

  b.proto->setKey(b, "B", 1);
  assert( b.proto->length(b) == 1);
  assert( *(char *)(b.proto->key(b)) == 'B');
  assert( b.proto->keyEquals(b, "B", 1));

#ifndef ArenaListEditableEditable
  assert( t.proto->used(t) == 99);
#else
  assert( t.proto->used(t) == 128);
#endif

  assert( t.proto->printsWithBracketsAs(t, "(Bcd)"));

  t.proto->free(t);
 }

void test9()                                                                    //Tswap
 {  const typeof(makeArenaListEditable()) s = makeArenaListEditable(); s.proto->fromLetters(s, "s");
    const typeof(makeArenaListEditable()) t = makeArenaListEditable(); t.proto->fromLetters(t, "t");
  assert( s.proto->printsWithBracketsAs(s, "(s)"));
  assert( t.proto->printsWithBracketsAs(t, "(t)"));
    s.proto->swap(s, t);
  assert( t.proto->printsWithBracketsAs(t, "(s)"));
  assert( s.proto->printsWithBracketsAs(s, "(t)"));
    s.proto->free(s);
    t.proto->free(t);
 }

void test10()                                                                   //Tdata //TmakeArenaListEditableWithWidth //Twidth //TgetData //TsetData
 {size_t D[] = {0,1,2,3};
    const typeof(makeArenaListEditableWithWidth (sizeof(D))) t = makeArenaListEditableWithWidth (sizeof(D));
  assert( t.proto->width(t) == sizeof(D));

    const typeof(t.proto->first(t)) n = t.proto->first(t);
  assert( n.proto->width(n) == sizeof(D));

  for(size_t i = 0; i < 10; ++i)
   {char c = '0'+i;
    const typeof(t.proto->node(t, &c, 1)) n = t.proto->node(t, &c, 1);
    n.proto->putTreeLast(n);
    n.proto->setData(n, D);
   }

  const typeof(t.proto->root(t)) root = t.proto->root(t);
  ArenaListEditablefe(n, root)
   {char d[sizeof(D)]; n.proto->getData(n, d);
    assert( !memcmp(d,        D, sizeof(D)));
    assert( !memcmp(n.proto->data(n), D, sizeof(D)));
   }

  assert( t.proto->printsWithBracketsAs(t, "(0123456789)"));
    t.proto->free(t);
 }

void test11()                                                                   //TcopyAndCompact //Tcopy //TcopyData
 {  const typeof(makeArenaListEditableWithWidth(sizeof(size_t))) s = makeArenaListEditableWithWidth(sizeof(size_t)); s.proto->fromLetters(s, "b(c(de)f)g");
    typeof(2ul) d = 2ul;
    const typeof(s.proto->findFirst(s, "e")) e = s.proto->findFirst(s, "e");
    e.proto->setData(e, &d);
    const typeof(e.proto->copy(e, s)) E = e.proto->copy(e, s);
  assert( e.proto->keyEquals(e, E.proto->key(E),  E.proto->length(E)));

    typeof(1ul) D = 1ul;
    E.proto->getData(E, &D);
  assert( D == d);

    d = 3ul;
    e.proto->setData(e, &d);
    E.proto->copyData(E, e);
    E.proto->getData(E, &D);
  assert( D == d);

    e.proto->unwrap(e);
    const typeof(s.proto->copyAndCompact(s)) t = s.proto->copyAndCompact(s);

  assert( s.proto->printsWithBracketsAs(s, "(b(c(d)f)g)"));
  assert( t.proto->printsWithBracketsAs(t, "(b(c(d)f)g)"));
  assert( t.proto->used(t) < s.proto->used(s));
    s.proto->free(s);
    t.proto->free(t);
 }

void test12()                                                                   //Treplace
 {  const typeof(makeArenaListEditableWithWidth(sizeof(size_t))) s = makeArenaListEditableWithWidth(sizeof(size_t)); s.proto->fromLetters(s, "b(c(d)e)f");
    const typeof(s.proto->findFirst(s, "d")) d = s.proto->findFirst(s, "d");
    const typeof(s.proto->node(s, "D", 1)) D = s.proto->node(s, "D", 1);
    D.proto->replace(D, d);
  assert( s.proto->printsWithBracketsAs(s, "(b(c(D)e)f)"));
    s.proto->free(s);
 }

void test13()
 {const typeof(makeArenaListEditable()) t = makeArenaListEditable();
  const typeof(t.proto->node(t, "a", 1)) a = t.proto->node(t, "a", 1);
  a.proto->putTreeLast(a);
#ifndef ArenaListEditableEditable
  assert( t.proto->used(t) == 49);
#else
  assert( t.proto->used(t) == 64);                                                             // More because the node gets rounded up
#endif
  a.proto->cut(a); a.proto->free(a);

  const typeof(t.proto->node(t, "b", 1)) b = t.proto->node(t, "b", 1);
  b.proto->putTreeLast(b);
#ifndef ArenaListEditableEditable
  assert( t.proto->used(t) == 74);
#else
  assert( t.proto->used(t) == 64);                                                             // Less due to reuse
#endif

  t.proto->free(t);
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0,  test1,  test2,  test3,  test4,
                           test5,  test6,  test7,  test8,  test9,
                           test10, test11, test12, test13, 0};
  run_tests("ArenaListEditable", repetitions, tests);

  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaList/arenaList
