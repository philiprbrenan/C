static ArenaArray makeArenaArray
 (const size_t width);
static  void * allocate__ArenaArray
 (const ArenaArray    * array);
static int notEmpty_int__ArenaArray
 (const ArenaArray * array);
static int empty_int__ArenaArray
 (const ArenaArray * array);
static size_t width_size__ArenaArray
 (const ArenaArray * array);
static void free__ArenaArray
 (const ArenaArray * array);
static size_t count__ArenaArray_pointer
 (const ArenaArray * array);
static void * top__ArenaArray
 (const ArenaArray * array);
static void * push__ArenaArray_pointer
 (const ArenaArray * array);
static void pushArray__ArenaArray_ArenaArray
 (const ArenaArray * first,
  const ArenaArray   second);
static void * pop__ArenaArray
 (const ArenaArray * array);
static void * at__ArenaArray_index
 (const ArenaArray * array,
  size_t    index);
static void reverse__ArenaArray
 (const ArenaArray * array);
static void write__ArenaArray_string
 (const ArenaArray    *       array,
  const char * const file);
struct ProtoTypes_ArenaArray {
  void *  (*allocate)(                                                          // Allocate the next element of the ArenaArray
    const ArenaArray    * array);                                               // ArenaArray in which to allocate
  void *  (*at)(                                                                // The address of the element at the specified index counting from one, else 0 if the index is not held in the ArenaArray
    const ArenaArray * array,                                                   // ArenaArray
    size_t index);                                                              // Index
  size_t  (*count)(                                                             // Number of elements in the ArenaArray
    const ArenaArray * array);                                                  // ArenaArray
  int  (*empty)(                                                                // Whether the specified ArenaArray is empty
    const ArenaArray * array);                                                  // ArenaArray
  void  (*free)(                                                                // Free an entire ArenaArray.
    const ArenaArray * array);                                                  // ArenaArray to free
  int  (*notEmpty)(                                                             // Whether the specified ArenaArray has an elements
    const ArenaArray * array);                                                  // ArenaArray
  void *  (*pop)(                                                               // Pop and return the address of the top most element
    const ArenaArray * array);                                                  // ArenaArray
  void  (*pushArray)(                                                           // Push a copy of the second array onto the first array.
    const ArenaArray * first,                                                   // First ArenaArray
    const ArenaArray second);                                                   // Second ArenaArray
  void *  (*push)(                                                              // Push an element on to the ArenaArray and return its address
    const ArenaArray * array);                                                  // ArenaArray
  void  (*reverse)(                                                             // Reverse a ArenaArray in situ.
    const ArenaArray * array);                                                  // ArenaArray
  void *  (*top)(                                                               // Address of the top most element
    const ArenaArray * array);                                                  // ArenaArray
  size_t  (*width)(                                                             // Get the width of an element in the ArenaArray
    const ArenaArray * array);                                                  // ArenaArrayNode
  void  (*write)(                                                               // Write a ArenaArray to a named file or abort
    const ArenaArray    * array,                                                // ArenaArray
    const char * const file);                                                   // File
 } const ProtoTypes_ArenaArray =
{allocate__ArenaArray, at__ArenaArray_index, count__ArenaArray_pointer, empty_int__ArenaArray, free__ArenaArray, notEmpty_int__ArenaArray, pop__ArenaArray, pushArray__ArenaArray_ArenaArray, push__ArenaArray_pointer, reverse__ArenaArray, top__ArenaArray, width_size__ArenaArray, write__ArenaArray_string};
ArenaArray newArenaArray(ArenaArray allocator) {return allocator;}

struct ProtoTypes_ArenaArrayArena {
 } const ProtoTypes_ArenaArrayArena =
{};
ArenaArrayArena newArenaArrayArena(ArenaArrayArena allocator) {return allocator;}

struct ProtoTypes_ArenaArrayDescription {
 } const ProtoTypes_ArenaArrayDescription =
{};
ArenaArrayDescription newArenaArrayDescription(ArenaArrayDescription allocator) {return allocator;}

