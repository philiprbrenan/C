//------------------------------------------------------------------------------
// Byte strings
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef ByteString_included
#define ByteString_included

#include <assert.h>
#include <fcntl.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing the tree
typedef struct BytesShort     BytesShort;                                       // A short byte string as stored in memory
typedef struct BytesLong      BytesLong;                                        // A long byte string as stored in memory
typedef struct ByteString     ByteString;                                       // Byte string description
typedef struct BytesAllocator BytesAllocator;                                   // Byte string description
typedef char  *               string;                                           // C string
typedef unsigned char length_of_short_string_t;                                 // Type of length field for a short string
typedef size_t        length_of_long_string_t;                                  // Type of length field for a long string

struct BytesShort                                                               // A short byte string as stored in memory
 {length_of_short_string_t width;                                               // The maximum width of this field - except that bit 0 is always 0.
  length_of_short_string_t length;                                              // Used length
  char          content[0];                                                     // Offset
 };

struct BytesLong                                                                // A long byte string as stored in memory
 {length_of_long_string_t width;                                                // The maximum width of this field - except that bit 0 is always 1.
  length_of_long_string_t length;                                               // Used length
  char   content[0];                                                            // Offset
 };

struct BytesAllocator                                                           // Means of allocating memory in which to store a byte string
 {void *(*allocator)(size_t length);                                            // Allocation method
  const struct ProtoTypes_BytesAllocator *proto;                                // Methods associated with byte allocators
 };

struct ByteString                                                               // Manipulate a byte string.
 {string data;                                                                  // Pointer to bytes in memory occupied by string
  BytesAllocator allocator;                                                     // Means used to allocate memory for the byte string described
  const struct ProtoTypes_ByteString *proto;                                    // Methods associated with byte strings
 };

#include <byteString_prototypes.h>                                              // Prototypes now that the relevant structures have been declared

//D1 Allocation                                                                 // Allocating memory in the tree

#define maximum_length_of_short_string (1<<(sizeOf(length_of_short_string_t)*8-1)) // Maximum length of short string

BytesAllocator newBytesAllocator                                                // Allocate a ByteString allocator
 (void *(*alloc)(size_t length))                                                // Function to allocate memory - malloc if NULL
 {BytesAllocator a = {alloc ? : malloc, &ProtoTypes_BytesAllocator};
  return a;
 }

static ByteString bytes_bytesAllocator_sizet_string_sizet                       // Allocate a byte string of the specified length and optionally load it from a string
 (BytesAllocator allocator,                                                     // Allocation method
  size_t         length,                                                        // Length of allocation. If zero the string length is used.
  string         str,                                                           // String
  size_t         strLength)                                                     // Length
 {const size_t len = length > strLength ? length : strLength;
  if (len == 0)     printStackBackTrace("Zero length string of bytes requested\n");
  if (length  < maximum_length_of_short_string)                                 // Use a short string if possible
   {BytesShort *s = allocator.allocator(sizeof(BytesShort) + len);
    s->length = (typeof(((BytesShort *)0)->length))(length << 1);
    strncpy(s->content, str, strLength);
    ByteString b = {(void *)s, allocator, &ProtoTypes_ByteString};
    return b;
   }
  else                                                                          // Use a long string
   {BytesLong *l = allocator.allocator(sizeof(BytesLong) + len);
    l->length = length < 1;
    strncpy(l->content, str, strLength);
    ByteString b = {(void *)l, allocator, &ProtoTypes_ByteString};
    return b;
   }
 }

static size_t length_byteString                                                 // Get the length of a string of bytes from the contentCopy the specified bytes into the specified preallocated string and add a terminating zero. The string should be preallocated large enough to take the specified byte content and terminator.
 (char *bytes)                                                                  // Bytes
 {const char length = *bytes;
  if (length && 0x01 == 0) return length >> 1;
  const size_t l = *(size_t *)bytes;
  return l >> 1;
 }

static size_t used_byteString                                                   // Get the used length of a string of bytes from the contentCopy the specified bytes into the specified preallocated string and add a terminating zero. The string should be preallocated large enough to take the specified byte content and terminator.
 (char *bytes)                                                                  // Bytes
 {const char length = *bytes;
  if (length && 0x01 == 0) return length >> 1;
  const size_t l = *(size_t *)bytes;
  return l >> 1;
 }

static size_t copy_bytes_into_string                                            // Copy the specified bytes into the specified preallocated string and add a terminating zero. The string should be preallocated large enough to take the specified byte content and terminator.
 (ByteString  bytes,                                                                 // Bytes
  string str)                                                                   // Target string
 {if (bytes.length < 1<<7)
   {BytesShort *s = (BytesShort *)bytes.data;
    strncpy(str, s->content, bytes.length);
   }
  else
   {BytesLong *l = (BytesLong *)bytes.data;
    strncpy(str, l->content, bytes.length);
   }
  str[bytes.length] = 0;
  return str;
 }

static ByteString concatenate_bytes_strings                                          // Concatenate zero or more ByteString.
 (BytesAllocator  allocator,                                                    // Bytes to concatenate
  ByteString  *bytes)                                                                // Bytes to concatenate
 {return (ByteString){0,0,allocator, &ProtoTypes_ByteString};
 }

#define copy_bytes_to_local_string(bytes, str) char str[bytes.length + 1]; bytes ▷ copy(str); // Allocate a string string on the stack and copy the specified bytes into it adding a zero terminator at the end of the local string.

static int compare_int_bytes_string                                             // Compare the specified bytes with the specified string
 (ByteString  bytes,                                                                 // Bytes
  string str)                                                                   // Target string
 {return strncmp(((void *)((char *)bytes.data)+1), str, bytes.length);
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //Tcompare //Tcopy
 {BytesAllocator  a = newBytesAllocator(NULL);
  volatile string s = "aaa";
  ByteString  b = a ▷ bytes  (s, strlen(s));
  assert    (b ▷ compare(s) == 0);

  char B[256]; b ▷ copy(B);         assert(strcmp(B, s) == 0);
  copy_bytes_to_local_string(b, S); assert(strcmp(s, S) == 0);
 }

int main(void)                                                                  // Run tests
 {const int repetitions = 1;                                                    // Number of times to test

  void (*tests[])(void) = {test0, 0};

  clock_t start = clock();                                                      // Start time
  int n = 0;
  for(int j = 0; j < repetitions; ++j)                                          // Test a few times
   {n = 0;
    say("Test:");
    for(int i = 0; tests[i]; ++i, ++n)                                          // Tests
     {say("%d ", i);
      tests[i]();
     }
    say("done\n");
   }

  say("SUCCESS: All %d tests passed successfully in %lu microseconds\n", n,     // Run time
     (long)(((double) (clock() - start)) / CLOCKS_PER_SEC * 1000000.0));
  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/arenaTree/arenaTree
// valgrind --tool=cachegrind  /home/phil/c/z/arenaTree/arenaTree
