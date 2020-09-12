//------------------------------------------------------------------------------
// Read Only Bytes
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef $_included
#define $_included
#include <fileName.c>
#include <utilities.c>

//D1 Read Only Bytes                                                            // Extract information from a buffer containing a read only sequence of bytes

typedef struct $                                                                //s Description of a read only sequence of bytes
 {char * data;                                                                  // Address of the first byte in the read only sequence of bytes
  size_t length;                                                                // Length of the byte sequence
  enum   $Allocator                                                             // Allocation of memory
   {$Allocator_none    = 0,                                                     // Stack
    $Allocator_malloc  = 1,                                                     // malloc
    $Allocator_malloc1 = 2,                                                     // malloc with extra string terminating zero
    $Allocator_mmap    = 3} allocator;                                          // mmap
  const struct ProtoTypes_$ *proto;                                             // Prototypes for methods
 } $;

#include <$$_prototypes.h>

//D1 Constructors                                                               // Construct a description of a read only byte sequence

static $ make$                                                                  //CP Create a new description of a read only sequence of bytes
 (char  * const data,
  const size_t length,                                                          // Length of the byte sequence
  const enum $Allocator allocator)                                              // Allocation of memory so we know how to free it (or not to free it)
 {const $ r = {data, length, allocator, &ProtoTypes_$};
  return r;
 }

static $ make$FromString                                                        //C Create a new description of a read only sequence of bytes read from a specified string.
 (char * const string)                                                          // Zero terminated string
 {return make$(string, strlen(string), $Allocator_none);
 }

static $ make$FromStringN                                                       //C Create a new description of a read only sequence of bytes read from a specified string of specified length.
 (char * const string,                                                          // String
  const size_t length)                                                          // Length of string, apply strlen if zero
 {const size_t l = length ? : strlen(string);
  return make$(string, l, $Allocator_none);
 }

static $ make$Dup                                                               //C Create a new description of a read only sequence of bytes read from a specified string by duplicating it.
 (const char * const string)                                                    // Zero terminated string
 {const size_t l = strlen(string);
  char * const s = alloc(l + 1); s[l] = 0;
  return make$(strncpy(s, string, l), l, $Allocator_malloc1);
 }

static $ make$DupN                                                               //C Create a new description of a read only sequence of bytes read from a specified string of specified length by duplicating it.
 (char * const string,                                                          // String
  const size_t length)                                                          // Length of string (no need to include any zero terminating byte)
 {const size_t l = length ? : strlen(string);
  char * const s = alloc(l + 1); s[l] = 0;
  return make$(strncpy(s, string, l), l, $Allocator_malloc1);
 }

static $ make$FromFormat                                                        //C Create a new description of a read only sequence of bytes read from a formatted string
 (const char * const format,                                                    // Format
  ...)                                                                          // Strings
 {va_list va;
  va_start(va, format);
  char * data; const int length = vasprintf(&data, format, va);                 // Allocate and format output string
  va_end(va);

  return make$(data, length, $Allocator_malloc);                                // Successful allocation
 }

static $ make$Buffer                                                            //C Create an empty read only sequence of bytes of the specified length which can be written into to load it.
 (const size_t length)                                                          // Length of string (no need to include any zero terminating byte)
 {char * const s = alloc(length);
  memset(s, 0, length);
  return make$(s, length, $Allocator_malloc);
 }

static $ make$FromFile                                                          //C Create a new description of a read only sequence of bytes read from a file.
 (const FileName file)                                                          // File to read
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

  return make$(data, l, $Allocator_mmap);                                       // Read only bytes description of a mapped file
 }

