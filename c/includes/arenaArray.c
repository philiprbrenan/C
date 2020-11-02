#line 1 "/home/phil/c/z/arenaArray/arenaArray.c"
//------------------------------------------------------------------------------
// Array held in an arena on which stack operations can be performed
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an ArenaArray.
#ifndef ArenaArray_included
#define ArenaArray_included

typedef struct ArenaArray                                                                // ArenaArray
 {const struct ProtoTypes_ArenaArray *proto;                                             // Methods associated with an ArenaArray
  struct ArenaArrayArena *arena;                                                         // The arena for the ArenaArray
 } ArenaArray;

typedef struct ArenaArrayArena                                                           // Description of the block of memory that holds an ArenaArray.  The individual nodes of the ArenaArray cannot be cannot be freed individually, but the entire ArenaArray can be freed. As the ArenaArray uses offsets to store addresses the ArenaArray can be moved to a different location in memory while retaining its structure.
 {size_t size;                                                                  // The total size of the arena used to hold data associated with the ArenaArray.
  size_t used;                                                                  // The number of bytes currently used.
  size_t width;                                                                 // Width of each array element
  void * data;                                                                  // The arena containing the data associated with the ArenaArray
 } ArenaArrayArena;

typedef struct ArenaArrayDescription                                                     // The description of an ArenaArray which is written as the header record for dump files.
 {const size_t version;                                                         // Version of ArenaArray
  const size_t littleEndian;                                                    // Little endian if true
  const size_t s64b;                                                            // 64 bit address if true, else 32 bit
  const size_t used;                                                            // The number of bytes currently used by the arena.
  const size_t width;                                                           // Width of user data associated with each node
  const time_t created;                                                         // Time in seconds since the epoch when this ArenaArray was created
 } ArenaArrayDescription;

#include <arenaArray_prototypes.h>                                                      // ArenaArray prototypes now that the relevant structures have been declared

//D1 Constructors                                                               // Construct a new ArenaArray.

static ArenaArray makeArenaArray                                                                  // Create a new ArenaArray with the specified width for the user held in the ArenaArray
 (const size_t width)                                                           // Width
 {ArenaArrayArena * a = alloc(sizeof(ArenaArrayArena));                                           // Allocate arena description
  memset(a, 0, sizeof(ArenaArrayArena));                                                 // Clear arena description
  const typeof(newArenaArray(({struct ArenaArray t = {arena: a, proto: &ProtoTypes_ArenaArray}; t;}))) t = newArenaArray(({struct ArenaArray t = {arena: a, proto: &ProtoTypes_ArenaArray}; t;}));                                                          // ArenaArray we are creating

  a->size   = 256;                                                              // This could be any reasonable value - it will be doubled everytime the arena overflows.
  a->data   = alloc( a->size);                                                  // Allocate arena
  memset(a->data, 0, a->size);                                                  // ValGrind
  a->used   = 0;                                                                // Length used so far
  a->width  = width;                                                            // Width of user data associated with each node
  return t;
 }

//D1 Pointers and Offsets                                                       // Operations on pointers and offsets

static void * pointer11__ArenaArray_size                                                 //PV Return a temporary pointer to the indexed item
 (const ArenaArray   *  array,                                                            // ArenaArray
  const size_t index)                                                           // Index of item
 {const typeof(index * array->proto->width(array)) o = index * array->proto->width(array);                                                     // Offset of index
  if (o >= array->arena->used)                                                   // Check that the offset is valid
   {printStackBackTraceAndExit(1, "Accessing area outside arena");
   }
  return (void *)(array->arena->data + o);                                       // Convert an offset in the arena to a temporary pointer
 }

static size_t width_size__ArenaArray                                                     // Get the width of an element in the ArenaArray
 (const ArenaArray * array)                                                               // ArenaArrayNode
 {return array->arena->width;
 }

//D1 Allocation                                                                 // Allocating memory in the ArenaArray

