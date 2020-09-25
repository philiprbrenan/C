#line 1 "/home/phil/c/z/arenaList/arenaList.c"
//------------------------------------------------------------------------------
// Lists held in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
/*
 A ArenaList contains nodes and keys. Each node can contain a list of sub nodes. Each
 node has a key and a length field which is used to store the data held by each
 node. The nodes are ordered solely by the user: the key field does not affect
 the ordering of the nodes. The key can contain \0 and other binary data as the
 length of the key field is determined solely by the length field.

*/
#define _GNU_SOURCE
#include <setjmp.h>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an ArenaList.
#ifndef ArenaList_included
#define ArenaList_included





typedef struct ArenaList                                                                // ArenaList
 {const struct ProtoTypes_ArenaList *proto;                                             // Methods associated with an ArenaList
  struct ArenaListArena *arena;                                                         // The arena for the ArenaList
 } ArenaList;

typedef struct ArenaListContent                                                         // The content of a node as stored in the ArenaList
 {unsigned int next, parent, first, last, prev, length;                         // Related nodes and the length of the key
  char         data[0];                                                         // The fixed data content of the node - the actual width of this field is held in arena.width
  char         key[0];                                                          // The content of the key
 } ArenaListContent;

typedef struct ArenaListNode                                                            // Offset to the content of a node in the ArenaList.
 {ArenaList      list;                                                                  // ArenaList containing the item
  size_t offset;                                                                // Offset
  const struct ProtoTypes_ArenaListNode *proto;                                         // Methods associated with nodes
 } ArenaListNode;

typedef struct ArenaListArena                                                           // Description of the block of memory that holds an ArenaList.  The individual nodes of the ArenaList cannot be cannot be freed individually, but the entire ArenaList can be freed. As the ArenaList uses offsets to store addresses the ArenaList can be moved to a different location in memory while retaining its structure.
 {size_t size;                                                                  // The total size of the arena used to hold data associated with the ArenaList.
  size_t used;                                                                  // The number of bytes currently used.
  size_t root;                                                                  // Offset to the root node
  size_t width;                                                                 // Width of a fixed data area added to each node measured in bytes
  void * data;                                                                  // The arena containing the data associated with the ArenaList
 } ArenaListArena;

typedef struct ArenaListDescription                                                     // The description of an ArenaList which is written as the header record for dump files.
 {const size_t version;                                                         // Version of ArenaList
  const size_t littleEndian;                                                    // Little endian if true
  const size_t s64b;                                                            // 64 bit address if true, else 32 bit
  const size_t used;                                                            // The number of bytes currently used by the arena.
  const size_t root;                                                            // Offset to the root node
  const size_t width;                                                           // Width of user data associated with each node
  const time_t created;                                                         // Time in seconds since the epoch when this ArenaList was created
 } ArenaListDescription;

#include <arenaList_prototypes.h>                                                      // ArenaList prototypes now that the relevant structures have been declared
#define ArenaListfe( child, parent) for(ArenaListNode child = parent.proto->first(parent); child.proto->valid(child); child = child.proto->next(child)) // Each child in a parent from first to last
#define ArenaListfer(child, parent) for(ArenaListNode child = parent.proto->last(parent) ; child.proto->valid(child); child = child.proto->prev(child)) // Each child in a parent from last to first
#define makeLocalCopyOfArenaListKey(string,stringLength,node) const size_t stringLength = content_ArenaListNode(node)->length; char string[stringLength+1]; string[stringLength] = 0; memcpy(string, key_pointer_ArenaListNode(node), stringLength); // Copy the key and the length of the key of the specified node to the stack.

//D1 Constructors                                                               // Construct a new ArenaList.

static ArenaList makeArenaListWithWidth                                                         // Create a new ArenaList with the specified width for the optional user data associated with each node.
 (const size_t width)                                                           // Width
 {ArenaListArena * const a = alloc(sizeof(ArenaListArena));                                     // Allocate arena description
  const typeof(newArenaList(({struct ArenaList t = {arena: a, proto: &ProtoTypes_ArenaList}; t;}))) t = newArenaList(({struct ArenaList t = {arena: a, proto: &ProtoTypes_ArenaList}; t;}));                                                          // ArenaList we are creating

  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  a->used   = 0;                                                                // Length used so far
  a->root   = 0;                                                                // Root not set in ArenaList
  a->width  = width;                                                            // Width of user data associated with each node
  t.proto->node(t, "", 0);                                                              // Initialize root node
  return t;
 }

static ArenaList makeArenaList                                                                  // Create a new ArenaList
 ()                                                                             // ArenaListallocator
 {return makeArenaListWithWidth(0);                                                     // Allocate arena description
 }

static void swap_ArenaList_ArenaList                                                            // Swap the arenas of two ArenaList
 (ArenaList source,                                                                     // Source ArenaList
  ArenaList target)                                                                     // Target ArenaList
 {const typeof(source.arena) s = source.arena; const typeof(target.arena) t = target.arena;                                           // Arenas
  ArenaListArena u = *t; *t = *s; *s = u;
 }

static char * check_ArenaList                                                           //P Return a string describing the structure
 (const ArenaList list)                                                                 // ArenaList
 {if (list.arena->data) {} return "ArenaList";
 }

//D1 Pointers and Offsets                                                       // Operations on pointers and offsets

static void * pointer_ArenaList_size                                                    //PV Return a temporary pointer to data in the arena of the specified ArenaList
 (const ArenaList      list,                                                            // ArenaList
  const size_t offset)                                                          // Offset
 {if (offset > list.arena->used)                                                // Check that the offset is valid
   {printStackBackTrace("Accessing area outside arena");
   }
  return (void *)(list.arena->data + offset);                                   // Convert a non zero delta that is within the arena to a valid pointer
 }

static ArenaListContent * content_ArenaListNode                                                 //PV Convert a node offset to an address so that the content of a node can be updated in situ as long as the ArenaList is not reallocated to a different position.
 (const ArenaListNode node)                                                             // Node
 {return (ArenaListContent *)pointer_ArenaList_size(node.list, node.offset);
 }

static size_t width_size_ArenaList                                                      // Get the width of the data area for a ArenaList
 (const ArenaList list)                                                                 // Node
 {return list.arena->width;
 }

static size_t width_size_ArenaListNode                                                  // Get the width of the data area for a node
 (const ArenaListNode node)                                                             // Node
 {return node.list.arena->width;
 }