static void free_$                                                              //D Free any resources associated with a read only byte sequence
 (const $ r)                                                                    // Description of a read only sequence of bytes
 {switch(r.allocator)
   {case $Allocator_none:                                            break;
    case $Allocator_malloc:
    case $Allocator_malloc1: free  ((void *)(r.data));               break;
    case $Allocator_mmap:    munmap((void *)(r.data), r.length + 1); break;     // Include the trailing zero used to delimit the file content.
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
 {return make$(r.data+start, length, $Allocator_none);                          // Create a new description of a read only sequence of bytes
 }

static FileName writeTemporaryFile_$_string                                     // Write a read only byte sequence to a temporary file with the specified base name.
 (const $            r,                                                         // Description of a read only sequence of bytes
  const char * const fileName)                                                  // Base name of the file
 {return makeFileNameTemporaryWithContent(fileName, r.data, r.length);
 }

static void writeFile_$_string                                                  // Write a read only byte sequence to the specified file.
 (const $        r,                                                             // Description of a read only sequence of bytes
  const FileName fileName)                                                      // Name of the file
 {fileName ▷ writeFile(r.data, r.length);                                       // Write content
 }

static int writeOpenFile_$_string                                               // Write a read only byte sequence to the specified file descriptor.
 (const  $       r,                                                             // Description of a read only sequence of bytes
  FILE   * const f)                                                             // File descriptor
 {return write(fileno(f), r.data, r.length);                                    // Write content
 }

static int equals_$_$                                                           // Compare two read only byte sequences
 (const $ p,                                                                    // Description of first read only sequence of bytes
  const $ q)                                                                    // Description second read only sequence of bytes
 {return p.length == q.length && strncmp(p.data, q.data, p.length) == 0;
 }

static int equalsString_$_zeroString                                            // Compare a read only byte sequence with a zero terminated string
 (const $            r,                                                         // Description of read only sequence of bytes
  const char * const s)                                                         // Zero terminated string
 {return r.length == strlen(s) && strncmp(r.data, s, r.length) == 0;
 }

static int containsString_$_zeroString                                          // Check that a read only byte sequence contains the specified zero terminated string
 (const $            r,                                                         // Description of read only sequence of bytes
  const char * const s)                                                         // Zero terminated string
 {return strstr(r.data, s) != NULL;
 }

static size_t b2SumW8_$                                                         // Get a BLAKE2 digest for a file represented as two hex digits.
 (const $ r)                                                                    // Description of read only sequence of bytes
 {const FileName i = makeFileNameTemporaryWithContent("i.txt", r.data,r.length);
  return i ▷ b2SumW8;                                                           // The digest is in the first two bytes
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0
#define treeIs(t, s) {string p = t ▶ print; assert(strcmp(p,s) == 0); free(p);} // Check the content of a tree

void test0()                                                                    //Tlength //Tdata //Tsubstring //TnewReadOnlyBytesFromFormat //TerrNo
 {char *s = "0123456789";                                                       // Sample sequence of read only bytes
  $ r = make$FromFormat(s);                                                     // New descriptor

  assert(r ▷ length == strlen(s));
  $ R = r ▷ substring(2, 2);
  assert(strncmp(R ▷ data, "23", R.length) == 0);

  r ▷ free; R ▷ free;
 }

void test1()                                                                    //TwriteFile //Tequals //TequalsString //Tfree //TnewReadOnlyBytesFromFile
 {char *s = "0123456789";                                                       // Sample data
  const FileName f = makeFileNameTemporaryWithContent
  ("readOnlyBytes.data", s, 0);                                                 // Temporary file


  $ q = make$FromFormat("%s", s);                                                // New descriptor
    q ▷ writeFile(f);

  $ r = make$FromFile(f);                                                        // New descriptor
  assert(q ▷ equals(r));
  assert(q ▷ equalsString(s));
    q ▷ free; r ▷ free;
  f ▷ unlink;
  assert(!f ▷ size);
 }

void test2()                                                                    //TnewReadOnlyBytesFromFormat
 {char *s = "abcdefg";
  $     q = make$FromFormat("%s %d %s", s, strlen(s), s);

  assert(q ▷ length == 17);
  assert(q ▷ equalsString("abcdefg 7 abcdefg"));

  q ▷ free;
 }

void test3()                                                                    //TwriteOpenFile
 {char    *s = "abcdefg";
  $        q = make$FromFormat("%s %d %s", s, strlen(s), s);
  FileName f = makeFileNameTemporaryWithContent("$.data", 0, 0);
  FILE    *o = fopen(f.name, "w");
           q ▷ writeOpenFile(o);
  fclose  (o);
  assert(f ▷ size == 17);
         q ▷ free;
 }

void test4()                                                                    //TnewReadOnlyBytesFromString
 {char *s = "abcdefg";
  $     q = make$FromString(s);
  assert(strncmp(q.data, s, q.length) == 0 && q.length == strlen(s));
        q ▷ free;
 }

void test5()                                                                    //Tb2SumW8
 {char  *s = strdup("aaaa\n");
  $      q = make$FromString(s);
  assert(q ▷ b2SumW8 == 0x83);
         q ▷ free;
  free  (s);
 }

void test6()                                                                    //TcontainsString
 {$ s = make$FromFormat("abcd");
  assert( s ▷ containsString("bc"));
  assert(!s ▷ containsString("cb"));
  s ▷ free;
 }

void test7()                                                                    //Tmake$Dup //Tmake$DupN //Tmake$FromString //Tmake$FromStringN //Tmake$FromFormat //Tmake$Buffer
 {$ d = make$Dup       ("aaa"), D = make$DupN       ("aaaa", 3),
    s = make$FromString("aaa"), S = make$FromStringN("aaaa", 3),
    f = make$FromFormat("%s", "aaa"), m = make$Buffer(3);
    memcpy(m.data, "aaaa", 3);
  assert(d ▷ equals(D)); assert(d ▷ length == 3); assert(D ▷ length == 3);
  assert(d ▷ equals(s)); assert(s ▷ length == 3);
  assert(d ▷ equals(S)); assert(S ▷ length == 3);
  assert(d ▷ equals(f)); assert(f ▷ length == 3);

  d ▷ free; D ▷ free; s ▷ free; S ▷ free; f ▷ free; m ▷ free;
 }

void test8()                                                                    //Tmake$FromFile //TwriteTemporaryFile
 {$ s = make$FromStringN("aaa", 2);
  FileName f = s ▷ writeTemporaryFile("test.data");
  $ t = make$FromFile(f);
  assert(t ▷ equals(s)); assert(t ▷ equalsString("aa"));
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4, test5,
                           test6, test7, test8, 0};
  run_tests("$", 1, tests);
  return 0;
 }

#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/readOnlyBytes/readOnlyBytes