static  void * allocate__ArenaArray                                                      //VP Allocate the next element of the ArenaArray
 (const ArenaArray    * array)                                                            // ArenaArray in which to allocate
 {const typeof(array->proto->width(array)) width = array->proto->width(array);                                                         // Amount of memory required for an element
  if (array->arena->used + width < array->arena->size)                            // Allocate within existing arena
   {const typeof(array->arena->data + array->arena->used) a = array->arena->data + array->arena->used;                                  // Existing Allocate
    array->arena->used += width;                                                 // Allocate
    return a;                                                                   // Return allocation
   }
  else                                                                          // Reallocate arena
   {const typeof(nextPowerOfTwo(array->arena->size + width)) S = nextPowerOfTwo(array->arena->size + width);                              // Round up memory required
    const typeof(realloc(array->arena->data, S)) m = realloc(array->arena->data, S);                                          // Reallocate arena - the old one will be copied into it if it is moved.
    if (m)                                                                      // Retry the memory allocation
     {array->arena->data = m;                                                    // New arena
      array->arena->size = S;                                                    // Arena size
      const typeof(array->arena->used) u = array->arena->used;                                                    // Length of free space
      memset(array->arena->data + u, 0, S - u);                                  // Clear free space
      return array->proto->allocate(array);                                                   // Allocate within arena
     }
   }
  printStackBackTraceAndExit(2, "Requested arena too large\n");                 // The arena has become too large for the chosen size of offsets.
 }

static size_t used_size__ArenaArray                                                      // Amount of space currently being used within the arena of a ArenaArray.
 (const ArenaArray * array)                                                               // ArenaArray
 {return array->arena->used;
 }

static void free__ArenaArray                                                             // Free an entire ArenaArray.
 (const ArenaArray * array)                                                               // ArenaArray to free
 {const typeof(array->arena) a = array->arena;
  if  (a->data) free(a->data);
  free(a);
 }

//D1 Statistics                                                                 // Numbers describing the ArenaArray

static size_t count__ArenaArray_pointer                                                  // Number of elements in the ArenaArray
 (const ArenaArray * array)                                                               // ArenaArray
 {return array->arena->used / array->arena->width;
 }

//D1 Stack                                                                      // Stack operations on an ArenaArray

static void * top__ArenaArray                                                            //V Address of the top most element
 (const ArenaArray * array)                                                               // ArenaArray
 {const typeof(array->proto->width(array)) w = array->proto->width(array);                                                             // Width of elements in ArenaArray
  if (array->arena->used >= w)                                                   // ArenaArray has elements
   {return array->arena->data + array->arena->used - w;
   }
  return 0;                                                                     // Empty ArenaArray
 }

static void * push__ArenaArray_pointer                                                   //V Push an element on to the ArenaArray and return its address
 (const ArenaArray * array)                                                               // ArenaArray
 {return array->proto->allocate(array);                                                       // Allocate space and return its address
 }

static void * pop__ArenaArray                                                            //V Pop and return the address of the top most element
 (const ArenaArray * array)                                                               // ArenaArray
 {if (array->proto->count(array))                                                             // ArenaArray has elements
   {const typeof(array->proto->width(array)) w = array->proto->width(array);                                                             // Width of elements in ArenaArray
    const typeof(array->arena->data + array->arena->used - w) p = array->arena->data + array->arena->used - w;
    array->arena->used -= w;
    return p;
   }
  return 0;                                                                     // Empty ArenaArray
 }

//D1 Traverse                                                                   // Traverse a ArenaArray.

static void * at__ArenaArray_index                                                       //V The address of the element at the specified index counting from one, else 0 if the index is not held in the ArenaArray
 (const ArenaArray * array,                                                               // ArenaArray
  size_t    index)                                                              // Index
 {if (!index)                                                                   // Index is one based
   {printStackBackTrace("Index from one not zero\n");
   }
  else if (index <= array->proto->count(array))                                               // Index in range
   {return array->arena->data + (index - 1) * array->proto->width(array);                      // One based
   }
  return 0;                                                                     // Index out of range
 }

//D1 Print                                                                      // Print ArenaArrays in various ways

//D1 Input and Output                                                           // Read and write a ArenaArray from/to a file

static void write__ArenaArray_string                                                     // Write a ArenaArray to a named file or abort
 (const ArenaArray    *       array,                                                      // ArenaArray
  const char * const file)                                                      // File
 {    const typeof(open(file, O_CREAT| O_WRONLY, S_IRWXU)) o = open(file, O_CREAT| O_WRONLY, S_IRWXU);                               // Open for output creating if needed
  if (o < 0) printStackBackTrace("Cannot open file: %s for write\n", file);

  time_t current; time(&current);
  const ArenaArrayDescription h =                                                        // Create ArenaArray header describing the ArenaArray
   {1, 1, 0, array->proto->used(array), array->proto->width(array), current};

  if (sizeof(h) != write(o, &h, sizeof(h)))                                     // Write header
   {printStackBackTrace("Cannot write ArenaArray header to file: %s\n", file);
   }

  const typeof(write(o, array->arena->data, array->arena->used)) w = write(o, array->arena->data, array->arena->used);                           // Write arena
  if (w < 0 || array->arena->used != (size_t)w)
   {printStackBackTrace("Cannot write ArenaArray arena to file: %s\n", file);
   }

  close(o);
 }

