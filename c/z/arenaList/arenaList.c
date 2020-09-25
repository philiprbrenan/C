//------------------------------------------------------------------------------
// Lists held in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
/*
 A $ contains nodes and keys. Each node can contain a list of sub nodes. Each
 node has a key and a length field which is used to store the data held by each
 node. The nodes are ordered solely by the user: the key field does not affect
 the ordering of the nodes. The key can contain \0 and other binary data as the
 length of the key field is determined solely by the length field.

*/
#define _GNU_SOURCE
#include <setjmp.h>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an $.
#ifndef $_included
#define $_included

exports structs $ $Node $Arena $Description
exports arena    pointer_$_size content_$Node key_pointer_$Node length_size_$Node allocate_offset_$_size used_$ make$ make$WithWidth node_$Node_$_string nodeFromOffset_$_size equals_int_$Node_$Node equalsString_$Node_string width_size_$ width_size_$Node
exports nodeData key_pointer_$Node data_pointer_$Node length_size_$Node dump_$Node

typedef struct $                                                                // $
 {const struct ProtoTypes_$ *proto;                                             // Methods associated with an $
  struct $Arena *arena;                                                         // The arena for the $
 } $;

typedef struct $Content                                                         // The content of a node as stored in the $
 {unsigned int next, parent, first, last, prev, length;                         // Related nodes and the length of the key
  char         data[0];                                                         // The fixed data content of the node - the actual width of this field is held in arena.width
  char         key[0];                                                          // The content of the key
 } $Content;

typedef struct $Node                                                            // Offset to the content of a node in the $.
 {$      list;                                                                  // $ containing the item
  size_t offset;                                                                // Offset
  const struct ProtoTypes_$Node *proto;                                         // Methods associated with nodes
 } $Node;

typedef struct $Arena                                                           // Description of the block of memory that holds an $.  The individual nodes of the $ cannot be cannot be freed individually, but the entire $ can be freed. As the $ uses offsets to store addresses the $ can be moved to a different location in memory while retaining its structure.
 {size_t size;                                                                  // The total size of the arena used to hold data associated with the $.
  size_t used;                                                                  // The number of bytes currently used.
  size_t root;                                                                  // Offset to the root node
  size_t width;                                                                 // Width of a fixed data area added to each node measured in bytes
  void * data;                                                                  // The arena containing the data associated with the $
 } $Arena;

typedef struct $Description                                                     // The description of an $ which is written as the header record for dump files.
 {const size_t version;                                                         // Version of $
  const size_t littleEndian;                                                    // Little endian if true
  const size_t s64b;                                                            // 64 bit address if true, else 32 bit
  const size_t used;                                                            // The number of bytes currently used by the arena.
  const size_t root;                                                            // Offset to the root node
  const size_t width;                                                           // Width of user data associated with each node
  const time_t created;                                                         // Time in seconds since the epoch when this $ was created
 } $Description;

#include <$$_prototypes.h>                                                      // $ prototypes now that the relevant structures have been declared
#define $fe( child, parent) for($Node child = parent ▷ first; child ▷ valid; child = child ▷ next) // Each child in a parent from first to last
#define $fer(child, parent) for($Node child = parent ▷ last ; child ▷ valid; child = child ▷ prev) // Each child in a parent from last to first
#define makeLocalCopyOfArenaListKey(string,stringLength,node) const size_t stringLength = content_ArenaListNode(node)->length; char string[stringLength+1]; string[stringLength] = 0; memcpy(string, key_pointer_ArenaListNode(node), stringLength); // Copy the key and the length of the key of the specified node to the stack.

//D1 Constructors                                                               // Construct a new $.

static $ make$WithWidth                                                         // Create a new $ with the specified width for the optional user data associated with each node.
 (const size_t width)                                                           // Width
 {$Arena * const a = alloc(sizeof($Arena));                                     // Allocate arena description
  t ◁ new $(arena: a);                                                          // $ we are creating

  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  a->used   = 0;                                                                // Length used so far
  a->root   = 0;                                                                // Root not set in $
  a->width  = width;                                                            // Width of user data associated with each node
  t ▷ node("", 0);                                                              // Initialize root node
  return t;
 }

static $ make$                                                                  // Create a new $
 ()                                                                             // $allocator
 {return make$WithWidth(0);                                                     // Allocate arena description
 }

static void swap_$_$                                                            // Swap the arenas of two $
 ($ source,                                                                     // Source $
  $ target)                                                                     // Target $
 {s ◁ source.arena; t ◁ target.arena;                                           // Arenas
  $Arena u = *t; *t = *s; *s = u;
 }

static char * check_$                                                           //P Return a string describing the structure
 (const $ list)                                                                 // $
 {if (list.arena->data) {} return "$";
 }

//D1 Pointers and Offsets                                                       // Operations on pointers and offsets

static void * pointer_$_size                                                    //PV Return a temporary pointer to data in the arena of the specified $
 (const $      list,                                                            // $
  const size_t offset)                                                          // Offset
 {if (offset > list.arena->used)                                                // Check that the offset is valid
   {printStackBackTrace("Accessing area outside arena");
   }
  return (void *)(list.arena->data + offset);                                   // Convert a non zero delta that is within the arena to a valid pointer
 }

static $Content * content_$Node                                                 //PV Convert a node offset to an address so that the content of a node can be updated in situ as long as the $ is not reallocated to a different position.
 (const $Node node)                                                             // Node
 {return ($Content *)pointer_$_size(node.list, node.offset);
 }

static size_t width_size_$                                                      // Get the width of the data area for a $
 (const $ list)                                                                 // Node
 {return list.arena->width;
 }

