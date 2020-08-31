//------------------------------------------------------------------------------
// Trees held in an arena.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef $_included
#define $_included
#include <assert.h>
#include <fcntl.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <utilities.c>

#define $StartString "phi"                                                      // String that starts the arena

//D1 Structures                                                                 // Structures describing the tree. ArenaTree and TreeAllocator are not held inside the arena and so are pointers not offsets.

typedef char * $String;                                                         // Arena Tree string

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

//D1 Constructor                                                                // Construct a new Arena tree.

$ new$                                                                          // Create a new arena tree
 (const struct $ allocator)                                                     // ArenaTree allocator
 {$ t;                                                                          // Arena tree we are creating
  $Arena *a = t.arena = alloc(sizeof($Arena));                                  // Allocate Arena description
  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  strcpy(a->data,    $StartString);                                             // Occupy offset 0 in the rena with some junk so that we can use a delta of 0 as a null offset
  a->used   = strlen($StartString) + 1;                                         // Length used so far
  a->root   = 0;                                                                // Root not set
  t.proto   = allocator.proto;                                                  // Initialize tree prototype
  t ▷ new("");                                                                  // Initialize root node
  return t;
 }

//D1 Pointers and Offsets                                                       // Operations on pointers and offsets

static void * pointer_tree_offset                                               //PV Return a temporary pointer to an offset in a tree.
 (const $      tree,                                                            // Tree
  const size_t delta)                                                           // Delta
 {if (!delta) return NULL;                                                      // A zero delta is an unset delta
  if ( delta > tree.arena->used) return NULL;                                   // An delta outside the arena is an unset delta
  return (void *)(tree.arena->data + delta);                                    // Convert a non zero delta that is within the arena to a valid pointer
 }

static void * pointer_offset                                                    //PV Convert a node describing an offset into an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const $Offset o)                                                              // Offset
 {return pointer_tree_offset(o.tree, o.offset);                                 // Return a temporary pointer to an offset in a tree.
 }

static $Content * content_$Node                                                 //PV Convert a node offset to an address so that the content of a node can be updated in situ as long as the arena tree is not reallocated to a different position.
 (const $Node n)                                                                // NodeContent $Offset
 {return ($Content *)pointer_tree_offset(n.tree, n.offset);
 }

static int equals_int_$Node_$Node                                               // Confirm two offsets are equal
 (const $Node a,                                                                // First offset
  const $Node b)                                                                // Second offset
 {return a.tree.arena == b.tree.arena && a.offset == b.offset;
 }

static $Node root_$NodeOffset_$                                                 // Return the root node of a tree.
 (const $ tree)                                                                 // Tree
 {const $Node n = {tree  : tree,                                                // Tree containing root node
                   offset: strlen($StartString) + 1,                            // Offset of root node
                   proto : &ProtoTypes_$Node};                                  // Prototype
  return n;
 }

static $Node root_$NodeOffset_$NodeOffset                                       // Return the root node of the tree containing the specified node.
 (const $Node node)                                                             // NodeContent
 {const $ t = node.tree;
  return  t ▷ root;
 }

static $String key_string_$Node                                                 //V Get a temporary pointer to the offset containing the key of a node.
 (const $Node node)                                                             // NodeContent
 {const $ t = node.tree;
  $Offset k = t ▷ offset((node ▷ content)->key.delta);
  return  k ▷ pointer;
 }

//D1 Allocation                                                                 // Allocating memory in the tree

static $Node setKey_$Node_$Node_string                                          // Copy a string into the key field of a node
 (const $Node   node,                                                           // NodeContent
  const $String key)                                                            // Key - it will be copied into the tree
 {const $   t  = node.tree;
 (($Content *)(node ▷ content))->key.delta = t ▷ saveString(key, 0).offset;
  return node;
 };

static $Offset  offset_tree_delta                                               //P Create an offset to locate an item within the tree.
 (const $       tree,                                                           // Tree
  const size_t  delta)                                                          // Delta within arena
 {const $Offset o = {tree, delta, &ProtoTypes_$Offset};                         // Create offset locally
  return        o;
 }

static $Node   node_tree_delta                                                  //P Create a node to locate an allocation within the arena of a tree.
 (const $      tree,                                                            // Tree
  const size_t delta)                                                           // Delta within arena. A delta of zero represents no such node.
 {const $Node  n = {tree, delta, &ProtoTypes_$Node};                            // Create node
  return       n;
 }

