#line 1 "/home/phil/c/z/readOnlyBytes/readOnlyBytes.c"
//------------------------------------------------------------------------------
// Read Only Bytes
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef ReadOnlyBytes_included
#define ReadOnlyBytes_included
#include <fileName.c>
#include <utilities.c>

//D1 Read Only Bytes                                                            // Extract information from a buffer containing a read only sequence of bytes

typedef struct ReadOnlyBytes                                                                //s Description of a read only sequence of bytes
 {const size_t length;                                                          // Length of the byte sequence
  const char  *data;                                                            // Address of the first byte in the read only sequence of bytes
  const enum   ReadOnlyBytes_allocator                                                      // Allocation of memory
   {ReadOnlyBytes_allocator_none   = 0,                                                     // Stack
    ReadOnlyBytes_allocator_malloc = 1,                                                     // malloc
    ReadOnlyBytes_allocator_mmap   = 2} allocator;                                          // mmap
  const struct ProtoTypes_ReadOnlyBytes *proto;                                             // Prototypes for methods
 } ReadOnlyBytes;

#include <readOnlyBytes_prototypes.h>

//D1 Constructors                                                               // Construct a description of a read only byte sequence

ReadOnlyBytes newReadOnlyBytes                                                                          //CP Create a new description of a read only sequence of bytes
 (const char  *data,
  const size_t length,                                                          // Length of the byte sequence
  const enum   ReadOnlyBytes_allocator allocator)                                           // Allocation of memory so we know how to free it (or not to free it)
 {ReadOnlyBytes r = {length, data, allocator, &ProtoTypes_ReadOnlyBytes};
  return r;
 }

ReadOnlyBytes newReadOnlyBytesFromString                                                                //C Create a new description of a read only sequence of bytes read from a specified string of specified length.
 (const char * string,                                                          // String
  const size_t length)                                                          // Length of string (no need to include any zero terminating byte)
 {return newReadOnlyBytes(strndup(string, length), length, ReadOnlyBytes_allocator_malloc);
 }

ReadOnlyBytes newReadOnlyBytesFromAllocatedString                                                       //C Create a new description of a read only sequence of bytes read from a specified string of specified length that has already been allocated via malloc.
 (const char * string,                                                          // String
  const size_t length)                                                          // Length of string (no need to include any zero terminating byte) or zero if a zero terminated string
 {const size_t l = length ? : (size_t)strlen(string);
  return newReadOnlyBytes(string, l, ReadOnlyBytes_allocator_none);
 }

ReadOnlyBytes newReadOnlyBytesFromFormat                                                                //C Create a new description of a read only sequence of bytes read from a formatted string
 (const char * format, ...)                                                     // Format followed by strings
 {va_list va;
  va_start(va, format);
  char *data; const int length = vasprintf(&data, format, va);                  // Allocate and format output string
  va_end(va);

  return newReadOnlyBytes(data, length, ReadOnlyBytes_allocator_malloc);                    // Successful allocation
 }

ReadOnlyBytes newReadOnlyBytesFromFile                                                                  //C Create a new description of a read only sequence of bytes read from a file
 (FileName file)                                                                // File to read
 {struct stat  s;
  char * const f = file.name;
  const  int   d = open(f, O_RDONLY);
  if (d < 0 || fstat(d, &s) < 0)                                                // Unsuccessful allocation
   {printStackBackTrace("Unable to open file: %s because: %m\n",  f);
   }
  const size_t l = s.st_size;                                                   // Size of file
  char * const data = mmap(NULL, l + 1, PROT_READ, MAP_PRIVATE, d, 0);          // Add space for a terminating zero byte - mmap sets unused bytes beyond the file to zero giving us a zero terminated string.
  if (!data)                                                                    // Unsuccessful mapping
   {printStackBackTrace("Unable to map file: %s because: %m\n",  f);
   }
  close(d);                                                                     // Close the file: the map is private so we no long need the underlying file

  return newReadOnlyBytes(data, l, ReadOnlyBytes_allocator_mmap);                                       // Read only bytes description of a mapped file
 }

static void free_ReadOnlyBytes                                                              //D Free any resources associated with a read only byte sequence
 (const ReadOnlyBytes r)                                                                    // Description of a read only sequence of bytes
 {switch(r.allocator)
   {case ReadOnlyBytes_allocator_none:                             break;
    case ReadOnlyBytes_allocator_malloc: free  ((void *)(r.data)); break;
    case ReadOnlyBytes_allocator_mmap:   munmap((void *)(r.data), r.length + 1); break;     // Include the trailing zero used to delimit the file content.
   }
 }

//D1 Methods                                                                    // Methods for a read only byte sequence

static size_t length_ReadOnlyBytes                                                          // Length of the sequence
 (const ReadOnlyBytes r)                                                                    // Description of a sequence of read only bytes
 {return r.length;
 }

static char *data_ReadOnlyBytes                                                             // Data field of the sequence
 (const ReadOnlyBytes r)                                                                    // Description of a sequence of read only bytes
 {return (char *)(r.data);
 }

static ReadOnlyBytes substring_string_ReadOnlyBytes_sizet_sizet                                         // Describe a sub-string of the specified sequence
 (const ReadOnlyBytes      r,                                                               // Description of a sequence of read only bytes
  const size_t start,                                                           // Start position of the sub-string
  const size_t length)                                                          // Length of the sub-string
 {return newReadOnlyBytes(r.data+start, length, ReadOnlyBytes_allocator_none);                          // Create a new description of a read only sequence of bytes
 }

