//------------------------------------------------------------------------------
// Array held in an arena on which stack operations can be performed
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE

//D1 Structures                                                                 // Structures describing an $.
#ifndef $_included
#define $_included
#include <utilities.c>

typedef struct $                                                                // $
 {const struct ProtoTypes_$ *proto;                                             // Methods associated with an $
  struct $Arena *arena;                                                         // The arena for the $
 } $;

typedef struct $Arena                                                           // Description of the block of memory that holds an $.  The individual nodes of the $ cannot be cannot be freed individually, but the entire $ can be freed. As the $ uses offsets to store addresses the $ can be moved to a different location in memory while retaining its structure.
 {size_t size;                                                                  // The total size of the arena used to hold data associated with the $.
  size_t used;                                                                  // The number of bytes currently used.
  size_t width;                                                                 // Width of each array element
  void * data;                                                                  // The arena containing the data associated with the $
 } $Arena;

typedef struct $Description                                                     // The description of an $ which is written as the header record for dump files.
 {const size_t version;                                                         // Version of $
  const size_t littleEndian;                                                    // Little endian if true
  const size_t s64b;                                                            // 64 bit address if true, else 32 bit
  const size_t used;                                                            // The number of bytes currently used by the arena.
  const size_t width;                                                           // Width of user data associated with each node
  const time_t created;                                                         // Time in seconds since the epoch when this $ was created
 } $Description;

#include <$$_prototypes.h>                                                      // $ prototypes now that the relevant structures have been declared

//D1 Constructors                                                               // Construct a new $.

static $ make$                                                                  // Create a new $ with the specified width for the user held in the $
 (const size_t width)                                                           // Width
 {$Arena * a = alloc(sizeof($Arena));                                           // Allocate arena description
  memset(a, 0, sizeof($Arena));                                                 // Clear arena description
  t ◁ new $(arena: a);                                                          // $ we are creating

  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  a->used   = 0;                                                                // Length used so far
  a->width  = width;                                                            // Width of user data associated with each node
  return t;
 }

//D1 Allocation                                                                 // Allocating memory in the $

static  void * allocate__$                                                      //VP Allocate the next element of the $
 (const $    * array)                                                           // $ in which to allocate
 {width ◁ array->arena->width;                                                  // Amount of memory required for an element
  if (array->arena->used + width < array->arena->size)                          // Allocate within existing arena
   {a ◁ array->arena->data + array->arena->used;                                // Existing Allocate
    array->arena->used += width;                                                // Allocate
    return a;                                                                   // Return allocation
   }
  else                                                                          // Reallocate arena
   {S ◁ nextPowerOfTwo(array->arena->size + width);                             // Round up memory required
    m ◁ realloc(array->arena->data, S);                                         // Reallocate arena - the old one will be copied into it if it is moved.
    if (m)                                                                      // Retry the memory allocation
     {array->arena->data = m;                                                   // New arena
      array->arena->size = S;                                                   // Arena size
      u ◁ array->arena->used;                                                   // Length of free space
      memset(array->arena->data + u, 0, S - u);                                 // Clear free space
      return array ▶ allocate;                                                  // Allocate within arena
     }
   }
  printStackBackTraceAndExit(2, "Requested arena too large\n");                 // The arena has become too large for the chosen size of offsets.
 }

static int notEmpty_int__$                                                      // Whether the specified $ has an elements
 (const $ * array)                                                              // $
 {return !!array->arena->used;
 }

static int empty_int__$                                                         // Whether the specified $ is empty
 (const $ * array)                                                              // $
 {return  !array->arena->used;
 }

static size_t width_size__$                                                     // Get the width of an element in the $
 (const $ * array)                                                              // $Node
 {return array->arena->width;
 }

static void free__$                                                             // Free an entire $.
 (const $ * array)                                                              // $ to free
 {a ◁ array->arena;
  if  (a->data) free(a->data);
  free(a);
 }

//D1 Statistics                                                                 // Numbers describing the $

static size_t count__$_pointer                                                  // Number of elements in the $
 (const $ * array)                                                              // $
 {return array->arena->used / array->arena->width;
 }

//D1 Stack                                                                      // Stack operations on an $

static void * top__$                                                            //V Address of the top most element
 (const $ * array)                                                              // $
 {if (array->arena->used)                                                       // $ has elements
   {return array->arena->data + array->arena->used - array->arena->width;
   }
  return 0;                                                                     // Empty $
 }

static void * push__$_pointer                                                   //V Push an element on to the $ and return its address
 (const $ * array)                                                              // $
 {return array ▶ allocate;                                                      // Allocate space and return its address
 }

static void * pop__$                                                            //V Pop and return the address of the top most element
 (const $ * array)                                                              // $
 {if (array ▶ count)                                                            // $ has elements
   {w ◁ array->arena->width;                                                    // Width of elements in $
    p ◁ array->arena->data + array->arena->used - w;
    array->arena->used -= w;
    return p;
   }
  return 0;                                                                     // Empty $
 }

//D1 Traverse                                                                   // Traverse a $.