static $Offset allocate_offset_tree_delta                                       //P Allocate memory within the arena of a tree and clear the allocated memory
 (const $      tree,                                                            // ArenaTree in which to allocate
  const size_t size)                                                            // Amount of memory required
 {if (tree.arena->used + size < tree.arena->size)                               // Allocate within existing arena
   {const size_t delta = tree.arena->used;                                      // Current delta to open memory
    const $Offset O = tree ▷ offset(delta);                                     // Current offset to open memory
    const $String o = O ▷ pointer;                                              // Address of memory to be allocated
    memset(o, 0, size);                                                         // Clear new memory
    tree.arena->used += size;                                                   // Allocate
    return O;                                                                   // Return allocation
   }
  else                                                                          // Reallocate arena
   {const size_t S = nextPowerOfTwo(tree.arena->size + size);                   // Round up memory required
    if (S < ((size_t)((1l<<(sizeof($Delta)*8)) - 1)))                           // The maximum size of a delta if there are 8 bits in a byte
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

static $Node newn_$Node_tree_string                                             // Create a new tree node keyed by a string of the specified length to which a terminating zero will be appended.
 (const $       tree,                                                           // Arena tree in which to create the node
  const $String key,                                                            // Key for this node.  Note: we do not order nodes automatically by key - the actually ordering of nodes in the tree is determined solely by the user.
  const size_t  length)                                                         // Length of the key, or if zero, I will use strlen
 {const $Offset c = tree ▷ allocate(sizeof($Content));                          // Space required for a tree node
  const $Offset k = tree ▷ saveString(key, length);                             // Save key string in tree arena
 (($Content *)  c ▷ pointer)->key.delta = k.offset;                             // Save key offset
  return     tree ▷ node(c.offset);                                             // Return node
 }

static $Node new_$Node_tree_string                                              // Create a new tree node keyed by a zero terminated string.
 (const $       tree,                                                           // Arena tree in which to create the node
  const $String key)                                                            // Key for this node.  Note: we do not order nodes automatically.
 {return tree ▷ newn(key, 0);
 }

static $Offset saveString_offset_tree_string                                    //P Save a copy of a zero terminated string in a tree and return the offset of the string.
 (const $       tree,                                                           // Arena tree in which to create the node
  const $String str,                                                            // String
  const size_t  length)                                                         // String, or if zero I will call strlen
 {const $Offset o = tree ▷ allocate((length ? : strlen(str)) + 1);              // Allocate space for the string plus a terminating zero
  const $String t = o ▷ pointer;
  strcpy(t, str);
  return o;
 }

static void fromLetters_tree_str                                                // Load tree from a string of letters and brackets.  The root node of the tree so constructed is always given the letter 'a'.
 (const $ tree,                                                                 // Tree
  $String str)                                                                  // String of letters and brackets describing desired tree structure
 {const $Node n = tree ▷ root;                                                  // The node we are currently  adding to
  n ▷ setKey("a");                                                              // The root is always called "a"

  void parse($Node current)                                                     // Parse the string of letters and brackets to construct the desired tree
   {while(*str)
     {const char c = *str;
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
          current ▷ putLast(tree ▷ new(s));
          ++str; continue;
         }
       }
     }
   }

  parse(n);                                                                     // Parse the string of letters and brackets to construct the desired tree
 };

static void free_tree                                                           // Free an entire arena tree.
 (const $ tree)                                                                 // Arena tree to free
 {if (tree.arena->data) free(tree.arena->data);
  free(tree.arena);
 }

//D1 Navigation                                                                 // Navigate through a tree.

static  $Node parent_$Node_$Node                                                // Get the parent of a child
 (const $Node child)                                                            // Child
 {if (child ▷ isRoot) printStackBackTrace("The root node of a tree has no parent\n");
  const $Delta p = (child ▷ content)->parent;
  const $      t = child.tree;
  return       t ▷ node(p.delta);
 }

static  $Node first_$Node_$Node                                                 // Get the first child under a parent.
 (const $Node parent)                                                           // Parent
 {const $ t = parent.tree;
  return  t ▷ node((parent ▷ content)->first.delta);
 }

