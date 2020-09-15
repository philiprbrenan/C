#line 1 "/home/phil/c/z/readOnlyBytes/readOnlyBytes.c"
//------------------------------------------------------------------------------
// Read Only Bytes
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef ReadOnlyBytes_included
#define ReadOnlyBytes_included
#include <ctype.h>
#include <fileName.c>
#include <utilities.c>

//D1 Read Only Bytes                                                            // Extract information from a buffer containing a read only sequence of bytes
typedef struct ArenaTree ArenaTree;

typedef struct ReadOnlyBytes                                                                //s Description of a read only sequence of bytes
 {const char * const data;                                                                  // Address of the first byte in the read only sequence of bytes
  const size_t length;                                                                // Length of the byte sequence
  const enum   ReadOnlyBytesError                                                                 // Description of any error that might have occurred during the creation of the ReadOnlyBytes from a file. The error message also appears in printed form in the ReadOnlyBytes data area.
   {ReadOnlyBytesError_none = 0,                                                            // No error occured
    ReadOnlyBytesError_no_such_file,                                                        // File does not exist
    ReadOnlyBytesError_unable_to_map                                                        // Cannot map file
   } error;                                                                     // Non zeroif an error occured. The text of the error wil also show up in the ReadOnlyBytes data .
  const enum   ReadOnlyBytesAllocator                                                             // Allocation of memory
   {ReadOnlyBytesAllocator_none    = 0,                                                     // Stack
    ReadOnlyBytesAllocator_malloc  = 1,                                                     // malloc
    ReadOnlyBytesAllocator_malloc1 = 2,                                                     // malloc with extra string terminating zero
    ReadOnlyBytesAllocator_mmap    = 3                                                      // mmap
   } allocator;
  const struct ProtoTypes_ReadOnlyBytes *proto;                                             // Prototypes for methods
 } ReadOnlyBytes;

#include <readOnlyBytes_prototypes.h>
#include <arenaTree.c>                                                          // Now we know what a ReadOnlyBytes looks like

//D1 Constructors                                                               // Construct a description of a ReadOnlyBytes that has been created without errors.

static ReadOnlyBytes makeReadOnlyBytes                                                                  //CP Create a new description of a ReadOnlyBytes
 (const char * const    data,                                                   // Memory to contain the read only bytes string
  size_t                length,                                                 // Length of the byte sequence
  const enum ReadOnlyBytesAllocator allocator)                                              // Allocation of memory so we know how to free it (or not to free it)
 {const ReadOnlyBytes r = {data, length, ReadOnlyBytesError_none, allocator, &ProtoTypes_ReadOnlyBytes};
  return r;
 }