static void * at__$_index                                                       //V The address of the element at the specified index counting from one, else 0 if the index is not held in the $
 (const $ * array,                                                              // $
  size_t    index)                                                              // Index
 {if (!index)                                                                   // Index is one based
   {printStackBackTrace("Index from one not zero\n");
   }
  else if (index <= array ▶ count)                                              // Index in range
   {return array->arena->data + (index - 1) * array->arena->width;              // One based
   }
  return 0;                                                                     // Index out of range
 }

static void reverse__$                                                          // Reverse a $ in situ.
 (const $ * array)                                                              // $
 {count ◁ array ▶ count;                                                        // Number of elements
  if (count <= 1) return;                                                       // Already reversed
  m ◁ count / 2;
  width ◁ array ▶ width;                                                        // Width of each element
  buffer ≋ width;
  for(i ◀ 0ul; i < m; ++i)                                                      // Swap elements
   {j ◁ count - 1 - i;                                                          // Opposing element
    J ◁ array->arena->data + j * width;
    I ◁ array->arena->data + i * width;
    memcpy(buffer, J, width);
    memcpy(J,      I, width);
    memcpy(I, buffer, width);
   }
 }

//D1 Print                                                                      // Print $s in various ways

//D1 Input and Output                                                           // Read and write a $ from/to a file

static void write__$_string                                                     // Write a $ to a named file or abort
 (const $    *       array,                                                     // $
  const char * const file)                                                      // File
 {    o ◁ open(file, O_CREAT| O_WRONLY, S_IRWXU);                               // Open for output creating if needed
  if (o < 0) printStackBackTrace("Cannot open file: %s for write\n", file);

  time_t current; time(&current);
  const $Description h =                                                        // Create $ header describing the $
   {1, 1, 0, array->arena->used, array->arena->width, current};

  if (sizeof(h) != write(o, &h, sizeof(h)))                                     // Write header
   {printStackBackTrace("Cannot write $ header to file: %s\n", file);
   }

  w ◁ write(o, array->arena->data, array->arena->used);                         // Write arena
  if (w < 0 || array->arena->used != (size_t)w)
   {printStackBackTrace("Cannot write $ arena to file: %s\n", file);
   }

  close(o);
 }

$ read$                                                                         // Read a $ from a file
 (const char * const file)                                                      // File
 {$Arena * const arena = alloc(sizeof($Arena));                                 // Create arena
  array ◁ new $(arena: arena);                                                  // Initialize $

      i ◁ open(file, 0, O_RDONLY);                                              // Open for input
  if (i < 0) printStackBackTrace("Cannot open file: %s for read\n", file);

  struct $Description h;
  if (sizeof(h) != read(i, &h, sizeof(h)))                                      // Read header
   {printStackBackTrace("Cannot read header from file: %s\n", file);
   }

  array.arena->data = alloc(arena->size = arena->used = h.used);                // Allocate arena

      r ◁ read(i, arena->data, arena->used);                                    // Read arena
  if (r < 0 || arena->used != (size_t)r)
   {printStackBackTrace("Cannot read $ from file: %s\n", file);
   }

  close(i);
  arena->width = h.width;                                                       // Width of each element
  return array;
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //Tmake$ //Tcount //Tpush //Ttop //Tat //Tpop //Tfree //Tempty //TnotEmpty //Twidth
 {a ◁ make$(sizeof(size_t));

  ✓ a ▷ empty;  ✓ a ▷ count == 0; ✓ a ▷ width == sizeof(size_t);
  p ◁ a ▷ push; ✓ a ▷ count == 1; p ◧ 2ul; ✓ p ◧◧ 2ul; ✓ a ▷ notEmpty;
  q ◁ a ▷ push; ✓ a ▷ count == 2; q ◧ 4ul; ✓ p ◧◧ 2ul; ✓ q ◧◧ 4ul;
  t ◁ a ▷ top;  ✓ a ▷ count == 2; ✓ q == t;

//a0 ◁ a ▷ at(0); ✓ a0 == 0;
  a1 ◁ a ▷ at(1); ✓ a1 ◧◧ 2ul;
  a2 ◁ a ▷ at(2); ✓ a2 ◧◧ 4ul;
  a3 ◁ a ▷ at(3); ✓ a3 == 0;

  s ◁ a ▷ pop;  ✓ a ▷ count == 1; ✓ q == s;
  r ◁ a ▷ pop;  ✓ a ▷ count == 0; ✓ r == p;

  a ▷ free;
 }

void test1()                                                                    //Treverse
 {a ◁ make$(sizeof(size_t));
  N ◁ 10ul;
  for(i ◀ 1ul; i <= N; ++i) {p ◁ a ▷ push;    p  ◧ N - i + 1;}
  for(i ◀ 1ul; i <= N; ++i) {p ◁ a ▷ at(i); ✓ p !◧ i;}
  a ▷ reverse;
  for(i ◀ 1ul; i <= N; ++i) {p ◁ a ▷ at(i); ✓ p ◧◧ i;}

  a ▷ free;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaArray/arenaArray