static  $Node last_$Node_$Node                                                  // Get the last child under a parent.
 (const $Node parent)                                                           // Parent
 {const $ t = parent.tree;
  return  t ▷ node((parent ▷ content)->last.delta);
 }

static  $Node next_$Node_$Node                                                  // Get the next sibling of the specified child.
 (const $Node child)                                                            // Child
 {const $ t = child.tree;
  return  t ▷ node((child ▷ content)->next.delta);
 }

static  $Node prev_$Node_$Node                                                  // Get the previous sibling of the specified child.
 (const $Node child)                                                            // Child
 {const $ t = child.tree;
  return  t ▷ node((child ▷ content)->prev.delta);
 }

static int findFirstKey_int_tree_string_$NodePointer                            // Find the first node with the specified key in the tree. Returns true and sets found if a matching key is found else returns false
 (const $        tree,                                                          // Tree
  const $String  key,                                                           // Key to find
  $Node  * const result)                                                        // Output area for node if found
 {jmp_buf found;
  $Node F;

  void find($Node node)                                                         // Check whether the key of the current node matches the specified key
   {if (strcmp(node ▷ key, key)) return;                                        // Not found
    F = node;                                                                   // Found
    longjmp(found, 1);
   }

  if (!setjmp(found))                                                           // Search the tree
   {tree ▷ by(find);
    return 0;                                                                   // No matching key
   }
  else
   {*result = F;
    return 1;
   }
 }

//D1 Location                                                                   // Verify the current location.

static int context_$Node                                                        // Return true if the parent of the specified child has the specified key.
 (const $Node  child,                                                           // Child
  $Node * const parent,                                                         // Parent container
  char  * const key)                                                            // Key
 {if (child ▷ isRoot) return 0;                                                 // The root node has no context
  const $Node p = *parent = child ▷ parent;                                     // Get parent if it exists
  return(p.offset && strcmp(p ▷ key, key) == 0);                                // Check key
 }

static int isFirst_$Node                                                        // Confirm a child is first under its parent
 (const $Node child)                                                            // Child
 {const $Node parent = child ▷ parent;
  return child ▷ equals(parent ▷ first);
 }

static int isLast_$Node                                                         // Confirm a child is last under its parent
 (const $Node child)                                                            // Child
 {const $Node parent = child ▷ parent;
  return child ▷ equals(parent ▷ last);
 }

static int isEmpty                                                              // Confirm a node has no children.
 (const $Node node)                                                             // NodeContent
 {const $Node child = node ▷ first;
  return child.offset == 0;                                                     // No first child so the node is empty
 }

static int isOnlyChild                                                          // Confirm that this child is the only child of its parent
 (const $Node child)                                                            // Child
 {const $Node parent = child ▷ parent;                                          // Parent
  return parent.offset != 0 && child ▷ isFirst && child ▷ isLast;               // Child is first and last and not the root so it is an only child
 }

static int isRoot                                                               // Confirm a node is the root
 (const $Node node)                                                             // NodeContent
 {const $Delta p = (node ▷ content)->parent;
  return p.delta == 0;                                                          // No parent so the node is a root node
 }

//D1 Put                                                                        // Insert children into a tree.

