#line 1 "/home/phil/c/z/arenaList/arenaList.c"
//------------------------------------------------------------------------------
// List of lists held in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
/*
 A ArenaList contains nodes and keys. Each node can contain a list of sub nodes. Each
 node has a key and a length field which is used to store the data held by each
 node. The nodes are ordered solely by the user: the key field does not affect
 the ordering of the nodes. The key can contain \0 and other binary data as the
 length of the key field is determined solely by the length field.

 Nodes can be freed and recycled - recycling will eventually fragment all of
 the memory in the arena, at which point the ArenaList should be compacted. Compaction
 can be carried out in parallel in a separate process as a ArenaList is relocatable.
*/
#define _GNU_SOURCE
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <setjmp.h>
#include <arenaArray.c>
#include <utilities.c>
#define ArenaListEditable

//D1 Structures                                                                 // Structures describing an ArenaList.
#ifndef ArenaList_included
#define ArenaList_included





typedef struct ArenaList                                                                // ArenaList
 {const struct ProtoTypes_ArenaList *proto;                                             // Methods associated with an ArenaList
  struct ArenaListArena *arena;                                                         // The arena for the ArenaList
 } ArenaList;

typedef struct ArenaListContent                                                         // The content of a node as stored in the ArenaList
 {unsigned int next, parent, first, last, prev, length;                         // Related nodes and the length of the key
#ifdef ArenaListEditable
  unsigned char size;                                                           // The size of the node including the key as the exponent of a power of two
#endif
  char          data[0];                                                        // The fixed data content of the node - the actual width of this field is held in arena.width
  char          key [0];                                                        // The content of the key
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
  size_t width;                                                                 // Width of a fixed data area, measured in bytes, added to each node
  void * data;                                                                  // The arena containing the data associated with the ArenaList
#ifdef ArenaListEditable
  size_t freeSpace[8*sizeof(unsigned int)];                                     // Lists of freed nodes by exponent of power of two of node size.
#endif
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

typedef struct ArenaListPosition                                                        // The position of a child in nodes and bytes in the ArenaList
 {const size_t node;                                                            // Node count
  const size_t byte;                                                            // Byte count from start of ArenaList
  const size_t depth;                                                           // Depth of the node
  const struct ProtoTypes_ArenaListPosition *proto;                                     // Methods associated with a position
 } ArenaListPosition;

typedef struct ArenaListNodeAndState                                                    // Replace recursive processing with linear processing
 {ArenaListNode  node;                                                                  // Node
  size_t count;                                                                 // Number of children
  int    state;                                                                 // Processing state
 } ArenaListNodeAndState;

#include <arenaList_prototypes.h>                                                      // ArenaList prototypes now that the relevant structures have been declared
#define ArenaListfe( child, parent) for(typeof(parent.proto->first(&parent)) child = parent.proto->first(&parent); child.proto->valid(&child); child = child.proto->next(&child)) // Each child under a parent from first to last
//#define ArenaListFe( child, list)   for(typeof(list.proto->first(&list)) child = list.proto->first(&list); child.proto->valid(&child); child = child.proto->next(&child)) // Each child under the root node of a ArenaList from first to last
#define ArenaListfr(child, parent) for(typeof(parent.proto->last(&parent) ) child = parent.proto->last(&parent) ; child.proto->valid(&child); child = child.proto->prev(&child)) // Each child under a parent from last to first
//#define ArenaListFer(child, list)   for(typeof(list.proto->last(&list)) child = list.proto->last(&list); child.proto->valid(&child); child = child.proto->prev(&child)) // Each child under the root node of a ArenaList from last to first
#define ArenaListfⁱ(child, parent) size_t child##ⁱ = 1; for(typeof(parent.proto->first(&parent)) child = parent.proto->first(&parent); child.proto->valid(&child); child = child.proto->next(&child), ++child##ⁱ) // Each child under a parent from first to last with a counter
#define makeLocalCopyOfArenaListKey(string,stringLength,node) const typeof(content__ArenaListNode(&node)->length) stringLength = content__ArenaListNode(&node)->length; char string[stringLength+1]; string[stringLength] = 0; memcpy(string, key_pointer__ArenaListNode(&node), stringLength); // Copy the key and the length of the key of the specified node to the stack.

//D1 Constructors                                                               // Construct a new ArenaList.

static ArenaList makeArenaListWithWidth                                                         // Create a new ArenaList with the specified width for the optional user data associated with each node.
 (const size_t width)                                                           // Width
 {ArenaListArena * a = alloc(sizeof(ArenaListArena));                                           // Allocate arena description
  memset(a, 0, sizeof(ArenaListArena));                                                 // Clear arena description
  const typeof(newArenaList(({struct ArenaList t = {arena: a, proto: &ProtoTypes_ArenaList}; t;}))) t = newArenaList(({struct ArenaList t = {arena: a, proto: &ProtoTypes_ArenaList}; t;}));                                                          // ArenaList we are creating

  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  a->used   = 0;                                                                // Length used so far
  a->root   = 0;                                                                // Root not set in ArenaList
  a->width  = width;                                                            // Width of user data associated with each node
  t.proto->node(&t, "", 0);                                                              // Initialize root node
  return t;
 }

static ArenaList makeArenaList                                                                  // Create a new ArenaList
 ()                                                                             // ArenaListallocator
 {return makeArenaListWithWidth(0);                                                     // Allocate arena description
 }

static void swap_ArenaList_ArenaList                                                            // Swap the arenas of two ArenaList
 (ArenaList *source,                                                                    // Source ArenaList
  ArenaList  target)                                                                    // Target ArenaList
 {const typeof(source->arena) s = source->arena; const typeof(target.arena) t = target.arena;                                          // Arenas
  ArenaListArena u = *t; *t = *s; *s = u;
 }

static ArenaList makeArenaListFromLines                                                         // Make a ArenaList from a string of lines.
 (const char * str)                                                             // String of lines separated by new lines
 {const typeof(makeArenaList()) l = makeArenaList();

  const char * p = str, *q = p;                                                 // Parse the string into new lines
  typeof(0ul) B = 0ul;

  void save()                                                                   // Save a line
   {if (B && p > q) {const typeof(l.proto->node(&l, q, p - q)) n = l.proto->node(&l, q, p - q); n.proto->putTreeLast(&n); B = 0;}
    q = p + 1;
   }

  for(; *p; ++p) if (*p == '\n') save(); else if (!isspace(*p)) ++B;
  save();

  return l;
 };

static ArenaList makeArenaListFromWords                                                         // Make a ArenaList from a string of words.
 (const char * str)                                                             // String of words separated by spaces;
 {const typeof(makeArenaList()) l = makeArenaList();

  const char * p = str, *q = p;                                                 // Parse the string into new lines

  void save()                                                                   // Save a line
   {if (p > q) {const typeof(l.proto->node(&l, q, p - q)) n = l.proto->node(&l, q, p - q); n.proto->putTreeLast(&n);}
    q = p + 1;
   }

  for(; *p; ++p) if (isspace(*p)) save();                                       // Break into words
  save();

  return l;
 };

static ArenaList makeArenaListFromLinesOfWords                                                  // Make a ArenaList of lists each sub list representing a line as a list of words.
 (const char * str)                                                             // String of lines of words separated by spaces;
 {const typeof(makeArenaListFromLines(str)) list = makeArenaListFromLines(str);                                                   // Load each line

  ArenaListfe(line, list)                                                               // Each line
   {makeLocalCopyOfArenaListKey(s, l, line);

    const char * p = s, *q = p;                                                 // Parse the string into new lines

    void save()                                                                 // Save a line
     {if (p > q) {const typeof(list.proto->node(&list, q, p - q)) n = list.proto->node(&list, q, p - q); line.proto->putLast(&line, n);}
      q = p + 1;
     }

    for(; *p; ++p) if (isspace(*p)) save();                                     // Break into words
    save();
   }

  return list;
 };

static char * check_string_ArenaList                                                    //P Return a string describing the structure
 (const ArenaList list)                                                                 // ArenaList
 {if (list.arena->data) {} return "ArenaList";
 }

//D1 Pointers and Offsets                                                       // Operations on pointers and offsets

static void * pointer__ArenaList_size                                                   //PV Return a temporary pointer to data in the arena of the specified ArenaList
 (const ArenaList   *  list,                                                            // ArenaList
  const size_t offset)                                                          // Offset
 {if (offset >= list->arena->used)                                              // Check that the offset is valid
   {printStackBackTraceAndExit(1, "Accessing area outside arena");
   }
  return (void *)(list->arena->data + offset);                                  // Convert a non zero delta that is within the arena to a valid pointer
 }

static ArenaListContent * content__ArenaListNode                                                //PV Convert a node offset to an address so that the content of a node can be updated in situ as long as the ArenaList is not reallocated to a different position.
 (const ArenaListNode   * node)                                                         // ArenaListNode
 {return (ArenaListContent *)pointer__ArenaList_size(&node->list, node->offset);
 }

static size_t width_size__ArenaList                                                     // Get the width of the data area in a node for a ArenaList
 (const ArenaList * list)                                                               // ArenaList
 {return list->arena->width;
 }

static size_t width_size__ArenaListNode                                                 // Get the width of the data area for a node
 (const ArenaListNode * node)                                                           // ArenaListNode
 {return node->list.arena->width;
 }

static char * key_pointer__ArenaListNode                                                //V Get a temporary pointer to the key of a node.
 (const ArenaListNode * node)                                                           // ArenaListNode
 {const typeof(node->proto->width(node)) width = node->proto->width(node);                                                         // Width of node
  return pointer__ArenaList_size(&node->list, node->offset + sizeof(ArenaListContent) + width); // The key is stored after the node and optional user data in the arena.
 }

static void   * data_pointer__ArenaListNode                                             //V Get a temporary pointer to the user data of a node.
 (const ArenaListNode * node)                                                           // ArenaListNode
 {return pointer__ArenaList_size(&node->list, node->offset + sizeof(ArenaListContent));         // The optional user data is stored immediately after the node in the arena.
 }

static size_t   length_size__ArenaListNode                                              // Get the length of the key associated with a node
 (const ArenaListNode * node)                                                           // ArenaListNode
 {return node->proto->content(node)->length;
 }

static size_t   maxLength_size__ArenaListNode                                           // Get the maximum allowed length of the key associated with a node.
 (const ArenaListNode * node)                                                           // ArenaListNode
#ifndef ArenaListEditable
 {return node->proto->length(node);                                                         // Nodes in not editable ArenaList are allocated just large enough to hold the key
 }
#else
 {return (1ul << node->proto->content(node)->size) - sizeof(ArenaListNode)- node->list.arena->width;
 }
#endif

static  ArenaListNode  offset__ArenaList_size                                                   //P Create a node to locate an allocation within the arena of a ArenaList.
 (const ArenaList    * list,                                                            // ArenaList
  const size_t delta)                                                           // Delta within arena. A delta of zero represents no such node.
 {return newArenaListNode(({struct ArenaListNode t = {list: *list, offset: delta, proto: &ProtoTypes_ArenaListNode}; t;}));
 }

//D1 Checks                                                                     // Check the validity of a node

static int keyEquals_int__ArenaListNode_pointer_size                                    // Confirm that the key of a node is equal to the specified memory
 (const ArenaListNode * node,                                                           // ArenaListNode whose key is to be checked
  const char  * const key,                                                      // Expected key value
  const size_t       length)                                                    // Length of expected key value
 {const typeof(node->proto->length(node)) l = node->proto->length(node);                                                            // Recorded length of node
  if (l != length) return 0;                                                    // Different lengths means the keys is unequal
  return !memcmp(node->proto->key(node), key, l);                                           // Compare memory
 }

static int keyEqualsString_int__ArenaListNode_pointer                                   // Confirm that the key of a node is equal to the specified zero terminated string
 (const ArenaListNode * node,                                                           // ArenaListNode whose key is to be checked
  const char  * const key)                                                      // Expected key value
 {return node->proto->keyEquals(node, key, strlen(key));                                    // Check key
 }

static int equals_int__ArenaListNode_ArenaListNode                                              // Confirm two nodes are equal
 (const ArenaListNode * a,                                                              // First offset
  const ArenaListNode   b)                                                              // Second offset
 {return a->list.arena == b.list.arena && a->offset == b.offset;
 }

static int notEquals_int__ArenaListNode_ArenaListNode                                           // Confirm two nodes are not equal
 (const ArenaListNode * a,                                                              // First offset
  const ArenaListNode   b)                                                              // Second offset
 {return !a->proto->equals(a, b);
 }

static int equalsPosition_int__ArenaListPosition_ArenaListPosition                              // Confirm two positions are equal
 (const ArenaListPosition * a,                                                          // First position
  const ArenaListPosition   b)                                                          // Second position
 {return a->byte == b.byte && a->node == b.node && a->depth == b.depth;
 }

static int equals_int__ArenaList_ArenaList                                                      // Confirm two ArenaList are equal by comparing their prints
 (const ArenaList * a,                                                                  // First ArenaList
  const ArenaList   b)                                                                  // Second ArenaList
 {int r;
  void printer(char * s, size_t l)
   {void printer(char * t, size_t m) {r = l == m && !memcmp(s, t, l);}          // Compare prints
    const typeof(b.proto->root(&b)) B = b.proto->root(&b);
    B.proto->print(&B, printer);
   }
  const typeof(a->proto->root(a)) A = a->proto->root(a);
  A.proto->print(&A, printer);
  return r;
 }

static int notEquals_int__ArenaList_ArenaList                                                   // Confirm two ArenaList are not equal by comparing their prints
 (const ArenaList * a,                                                                  // First ArenaList
  const ArenaList   b)                                                                  // Second ArenaList
 {return !a->proto->equals(a, b);
 }

//D1 Root node                                                                  // The root node of an ArenaList is at offset 0

static  ArenaListNode root_ArenaListNode__ArenaList                                                     // Return the root node of a ArenaList.
 (const ArenaList *   list)                                                             // ArenaList
 {return newArenaListNode(({struct ArenaListNode t = {list: *list, proto: &ProtoTypes_ArenaListNode}; t;}));                                                // Root node
 }

static  ArenaListNode root_ArenaListNode__ArenaListNode                                                 // Return the root node of the ArenaList containing the specified node.
 (const ArenaListNode node)                                                             // ArenaListNode
 {return node.list.proto->root(&node.list);
 }

//D1 Allocation                                                                 // Allocating memory in the ArenaList

static  ArenaListNode  allocate_ArenaListNode__ArenaList_size                                           //P Allocate a node within the arena of a ArenaList
 (const ArenaList    * list,                                                            // ArenaList in which to allocate
  const size_t size)                                                            // Amount of memory required
 {
#ifdef ArenaListEditable                                                                // Check the free space chains first to see if there is any free space we can reuse rather than allocating more space in the arena.
  const typeof(exponentOfNextPowerOfTwo(size)) e = exponentOfNextPowerOfTwo(size);                                           // Exponent of power of two of block size
  const typeof(list->arena ->freeSpace[e]) f = list->arena ->freeSpace[e];                                               // First element (if any) on the free space chain of this size
  if (f)                                                                        // Free chain has an element
   {ArenaListContent * c = list->proto->pointer(list, f);                                           // Content of first free node
    list->arena->freeSpace[e] = c->next;                                        // Remove node from free chain
    return list->proto->offset(list, f);                                                    // Return node - it was cleared when it was freed
   }
#endif

 if (list->arena->used + size < list->arena->size)                              // Allocate within existing arena
   {const typeof(list->proto->offset(list, list->arena->used)) n = list->proto->offset(list, list->arena->used);                                       // Current offset to open memory
    list->arena->used += size;                                                  // Allocate
#ifdef ArenaListEditable                                                                // Check the free space chains first to see if there is any free space we can reuse rather than allocating more space in the arena.
    n.proto->content(&n)->size = exponentOfNextPowerOfTwo(size);                         // Node size
#endif
    return n;                                                                   // Return allocation
   }
  else                                                                          // Reallocate arena
   {const typeof(nextPowerOfTwo(list->arena->size + size)) S = nextPowerOfTwo(list->arena->size + size);                               // Round up memory required
    if (S < ((size_t)((1l<<(sizeof(((ArenaListContent *)0)->length)*8)) - 1)))          // The maximum size of a delta if there are 8 bits in a byte
     {const typeof(realloc(list->arena->data, S)) m = realloc(list->arena->data, S);                                        // Reallocate arena - the old one will be copied into it if it is moved.
      if (m)                                                                    // Retry the memory allocation
       {list->arena->data = m;                                                  // New arena
        list->arena->size = S;                                                  // Arena size
        const typeof(list->arena->used) u = list->arena->used;                                                  // Length of free space
        memset(list->arena->data + u, 0, S - u);                                // Clear free space
        return list->proto->allocate(list, size);                                           // Allocate within arena
       }
     }
   }
  printStackBackTraceAndExit(2, "Requested arena too large\n");                 // The arena has become too large for the chosen size of offsets.
 }

static size_t used_size__ArenaList                                                      // Amount of space currently being used within the arena of a ArenaList.
 (const ArenaList * list)                                                               // ArenaList
 {return list->arena->used;
 }

static ArenaListNode node_ArenaListNode__ArenaList_string_size                                          // Create a new ArenaList node with the specified key.
 (const ArenaList    *       list,                                                      // ArenaList in which to create the node
  const void * const key,                                                       // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the ArenaList is determined solely by the user.
  const size_t       length)                                                    // Length of the key.
 {const typeof(sizeof(ArenaListNode) + length + list->proto->width(list)) s = sizeof(ArenaListNode) + length + list->proto->width(list);                                    // Minimum width of node
#ifndef ArenaListEditable
  const typeof(list->proto->allocate(list, s)) n = list->proto->allocate(list, s);                                                       // Offset of space allocated for a node and key
#else
  const typeof(exponentOfNextPowerOfTwo(s)) t = exponentOfNextPowerOfTwo(s);                                              // Round size up to next power of two
  const typeof(list->proto->allocate(list, 1ul<<t)) n = list->proto->allocate(list, 1ul<<t);                                                  // Offset of space allocated for a node and key
#endif
  const typeof(n.proto->content(&n)) c = n.proto->content(&n);                                                              // Address content
  c->length = length;                                                           // Save key length
  memcpy(n.proto->key(&n), key, length);                                                 // Copy in key
  return n;                                                                     // Return node
 }

static ArenaListNode nodez_ArenaListNode__ArenaList_string                                              // Create a new ArenaList node with a key specified by a zero terminated string.
 (const ArenaList    *       list,                                                      // ArenaList in which to create the node
  const char * const key)                                                       // Zero terminated string representing the key for this node.
 {return list->proto->node(list, key, strlen(key));
 }

static void setKey__ArenaListNode_string_size                                           // Set the key of a node to a different value as long as there is enough room from the last setting.
 (ArenaListNode      *       node,                                                      // ArenaListNode
  const void * const key,                                                       // Key
  const size_t       length)                                                    // Length of key
 {const typeof(node->proto->maxLength(node)) m = node->proto->maxLength(node);                                                         // Maximum length of key
  if (length <= m)                                                              // There is enough room in the existing key for the new key
   {memcpy(node->proto->key(node), key, length);                                            // Copy in the key
    node->proto->content(node) -> length = length;                                          // Set key length
    return;
   }
#ifndef ArenaListEditable
  printStackBackTraceAndExit(1, "Attempt to set longer key\n");                 // Error: no room for the key and not editable
#else
  const typeof(node->list.proto->node(&node->list, key, length)) n = node->list.proto->node(&node->list, key, length);                                           // Create a node capable of storing the key
  n.proto->replace(&n, *node);                                                           // Replace the existing node with the new node
  node->proto->free(node);                                                                  // Place the original node on the appropriate free chain
#endif
 }

static size_t freedSpace_size__ArenaList                                                //P Amount of space on the free chains
 (const ArenaList * list)                                                               // ArenaList
#ifndef ArenaListEditable
 {return 0 * sizeof(*list);
 }
#else
 {const typeof(sizeof(((ArenaListArena *)0)->freeSpace) / sizeof(((ArenaListArena *)0)->freeSpace[0])) N = sizeof(((ArenaListArena *)0)->freeSpace) / sizeof(((ArenaListArena *)0)->freeSpace[0]);   // Number of free space chains
  typeof(0ul) F = 0ul;                                                                     // Total free space
  for(size_t i = 0; i < N; ++i)                                                 // Each chain
   {typeof(list->arena->freeSpace[i]) f = list->arena->freeSpace[i];                                              // First element of chain
    while(f)                                                                    // Run along chain
     {F += 1ul<<i;                                                              // Size of node
      ArenaListContent * c = list->proto->pointer(list, f); f = c -> next;                          // Next element
     }
   }
  return F;                                                                     // Freed space
 }
#endif

static void setData_ArenaListNode_pointer                                               // Set the optional user data associated with a node in an ArenaList
 (ArenaListNode       *       node,                                                     // ArenaListNode
  const void  * const data)                                                     // Data
 {memcpy(node->proto->data(node), data, node->proto->width(node));                                      // Copy in the data
 }

static void getData_ArenaListNode_pointer                                               // Get the optional user data associated with a node in an ArenaList
 (const ArenaListNode * node,                                                           // ArenaListNode
  void * const  data)                                                           // Data
 {memcpy(data, node->proto->data(node), node->proto->width(node));                                      // Copy out the data
 }

static void copyData_ArenaListNode_ArenaListNode                                                // Copy the data from the source node to the target node.
 (      ArenaListNode * target,                                                         // Target ArenaListNode
  const ArenaListNode   source)                                                         // Source ArenaListNode
 {memcpy(target->proto->data(target), source.proto->data(&source), source.proto->width(&source));                         // Copy the data if any
 }

static ArenaListNode  copy_ArenaListNode__ArenaListNode_ArenaList                                               // Copy a node from one ArenaList to another ArenaList
 (const ArenaListNode * source,                                                         // Source ArenaListNode
  const ArenaList       target)                                                         // Target ArenaList
 {makeLocalCopyOfArenaListKey(s, l, *source);
  typeof(target.proto->node(&target, s, l)) n = target.proto->node(&target, s, l);                                                    // Copy node to target
  if  (n.proto->width(&n) != source->proto->width(source))
   {printStackBackTrace("Width of target different from width of source\n");
   }
  else n.proto->copyData(&n, *source);                                                   // Copy data from source to target
  return n;                                                                     // Copy any data associated with the node
 }                                                                              // Return new node

static void fromLetters__ArenaList_string                                               // Load ArenaList from a string of letters and brackets.  The root node of the ArenaList so constructed is always given the letter 'a'.
 (const ArenaList    * list,                                                            // ArenaList
  const char * str)                                                             // String of letters and brackets describing desired list structure
 {const ArenaListNode n = list->proto->root(list);                                                  // The node we are currently  adding to

  void parse(ArenaListNode current)                                                     // Parse the string of letters and brackets to construct the desired ArenaList
   {while(*str)
     {const typeof(*str) c = *str;
      switch(c)
       {case '(':                                                               // Enter new parent scope
         {++str;
          parse(current.proto->last(&current));
          break;
         }
        case ')':                                                               // Exit current parent scope
         {++str; return;
         }
        default:                                                                // Add to current parent
         {current.proto->putLast(&current, list->proto->node(list, &c, 1));
          ++str; continue;
         }
       }
     }
   }

  parse(n);                                                                     // Parse the string of letters and brackets to construct the desired ArenaList
 };

static void free__ArenaList                                                             // Free an entire ArenaList.
 (const ArenaList * list)                                                               // ArenaList to free
 {const typeof(list->arena) a = list->arena;
  if  (a->data) free(a->data);
  free(a);
 }

static void free__ArenaListNode                                                         // Free a node. If the ArenaList is editable the node is made available for reuse otherwise the node wastes space. A new copy of the ArenaList without wasted space can be made with copyAndCompact_ArenaList .
 (ArenaListNode * node)                                                                 // ArenaListNode to free
#ifdef ArenaListEditable
 {const typeof(node->proto->content(node)) c = node->proto->content(node);                                                           // Content of node
  const typeof(c->size) s = c->size;                                                                  // Size of node as a power of two
  memset(c, 0, 1ul<<s);                                                         // Clear node
  const typeof(node->list.arena->freeSpace[c->size  = s]) f = node->list.arena->freeSpace[c->size  = s];                                // First element  of free space chain
  if (f) c->next  = f;                                                          // Place the incoming node at the front of the chain
      node->list.arena->freeSpace[s] = node->offset;                            // New first node on the free space chain
 }
#else
 {node=node;                                                                    // Free has no effect if we are not editable.
 }
#endif

static void clear__ArenaList                                                            // Clear a ArenaList
 (ArenaList * list)                                                                     // ArenaList to clear
 {const typeof(list->arena) a = list->arena;                                                              // Arena for list
  memset(a->data, 0, a->size);                                                  // ValGrind
  a->used   = 0;                                                                // Length used so far
  a->root   = 0;                                                                // Root not set in ArenaList
  list->proto->node(list, "", 0);                                                           // Initialize root node
#ifdef ArenaListEditable
  memset(a->freeSpace, 0, sizeof(a->freeSpace));
#endif
 }

//D1 Characters in Keys                                                         // Operations on characters in the keys of nodes

static void insertChar__ArenaListNode_char_size                                         // Insert the specified character into the key string of a node at the specified position.
 (ArenaListNode    * node,                                                              // ArenaListNode
  const char ins,                                                               // Character to insert
  size_t     pos)                                                               // Position in key. 0 prepends the char, while >= length appends the char.
 {const typeof(node->proto->maxLength(node)) m = node->proto->maxLength(node);                                                         // Maximum length of key
  const typeof(node->proto->length(node)) l = node->proto->length(node);                                                            // Current length of key
  if (pos > l) pos = l;                                                         // Append to end of string if requested position is beyond the end of the string
  if (l + 1 <= m)                                                               // There is enough room in the existing key for the new key
   {char * k = node->proto->key(node);                                                      // Address key
    memmove(k + pos + 1, k + pos, l - pos);                                     // Overlapping move
    node->proto->content(node)->length++;                                                   // Update key length
    k[pos] = ins;                                                               // Insert
   }
  else                                                                          // Create a new node to hold a key that is too large for the current node
   {makeLocalCopyOfArenaListKey(k, l, *node);                                           // Local copy with trailing zero
    memmove(k + pos + 1, k + pos, l - pos);                                     // Overlapping move
    k[pos] = ins;                                                               // Insert
    const typeof(node->list.proto->node(&node->list, k, l+1)) n = node->list.proto->node(&node->list, k, l+1);                                              // Create a node capable of storing the key
    n.proto->replace(&n, *node);                                                         // Replace the existing node with the new node
    node->proto->free(node);                                                                // Place the original node on the appropriate free chain
   }
 }

static void replaceChar__ArenaListNode_char_size                                        // Replace the character at the specified position in the key string of a node with the specified character.
 (const ArenaListNode * node,                                                           // ArenaListNode
  const char    repl,                                                           // Replacement character
  size_t        pos)                                                            // Position in key. 0 replaces the first character.  No replacement occurs if the requested character is beyond the end of the key string
 {const typeof(node->proto->length(node)) l = node->proto->length(node);                                                            // Current length of key
  if (pos >= l) return;                                                         // Cannot delete the character specified because it is beyond the end of the string
  char * k = node->proto->key(node);                                                        // Address key
  k[pos] = repl;                                                                // Replace
 }

static void swapChars__ArenaListNode_size                                               // Swap the characters on either side of the specified position if it is between 1 and length - 1.
 (const ArenaListNode * node,                                                           // ArenaListNode
  size_t        pos)                                                            // Position in key. 1 swaps the first two characters.  length - 1 swaps the last two characters.
 {const typeof(node->proto->length(node)) l = node->proto->length(node);                                                            // Current length of key
  if (pos == 0 || pos >= l) return;                                             // Not in a swappable range
  char * k = node->proto->key(node);                                                        // Address key
  const char c = k[pos]; k[pos] = k[pos - 1]; k[pos - 1] = c;                   // Swap
 }

static void deleteChar__ArenaListNode_size                                              // Delete the character at the specified position in the key string of a node.
 (const ArenaListNode * node,                                                           // ArenaListNode
  size_t        pos)                                                            // Position in key. 0 deletes the first character.  No deletion occurs if the requested character is beyond the end of the key string
 {const typeof(node->proto->length(node)) l = node->proto->length(node);                                                            // Current length of key
  if (pos >= l) return;                                                         // Cannot delete the character specified because it is beyond the end of the string
  char * k = node->proto->key(node);                                                        // Address key
  memmove(k + pos, k + pos + 1, l - pos + 1);                                   // Overlapping move
  node->proto->content(node)->length--;                                                     // Update key length
 }

//D1 Navigation                                                                 // Navigate through a ArenaList.

static int valid_int__ArenaListNode                                                     // Check that a node is valid.
 (const ArenaListNode * child)                                                          // ArenaListNode
 {return child->offset;                                                         // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

static int invalid_int__ArenaListNode                                                   // Check that a node is not valid.
 (const ArenaListNode * child)                                                          // ArenaListNode
 {return !child->offset;                                                        // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

static  ArenaListNode parent_ArenaListNode__ArenaListNode                                               // Get the parent of a child
 (const ArenaListNode * child)                                                          // Child
 {if (child->proto->isRoot(child)) return *child;
  return child->list.proto->offset(&child->list, child->proto->content(child)->parent);
 }

static  ArenaListNode first_ArenaListNode__ArenaListNode                                                // Get the first child under a parent.
 (const ArenaListNode * parent)                                                         // Parent
 {return  parent->list.proto->offset(&parent->list, parent->proto->content(parent)->first);
 }
static  ArenaListNode last_ArenaListNode__ArenaListNode                                                // Get the last child under a parent.
 (const ArenaListNode * parent)                                                         // Parent
 {return  parent->list.proto->offset(&parent->list, parent->proto->content(parent)->last);
 }
#line 586 "/home/phil/c/z/arenaList/arenaList.c"
static  ArenaListNode next_ArenaListNode__ArenaListNode                                                // Get the next child under a parent.
 (const ArenaListNode * parent)                                                         // Parent
 {return  parent->list.proto->offset(&parent->list, parent->proto->content(parent)->next);
 }
#line 586 "/home/phil/c/z/arenaList/arenaList.c"
static  ArenaListNode prev_ArenaListNode__ArenaListNode                                                // Get the prev child under a parent.
 (const ArenaListNode * parent)                                                         // Parent
 {return  parent->list.proto->offset(&parent->list, parent->proto->content(parent)->prev);
 }
#line 586 "/home/phil/c/z/arenaList/arenaList.c"

static  ArenaListNode first_ArenaListNode__ArenaList                                                    // Get the first child in the specified ArenaList.
 (const ArenaList * list)                                                               // Parent
 {const typeof(list->proto->root(list)) root = list->proto->root(list);
  return root.proto->first(&root);
 }
static  ArenaListNode last_ArenaListNode__ArenaList                                                    // Get the last child in the specified ArenaList.
 (const ArenaList * list)                                                               // Parent
 {const typeof(list->proto->root(list)) root = list->proto->root(list);
  return root.proto->last(&root);
 }
#line 593 "/home/phil/c/z/arenaList/arenaList.c"

//D1 Search                                                                     // Search for nodes.

static int equalsString_int__ArenaListNode_string                                       // Check that the key of a node equals a string
 (const ArenaListNode      * node,                                                      // ArenaListNode
  const char * const key)                                                       // Key
 {return node->proto->keyEquals(node, key, strlen(key));
 }

static  ArenaListNode findFirst_ArenaListNode__ArenaListNode_string                                     // Find the first node with the specified key in a post-order traversal of the ArenaList starting at the specified node.
 (const ArenaListNode * node,                                                           // ArenaListNode at the start of the list to be searched
  const char  * const key)                                                      // Key to find
 {jmp_buf found;
  ArenaListNode F = newArenaListNode(({struct ArenaListNode t = {list: node->list, proto: &ProtoTypes_ArenaListNode}; t;}));                                        // An invalid node

  void find(ArenaListNode node)                                                         // Check whether the key of the current node matches the specified key
   {if (node.proto->equalsString(&node, key))                                               // Found
     {F.offset = node.offset;
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) node->proto->by(node, find);                                          // Search the ArenaList

  return F;                                                                     // Return an invalid node
 }

static  ArenaListNode findFirst_ArenaListNode__ArenaList_string                                         // Find the first node with the specified key in a post-order traversal of the ArenaList.
 (const ArenaList    *       list,                                                      // ArenaList
  const char * const key)                                                       // Key to find
 {jmp_buf found;
  ArenaListNode F = newArenaListNode(({struct ArenaListNode t = {list: *list, proto: &ProtoTypes_ArenaListNode}; t;}));                                             // An invalid node

  void find(ArenaListNode node)                                                         // Check whether the key of the current node matches the specified key
   {if (node.proto->equalsString(&node, key))                                               // Found
     {F.offset = node.offset;
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) list->proto->by(list, find);                                          // Search the ArenaList

  return F;                                                                     // Return an invalid node
 }

static  ArenaListNode findFirstChild_ArenaListNode__ArenaListNode_string                                // Find the first child of the specified parent with the specified key.
 (const ArenaListNode *       Parent,                                                   // Parent node
  const char  * const key)                                                      // Key to find immediately under parent
 {const typeof(*Parent) parent = *Parent;
  ArenaListfe(child, parent) if (child.proto->equalsString(&child, key)) return child;               // Found matching child
  return newArenaListNode(({struct ArenaListNode t = {proto: &ProtoTypes_ArenaListNode};   t;}));                                                           // An invalid node
 }

static  ArenaListNode findFirstChild_ArenaListNode__ArenaList_string                                    // Find the first child immediately under the root with the specified key.
 (const ArenaList    *       list,                                                      // ArenaList
  const char * const key)                                                       // Key to find
 {       const typeof(list->proto->root(list)) r = list->proto->root(list);                                                      // Root node of the ArenaList
  return r.proto->findFirstChild(&r, key);                                               // Search the ArenaList
 }

static  ArenaListNode at_ArenaListNode__ArenaListNode_size                                              // The child at the specified index under its parent counting from one.  An invalid node is returned if the index is too large.
 (const ArenaListNode * Parent,                                                         // Parent node
  const size_t  index)                                                          // Index
 {const typeof(* Parent) parent = * Parent;
  typeof(0ul) n = 0ul; ArenaListfe(child, parent) if (++n == index) return child;                   // Found matching child
  return newArenaListNode(({struct ArenaListNode t = {proto: &ProtoTypes_ArenaListNode};   t;}));                                                           // An invalid node
 }

static  ArenaListNode at_ArenaListNode__ArenaList_size                                                  // The child at the specified index under the root node of the ArenaList.
 (const ArenaList    * list,                                                            // ArenaList
  const size_t index)                                                           // Index
 {       const typeof(list->proto->root(list)) r = list->proto->root(list);                                                       // Root node of the ArenaList
  return r.proto->at(&r, index);                                                         // Search the ArenaList
 }

static  size_t index__ArenaListNode                                                     // The index of the specified child under its parent counting from one.
 (const ArenaListNode * child)                                                          // Child
 {typeof(0ul) n = 0ul;
  const typeof(child->proto->parent(child)) parent = child->proto->parent(child);
  ArenaListfe(c, parent) {++n; if (c.offset == child->offset) return n;}                // Index
  printStackBackTrace("Child not present under parent");                        // Failed
  return 0;
 }

//D1 Location                                                                   // Verify the current location.

static int context_int__ArenaListNode_ArenaListNode_string                                      // Return true if the parent of the specified child has the specified key.
 (const ArenaListNode *       child,                                                    // Child
        ArenaListNode * const parent,                                                   // Parent container
  const char  * const key)                                                      // Key
 {if (child->proto->isRoot(child)) return 0;                                                 // The root node has no context
  const typeof(*parent = child->proto->parent(child)) p = *parent = child->proto->parent(child);
  makeLocalCopyOfArenaListKey(k, l, p);
  return p.proto->valid(&p) && !memcmp(k, key, l);                                       // Check key
 }

static int isFirst_int__ArenaListNode                                                   // Confirm a child is first under its parent
 (const ArenaListNode * child)                                                          // Child
 {const typeof(child->proto->parent(child)) parent = child->proto->parent(child);
  return child->proto->equals(child, parent.proto->first(&parent));
 }
static int isLast_int__ArenaListNode                                                   // Confirm a child is last under its parent
 (const ArenaListNode * child)                                                          // Child
 {const typeof(child->proto->parent(child)) parent = child->proto->parent(child);
  return child->proto->equals(child, parent.proto->last(&parent));
 }
#line 695 "/home/phil/c/z/arenaList/arenaList.c"

static int isEmpty_int__ArenaListNode                                                   // Confirm a node has no children.
 (const ArenaListNode   * node)                                                         // ArenaListNode
 {const typeof(node->proto->first(node)) child = node->proto->first(node);
  return !child.proto->valid(&child);                                                        // No first child so the node is empty
 }

static int isOnlyChild_int__ArenaListNode                                               // Confirm that this child is the only child of its parent
 (const ArenaListNode    * child)                                                       // Child
 {const typeof(child->proto->parent(child)) parent = child->proto->parent(child);                                                      // Parent
  return parent.proto->valid(&parent) && child->proto->isFirst(child) && child->proto->isLast(child);                  // Child is first and last and not the root so it is an only child
 }

static int isRoot_int__ArenaListNode                                                    // Confirm a node is the root
 (const ArenaListNode * node)                                                           // ArenaListNode
 {return !node->offset;
 }

//D1 Put                                                                        // Insert children into a ArenaList.

static  ArenaListNode putFL_ArenaListNode__int_ArenaListNode_ArenaListNode                                      //P Put a child first or last under its parent
 (const int   first,                                                            // Put first if true, else last
  const ArenaListNode parent,                                                           // Parent
  const ArenaListNode child)                                                            // Child
 {const typeof(parent.list) list = parent.list;
  if (child.list.arena != list.arena) printStackBackTrace("Wrong list\n");      // Parent and child must be in same ArenaList
  const typeof(parent.offset) P = parent.offset;    const typeof(child.offset) C = child.offset;                                       // Delta of parent and child
  const typeof(parent.proto->content(&parent)) p = parent.proto->content(&parent); const typeof(child.proto->content(&child)) c = child.proto->content(&child);                                   // Pointers to parent and child
  const typeof(first ? p->first : p->last) L = first ? p->first : p->last;                                               // Delta of child currently first/last
  if (!L)                                                                       // No children yet
   {if (first) p->last = C; else p->first = C;                                  // Delta to last/first child
   }
  else                                                                          // Existing children
   {ArenaListContent * const l = list.proto->pointer(&list, L);                                     // Link first/last two children together
    if (first) l->prev = C; else l->next = C;
    if (first) c->next = L; else c->prev = L;
   }
  c->parent = P;                                                                // Parent new first/last child
  if (first)  p->first = C; else p->last = C;                                   // Parent points to new first/last child
  return child;
 }

static  ArenaListNode   putTreeFirst_ArenaListNode__ArenaListNode                                       // Put a child first in the ArenaList containing the arena in which the child was allocated.
 (const ArenaListNode * child)                                                          // Child
 {const typeof(child->list) t = child->list;                                                              // ArenaList containing arena containing child
  const typeof(t.proto->root(&t)) r = t.proto->root(&t);
  return r.proto->putFirst(&r, *child);                                                  // Put the child first
 }
static  ArenaListNode   putTreeLast_ArenaListNode__ArenaListNode                                       // Put a child last in the ArenaList containing the arena in which the child was allocated.
 (const ArenaListNode * child)                                                          // Child
 {const typeof(child->list) t = child->list;                                                              // ArenaList containing arena containing child
  const typeof(t.proto->root(&t)) r = t.proto->root(&t);
  return r.proto->putLast(&r, *child);                                                  // Put the child last
 }
#line 744 "/home/phil/c/z/arenaList/arenaList.c"

static  ArenaListNode   putFirst_ArenaListNode__ArenaListNode_ArenaListNode                                     // Put a child first under its parent
 (const ArenaListNode * parent,                                                         // Parent
  const ArenaListNode   child)                                                          // Child
 {return putFL_ArenaListNode__int_ArenaListNode_ArenaListNode(1, *parent, child);                       // Put a child first under its parent
 }
static  ArenaListNode   putLast_ArenaListNode__ArenaListNode_ArenaListNode                                     // Put a child last under its parent
 (const ArenaListNode * parent,                                                         // Parent
  const ArenaListNode   child)                                                          // Child
 {return putFL_ArenaListNode__int_ArenaListNode_ArenaListNode(0, *parent, child);                       // Put a child last under its parent
 }
#line 751 "/home/phil/c/z/arenaList/arenaList.c"

static  ArenaListNode   nodeFirst_ArenaListNode__ArenaList_string_size                                  // Create a new node with the specified key and place it first in the ArenaList
 (const ArenaList     * list,                                                           // ArenaList
  const char  * string,                                                         // Value of key for new child
  const size_t  length)                                                         // Length of key for new child
 {const typeof(list->proto->node(list, string, length)) n = list->proto->node(list, string, length);                                              // Create node
  const typeof(list->proto->root(list)) r = list->proto->root(list);                                                              // Root of ArenaList
  return r.proto->putFirst(&r, n);                                                       // Put the new node first in the ArenaList
 }
static  ArenaListNode   nodeLast_ArenaListNode__ArenaList_string_size                                  // Create a new node with the specified key and place it first in the ArenaList
 (const ArenaList     * list,                                                           // ArenaList
  const char  * string,                                                         // Value of key for new child
  const size_t  length)                                                         // Length of key for new child
 {const typeof(list->proto->node(list, string, length)) n = list->proto->node(list, string, length);                                              // Create node
  const typeof(list->proto->root(list)) r = list->proto->root(list);                                                              // Root of ArenaList
  return r.proto->putLast(&r, n);                                                       // Put the new node first in the ArenaList
 }
#line 761 "/home/phil/c/z/arenaList/arenaList.c"

static  ArenaListNode   nodeFirst_ArenaListNode__ArenaListNode_string_size                              // Create a new node and place it first under the specified node
 (const ArenaListNode * parent,                                                         // Parent
  const char  * string,                                                         // Value of key for new child
  const size_t  length)                                                         // Length of key for new child
 {const typeof(parent->list) l = parent->list;                                                             // ArenaList containing parent
  const typeof(l.proto->node(&l, string, length)) n = l.proto->node(&l, string, length);                                                 // Create node
  return parent->proto->putFirst(parent, n);                                                  // Put a child first under its parent
 }
static  ArenaListNode   nodeLast_ArenaListNode__ArenaListNode_string_size                              // Create a new node and place it first under the specified node
 (const ArenaListNode * parent,                                                         // Parent
  const char  * string,                                                         // Value of key for new child
  const size_t  length)                                                         // Length of key for new child
 {const typeof(parent->list) l = parent->list;                                                             // ArenaList containing parent
  const typeof(l.proto->node(&l, string, length)) n = l.proto->node(&l, string, length);                                                 // Create node
  return parent->proto->putLast(parent, n);                                                  // Put a child first under its parent
 }
#line 771 "/home/phil/c/z/arenaList/arenaList.c"

static  ArenaListNode putNP_ArenaListNode__int_ArenaListNode_ArenaListNode                                      //P Put a child next or previous to the specified sibling
 (const int   next,                                                             // Put next if true, else previous
  const ArenaListNode sibling,                                                          // Sibling
  const ArenaListNode child)                                                            // Child
 {const typeof(sibling.list) list = sibling.list;                                                        // ArenaList
  const typeof(sibling.proto->parent(&sibling)) parent = sibling.proto->parent(&sibling);                                                    // Parent node
  const typeof(next ? sibling.proto->next(&sibling) : sibling.proto->prev(&sibling)) twin = next ? sibling.proto->next(&sibling) : sibling.proto->prev(&sibling);                              // Next or prev sibling to sibling is such a node exists
  assert(child.list.arena == list.arena);                                       // Parent and child must be in same ArenaList
  const typeof(sibling.proto->content(&sibling)) s = sibling.proto->content(&sibling);                                                        // Pointer to sibling
  const typeof(child.proto->content(&child)) c = child.proto->content(&child);                                                        // Pointer to child

  if (!twin.proto->valid(&twin))                                                            // Sibling is last/first under parent
   {if (next) parent.proto->putLast(&parent, child); else parent.proto->putFirst(&parent, child);           // Place child
   }
  else                                                                          // Not last/first
   {const typeof(twin.proto->content(&twin)) w = twin.proto->content(&twin);                                                         // Pointer to twin of sibling
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

static void setUp__ArenaListNode_ArenaListNode                                                  //P Make the specified parent node the parent of the specified child node
 (const ArenaListNode child,                                                            // Child
  const ArenaListNode parent)                                                           // Child
 {child.proto->content(&child)->parent = parent.offset;                                      // Set parent of child
 }

static  ArenaListNode   putNext_ArenaListNode__ArenaListNode_ArenaListNode                                      // Put a child next after the specified sibling
 (const ArenaListNode * sibling,                                                        // Sibling
  const ArenaListNode   child)                                                          // Child
 {return putNP_ArenaListNode__int_ArenaListNode_ArenaListNode(1, *sibling, child);                      // Put child next
 }
static  ArenaListNode   putPrev_ArenaListNode__ArenaListNode_ArenaListNode                                      // Put a child previous after the specified sibling
 (const ArenaListNode * sibling,                                                        // Sibling
  const ArenaListNode   child)                                                          // Child
 {return putNP_ArenaListNode__int_ArenaListNode_ArenaListNode(0, *sibling, child);                      // Put child previous
 }
#line 815 "/home/phil/c/z/arenaList/arenaList.c"

static  ArenaListNode   nodeNext_ArenaListNode__ArenaListNode_string_size                               // Create a new node next after the specified sibling
 (const ArenaListNode * sibling,                                                        // Sibling
  const char  * string,                                                         // Value of key for new child
  const size_t  length)                                                         // Length of key for new child
 {const typeof(sibling->list) l = sibling->list;                                                            // ArenaList containing parent
  const typeof(l.proto->node(&l, string, length)) n = l.proto->node(&l, string, length);                                                 // Create node
  return sibling->proto->putNext(sibling, n);                                                  // Put a child next after its parent
 }
static  ArenaListNode   nodePrev_ArenaListNode__ArenaListNode_string_size                               // Create a new node next after the specified sibling
 (const ArenaListNode * sibling,                                                        // Sibling
  const char  * string,                                                         // Value of key for new child
  const size_t  length)                                                         // Length of key for new child
 {const typeof(sibling->list) l = sibling->list;                                                            // ArenaList containing parent
  const typeof(l.proto->node(&l, string, length)) n = l.proto->node(&l, string, length);                                                 // Create node
  return sibling->proto->putPrev(sibling, n);                                                  // Put a child next after its parent
 }
#line 825 "/home/phil/c/z/arenaList/arenaList.c"

static  void replace__ArenaListNode_ArenaListNode                                               // Replace the specified node with this node
 (const ArenaListNode * with,                                                           // Replace with this node
  const ArenaListNode   over)                                                           // Node to be replaced
 {over.proto->putPrev(&over, *with);                                                        // Place new node
  ArenaListContent * w = with->proto->content(with);                                                // Transfer children
  ArenaListContent * o = over.proto->content(&over);
  w->first = o->first; w->last = o->last;
  over.proto->cut(&over);                                                                   // Remove node being replaced
 }

static void splitKey_ArenaListNode_ArenaListNode                                                // Split the specified node at the specified position.
 (const ArenaListNode * Node,                                                           // ArenaListNode
  const size_t  pos)                                                            // Position in key at which to split.
 {const typeof(*Node) node = *Node;                                                                 // Local copy
  makeLocalCopyOfArenaListKey(k, l, node);                                              // Local copy
  if (pos == 0 || pos >= l) return;                                             // Requested position would result in an empty key
  node.proto->content(&node) -> length = pos;                                               // New length of left node
  const typeof(node.list.proto->node(&node.list, k+pos, l - pos)) n = node.list.proto->node(&node.list, k+pos, l - pos);                                      // Create right hand node
  node.proto->putNext(&node, n);                                                            // Insert right hand node
 }

//D1 Traverse                                                                   // Traverse a ArenaList.

static void by__ArenaListNode_sub                                                       // Traverse the ArenaList rooted at the specified node in post-order calling the specified function to process each child node.  The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
 (const ArenaListNode * node,                                                           // ArenaListNode
  void (* const function) (const ArenaListNode node))                                   // Function
 {void children(const ArenaListNode parent)                                             // Process the children of the specified parent
   {if (!parent.proto->valid(&parent)) return;                                                // Empty child
    const typeof(parent.proto->countChildren(&parent)) N = parent.proto->countChildren(&parent);                                                // Number of children
    if (N)                                                                      // Process children if there are any
     {size_t c[N+1];                                                            // Array of children terminated by a trailing zero
      size_t n = 0; ArenaListfe(child, parent) c[n++] = child.offset;                   // Load each child into an array
      for(size_t i = 0; i < N; ++i)                                             // Process each child allowing it to change its position without changing the traversal
       {children(newArenaListNode(({struct ArenaListNode t = {list: parent.list, offset: c[i], proto: &ProtoTypes_ArenaListNode}; t;})));
       }
     }
    function(parent);                                                           // Process the parent
   }
  children(*node);                                                              // Start at the specified node
 }

static void by__ArenaList_sub                                                           // Traverse a ArenaList in post-order calling the specified function to process each child node.  The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
 (const ArenaList * list,                                                               // ArenaList
  void (* const function) (const ArenaListNode node))                                   // Function
 {const typeof(list->proto->root(list)) root = list->proto->root(list);                                                           // Root node
  const typeof(list->proto->countChildren(list)) N = list->proto->countChildren(list);                                                  // Number of children under root
  if (N)                                                                        // Process children under root if there are any
   {size_t c[N+1];                                                              // Array of children terminated by a trailing zero
    size_t n = 0; ArenaListfe(child, root) c[n++] = child.offset;                       // Load each child into an array
    for(size_t i = 0; i < N; ++i)
     {ArenaListNode n = newArenaListNode(({struct ArenaListNode t = {list: *list, offset: c[i], proto: &ProtoTypes_ArenaListNode}; t;}));                           // Process each child allowing it to change position
      n.proto->by(&n, function);
     }
   }
  function(root);                                                               // Start at the root
 }

static void scan__ArenaListNode_sub                                                     // Traverse the ArenaList rooted at the specified node calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
 (const ArenaListNode * node,                                                           // ArenaListNode
  void (* const function) (ArenaListNode node, int start, int depth))                   // Function: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
 {void children(ArenaListNode parent, int depth)                                        // Process the children of the specified parent
   {if (!parent.proto->valid(&parent)) return;                                                // Empty child
    const typeof(parent.proto->countChildren(&parent)) N = parent.proto->countChildren(&parent);                                                // Number of children
    if (N)                                                                      // Process children if there are any
     {size_t c[N+1];                                                            // Array of children terminated by a trailing zero
      size_t n = 0; ArenaListfe(child, parent) c[n++] = child.offset;                   // Load each child into an array
      function(parent, 1, depth);                                               // Call before
      for(size_t i = 0; i < N; ++i)                                             // Process each child allowing it to change its position without changing the traversal
       {children(newArenaListNode(({struct ArenaListNode t = {list: parent.list, offset: c[i], proto: &ProtoTypes_ArenaListNode}; t;})), depth+1);
       }
      function(parent, -1, depth);                                              // Call after
     }
    else function(parent, 0, depth);                                            // Call once as there are no child nodes
   }
  children(*node, 1);                                                           // Start at the root node
 }

static void scan__ArenaList_sub                                                         // Traverse a ArenaList calling the specified function before(+1) and after(-1) processing the children of each node - or - if the node has no children the function is called once(0) . The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
 (const ArenaList * list,                                                               // ArenaList
  void (* const function) (ArenaListNode node, int start, int depth))                   // Function: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the function is called once with start set to zero.
 {const typeof(list->proto->root(list)) root = list->proto->root(list);                                                           // Root node
  const typeof(list->proto->countChildren(list)) N = list->proto->countChildren(list);                                                  // Number of children under root
  if (N)                                                                        // Process children under root if there are any
   {size_t c[N+1];                                                              // Array of children terminated by a trailing zero
    size_t n = 0; ArenaListfe(child, root) c[n++] = child.offset;                       // Load each child into an array
    function(root, 1, 0);                                                       // Call before
    for(size_t i = 0; i < N; ++i)
     {const typeof(newArenaListNode(({struct ArenaListNode t = {list: *list, offset: c[i], proto: &ProtoTypes_ArenaListNode}; t;}))) n = newArenaListNode(({struct ArenaListNode t = {list: *list, offset: c[i], proto: &ProtoTypes_ArenaListNode}; t;}));                                 // Process each child allowing it to change position
      n.proto->scan(&n, function);
     }
    function(root, -1, 0);                                                      // Call after
   }
  else function(root, 0, 0);                                                    // Call once as the root has no children
 }

static void scanFrom__ArenaListNode_sub_int                                             // Traverse the ArenaList starting at the specified node in post-order calling the specified function to process each child node continuing through the siblings of all the specified node's ancestors.  The ArenaList is buffered allowing changes to be made to the structure of the ArenaList without disruption as long as each child checks its context.
 (ArenaListNode * node,                                                                 // ArenaListNode
  int  (* const sub) (ArenaListNode node, int start, int depth),                        // Function: start is set to +1 before the children are processed, -1 afterwards. if the parent has no children the sub is called once with start set to zero.  The funstion should return true if processing should continue, else false.
  int     close)                                                                // Start at the close tag of a non singleton tag if true otherwise start at the open.
 {ArenaListNodeAndState *s, *S;

  typeof(makeArenaArray(sizeof(*s))) stack = makeArenaArray(sizeof(*s));                                           // Path from root to current node being expanded

  for(typeof(*node) parent = *node; parent.proto->valid(&parent); parent = parent.proto->parent(&parent))                 // Load path on stack
   {s = stack.proto->push(&stack);
    s->node = parent; s->state = 1; s->count  = parent.proto->countChildren(&parent);         // Process forwards from each node om the stack
   }

  stack.proto->reverse(&stack);                                                              // Path from [root to current node
  s = stack.proto->top(&stack); s->state = close;                                            // Start on the specified node not just after it

  jmp_buf finish;
  if (!setjmp(finish))                                                          // Scan forwards until terminated
   {while(stack.proto->notEmpty(&stack))                                                     // Process remaining nodes
     {s = stack.proto->top(&stack);                                                          // Latest node to expand
      const typeof(s->node) node = s->node; const typeof(s->state) state = s->state; const typeof(s->count) count = s->count;
      if (!state)                                                               // Expand node
       {s->state = 1;                                                           // Mark node for processing after expansion
        if ((s->count = node.proto->countChildren(&node)))                                  // Latest node has children
         {if (sub(node, +1, stack.proto->count(&stack))) longjmp(finish, 1);                 // Open scope
          S = stack.proto->push(&stack); S->state = 0; S->node = node.proto->first(&node);               // Add first child to scope
         }
        else
         {if (sub(node,  0, stack.proto->count(&stack))) longjmp(finish, 2);                 // Latest node is a leaf
         }
       }
      else                                                                      // Move to next node  after expansion has been completed
       {if (count && sub(node, -1, stack.proto->count(&stack))) longjmp(finish, 3);          // Close scope if necessary
        typeof(node.proto->next(&node)) n = node.proto->next(&node);                                                        // Next sibling
        if (n.proto->valid(&n)) {s->node = n; s->state = 0;} else stack.proto->pop(&stack);           // Place next sibling on stack for processing or close the scope if there are no more siblings
       }
     }
   }

  stack.proto->free(&stack);
 }

static  size_t countChildren_size__ArenaList                                            // Count the number of children directly under a parent.
 (const ArenaList * list)                                                               // ArenaList
 {size_t l = 0; const typeof(list->proto->root(list)) root = list->proto->root(list); ArenaListfe(child, root) ++l;
  return l;                                                                     // Return count
 }

static  size_t countChildren_size__ArenaListNode                                        // Count the number of children directly under a node.
 (const ArenaListNode * Parent)                                                         // Parent
 {size_t l = 0; const typeof(*Parent) parent = *Parent; ArenaListfe(child, parent) ++l;
  return l;                                                                     // Return count
 }

static  size_t count_size__ArenaListNode                                                // Count the number of nodes under a specified node but not including the specified node.
 (const ArenaListNode * node)                                                           // ArenaListNode
 {size_t l = 0;

  void count(const ArenaListNode parent)                                                // Process the children of the specified parent
   {if (!parent.proto->valid(&parent)) return;
    l++; ArenaListfe(child, parent) count(child);                                       // Each child
   }

  count(*node);                                                                 // Start at the specified node
  return l - 1;                                                                 // Return count without counting the root node
 }

static  size_t count_size__ArenaList                                                    // Count the number of nodes in a ArenaList
 (const ArenaList * list)                                                               // ArenaList
 {size_t l = 0;

  const typeof(list->proto->root(list)) root = list->proto->root(list); ArenaListfe(child, root) l += 1 + child.proto->count(&child);                  // Child under root plus its children
  return l;                                                                     // Return count without counting the root node
 }

static ArenaListPosition preOrderPosition__ArenaListTag                                         // Position of a node in pre-order - nodes are counted from one in preorder, bytes are the number of preceding bytes.
 (const ArenaListNode * node)                                                           // ArenaListTag
 {ArenaListPosition p = newArenaListPosition(({struct ArenaListPosition t = {proto: &ProtoTypes_ArenaListPosition};   t;}));
  typeof(0ul) n = 0ul; typeof(0ul) b = 0ul;
  jmp_buf found;

  void children(ArenaListNode parent, int depth)                                        // Process the children of the specified parent
   {if (parent.proto->invalid(&parent)) return;                                               // Empty child
    ++n;                                                                        // Node count
    if (parent.offset == node->offset)                                          // Found current node
     {ArenaListPosition q = newArenaListPosition(({struct ArenaListPosition t = {byte: b, node: n, depth: depth, proto: &ProtoTypes_ArenaListPosition}; t;}));              // Record position
      ({typeof(p) sourcesourcesource = q; memcpy((void *)&p, (void *)&sourcesourcesource, sizeof(p));});                                                                    // Set read only structure in memory
      longjmp(found, 1);                                                        // Finished
     }
    b += parent.proto->length(&parent);                                                       // Preceding byte count
    ArenaListfe(child, parent) children(child, depth + 1);                              // Start at the root node
   }

  const typeof(node->list.proto->root(&node->list)) root = node->list.proto->root(&node->list);
  if (!setjmp(found)) ArenaListfe(child, root) children(child, 1);                      // Start at the root node
  return p;
 }

static  ArenaList copyAndCompact_ArenaList__ArenaList                                                   // Copy a ArenaList compacting any free space.  This method assumes that there are no direct external references to the nodes in the list as this process might change the location of one or more nodes in the arena.
 (const ArenaList * source)                                                             // Source ArenaList
 {const typeof(makeArenaListWithWidth(source->proto->width(source))) target = makeArenaListWithWidth(source->proto->width(source));                                      // ArenaList with same width
  void children(ArenaListNode old, ArenaListNode new)                                           // Make a new copy of each node in the source ArenaList.
   {ArenaListfe(child, old)                                                             // Each child of old parent
     {const typeof(child.proto->copy(&child, target)) n = child.proto->copy(&child, target);                                                 // Copy child to new ArenaList
      new.proto->putLast(&new, n);                                                         // Place new child
      children(child, n);                                                       // Copy grand children
     }
   }
  const typeof(source->proto->root(source)) rs = source->proto->root(source); const typeof(target.proto->root(&target)) rt = target.proto->root(&target);                                       // Start at root
  children(rs, rt);                                                             // Each child of the root node
  return target;                                                                // Return new ArenaList
 }

//D1 Print                                                                      // Print ArenaLists in various ways

static void printWithBrackets__ArenaListNode_function                                   // Apply a function to a string containing a print of the specified  node and the ArenaList below it in preorder as a string with each sub ArenaList enclosed in brackets.
 (const ArenaListNode * node,                                                           // ArenaListNode
  void (*printer)(char * string, size_t length))                                // Function to apply to printed ArenaList
 {size_t l = 0;                                                                 // Length of output string
  void len(const ArenaListNode parent)                                                  // Find the size of buffer we will need
   {l += parent.proto->length(&parent);                                                       // Parent key
    if (!parent.proto->isEmpty(&parent))                                                      // Parent has children
     {l += 2;                                                                   // Space for brackets
      ArenaListfe(child, parent) len(child);                                            // Each child
     }
   }

  char * p;
  void print(const ArenaListNode parent)                                                // Print the children of the specified parent
   {makeLocalCopyOfArenaListKey(k, l, parent);                                          // Local copy of key
    memcpy(p, k, l); p += l;                                                    // Copy key into print string
    if (!parent.proto->isEmpty(&parent))                                                      // Parent has children
     {*p++ = '(';                                                               // Start children
      ArenaListfe(child, parent) print(child);                                          // Each child
      *p++ = ')';                                                               // Start children
     }
    *p = 0;                                                                     // End the string
   }

  len(*node); char P[l+1]; P[0] = 0; p = P; print(*node);                       // Print in local string
  printer(P, l);                                                                // Apply function
 }

static void printWithBrackets__string_ArenaList                                         // Print an entire ArenaList in preorder as a string with brackets around each sub ArenaList
 (const ArenaList * list,                                                               // ArenaList
  void (*printer)(char * string, size_t length))                                // Function to apply to printed ArenaList
 {const typeof(list->proto->root(list)) node = list->proto->root(list);                                                           // Root
  node.proto->printWithBrackets(&node, printer);
 }

static int printsWithBracketsAs_int__ArenaListNode_string                               // Check that the ArenaList starting at the specified node prints with brackets as expected.
 (const ArenaListNode *       node,                                                     // ArenaListNode
  const char  * const expected)                                                 // Expected string when printed
 {int r;
  void printer(char * s, size_t l)
   {r = !memcmp(s, expected, l);
   }
  node->proto->printWithBrackets(node, printer);
  return r;
 }

static int printsWithBracketsAs_int__ArenaList_string                                   // Check that the specified ArenaList prints with brackets as expected.
 (const ArenaList    *       list,                                                      // ArenaList
  const char * const expected)                                                  // Expected string when printed
 {       const typeof(list->proto->root(list)) root = list->proto->root(list);                                                    // Root
  return root.proto->printsWithBracketsAs(&root, expected);
 }

static void dumpWithBrackets__ArenaList                                                 //P Dump a ArenaList printed with brackets to stderr
 (const ArenaList * list)                                                               // ArenaList
 {void printer(char * s, size_t l) {say("\n%s", s); l=l;}
  list->proto->printWithBrackets(list, printer);
 }

static void dump__ArenaList                                                             //P Dump a ArenaList on stderr
 (const ArenaList * list)                                                               // ArenaList
 {size_t n = 0;
  void print(const ArenaListNode parent, int depth)                                     // Print the children of the specified parent
   {makeLocalCopyOfArenaListKey(k, l, parent);                                          // Local copy of key
    printf("%lu ", ++n);                                                        // Print key number
    for(int i = 0; i < depth; ++i) printf("  ");                                // Spacer
    say("%s", k);                                                               // Print key
    if (!parent.proto->isEmpty(&parent)) ArenaListfe(child, parent) print(child, depth+1);            // Each child
   }

  const typeof(list->proto->root(list)) root = list->proto->root(list); say("Dump:"); print(root, 0);                             // Dump from root
 }

static void dump__ArenaListNode                                                         //P Dump a ArenaListNode on stderr
 (const ArenaListNode * Node)                                                           // ArenaListNode
 {const typeof(*Node) node = *Node;
  makeLocalCopyOfArenaListKey(k, l, node);                                              // Local copy of key
  say("%d %s", l, k);                                                           // Print key number
 }

static void dumpPosition__ArenaListPosition                                             //P Dump a ArenaListPosition
 (const ArenaListPosition * position)                                                   // ArenaListPosition
 {const typeof(*position) p = *position;
  say("position(byte: %lu, node: %lu, depth: %lu)", p.byte, p.node, p.depth);                                                           // Print key number
 }

static void print__ArenaListNode_function                                               // Apply a function to the print of a ArenaListNode and the tree below it.
 (const ArenaListNode * node,                                                           // ArenaListNode
  void (*printer)(char * string, size_t length))                                // Function to apply to printed ArenaList
 {size_t l = 0;                                                                 // Length of output string
  void len(const ArenaListNode parent)                                                  // Find the size of buffer we will need
   {l += parent.proto->length(&parent);                                                       // Parent key
    if (!parent.proto->isEmpty(&parent)) ArenaListfe(child, parent) len(child);                       // Each child
   }

  char * p;
  void print(const ArenaListNode parent)                                                // Print the children of the specified parent
   {makeLocalCopyOfArenaListKey(k, l, parent);                                          // Local copy of key
    memcpy(p, k, l); p += l;
    if (!parent.proto->isEmpty(&parent)) ArenaListfe(child, parent) print(child);                     // Each child
    *p = 0;                                                                     // End the string
   }

  len(*node); char P[l+1]; P[0] = 0; p = P; print(*node); printer(P, l);        // Print in local string
 }

static int printsAs_int__ArenaListNode_string                                           // Check that the specified node prints as expected.
 (const ArenaListNode *       node,                                                     // ArenaList
  const char  * const expected)                                                 // Expected string when printed
 {int r;
  void printer(char * s, size_t l) {r = !memcmp(s, expected, l);}
  node->proto->print(node, printer);
  return r;
 }

static int printsAs_int__ArenaList_string                                               // Check that the specified ArenaList prints as expected.
 (const ArenaList    *       list,                                                      // ArenaList
  const char * const expected)                                                  // Expected string when printed
 {int r;
  void printer(char * s, size_t l) {r = !memcmp(s, expected, l);}               // Compare print with expected
  const typeof(list->proto->root(list)) root = list->proto->root(list); root.proto->print(&root, printer);                                    // Print from root of ArenaList
  return r;
 }

static int printContains_int__ArenaListNode                                             // Check the print of an ArenaList starting at the specified tag contains the expected string.
 (const ArenaListNode * node,                                                           // Starting node
  const char  * expected)                                                       // Expected string
 {int r;
  void printer(char * s, size_t l) {r = !!strstr(s, expected); if(0)l=l;}       // Compare print with expected
  node->proto->print(node, printer);
  return r;
 }

static int printContains_int__ArenaList                                                 // Check the print of an ArenaList contains the expected string.
 (const ArenaList    *  list,                                                           // ArenaList parse ArenaList
  const char *  expected)                                                       // Expected string
 {int r;
  void printer(char * s, size_t l) {r = !!strstr(s, expected); if(0)l=l;}       // Compare print with expected
  const typeof(list->proto->root(list)) root = list->proto->root(list); root.proto->print(&root, printer);                                    // Print from root of ArenaList
  return r;
 }

//D1 Sort                                                                       // Sort a ArenaList by its keys

static int cmp_int__ArenaList_ArenaList                                                         //P Compare two keys
 (ArenaListNode * First,                                                                // First node
  ArenaListNode   second)                                                               // Second node
 {const typeof(*First) first = *First;
  makeLocalCopyOfArenaListKey(K, L, first);                                             // Key of first node
  makeLocalCopyOfArenaListKey(k, l, second);                                            // Key of second node
  if (l < L) {const typeof(memcmp(K, k, l)) i = memcmp(K, k, l); return i ? i : +1;}                          // First key longer than second key
  if (L < l) {const typeof(memcmp(K, k, L)) i = memcmp(K, k, L); return i ? i : -1;}                          // First key shorter then second key
  return memcmp(K, k, L);                                                       // Equal length keys
 }

static void sort__ArenaListNode                                                         // Quick sort the children of a node in the ArenaList in situ
 (const           ArenaListNode * parent)                                               // Parent node
 {typedef struct {ArenaListNode   first, last;} Range;                                  // A range of nodes

  typeof(makeArenaArray(sizeof(Range))) stack = makeArenaArray(sizeof(Range));                                        // Arena stack of ranges to use heap rather than local storage

  void range(ArenaListNode first, ArenaListNode last)                                           // Start and end of range to be sorted: the start and end nodes must already be in their correct positions.
   {Range r = {first, last}; const typeof(stack.proto->push(&stack)) s = stack.proto->push(&stack); ({typeof(r) sourcesourcesource = r;  memcpy((void *)s,  (void *)&sourcesourcesource, sizeof(r));});                           // Push a range onto the stack
   }

  const typeof(*parent) p = *parent; const typeof(p.proto->countChildren(&p)) N = p.proto->countChildren(&p);                                           // Check for special cases

  if (N > 1)                                                                    // Already sorted if no children or just one child
   {typeof(p.proto->lowest(&p)) l = p.proto->lowest(&p);  p.proto->putFirst(&p, l.proto->cut(&l));                                     // Place child with  lowest key first
    const typeof(p.proto->highest(&p)) h = p.proto->highest(&p); p.proto->putLast(&p, h.proto->cut(&h));                                     // Place child with highest key last

    if (N > 3)                                                                  // Already sorted if two or three children
     {typeof(l) p = l;
      for  (typeof(p.proto->next(&p)) q = p.proto->next(&p); q.proto->valid(&q); p = q, q = q.proto->next(&q))                       // Sort if still out of order
       {if (q.proto->cmp(&q, p) < 0) {range(l, h); break;}                               // Lowest and highest are in position so we can use them to to delimit the interior range to be sorted
       }
     }
   }

  while(stack.proto->notEmpty(&stack))                                                       // Perform all the sorts outstanding
   {Range r; const typeof(stack.proto->pop(&stack)) s = stack.proto->pop(&stack);                        memcpy((void *)&r, (void *)s, sizeof(r));                                            // Pop the next range to be sorted off the stack
        const typeof(r.first.proto->next(&r.first)) next = r.first.proto->next(&r.first);                                                  // Parent key
    if (next.proto->notEquals(&next, r.last))                                               // Range has more than two nodes
     {for(typeof(next.proto->next(&next)) p = next.proto->next(&next); p.proto->notEquals(&p, r.last); p = p.proto->next(&p))                 // Partition interior
       {if (p.proto->cmp(&p, next) < 0) next.proto->putPrev(&next, p.proto->cut(&p));                         // Partition around next
       }
      range(r.first, next); range(next, r.last);                                // Sort each partition
     }
   }

  stack.proto->free(&stack);
 }

static void sort__ArenaList                                                             // Sort the children of the root of a ArenaList in situ
 (const ArenaList * list)                                                               // ArenaList
 {const typeof(list->proto->root(list)) node = list->proto->root(list);                                                           // Root
  node.proto->sort(&node);
 }

static ArenaListNode lowest_ArenaListNode__ArenaListNode                                                // Find an example of the lowest key under a parent node
 (const ArenaListNode * Parent)                                                         // ArenaListNode
 {const typeof(*Parent) parent = *Parent;
  typeof(parent.proto->first(&parent)) l = parent.proto->first(&parent); ArenaListfe(c, parent) if (c.proto->cmp(&c, l) < 0) l = c;
  return l;
 }

static ArenaListNode highest_ArenaListNode__ArenaListNode                                               // Find an example of the highest key under a parent node
 (const ArenaListNode * Parent)                                                         // ArenaListNode
 {const typeof(*Parent) parent = *Parent;
  typeof(parent.proto->first(&parent)) h = parent.proto->first(&parent); ArenaListfe(c, parent) if (c.proto->cmp(&c, h) > 0) h = c;
  return h;
 }

static ArenaListNode lowest_ArenaListNode__ArenaList                                                    // Find an example of the lowest key under the root node of a ArenaList
 (const  ArenaList * Parent)                                                            // ArenaListNode
 {const typeof(* Parent) parent = * Parent;
  const typeof(parent.proto->root(&parent)) root = parent.proto->root(&parent);
  return root.proto->lowest(&root);
 }

static ArenaListNode highest_ArenaListNode__ArenaList                                                   // Find an example of the highest key under the root node of a ArenaList
 (const ArenaList  * Parent)                                                            // ArenaListNode
 {const typeof(* Parent) parent = * Parent;
  const typeof(parent.proto->root(&parent)) root = parent.proto->root(&parent);
  return root.proto->highest(&root);
 }

//D1 Edit                                                                       // Edit a ArenaList in situ.

static  ArenaListNode cut_ArenaListNode__ArenaListNode                                                  // Cut out a child.
 (const ArenaListNode * child)                                                          // Child to cut out
 {const typeof(child->proto->parent(child)) parent = child->proto->parent(child);                                                      // Parent
  const typeof(parent.proto->content(&parent)) p = parent.proto->content(&parent); const typeof(child->proto->content(child)) c = child->proto->content(child);                                    // Parent pointer, child content

  if (child->proto->isOnlyChild(child))                                                      // Only child
   {p->first = p->last = 0;                                                     // Remove child
   }
  else if (child->proto->isLast(child))                                                      // Last child
   {const typeof(child->proto->prev(child)) L = child->proto->prev(child);
    const typeof(L.proto->content(&L)) l = L.proto->content(&L);
    p->last = L.offset; l->next = 0;                                            // Last child under parent
   }
  else if (child->proto->isFirst(child))                                                     // First child
   {const typeof(child->proto->next(child)) F = child->proto->next(child);
    const typeof(F.proto->content(&F)) f = F.proto->content(&F);
    p->first  = F.offset; f->prev = 0;                                          // Last child under parent
   }
  else                                                                          // Some where in the middle
   {const typeof(child->proto->next(child)) N = child->proto->next(child);    const typeof(child->proto->prev(child)) P = child->proto->prev(child);
    const typeof(N.proto->content(&N)) n = N.proto->content(&N); const typeof(P.proto->content(&P)) p = P.proto->content(&P);
    p->next   = N.offset; n->prev  = P.offset;
   }

  c->next = c->prev = 0;                                                        // Remove child

  return *child;
 }

static  ArenaListNode unwrap_ArenaListNode__ArenaListNode                                               // Unwrap the specified parent and return it.
 (const ArenaListNode * parent)                                                         // Parent to unwrap
 {for(ArenaListNode child = newArenaListNode(({struct ArenaListNode t = {list: parent->list, offset: parent->proto->last(parent).offset, proto: &ProtoTypes_ArenaListNode}; t;}));// Remove last children
    child.proto->valid(&child);
    child.offset = parent->proto->last(parent).offset)
   {parent->proto->putNext(parent, child.proto->cut(&child));                                              // Place each child after the parent
   }
  return parent->proto->cut(parent);                                                          // Remove and return empty parent
 }

static  ArenaListNode wrap_ArenaListNode__string                                                // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
 (const ArenaListNode *       child,                                                    // Child to wrap
  const char  * const key)                                                      // Key for new parent
 {const typeof(child->list) list = child->list;                                                         // Tree
  const typeof(list.proto->node(&list, key, strlen(key))) parent = list.proto->node(&list, key, strlen(key));                                       // New parent
  child->proto->putNext(child, parent);                                                     // Place parent after child
  parent.proto->putLast(&parent, child->proto->cut(child));                                                // Place child under parent
  return parent;
 }

//D1 Input and Output                                                           // Read and write a ArenaList from/to a file

static void write__ArenaList_string                                                     // Write a ArenaList to a named file or abort
 (const ArenaList    * list,                                                            // ArenaList
  const char * const file)                                                      // File
 {    const typeof(open(file, O_CREAT| O_WRONLY, S_IRWXU)) o = open(file, O_CREAT| O_WRONLY, S_IRWXU);                               // Open for output creating if needed
  if (o < 0) printStackBackTrace("Cannot open file: %s for write\n", file);

  time_t current; time(&current);
  const ArenaListDescription h =                                                        // Create ArenaList header describing the ArenaList
   {1, 1, 0, list->proto->used(list), list->proto->root(list).offset, list->proto->width(list), current};

  if (sizeof(h) != write(o, &h, sizeof(h)))                                     // Write header
   {printStackBackTrace("Cannot write ArenaList header to file: %s\n", file);
   }

  const typeof(write(o, list->arena->data, list->arena->used)) w = write(o, list->arena->data, list->arena->used);                           // Write arena
  if (w < 0 || list->arena->used != (size_t)w)
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

static void files__ArenaList                                                            // List all the files under the directory name specified by the key of the child
 (const ArenaListNode   * node)                                                         // Child whose key represents the folder to start at
 {void add(char * f)                                                            // Add the entries in a directory
   {typeof(opendir(f)) d = opendir(f);                                                             // Open directory
    if (d)                                                                      // Directory opened
     {for(typeof(readdir(d)) e = readdir(d); e; e = readdir(d))                                    // Read through folder to end
       {if (strcmp(e->d_name, ".") && strcmp(e->d_name, ".."))                  // Skip the two special entries
         {lsprintf(F, 256, "%s/%s", f, e->d_name);                              // Fully qualified file name
          if      (e->d_type == DT_DIR) add(F);                                 // Recurse into folder
          else if (e->d_type == DT_REG) node->proto->nodeLast(node, F, Fl);                 // Add file
         }
       }
      closedir(d);
     }                                                                          // Close folder
    else printStackBackTrace("No such folder %s\n", f);                         // Cannot find folder
   }

  makeLocalCopyOfArenaListKey(start, l, *node);                                         // Start folder
  if (l > 0) add(start); else {const typeof(getcwd(NULL, 0)) p = getcwd(NULL, 0); add(p); free(p);}           // List files in named start folder or in present working directory if named folder has zero length
 }

static void folders__ArenaList                                                          // List all the folders under the directory name specified by the key of the child
 (const ArenaListNode   * node)                                                         // Child whose key represents the folder to start at
 {void add(char * f, size_t l)                                                  // Add the entries in a directory
   {node->proto->nodeLast(node, f, l);                                                      // Save folder
    typeof(opendir(f)) d = opendir(f);                                                             // Open directory
    if (d)                                                                      // Directory opened
     {for(typeof(readdir(d)) e = readdir(d); e; e = readdir(d))                                    // Read through folder to end
       {if (strcmp(e->d_name, ".") && strcmp(e->d_name, ".."))                  // Skip the two special entries
         {lsprintf(F, 256, "%s/%s", f, e->d_name);                              // Fully qualified file name
          if (e->d_type == DT_DIR) add(F, Fl);                                  // Recurse into folder
         }
       }
      closedir(d);
     }                                                                          // Close folder
    else printStackBackTrace("No such folder %s\n", f);                         // Cannot find folder
   }

  makeLocalCopyOfArenaListKey(start, length, *node);                                    // Start folder
  if (length > 0) add(start, length);                                           // List folders in named start folder or in present working directory if named folder has zero length
  else {const typeof(getcwd(NULL, 0)) p = getcwd(NULL, 0); add(p, strlen(p)); free(p);}
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0
#include <stringBuffer.c>

void test0()                                                                    //TmakeArenaList //Tnode //Tfree //TputFirst //TputLast //Tfe //Tfer
 {   const typeof(makeArenaList()) t = makeArenaList();                                                               // Create a ArenaList
  typeof(t.proto->root(&t)) root = t.proto->root(&t);
  const typeof(10ul) N = 10ul;
  for(typeof(0ul) i = 0ul; i < N; ++i)                                                      // Load ArenaList
   {char c = 'a'+i;

    const typeof(t.proto->node(&t, &c, 1)) c1 = t.proto->node(&t, &c, 1); root.proto->putLast(&root, c1);
    typeof(t.proto->node(&t, &c, 1)) c2 = t.proto->node(&t, &c, 1); root.proto->putFirst(&root, c2);
    c = 'A'+i; c2.proto->setKey(&c2, &c, 1);
    makeLocalCopyOfArenaListKey(k2, l2, c2);
    assert( k2[0] == c);
   }

  if (1)                                                                        // For each
   {const typeof(t.proto->count(&t)) n = t.proto->count(&t); char l[n + 1]; memset((void *)&l, 0,                                           sizeof(l));
    assert( n == 2 * N);

    ArenaListfe (child, root) strncat(l, child.proto->key(&child), child.proto->length(&child));
    assert( !strcmp(l, "JIHGFEDCBAabcdefghij"));
   }

  if (1)                                                                        // For each in reverse
   {const typeof(t.proto->count(&t)) n = t.proto->count(&t); char l[n + 1]; memset((void *)&l, 0,                                           sizeof(l));
    ArenaListfr(child, root) strncat(l, child.proto->key(&child), child.proto->length(&child));
    assert( !strcmp(l, "jihgfedcbaABCDEFGHIJ"));
   }

  t.proto->free(&t);
 }

void test1()                                                                    //Troot //Tfirst //Tlast //Tnext //Tprev //Tparent //Tequals //TnotEquals //Tprint //TprintWithBrackets //TfromLetters //TprintsAs
 {  const typeof(makeArenaList()) t = makeArenaList(); t.proto->fromLetters(&t, "b(c(de)f)g(hi)j");
  assert( t.proto->printsWithBracketsAs(&t, "(b(c(de)f)g(hi)j)"));
  assert( t.proto->printsAs(&t, "bcdefghij"));

  const typeof(t.proto->root(&t)) a = t.proto->root(&t);
  const typeof(a.proto->first(&a)) b = a.proto->first(&a);
  const typeof(b.proto->first(&b)) c = b.proto->first(&b);
  const typeof(c.proto->last(&c)) e = c.proto->last(&c);
  const typeof(e.proto->prev(&e)) d = e.proto->prev(&e);

  assert( a.proto->equals(&a, a));  assert( a.proto->notEquals(&a, b));

  char * k = d.proto->key(&d);
  assert( *k == 'd');
    b.proto->printsWithBracketsAs(&b, "b(c(de)f)");
    c.proto->printsWithBracketsAs(&c,   "c(de)");

  assert( c.proto->equals(&c, d.proto->parent(&d)));
  assert( b.proto->equals(&b, c.proto->parent(&c)));
  assert( a.proto->equals(&a, b.proto->parent(&b)));
  assert( a.proto->equals(&a, t.proto->root(&t)));

  assert( c.proto->printsAs(&c, "cde"));
  assert( t.proto->printsAs(&t, "bcdefghij"));

  t.proto->free(&t);
 }

void test2()                                                                    //Tby //TprintsWithBracketsAs //TprintContains //Tscan
 {  const typeof(makeArenaList()) t = makeArenaList(); t.proto->fromLetters(&t, "b(c(de)f)g(hi)j");
  assert( t.proto->printsWithBracketsAs(&t, "(b(c(de)f)g(hi)j)"));
  assert( t.proto->printContains(&t, "def"));

  if (1)
   {char l[1024], *p = l;

    void process(ArenaListNode n)
     {makeLocalCopyOfArenaListKey(k, l, n);
      memcpy(p, k, l); p += l; *p = 0;
     }

    t.proto->by(&t, process);
    assert( !strcmp(l, "decfbhigj"));
   }

  if (1)
   {char l[1024]; char * p = l;

    void process(ArenaListNode n, int start, int depth)
     {makeLocalCopyOfArenaListKey(k, l, n);
      p += sprintf(p, "%s(%d,%d)", k, start, depth);
     }

    t.proto->scan(&t, process);
    const typeof("(1,0)b(1,1)c(1,2)d(0,3)e(0,3)c(-1,2)f(0,2)b(-1,1)g(1,1)h(0,2)i(0,2)g(-1,1)j(0,1)(-1,0)") r = "(1,0)b(1,1)c(1,2)d(0,3)e(0,3)c(-1,2)f(0,2)b(-1,1)g(1,1)h(0,2)i(0,2)g(-1,1)j(0,1)(-1,0)";
    assert( !strcmp(l, r));
   }

  t.proto->free(&t);
 }

void test3()                                                                    //TisFirst //TisLast //TisEmpty //TisRoot //TisOnlyChild //Tcontext //TcheckKey //Tvalid
 {const typeof(makeArenaList()) t = makeArenaList();    t.proto->fromLetters(&t, "b(c(de(f)gh)i)j");
  assert(t.proto->printsWithBracketsAs(&t, "(b(c(de(f)gh)i)j)"));

  const typeof(t.proto->root(&t)) a = t.proto->root(&t);  assert(a.proto->equalsString(&a, ""));                           assert(a.proto->printsWithBracketsAs(&a, "(b(c(de(f)gh)i)j)"));
  const typeof(a.proto->first(&a)) b = a.proto->first(&a); assert(b.proto->equalsString(&b, "b")); assert(a.proto->equals(&a, b.proto->parent(&b))); assert(b.proto->printsWithBracketsAs(&b,  "b(c(de(f)gh)i)"));
  const typeof(b.proto->first(&b)) c = b.proto->first(&b); assert(c.proto->equalsString(&c, "c")); assert(b.proto->equals(&b, c.proto->parent(&c))); assert(c.proto->printsWithBracketsAs(&c,    "c(de(f)gh)"));
  const typeof(c.proto->first(&c)) d = c.proto->first(&c); assert(d.proto->equalsString(&d, "d")); assert(c.proto->equals(&c, d.proto->parent(&d))); assert(d.proto->printsWithBracketsAs(&d,      "d"));
  const typeof(d.proto->next(&d)) e = d.proto->next(&d);  assert(e.proto->equalsString(&e, "e")); assert(c.proto->equals(&c, e.proto->parent(&e))); assert(e.proto->printsWithBracketsAs(&e,       "e(f)"));
  const typeof(e.proto->last(&e)) f = e.proto->last(&e);  assert(f.proto->equalsString(&f, "f")); assert(e.proto->equals(&e, f.proto->parent(&f))); assert(f.proto->printsWithBracketsAs(&f,         "f"));
  const typeof(e.proto->next(&e)) g = e.proto->next(&e);  assert(g.proto->equalsString(&g, "g")); assert(c.proto->equals(&c, g.proto->parent(&g))); assert(g.proto->printsWithBracketsAs(&g,           "g"));
  const typeof(g.proto->next(&g)) h = g.proto->next(&g);  assert(h.proto->equalsString(&h, "h")); assert(c.proto->equals(&c, h.proto->parent(&h))); assert(h.proto->printsWithBracketsAs(&h,            "h"));
  const typeof(c.proto->next(&c)) i = c.proto->next(&c);  assert(i.proto->equalsString(&i, "i")); assert(b.proto->equals(&b, i.proto->parent(&i))); assert(i.proto->printsWithBracketsAs(&i,              "i"));
  const typeof(b.proto->next(&b)) j = b.proto->next(&b);  assert(j.proto->equalsString(&j, "j")); assert(a.proto->equals(&a, j.proto->parent(&j))); assert(j.proto->printsWithBracketsAs(&j,                "j"));

  assert( !a.proto->valid(&a));
  assert(  b.proto->isFirst(&b));
  assert(  j.proto->isLast(&j));
  assert(  f.proto->isFirst(&f));
  assert(  f.proto->isLast(&f));

  assert(  f.proto->isEmpty(&f));
  assert(  f.proto->isOnlyChild(&f));
  assert( !e.proto->isOnlyChild(&e));
  assert(  a.proto->isRoot(&a));

  ArenaListNode A, C, E;
  assert(  f.proto->context(&f, &E, "e"));
  assert(  E.proto->context(&E, &C, "c"));
  assert( !a.proto->context(&a, &A, "a"));

  t.proto->free(&t);
 }

void test4()                                                                    //Tcut //TfindFirst //TcountChildren //TequalsString //TfindFirstChild
 {  const typeof(makeArenaList()) t = makeArenaList();     t.proto->fromLetters(&t, "b(c(de(f)gh)i)j");
  assert( t.proto->printsWithBracketsAs(&t, "(b(c(de(f)gh)i)j)"));

    const typeof(t.proto->findFirst(&t, "c")) c = t.proto->findFirst(&t, "c");

  assert( c.proto->equalsString(&c, "c"));
    const typeof(t.proto->findFirst(&t, "d")) d = t.proto->findFirst(&t, "d"); assert(d.proto->equalsString(&d, "d"));
    const typeof(t.proto->findFirst(&t, "e")) e = t.proto->findFirst(&t, "e"); assert(e.proto->equalsString(&e, "e"));
    const typeof(t.proto->findFirst(&t, "f")) f = t.proto->findFirst(&t, "f"); assert(f.proto->equalsString(&f, "f"));
    const typeof(t.proto->findFirst(&t, "g")) g = t.proto->findFirst(&t, "g"); assert(g.proto->equalsString(&g, "g"));
    const typeof(t.proto->findFirst(&t, "h")) h = t.proto->findFirst(&t, "h"); assert(h.proto->equalsString(&h, "h"));

  assert( g.proto->equals(&g, c.proto->findFirstChild(&c, "g")));
  assert( c.proto->countChildren(&c) == 4);
  assert( e.proto->countChildren(&e) == 1);

  f.proto->cut(&f);           assert(t.proto->printsWithBracketsAs(&t, "(b(c(degh)i)j)"));
  e.proto->putFirst(&e, f);   assert(t.proto->printsWithBracketsAs(&t, "(b(c(de(f)gh)i)j)"));

  e.proto->cut(&e);           assert(t.proto->printsWithBracketsAs(&t, "(b(c(dgh)i)j)"));
  d.proto->putNext(&d, e);    assert(t.proto->printsWithBracketsAs(&t, "(b(c(de(f)gh)i)j)"));

  d.proto->cut(&d);           assert(t.proto->printsWithBracketsAs(&t, "(b(c(e(f)gh)i)j)"));
  e.proto->putPrev(&e, d);    assert(t.proto->printsWithBracketsAs(&t, "(b(c(de(f)gh)i)j)"));

  h.proto->cut(&h);           assert(t.proto->printsWithBracketsAs(&t, "(b(c(de(f)g)i)j)"));
  g.proto->putNext(&g, h);    assert(t.proto->printsWithBracketsAs(&t, "(b(c(de(f)gh)i)j)"));

  t.proto->free(&t);
 }

void test5()                                                                    //TreadArenaTree //Twrite
 {  const typeof(makeArenaList()) t = makeArenaList();     t.proto->fromLetters(&t, "b(c(de(f)gh)i)j");
  assert( t.proto->printsWithBracketsAs(&t, "(b(c(de(f)gh)i)j)"));

    const typeof("/tmp/arenaTreeTest.data") f = "/tmp/arenaTreeTest.data";
  assert( t.proto->printsWithBracketsAs(&t, "(b(c(de(f)gh)i)j)"));
    t.proto->write(&t, f);

    const typeof(readArenaList(f)) u = readArenaList(f);
  assert( u.proto->printsWithBracketsAs(&u, "(b(c(de(f)gh)i)j)"));

    t.proto->free(&t);
    u.proto->free(&u);
  unlink(f);
 }

void test6()                                                                    //Tunwrap //Twrap //Tcount
 {  const typeof(makeArenaList()) t = makeArenaList();     t.proto->fromLetters(&t, "bce");
  assert( t.proto->printsWithBracketsAs(&t, "(bce)"));
  assert( t.proto->count(&t) == 3);

    const typeof(t.proto->findFirst(&t, "c")) c = t.proto->findFirst(&t, "c");
  assert( c.proto->valid(&c));

    const typeof(c.proto->wrap(&c, "d")) d = c.proto->wrap(&c, "d");
  assert( t.proto->printsWithBracketsAs(&t, "(bd(c)e)"));
  assert( t.proto->count(&t) == 4);

    d.proto->unwrap(&d);
  assert( t.proto->printsWithBracketsAs(&t, "(bce)"));
  assert( t.proto->count(&t) == 3);
    t.proto->free(&t);
 }

void test7()                                                                    //TputNext //TputPrev
 {const typeof(makeArenaList()) t = makeArenaList();    t.proto->fromLetters(&t, "A");
              const typeof(t.proto->root(&t)) a = t.proto->root(&t);
          const typeof(a.proto->first(&a)) A = a.proto->first(&a);
  assert(A.proto->equalsString(&A, "A"));

  for(int i = 0; i < 10; ++i)
   {char c = '0'+i;
    A.proto->putNext(&A, t.proto->node(&t, &c, 1));
    A.proto->putPrev(&A, t.proto->node(&t, &c, 1));
   }

  assert(t.proto->printsWithBracketsAs(&t, "(0123456789A9876543210)"));

  t.proto->free(&t);
 }

void test8()                                                                    //TputTreeFirst //TputTreeLast //TsetKey //Tkey //Tlength //Tused //TkeyEquals //Tnodez
 {  const typeof(makeArenaList()) t = makeArenaList();

    const typeof(t.proto->node(&t, "c", 1)) c = t.proto->node(&t, "c", 1); c.proto->putTreeFirst(&c);
    const typeof(t.proto->node(&t, "d", 1)) d = t.proto->node(&t, "d", 1); d.proto->putTreeLast(&d);
    typeof(t.proto->nodez(&t, "b")) b = t.proto->nodez(&t, "b");    b.proto->putTreeFirst(&b);

    b.proto->setKey(&b, "B", 1);
  assert( b.proto->length(&b) == 1);
  assert( *(char *)(b.proto->key(&b)) == 'B');
  assert( b.proto->keyEquals(&b, "B", 1));

#ifndef ArenaListEditable
  assert( t.proto->used(&t) == 131);
#else
  assert( t.proto->used(&t) == 224);
#endif

  assert( t.proto->printsWithBracketsAs(&t, "(Bcd)"));

    t.proto->free(&t);
 }

void test9()                                                                    //Tswap
 {  typeof(makeArenaList()) s = makeArenaList(); s.proto->fromLetters(&s, "s");
    const typeof(makeArenaList()) t = makeArenaList(); t.proto->fromLetters(&t, "t");
  assert( s.proto->printsWithBracketsAs(&s, "(s)"));
  assert( t.proto->printsWithBracketsAs(&t, "(t)"));
    s.proto->swap(&s, t);
  assert( t.proto->printsWithBracketsAs(&t, "(s)"));
  assert( s.proto->printsWithBracketsAs(&s, "(t)"));
    s.proto->free(&s);
    t.proto->free(&t);
 }

void test10()                                                                   //Tdata //TmakeArenaListWithWidth //Twidth //TgetData //TsetData
 {size_t D[] = {0,1,2,3};
    const typeof(makeArenaListWithWidth (sizeof(D))) t = makeArenaListWithWidth (sizeof(D));
  assert( t.proto->width(&t) == sizeof(D));

    const typeof(t.proto->first(&t)) n = t.proto->first(&t);
  assert( n.proto->width(&n) == sizeof(D));

  for(size_t i = 0; i < 10; ++i)
   {char c = '0'+i;
    typeof(t.proto->node(&t, &c, 1)) n = t.proto->node(&t, &c, 1);
    n.proto->putTreeLast(&n);
    n.proto->setData(&n, D);
   }

  const typeof(t.proto->root(&t)) root = t.proto->root(&t);
  ArenaListfe(n, root)
   {char d[sizeof(D)]; n.proto->getData(&n, d);
    assert( !memcmp(d,        D, sizeof(D)));
    assert( !memcmp(n.proto->data(&n), D, sizeof(D)));
   }

  assert( t.proto->printsWithBracketsAs(&t, "(0123456789)"));
    t.proto->free(&t);
 }

void test11()                                                                   //TcopyAndCompact //Tcopy //TcopyData
 {  const typeof(makeArenaListWithWidth(sizeof(size_t))) s = makeArenaListWithWidth(sizeof(size_t)); s.proto->fromLetters(&s, "b(c(de)f)g");
    typeof(2ul) d = 2ul;
    typeof(s.proto->findFirst(&s, "e")) e = s.proto->findFirst(&s, "e");
    e.proto->setData(&e, &d);
    typeof(e.proto->copy(&e, s)) E = e.proto->copy(&e, s);
  assert( e.proto->keyEquals(&e, E.proto->key(&E),  E.proto->length(&E)));

    typeof(1ul) D = 1ul;
    E.proto->getData(&E, &D);
  assert( D == d);

    d = 3ul;
    e.proto->setData(&e, &d);
    E.proto->copyData(&E, e);
    E.proto->getData(&E, &D);
  assert( D == d);

    e.proto->unwrap(&e);
    const typeof(s.proto->copyAndCompact(&s)) t = s.proto->copyAndCompact(&s);

  assert( s.proto->printsWithBracketsAs(&s, "(b(c(d)f)g)"));
  assert( t.proto->printsWithBracketsAs(&t, "(b(c(d)f)g)"));
  assert( t.proto->used(&t) < s.proto->used(&s));
    s.proto->free(&s);
    t.proto->free(&t);
 }

void test12()                                                                   //Treplace
 {  const typeof(makeArenaListWithWidth(sizeof(size_t))) s = makeArenaListWithWidth(sizeof(size_t)); s.proto->fromLetters(&s, "b(c(d)e)f");
    const typeof(s.proto->findFirst(&s, "d")) d = s.proto->findFirst(&s, "d");
    const typeof(s.proto->node(&s, "D", 1)) D = s.proto->node(&s, "D", 1);
    D.proto->replace(&D, d);
  assert( s.proto->printsWithBracketsAs(&s, "(b(c(D)e)f)"));
    s.proto->free(&s);
 }

void test13()
 {const typeof(makeArenaList()) t = makeArenaList();

  typeof(t.proto->node(&t, "a", 1)) a = t.proto->node(&t, "a", 1);

  a.proto->putTreeLast(&a);
#ifndef ArenaListEditable
  assert( t.proto->used(&t) == 65);
#else
  assert( t.proto->used(&t) == 96);                                                             // More because the node gets over allocated
#endif

  assert( t.proto->freedSpace(&t) == 0);
  a.proto->cut(&a);
  a.proto->free(&a);
#ifndef ArenaListEditable
  assert( t.proto->freedSpace(&t) == 0);
#else
  assert( t.proto->freedSpace(&t) == 64);
#endif

  const typeof(t.proto->node(&t, "b", 1)) b = t.proto->node(&t, "b", 1);
  b.proto->putTreeLast(&b);
#ifndef ArenaListEditable
  assert( t.proto->used(&t) == 98);
#else
  assert( t.proto->used(&t) == 96);                                                             // Less due to reuse
#endif
  assert( t.proto->freedSpace(&t) == 0);

  t.proto->free(&t);
 }

void test14()                                                                   //TdeleteChar //TinsertChar //TmaxLength //TreplaceChar //TswapChars
 {const typeof(makeArenaList()) t = makeArenaList();

    typeof(t.proto->node(&t, "abce", 4)) a = t.proto->node(&t, "abce", 4);
#ifndef ArenaListEditable
  assert( a.proto->maxLength(&a) == 4);
#else
  assert( a.proto->maxLength(&a) == 32);

                            assert( a.proto->keyEquals(&a, "abce", 4));
    a.proto->replaceChar(&a, 'd',3); assert( a.proto->keyEquals(&a, "abcd", 4));
    a.proto->swapChars(&a, 1);       assert( a.proto->keyEquals(&a, "bacd", 4));
    a.proto->swapChars(&a, 1);       assert( a.proto->keyEquals(&a, "abcd", 4));
    a.proto->insertChar(&a, 'E', 5); assert( a.proto->keyEquals(&a, "abcdE", 5));
    a.proto->insertChar(&a, 'D', 3); assert( a.proto->keyEquals(&a, "abcDdE", 6));
    a.proto->insertChar(&a, 'C', 2); assert( a.proto->keyEquals(&a, "abCcDdE", 7));
    a.proto->insertChar(&a, 'B', 1); assert( a.proto->keyEquals(&a, "aBbCcDdE", 8));
    a.proto->insertChar(&a, 'A', 0); assert( a.proto->keyEquals(&a, "AaBbCcDdE", 9));
    a.proto->deleteChar(&a, 9);      assert( a.proto->keyEquals(&a, "AaBbCcDdE", 9));
    a.proto->deleteChar(&a, 4);      assert( a.proto->keyEquals(&a, "AaBbcDdE", 8));
    a.proto->deleteChar(&a, 4);      assert( a.proto->keyEquals(&a, "AaBbDdE", 7));
    a.proto->deleteChar(&a, 2);      assert( a.proto->keyEquals(&a, "AabDdE", 6));
    a.proto->deleteChar(&a, 2);      assert( a.proto->keyEquals(&a, "AaDdE", 5));
    a.proto->deleteChar(&a, 0);      assert( a.proto->keyEquals(&a, "aDdE", 4));
    a.proto->deleteChar(&a, 0);      assert( a.proto->keyEquals(&a, "DdE", 3));
    a.proto->deleteChar(&a, 0);      assert( a.proto->keyEquals(&a, "dE", 2));
    a.proto->deleteChar(&a, 0);      assert( a.proto->keyEquals(&a, "E", 1));
    a.proto->deleteChar(&a, 0);      assert( a.proto->keyEquals(&a, "", 0));
#endif

  t.proto->free(&t);
 }

void test15()                                                                   //TsplitKey
 {const typeof(makeArenaList()) t = makeArenaList();

    const typeof(t.proto->node(&t, "aabb", 4)) a = t.proto->node(&t, "aabb", 4);
    a.proto->putTreeLast(&a);
    a.proto->splitKey(&a, 2);
  assert( a.proto->printsAs(&a, "aa"));
    const typeof(a.proto->next(&a)) b = a.proto->next(&a);
  assert( b.proto->printsAs(&b, "bb"));
  assert( t.proto->countChildren(&t) == 2);

  t.proto->free(&t);
 }

void test16()                                                                   //TmakeArenaListFromLines //TmakeArenaListFromWords //TmakeArenaListFromLinesOfWords //TArenaListfeⁱ
 {const typeof(makeArenaListFromWords("a   bb\n \nccc\n\n   \n dddd \n  \n \n")) w = makeArenaListFromWords("a   bb\n \nccc\n\n   \n dddd \n  \n \n");

  assert( w.proto->countChildren(&w) == 4);
  assert( w.proto->printsWithBracketsAs(&w, "(abbcccdddd)"));

   {typeof(0ul) c = 0ul; ArenaListfⁱ(x, w) c += xⁱ; assert( c == 10);}

  w.proto->free(&w);

  const typeof(makeArenaListFromLines("a\nbb\nccc\n    \n\ndddd\n\n\n")) l = makeArenaListFromLines("a\nbb\nccc\n    \n\ndddd\n\n\n");

  assert( l.proto->countChildren(&l) == 4);
  assert( l.proto->printsWithBracketsAs(&l, "(abbcccdddd)"));

  l.proto->free(&l);

  const typeof(makeArenaListFromLinesOfWords("a b c d  \n  aa bb cc dd \n\n   \n\n aaa   bbb ccc ddd\n\n\n")) x = makeArenaListFromLinesOfWords("a b c d  \n  aa bb cc dd \n\n   \n\n aaa   bbb ccc ddd\n\n\n");

  assert( x.proto->countChildren(&x) == 3);
  assert( x.proto->count(&x)         == 15);

    const typeof(x.proto->first(&x)) n1 = x.proto->first(&x); assert( n1.proto->count(&n1) == 4);
    const typeof(n1.proto->next(&n1)) n2 = n1.proto->next(&n1); assert( n2.proto->count(&n2) == 4);
    const typeof(n2.proto->next(&n2)) n3 = n2.proto->next(&n2); assert( n3.proto->count(&n3) == 4);
    const typeof(n3.proto->next(&n3)) n4 = n3.proto->next(&n3); assert(!n4.proto->valid(&n4));

  assert( x.proto->printsWithBracketsAs(&x, "(a b c d  (abcd)  aa bb cc dd (aabbccdd) aaa   bbb ccc ddd(aaabbbcccddd))"));

  if (1)
   {char s[1024], *p = s;
    ArenaListfe(y,x)
     {makeLocalCopyOfArenaListKey(k, l, y);
       p = stpcpy(p, k);
       p = stpcpy(p, " - ");
      }
    assert( !strcmp(s, "a b c d   -   aa bb cc dd  -  aaa   bbb ccc ddd - "));
   }

  if (1)
   {char s[1024], *p = s;
    ArenaListfr(y,x)
     {makeLocalCopyOfArenaListKey(k, l, y);
       p = stpcpy(p, k);
       p = stpcpy(p, " - ");
      }
    assert( !strcmp(s, " aaa   bbb ccc ddd -   aa bb cc dd  - a b c d   - "));
   }

  x.proto->free(&x);
 }

void test17()                                                                   //Tsort //Thighest //Tlowest
 {  const typeof(makeArenaList()) z = makeArenaList();
    z.proto->sort(&z);
  assert( z.proto->countChildren(&z) == 0);
  assert( z.proto->printsWithBracketsAs(&z, "()"));
    z.proto->free(&z);

    const typeof(makeArenaListFromWords("1")) o = makeArenaListFromWords("1");
    o.proto->sort(&o);
  assert( o.proto->countChildren(&o) == 1);
  assert( o.proto->printsWithBracketsAs(&o, "(1)"));
    o.proto->free(&o);

    const typeof(makeArenaListFromWords("1 2")) t = makeArenaListFromWords("1 2");
    t.proto->sort(&t);
  assert( t.proto->countChildren(&t) == 2);
  assert( t.proto->printsWithBracketsAs(&t, "(12)"));
    t.proto->free(&t);

    const typeof(makeArenaListFromWords("3 1 2")) r = makeArenaListFromWords("3 1 2");
    r.proto->sort(&r);
  assert( r.proto->countChildren(&r) == 3);
  assert( r.proto->printsWithBracketsAs(&r, "(123)"));
    r.proto->free(&r);

    const typeof(makeArenaListFromWords("1 2 3 4")) f = makeArenaListFromWords("1 2 3 4");
    f.proto->sort(&f);
  assert( f.proto->countChildren(&f) == 4);
  assert( f.proto->printsWithBracketsAs(&f, "(1234)"));
    f.proto->free(&f);

    const typeof(makeArenaListFromWords("4 3 2 1")) F = makeArenaListFromWords("4 3 2 1");
    F.proto->sort(&F);
  assert( F.proto->countChildren(&F) == 4);

  assert( F.proto->printsWithBracketsAs(&F, "(1234)"));
    F.proto->free(&F);

    const typeof(makeArenaListFromWords(" 9 8 7 6 5 0 2 3 4 1")) w = makeArenaListFromWords(" 9 8 7 6 5 0 2 3 4 1");
    w.proto->sort(&w);
  assert( w.proto->countChildren(&w) == 10);
  assert( w.proto->printsWithBracketsAs(&w, "(0123456789)"));
    w.proto->free(&w);

    const typeof("aaaa0 aaa1 aa2 a3 acc4 cccc5 bbaa6 ccc7 cc8 c9 bbbb10 bbb11 bb12 b13 14\n") S = "aaaa0 aaa1 aa2 a3 acc4 cccc5 bbaa6 ccc7 cc8 c9 bbbb10 bbb11 bb12 b13 14\n";
//◉aaaa0 aaa1 aa2 a3 acc4 cccc5 bbaa6 ccc7 cc8 c9 bbbb10 bbb11 bb12 b13 14
//◉

    const typeof(makeArenaListFromWords(S)) a = makeArenaListFromWords(S);    assert( a.proto->countChildren(&a) == 15);

    a.proto->sort(&a);                 assert( a.proto->countChildren(&a) == 15);

    const typeof(makeStringBuffer()) s = makeStringBuffer(); ArenaListfe(A, a) s.proto->addFormat(&s, "%s\n", A.proto->key(&A));

  assert( s.proto->equalsString(&s, "14\n" "a3\n" "aa2\n" "aaa1\n" "aaaa0\n" "acc4\n" "b13\n" "bb12\n" "bbaa6\n" "bbb11\n" "bbbb10\n" "c9\n" "cc8\n" "ccc7\n" "cccc5\n"));
//◉14
//◉a3
//◉aa2
//◉aaa1
//◉aaaa0
//◉acc4
//◉b13
//◉bb12
//◉bbaa6
//◉bbb11
//◉bbbb10
//◉c9
//◉cc8
//◉ccc7
//◉cccc5
//◉

    const typeof(a.proto->lowest(&a)) l = a.proto->lowest(&a);   assert( l.proto->equalsString(&l, "14"));
    const typeof(a.proto->highest(&a)) h = a.proto->highest(&a);  assert( h.proto->equalsString(&h, "cccc5"));

    a.proto->free(&a); s.proto->free(&s);
 }

void test18()                                                                   //TpreOrderPosition //TequalsPosition
 {const typeof(makeArenaList()) z = makeArenaList();

  const typeof(z.proto->nodez(&z, "a")) a = z.proto->nodez(&z, "a");  a.proto->putTreeLast(&a);
  const typeof(z.proto->nodez(&z, "b")) b = z.proto->nodez(&z, "b");  b.proto->putTreeLast(&b);
  const typeof(z.proto->nodez(&z, "aa")) aa = z.proto->nodez(&z, "aa"); a.proto->putLast(&a, aa);
  const typeof(z.proto->nodez(&z, "ab")) ab = z.proto->nodez(&z, "ab"); a.proto->putLast(&a, ab);
  const typeof(z.proto->nodez(&z, "ba")) ba = z.proto->nodez(&z, "ba"); b.proto->putLast(&b, ba);
  const typeof(z.proto->nodez(&z, "bb")) bb = z.proto->nodez(&z, "bb"); b.proto->putLast(&b, bb);

  assert( z.proto->printsWithBracketsAs(&z, "(a(aaab)b(babb))"));

  {const typeof(a.proto->preOrderPosition(&a)) p = a.proto->preOrderPosition(&a); const typeof(newArenaListPosition(({struct ArenaListPosition t = {byte: 0, node: 1, depth: 1, proto: &ProtoTypes_ArenaListPosition}; t;}))) q = newArenaListPosition(({struct ArenaListPosition t = {byte: 0, node: 1, depth: 1, proto: &ProtoTypes_ArenaListPosition}; t;})); assert( p.proto->equalsPosition(&p, q));}
  {const typeof(aa.proto->preOrderPosition(&aa)) p = aa.proto->preOrderPosition(&aa); const typeof(newArenaListPosition(({struct ArenaListPosition t = {byte: 1, node: 2, depth: 2, proto: &ProtoTypes_ArenaListPosition}; t;}))) q = newArenaListPosition(({struct ArenaListPosition t = {byte: 1, node: 2, depth: 2, proto: &ProtoTypes_ArenaListPosition}; t;})); assert( p.proto->equalsPosition(&p, q));}
  {const typeof(ab.proto->preOrderPosition(&ab)) p = ab.proto->preOrderPosition(&ab); const typeof(newArenaListPosition(({struct ArenaListPosition t = {byte: 3, node: 3, depth: 2, proto: &ProtoTypes_ArenaListPosition}; t;}))) q = newArenaListPosition(({struct ArenaListPosition t = {byte: 3, node: 3, depth: 2, proto: &ProtoTypes_ArenaListPosition}; t;})); assert( p.proto->equalsPosition(&p, q));}
  {const typeof(b.proto->preOrderPosition(&b)) p = b.proto->preOrderPosition(&b); const typeof(newArenaListPosition(({struct ArenaListPosition t = {byte: 5, node: 4, depth: 1, proto: &ProtoTypes_ArenaListPosition}; t;}))) q = newArenaListPosition(({struct ArenaListPosition t = {byte: 5, node: 4, depth: 1, proto: &ProtoTypes_ArenaListPosition}; t;})); assert( p.proto->equalsPosition(&p, q));}
  {const typeof(ba.proto->preOrderPosition(&ba)) p = ba.proto->preOrderPosition(&ba); const typeof(newArenaListPosition(({struct ArenaListPosition t = {byte: 6, node: 5, depth: 2, proto: &ProtoTypes_ArenaListPosition}; t;}))) q = newArenaListPosition(({struct ArenaListPosition t = {byte: 6, node: 5, depth: 2, proto: &ProtoTypes_ArenaListPosition}; t;})); assert( p.proto->equalsPosition(&p, q));}
  {const typeof(bb.proto->preOrderPosition(&bb)) p = bb.proto->preOrderPosition(&bb); const typeof(newArenaListPosition(({struct ArenaListPosition t = {byte: 8, node: 6, depth: 2, proto: &ProtoTypes_ArenaListPosition}; t;}))) q = newArenaListPosition(({struct ArenaListPosition t = {byte: 8, node: 6, depth: 2, proto: &ProtoTypes_ArenaListPosition}; t;})); assert( p.proto->equalsPosition(&p, q));}

  z.proto->free(&z);
 }

void test19()                                                                   //Tat //Tindex //Tinvalid //TkeyEqualsString
 {  const typeof(makeArenaList()) s = makeArenaList(); s.proto->fromLetters(&s, "ab(cde)fg");

    const typeof(s.proto->findFirst(&s, "a")) a = s.proto->findFirst(&s, "a"); assert( a.proto->index(&a) == 1);  assert( a.proto->equals(&a, s.proto->at(&s, 1)));
    const typeof(s.proto->findFirst(&s, "b")) b = s.proto->findFirst(&s, "b"); assert( b.proto->index(&b) == 2);  assert( b.proto->equals(&b, s.proto->at(&s, 2)));

    const typeof(s.proto->findFirst(&s, "c")) c = s.proto->findFirst(&s, "c"); assert( c.proto->index(&c) == 1);  assert( c.proto->equals(&c, b.proto->at(&b, 1)));
    const typeof(s.proto->findFirst(&s, "d")) d = s.proto->findFirst(&s, "d");
  assert( d.proto->index(&d) == 2);
  assert( d.proto->equals(&d, b.proto->at(&b, 2)));
  assert( a.proto->keyEqualsString(&a, "a"));

    const typeof(s.proto->at(&s, 20)) z = s.proto->at(&s, 20); assert( z.proto->invalid(&z));
    const typeof(b.proto->at(&b, 20)) y = b.proto->at(&b, 20); assert( y.proto->invalid(&y));

    s.proto->free(&s);
 }

void test20()                                                                   //TscanFrom
 {  const typeof(makeArenaList()) s = makeArenaList(); s.proto->fromLetters(&s, "ab(cde(fg)h(ij))k(l)m");
    const typeof(makeStringBuffer()) S = makeStringBuffer(); const typeof(makeStringBuffer()) T = makeStringBuffer();

    typeof(s.proto->findFirst(&s, "f")) f = s.proto->findFirst(&s, "f");
    typeof(s.proto->findFirst(&s, "h")) h = s.proto->findFirst(&s, "h");

    int sub(ArenaListNode node, int start, int depth)
     {makeLocalCopyOfArenaListKey(k, l, node);
      S.proto->addFormat(&S, "start=%2d  depth=%d  %s\n", start, depth, k);
      return start < 0 && !strcmp(k, "k");                                             // Closing k
     }

      f.proto->scanFrom(&f, sub, 0);
    assert( S.proto->equalsString(&S, "start= 0  depth=3  f\n" "start= 0  depth=3  g\n" "start=-1  depth=2  e\n" "start= 1  depth=2  h\n" "start= 0  depth=3  i\n" "start= 0  depth=3  j\n" "start=-1  depth=2  h\n" "start=-1  depth=1  b\n" "start= 1  depth=1  k\n" "start= 0  depth=2  l\n" "start=-1  depth=1  k\n"));
//◉start= 0  depth=3  f
//◉start= 0  depth=3  g
//◉start=-1  depth=2  e
//◉start= 1  depth=2  h
//◉start= 0  depth=3  i
//◉start= 0  depth=3  j
//◉start=-1  depth=2  h
//◉start=-1  depth=1  b
//◉start= 1  depth=1  k
//◉start= 0  depth=2  l
//◉start=-1  depth=1  k
//◉

    int tub(ArenaListNode node, int start, int depth)
     {makeLocalCopyOfArenaListKey(k, l, node);
      T.proto->addFormat(&T, "start=%2d  depth=%d  %s\n", start, depth, k);
      return 0;                                                                 // Continue to bitter end
     }

    h.proto->scanFrom(&h, tub, 1);
    assert( T.proto->equalsString(&T, "start=-1  depth=2  h\n" "start=-1  depth=1  b\n" "start= 1  depth=1  k\n" "start= 0  depth=2  l\n" "start=-1  depth=1  k\n" "start= 0  depth=1  m\n"));
//◉start=-1  depth=2  h
//◉start=-1  depth=1  b
//◉start= 1  depth=1  k
//◉start= 0  depth=2  l
//◉start=-1  depth=1  k
//◉start= 0  depth=1  m
//◉
    s.proto->free(&s); S.proto->free(&S); T.proto->free(&T);
 }

void test21()                                                                   //Tclear //Tequals //TnotEquals  //Need clone__ArenaList equals__ArenaList
 {typeof(makeArenaList()) s = makeArenaList(); typeof(makeArenaList()) t = makeArenaList(); typeof(makeArenaList()) n = makeArenaList(); const typeof(10ul) N = 10ul; char c[N];

  for(size_t i = 0; i < N; ++i) c[i] = '0'+i%10;

  for  (size_t i = 0; i < N; ++i)
   {s.proto->clear(&s); t.proto->clear(&t); n.proto->clear(&n);

    for(size_t j = 0; j < N; ++j) s.proto->nodeLast(&s, c, j+1);

    assert( s.proto->printsAs(&s, "0010120123012340123450123456012345670123456780123456789\n"));
//◉0010120123012340123450123456012345670123456780123456789
//◉

    for(size_t j = N; j > 0; --j) t.proto->nodeFirst(&t, c, j);
    for(size_t j = N; j > 1; --j) n.proto->nodeLast(&n, c, j);

    assert( s.proto->equals(&s, t)); assert( s.proto->notEquals(&s, n)); assert( t.proto->notEquals(&t, n));
   }

  s.proto->free(&s); t.proto->free(&t); n.proto->free(&n);
 }

void test22()                                                                   //TnodeFirst //TnodeLast //TnodeNext //TnodePrev
 {typeof(makeArenaList()) s = makeArenaList();

  const typeof(s.proto->nodeFirst(&s, "a", 1)) a = s.proto->nodeFirst(&s, "a", 1);
  const typeof(s.proto->nodeLast(&s, "d", 1)) d = s.proto->nodeLast(&s, "d", 1);
      a.proto->nodeNext(&a, "b", 1);
      d.proto->nodePrev(&d, "c", 1);
      d.proto->nodeFirst(&d, "e", 1);
      d.proto->nodeLast(&d, "f", 1);

  assert( s.proto->printsWithBracketsAs(&s, "(abcd(ef))\n"));
//◉(abcd(ef))
//◉

  s.proto->free(&s);
 }

void test23()                                                                   //Tfiles //TFolders
 {typeof(makeArenaList()) D = makeArenaList(); typeof(makeArenaList()) F = makeArenaList();

  const typeof("") s = "";

  const typeof(F.proto->nodeFirst(&F, s, strlen(s))) f = F.proto->nodeFirst(&F, s, strlen(s)); f.proto->files(&f);   assert( F.proto->count(&F) > 0);
  const typeof(D.proto->nodeFirst(&D, s, strlen(s))) d = D.proto->nodeFirst(&D, s, strlen(s)); d.proto->folders(&d); assert( D.proto->count(&D) > 0);

  D.proto->free(&D); F.proto->free(&F);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0,  test1,  test2,  test3,  test4,
                           test5,  test6,  test7,  test8,  test9,
                           test10, test11, test12, test13, test14,
                           test15, test16, test17, test18, test19,
                           test20, test21, test22, test23, 0};
  run_tests("ArenaList", 1, tests);

  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaList/arenaList