static size_t width_size_$Node                                                  // Get the width of the data area for a node
 (const $Node node)                                                             // Node
 {return node.list.arena->width;
 }

static void * key_pointer_$Node                                                 //V Get a temporary pointer to the key of a node.
 (const $Node node)                                                             // Node
 {width ◁ node ▷ width;                                                         // Width of node
  return pointer_$_size(node.list, node.offset + sizeof($Content) + width);     // The key is stored after the node and optional user data in the arena.
 }

static void * data_pointer_$Node                                                //V Get a temporary pointer to the user data of a node.
 (const $Node node)                                                             // Node
 {return pointer_$_size(node.list, node.offset + sizeof($Content));             // The optional user data is stored immediately after the node in the arena.
 }

static size_t length_size_$Node                                                 // Get the length of the key associated with a node
 (const $Node node)                                                             // Node
 {return node ▷ content->length;
 }

static  $Node  nodeFromOffset_$_size                                            //P Create a node to locate an allocation within the arena of a $.
 (const $      list,                                                            // $
  const size_t delta)                                                           // Delta within arena. A delta of zero represents no such node.
 {return new $Node(list: list, offset: delta);
 }

//D1 Checks                                                                     // Check the validity of a node

static int keyEquals_int_$Node_pointer_size                                     // Confirm that the key of a node is equal to the specified memory
 (const $Node        node,                                                      // Node whose key is to be checked
  const char * const key,                                                       // Expected key value
  const size_t       length)                                                    // Length of expected key value
 {l ◁ node ▷ length;                                                            // Recorded length of node
  if (l != length) return 0;                                                    // Different lengths means the keys is unequal
  return !memcmp(node ▷ key, key, l);                                           // Compare memory
 }

static int equals_int_$Node_$Node                                               // Confirm two Nodes are equal
 (const $Node a,                                                                // First offset
  const $Node b)                                                                // Second offset
 {return a.list.arena == b.list.arena && a.offset == b.offset;
 }

//D1 Root node                                                                  // The root node of an $ is at offset 0

static  $Node root_$Node_$                                                      // Return the root node of a $.
 (const $ list)                                                                 // $
 {return new $Node(list: list);                                                 // Root node
 }

static  $Node root_$NodeOffset_$NodeOffset                                      // Return the root node of the $ containing the specified node.
 (const $Node node)                                                             // Node
 {return node.list ▷ root;
 }

//D1 Allocation                                                                 // Allocating memory in the $

static  $Node  allocate_offset_$_size                                           //P Allocate memory within the arena of a $ and clear the allocated memory
 (const $      list,                                                            // $ in which to allocate
  const size_t size)                                                            // Amount of memory required
 {if (list.arena->used + size < list.arena->size)                               // Allocate within existing arena
   {n ◁ list ▷ nodeFromOffset(list.arena->used);                                // Current offset to open memory
    list.arena->used += size;                                                   // Allocate
    return n;                                                                   // Return allocation
   }
  else                                                                          // Reallocate arena
   {S ◁ nextPowerOfTwo(list.arena->size + size);                                // Round up memory required
    if (S < ((size_t)((1l<<(sizeof((($Content *)0)->length)*8)) - 1)))          // The maximum size of a delta if there are 8 bits in a byte
     {m ◁ realloc(list.arena->data, S);                                         // Reallocate arena - the old one will be copied into it if it is moved.
      if (m)                                                                    // Retry the memory allocation
       {list.arena->data = m;                                                   // New arena
        list.arena->size = S;                                                   // Arena size
        u ◁ list.arena->used;                                                   // Length of free space
        memset(list.arena->data + u, 0, S - u);                                 // Clear free space
        return list ▷ allocate(size);                                           // Allocate within arena
       }
     }
   }
  printStackBackTraceAndExit(2, "Requested arena too large\n");                 // The arena has become too large for the chosen size of offsets.
 }

static size_t used_$                                                            // Amount of space currently being used within the arena of a $.
 (const $ list)                                                                 // $
 {return list.arena->used;
 }

static $Node node_$Node_$_string                                                // Create a new $ node with the specified key.
 (const $            list,                                                      // $ in which to create the node
  const void * const key,                                                       // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the $ is determined solely by the user.
  const size_t       length)                                                    // Length of the key, or if zero, I will use strlen
 {n ◁ list ▷ allocate(sizeof($Content) + length + list ▷ width);                // Offset of space allocated for a node and key
  c ◁ n ▷ content;                                                              // Address content
  c->length = length;                                                           // Save key length
  memcpy(n ▷ key, key, length);                                                 // Copy in key
  return n;                                                                     // Return node
 }

static void setKey_$Node_pointer                                                // Set the key of a node to a different value as long as there is enough room from the last setting.
 (const $Node        node,                                                      // Node
  const void * const key,                                                       // Key
  const size_t       length)                                                    // Length of key
 {l ◁ node ▷ length;                                                            // Length of existing key
  if (length <= l)                                                              // There is enough room in the existing key for the new key
   {memcpy(node ▷ key, key, length);                                            // Copy in the key
    node ▷ content -> length = length;                                          // Set key length
    return;
   }
  printStackBackTraceAndExit(1, "Attempt to set longer key\n");                 // Error- no room for the key
 }

static void setData_$Node_pointer                                               // Set the optional user data associated with a node in an $
 (const $Node        node,                                                      // Node
  const void * const data)                                                      // Data
 {memcpy(node ▷ data, data, node ▷ width);                                      // Copy in the data
 }

static void getData_$Node_pointer                                               // get the optional user data associated with a node in an $
 (const $Node  node,                                                            // Node
  void * const data)                                                           // Data
 {memcpy(data, node ▷ data, node ▷ width);                                      // Copy out the data
 }

