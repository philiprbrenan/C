//------------------------------------------------------------------------------
// Read Only Bytes
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef $_included
#define $_included
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fileName.c>
#include <utilities.c>

//D1 Read Only Bytes                                                            // Extract information from a buffer containing a read only sequence of bytes

typedef struct $                                                                //s Description of a read only sequence of bytes
 {const size_t length;                                                          // Length of the byte sequence
  const char  *data;                                                            // Address of the first byte in the read only sequence of bytes
  const enum   $_allocator                                                      // Allocation of memory
   {$_allocator_none   = 0,                                                     // Stack
    $_allocator_malloc = 1,                                                     // malloc
    $_allocator_mmap   = 2} allocator;                                          // mmap
  const struct ProtoTypes_$ *proto;                                             // Prototypes for methods
 } $;

#include <$$_prototypes.h>

//D1 Constructors                                                               // Construct a description of a read only byte sequence

$ new$                                                                          //CP Create a new description of a read only sequence of bytes
 (const char  *data,
  const size_t length,                                                          // Length of the byte sequence
  const enum   $_allocator allocator)                                           // Allocation of memory so we know how to free it (or not to free it)
 {$ r = {length, data, allocator, &ProtoTypes_$};
  return r;
 }

$ new$FromString                                                                //C Create a new description of a read only sequence of bytes read from a specified string of specified length.
 (const char * string,                                                          // String
  const size_t length)                                                          // Length of string (no need to include any zero terminating byte)
 {return new$(strndup(string, length), length, $_allocator_malloc);
 }

$ new$FromAllocatedString                                                       //C Create a new description of a read only sequence of bytes read from a specified string of specified length that has already been allocated via malloc.
 (const char * string,                                                          // String
  const size_t length)                                                          // Length of string (no need to include any zero terminating byte) or zero if a zero terminated string
 {const size_t l = length ? : (size_t)strlen(string);
  return new$(string, l, $_allocator_none);
 }

$ new$FromFormat                                                                //C Create a new description of a read only sequence of bytes read from a formatted string
 (const char * format, ...)                                                     // Format followed by strings
 {va_list va;
  va_start(va, format);
  char *data; const int length = vasprintf(&data, format, va);                  // Allocate and format output string
  va_end(va);

  return new$(data, length, ReadOnlyBytes_allocator_malloc);                    // Successful allocation
 }

$ new$FromFile                                                                  //C Create a new description of a read only sequence of bytes read from a file
 (FileName file)                                                                // File to read
 {struct stat  s;
  char * const f = file.name;
  const  int   d = open(f, O_RDONLY);
  if (d < 0 || fstat(d, &s) < 0)                                                // Unsuccessful allocation
   {printStackBackTrace("Unable to open file %s because: %m\n",  f);
   }
  const size_t l = s.st_size;                                                   // Size of file
  char * const data = mmap(NULL, l + 1, PROT_READ, MAP_PRIVATE, d, 0);          // Add space for a terminating zero byte - mmap sets unused bytes beyond the file to zero giving us a zero terminated string.
  if (!data)                                                                    // Unsuccessful mapping
   {printStackBackTrace("Unable to map file %s because: %m\n",  f);
   }
  close(d);                                                                     // Close the file: the map is private so we no long need the underlying file

  return new$(data, l, $_allocator_mmap);                                       // Read only bytes description of a mapped file
 }

static void free_$                                                              //D Free any resources associated with a read only byte sequence
 (const $ r)                                                                    // Description of a read only sequence of bytes
 {switch(r.allocator)
   {case $_allocator_none:                             break;
    case $_allocator_malloc: free  ((void *)(r.data)); break;
    case $_allocator_mmap:   munmap((void *)(r.data), r.length + 1); break;     // Include the trailing zero used to delimit the file content.
   }
 }

//D1 Methods                                                                    // Methods for a read only byte sequence

static size_t length_$                                                          // Length of the sequence
 (const $ r)                                                                    // Description of a sequence of read only bytes
 {return r.length;
 }

static char *data_$                                                             // Data field of the sequence
 (const $ r)                                                                    // Description of a sequence of read only bytes
 {return (char *)(r.data);
 }

static $ substring_string_$_sizet_sizet                                         // Describe a sub-string of the specified sequence
 (const $      r,                                                               // Description of a sequence of read only bytes
  const size_t start,                                                           // Start position of the sub-string
  const size_t length)                                                          // Length of the sub-string
 {return new$(r.data+start, length, $_allocator_none);                          // Create a new description of a read only sequence of bytes
 }

