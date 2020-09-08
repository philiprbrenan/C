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
 {char  *data;                                                                  // Address of the first byte in the read only sequence of bytes
  size_t length;                                                                // Length of the byte sequence
  enum   ReadOnlyBytesAllocator                                                             // Allocation of memory
   {ReadOnlyBytesAllocator_none    = 0,                                                     // Stack
    ReadOnlyBytesAllocator_malloc  = 1,                                                     // malloc
    ReadOnlyBytesAllocator_malloc1 = 2,                                                     // malloc with extra string terminating zero
    ReadOnlyBytesAllocator_mmap    = 3} allocator;                                          // mmap
  const struct ProtoTypes_ReadOnlyBytes *proto;                                             // Prototypes for methods
 } ReadOnlyBytes;

#include <readOnlyBytes_prototypes.h>

//D1 Constructors                                                               // Construct a description of a read only byte sequence

static ReadOnlyBytes makeReadOnlyBytes                                                                  //CP Create a new description of a read only sequence of bytes
 (char  * const data,
  const size_t length,                                                          // Length of the byte sequence
  const enum ReadOnlyBytesAllocator allocator)                                              // Allocation of memory so we know how to free it (or not to free it)
 {const ReadOnlyBytes r = {data, length, allocator, &ProtoTypes_ReadOnlyBytes};
  return r;
 }

static ReadOnlyBytes makeReadOnlyBytesFromString                                                        //C Create a new description of a read only sequence of bytes read from a specified string.
 (char * const string)                                                          // Zero terminated string
 {return makeReadOnlyBytes(string, strlen(string), ReadOnlyBytesAllocator_none);
 }

static ReadOnlyBytes makeReadOnlyBytesFromStringN                                                       //C Create a new description of a read only sequence of bytes read from a specified string of specified length.
 (char * const string,                                                          // String
  const size_t length)                                                          // Length of string, apply strlen if zero
 {const size_t l = length ? : strlen(string);
  return makeReadOnlyBytes(string, l, ReadOnlyBytesAllocator_none);
 }

static ReadOnlyBytes makeReadOnlyBytesDup                                                               //C Create a new description of a read only sequence of bytes read from a specified string by duplicating it.
 (char * const string)                                                          // Zero terminated string
 {const size_t l = strlen(string);
  char * const s = alloc(l + 1); s[l] = 0;
  return makeReadOnlyBytes(strncpy(s, string, l), l, ReadOnlyBytesAllocator_malloc1);
 }

static ReadOnlyBytes makeReadOnlyBytesDupN                                                               //C Create a new description of a read only sequence of bytes read from a specified string of specified length by duplicating it.
 (char * const string,                                                          // String
  const size_t length)                                                          // Length of string (no need to include any zero terminating byte)
 {const size_t l = length ? : strlen(string);
  char * const s = alloc(l + 1); s[l] = 0;
  return makeReadOnlyBytes(strncpy(s, string, l), l, ReadOnlyBytesAllocator_malloc1);
 }

static ReadOnlyBytes makeReadOnlyBytesFromFormat                                                        //C Create a new description of a read only sequence of bytes read from a formatted string
 (const char * format, ...)                                                     // Format followed by strings
 {va_list va;
  va_start(va, format);
  char *data; const int length = vasprintf(&data, format, va);                  // Allocate and format output string
  va_end(va);

  return makeReadOnlyBytes(data, length, ReadOnlyBytesAllocator_malloc);                                // Successful allocation
 }

static ReadOnlyBytes makeReadOnlyBytesBuffer                                                            //C Create an empty read only sequence of bytes of the specified length which can be written into to load it.
 (const size_t length)                                                          // Length of string (no need to include any zero terminating byte)
 {char * const s = alloc(length);
  memset(s, 0, length);
  return makeReadOnlyBytes(s, length, ReadOnlyBytesAllocator_malloc);
 }

static ReadOnlyBytes makeReadOnlyBytesFromFile                                                          //C Create a new description of a read only sequence of bytes read from a file.
 (FileName file)                                                                // File to read
 {struct stat  s;
  const char * const f = file.name;
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

  return makeReadOnlyBytes(data, l, ReadOnlyBytesAllocator_mmap);                                       // Read only bytes description of a mapped file
 }