static void * key_pointer_ArenaListNode                                                 //V Get a temporary pointer to the key of a node.
 (const ArenaListNode node)                                                             // Node
 {const typeof(node.proto->width(node)) width = node.proto->width(node);                                                         // Width of node
  return pointer_ArenaList_size(node.list, node.offset + sizeof(ArenaListContent) + width);     // The key is stored after the node and optional user data in the arena.
 }

static void * data_pointer_ArenaListNode                                                //V Get a temporary pointer to the user data of a node.
 (const ArenaListNode node)                                                             // Node
 {return pointer_ArenaList_size(node.list, node.offset + sizeof(ArenaListContent));             // The optional user data is stored immediately after the node in the arena.
 }

static size_t length_size_ArenaListNode                                                 // Get the length of the key associated with a node
 (const ArenaListNode node)                                                             // Node
 {return node.proto->content(node)->length;
 }

static  ArenaListNode  nodeFromOffset_ArenaList_size                                            //P Create a node to locate an allocation within the arena of a ArenaList.
 (const ArenaList      list,                                                            // ArenaList
  const size_t delta)                                                           // Delta within arena. A delta of zero represents no such node.
 {return newArenaListNode(({struct ArenaListNode t = {list: list, offset: delta, proto: &ProtoTypes_ArenaListNode}; t;}));
 }

//D1 Checks                                                                     // Check the validity of a node

static int keyEquals_int_ArenaListNode_pointer_size                                     // Confirm that the key of a node is equal to the specified memory
 (const ArenaListNode        node,                                                      // Node whose key is to be checked
  const char * const key,                                                       // Expected key value
  const size_t       length)                                                    // Length of expected key value
 {const typeof(node.proto->length(node)) l = node.proto->length(node);                                                            // Recorded length of node
  if (l != length) return 0;                                                    // Different lengths means the keys is unequal
  return !memcmp(node.proto->key(node), key, l);                                           // Compare memory
 }

static int equals_int_ArenaListNode_ArenaListNode                                               // Confirm two Nodes are equal
 (const ArenaListNode a,                                                                // First offset
  const ArenaListNode b)                                                                // Second offset
 {return a.list.arena == b.list.arena && a.offset == b.offset;
 }

//D1 Root node                                                                  // The root node of an ArenaList is at offset 0

static  ArenaListNode root_ArenaListNode_ArenaList                                                      // Return the root node of a ArenaList.
 (const ArenaList list)                                                                 // ArenaList
 {return newArenaListNode(({struct ArenaListNode t = {list: list, proto: &ProtoTypes_ArenaListNode}; t;}));                                                 // Root node
 }

static  ArenaListNode root_ArenaListNodeOffset_ArenaListNodeOffset                                      // Return the root node of the ArenaList containing the specified node.
 (const ArenaListNode node)                                                             // Node
 {return node.list.proto->root(node.list);
 }

//D1 Allocation                                                                 // Allocating memory in the ArenaList