static ReadOnlyBytes makeReadOnlyBytesError                                                             //CP Reason why a ReadOnlyBytes could not be created from a file.
 (const enum ReadOnlyBytesError  error,                                                     // Error status
  const char * const format,                                                    // Format
  ...)                                                                          // Strings
 {va_list va;
  va_start(va, format);
  char * data; const int length = vasprintf(&data, format, va);                 // Allocate and format output string
  va_end(va);

  const ReadOnlyBytes r = {data, length, error, ReadOnlyBytesAllocator_malloc1, &ProtoTypes_ReadOnlyBytes};
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
 (const char * const string)                                                    // Zero terminated string
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
 (const char * const format,                                                    // Format
  ...)                                                                          // Strings
 {va_list va;
  va_start(va, format);
  char * data; const int length = vasprintf(&data, format, va);                 // Allocate and format output string
  va_end(va);

  return makeReadOnlyBytes(data, length, ReadOnlyBytesAllocator_malloc);                                // Successful allocation
 }

//static ReadOnlyBytes makeReadOnlyBytesBuffer                                                            //C Create an empty read only sequence of bytes of the specified length which can be written into to load it.
// (const size_t length)                                                          // Length of string (no need to include any zero terminating byte)
// {char * const s = alloc(length);
//  memset(s, 0, length);
//  return makeReadOnlyBytes(s, length, ReadOnlyBytesAllocator_malloc);
// }

static ReadOnlyBytes makeReadOnlyBytesFromFile                                                          //C Create a new description of a read only sequence of bytes read from a file.
 (const FileName file)                                                          // File to read
 {struct stat  s;
  const char * const f = file.name;
  const  int   d = open(f, O_RDONLY);

  if (d < 0 || fstat(d, &s) < 0) return makeReadOnlyBytesError(ReadOnlyBytesError_no_such_file,         // Unsuccessful allocation
     "Unable to open file: %s because: %m\n",  f);

  const size_t l = s.st_size;                                                   // Size of file
  char * const data = mmap(NULL, l + 1, PROT_READ, MAP_PRIVATE, d, 0);          // Add space for a terminating zero byte - mmap sets unused bytes beyond the file to zero giving us a zero terminated string.

  if (!data) return makeReadOnlyBytesError(ReadOnlyBytesError_unable_to_map,                            // Unsuccessful mapping
    "Unable to map file: %s because: %m\n",  f);

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
 (const ReadOnlyBytes            r,                                                         // Description of a read only sequence of bytes
  const char * const fileName)                                                  // Base name of the file
 {return makeFileNameTemporaryWithContent(fileName, r.data, r.length);
 }

static void writeFile_ReadOnlyBytes_string                                                  // Write a read only byte sequence to the specified file.
 (const ReadOnlyBytes        r,                                                             // Description of a read only sequence of bytes
  const FileName fileName)                                                      // Name of the file
 {fileName.proto->writeFile(fileName, r.data, r.length);                                       // Write content
 }

static int writeOpenFile_ReadOnlyBytes_string                                               // Write a read only byte sequence to the specified file descriptor.
 (const  ReadOnlyBytes       r,                                                             // Description of a read only sequence of bytes
  FILE   * const f)                                                             // File descriptor
 {return write(fileno(f), r.data, r.length);                                    // Write content
 }

static int equals_ReadOnlyBytes_ReadOnlyBytes                                                           // Compare two read only byte sequences
 (const ReadOnlyBytes p,                                                                    // Description of first read only sequence of bytes
  const ReadOnlyBytes q)                                                                    // Description second read only sequence of bytes
 {return p.length == q.length && strncmp(p.data, q.data, p.length) == 0;
 }

static int equalsString_ReadOnlyBytes_zeroString                                            // Compare a read only byte sequence with a zero terminated string
 (const ReadOnlyBytes            r,                                                         // Description of read only sequence of bytes
  const char * const s)                                                         // Zero terminated string
 {return r.length == strlen(s) && strncmp(r.data, s, r.length) == 0;
 }

static int containsString_ReadOnlyBytes_zeroString                                          // Check that a read only byte sequence contains the specified zero terminated string
 (const ReadOnlyBytes            r,                                                         // Description of read only sequence of bytes
  const char * const s)                                                         // Zero terminated string
 {return strstr(r.data, s) != NULL;
 }

static size_t b2SumW8_ReadOnlyBytes                                                         // Get a BLAKE2 digest for a file represented as two hex digits.
 (const ReadOnlyBytes r)                                                                    // Description of read only sequence of bytes
 {const FileName i = makeFileNameTemporaryWithContent("i.txt", r.data,r.length);
  return i.proto->b2SumW8(i);                                                           // The digest is in the first two bytes
 }

static ArenaTree splitNewLine                                                   // Split the specified ReadOnlyBytes on any new line characters
 (const ReadOnlyBytes r)                                                                    // Description of read only sequence of bytes
 {const ArenaTree t = makeArenaTree();                                          // Results are held in an arena tree
  size_t i = 0, j = 0;
  void save()                                                                   // Save a string
   {const ArenaTreeNode n = t.proto->noden(t, r.data+j, i - j + 1);
                            n.proto->putTreeLast(n);
    j = i+1;
   }
  for(; i < r.length; ++i) if (*(r.data + i) == '\n') save();
  if (  i > j) save();
  return t;
 }

static ArenaTree splitSpaces                                                    // Split the specified ReadOnlyBytes on any sequence of white space, removing the white space from the results
 (const ReadOnlyBytes r)                                                                    // Description of read only sequence of bytes
 {const ArenaTree t = makeArenaTree();                                          // Results are held in an arena tree
  size_t i = 0, j = 0;
  void save()                                                                   // Save a string
   {if (i > j)
     {const ArenaTreeNode n = t.proto->noden(t, r.data+j, i - j);
                          n.proto->putTreeLast(n);
     }
    j = i + 1;
   }
  for(; i < r.length; ++i) if (isspace(*(r.data + i))) save();
  if (  i > j) save();
  return t;
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

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
  const FileName f = makeFileNameTemporaryWithContent("readOnlyBytes.data",s,0);


  ReadOnlyBytes q = makeReadOnlyBytesFromFormat("%s", s);
    q.proto->writeFile(q, f);

  ReadOnlyBytes r = makeReadOnlyBytesFromFile    (f);
  assert(q.proto->equals(q, r));
  assert(q.proto->equalsString(q, s));
         q.proto->free(q);        r.proto->free(r);
  f.proto->unlink(f);     assert(!f.proto->size(f));
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
    f = makeReadOnlyBytesFromFormat("%s", "aaa");
  assert(d.proto->equals(d, D)); assert(d.proto->length(d) == 3); assert(D.proto->length(D) == 3);
  assert(d.proto->equals(d, s)); assert(s.proto->length(s) == 3);
  assert(d.proto->equals(d, S)); assert(S.proto->length(S) == 3);
  assert(d.proto->equals(d, f)); assert(f.proto->length(f) == 3);

  d.proto->free(d); D.proto->free(D); s.proto->free(s); S.proto->free(S); f.proto->free(f);
 }

void test8()                                                                    //TmakeReadOnlyBytesFromFile //TwriteTemporaryFile
 {ReadOnlyBytes s = makeReadOnlyBytesFromStringN("aaa", 2);
  FileName f = s.proto->writeTemporaryFile(s, "test.data");
  ReadOnlyBytes t = makeReadOnlyBytesFromFile(f);
  assert(t.proto->equals(t, s)); assert(t.proto->equalsString(t, "aa"));
 }

void test9()                                                                    //TsplitNewLine //TsplitSpaces
 {if (1)
   {ReadOnlyBytes l = makeReadOnlyBytesFromString("\na\nbb\nccc\ndddd");
    ArenaTree L = l.proto->splitNewLine(l);
    ArenaTreeNode l1 = L.proto->first(L), l2 = l1.proto->next(l1), l3 = l2.proto->next(l2),
                                  l4 = l3.proto->next(l3), l5 = l4.proto->next(l4);
    assert(l1.proto->equalsString(l1, "\n"));
    assert(l2.proto->equalsString(l2, "a\n"));
    assert(l3.proto->equalsString(l3, "bb\n"));
    assert(l4.proto->equalsString(l4, "ccc\n"));
    assert(l5.proto->equalsString(l5, "dddd"));

    l.proto->free(l); L.proto->free(L);
   }

  if (1)
   {ReadOnlyBytes l = makeReadOnlyBytesFromString("\n\n\n");
    ArenaTree L = l.proto->splitNewLine(l);
    ArenaTreeNode l1 = L.proto->first(L), l2 = l1.proto->next(l1), l3 = l2.proto->next(l2);
    assert(l1.proto->equalsString(l1, "\n"));
    assert(l2.proto->equalsString(l2, "\n"));
    assert(l3.proto->equalsString(l3, "\n"));

    l.proto->free(l); L.proto->free(L);
   }

  if (1)
   {ReadOnlyBytes l = makeReadOnlyBytesFromString(" ");
    ArenaTree L = l.proto->splitNewLine(l);
    ArenaTreeNode l1 = L.proto->first(L);
    assert(l1.proto->equalsString(l1, " "));

    l.proto->free(l); L.proto->free(L);
   }

  if (1)
   {ReadOnlyBytes l = makeReadOnlyBytesFromString("");
    ArenaTree L = l.proto->splitNewLine(l);
    ArenaTreeNode l1 = L.proto->first(L);
    assert(l1.proto->equalsString(l1, ""));

    l.proto->free(l); L.proto->free(L);
   }

  if (1)
   {ReadOnlyBytes s = makeReadOnlyBytesFromString(" \na bb   ccc dddd  ");
    ArenaTree S = s.proto->splitSpaces(s);
    ArenaTreeNode s1 = S.proto->first(S), s2 = s1.proto->next(s1), s3 = s2.proto->next(s2),
                                  s4 = s3.proto->next(s3);
    assert(s1.proto->equalsString(s1, "a"));
    assert(s2.proto->equalsString(s2, "bb"));
    assert(s3.proto->equalsString(s3, "ccc"));
    assert(s4.proto->equalsString(s4, "dddd"));

    s.proto->free(s); S.proto->free(S);
   }
 }

void test10()
 {ReadOnlyBytes f = makeReadOnlyBytesFromFile(makeFileName("/aaa/bbb"));
  assert(f.error == ReadOnlyBytesError_no_such_file);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4,  test5,
                           test6, test7, test8, test9, test10, 0};
// {void (*tests[])(void) = {test9, 0};
  run_tests("ReadOnlyBytes", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/readOnlyBytes/readOnlyBytes