static void fromLetters_$_string                                                // Load $ from a string of letters and brackets.  The root node of the $ so constructed is always given the letter 'a'.
 (const $      list,                                                            // $
  const char * str)                                                             // String of letters and brackets describing desired list structure
 {const $Node n = list ▷ root;                                                  // The node we are currently  adding to

  void parse($Node current)                                                     // Parse the string of letters and brackets to construct the desired $
   {while(*str)
     {c ◁ *str;
      switch(c)
       {case '(':                                                               // Enter new parent scope
         {++str;
          parse(current ▷ last);
          break;
         }
        case ')':                                                               // Exit current parent scope
         {++str; return;
         }
        default:                                                                // Add to current parent
         {current ▷ putLast(list ▷ node(&c, 1));
          ++str; continue;
         }
       }
     }
   }

  parse(n);                                                                     // Parse the string of letters and brackets to construct the desired $
 };

static void free_$                                                              // Free an entire $.
 (const $ list)                                                                 // $ to free
 {a ◁ list.arena;
  if  (a->data) free(a->data);
  free(a);
 }

//D1 Navigation                                                                 // Navigate through a $.

static int valid_$Node                                                          // Check that a node is valid.
 (const $Node child)                                                            // Node
 {return child.offset;                                                          // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

static  $Node parent_$Node_$Node                                                // Get the parent of a child
 (const $Node child)                                                            // Child
 {if (child ▷ isRoot) return child;
  return child.list ▷ nodeFromOffset(child ▷ content->parent);
 }

static  $Node first_$Node_$Node                                                 // Get the first child under a parent.
 (const $Node parent)                                                           // Parent
 {return  parent.list ▷ nodeFromOffset(parent ▷ content->first);
 }
duplicate s/first/last/g s/first/next/g s/first/prev/g

static  $Node first_$Node_$                                                     // Get the first child in the specified $.
 (const $ list)                                                                 // Parent
 {const $Node root = list ▷ root;
  return root ▷ first;
 }
duplicate s/first/last/g s/first/next/g s/first/prev/g

//D1 Search                                                                     // Search for nodes.

static int equalsString_$Node_string                                            // Check that the key of a node equals a string
 (const $Node        node,                                                      // Node
  const char * const key)                                                       // Key
 {return node ▷ keyEquals(key, strlen(key));
 }

static  $Node findFirst_$Node_string                                            // Find the first node with the specified key in a post-order traversal of the $ starting at the specified node.
 (const $Node        node,                                                      // Node at the start of the list to be searched
  const char * const key)                                                       // Key to find
 {jmp_buf found;
  $Node F = new $Node(list: node.list);                                         // An invalid node

  void find($Node node)                                                         // Check whether the key of the current node matches the specified key
   {if (node ▷ equalsString(key))                                               // Found
     {F.offset = node.offset;
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) node ▷ by(find);                                          // Search the $

  return F;                                                                     // Return an invalid node
 }

static  $Node findFirst_$_string                                                // Find the first node with the specified key in a post-order traversal of the $.
 (const $            list,                                                      // $
  const char * const key)                                                       // Key to find
 {jmp_buf found;
  $Node F = new $Node(list: list);                                              // An invalid node

  void find($Node node)                                                         // Check whether the key of the current node matches the specified key
   {if (node ▷ equalsString(key))                                               // Found
     {F.offset = node.offset;
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) list ▷ by(find);                                          // Search the $

  return F;                                                                     // Return an invalid node
 }

static  $Node findFirstChild_$Node_string                                       // Find the first child of the specified parent with the specified key.
 (const $Node        parent,                                                    // Parent node
  const char * const key)                                                       // Key to find immediately under parent
 {$fe(child, parent) if (child ▷ equalsString(key)) return child;               // Found matching child

  const $Node invalid = {};                                                     // An invalid node
  return invalid;                                                               // Failed - return an invalid node
 }

static  $Node findFirstChild_$_string                                           // Find the first child immediately under the root with the specified key.
 (const $            list,                                                      // $
  const char * const key)                                                       // Key to find
 {const $Node  r = list ▷ root;                                                 // Root node of the $
  return r ▷ findFirstChild(key);                                               // Search the $
 }

//D1 Location                                                                   // Verify the current location.

static int context_$Node                                                        // Return true if the parent of the specified child has the specified key.
 (const $Node         child,                                                    // Child
        $Node * const parent,                                                   // Parent container
  const char  * const key)                                                      // Key
 {if (child ▷ isRoot) return 0;                                                 // The root node has no context
  p ◁ *parent = child ▷ parent;
  makeLocalCopyOf$Key(k, l, p);
  return p ▷ valid && !memcmp(k, key, l);                                       // Check key
 }

static int isFirst_$Node                                                        // Confirm a child is first under its parent
 (const $Node child)                                                            // Child
 {const $Node parent = child ▷ parent;
  return child ▷ equals(parent ▷ first);
 }
duplicate s/First/Last/,s/first/last/

static int isEmpty_$Node                                                        // Confirm a node has no children.
 (const $Node node)                                                             // Node
 {const $Node child = node ▷ first;
  return !child ▷ valid;                                                        // No first child so the node is empty
 }

static int isOnlyChild_$Node                                                    // Confirm that this child is the only child of its parent
 (const $Node child)                                                            // Child
 {parent ◁ child ▷ parent;                                                      // Parent
  return parent ▷ valid && child ▷ isFirst && child ▷ isLast;                   // Child is first and last and not the root so it is an only child
 }

static int isRoot_$Node                                                         // Confirm a node is the root
 (const $Node node)                                                             // NodeContent
 {return !node.offset;
 }

//D1 Put                                                                        // Insert children into a $.

static  $Node putFL_$Node_$Node_$Node                                           //P Put a child first or last under its parent
 (const int   first,                                                            // Put first if true, else last
  const $Node parent,                                                           // Parent
  const $Node child)                                                            // Child
 {list ◁ parent.list;
  if (child.list.arena != list.arena) printStackBackTrace("Wrong list\n");      // Parent and child must be in same $
  P ◁ parent.offset;    C ◁ child.offset;                                       // Delta of parent and child
  p ◁ parent ▷ content; c ◁ child  ▷ content;                                   // Pointers to parent and child
  L ◁ first ? p->first : p->last;                                               // Delta of child currently first/last
  if (!L)                                                                       // No children yet
   {if (first) p->last = C; else p->first = C;                                  // Delta to last/first child
   }
  else                                                                          // Existing children
   {$Content * const l = list ▷ pointer(L);                                     // Link first/last two children together
    if (first) l->prev = C; else l->next = C;
    if (first) c->next = L; else c->prev = L;
   }
  c->parent = P;                                                                // Parent new first/last child
  if (first)  p->first = C; else p->last = C;                                   // Parent points to new first/last child
  return child;
 }

static  $Node putTreeFirst_$Node_$Node                                          // Put a child first in the $ containing the arena in which the child was allocated.
 (const $Node child)                                                            // Child
 {t ◁ child.list;                                                               // $ containing arena containing child
  r ◁ t ▷ root;
  return r ▷ putFirst(child);                                                   // Put the child first
 }
duplicate s/First/Last/g,s/first/last/g,

static  $Node putFirst_$Node_$Node_$Node                                        // Put a child first under its parent
 (const $Node parent,                                                           // Parent
  const $Node child)                                                            // Child
 {return putFL_$Node_$Node_$Node(1, parent, child);                             // Put a child first under its parent
 }
duplicate s/First/Last/g,s/first/last/g,s/1/0/

static  $Node putNP_$Node_$Node_$Node                                           //P Put a child next or previous to the specified sibling
 (const int   next,                                                             // Put next if true, else previous
  const $Node sibling,                                                          // Sibling
  const $Node child)                                                            // Child
 {list   ◁ sibling.list;                                                        // $
  parent ◁ sibling ▷ parent;                                                    // Parent node
  twin   ◁ next ? sibling ▷ next : sibling ▷ prev;                              // Next or prev sibling to sibling is such a node exists
  assert(child.list.arena == list.arena);                                       // Parent and child must be in same $
  s ◁ sibling ▷ content;                                                        // Pointer to sibling
  c ◁ child   ▷ content;                                                        // Pointer to child

  if (!twin ▷ valid)                                                            // Sibling is last/first under parent
   {if (next) parent ▷ putLast(child); else parent ▷ putFirst(child);           // Place child
   }
  else                                                                          // Not last/first
   {w ◁ twin ▷ content;                                                         // Pointer to twin of sibling
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

static void setUp_$Node_$Node                                                   //P Make the specified parent node the parent of the specified child node
 (const $Node child,                                                            // Child
  const $Node parent)                                                           // Child
 {child ▷ content->parent = parent.offset;                                      // Set parent of child
 }

static  $Node putNext_$Node_$Node_$Node                                         // Put a child next after the specified sibling
 (const $Node sibling,                                                          // Sibling
  const $Node child)                                                            // Child
 {return putNP_$Node_$Node_$Node(1, sibling, child);                            // Put child next
 }
duplicate s/Next/Prev/g,s/next/previous/g,s/1/0/

//D1 Traverse                                                                   // Traverse a $.

static void by_$Node_sub                                                        // Traverse the $ rooted at the specified node in post-order calling the specified function to process each child node.  The $ is buffered allowing changes to be made to the structure of the $ without disruption as long as each child checks its context.
 ($Node node,                                                                   // Node
  void (* const function) (const $Node node))                                   // Function
 {void children(const $Node parent)                                             // Process the children of the specified parent
   {if (!parent ▷ valid) return;                                                // Empty child
    N ◁ parent ▷ countChildren;                                                 // Number of children
    size_t c[N+1];                                                              // Array of children terminated by a trailing zero
    size_t n = 0; $fe(child, parent) c[n++] = child.offset;                     // Load each child into an array
    for(size_t i = 0; i < N; ++i)                                               // Process each child allowing it to change its position without changing the traversal
     {children(new $Node(list: parent.list, offset: c[i]));
     }
    function(parent);                                                           // Process the parent
   }
  children(node);                                                               // Start at the specified root node
 }

static void by_$_sub                                                            // Traverse a $ in post-order calling the specified function to process each child node.  The $ is buffered allowing changes to be made to the structure of the $ without disruption as long as each child checks its context.
 (const $ list,                                                                 // $
  void (* const function) (const $Node node))                                   // Function
 {root ◁ list ▷ root;                                                           // Root node
  N    ◁ list ▷ countChildren;                                                  // Number of children under root
  size_t c[N+1];                                                                // Array of children terminated by a trailing zero
  size_t n = 0; $fe(child, root) c[n++] = child.offset;                         // Load each child into an array
  for(size_t i = 0; i < N; ++i)
   {$Node n = new $Node(list: list, offset: c[i]);                              // Process each child allowing it to change position
    n ▷ by(function);
   }
  function(root);
 }

static  size_t countChildren_size_$                                             // Count the number of children directly under a parent.
 (const $ list)                                                                 // $
 {size_t l = 0;
  root ◁ list ▷ root;
  $fe(child, root) ++l;
  return l;                                                                     // Return count
 }

static  size_t countChildren_size_$Node                                         // Count the number of children directly under a node.
 (const $Node parent)                                                           // Parent
 {size_t l = 0;
  $fe(child, parent) ++l;
  return l;                                                                     // Return count
 }

static  size_t count_size_$Node                                                 // Count the number of nodes under a specified node but not including the specified node.
 (const $Node node)                                                             // Node
 {size_t l = 0;

  void count(const $Node parent)                                                // Process the children of the specified parent
   {if (!parent ▷ valid) return;
    l++; $fe(child, parent) count(child);                                       // Each child
   }

  count(node);                                                                  // Start at the specified node
  return l - 1;                                                                 // Return count without counting the root node
 }


static  size_t count_size_$                                                     // Count the number of nodes in a $
 (const $ list)                                                                 // $
 {size_t l = 0;

  root ◁ list ▷ root;                                                           // Root is always invalid
  $fe(child, root) l += 1 + child ▷ count;                                      // Child under root plus its children
  return l;                                                                     // Return count without counting the root node
 }

//D1 Print                                                                      // Print $s in various ways

static void printWithBrackets_$Node_function                                    // Apply a function to a string containing a print of the specified  node and the $ below it in preorder as a string with each sub $ enclosed in brackets.
 (const $Node   node,                                                           // Node
  void (*printer)(char * string, size_t length))                                // Function to apply to printed $
 {size_t l = 0;                                                                 // Length of output string
  void len(const $Node parent)                                                  // Find the size of buffer we will need
   {l += parent ▷ length;                                                       // Parent key
    if (!parent ▷ isEmpty)                                                      // Parent has children
     {l += 2;                                                                   // Space for brackets
      $fe(child, parent) len(child);                                            // Each child
     }
   }

  char * p;
  void print(const $Node parent)                                                // Print the children of the specified parent
   {makeLocalCopyOf$Key(k, l, parent);                                          // Local copy of key
    memcpy(p, k, l); p += l;                                                    // Copy key into print string
    if (!parent ▷ isEmpty)                                                      // Parent has children
     {*p++ = '(';                                                               // Start children
      $fe(child, parent) print(child);                                          // Each child
      *p++ = ')';                                                               // Start children
     }
    *p = 0;                                                                     // End the string
   }

  len(node); char P[l+1]; P[0] = 0; p = P; print(node);                         // Print in local string
  printer(P, l);                                                                // Apply function
 }

static void printWithBrackets_string_$                                          // Print an entire $ in preorder as a string with brackets around each sub $
 (const $ list,                                                                 // $
  void (*printer)(char * string, size_t length))                                // Function to apply to printed $
 {node ◁ list ▷ root;                                                           // Root
  node ▷ printWithBrackets(printer);
 }

static int printsWithBracketsAs_int_$Node_string                                // Check that the $ starting at the specified node prints with brackets as expected.
 (const $Node        node,                                                      // Node
  const char * const expected)                                                  // Expected string when printed
 {int r;
  void printer(char * s, size_t l)
   {r = !strncmp(s, expected, l);
   }
  node ▷ printWithBrackets(printer);
  return r;
 }

static int printsWithBracketsAs_int_$_string                                    // Check that the specified $ prints with brackets as expected.
 (const $            list,                                                      // $
  const char * const expected)                                                  // Expected string when printed
 {       root ◁ list ▷ root;                                                    // Root
  return root ▷ printsWithBracketsAs(expected);
 }

static void dump_$                                                              //P Dump a $ on stderr
 (const $ list)                                                                 // $
 {size_t n = 0;
  void print(const $Node parent, int depth)                                     // Print the children of the specified parent
   {makeLocalCopyOf$Key(k, l, parent);                                          // Local copy of key
    say("%lu ", ++n);                                                           // Print key number
    for(int i = 0; i < depth; ++i) say("  ");                                   // Spacer
    say("%s\n", k);                                                             // Print key
    if (!parent ▷ isEmpty) $fe(child, parent) print(child, depth+1);            // Each child
   }

  root ◁ list ▷ root;                                                           // Root
  print(root, 0);
 }

static void dump_$Node                                                          //P Dump a $Node on stderr
 (const $Node node)                                                             // $Node
 {makeLocalCopyOf$Key(k, l, node);                                              // Local copy of key
  say("%lu %s\n", l, k);                                                        // Print key number
 }

static void print_$Node_function                                                // Apply a function to the print of a $.
 (const $Node   node,                                                           // Node
  void (*printer)(char * string, size_t length))                                // Function to apply to printed $
 {size_t l = 0;                                                                 // Length of output string
  void len(const $Node parent)                                                  // Find the size of buffer we will need
   {l += parent ▷ length;                                                       // Parent key
    if (!parent ▷ isEmpty) $fe(child, parent) len(child);                       // Each child
   }

  char * p;
  void print(const $Node parent)                                                // Print the children of the specified parent
   {makeLocalCopyOf$Key(k, l, parent);                                          // Local copy of key
    memcpy(p, k, l); p += l;
    if (!parent ▷ isEmpty) $fe(child, parent) print(child);                     // Each child
    *p = 0;                                                                     // End the string
   }

  len(node); char P[l+1]; P[0] = 0; p = P; print(node);                         // Print in local string
  printer(P, l);                                                                // Apply function
 }

static int printsAs_int_$Node_string                                            // Check that the specified node prints as expected.
 (const $Node        node,                                                      // $
  const char * const expected)                                                  // Expected string when printed
 {int r;
  void printer(char * s, size_t l)
   {r = !strncmp(s, expected, l);
   }
  node ▷ print(printer);
  return r;
 }

static int printsAs_int_$_string                                                // Check that the specified $ prints as expected.
 (const $            list,                                                      // $
  const char * const expected)                                                  // Expected string when printed
 {int r;
  void printer(char * s, size_t l)
   {r = !strncmp(s, expected, l);
   }
  root ◁ list ▷ root;                                                           // Print from root of $
  root ▷ print(printer);
  return r;
 }

static int printContains_$Node                                                  // Check the print of an $ starting at the specified tag contains the expected string.
 (const $Node   node,                                                           // Starting node
  const char *  expected)                                                       // Expected string
 {int r;
  void printer(char * s, size_t l)
   {r = !!strstr(s, expected); if(0)l=l;
   }
  node ▷ print(printer);
  return r;
 }

static int printContains_$                                                      // Check the print of an $ contains the expected string.
 (const $       list,                                                           // $ parse $
  const char *  expected)                                                       // Expected string
 {int r;
  void printer(char * s, size_t l)
   {r = !!strstr(s, expected); if(0)l=l;
   }
  root ◁ list ▷ root;                                                           // Print from root of $
  root ▷ print(printer);
  return r;
 }

//D1 Edit                                                                       // Edit a $ in situ.

static  $Node cut_$Node_$Node                                                   // Cut out a child.
 (const $Node child)                                                            // Child to cut out
 {const $Node parent = child  ▷ parent;                                         // Parent
  p ◁ parent ▷ content; c ◁ child ▷ content;                                    // Parent pointer, child content

  if (child ▷ isOnlyChild)                                                      // Only child
   {p->first = p->last = 0;                                                     // Remove child
   }
  else if (child ▷ isLast)                                                      // Last child
   {L ◁ child  ▷ prev;
    l ◁ L ▷ content;
    p->last = L.offset; l->next = 0;                                            // Last child under parent
   }
  else if (child ▷ isFirst)                                                     // First child
   {F ◁ child ▷ next;
    f ◁ F     ▷ content;
    p->first  = F.offset; f->prev = 0;                                          // Last child under parent
   }
  else                                                                          // Some where in the middle
   {N ◁ child ▷ next;    P ◁ child ▷ prev;
    n ◁     N ▷ content; p ◁     P ▷ content;
    p->next   = N.offset; n->prev  = P.offset;
   }

  c->next = c->prev = 0;                                                        // Remove child

  return child;
 }

static  $Node unwrap_$Node_$Node                                                // Unwrap the specified parent and return it.
 (const $Node parent)                                                           // Parent to unwrap
 {for($Node child = new $Node(list: parent.list, offset: parent ▷ last.offset); // Remove last children
    child ▷ valid;
    child.offset = parent ▷ last.offset)
   {parent ▷ putNext(child ▷ cut);                                              // Place each child after the parent
   }
  return parent ▷ cut;                                                          // Remove and return empty parent
 }

static  $Node wrap_$Node_string                                                 // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
 (const $Node   child,                                                          // Child to wrap
  const char * const key)                                                       // Key for new parent
 {list   ◁ child.list;                                                          // Tree
  parent ◁ list ▷ node(key, strlen(key));                                       // New parent
  child  ▷ putNext(parent);                                                     // Place parent after child
  parent ▷ putLast(child ▷ cut);                                                // Place child under parent
  return parent;
 }

//D1 Input and Output                                                           // Read and write a $ from/to a file

static void write_void_$_string                                                 // Write a $ to a named file or abort
 (const $       list,                                                           // $
  const char * const file)                                                      // File
 {    o ◁ open(file, O_CREAT| O_WRONLY, S_IRWXU);                               // Open for output creating if needed
  if (o < 0) printStackBackTrace("Cannot open file: %s for write\n", file);

  time_t current; time(&current);
  const $Description h =                                                        // Create $ header describing the $
   {1, 1, 0, list ▷ used, list ▷ root.offset, list ▷ width, current};

  if (sizeof(h) != write(o, &h, sizeof(h)))                                     // Write header
   {printStackBackTrace("Cannot write $ header to file: %s\n", file);
   }

  w ◁ write(o, list.arena->data, list.arena->used);                             // Write arena
  if (w < 0 || list.arena->used != (size_t)w)
   {printStackBackTrace("Cannot write $ arena to file: %s\n", file);
   }

  close(o);
 }

$ read$                                                                         // Read a $ from a file
 (const char * const file)                                                      // File
 {$Arena * const arena = alloc(sizeof($Arena));                                 // Create arena
  list ◁ new $(arena: arena);                                                   // Initialize $

      i ◁ open(file, 0, O_RDONLY);                                              // Open for input
  if (i < 0) printStackBackTrace("Cannot open file: %s for read\n", file);

  struct $Description h;
  if (sizeof(h) != read(i, &h, sizeof(h)))                                      // Read header
   {printStackBackTrace("Cannot read header from file: %s\n", file);
   }

  list.arena->data = alloc(arena->size = arena->used = h.used);                 // Allocate arena

      r ◁ read(i, arena->data, arena->used);                                    // Read arena
  if (r < 0 || arena->used != (size_t)r)
   {printStackBackTrace("Cannot read $ from file: %s\n", file);
   }

  close(i);
  arena->root = h.root; arena->width = h.width;                                 // Offset to root in arena
  return list;
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //Tmake$ //Tnode //Tfree //TputFirst //TputLast //Tfe //Tfer
 {   t ◁ make$();                                                               // Create a $
  root ◁ t ▷ root;

  for(int i = 0; i < 10; ++i)                                                   // Load $
   {char c = 'a'+i;

    c1 ◁ t ▷ node(&c, 1); root ▷ putLast (c1);
    c2 ◁ t ▷ node(&c, 1); root ▷ putFirst(c2);
    c = 'A'+i; c2 ▷ setKey(&c, 1);
    makeLocalCopyOf$Key(k2, l2, c2);
    ✓k2[0] == c;
   }

  if (1)                                                                        // For each
   {char l[t ▷ count + 1]; *l = 0;
    $fe (child, root) strncat(l, child ▷ key, child ▷ length);

    ✓ !strcmp(l, "JIHGFEDCBAabcdefghij");
   }

  if (1)                                                                        // For each in reverse
   {char l[t ▷ count + 1]; *l = 0;
    $fer(child, root) strncat(l, child ▷ key, child ▷ length);
    ✓strcmp(l, "jihgfedcbaABCDEFGHIJ") == 0;
   }

  t ▷ free;
 }

void test1()                                                                    //Troot //Tfirst //Tlast //Tnext //Tprev //Tparent //Tequals //Tprint //TprintWithBrackets //TfromLetters //TprintsAs
 {t ◁ make$();    t ▷ fromLetters("b(c(de)f)g(hi)j");
  ✓t ▷ printsWithBracketsAs("(b(c(de)f)g(hi)j)");
  ✓t ▷ printsAs("bcdefghij");

  a ◁ t ▷ root;
  b ◁ a ▷ first;
  c ◁ b ▷ first;
  e ◁ c ▷ last;
  d ◁ e ▷ prev;

  char * k = d ▷ key;
  ✓k[0] == 'd';
  b ▷ printsWithBracketsAs("b(c(de)f)");
  c ▷ printsWithBracketsAs(  "c(de)");

  ✓c ▷ equals(d ▷ parent);
  ✓b ▷ equals(c ▷ parent);
  ✓a ▷ equals(b ▷ parent);
  ✓a ▷ equals(t ▷ root);

  ✓c ▷ printsAs("cde");
  ✓t ▷ printsAs("bcdefghij");

  t ▷ free;
 }

void test2()                                                                    //Tby //TprintsWithBracketsAs //TprintContains
 {t ◁ make$();     t ▷ fromLetters("b(c(de)f)g(hi)j");
  ✓t ▷ printsWithBracketsAs("(b(c(de)f)g(hi)j)");
  ✓t ▷ printContains("def");

  char l[t ▷ count + 2], *p = l;

  void process($Node n)
   {makeLocalCopyOf$Key(k, l, n);
    strncpy(p, k, l); p += l; *p = 0;
   }

  t ▷ by(process);
  ✓strcmp(l, "decfbhigj") == 0;

  t ▷ free;
 }

void test3()                                                                    //TisFirst //TisLast //TisEmpty //TisRoot //TisOnlyChild //Tcontext //TcheckKey //Tvalid
 {t ◁ make$();     t ▷ fromLetters("b(c(de(f)gh)i)j");
  ✓t ▷ printsWithBracketsAs("(b(c(de(f)gh)i)j)");

  a ◁ t ▷ root;  ✓a ▷ equalsString("");                                  ✓a ▷ printsWithBracketsAs( "(b(c(de(f)gh)i)j)");
  b ◁ a ▷ first; ✓b ▷ equalsString("b"); ✓a ▷ equals(b ▷ parent); ✓b ▷ printsWithBracketsAs( "b(c(de(f)gh)i)");
  c ◁ b ▷ first; ✓c ▷ equalsString("c"); ✓b ▷ equals(c ▷ parent); ✓c ▷ printsWithBracketsAs(   "c(de(f)gh)");
  d ◁ c ▷ first; ✓d ▷ equalsString("d"); ✓c ▷ equals(d ▷ parent); ✓d ▷ printsWithBracketsAs(     "d");
  e ◁ d ▷ next;  ✓e ▷ equalsString("e"); ✓c ▷ equals(e ▷ parent); ✓e ▷ printsWithBracketsAs(      "e(f)");
  f ◁ e ▷ last;  ✓f ▷ equalsString("f"); ✓e ▷ equals(f ▷ parent); ✓f ▷ printsWithBracketsAs(        "f");
  g ◁ e ▷ next;  ✓g ▷ equalsString("g"); ✓c ▷ equals(g ▷ parent); ✓g ▷ printsWithBracketsAs(          "g");
  h ◁ g ▷ next;  ✓h ▷ equalsString("h"); ✓c ▷ equals(h ▷ parent); ✓h ▷ printsWithBracketsAs(           "h");
  i ◁ c ▷ next;  ✓i ▷ equalsString("i"); ✓b ▷ equals(i ▷ parent); ✓i ▷ printsWithBracketsAs(             "i");
  j ◁ b ▷ next;  ✓j ▷ equalsString("j"); ✓a ▷ equals(j ▷ parent); ✓j ▷ printsWithBracketsAs(               "j");

  ✓ !a ▷ valid;
  ✓  b ▷ isFirst;
  ✓  j ▷ isLast;
  ✓  f ▷ isFirst;
  ✓  f ▷ isLast;

  ✓  f ▷ isEmpty;
  ✓  f ▷ isOnlyChild;
  ✓ !e ▷ isOnlyChild;
  ✓  a ▷ isRoot;

  $Node A, C, E;
  ✓  f ▷ context(&E, "e");
  ✓  E ▷ context(&C, "c");
  ✓ !a ▷ context(&A, "a");

  t ▷ free;
 }

void test4()                                                                    //Tcut //TfindFirst //TcountChildren //TequalsString //TfindFirstChild
 {t ◁ make$();     t ▷ fromLetters("b(c(de(f)gh)i)j");
  ✓t ▷ printsWithBracketsAs("(b(c(de(f)gh)i)j)");

  c ◁ t ▷ findFirst("c");

  ✓c ▷ equalsString("c");
  d ◁ t ▷ findFirst("d"); ✓d ▷ equalsString("d");
  e ◁ t ▷ findFirst("e"); ✓e ▷ equalsString("e");
  f ◁ t ▷ findFirst("f"); ✓f ▷ equalsString("f");
  g ◁ t ▷ findFirst("g"); ✓g ▷ equalsString("g");
  h ◁ t ▷ findFirst("h"); ✓h ▷ equalsString("h");

  ✓g ▷ equals(c ▷ findFirstChild("g"));
  ✓c ▷ countChildren == 4;
  ✓e ▷ countChildren == 1;

  f ▷ cut;           ✓t ▷ printsWithBracketsAs("(b(c(degh)i)j)");
  e ▷ putFirst(f);   ✓t ▷ printsWithBracketsAs("(b(c(de(f)gh)i)j)");

  e ▷ cut;           ✓t ▷ printsWithBracketsAs("(b(c(dgh)i)j)");
  d ▷ putNext(e);    ✓t ▷ printsWithBracketsAs("(b(c(de(f)gh)i)j)");

  d ▷ cut;           ✓t ▷ printsWithBracketsAs("(b(c(e(f)gh)i)j)");
  e ▷ putPrev(d);    ✓t ▷ printsWithBracketsAs("(b(c(de(f)gh)i)j)");

  h ▷ cut;           ✓t ▷ printsWithBracketsAs("(b(c(de(f)g)i)j)");
  g ▷ putNext(h);    ✓t ▷ printsWithBracketsAs("(b(c(de(f)gh)i)j)");

  t ▷ free;
 }

void test5()                                                                    //TreadArenaTree //Twrite
 {t ◁ make$();     t ▷ fromLetters("b(c(de(f)gh)i)j");
  ✓t ▷ printsWithBracketsAs("(b(c(de(f)gh)i)j)");

   f ◁ "/tmp/arenaTreeTest.data";
  ✓t ▷ printsWithBracketsAs("(b(c(de(f)gh)i)j)");
   t ▷ write(f);

   u ◁ read$(f);
  ✓u ▷ printsWithBracketsAs("(b(c(de(f)gh)i)j)");

  t ▷ free;
  u ▷ free;
  unlink(f);
 }

void test6()                                                                    //Tunwrap //Twrap //Tcount
 {t ◁ make$();     t ▷ fromLetters("bce");
  ✓ t ▷ printsWithBracketsAs("(bce)");
  ✓ t ▷ count == 3;

         c ◁ t ▷ findFirst("c");
  ✓ c ▷ valid;

    d ◁ c ▷ wrap("d");
  ✓ t ▷ printsWithBracketsAs("(bd(c)e)");
  ✓ t ▷ count == 4;

    d ▷ unwrap;
  ✓ t ▷ printsWithBracketsAs("(bce)");
  ✓ t ▷ count == 3;
    t ▷ free;
 }

void test7()                                                                    //TputNext //TputPrev
 {t ◁ make$();    t ▷ fromLetters("A");
              a ◁ t ▷ root;
          A ◁ a ▷ first;
  ✓A ▷ equalsString("A");

  for(int i = 0; i < 10; ++i)
   {char c = '0'+i;
    A ▷ putNext(t ▷ node(&c, 1));
    A ▷ putPrev(t ▷ node(&c, 1));
   }

  ✓t ▷ printsWithBracketsAs("(0123456789A9876543210)");

  t ▷ free;
 }

void test8()                                                                    //TputTreeFirst //TputTreeLast //TsetKey //Tkey //Tlength //Tused //TkeyEquals
 {t ◁ make$();

  c ◁ t ▷ node("c", 1); c ▷ putTreeFirst;
  d ◁ t ▷ node("d", 1); d ▷ putTreeLast;
  b ◁ t ▷ node("b", 1); b ▷ putTreeFirst;

  b ▷ setKey("B", 1);
  ✓ b ▷ length == 1;
  ✓ *(char *)(b ▷ key) == 'B';
  ✓ b ▷ keyEquals("B", 1);

  ✓ t ▷ used == 99;

  ✓ t ▷ printsWithBracketsAs("(Bcd)");

  t ▷ free;
 }

void test9()                                                                    //Tswap
 {s ◁ make$(); s ▷ fromLetters("s");
  t ◁ make$(); t ▷ fromLetters("t");
  ✓s ▷ printsWithBracketsAs("(s)");
  ✓t ▷ printsWithBracketsAs("(t)");
  s ▷ swap(t);
  ✓t ▷ printsWithBracketsAs("(s)");
  ✓s ▷ printsWithBracketsAs("(t)");
  s ▷ free;
  t ▷ free;
 }

void test10()                                                                   //Tdata //Tmake$WithWidth //Twidth //TgetData //TsetData
 {size_t D[] = {0,1,2,3};
  t ◁ make$WithWidth (sizeof(D));
  ✓t ▷ width == sizeof(D);

  n ◁ t ▷ first;
  ✓n ▷ width == sizeof(D);

  for(size_t i = 0; i < 10; ++i)
   {char c = '0'+i;
    n ◁ t ▷ node(&c, 1);
    n ▷ putTreeLast;
    n ▷ setData(D);
   }

  root ◁ t ▷ root;
  $fe(n, root)
   {char d[sizeof(D)]; n ▷ getData(d);
    ✓!memcmp(d,        D, sizeof(D));
    ✓!memcmp(n ▷ data, D, sizeof(D));
   }

  ✓t ▷ printsWithBracketsAs("(0123456789)");
  t ▷ free;
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0,  test1,  test2,  test3,  test4,
                           test5,  test6,  test7,  test8,  test9,
                           test10, 0};
  run_tests("$", repetitions, tests);

  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaList/arenaList