static void writeFile_$_string                                                  // Write a read only byte sequence to the specified file.
 (const $        r,                                                             // Description of a read only sequence of bytes
  const FileName fileName)                                                      // Name of the file
 {fileName ▷ writeFile(r.data, r.length);                                       // Write content
 }

static int writeOpenFile_$_string                                               // Write a read only byte sequence to the specified file descriptor.
 (const $ r,                                                                    // Description of a read only sequence of bytes
  FILE   *f)                                                                    // File descriptor
 {return write(fileno(f), r.data, r.length);                                    // Write content
 }

static int equals_$_$                                                           // Compare two read only byte sequences
 (const $ p,                                                                    // Description of first read only sequence of bytes
  const $ q)                                                                    // Description second read only sequence of bytes
 {return p.length == q.length && strncmp(p.data, q.data, p.length) == 0;
 }

static int equalsString_$_zeroString                                            // Compare a read only byte sequence with a zero terminated string
 (const $     r,                                                                // Description of read only sequence of bytes
  const char *s)                                                                // Zero terminated string
 {return r.length == strlen(s) && strncmp(r.data, s, r.length) == 0;
 }

static size_t b2SumW8_$                                                         // Get a BLAKE2 digest for a file represented as two hex digits.
 (const $ r)                                                                    // Description of read only sequence of bytes
 {const FileName i = newFileNameTemporaryWithContent("i.txt", 0, 0);
  r ▷ writeFile(i);
  return i ▷ b2SumW8;                                                           // The digest is in the first two bytes
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0
#define treeIs(t, s) {string p = t ▶ print; assert(strcmp(p,s) == 0); free(p);} // Check the content of a tree

void test0()                                                                    //Tlength //Tdata //Tsubstring //TnewReadOnlyBytesFromFormat //TerrNo
 {char *s = "0123456789";                                                       // Sample sequence of read only bytes
  $ r = new$FromFormat(s);                                                      // New descriptor

  assert(r ▷ length == strlen(s));
  $ R = r ▷ substring(2, 2);
  assert(strncmp(R ▷ data, "23", R.length) == 0);

  r ▷ free; R ▷ free;
 }

void test1()                                                                    //TwriteFile //Tequals //TequalsString //Tfree //TnewReadOnlyBytesFromFile
 {const FileName f = newFileName("zzzReadOnlyBytes.data");                      // Temporary file
  char          *s = "0123456789";                                              // Sample data

  $ q = new$FromFormat("%s", s);                                                // New descriptor
    q ▷ writeFile(f);

  $ r = new$FromFile(f);                                                        // New descriptor
  assert(q ▷ equals(r));
  assert(q ▷ equalsString(s));
    q ▷ free; r ▷ free;
  f ▷ unlink;
  assert(!f ▷ size);
 }

void test2()                                                                    //TnewReadOnlyBytesFromFormat
 {char *s = "abcdefg";
  $     q = new$FromFormat("%s %d %s", s, strlen(s), s);

  assert(q ▷ length == 17);
  assert(q ▷ equalsString("abcdefg 7 abcdefg"));

  q ▷ free;
 }

void test3()                                                                    //TwriteOpenFile
 {char    *s = "abcdefg";
  $        q = new$FromFormat("%s %d %s", s, strlen(s), s);
  FileName f = newFileNameTemporaryWithContent("$.data", 0, 0);
  FILE    *o = fopen(f.name, "w");
           q ▷ writeOpenFile(o);
  fclose  (o);
  assert(f ▷ size == 17);
         q ▷ free;
 }

void test4()                                                                    //TnewReadOnlyBytesFromString
 {char *s = "abcdefg";
  $     q = new$FromString(s, strlen(s));
  assert(strncmp(q.data, s, q.length) == 0 && q.length == strlen(s));
        q ▷ free;
 }

void test5()                                                                    //TnewReadOnlyBytesFromAllocatedString
 {char *s = strdup("abcdefg");
  $     q = new$FromAllocatedString(s, strlen(s));
  assert(strncmp(q.data, s, q.length) == 0 && q.length == strlen(s));
        q ▷ free;
  free (s);
 }

void test6()                                                                    //Tb2SumW8
 {char  *s = strdup("aaaa\n");
  $      q = new$FromString(s, strlen(s));
  assert(q ▷ b2SumW8 == 0x83);
         q ▷ free;
  free  (s);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4, test5,
                           test6, 0};
  run_tests("$", 1, tests);
  return 0;
 }

#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/readOnlyBytes/readOnlyBytes