static  ArenaListNode  allocate_offset_ArenaList_size                                           //P Allocate memory within the arena of a ArenaList and clear the allocated memory
 (const ArenaList      list,                                                            // ArenaList in which to allocate
  const size_t size)                                                            // Amount of memory required
 {if (list.arena->used + size < list.arena->size)                               // Allocate within existing arena
   {const typeof(list.proto->nodeFromOffset(list, list.arena->used)) n = list.proto->nodeFromOffset(list, list.arena->used);                                // Current offset to open memory
    list.arena->used += size;                                                   // Allocate
    return n;                                                                   // Return allocation
   }
  else                                                                          // Reallocate arena
   {const typeof(nextPowerOfTwo(list.arena->size + size)) S = nextPowerOfTwo(list.arena->size + size);                                // Round up memory required
    if (S < ((size_t)((1l<<(sizeof(((ArenaListContent *)0)->length)*8)) - 1)))          // The maximum size of a delta if there are 8 bits in a byte
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

static size_t used_ArenaList                                                            // Amount of space currently being used within the arena of a ArenaList.
 (const ArenaList list)                                                                 // ArenaList
 {return list.arena->used;
 }

static ArenaListNode node_ArenaListNode_ArenaList_string                                                // Create a new ArenaList node with the specified key.
 (const ArenaList            list,                                                      // ArenaList in which to create the node
  const void * const key,                                                       // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the ArenaList is determined solely by the user.
  const size_t       length)                                                    // Length of the key, or if zero, I will use strlen
 {const typeof(list.proto->allocate(list, sizeof(ArenaListContent) + length + list.proto->width(list))) n = list.proto->allocate(list, sizeof(ArenaListContent) + length + list.proto->width(list));                // Offset of space allocated for a node and key
  const typeof(n.proto->content(n)) c = n.proto->content(n);                                                              // Address content
  c->length = length;                                                           // Save key length
  memcpy(n.proto->key(n), key, length);                                                 // Copy in key
  assert(!memcmp(n.proto->key(n), key, length));
  makeLocalCopyOfArenaListKey(k, l, n);
//say("AAAA %lu %lu %lu %lu  %s\n", strlen(k), length, l, n.proto->width(n), n.proto->key(n));
    assert(l == length);
    assert(!memcmp(n.proto->key(n), key, length));
  return list.proto->nodeFromOffset(list, n.offset);                                       // Return node
 }

static void setKey_ArenaListNode_pointer                                                // Set the key of a node to a different value as long as there is enough room from the last setting.
 (const ArenaListNode        node,                                                      // Node
  const void * const key,                                                       // Key
  const size_t       length)                                                    // Length of key
 {const typeof(node.proto->length(node)) l = node.proto->length(node);                                                            // Length of existing key
  if (length <= l)                                                              // There is enough room in the existing key for the new key
   {memcpy(node.proto->key(node), key, length);                                            // Copy in the key
    node.proto->content(node) -> length = length;                                          // Set key length
    return;
   }
  printStackBackTraceAndExit(1, "Attempt to set longer key\n");                 // Error- no room for the key
 }

static void setData_ArenaListNode_pointer                                               // Set the optional user data associated with a node in an ArenaList
 (const ArenaListNode        node,                                                      // Node
  const void * const data)                                                      // Data
 {memcpy(node.proto->data(node), data, node.proto->width(node));                                      // Copy in the data
 }

static void getData_ArenaListNode_pointer                                               // get the optional user data associated with a node in an ArenaList
 (const ArenaListNode  node,                                                            // Node
  void * const data)                                                           // Data
 {memcpy(data, node.proto->data(node), node.proto->width(node));                                      // Copy out the data
 }

static void fromLetters_ArenaList_string                                                // Load ArenaList from a string of letters and brackets.  The root node of the ArenaList so constructed is always given the letter 'a'.
 (const ArenaList      list,                                                            // ArenaList
  const char * str)                                                             // String of letters and brackets describing desired list structure
 {const ArenaListNode n = list.proto->root(list);                                                  // The node we are currently  adding to

  void parse(ArenaListNode current)                                                     // Parse the string of letters and brackets to construct the desired ArenaList
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

  parse(n);                                                                     // Parse the string of letters and brackets to construct the desired ArenaList
 };

static void free_ArenaList                                                              // Free an entire ArenaList.
 (const ArenaList list)                                                                 // ArenaList to free
 {const typeof(list.arena) a = list.arena;
  if  (a->data) free(a->data);
  free(a);
 }

//D1 Navigation                                                                 // Navigate through a ArenaList.

static int valid_ArenaListNode                                                          // Check that a node is valid.
 (const ArenaListNode child)                                                            // Node
 {return child.offset;                                                          // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

static  ArenaListNode parent_ArenaListNode_ArenaListNode                                                // Get the parent of a child
 (const ArenaListNode child)                                                            // Child
 {if (child.proto->isRoot(child)) return child;
  return child.list.proto->nodeFromOffset(child.list, child.proto->content(child)->parent);
 }

static  ArenaListNode first_ArenaListNode_ArenaListNode                                                 // Get the first child under a parent.
 (const ArenaListNode parent)                                                           // Parent
 {return  parent.list.proto->nodeFromOffset(parent.list, parent.proto->content(parent)->first);
 }
static  ArenaListNode last_ArenaListNode_ArenaListNode                                                 // Get the last child under a parent.
 (const ArenaListNode parent)                                                           // Parent
 {return  parent.list.proto->nodeFromOffset(parent.list, parent.proto->content(parent)->last);
 }
#line 300 "/home/phil/c/z/arenaList/arenaList.c"
static  ArenaListNode next_ArenaListNode_ArenaListNode                                                 // Get the next child under a parent.
 (const ArenaListNode parent)                                                           // Parent
 {return  parent.list.proto->nodeFromOffset(parent.list, parent.proto->content(parent)->next);
 }
#line 300 "/home/phil/c/z/arenaList/arenaList.c"
static  ArenaListNode prev_ArenaListNode_ArenaListNode                                                 // Get the prev child under a parent.
 (const ArenaListNode parent)                                                           // Parent
 {return  parent.list.proto->nodeFromOffset(parent.list, parent.proto->content(parent)->prev);
 }
#line 300 "/home/phil/c/z/arenaList/arenaList.c"

static  ArenaListNode first_ArenaListNode_ArenaList                                                     // Get the first child in the specified ArenaList.
 (const ArenaList list)                                                                 // Parent
 {const ArenaListNode root = list.proto->root(list);
  return root.proto->first(root);
 }
static  ArenaListNode last_ArenaListNode_ArenaList                                                     // Get the last child in the specified ArenaList.
 (const ArenaList list)                                                                 // Parent
 {const ArenaListNode root = list.proto->root(list);
  return root.proto->last(root);
 }
#line 307 "/home/phil/c/z/arenaList/arenaList.c"
static  ArenaListNode next_ArenaListNode_ArenaList                                                     // Get the next child in the specified ArenaList.
 (const ArenaList list)                                                                 // Parent
 {const ArenaListNode root = list.proto->root(list);
  return root.proto->next(root);
 }
#line 307 "/home/phil/c/z/arenaList/arenaList.c"
static  ArenaListNode prev_ArenaListNode_ArenaList                                                     // Get the prev child in the specified ArenaList.
 (const ArenaList list)                                                                 // Parent
 {const ArenaListNode root = list.proto->root(list);
  return root.proto->prev(root);
 }
#line 307 "/home/phil/c/z/arenaList/arenaList.c"

//D1 Search                                                                     // Search for nodes.

static int equalsString_ArenaListNode_string                                            // Check that the key of a node equals a string
 (const ArenaListNode        node,                                                      // Node
  const char * const key)                                                       // Key
 {return node.proto->keyEquals(node, key, strlen(key));
 }

static  ArenaListNode findFirst_ArenaListNode_string                                            // Find the first node with the specified key in a post-order traversal of the ArenaList starting at the specified node.
 (const ArenaListNode        node,                                                      // Node at the start of the list to be searched
  const char * const key)                                                       // Key to find
 {jmp_buf found;
  ArenaListNode F = newArenaListNode(({struct ArenaListNode t = {list: node.list, proto: &ProtoTypes_ArenaListNode}; t;}));                                         // An invalid node

  void find(ArenaListNode node)                                                         // Check whether the key of the current node matches the specified key
   {if (node.proto->equalsString(node, key))                                               // Found
     {F.offset = node.offset;
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) node.proto->by(node, find);                                          // Search the ArenaList

  return F;                                                                     // Return an invalid node
 }

static  ArenaListNode findFirst_ArenaList_string                                                // Find the first node with the specified key in a post-order traversal of the ArenaList.
 (const ArenaList            list,                                                      // ArenaList
  const char * const key)                                                       // Key to find
 {jmp_buf found;
  ArenaListNode F = newArenaListNode(({struct ArenaListNode t = {list: list, proto: &ProtoTypes_ArenaListNode}; t;}));                                              // An invalid node

  void find(ArenaListNode node)                                                         // Check whether the key of the current node matches the specified key
   {if (node.proto->equalsString(node, key))                                               // Found
     {F.offset = node.offset;
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) list.proto->by(list, find);                                          // Search the ArenaList

  return F;                                                                     // Return an invalid node
 }

static  ArenaListNode findFirstChild_ArenaListNode_string                                       // Find the first child of the specified parent with the specified key.
 (const ArenaListNode        parent,                                                    // Parent node
  const char * const key)                                                       // Key to find immediately under parent
 {ArenaListfe(child, parent) if (child.proto->equalsString(child, key)) return child;               // Found matching child

  const ArenaListNode invalid = {};                                                     // An invalid node
  return invalid;                                                               // Failed - return an invalid node
 }

static  ArenaListNode findFirstChild_ArenaList_string                                           // Find the first child immediately under the root with the specified key.
 (const ArenaList            list,                                                      // ArenaList
  const char * const key)                                                       // Key to find
 {const ArenaListNode  r = list.proto->root(list);                                                 // Root node of the ArenaList
  return r.proto->findFirstChild(r, key);                                               // Search the ArenaList
 }

//D1 Location                                                                   // Verify the current location.

static int context_ArenaListNode                                                        // Return true if the parent of the specified child has the specified key.
 (const ArenaListNode         child,                                                    // Child
        ArenaListNode * const parent,                                                   // Parent container
  const char  * const key)                                                      // Key
 {if (child.proto->isRoot(child)) return 0;                                                 // The root node has no context
  const typeof(*parent = child.proto->parent(child)) p = *parent = child.proto->parent(child);
  makeLocalCopyOfArenaListKey(k, l, p);
  return p.proto->valid(p) && !memcmp(k, key, l);                                       // Check key
 }

static int isFirst_ArenaListNode                                                        // Confirm a child is first under its parent
 (const ArenaListNode child)                                                            // Child
 {const ArenaListNode parent = child.proto->parent(child);
  return child.proto->equals(child, parent.proto->first(parent));
 }
static int isLast_ArenaListNode                                                        // Confirm a child is last under its parent
 (const ArenaListNode child)                                                            // Child
 {const ArenaListNode parent = child.proto->parent(child);
  return child.proto->equals(child, parent.proto->last(parent));
 }
#line 386 "/home/phil/c/z/arenaList/arenaList.c"

static int isEmpty_ArenaListNode                                                        // Confirm a node has no children.
 (const ArenaListNode node)                                                             // Node
 {const ArenaListNode child = node.proto->first(node);
  return !child.proto->valid(child);                                                        // No first child so the node is empty
 }

static int isOnlyChild_ArenaListNode                                                    // Confirm that this child is the only child of its parent
 (const ArenaListNode child)                                                            // Child
 {const typeof(child.proto->parent(child)) parent = child.proto->parent(child);                                                      // Parent
  return parent.proto->valid(parent) && child.proto->isFirst(child) && child.proto->isLast(child);                   // Child is first and last and not the root so it is an only child
 }

static int isRoot_ArenaListNode                                                         // Confirm a node is the root
 (const ArenaListNode node)                                                             // NodeContent
 {return !node.offset;
 }

//D1 Put                                                                        // Insert children into a ArenaList.

static  ArenaListNode putFL_ArenaListNode_ArenaListNode_ArenaListNode                                           //P Put a child first or last under its parent
 (const int   first,                                                            // Put first if true, else last
  const ArenaListNode parent,                                                           // Parent
  const ArenaListNode child)                                                            // Child
 {const typeof(parent.list) list = parent.list;
  if (child.list.arena != list.arena) printStackBackTrace("Wrong list\n");      // Parent and child must be in same ArenaList
  const typeof(parent.offset) P = parent.offset;    const typeof(child.offset) C = child.offset;                                       // Delta of parent and child
  const typeof(parent.proto->content(parent)) p = parent.proto->content(parent); const typeof(child.proto->content(child)) c = child.proto->content(child);                                   // Pointers to parent and child
  const typeof(first ? p->first : p->last) L = first ? p->first : p->last;                                               // Delta of child currently first/last
  if (!L)                                                                       // No children yet
   {if (first) p->last = C; else p->first = C;                                  // Delta to last/first child
   }
  else                                                                          // Existing children
   {ArenaListContent * const l = list.proto->pointer(list, L);                                     // Link first/last two children together
    if (first) l->prev = C; else l->next = C;
    if (first) c->next = L; else c->prev = L;
   }
  c->parent = P;                                                                // Parent new first/last child
  if (first)  p->first = C; else p->last = C;                                   // Parent points to new first/last child
  return child;
 }

static  ArenaListNode putTreeFirst_ArenaListNode_ArenaListNode                                          // Put a child first in the ArenaList containing the arena in which the child was allocated.
 (const ArenaListNode child)                                                            // Child
 {const typeof(child.list) t = child.list;                                                               // ArenaList containing arena containing child
  const typeof(t.proto->root(t)) r = t.proto->root(t);
  return r.proto->putFirst(r, child);                                                   // Put the child first
 }
static  ArenaListNode putTreeLast_ArenaListNode_ArenaListNode                                          // Put a child last in the ArenaList containing the arena in which the child was allocated.
 (const ArenaListNode child)                                                            // Child
 {const typeof(child.list) t = child.list;                                                               // ArenaList containing arena containing child
  const typeof(t.proto->root(t)) r = t.proto->root(t);
  return r.proto->putLast(r, child);                                                   // Put the child last
 }
#line 435 "/home/phil/c/z/arenaList/arenaList.c"

static  ArenaListNode putFirst_ArenaListNode_ArenaListNode_ArenaListNode                                        // Put a child first under its parent
 (const ArenaListNode parent,                                                           // Parent
  const ArenaListNode child)                                                            // Child
 {return putFL_ArenaListNode_ArenaListNode_ArenaListNode(1, parent, child);                             // Put a child first under its parent
 }
static  ArenaListNode putLast_ArenaListNode_ArenaListNode_ArenaListNode                                        // Put a child last under its parent
 (const ArenaListNode parent,                                                           // Parent
  const ArenaListNode child)                                                            // Child
 {return putFL_ArenaListNode_ArenaListNode_ArenaListNode(0, parent, child);                             // Put a child last under its parent
 }
#line 442 "/home/phil/c/z/arenaList/arenaList.c"

static  ArenaListNode putNP_ArenaListNode_ArenaListNode_ArenaListNode                                           //P Put a child next or previous to the specified sibling
 (const int   next,                                                             // Put next if true, else previous
  const ArenaListNode sibling,                                                          // Sibling
  const ArenaListNode child)                                                            // Child
 {const typeof(sibling.list) list = sibling.list;                                                        // ArenaList
  const typeof(sibling.proto->parent(sibling)) parent = sibling.proto->parent(sibling);                                                    // Parent node
  const typeof(next ? sibling.proto->next(sibling) : sibling.proto->prev(sibling)) twin = next ? sibling.proto->next(sibling) : sibling.proto->prev(sibling);                              // Next or prev sibling to sibling is such a node exists
  assert(child.list.arena == list.arena);                                       // Parent and child must be in same ArenaList
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

static void setUp_ArenaListNode_ArenaListNode                                                   //P Make the specified parent node the parent of the specified child node
 (const ArenaListNode child,                                                            // Child
  const ArenaListNode parent)                                                           // Child
 {child.proto->content(child)->parent = parent.offset;                                      // Set parent of child
 }

static  ArenaListNode putNext_ArenaListNode_ArenaListNode_ArenaListNode                                         // Put a child next after the specified sibling
 (const ArenaListNode sibling,                                                          // Sibling
  const ArenaListNode child)                                                            // Child
 {return putNP_ArenaListNode_ArenaListNode_ArenaListNode(1, sibling, child);                            // Put child next
 }
static  ArenaListNode putPrev_ArenaListNode_ArenaListNode_ArenaListNode                                         // Put a child previous after the specified sibling
 (const ArenaListNode sibling,                                                          // Sibling
  const ArenaListNode child)                                                            // Child
 {return putNP_ArenaListNode_ArenaListNode_ArenaListNode(0, sibling, child);                            // Put child previous
 }
#line 486 "/home/phil/c/z/arenaList/arenaList.c"

//D1 Traverse                                                                   // Traverse a ArenaList.

static void by_ArenaListNode_sub                                                        // Traverse the ArenaList rooted at the specified node in post-order calling the specified function to process each child node.  The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
 (ArenaListNode node,                                                                   // Node
  void (* const function) (const ArenaListNode node))                                   // Function
 {void children(const ArenaListNode parent)                                             // Process the children of the specified parent
   {if (!parent.proto->valid(parent)) return;                                                // Empty child
    const typeof(parent.proto->countChildren(parent)) N = parent.proto->countChildren(parent);                                                 // Number of children
    size_t c[N+1];                                                              // Array of children terminated by a trailing zero
    size_t n = 0; ArenaListfe(child, parent) c[n++] = child.offset;                     // Load each child into an array
    for(size_t i = 0; i < N; ++i)                                               // Process each child allowing it to change its position without changing the traversal
     {children(newArenaListNode(({struct ArenaListNode t = {list: parent.list, offset: c[i], proto: &ProtoTypes_ArenaListNode}; t;})));
     }
    function(parent);                                                           // Process the parent
   }
  children(node);                                                               // Start at the specified root node
 }

static void by_ArenaList_sub                                                            // Traverse a ArenaList in post-order calling the specified function to process each child node.  The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
 (const ArenaList list,                                                                 // ArenaList
  void (* const function) (const ArenaListNode node))                                   // Function
 {const typeof(list.proto->root(list)) root = list.proto->root(list);                                                           // Root node
  const typeof(list.proto->countChildren(list)) N = list.proto->countChildren(list);                                                  // Number of children under root
  size_t c[N+1];                                                                // Array of children terminated by a trailing zero
  size_t n = 0; ArenaListfe(child, root) c[n++] = child.offset;                         // Load each child into an array
  for(size_t i = 0; i < N; ++i)
   {ArenaListNode n = newArenaListNode(({struct ArenaListNode t = {list: list, offset: c[i], proto: &ProtoTypes_ArenaListNode}; t;}));                              // Process each child allowing it to change position
    n.proto->by(n, function);
   }
  function(root);
 }

static  size_t countChildren_size_ArenaList                                             // Count the number of children directly under a parent.
 (const ArenaList list)                                                                 // ArenaList
 {size_t l = 0;
  const typeof(list.proto->root(list)) root = list.proto->root(list);
  ArenaListfe(child, root) ++l;
  return l;                                                                     // Return count
 }

static  size_t countChildren_size_ArenaListNode                                         // Count the number of children directly under a node.
 (const ArenaListNode parent)                                                           // Parent
 {size_t l = 0;
  ArenaListfe(child, parent) ++l;
  return l;                                                                     // Return count
 }

static  size_t count_size_ArenaListNode                                                 // Count the number of nodes under a specified node but not including the specified node.
 (const ArenaListNode node)                                                             // Node
 {size_t l = 0;

  void count(const ArenaListNode parent)                                                // Process the children of the specified parent
   {if (!parent.proto->valid(parent)) return;
    l++; ArenaListfe(child, parent) count(child);                                       // Each child
   }

  count(node);                                                                  // Start at the specified node
  return l - 1;                                                                 // Return count without counting the root node
 }


static  size_t count_size_ArenaList                                                     // Count the number of nodes in a ArenaList
 (const ArenaList list)                                                                 // ArenaList
 {size_t l = 0;

  const typeof(list.proto->root(list)) root = list.proto->root(list);                                                           // Root is always invalid
  ArenaListfe(child, root) l += 1 + child.proto->count(child);                                      // Child under root plus its children
  return l;                                                                     // Return count without counting the root node
 }

//D1 Print                                                                      // Print ArenaLists in various ways

static void printWithBrackets_ArenaListNode_function                                    // Apply a function to a string containing a print of the specified  node and the ArenaList below it in preorder as a string with each sub ArenaList enclosed in brackets.
 (const ArenaListNode   node,                                                           // Node
  void (*printer)(char * string, size_t length))                                // Function to apply to printed ArenaList
 {size_t l = 0;                                                                 // Length of output string
  void len(const ArenaListNode parent)                                                  // Find the size of buffer we will need
   {l += parent.proto->length(parent);                                                       // Parent key
    if (!parent.proto->isEmpty(parent))                                                      // Parent has children
     {l += 2;                                                                   // Space for brackets
      ArenaListfe(child, parent) len(child);                                            // Each child
     }
   }

  char * p;
  void print(const ArenaListNode parent)                                                // Print the children of the specified parent
   {makeLocalCopyOfArenaListKey(k, l, parent);                                          // Local copy of key
    memcpy(p, k, l); p += l;                                                    // Copy key into print string
    if (!parent.proto->isEmpty(parent))                                                      // Parent has children
     {*p++ = '(';                                                               // Start children
      ArenaListfe(child, parent) print(child);                                          // Each child
      *p++ = ')';                                                               // Start children
     }
    *p = 0;                                                                     // End the string
   }

  len(node); char P[l+1]; P[0] = 0; p = P; print(node);                         // Print in local string
  printer(P, l);                                                                // Apply function
 }

static void printWithBrackets_string_ArenaList                                          // Print an entire ArenaList in preorder as a string with brackets around each sub ArenaList
 (const ArenaList list,                                                                 // ArenaList
  void (*printer)(char * string, size_t length))                                // Function to apply to printed ArenaList
 {const typeof(list.proto->root(list)) node = list.proto->root(list);                                                           // Root
  node.proto->printWithBrackets(node, printer);
 }

static int printsWithBracketsAs_int_ArenaListNode_string                                // Check that the ArenaList starting at the specified node prints with brackets as expected.
 (const ArenaListNode        node,                                                      // Node
  const char * const expected)                                                  // Expected string when printed
 {int r;
  void printer(char * s, size_t l)
   {r = !strncmp(s, expected, l);
   }
  node.proto->printWithBrackets(node, printer);
  return r;
 }

static int printsWithBracketsAs_int_ArenaList_string                                    // Check that the specified ArenaList prints with brackets as expected.
 (const ArenaList            list,                                                      // ArenaList
  const char * const expected)                                                  // Expected string when printed
 {       const typeof(list.proto->root(list)) root = list.proto->root(list);                                                    // Root
  return root.proto->printsWithBracketsAs(root, expected);
 }

static void dump_ArenaList                                                              //P Dump a ArenaList on stderr
 (const ArenaList list)                                                                 // ArenaList
 {size_t n = 0;
  void print(const ArenaListNode parent, int depth)                                     // Print the children of the specified parent
   {makeLocalCopyOfArenaListKey(k, l, parent);                                          // Local copy of key
    say("%lu ", ++n);                                                           // Print key number
    for(int i = 0; i < depth; ++i) say("  ");                                   // Spacer
    say("%s\n", k);                                                             // Print key
    if (!parent.proto->isEmpty(parent)) ArenaListfe(child, parent) print(child, depth+1);            // Each child
   }

  const typeof(list.proto->root(list)) root = list.proto->root(list);                                                           // Root
  print(root, 0);
 }

static void dump_ArenaListNode                                                          //P Dump a ArenaListNode on stderr
 (const ArenaListNode node)                                                             // ArenaListNode
 {makeLocalCopyOfArenaListKey(k, l, node);                                              // Local copy of key
  say("%lu %s\n", l, k);                                                        // Print key number
 }

static void print_ArenaListNode_function                                                // Apply a function to the print of a ArenaList.
 (const ArenaListNode   node,                                                           // Node
  void (*printer)(char * string, size_t length))                                // Function to apply to printed ArenaList
 {size_t l = 0;                                                                 // Length of output string
  void len(const ArenaListNode parent)                                                  // Find the size of buffer we will need
   {l += parent.proto->length(parent);                                                       // Parent key
    if (!parent.proto->isEmpty(parent)) ArenaListfe(child, parent) len(child);                       // Each child
   }

  char * p;
  void print(const ArenaListNode parent)                                                // Print the children of the specified parent
   {makeLocalCopyOfArenaListKey(k, l, parent);                                          // Local copy of key
    memcpy(p, k, l); p += l;
    if (!parent.proto->isEmpty(parent)) ArenaListfe(child, parent) print(child);                     // Each child
    *p = 0;                                                                     // End the string
   }

  len(node); char P[l+1]; P[0] = 0; p = P; print(node);                         // Print in local string
  printer(P, l);                                                                // Apply function
 }

static int printsAs_int_ArenaListNode_string                                            // Check that the specified node prints as expected.
 (const ArenaListNode        node,                                                      // ArenaList
  const char * const expected)                                                  // Expected string when printed
 {int r;
  void printer(char * s, size_t l)
   {r = !strncmp(s, expected, l);
   }
  node.proto->print(node, printer);
  return r;
 }

static int printsAs_int_ArenaList_string                                                // Check that the specified ArenaList prints as expected.
 (const ArenaList            list,                                                      // ArenaList
  const char * const expected)                                                  // Expected string when printed
 {int r;
  void printer(char * s, size_t l)
   {r = !strncmp(s, expected, l);
   }
  const typeof(list.proto->root(list)) root = list.proto->root(list);                                                           // Print from root of ArenaList
  root.proto->print(root, printer);
  return r;
 }

static int printContains_ArenaListNode                                                  // Check the print of an ArenaList starting at the specified tag contains the expected string.
 (const ArenaListNode   node,                                                           // Starting node
  const char *  expected)                                                       // Expected string
 {int r;
  void printer(char * s, size_t l)
   {r = !!strstr(s, expected); if(0)l=l;
   }
  node.proto->print(node, printer);
  return r;
 }

static int printContains_ArenaList                                                      // Check the print of an ArenaList contains the expected string.
 (const ArenaList       list,                                                           // ArenaList parse ArenaList
  const char *  expected)                                                       // Expected string
 {int r;
  void printer(char * s, size_t l)
   {r = !!strstr(s, expected); if(0)l=l;
   }
  const typeof(list.proto->root(list)) root = list.proto->root(list);                                                           // Print from root of ArenaList
  root.proto->print(root, printer);
  return r;
 }

//D1 Edit                                                                       // Edit a ArenaList in situ.

static  ArenaListNode cut_ArenaListNode_ArenaListNode                                                   // Cut out a child.
 (const ArenaListNode child)                                                            // Child to cut out
 {const ArenaListNode parent = child.proto->parent(child);                                         // Parent
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

static  ArenaListNode unwrap_ArenaListNode_ArenaListNode                                                // Unwrap the specified parent and return it.
 (const ArenaListNode parent)                                                           // Parent to unwrap
 {for(ArenaListNode child = newArenaListNode(({struct ArenaListNode t = {list: parent.list, offset: parent.proto->last(parent).offset, proto: &ProtoTypes_ArenaListNode}; t;})); // Remove last children
    child.proto->valid(child);
    child.offset = parent.proto->last(parent).offset)
   {parent.proto->putNext(parent, child.proto->cut(child));                                              // Place each child after the parent
   }
  return parent.proto->cut(parent);                                                          // Remove and return empty parent
 }

static  ArenaListNode wrap_ArenaListNode_string                                                 // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
 (const ArenaListNode   child,                                                          // Child to wrap
  const char * const key)                                                       // Key for new parent
 {const typeof(child.list) list = child.list;                                                          // Tree
  const typeof(list.proto->node(list, key, strlen(key))) parent = list.proto->node(list, key, strlen(key));                                       // New parent
  child.proto->putNext(child, parent);                                                     // Place parent after child
  parent.proto->putLast(parent, child.proto->cut(child));                                                // Place child under parent
  return parent;
 }

//D1 Input and Output                                                           // Read and write a ArenaList from/to a file

static void write_void_ArenaList_string                                                 // Write a ArenaList to a named file or abort
 (const ArenaList       list,                                                           // ArenaList
  const char * const file)                                                      // File
 {    const typeof(open(file, O_CREAT| O_WRONLY, S_IRWXU)) o = open(file, O_CREAT| O_WRONLY, S_IRWXU);                               // Open for output creating if needed
  if (o < 0) printStackBackTrace("Cannot open file: %s for write\n", file);

  time_t current; time(&current);
  const ArenaListDescription h =                                                        // Create ArenaList header describing the ArenaList
   {1, 1, 0, list.proto->used(list), list.proto->root(list).offset, list.proto->width(list), current};

  if (sizeof(h) != write(o, &h, sizeof(h)))                                     // Write header
   {printStackBackTrace("Cannot write ArenaList header to file: %s\n", file);
   }

  const typeof(write(o, list.arena->data, list.arena->used)) w = write(o, list.arena->data, list.arena->used);                             // Write arena
  if (w < 0 || list.arena->used != (size_t)w)
   {printStackBackTrace("Cannot write ArenaList arena to file: %s\n", file);
   }

  close(o);
 }

ArenaList readArenaList                                                                         // Read a ArenaList from a file
 (const char * const file)                                                      // File
 {ArenaListArena * const arena = alloc(sizeof(ArenaListArena));                                 // Create arena
  const typeof(newArenaList(({struct ArenaList t = {arena: arena, proto: &ProtoTypes_ArenaList}; t;}))) list = newArenaList(({struct ArenaList t = {arena: arena, proto: &ProtoTypes_ArenaList}; t;}));                                                   // Initialize ArenaList

      const typeof(open(file, 0, O_RDONLY)) i = open(file, 0, O_RDONLY);                                              // Open for input
  if (i < 0) printStackBackTrace("Cannot open file: %s for read\n", file);

  struct ArenaListDescription h;
  if (sizeof(h) != read(i, &h, sizeof(h)))                                      // Read header
   {printStackBackTrace("Cannot read header from file: %s\n", file);
   }

  list.arena->data = alloc(arena->size = arena->used = h.used);                 // Allocate arena

      const typeof(read(i, arena->data, arena->used)) r = read(i, arena->data, arena->used);                                    // Read arena
  if (r < 0 || arena->used != (size_t)r)
   {printStackBackTrace("Cannot read ArenaList from file: %s\n", file);
   }

  close(i);
  arena->root = h.root; arena->width = h.width;                                 // Offset to root in arena
  return list;
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TmakeArenaList //Tnode //Tfree //TputFirst //TputLast //Tfe //Tfer
 {   const typeof(makeArenaList()) t = makeArenaList();                                                               // Create a ArenaList
  const typeof(t.proto->root(t)) root = t.proto->root(t);

  for(int i = 0; i < 10; ++i)                                                   // Load ArenaList
   {char c = 'a'+i;

    const typeof(t.proto->node(t, &c, 1)) c1 = t.proto->node(t, &c, 1); root.proto->putLast(root, c1);
    const typeof(t.proto->node(t, &c, 1)) c2 = t.proto->node(t, &c, 1); root.proto->putFirst(root, c2);
    c = 'A'+i; c2.proto->setKey(c2, &c, 1);
    makeLocalCopyOfArenaListKey(k2, l2, c2);
    assert(k2[0] == c);
   }

  if (1)                                                                        // For each
   {char l[t.proto->count(t) + 1]; *l = 0;
    ArenaListfe (child, root) strncat(l, child.proto->key(child), child.proto->length(child));

    assert(strcmp(l, "JIHGFEDCBAabcdefghij") == 0);
   }

  if (1)                                                                        // For each in reverse
   {char l[t.proto->count(t) + 1]; *l = 0;
    ArenaListfer(child, root) strncat(l, child.proto->key(child), child.proto->length(child));
    assert(strcmp(l, "jihgfedcbaABCDEFGHIJ") == 0);
   }

  t.proto->free(t);
 }

void test1()                                                                    //Troot //Tfirst //Tlast //Tnext //Tprev //Tparent //Tequals //Tprint //TprintWithBrackets //TfromLetters //TprintsAs
 {const typeof(makeArenaList()) t = makeArenaList();    t.proto->fromLetters(t, "b(c(de)f)g(hi)j");
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

void test2()                                                                    //Tby //TprintsWithBracketsAs //TprintContains
 {const typeof(makeArenaList()) t = makeArenaList();     t.proto->fromLetters(t, "b(c(de)f)g(hi)j");
  assert(t.proto->printsWithBracketsAs(t, "(b(c(de)f)g(hi)j)"));
  assert(t.proto->printContains(t, "def"));

  char l[t.proto->count(t) + 2], *p = l;

  void process(ArenaListNode n)
   {makeLocalCopyOfArenaListKey(k, l, n);
    strncpy(p, k, l); p += l; *p = 0;
   }

  t.proto->by(t, process);
  assert(strcmp(l, "decfbhigj") == 0);

  t.proto->free(t);
 }

void test3()                                                                    //TisFirst //TisLast //TisEmpty //TisRoot //TisOnlyChild //Tcontext //TcheckKey //Tvalid
 {const typeof(makeArenaList()) t = makeArenaList();     t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
  assert(t.proto->printsWithBracketsAs(t, "(b(c(de(f)gh)i)j)"));

  const typeof(t.proto->root(t)) a = t.proto->root(t);  assert(a.proto->equalsString(a, ""));                                  assert(a.proto->printsWithBracketsAs(a,  "(b(c(de(f)gh)i)j)"));
  const typeof(a.proto->first(a)) b = a.proto->first(a); assert(b.proto->equalsString(b, "b")); assert(a.proto->equals(a, b.proto->parent(b))); assert(b.proto->printsWithBracketsAs(b,  "b(c(de(f)gh)i)"));
  const typeof(b.proto->first(b)) c = b.proto->first(b); assert(c.proto->equalsString(c, "c")); assert(b.proto->equals(b, c.proto->parent(c))); assert(c.proto->printsWithBracketsAs(c,    "c(de(f)gh)"));
  const typeof(c.proto->first(c)) d = c.proto->first(c); assert(d.proto->equalsString(d, "d")); assert(c.proto->equals(c, d.proto->parent(d))); assert(d.proto->printsWithBracketsAs(d,      "d"));
  const typeof(d.proto->next(d)) e = d.proto->next(d);  assert(e.proto->equalsString(e, "e")); assert(c.proto->equals(c, e.proto->parent(e))); assert(e.proto->printsWithBracketsAs(e,       "e(f)"));
  const typeof(e.proto->last(e)) f = e.proto->last(e);  assert(f.proto->equalsString(f, "f")); assert(e.proto->equals(e, f.proto->parent(f))); assert(f.proto->printsWithBracketsAs(f,         "f"));
  const typeof(e.proto->next(e)) g = e.proto->next(e);  assert(g.proto->equalsString(g, "g")); assert(c.proto->equals(c, g.proto->parent(g))); assert(g.proto->printsWithBracketsAs(g,           "g"));
  const typeof(g.proto->next(g)) h = g.proto->next(g);  assert(h.proto->equalsString(h, "h")); assert(c.proto->equals(c, h.proto->parent(h))); assert(h.proto->printsWithBracketsAs(h,            "h"));
  const typeof(c.proto->next(c)) i = c.proto->next(c);  assert(i.proto->equalsString(i, "i")); assert(b.proto->equals(b, i.proto->parent(i))); assert(i.proto->printsWithBracketsAs(i,              "i"));
  const typeof(b.proto->next(b)) j = b.proto->next(b);  assert(j.proto->equalsString(j, "j")); assert(a.proto->equals(a, j.proto->parent(j))); assert(j.proto->printsWithBracketsAs(j,                "j"));

  assert(!a.proto->valid(a));
  assert( b.proto->isFirst(b));
  assert( j.proto->isLast(j));
  assert( f.proto->isFirst(f));
  assert( f.proto->isLast(f));

  assert( f.proto->isEmpty(f));
  assert( f.proto->isOnlyChild(f));
  assert(!e.proto->isOnlyChild(e));
  assert( a.proto->isRoot(a));

  ArenaListNode A, C, E;
  assert( f.proto->context(f, &E, "e"));
  assert( E.proto->context(E, &C, "c"));
  assert(!a.proto->context(a, &A, "a"));

  t.proto->free(t);
 }

void test4()                                                                    //Tcut //TfindFirst //TcountChildren //TequalsString //TfindFirstChild
 {const typeof(makeArenaList()) t = makeArenaList();     t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
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
 {const typeof(makeArenaList()) t = makeArenaList();     t.proto->fromLetters(t, "b(c(de(f)gh)i)j");
  assert(t.proto->printsWithBracketsAs(t, "(b(c(de(f)gh)i)j)"));

         const typeof("/tmp/arenaTreeTest.data") f = "/tmp/arenaTreeTest.data";
  assert(t.proto->printsWithBracketsAs(t, "(b(c(de(f)gh)i)j)"));
         t.proto->write(t, f);

         const typeof(readArenaList(f)) u = readArenaList(f);
  assert(u.proto->printsWithBracketsAs(u, "(b(c(de(f)gh)i)j)"));

  t.proto->free(t);
  u.proto->free(u);
  unlink(f);
 }

void test6()                                                                    //Tunwrap //Twrap //Tcount
 {const typeof(makeArenaList()) t = makeArenaList();     t.proto->fromLetters(t, "bce");
  assert(t.proto->printsWithBracketsAs(t, "(bce)"));
  assert(t.proto->count(t) == 3);

         const typeof(t.proto->findFirst(t, "c")) c = t.proto->findFirst(t, "c");
  assert(c.proto->valid(c));

     const typeof(c.proto->wrap(c, "d")) d = c.proto->wrap(c, "d");
  assert(t.proto->printsWithBracketsAs(t, "(bd(c)e)"));
  assert(t.proto->count(t) == 4);

         d.proto->unwrap(d);
  assert(t.proto->printsWithBracketsAs(t, "(bce)"));
  assert(t.proto->count(t) == 3);
         t.proto->free(t);
 }

void test7()                                                                    //TputNext //TputPrev
 {const typeof(makeArenaList()) t = makeArenaList();    t.proto->fromLetters(t, "A");
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
 {const typeof(makeArenaList()) t = makeArenaList();

  const typeof(t.proto->node(t, "c", 1)) c = t.proto->node(t, "c", 1); c.proto->putTreeFirst(c);
  const typeof(t.proto->node(t, "d", 1)) d = t.proto->node(t, "d", 1); d.proto->putTreeLast(d);
  const typeof(t.proto->node(t, "b", 1)) b = t.proto->node(t, "b", 1); b.proto->putTreeFirst(b);

  b.proto->setKey(b, "B", 1);
  assert( b.proto->length(b) == 1);
  assert(*(char *)(b.proto->key(b)) == 'B');
  assert(b.proto->keyEquals(b, "B", 1));

  assert(t.proto->used(t) == 99);

  assert(t.proto->printsWithBracketsAs(t, "(Bcd)"));

  t.proto->free(t);
 }

void test9()                                                                    //Tswap
 {const typeof(makeArenaList()) s = makeArenaList(); s.proto->fromLetters(s, "s");
  const typeof(makeArenaList()) t = makeArenaList(); t.proto->fromLetters(t, "t");
  assert(s.proto->printsWithBracketsAs(s, "(s)"));
  assert(t.proto->printsWithBracketsAs(t, "(t)"));
  s.proto->swap(s, t);
  assert(t.proto->printsWithBracketsAs(t, "(s)"));
  assert(s.proto->printsWithBracketsAs(s, "(t)"));
  s.proto->free(s);
  t.proto->free(t);
 }

void test10()                                                                   //Tdata //TmakeArenaListWithWidth //Twidth //TgetData //TsetData
 {size_t D[] = {0,1,2,3};
  const typeof(makeArenaListWithWidth (sizeof(D))) t = makeArenaListWithWidth (sizeof(D));
  assert(t.proto->width(t) == sizeof(D));

  const typeof(t.proto->first(t)) n = t.proto->first(t);
  assert(n.proto->width(n) == sizeof(D));

  for(size_t i = 0; i < 10; ++i)
   {char c = '0'+i;
    const typeof(t.proto->node(t, &c, 1)) n = t.proto->node(t, &c, 1);
    n.proto->putTreeLast(n);
    n.proto->setData(n, D);
   }

  const typeof(t.proto->root(t)) root = t.proto->root(t);
  ArenaListfe(n, root)
   {char d[sizeof(D)]; n.proto->getData(n, d);
    assert(!memcmp(d,        D, sizeof(D)));
    assert(!memcmp(n.proto->data(n), D, sizeof(D)));
   }

  assert(t.proto->printsWithBracketsAs(t, "(0123456789)"));
  t.proto->free(t);
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0,  test1,  test2,  test3,  test4,
                           test5,  test6,  test7,  test8,  test9,
                           test10, 0};
  run_tests("ArenaList", repetitions, tests);

  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaList/arenaList