static  $Node putFirstLast_$Node_$Node_$Node                                    //P Put a child last under its parent
 (const int   first,                                                            // Put first if true, else last
  const $Node parent,                                                           // Parent
  const $Node child)                                                            // Child
 {const $ tree = parent.tree;
  if (child.tree.arena != tree.arena) printStackBackTrace("Wrong tree\n");      // Parent and child must be in same tree
  const size_t     P = parent.offset,                     C = child.offset;     // Delta of parent and child
  $Content * const p = parent ▷ content; $Content * const c = child  ▷ content; // Pointers to parent and child
  const size_t     L = first ? p->first.delta : p->last.delta;                  // Delta of child currently first/last
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

static  $Node putFirst_$Node_tree_$Node                                         // Put a child first in a tree
 (const $     tree,                                                             // Tree
  const $Node child)                                                            // Child
 {const $Node r = tree ▷ root;
  return r ▷ putFirst(child);                                                   // Put the child first
 }

static  $Node putLast_$Node_tree_$Node                                          // Put a child last in a tree
 (const $     tree,                                                             // Tree
  const $Node child)                                                            // Child
 {const $Node r = tree ▷ root;
  return r ▷ putLast(child);                                                    // Put the child last
 }

static  $Node putFirst_$Node_$Node_$Node                                        // Put a child first under its parent
 (const $Node parent,                                                           // Parent
  const $Node child)                                                            // Child
 {return putFirstLast_$Node_$Node_$Node(1, parent, child);                      // Put a child first under its parent
 }

static  $Node putLast_$Node_$Node_$Node                                         // Put a child last under its parent
 (const $Node parent,                                                           // Parent
  const $Node child)                                                            // Child
 {return putFirstLast_$Node_$Node_$Node(0, parent, child);                      // Put a child last under its parent
 }

static  $Node putNextPrev_$Node_$Node_$Node                                     //P Put a child next or previous to the specified sibling
 (const int   next,                                                             // Put next if true, else previous
  const $Node sibling,                                                          // Sibling
  const $Node child)                                                            // Child
 {const $     tree   = sibling.tree;                                            // Tree
  const $Node parent = sibling ▷ parent;                                        // Parent node
  const $Node twin   = next ? sibling ▷ next : sibling ▷ prev;                  // Next or prev sibling to sibling is such a node exists
  assert(child.tree.arena == tree.arena);                                       // Parent and child must be in same tree
  $Content * const s = sibling ▷ content;                                       // Pointer to sibling
  $Content * const c = child   ▷ content;                                       // Pointer to child

  if (!twin.offset)                                                             // Sibling is last/first under parent
   {if (next) parent ▷ putLast(child); else parent ▷ putFirst(child);           // Place child
   }
  else                                                                          // Not last/first
   {$Content * const w = twin ▷ content;                                        // Pointer to twin of sibling
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

static  $Node putNext_$Node_$Node_$Node                                         // Put a child next after the specified sibling
 (const $Node sibling,                                                          // Sibling
  const $Node child)                                                            // Child
 {return putNextPrev_$Node_$Node_$Node(1, sibling, child);                      // Put child next
 }

static  $Node putPrev_$Node_$Node_$Node                                         // Put a child before the specified sibling
 (const $Node sibling,                                                          // Sibling
  const $Node child)                                                            // Child
 {return putNextPrev_$Node_$Node_$Node(0, sibling, child);                      // Put a child last under its parent
 }

//D1 Traverse                                                                   // Traverse a tree.

#define $fe( child, parent) for($Node child = parent ▷ first; child.offset; child = next_$Node_$Node(child)) // Each child in a parent from first to last
#define $fer(child, parent) for($Node child = parent ▷ last;  child.offset; child = prev_$Node_$Node(child)) // Each child in a parent from last to first

static void by_$_sub                                                            // Traverse a tree in post-order calling the specified function to process each child node.  The tree is buffered allowing changes to be made to the structure of the tree without disruption as long as each child checks its context.
 ($ t,                                                                          // Tree
  void (* const function) (const $Node node))                                   // Function
 {void children(const $Node parent)                                             // Process the children of the specified parent
   {const size_t N = parent ▷ count;                                            // Children plus trailing zero
    $Node c[N];                                                                 // Array of children
    size_t n = 0; $fe(child, parent) c[n++] = child;                             // Load each child into an array
    for(size_t i = 0; i < N; ++i) children(c[i]);                               // Process each child allowing it to change position
    function(parent);                                                           // Process the parent
   }
  children(t ▷ root);                                                           // Start at the root
 }

static  size_t count_sizet_$NodeOffset                                          // Count the number of children in a parent
 (const $Node parent)                                                           // Parent
 {size_t l = 0;
  $fe(child, parent) ++l;
  return l;                                                                     // Return count
 }

static  size_t count_size_t_tree                                                // Count the number of nodes in a tree
 (const $ tree)                                                                 // Tree
 {size_t l = 0;
  void count(const $Node parent)                                                // Process the children of the specified parent
   {l++; $fe(child, parent) count(child);                                       // Each child
    if (l > 99) printStackBackTrace("Too many\n");
   }
  count(tree ▷ root);                                                           // Start at the root
  return l;                                                                     // Return count
 }

static  $String printWithBrackets_string_$Node                                  // Print a node and the tree below it in preorder as a string with each sub tree enclosed in brackets.
 (const $Node   node)                                                           // Node
 {size_t  l = 0;                                                                // Length of output string
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

  len(node);
  const $String s = p = alloc(l+1); *s = 0;
  print(node);
  return s;
 }

static  $String printWithBrackets_string_tree                                   // Print an entire tree in preorder as a string with brackets around each sub tree
 (const $ t)                                                                    // Tree
 {const $Node node = t ▷ root;                                                  // Root
  return node ▷ printWithBrackets;
 }

static  $String print_string_$Node                                              // Print a node and the tree below it in preorder as a string.
 (const $Node   node)                                                           // Node
 {size_t  l = 0;                                                                // Length of output string
  void len(const $Node parent)                                                  // Find the size of buffer we will need
   {l += strlen(parent ▷ key);                                                  // Parent key
    $fe(child, parent) len(child);                                              // Each child
   }

  $String p;
  void print(const $Node parent)                                                // Print the children of the specified parent
   {const $String k = parent ▷ key;
    p = stpcpy(p, k);
    $fe(child, parent) print(child);                                            // Each child
    *p = 0;                                                                     // End the string so far
   }

  len(node);
  const $String s = p = alloc(l+1); *s = 0;
  print(node);
  return s;
 }

static  $String print_string_tree                                               // Print an entire tree in preorder as a string.
 (const $ t)                                                                    // Tree
 {const $Node node = t ▷ root;                                                  // Root
  return node ▷ print;
 }

//D1 Edit                                                                       // Edit a tree in situ.

static  $Node cut_$Node_$Node                                                   // Cut out a child.
 (const $Node child)                                                            // Child to cut out
 {const $Node parent = child  ▷ parent;                                         // Parent
  $Content * const p = parent ▷ content, * const c = child ▷ content;           // Parent pointer
  if (child ▷ isOnlyChild)                                                      // Only child
   {p->first.delta = p->last.delta = 0;                                         // Remove child
   }
  else if (child ▷ isLast)                                                      // Last child
   {const $Node L = child  ▷ prev;
    $Content * const l = L ▷ content;
    p->last.delta =  L.offset; l->next.delta = 0;                               // Last child under parent
   }
  else if (child ▷ isFirst)                                                     // First child
   {const $Node F = child   ▷ next;
    $Content * const f =  F ▷ content;
    p->first.delta = F.offset; f->prev.delta = 0;                               // Last child under parent
   }
  else                                                                          // Some where in the middle
   {const $Node N = child ▷ next,  P = child ▷ prev;
    $Content * const n =  N ▷ content, * const p =  P ▷ content;
    p->next.delta = N.offset; n->prev.delta = P.offset;
   }
  c->next.delta = c->prev.delta = 0;                                            // Remove child

  return child;
 }

static  $Node unwrap_$Node_$Node                                                // Unwrap the specified parent and return it.
 (const $Node parent)                                                           // Parent to unwrap
 {for  ($Node child = parent  ▷ last; child.offset; child = parent ▷ last)      // Each child in a parent from first to last
   {parent ▷ putNext(child ▷ cut);                                              // Place each child after the parent
   }
  return parent ▷ cut;                                                          // Remove and return empty parent
 }

static  $Node wrap_$Node_string_pointer                                         // Wrap the specified child with a new parent and return the new parent optionally setting its L[key] and L[value].
 (const $Node   child,                                                          // Child to wrap
  const $String key)                                                            // Key for new parent
 {const $       tree = child.tree;                                              // Tree
  const $Node   parent = tree ▷ new(key);                                       // New parent
  child  ▷ putNext(parent);                                                     // Place parent after child
  parent ▷ putLast(child ▷ cut);                                                // Place child under parent
  return parent;
 }

//D1 Input and Output                                                           // Read and write a tree from/to a file

static size_t used_$                                                            // Amount of space currently being used within the arena of a tree.
 (const $ tree)                                                                 // Tree
 {return tree.arena->used;
 }

static void write_void_$_string                                                 // Write a tree to a named file or abort
 (const $       tree,                                                           // Tree
  const $String file)                                                           // File
 {const int o = open(file, O_CREAT| O_WRONLY, S_IRWXU);                         // Open for output creating if needed
  if (o < 0) printStackBackTrace("Cannot open file: %s for write\n", file);

  time_t current; time(&current);
  const $Description h = {1, 1, 0, tree ▷ used, tree ▷ root.offset, current};   // Create arena tree header describing the tree

  if (sizeof(h) != write(o, &h, sizeof(h)))                                     // Write header
   {printStackBackTrace("Cannot write $ header to file: %s\n", file);
   }

  const ssize_t w = write(o, tree.arena->data, tree.arena->used);               // Write arena
  if (w < 0 || tree.arena->used != (size_t)w)
   {printStackBackTrace("Cannot write $ arena to file: %s\n", file);
   }

  close(o);
 }

$ read$                                                                         // Read a tree from a file
 (const $String file)                                                           // File
 {$ tree;
  tree.arena = alloc(sizeof($Arena));
  tree.proto = &ProtoTypes_$;                                                   // Initialize tree prototype

  const int i = open(file, 0, O_RDONLY);                                        // Open for input
  if (i < 0) printStackBackTrace("Cannot open file: %s for read\n", file);

  struct $Description h;
  if (sizeof(h) != read(i, &h, sizeof(h)))                                      // Read header
   {printStackBackTrace("Cannot read header from file: %s\n", file);
   }

  tree.arena->data = alloc(tree.arena->size = tree.arena->used = h.used);       // Allocate arena

  const ssize_t r = read(i, tree.arena->data, tree.arena->used);                // Read arena
  if (r < 0 || tree.arena->used != (size_t)r)
   {printStackBackTrace("Cannot read $ from file: %s\n", file);
   }

  close(i);
  tree.proto = &ProtoTypes_$;
  tree.arena->root = h.root;                                                    // Offset to root in arena
  return tree;
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0
#define treeIs(n, s) {$String p = n ▷ printWithBrackets; assert(strcmp(p,s) == 0); free(p);}// Check the content of a tree

void test0()                                                                    //Tnew$ //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer
 {$        t = new$;                                                            // Create a tree
  $Node root = t ▷ root;

  for(int i = 0; i < 10; ++i)                                                   // Load tree
   {char s[2] = {'a'+i, 0};
    $Node         child = t ▷ new(s); root ▷ putLast (child);
                  child = t ▷ new(s); root ▷ putFirst(child);
    s[0] = 'A'+i; child ▷ setKey(s);
    assert(strcmp(child ▷ key, s) == 0);
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

void test1()                                                                    //Troot //Tfirst //Tlast //Tnext //Tprev //Tparent //Tequals //Tprint //TprintWithBrackets //TfromLetters
 {$ t = new$; t ▷ fromLetters("b(c(de)f)g(hi)j");
       treeIs(t ▷ root,     "a(b(c(de)f)g(hi)j)");

  {$String p = t ▷ printWithBrackets; assert(!strcmp(p, "a(b(c(de)f)g(hi)j)")); free(p);}
  {$String p = t ▷ print;             assert(!strcmp(p, "abcdefghij"));         free(p);}

  $Node a = t ▷ root,
        b = a ▷ first,
        c = b ▷ first,
        e = c ▷ last,
        d = e ▷ prev;

  $String     k = d ▷ key;
  assert(k[0] == 'd');
  treeIs(b, "b(c(de)f)");
  treeIs(c,   "c(de)");

  assert(c ▷ equals(d ▷ parent));
  assert(b ▷ equals(c ▷ parent));
  assert(a ▷ equals(b ▷ parent));
  assert(a ▷ equals(t ▷ root));

  t ▷ free;
 }

void test2()                                                                    //Tby
 {$ t = new$; t ▷ fromLetters("b(c(de)f)g(hi)j");
       treeIs(t ▷ root,     "a(b(c(de)f)g(hi)j)");

  char l[t ▷ count + 1]; *l = 0;
  void process($Node n) {strcat(l, n ▷ key);}
  t ▷ by(process);
  assert(strcmp(l, "decfbhigja") == 0);

  t ▷ free;
 }

void test3()                                                                    //TisFirst //TisLast //TisEmpty //TisRoot //TisOnlyChild //Tcontext
 {$ t = new$ ; t ▷ fromLetters("b(c(de(f)gh)i)j");
        treeIs(t ▷ root,     "a(b(c(de(f)gh)i)j)");

  $Node a = t ▷ root;  assert((a ▷ key)[0] == 'a');                                 treeIs(a, "a(b(c(de(f)gh)i)j)");
  $Node b = a ▷ first; assert((b ▷ key)[0] == 'b'); assert(a ▷ equals(b ▷ parent)); treeIs(b, "b(c(de(f)gh)i)");
  $Node c = b ▷ first; assert((c ▷ key)[0] == 'c'); assert(b ▷ equals(c ▷ parent)); treeIs(c, "c(de(f)gh)");
  $Node d = c ▷ first; assert((d ▷ key)[0] == 'd'); assert(c ▷ equals(d ▷ parent)); treeIs(d, "d");
  $Node e = d ▷ next;  assert((e ▷ key)[0] == 'e'); assert(c ▷ equals(e ▷ parent)); treeIs(e, "e(f)");
  $Node f = e ▷ last;  assert((f ▷ key)[0] == 'f'); assert(e ▷ equals(f ▷ parent)); treeIs(f, "f");
  $Node g = e ▷ next;  assert((g ▷ key)[0] == 'g'); assert(c ▷ equals(g ▷ parent)); treeIs(g, "g");
  $Node h = g ▷ next;  assert((h ▷ key)[0] == 'h'); assert(c ▷ equals(h ▷ parent)); treeIs(h, "h");
  $Node i = c ▷ next;  assert((i ▷ key)[0] == 'i'); assert(b ▷ equals(i ▷ parent)); treeIs(i, "i");
  $Node j = b ▷ next;  assert((j ▷ key)[0] == 'j'); assert(a ▷ equals(j ▷ parent)); treeIs(j, "j");

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

void test4()                                                                    //Tcut //TfindFirstKey
 {$ t = new$ ; t ▷ fromLetters("b(c(de(f)gh)i)j");
        treeIs(t ▷ root,     "a(b(c(de(f)gh)i)j)");

  $Node f, g;

  assert(t ▷ findFirstKey("f", &f) && t ▷ findFirstKey("g", &g));

  g ▷ putNext(f ▷ cut); treeIs(t ▷ root, "a(b(c(degfh)i)j)");

  t ▷ free;
 }

void test5()                                                                    //TreadArenaTree //Twrite
 {$ t = new$ ; t ▷ fromLetters("b(c(de(f)gh)i)j");
        treeIs(t ▷ root,     "a(b(c(de(f)gh)i)j)");

  $String f = "/tmp/arenaTreeTest.data";
        treeIs(t ▷ root,     "a(b(c(de(f)gh)i)j)");
               t ▷ write(f);

  $      u = read$      (f);
  treeIs(u ▷ root,           "a(b(c(de(f)gh)i)j)");

  t ▷ free;
  u ▷ free;
  unlink(f);
 }

void test6()                                                                    //Tunwrap //Twrap //Tcount
 {$ t = new$; t ▷ fromLetters("bce");
  treeIs(t ▷ root,          "a(bce)");
  assert(t ▷ count == 4);

  $Node c;  assert(t ▷ findFirstKey("c", &c));

  $Node d = c ▷ wrap("d");
  treeIs (t ▷ root, "a(bd(c)e)");
  assert (t ▷ count == 5);

  d ▷ unwrap;
  treeIs(t ▷ root,  "a(bce)");
  assert(t ▷ count == 4);
         t ▷ free;
 }

void test7()                                                                    //TsetKey //Tkey //TsaveString //Tused //Tsize
 {$ t = new$ ; t ▷ fromLetters("");
  $Delta   d = {delta : 1};
  assert(sizeof(d) == 4);

  $Offset a = t ▷ saveString("aaaa", 0);
  assert(strcmp(a ▷ pointer, "aaaa") == 0);

  $Node      n = t ▷ new   ("a");
                 n ▷ setKey("A");
  assert(strcmp (n ▷ key,   "A") == 0);

  t ▷ free;
 }

void test8()                                                                    //TputNext //TputPrev //Tnewn
 {$ t = new$; t ▷ fromLetters("A");
  $Node a = t ▷ root;  assert((a ▷ key)[0] == 'a');
  $Node A = a ▷ first; assert((A ▷ key)[0] == 'A');

  for(int i = 0; i < 10; ++i)
   {char c[2] = {'0'+i};
    A ▷ putNext(t ▷ newn(c, 1));
    A ▷ putPrev(t ▷ newn(c, 1));
   }

  treeIs(t ▷ root, "a(0123456789A9876543210)");

  t ▷ free;
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test
  void (*tests[])(void) = {test0, test1, test2, test3, test4, test5,
                           test6, test7, test8, 0};
  run_tests("$", repetitions, tests);

  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaTree/arenaTree
