//------------------------------------------------------------------------------
// Trees held in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <setjmp.h>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Tree.
#ifndef $_included
#define $_included

exports structs $ $Delta $Node $Offset $Arena $Description
exports arena   pointer_$_size allocate_offset_$_size pointer_$Offset size_$ used_$ saveString_$Offset_$_$String node_$Node_$_$String noden_$Node_$_$String nodeFromOffset_$_size content_$Node offset_$_size make$ new$Offset setData_$Node_size getData_size_$Node equals_int_$Node_$Node equalsString_$Node_string

typedef char * $String;                                                         // Arena Tree string
typedef struct StringBuffer  StringBuffer;

typedef struct $                                                                // Arena Tree.
 {const struct ProtoTypes_$ *proto;                                             // Methods associated with an arena tree
  struct $Arena *arena;                                                         // The arena for the arena tree
 } $;

typedef struct $Delta                                                           // The amount an item is offset within a tree.
 {unsigned int delta;
 } $Delta;

typedef struct $Content                                                         // A tree node in the arena
 {$Delta parent;                                                                // Offset to the parent of this node or zero if this is the root of the tree
  $Delta first, last, next, prev;                                               // Offsets to the first, last children of this node, amd the next and previous siblings of this node.
  $Delta key;                                                                   // The name of this node represented by a zero terminated string at the specified offset in the arena. Nodes are not ordered by their keys, their order is determined solely by the user. Keys can be reused between nodes.
  $Delta data;                                                                  // The data carried by this node.
 } $Content;

typedef struct $Offset                                                          // Offset to any item in the tree.  The caller is responsible for interpreting the content of the memory so addressed.
 {const $      tree;                                                            // ArenaTree containing the item
  const size_t offset;                                                          // Offset
  const struct ProtoTypes_$Offset *proto;                                       // Methods associated with offsets
 } $Offset;

typedef struct $Node                                                            // Offset to a node in the tree. As we know what is being addressed we have a more specific set of methods available than those available to a generic Offset.
 {$      tree;                                                                  // ArenaTree containing the item
  size_t offset;                                                                // Offset
  const struct ProtoTypes_$Node *proto;                                         // Methods associated with nodes
 } $Node;

typedef struct $Arena                                                           // Description of the block of memory that holds a tree.  The individual nodes of the tree cannot be cannot be freed individually, but of course the entire arena can be. As the arena uses offsets to store addresses, the arena can be resized by copying it into a new, larger arena.
 {size_t  size;                                                                 // The total size of the arena used to hold data associated with the tree.
  size_t  used;                                                                 // The number of bytes currently used.
  size_t  root;                                                                 // Offset to the root node
  $String data;                                                                 // The arena containing the data associated with the tree
 } $Arena;

typedef struct $Description                                                     // The description of an arena tree which is written as the header record for dump files.
 {const size_t version;                                                         // Version of arena tree
  const size_t littleEndian;                                                    // Little endian if true
  const size_t s64b;                                                            // 64 bit address if true, else 32 bit
  const size_t used;                                                            // The number of bytes currently used by the arena.
  const size_t root;                                                            // Offset to the root node
  const time_t created;                                                         // Time in seconds since the epoch when this tree was created
 } $Description;

#include <$$_prototypes.h>                                                      // Arena tree prototypes now that the relevant structures have been declared
#define $fe( child, parent) for($Node child = parent ▷ first; child ▷ valid; child = child ▷ next) // Each child in a parent from first to last
#define $fer(child, parent) for($Node child = parent ▷ last ; child ▷ valid; child = child ▷ prev) // Each child in a parent from last to first
#include <stringBuffer.c>

//D1 Constructors                                                               // Construct a new Arena tree.

static $ make$                                                                  // Create a new arena tree
 ()                                                                             // ArenaTree allocator
 {$Arena * const a = alloc(sizeof($Arena));                                     // Allocate arena description
  t ◁ new $(arena: a);                                                          // Arena tree we are creating

  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  a->used   = 0;                                                                // Length used so far
  a->root   = 0;                                                                // Root not set in $
  t ▷ node("");                                                                 // Initialize root node
  return t;
 }

static void swap_$_$                                                            // Swap the arenas of two trees.
 ($ source,                                                                     // Source tree
  $ target)                                                                     // Target tree
 {s ◁ source.arena; t ◁ target.arena;                                           // Arenas
  $Arena u = *t; *t = *s; *s = u;
 }

static char * check_$                                                           //P Return a string describing the structure
 (const $ tree)                                                                 // Tree
 {if (tree.arena->data) {} return "tree";
 }

//D1 Pointers and Offsets                                                       // Operations on pointers and offsets

static void * pointer_$_size                                                    //PV Return a temporary pointer to an offset in a tree.
 (const $      tree,                                                            // Tree
  const size_t delta)                                                           // Delta
 {if (delta > tree.arena->used)                                                 // An delta outside the arena is an unset delta
   {printStackBackTrace("Accessing area outside arena");
   }
  return (void *)(tree.arena->data + delta);                                    // Convert a non zero delta that is within the arena to a valid pointer
 }

static void * pointer_$Offset                                                   //PV Convert a node describing an offset into an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const $Offset o)                                                              // Offset
 {return pointer_$_size(o.tree, o.offset);                                      // Return a temporary pointer to an offset in a tree.
 }

static $Offset offsetTo_$_pointer                                               //P Create an offset from a pointer into the arena of a tree.
 (const $            tree,                                                      // Offset
  const void * const pointer)                                                   // Pointer into arena
 {a ◁ *tree.arena;
  p ◁ (char const *)pointer;
  if (p > a.data && p < a.data + a.used)
   {return new$Offset(tree: tree, offset: p - a.data);
   }
  printStackBackTraceAndExit(1, "Arena does not contain pointer %p\n", pointer);
 }