static void free_ReadOnlyBytes                                                              //D Free any resources associated with a read only byte sequence
 (const ReadOnlyBytes r)                                                                    // Description of a read only sequence of bytes
 {switch(r.allocator)
   {case ReadOnlyBytesAllocator_none:                                            break;
    case ReadOnlyBytesAllocator_malloc:
    case ReadOnlyBytesAllocator_malloc1: free  ((void *)(r.data));               break;
    case ReadOnlyBytesAllocator_mmap:    munmap((void *)(r.data), r.length + 1); break;     // Include the trailing zero used to delimit the file content.
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
 {return makeReadOnlyBytes(r.data+start, length, ReadOnlyBytesAllocator_none);                          // Create a new description of a read only sequence of bytes
 }

static FileName writeTemporaryFile_ReadOnlyBytes_string                                     // Write a read only byte sequence to a temporary file with the specified base name.
 (const ReadOnlyBytes      r,                                                               // Description of a read only sequence of bytes
  const char * fileName)                                                        // Base name of the file
 {return makeFileNameTemporaryWithContent(fileName, r.data, r.length);
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

static int containsString_ReadOnlyBytes_zeroString                                          // Check that a read only byte sequence contains the specified zero terminated string
 (const ReadOnlyBytes     r,                                                                // Description of read only sequence of bytes
  const char * const s)                                                                // Zero terminated string
 {return strstr(r.data, s) != NULL;
 }

static size_t b2SumW8_ReadOnlyBytes                                                         // Get a BLAKE2 digest for a file represented as two hex digits.
 (const ReadOnlyBytes r)                                                                    // Description of read only sequence of bytes
 {const FileName i = makeFileNameTemporaryWithContent("i.txt", r.data,r.length);
  return i.proto->b2SumW8(i);                                                           // The digest is in the first two bytes
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0
#define treeIs(t, s) {string p = t->proto->print(t); assert(strcmp(p,s) == 0); free(p);} // Check the content of a tree

void test0()                                                                    //Tlength //Tdata //Tsubstring //TnewReadOnlyBytesFromFormat //TerrNo
 {char *s = "0123456789";                                                       // Sample sequence of read only bytes
  ReadOnlyBytes r = makeReadOnlyBytesFromFormat(s);                                                     // New descriptor

  assert(r.proto->length(r) == strlen(s));
  ReadOnlyBytes R = r.proto->substring(r, 2, 2);
  assert(strncmp(R.proto->data(R), "23", R.length) == 0);

  r.proto->free(r); R.proto->free(R);
 }

void test1()                                                                    //TwriteFile //Tequals //TequalsString //Tfree //TnewReadOnlyBytesFromFile
 {char *s = "0123456789";                                                       // Sample data
  const FileName f = makeFileNameTemporaryWithContent
  ("readOnlyBytes.data", s, 0);                                                 // Temporary file


  ReadOnlyBytes q = makeReadOnlyBytesFromFormat("%s", s);                                                // New descriptor
    q.proto->writeFile(q, f);

  ReadOnlyBytes r = makeReadOnlyBytesFromFile(f);                                                        // New descriptor
  assert(q.proto->equals(q, r));
  assert(q.proto->equalsString(q, s));
    q.proto->free(q); r.proto->free(r);
  f.proto->unlink(f);
  assert(!f.proto->size(f));
 }

void test2()                                                                    //TnewReadOnlyBytesFromFormat
 {char *s = "abcdefg";
  ReadOnlyBytes     q = makeReadOnlyBytesFromFormat("%s %d %s", s, strlen(s), s);

  assert(q.proto->length(q) == 17);
  assert(q.proto->equalsString(q, "abcdefg 7 abcdefg"));

  q.proto->free(q);
 }

void test3()                                                                    //TwriteOpenFile
 {char    *s = "abcdefg";
  ReadOnlyBytes        q = makeReadOnlyBytesFromFormat("%s %d %s", s, strlen(s), s);
  FileName f = makeFileNameTemporaryWithContent("ReadOnlyBytes.data", 0, 0);
  FILE    *o = fopen(f.name, "w");
           q.proto->writeOpenFile(q, o);
  fclose  (o);
  assert(f.proto->size(f) == 17);
         q.proto->free(q);
 }

void test4()                                                                    //TnewReadOnlyBytesFromString
 {char *s = "abcdefg";
  ReadOnlyBytes     q = makeReadOnlyBytesFromString(s);
  assert(strncmp(q.data, s, q.length) == 0 && q.length == strlen(s));
        q.proto->free(q);
 }

void test5()                                                                    //Tb2SumW8
 {char  *s = strdup("aaaa\n");
  ReadOnlyBytes      q = makeReadOnlyBytesFromString(s);
  assert(q.proto->b2SumW8(q) == 0x83);
         q.proto->free(q);
  free  (s);
 }

void test6()                                                                    //TcontainsString
 {ReadOnlyBytes s = makeReadOnlyBytesFromFormat("abcd");
  assert( s.proto->containsString(s, "bc"));
  assert(!s.proto->containsString(s, "cb"));
  s.proto->free(s);
 }

void test7()                                                                    //TmakeReadOnlyBytesDup //TmakeReadOnlyBytesDupN //TmakeReadOnlyBytesFromString //TmakeReadOnlyBytesFromStringN //TmakeReadOnlyBytesFromFormat //TmakeReadOnlyBytesBuffer
 {ReadOnlyBytes d = makeReadOnlyBytesDup       ("aaa"), D = makeReadOnlyBytesDupN       ("aaaa", 3),
    s = makeReadOnlyBytesFromString("aaa"), S = makeReadOnlyBytesFromStringN("aaaa", 3),
    f = makeReadOnlyBytesFromFormat("%s", "aaa"), m = makeReadOnlyBytesBuffer(3);
    memcpy(m.data, "aaaa", 3);
  assert(d.proto->equals(d, D)); assert(d.proto->length(d) == 3); assert(D.proto->length(D) == 3);
  assert(d.proto->equals(d, s)); assert(s.proto->length(s) == 3);
  assert(d.proto->equals(d, S)); assert(S.proto->length(S) == 3);
  assert(d.proto->equals(d, f)); assert(f.proto->length(f) == 3);

  d.proto->free(d); D.proto->free(D); s.proto->free(s); S.proto->free(S); f.proto->free(f); m.proto->free(m);
 }

void test8()                                                                    //TmakeReadOnlyBytesFromFile //TwriteReadOnlyBytesTemporaryFile
 {ReadOnlyBytes s = makeReadOnlyBytesFromStringN("aaa", 2);
  FileName f = s.proto->writeTemporaryFile(s, "test.data");
  ReadOnlyBytes t = makeReadOnlyBytesFromFile(f);
  assert(t.proto->equals(t, s)); assert(t.proto->equalsString(t, "aa"));
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4, test5,
                           test6, test7, test8, 0};
  run_tests("ReadOnlyBytes", 1, tests);
  return 0;
 }

#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/readOnlyBytes/readOnlyBytes