ArenaArray readArenaArray                                                                         // Read a ArenaArray from a file
 (const char * const file)                                                      // File
 {ArenaArrayArena * const arena = alloc(sizeof(ArenaArrayArena));                                 // Create arena
  const typeof(newArenaArray(({struct ArenaArray t = {arena: arena, proto: &ProtoTypes_ArenaArray}; t;}))) array = newArenaArray(({struct ArenaArray t = {arena: arena, proto: &ProtoTypes_ArenaArray}; t;}));                                                   // Initialize ArenaArray

      const typeof(open(file, 0, O_RDONLY)) i = open(file, 0, O_RDONLY);                                              // Open for input
  if (i < 0) printStackBackTrace("Cannot open file: %s for read\n", file);

  struct ArenaArrayDescription h;
  if (sizeof(h) != read(i, &h, sizeof(h)))                                      // Read header
   {printStackBackTrace("Cannot read header from file: %s\n", file);
   }

  array.arena->data = alloc(arena->size = arena->used = h.used);                 // Allocate arena

      const typeof(read(i, arena->data, arena->used)) r = read(i, arena->data, arena->used);                                    // Read arena
  if (r < 0 || arena->used != (size_t)r)
   {printStackBackTrace("Cannot read ArenaArray from file: %s\n", file);
   }

  close(i);
  arena->width = h.width;                                                       // Width of each element
  return array;
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TmakeArenaArray //Tcount //Tpush //Ttop //Tat //Tpop //Tfree
 {const typeof(makeArenaArray(sizeof(size_t))) a = makeArenaArray(sizeof(size_t));

                assert( a.proto->count(&a) == 0);
  const typeof(a.proto->push(&a)) p = a.proto->push(&a); assert( a.proto->count(&a) == 1); ({typeof(2ul) sourcesourcesource = 2ul;  memcpy((void *)p,  (void *)&sourcesourcesource, sizeof(2ul));}); assert( ({typeof(2ul) sourcesourcesource = 2ul; !memcmp((void *)p,  (void *)&sourcesourcesource, sizeof(2ul));}));
  const typeof(a.proto->push(&a)) q = a.proto->push(&a); assert( a.proto->count(&a) == 2); ({typeof(4ul) sourcesourcesource = 4ul;  memcpy((void *)q,  (void *)&sourcesourcesource, sizeof(4ul));}); assert( ({typeof(2ul) sourcesourcesource = 2ul; !memcmp((void *)p,  (void *)&sourcesourcesource, sizeof(2ul));})); assert( ({typeof(4ul) sourcesourcesource = 4ul; !memcmp((void *)q,  (void *)&sourcesourcesource, sizeof(4ul));}));
  const typeof(a.proto->top(&a)) t = a.proto->top(&a);  assert( a.proto->count(&a) == 2); assert( q == t);

//const typeof(a.proto->at(&a, 0)) a0 = a.proto->at(&a, 0); assert( a0 == 0);
  const typeof(a.proto->at(&a, 1)) a1 = a.proto->at(&a, 1); assert( ({typeof(2ul) sourcesourcesource = 2ul; !memcmp((void *)a1,  (void *)&sourcesourcesource, sizeof(2ul));}));
  const typeof(a.proto->at(&a, 2)) a2 = a.proto->at(&a, 2); assert( ({typeof(4ul) sourcesourcesource = 4ul; !memcmp((void *)a2,  (void *)&sourcesourcesource, sizeof(4ul));}));
  const typeof(a.proto->at(&a, 3)) a3 = a.proto->at(&a, 3); assert( a3 == 0);

  const typeof(a.proto->pop(&a)) s = a.proto->pop(&a);  assert( a.proto->count(&a) == 1); assert( q == s);
  const typeof(a.proto->pop(&a)) r = a.proto->pop(&a);  assert( a.proto->count(&a) == 0); assert( r == p);

  a.proto->free(&a);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, 0};
  run_tests("ArenaArray", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaArray/arenaArray