static $Content * content_$Node                                                 //PV Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const $Node n)                                                                // NodeContent $Offset
 {return ($Content *)pointer_$_size(n.tree, n.offset);
 }

static int checkKey_int_$Node_string_size                                       // Confirm that the key of a node is equal to the specified string
 (const $Node        node,                                                      // Node whose key is to be checked
  const char * const key,                                                       // Expected key value
  const size_t       length)                                                    // Length of expected key value or zero to request strlen
 {return !strncmp(node ▷ key, key, length ? : strlen(key));
 }

static int equals_int_$Node_$Node                                               // Confirm two offsets are equal
 (const $Node a,                                                                // First offset
  const $Node b)                                                                // Second offset
 {return a.tree.arena == b.tree.arena && a.offset == b.offset;
 }

static  $Node root_$NodeOffset_$                                                // Return the root node of a tree.
 (const $ tree)                                                                 // Tree
 {return new $Node(tree: tree);                                                 // Tree containing root node
 }

static  $Node root_$NodeOffset_$NodeOffset                                      // Return the root node of the tree containing the specified node.
 (const $Node node)                                                             // NodeContent
 {return node.tree ▷ root;
 }

static $String key_string_$Node                                                 //V Get a temporary pointer to the offset containing the key of a node.
 (const $Node node)                                                             // NodeContent
 {const $ t = node.tree;
  k ◁ t ▷ offset((node ▷ content)->key.delta);
  return  k ▷ pointer;
 }

//D1 Allocation                                                                 // Allocating memory in the tree

static void setKey_$Node_$String                                                // Copy a string into the key field of a node
 (const $Node        node,                                                      // Node
  const char * const key)                                                       // Key - it will be copied into the tree
 {k ◁ node ▷ key;                                                               // Existing key                                                                               //
  const size_t  l = strlen(k), m = strlen(key);
  if (k && m <= l)                                                              // There is enough room in the existing key for the new key
   {strcpy(k, key);
    return;
   }
  node ▷ content->key.delta = node.tree ▷ saveString(key, m).offset;            // Allocate new (longer) key
 };

static size_t used_$                                                            // Amount of space currently being used within the arena of a tree.
 (const $ tree)                                                                 // Tree
 {return tree.arena->used;
 }

static $Offset  offset_$_size                                                   //P Create an offset to locate an item within the tree.
 (const $       tree,                                                           // Tree
  const size_t  delta)                                                          // Delta within arena
 {const $Offset o = {tree, delta, &ProtoTypes_$Offset};                         // Create offset locally
  return        o;
 }

static $Node   nodeFromOffset_$_size                                            //P Create a node to locate an allocation within the arena of a tree.
 (const $      tree,                                                            // Tree
  const size_t delta)                                                           // Delta within arena. A delta of zero represents no such node.
 {return new $Node(tree: tree, offset: delta);
 }

static $Offset allocate_offset_$_size                                           //P Allocate memory within the arena of a tree and clear the allocated memory
 (const $      tree,                                                            // ArenaTree in which to allocate
  const size_t size)                                                            // Amount of memory required
 {if (tree.arena->used + size < tree.arena->size)                               // Allocate within existing arena
   {o ◁ tree ▷ offset(tree.arena->used);                                        // Current offset to open memory
    memset(o ▷ pointer, 0, size);                                               // Clear new memory
    tree.arena->used += size;                                                   // Allocate
    return o;                                                                   // Return allocation
   }
  else                                                                          // Reallocate arena
   {S ◁ nextPowerOfTwo(tree.arena->size + size);                                // Round up memory required
    if (S < ((size_t)((1l<<(sizeof($Delta)*8)) - 1)))                           // The maximum size of a delta if there are 8 bits in a byte
     {m ◁ realloc(tree.arena->data, S);                                         // Reallocate arena - the old one will be copied into it if it is moved.
      if (m)                                                                    // Retry the memory allocation
       {tree.arena->data = m;                                                   // New arena
        tree.arena->size = S;                                                   // Arena size
        u ◁ tree.arena->used;                                                   // Length of free space
        memset(tree.arena->data + u, 0, S - u);                                 // Clear free space
        return tree ▷ allocate(size);                                           // Allocate within arena
       }
     }
   }
  printStackBackTraceAndExit(2, "Arena too large\n");                           // The arena has become too large for the chosen size of offsets.
 }

static $Node noden_$Node_$_$String                                              // Create a new tree node keyed by a string of the specified length to which a terminating zero will be appended.
 (const $       tree,                                                           // Arena tree in which to create the node
  const char * const key,                                                       // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the tree is determined solely by the user.
  const size_t  length)                                                         // Length of the key, or if zero, I will use strlen
 {c ◁ tree ▷ allocate(sizeof($Content));                                        // Space required for a tree node
 (($Content *)(c ▷ pointer))->key.delta = tree ▷ saveString(key, length).offset;// Save key                                                      // Save key offset
  return tree ▷ nodeFromOffset(c.offset);                                       // Return node
 }

static $Node node_$Node_$_$String                                               // Create a new tree node keyed by a zero terminated string.
 (const $             tree,                                                     // Arena tree in which to create the node
  const char * const  key)                                                      // Key for this node.  Note: we do not order nodes automatically.
 {return tree ▷ noden(key, 0);
 }

static  $Node nodeFromStringBuffer_$_$Node_$_StringBuffer                       // Create a new tree node from a String Buffer
 (const $             tree,                                                     // Arena tree in which to create the node
  const StringBuffer  string)                                                   // Key for this node as a string buffer
 {N ◁ string ▷ length;                                                          // Length of buffer
  char b[N+1];                                                                  // Space for buffer and terminating zero
  string   ▷ string(b);                                                         // Create continuous string
  n ◁ tree ▷ noden (b, N);                                                      // Node from string of known length
  return n;
 }