static void writeFile_ReadOnlyBytes_string                                                  // Write a read only byte sequence to the specified file.
 (const ReadOnlyBytes        r,                                                             // Description of a read only sequence of bytes
  const FileName fileName)                                                      // Name of the file
 {fileName.proto->writeFile(fileName, r.data, r.length);                                       // Write content
 }

static int writeOpenFile_ReadOnlyBytes_string                                               // Write a read only byte sequence to the specified file descriptor.
 (const ReadOnlyBytes r,                                                                    // Description of a read only sequence of bytes
  FILE   *f)                                                                    // File descriptor
 {return write(fileno(f), r.data, r.length);                                    // Write content
 }

static int equals_ReadOnlyBytes_ReadOnlyBytes                                                           // Compare two read only byte sequences
 (const ReadOnlyBytes p,                                                                    // Description of first read only sequence of bytes
  const ReadOnlyBytes q)                                                                    // Description second read only sequence of bytes
 {return p.length == q.length && strncmp(p.data, q.data, p.length) == 0;
 }

static int equalsString_ReadOnlyBytes_zeroString                                            // Compare a read only byte sequence with a zero terminated string
 (const ReadOnlyBytes     r,                                                                // Description of read only sequence of bytes
  const char *s)                                                                // Zero terminated string
 {return r.length == strlen(s) && strncmp(r.data, s, r.length) == 0;
 }

static size_t b2SumW8_ReadOnlyBytes                                                         // Get a BLAKE2 digest for a file represented as two hex digits.
 (const ReadOnlyBytes r)                                                                    // Description of read only sequence of bytes
 {const FileName i = newFileNameTemporaryWithContent("i.txt", 0, 0);
  r.proto->writeFile(r, i);
  return i.proto->b2SumW8(i);                                                           // The digest is in the first two bytes
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0
#define treeIs(t, s) {string p = t->proto->print(t); assert(strcmp(p,s) == 0); free(p);} // Check the content of a tree

void test0()                                                                    //Tlength //Tdata //Tsubstring //TnewReadOnlyBytesFromFormat //TerrNo
 {char *s = "0123456789";                                                       // Sample sequence of read only bytes
  ReadOnlyBytes r = newReadOnlyBytesFromFormat(s);                                                      // New descriptor

  assert(r.proto->length(r) == strlen(s));
  ReadOnlyBytes R = r.proto->substring(r, 2, 2);
  assert(strncmp(R.proto->data(R), "23", R.length) == 0);

  r.proto->free(r); R.proto->free(R);
 }

void test1()                                                                    //TwriteFile //Tequals //TequalsString //Tfree //TnewReadOnlyBytesFromFile
 {char *s = "0123456789";                                                       // Sample data
  const FileName f = newFileNameTemporaryWithContent
  ("readOnlyBytes.data", s, 0);                                                 // Temporary file


  ReadOnlyBytes q = newReadOnlyBytesFromFormat("%s", s);                                                // New descriptor
    q.proto->writeFile(q, f);

  ReadOnlyBytes r = newReadOnlyBytesFromFile(f);                                                        // New descriptor
  assert(q.proto->equals(q, r));
  assert(q.proto->equalsString(q, s));
    q.proto->free(q); r.proto->free(r);
  f.proto->unlink(f);
  assert(!f.proto->size(f));
 }

void test2()                                                                    //TnewReadOnlyBytesFromFormat
 {char *s = "abcdefg";
  ReadOnlyBytes     q = newReadOnlyBytesFromFormat("%s %d %s", s, strlen(s), s);

  assert(q.proto->length(q) == 17);
  assert(q.proto->equalsString(q, "abcdefg 7 abcdefg"));

  q.proto->free(q);
 }

void test3()                                                                    //TwriteOpenFile
 {char    *s = "abcdefg";
  ReadOnlyBytes        q = newReadOnlyBytesFromFormat("%s %d %s", s, strlen(s), s);
  FileName f = newFileNameTemporaryWithContent("ReadOnlyBytes.data", 0, 0);
  FILE    *o = fopen(f.name, "w");
           q.proto->writeOpenFile(q, o);
  fclose  (o);
  assert(f.proto->size(f) == 17);
         q.proto->free(q);
 }

void test4()                                                                    //TnewReadOnlyBytesFromString
 {char *s = "abcdefg";
  ReadOnlyBytes     q = newReadOnlyBytesFromString(s, strlen(s));
  assert(strncmp(q.data, s, q.length) == 0 && q.length == strlen(s));
        q.proto->free(q);
 }

void test5()                                                                    //TnewReadOnlyBytesFromAllocatedString
 {char *s = strdup("abcdefg");
  ReadOnlyBytes     q = newReadOnlyBytesFromAllocatedString(s, strlen(s));
  assert(strncmp(q.data, s, q.length) == 0 && q.length == strlen(s));
        q.proto->free(q);
  free (s);
 }

void test6()                                                                    //Tb2SumW8
 {char  *s = strdup("aaaa\n");
  ReadOnlyBytes      q = newReadOnlyBytesFromString(s, strlen(s));
  assert(q.proto->b2SumW8(q) == 0x83);
         q.proto->free(q);
  free  (s);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4, test5,
                           test6, 0};
  run_tests("ReadOnlyBytes", 1, tests);
  return 0;
 }

#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/readOnlyBytes/readOnlyBytes