static $Offset saveString_$Offset_$_$String                                     //P Save a copy of a zero terminated string in a tree and return the offset of the string.
 (const $       tree,                                                           // Arena tree in which to create the node
  const char * const str,                                                       // String
  const size_t  length)                                                         // String, or if zero I will call strlen
 {l ◁ length ? : strlen(str);                                                   // Length of string
  o ◁ tree ▷ allocate(l + 1);                                                   // Allocate space for the string plus a terminating zero
  T ◁ stpncpy(o ▷ pointer, str, l); *T = 0;
  return o;
 }

static void get_$_data_size                                                     // Get a copy of the data at the specified offset in the arena of a tree.
 (const $      tree,                                                            // Arena tree
  const size_t offset,                                                          // Offset to data in arena
  void * const data,                                                            // Pointer to the area into which the data is to be copied
  const size_t length)                                                          // Length of the data to be retrieved
 {memcpy(data, tree ▷ pointer(offset), length);                                 // Copy out data
 }

static $Offset store_offset_$_data_size                                         // Store a copy of the specified data in the arena of the tree and return the offset of the data.
 (const $       tree,                                                           // Tree in whose arena the data will be stored
  const void *  const data,                                                     // Pointer to the data to be stored
  const size_t  length)                                                         // Length of the data to be stored
 {       o ◁ tree ▷ allocate(length);                                           // Allocate space for data
  memcpy(o ▷ pointer, data, length);                                            // Copy in data
  return o;
 }

static void retrieve_$_$Offset_data_size                                        // Retrieve a copy of the data stored at the specified offset in the arena of the specified tree.
 (const $       tree,                                                           // Tree in whose arena the data is stored.
  const $Offset offset,                                                         // Offset in the arena at which the data to be retrieved is stored.
  void * const  data,                                                           // Area into which the retrieved data is to be copied.
  const size_t  length)                                                         // Length of the data to be retrieved
 {memcpy(data, tree ▷ pointer(offset.offset), length);                          // Copy out data
 }

static void get_$Node_data_size                                                 // Get a copy of the data addressed by a node.
 (const $Node  node,                                                            // Node in an arena tree associated with the data
  void * const data,                                                            // Pointer to the area into which the data is to be copied
  const size_t length)                                                          // Length of the data to be retrieved
 {node.tree ▷ get(node ▷ content->data.delta, data, length);                         // Locate data
 }

static void set_$Node_data_size                                                 // Save a copy of the specified data in the arena of the tree and return the offset of the data.
 (const $Node  node,                                                            // Node in an arena tree to associate with the data
  const void * const data,                                                      // Pointer to the data to be saved
  const size_t length)                                                          // Length of the data to be saved
 {node ▷ content->data.delta = node.tree ▷ store(data, length).offset;          // Record offset of saved data
 }

static void copyData_$Node_$Node                                                // Copy the data associated with the sourtce node to the target node by copying the offset to the data held in the source node to the target node.
 (const $Node target,                                                           // Target node
  const $Node source)                                                           // Source node
 {target ▷ content->data = source ▷ content->data;                              // Copy data offset
 }

static size_t getData_size_$Node                                                // Get the value of the data offset associated with a node.
 (const $Node  node)                                                            // Node in an arena tree associated with the data
 {return node ▷ content->data.delta;                                            // Value of the data offset field
 }

static void setData_$Node_size                                                  // Set the value of the data offset associated with a node.
 (const $Node  node,                                                            // Node in an arena tree to associate with the data
  const size_t offset)                                                          // Value the data offset is to be set to
 {node ▷ content->data.delta = offset;                                          // Record offset
 }

static void fromLetters_$_$String                                               // Load tree from a string of letters and brackets.  The root node of the tree so constructed is always given the letter 'a'.
 (const $ tree,                                                                 // Tree
  $String str)                                                                  // String of letters and brackets describing desired tree structure
 {const $Node n = tree ▷ root;                                                  // The node we are currently  adding to
  n ▷ setKey("a");                                                              // The root is always called "a"

  void parse($Node current)                                                     // Parse the string of letters and brackets to construct the desired tree
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
         {char s[2] = {c, 0};
          current ▷ putLast(tree ▷ node(s));
          ++str; continue;
         }
       }
     }
   }

  parse(n);                                                                     // Parse the string of letters and brackets to construct the desired tree
 };

static void free_$                                                              // Free an entire arena tree.
 (const $ tree)                                                                 // Arena tree to free
 {a ◁ tree.arena;
  if  (a->data) free(a->data);
  free(a);
 }

//D1 Navigation                                                                 // Navigate through a tree.

static int valid_$Node                                                          // Check that a node is valid.
 (const $Node child)                                                            // Node
 {return child.offset;                                                          // A node is valid unless it has a zero offset in which case it is not a valid node. Typically an invalid node is returned when a method applied to a valid node cannot be completed as in: get the next sibling following the last sibling.
 }

static  $Node parent_$Node_$Node                                                // Get the parent of a child
 (const $Node child)                                                            // Child
 {if (child ▷ isRoot) return child;
  return child.tree ▷ nodeFromOffset(child ▷ content->parent.delta);
 }

static  $Node first_$Node_$Node                                                 // Get the first child under a parent.
 (const $Node parent)                                                           // Parent
 {return  parent.tree ▷ nodeFromOffset(parent ▷ content->first.delta);
 }
duplicate s/first/last/g s/first/next/g s/first/prev/g

static  $Node first_$Node_$                                                     // Get the first child in the specified $.
 (const $ tree)                                                                 // Parent
 {const $Node root = tree ▷ root;
  return root ▷ first;
 }
duplicate s/first/last/g s/first/next/g s/first/prev/g

//D1 Search                                                                     // Search for nodes.

static int equalsString_$Node_string                                            // Check that the key of a node
 (const $Node        node,                                                      // Node
  const char * const key)                                                       // Key
 {return !strcmp(node ▷ key, key);
 }

static  $Node findFirst_$Node_string                                            // Find the first node with the specified key in a post-order traversal of the tree starting at the specified node.
 (const $Node        node,                                                      // Node at the start of the tree to be searched
  const char * const key)                                                       // Key to find
 {jmp_buf found;
  $Node F = new $Node(tree: node.tree);                                         // An invalid node

  void find($Node node)                                                         // Check whether the key of the current node matches the specified key
   {if (node ▷ equalsString(key))                                               // Found
     {F.offset = node.offset;
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) node ▷ by(find);                                          // Search the tree

  return F;                                                                     // Return an invalid node
 }

static  $Node findFirst_$_string                                                // Find the first node with the specified key in a post-order traversal of the tree.
 (const $            tree,                                                      // Tree
  const char * const key)                                                       // Key to find
 {jmp_buf found;
  $Node F = new $Node(tree: tree);                                                                      // An invalid node

  void find($Node node)                                                         // Check whether the key of the current node matches the specified key
   {if (node ▷ equalsString(key))                                               // Found
     {F.offset = node.offset;
      longjmp(found, 1);
     }
   }

  if (!setjmp(found)) tree ▷ by(find);                                          // Search the tree

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
 (const $            tree,                                                      // Tree
  const char * const key)                                                       // Key to find
 {const $Node  r = tree ▷ root;                                                 // Root node of the tree
  return r ▷ findFirstChild(key);                                               // Search the tree
 }

//D1 Location                                                                   // Verify the current location.

static int context_$Node                                                        // Return true if the parent of the specified child has the specified key.
 (const $Node         child,                                                    // Child
        $Node * const parent,                                                   // Parent container
  const char  * const key)                                                      // Key
 {if (child ▷ isRoot) return 0;                                                 // The root node has no context
  p ◁ *parent = child ▷ parent;
  return p ▷ valid && !strcmp(p ▷ key, key);                                    // Check key
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

//D1 Put                                                                        // Insert children into a tree.

static  $Node putFL_$Node_$Node_$Node                                           //P Put a child first or last under its parent
 (const int   first,                                                            // Put first if true, else last
  const $Node parent,                                                           // Parent
  const $Node child)                                                            // Child
 {tree ◁ parent.tree;
  if (child.tree.arena != tree.arena) printStackBackTrace("Wrong tree\n");      // Parent and child must be in same tree
  P ◁ parent.offset;    C ◁ child.offset;                                       // Delta of parent and child
  p ◁ parent ▷ content; c ◁ child  ▷ content;                                   // Pointers to parent and child
  L ◁ first ? p->first.delta : p->last.delta;                                   // Delta of child currently first/last
  if (!L)                                                                       // No children yet
   {if (first) p->last.delta = C; else p->first.delta = C;                      // Delta to last/first child
   }
  else                                                                          // Existing children
   {$Content * const l = tree ▷ pointer(L);                                     // Link first/last two children together
    if (first) l->prev.delta = C; else l->next.delta = C;
    if (first) c->next.delta = L; else c->prev.delta = L;
   }
  c->parent.delta = P;                                                          // Parent new first/last child
  if (first)  p->first.delta = C; else p->last.delta = C;                       // Parent points to new first/last child
  return child;
 }

static  $Node putTreeFirst_$Node_$Node                                          // Put a child first in the tree containing the arena in which the child was allocated.
 (const $Node child)                                                            // Child
 {t ◁ child.tree;                                                               // Tree containing arena containing child
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
 {tree   ◁ sibling.tree;                                                        // Tree
  parent ◁ sibling ▷ parent;                                                    // Parent node
  twin   ◁ next ? sibling ▷ next : sibling ▷ prev;                              // Next or prev sibling to sibling is such a node exists
  assert(child.tree.arena == tree.arena);                                       // Parent and child must be in same tree
  s ◁ sibling ▷ content;                                                        // Pointer to sibling
  c ◁ child   ▷ content;                                                        // Pointer to child

  if (!twin ▷ valid)                                                            // Sibling is last/first under parent
   {if (next) parent ▷ putLast(child); else parent ▷ putFirst(child);           // Place child
   }
  else                                                                          // Not last/first
   {w ◁ twin ▷ content;                                                         // Pointer to twin of sibling
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

static void setUp_$Node_$Node                                                   //P Make the specified parent node the parent of the specified child node
 (const $Node child,                                                            // Child
  const $Node parent)                                                           // Child
 {child ▷ content->parent.delta = parent.offset;                                // Set parent of child
 }

static  $Node putNext_$Node_$Node_$Node                                         // Put a child next after the specified sibling
 (const $Node sibling,                                                          // Sibling
  const $Node child)                                                            // Child
 {return putNP_$Node_$Node_$Node(1, sibling, child);                            // Put child next
 }
duplicate s/Next/Prev/g,s/next/previous/g,s/1/0/

//D1 Traverse                                                                   // Traverse a tree.

static void by_$Node_sub                                                        // Traverse the tree rooted at the specified node in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 ($Node node,                                                                   // Node
  void (* const function) (const $Node node))                                   // Function
 {void children(const $Node parent)                                             // Process the children of the specified parent
   {if (!parent ▷ valid) return;                                                // Empty child
    N ◁ parent ▷ countChildren;                                                 // Number of children
    size_t c[N+1];                                                              // Array of children terminated by a trailing zero
    size_t n = 0; $fe(child, parent) c[n++] = child.offset;                     // Load each child into an array
    for(size_t i = 0; i < N; ++i) children(new $Node(tree: parent.tree, offset: c[i]));            // Process each child allowing it to change position
    function(parent);                                                           // Process the parent
   }
  children(node);                                                               // Start at the specified root node
 }

static void by_$_sub                                                            // Traverse a tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 (const $ tree,                                                                 // Tree
  void (* const function) (const $Node node))                                   // Function
 {root ◁ tree ▷ root;                                                           // Root node
  N    ◁ tree ▷ countChildren;                                                  // Number of children under root
  size_t c[N+1];                                                                // Array of children terminated by a trailing zero
  size_t n = 0; $fe(child, root) c[n++] = child.offset;                         // Load each child into an array
  for(size_t i = 0; i < N; ++i)
   {$Node n = new $Node(tree: tree, offset: c[i]);                              // Process each child allowing it to change position
    n ▷ by(function);
   }
  function(root);
 }

static  size_t countChildren_size_$                                             // Count the number of children directly under a parent.
 (const $ tree)                                                                 // $
 {size_t l = 0;
  root ◁ tree ▷ root;
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


static  size_t count_size_$                                                     // Count the number of nodes in a tree
 (const $ tree)                                                                 // Tree
 {size_t l = 0;

  root ◁ tree ▷ root;                                                           // Root is always invalid
  $fe(child, root) l += 1 + child ▷ count;                                      // Child under root plus its children
  return l;                                                                     // Return count without counting the root node
 }

//D1 Print                                                                      // Print Arena Trees in various ways

static  $String printWithBrackets_string_$Node                                  // Print a node and the tree below it in preorder as a string with each sub tree enclosed in brackets.
 (const $Node   node)                                                           // Node
 {size_t l = 0;                                                                 // Length of output string
  void len(const $Node parent)                                                  // Find the size of buffer we will need
   {l += strlen(parent ▷ key);                                                  // Parent key
    if (!parent ▷ isEmpty)                                                      // Parent has children
     {l += 2;                                                                   // Space for brackets
      $fe(child, parent) len(child);                                            // Each child
     }
   }

  $String p;
  void print(const $Node parent)                                                // Print the children of the specified parent
   {const $String k = parent ▷ key;
    p = stpcpy(p, k);
    if (!parent ▷ isEmpty)                                                      // Parent has children
     {*p++ = '(';                                                               // Start children
      $fe(child, parent) print(child);                                          // Each child
      *p++ = ')';                                                               // Start children
     }
    *p = 0;                                                                     // End the string
   }

  len(node);                                                                    // Length of the print
  s ◁ p = alloc(l+1); *s = 0;
  print(node);
  return s;
 }

static  $String printWithBrackets_$String_$                                     // Print an entire tree in preorder as a string with brackets around each sub tree
 (const $ t)                                                                    // Tree
 {node ◁  t ▷ root;                                                             // Root
  return node ▷ printWithBrackets;
 }

static int printsWithBracketsAs_int_$Node_string                                // Check that the $ starting at the specified node prints with brackets as expected.
 (const $Node        node,                                                      // Node
  const char * const expected)                                                  // Expected string when printed
 {s ◁ node ▷ printWithBrackets;
  r ◁ !strcmp(s, expected);
  free(s);
  return r;
 }

static int printsWithBracketsAs_int_$_string                                    // Check that the specified $ prints with brackets as expected.
 (const $            tree,                                                      // $
  const char * const expected)                                                  // Expected string when printed
 {       root ◁ tree ▷ root;                                                    // Root
  return root ▷ printsWithBracketsAs(expected);
 }

static StringBuffer print_string_$Node                                          // Print a node and the $ below it in preorder as a string.
 (const $Node node)                                                             // Node to print from
 {s ◁ makeStringBuffer();                                                         // String buffer being constructed

  void print(const $Node parent)                                                // Print the children of the specified parent
   {s ▷ add(parent ▷ key);
    $fe(child, parent) print(child);                                            // Each child
   }

  print(node);
  return s;
 }

static StringBuffer print_string_$                                              // Print an entire tree in preorder as a string.
 (const $ tree)                                                                 // $
 {        node ◁ tree ▷ root;                                                   // Root
  return  node ▷ print;
 }

static int printsAs_int_$Node_string                                            // Check that the $ starting at the specified node prints as expected.
 (const $Node        node,                                                      // Node
  const char * const expected)                                                  // Expected string when printed
 {s ◁ node ▷ print;
  r ◁ s ▷ equalsString(expected);
  s ▷ free;
  return r;
 }

static int printsAs_int_$_string                                                // Check that the specified $ prints as expected.
 (const $            tree,                                                      // $
  const char * const expected)                                                  // Expected string when printed
 {       root ◁ tree ▷ root;                                                    // Root
  return root ▷ printsAs(expected);
 }

static int printContains_$Node                                                  // Check the print of an $ starting at the specified tag contains the expected string.
 (const $Node   node,                                                           // Starting node
  const char *  expected)                                                       // Expected string
 {s ◁ node ▷ print;
  r ◁ s ▷ containsString(expected);
  s ▷ free;
  return r;
 }

static int printContains_$                                                      // Check the print of an $ contains the expected string.
 (const $       tree,                                                            // $ parse tree
  const char *  expected)                                                       // Expected string
 {s ◁ tree ▷ print;
  r ◁ s ▷ containsString(expected);
  s ▷ free;
  return r;
 }

//D1 Edit                                                                       // Edit a tree in situ.

static  $Node cut_$Node_$Node                                                   // Cut out a child.
 (const $Node child)                                                            // Child to cut out
 {const $Node parent = child  ▷ parent;                                         // Parent
  p ◁ parent ▷ content; c ◁ child ▷ content;                                    // Parent pointer, child content

  if (child ▷ isOnlyChild)                                                      // Only child
   {p->first.delta = p->last.delta = 0;                                         // Remove child
   }
  else if (child ▷ isLast)                                                      // Last child
   {L ◁ child  ▷ prev;
    l ◁ L ▷ content;
    p->last.delta = L.offset; l->next.delta = 0;                                // Last child under parent
   }
  else if (child ▷ isFirst)                                                     // First child
   {F ◁ child ▷ next;
    f ◁ F     ▷ content;
    p->first.delta = F.offset; f->prev.delta = 0;                               // Last child under parent
   }
  else                                                                          // Some where in the middle
   {N ◁ child ▷ next;    P ◁ child ▷ prev;
    n ◁     N ▷ content; p ◁     P ▷ content;
    p->next.delta = N.offset; n->prev.delta = P.offset;
   }

  c->next.delta = c->prev.delta = 0;                                            // Remove child

  return child;
 }

static  $Node unwrap_$Node_$Node                                                // Unwrap the specified parent and return it.
 (const $Node parent)                                                           // Parent to unwrap
 {for($Node child = new $Node(tree: parent.tree, offset: parent ▷ last.offset); // Remove last children
    child ▷ valid;
    child.offset = parent ▷ last.offset)
   {parent ▷ putNext(child ▷ cut);                                              // Place each child after the parent
   }
  return parent ▷ cut;                                                          // Remove and return empty parent
 }

static  $Node wrap_$Node_$String                                                // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
 (const $Node   child,                                                          // Child to wrap
  const char * const key)                                                       // Key for new parent
 {tree   ◁ child.tree;                                                          // Tree
  parent ◁ tree ▷ node(key);                                                    // New parent
  child  ▷ putNext(parent);                                                     // Place parent after child
  parent ▷ putLast(child ▷ cut);                                                // Place child under parent
  return parent;
 }

//D1 Input and Output                                                           // Read and write a tree from/to a file

static void write_void_$_$String                                                // Write a tree to a named file or abort
 (const $       tree,                                                           // Tree
  const char * const file)                                                      // File
 {    o ◁ open(file, O_CREAT| O_WRONLY, S_IRWXU);                               // Open for output creating if needed
  if (o < 0) printStackBackTrace("Cannot open file: %s for write\n", file);

  time_t current; time(&current);
  const $Description h = {1, 1, 0, tree ▷ used, tree ▷ root.offset, current};   // Create arena tree header describing the tree

  if (sizeof(h) != write(o, &h, sizeof(h)))                                     // Write header
   {printStackBackTrace("Cannot write $ header to file: %s\n", file);
   }

  w ◁ write(o, tree.arena->data, tree.arena->used);                             // Write arena
  if (w < 0 || tree.arena->used != (size_t)w)
   {printStackBackTrace("Cannot write $ arena to file: %s\n", file);
   }

  close(o);
 }

$ read$                                                                         // Read a tree from a file
 (const char * const file)                                                      // File
 {$Arena * const arena = alloc(sizeof($Arena));                                 // Create arena
  tree ◁ new $(arena: arena);                                                   // Initialize tree

      i ◁ open(file, 0, O_RDONLY);                                              // Open for input
  if (i < 0) printStackBackTrace("Cannot open file: %s for read\n", file);

  struct $Description h;
  if (sizeof(h) != read(i, &h, sizeof(h)))                                      // Read header
   {printStackBackTrace("Cannot read header from file: %s\n", file);
   }

  tree.arena->data = alloc(arena->size = arena->used = h.used);                 // Allocate arena

      r ◁ read(i, arena->data, arena->used);                                    // Read arena
  if (r < 0 || arena->used != (size_t)r)
   {printStackBackTrace("Cannot read $ from file: %s\n", file);
   }

  close(i);
  arena->root = h.root;                                                         // Offset to root in arena
  return tree;
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //Tmake$ //Tnode //Tfree //TputFirst //TputLast //Tfe //Tfer
 {   t ◁ make$();                                                               // Create a tree
  root ◁ t ▷ root;

  for(int i = 0; i < 10; ++i)                                                   // Load tree
   {char s[2] = {'a'+i, 0};

    c1 ◁ t ▷ node(s); root ▷ putLast (c1);
    c2 ◁ t ▷ node(s); root ▷ putFirst(c2);
    s[0] = 'A'+i; c2 ▷ setKey(s);
    assert(strcmp(c2 ▷ key, s) == 0);
   }

  if (1)                                                                        // For each
   {char l[t ▷ count + 1]; *l = 0;
    $fe (child, root) strcat(l, child ▷ key);
    assert(strcmp(l, "JIHGFEDCBAabcdefghij") == 0);
   }

  if (1)                                                                        // For each in reverse
   {char l[t ▷ count + 1]; *l = 0;
    $fer(child, root) strcat(l, child ▷ key);
    assert(strcmp(l, "jihgfedcbaABCDEFGHIJ") == 0);
   }

  t ▷ free;
 }

void test1()                                                                    //Troot //Tfirst //Tlast //Tnext //Tprev //Tparent //Tequals //Tprint //TprintWithBrackets //TfromLetters //TprintsAs
 {t ◁ make$();    t ▷ fromLetters("b(c(de)f)g(hi)j");
  assert(t ▷ printsWithBracketsAs("a(b(c(de)f)g(hi)j)"));
  assert(t ▷ printsAs("abcdefghij"));

  a ◁ t ▷ root;
  b ◁ a ▷ first;
  c ◁ b ▷ first;
  e ◁ c ▷ last;
  d ◁ e ▷ prev;

  $String     k = d ▷ key;
  assert(k[0] == 'd');
  b ▷ printsWithBracketsAs("b(c(de)f)");
  c ▷ printsWithBracketsAs("c(de)");

  assert(c ▷ equals(d ▷ parent));
  assert(b ▷ equals(c ▷ parent));
  assert(a ▷ equals(b ▷ parent));
  assert(a ▷ equals(t ▷ root));

  assert(c ▷ printsAs("cde"));
  assert(t ▷ printsAs("abcdefghij"));

  t ▷ free;
 }

void test2()                                                                    //Tby //TprintsWithBracketsAs //TprintContains
 {t ◁ make$();      t ▷ fromLetters("b(c(de)f)g(hi)j");
  assert(t ▷ printsWithBracketsAs("a(b(c(de)f)g(hi)j)"));
  assert(t ▷ printContains("def"));

  char l[t ▷ count + 2], *p = l;

  void process($Node n) {p = stpcpy(p, n ▷ key); *p = 0;}
  t ▷ by(process);
  assert(strcmp(l, "decfbhigja") == 0);

  t ▷ free;
 }

void test3()                                                                    //TisFirst //TisLast //TisEmpty //TisRoot //TisOnlyChild //Tcontext //TcheckKey //Tvalid
 {t ◁ make$();      t ▷ fromLetters("b(c(de(f)gh)i)j");
  assert(t ▷ printsWithBracketsAs("a(b(c(de(f)gh)i)j)"));

  a ◁ t ▷ root;  assert(a ▷ checkKey("a", 0));                                 assert(a ▷ printsWithBracketsAs( "a(b(c(de(f)gh)i)j)"));
  b ◁ a ▷ first; assert(b ▷ checkKey("b", 0)); assert(a ▷ equals(b ▷ parent)); assert(b ▷ printsWithBracketsAs(   "b(c(de(f)gh)i)"));
  c ◁ b ▷ first; assert(c ▷ checkKey("c", 0)); assert(b ▷ equals(c ▷ parent)); assert(c ▷ printsWithBracketsAs(     "c(de(f)gh)"));
  d ◁ c ▷ first; assert(d ▷ checkKey("d", 0)); assert(c ▷ equals(d ▷ parent)); assert(d ▷ printsWithBracketsAs(       "d"));
  e ◁ d ▷ next;  assert(e ▷ checkKey("e", 0)); assert(c ▷ equals(e ▷ parent)); assert(e ▷ printsWithBracketsAs(        "e(f)"));
  f ◁ e ▷ last;  assert(f ▷ checkKey("f", 0)); assert(e ▷ equals(f ▷ parent)); assert(f ▷ printsWithBracketsAs(          "f"));
  g ◁ e ▷ next;  assert(g ▷ checkKey("g", 0)); assert(c ▷ equals(g ▷ parent)); assert(g ▷ printsWithBracketsAs(            "g"));
  h ◁ g ▷ next;  assert(h ▷ checkKey("h", 0)); assert(c ▷ equals(h ▷ parent)); assert(h ▷ printsWithBracketsAs(             "h"));
  i ◁ c ▷ next;  assert(i ▷ checkKey("i", 0)); assert(b ▷ equals(i ▷ parent)); assert(i ▷ printsWithBracketsAs(               "i"));
  j ◁ b ▷ next;  assert(j ▷ checkKey("j", 0)); assert(a ▷ equals(j ▷ parent)); assert(j ▷ printsWithBracketsAs(                 "j"));

  assert(!a ▷ valid);
  assert( b ▷ isFirst);
  assert( j ▷ isLast);
  assert( f ▷ isFirst);
  assert( f ▷ isLast);

  assert( f ▷ isEmpty);
  assert( f ▷ isOnlyChild);
  assert(!e ▷ isOnlyChild);
  assert( a ▷ isRoot);

  $Node A, C, E; assert(f ▷ context(&E, "e") && E ▷ context(&C, "c"));
  assert(!a ▷ context(&A, "a"));

  t ▷ free;
 }

void test4()                                                                    //Tcut //TfindFirst //TcountChildren //TequalsString //TfindFirstChild
 {t ◁ make$();      t ▷ fromLetters("b(c(de(f)gh)i)j");
  assert(t ▷ printsWithBracketsAs("a(b(c(de(f)gh)i)j)"));

  c ◁ t ▷ findFirst("c");

  assert(c ▷ equalsString("c"));
  d ◁ t ▷ findFirst("d"); assert(d ▷ equalsString("d"));
  e ◁ t ▷ findFirst("e"); assert(e ▷ equalsString("e"));
  f ◁ t ▷ findFirst("f"); assert(f ▷ equalsString("f"));
  g ◁ t ▷ findFirst("g"); assert(g ▷ equalsString("g"));
  h ◁ t ▷ findFirst("h"); assert(h ▷ equalsString("h"));

  assert(g ▷ equals(c ▷ findFirstChild("g")));
  assert(c ▷ countChildren == 4);
  assert(e ▷ countChildren == 1);

  f ▷ cut;           assert(t ▷ printsWithBracketsAs("a(b(c(degh)i)j)"));
  e ▷ putFirst(f);   assert(t ▷ printsWithBracketsAs("a(b(c(de(f)gh)i)j)"));

  e ▷ cut;           assert(t ▷ printsWithBracketsAs("a(b(c(dgh)i)j)"));
  d ▷ putNext(e);    assert(t ▷ printsWithBracketsAs("a(b(c(de(f)gh)i)j)"));

  d ▷ cut;           assert(t ▷ printsWithBracketsAs("a(b(c(e(f)gh)i)j)"));
  e ▷ putPrev(d);    assert(t ▷ printsWithBracketsAs("a(b(c(de(f)gh)i)j)"));

  h ▷ cut;           assert(t ▷ printsWithBracketsAs("a(b(c(de(f)g)i)j)"));
  g ▷ putNext(h);    assert(t ▷ printsWithBracketsAs("a(b(c(de(f)gh)i)j)"));

  t ▷ free;
 }

void test5()                                                                    //TreadArenaTree //Twrite
 {t ◁ make$();      t ▷ fromLetters("b(c(de(f)gh)i)j");
  assert(t ▷ printsWithBracketsAs("a(b(c(de(f)gh)i)j)"));

         f ◁ "/tmp/arenaTreeTest.data";
  assert(t ▷ printsWithBracketsAs("a(b(c(de(f)gh)i)j)"));
         t ▷ write(f);

         u ◁ read$(f);
  assert(u ▷ printsWithBracketsAs("a(b(c(de(f)gh)i)j)"));

  t ▷ free;
  u ▷ free;
  unlink(f);
 }

void test6()                                                                    //Tunwrap //Twrap //Tcount
 {t ◁ make$();    t ▷ fromLetters("bce");
  assert(t ▷ printsWithBracketsAs("a(bce)"));
  assert(t ▷ count == 3);

         c ◁ t ▷ findFirst("c");
  assert(c ▷ valid);

     d ◁ c ▷ wrap("d");
  assert(t ▷ printsWithBracketsAs("a(bd(c)e)"));
  assert(t ▷ count == 4);

         d ▷ unwrap;
  assert(t ▷ printsWithBracketsAs("a(bce)"));
  assert(t ▷ count == 3);
         t ▷ free;
 }

void test7()                                                                    //TsetKey //Tkey //TsaveString //Tused //Tsize
 {t ◁ make$(); t ▷ fromLetters("");
  $Delta d = {delta : 1};
  assert(sizeof(d) == 4);

  a ◁ t ▷ saveString("aaaa", 0);
  assert(strcmp(a ▷ pointer, "aaaa") == 0);

                 n ◁ t ▷ node("a");
                 n ▷ setKey  ("A");
  assert(strcmp (n ▷ key,     "A") == 0);

  t ▷ free;
 }

void test8()                                                                    //TputNext //TputPrev //Tnoden
 {t ◁ make$(); t ▷ fromLetters("A");
  a ◁ t ▷ root;  assert((a ▷ key)[0] == 'a');
  A ◁ a ▷ first; assert((A ▷ key)[0] == 'A');

  for(int i = 0; i < 10; ++i)
   {char c[2] = {'0'+i};
    A ▷ putNext(t ▷ noden(c, 1));
    A ▷ putPrev(t ▷ noden(c, 1));
   }

  assert(t ▷ printsWithBracketsAs("a(0123456789A9876543210)"));

  t ▷ free;
 }

void test9()                                                                    //Tget //Tset //TcopyData
 {t ◁ make$(); t ▷ fromLetters("a");
  struct S
   {int a;
   } S = {1}, T;

  b ◁ t ▷ node("b");
  b ▷ set(&S, sizeof(S));
  T.a = 0;
  b ▷ get(&T, sizeof(T));
  assert (T.a == 1);

  c ◁ t ▷ node("c");
  c ▷ copyData(b);
  T.a = 0;
  c ▷ get(&T, sizeof(T));
  assert (T.a == 1);

  t ▷ free;
 }

void test10()                                                                   //TgetData //TsetData
 {t ◁ make$(); t ▷ fromLetters("a");

          b ◁ t ▷ node("b");
          b ▷ setData   (42);
  assert (b ▷ getData == 42);

  t ▷ free;
 }

void test11()                                                                   //Tstore //Tretrieve //Tnew$Offset
 {t ◁ make$() ;

  a ◁ t ▷ store("abcd", 4);
  char c[4];
  for(size_t i = 0; i < sizeof(c); ++i)
   {t ▷ retrieve(a, c, i);
    assert(!strncmp(c, "abcd", i));
   }

  if (1)                                                                        // Create an offset explicitly
   {void * p = a ▷ pointer;
           A ◁ t ▷ offsetTo(p);
    assert(a.offset == A.offset && a.tree.arena->data == A.tree.arena->data);
   }

  t ▷ free;
 }

void test12()                                                                   //TputTreeFirst //TputTreeLast
 {t ◁ make$(); r ◁ t ▷ root; r ▷ setKey("a");

  c ◁ t ▷ node("c"); c ▷ putTreeFirst;
  d ◁ t ▷ node("d"); d ▷ putTreeLast;
  b ◁ t ▷ node("b"); b ▷ putTreeFirst;

  assert(t ▷ printsWithBracketsAs("a(bcd)"));

  t ▷ free;
 }

void test13()                                                                   //TnodeFromStringBuffer
 {t ◁ make$();
  s ◁ makeStringBuffer();
  s ▷ add("b");
  n1 ◁ t ▷ nodeFromStringBuffer(s); n1 ▷ putTreeLast;
  n2 ◁ t ▷ nodeFromStringBuffer(s); n2 ▷ putTreeLast;

  assert(t ▷ printsWithBracketsAs("(bb)"));

  s ▷ free; t ▷ free;
 }

void test14()                                                                   //Tswap
 {s ◁ make$(); s ▷ fromLetters("s");
  t ◁ make$(); t ▷ fromLetters("t");
  assert(s ▷ printsWithBracketsAs("a(s)"));
  assert(t ▷ printsWithBracketsAs("a(t)"));
  s ▷ swap(t);
  assert(t ▷ printsWithBracketsAs("a(s)"));
  assert(s ▷ printsWithBracketsAs("a(t)"));
  s ▷ free;
  t ▷ free;
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0,  test1,  test2,  test3,  test4,
                           test5,  test6,  test7,  test8,  test9,
                           test10, test11, test12, test13, test14, 0};
  run_tests("$", repetitions, tests);

  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaTree/arenaTree
