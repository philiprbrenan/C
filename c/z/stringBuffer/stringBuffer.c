//------------------------------------------------------------------------------
// String buffer
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef $_included
#define $_included
#include <arenaTree.c>
#include <readOnlyBytes.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Tree.

typedef struct $                                                                // $
 {const struct ProtoTypes_$ *proto;                                             // Prototypes for methods
  ArenaTree string;                                                             // String being built
 } $;

#include <$$_prototypes.h>

static $ make$                                                                  // Make a $
 ()                                                                             //
 {ArenaTree string = makeArenaTree();
  return new$(string: string);
 }

static void free_$                                                              // Free a $
 (const $ buffer)                                                               // $
 {buffer.string ▷ free;
 }

//D1 Concatenate                                                                // Concatenate various items

static void add                                                                 // Concatenate a string
 (const $ buffer,                                                               // $
  const char * const string)                                                    // Zero terminated string
 {const ArenaTreeNode s = buffer.string ▷ node(string);
  s ▷ putTreeLast;
 }

static void addReadOnlyBytes                                                    // Concatenate a read only bytes
 (const $             buffer,                                                   // $
  const ReadOnlyBytes string)                                                   // Read only bytes
 {const ArenaTreeNode s = buffer.string ▷ noden(string.data, string.length);
  s ▷ putTreeLast;
 }

static void addFormat                                                           // Add a formatted string
 (const $ buffer,                                                               // $
  const char * const format,                                                    // Format
  ...)                                                                          // Strings
 {va_list va;
  va_start(va, format);
  char * data; const int length = vasprintf(&data, format, va);                 // Allocate and format output string
  va_end(va);
  const ArenaTreeNode s = buffer.string ▷ noden(data, length);
  s ▷ putTreeLast;
  free(data);
 }

static void addChar_$                                                           // Add the specified character
 (const $    buffer,                                                            // $
  const char c)                                                                 // Character to add
 {const char s[2] = {c, 0};
  buffer ▷ add(s);
 }

static void addNewLine                                                          // Add a new line
 (const $ buffer)                                                               // $
 {buffer ▷ add("\n");
 }

//D1 Statistics                                                                 // Statistics on the contents of the $.

static size_t length_$                                                          // Length of the string held in the buffer
 (const $ buffer)                                                               // $
 {size_t length = 0;
  ArenaTreefe(c, buffer.string) length += strlen(c ▷ key);
  return length;
 }

static int equals_$_$                                                           // Checks whether two $ are equal.
 (const $ a,                                                                    // First $
  const $ b)                                                                    // Second $
 {const size_t la = a ▷ length, lb = b ▷ length;
  if (la != lb) return 0;                                                       // Cannot be equal if they do not have the same length
  char A[la+1], B[lb+1]; a ▷ string(A); b ▷ string(B);                          // Use space on the stack to hold copies of the strings to be compared
  return !strcmp(A, B);
 }

static int equalsString_$_string                                                // Checks whether a $ is equal to a specified zero terminated string.
 (const $            buffer,                                                    // $
  const char * const string)                                                    // String
 {const size_t l = strlen(string);
  if (buffer ▷ length != l) return 0;
  char B[l+1]; buffer ▷ string(B);
  return !strcmp(B, string);
 }

static int contains_$_$                                                         // Checks whether the first $ contains the second $
 (const $ a,                                                                    // First $
  const $ b)                                                                    // Second $
 {const size_t la = a ▷ length, lb = b ▷ length;
  if (la < lb) return 0;                                                        // Cannot be contained in a shorter string
  char A[la+1], B[lb+1]; a ▷ string(A); b ▷ string(B);
  return !!strstr(A, B);
 }

static int containsString_$_$                                                   // Checks whether a $ contains a specified zero terminated string.
 (const $            buffer,                                                    // $
  const char * const string)                                                    // String
 {const size_t l = strlen(string), lb = buffer ▷ length;
  if (lb < l) return 0;                                                         // Cannot be contained in a shorter string
  char B[lb+1]; buffer ▷ string(B);
  return !!strstr(B, string);
 }

static int substringEquals_int_$_int_int_string                                 // Checks whether a sub string of the specified $ is equal to the specified zero terminated string.
 (const $            buffer,                                                    // $
  const size_t       start,                                                     // Offset to start of string
  const size_t       length,                                                    // Length of sub string. The length of the zero terminate string to be loaded must be larger than this.
  const char * const string)                                                    // Zero terminated string expected
 {const size_t l =   strlen(string), lb = buffer ▷ length;
  if (lb < l) return 0;                                                         // Cannot be contained in a shorter string
  char s[length+1]; buffer ▷ substring(start, length, s);
  return !strcmp(s, string);
 }

//D1 String                                                                     // Make into a string

static void string_$_string                                                     // Load a zero terminated string from a $.
 (const $        buffer,                                                        // $
  char  * const  string)                                                        // String to load with enough space for the string and its terminating zero
 {char  * p =    string;
  ArenaTreefe(c, buffer.string) p = stpcpy(p, c ▷ key);
  *p = 0;
 }

static size_t substring_size_$_int_int_string                                   // Load a zero terminates string with a sub string of a $ and return the number of bytes loaded.
 (const $      buffer,                                                          // $
  const size_t start,                                                           // Offset to start of string
  const size_t length,                                                          // Length of sub string. The length of the zero terminate string to be loaded must be larger than this.
  char * const string)                                                          // String to load with enough space for the string and its terminating zero
 {const size_t l = buffer ▷ length;                                             // Length of source string
  *string        = 0;                                                           // Start with an empty output string
  if (start >= l) return 0;                                                     // Substring starts beyond the end of the string
  char s[l+1]; buffer ▷ string(s);                                              // Buffer as a single string
  const size_t n = start + length < l ? length : l - start;                     // Amount we can copy
  strncpy(string, s + start, n);                                                // Copy out as much of the sub string as we can
  string[n] = 0;                                                                // Zero terminate sub string
  return n;                                                                     // Actual length of sub string
 }

static ReadOnlyBytes readOnlyBytes_$                                            // Create a read only bytes string from the $.
 (const $ buffer)                                                               // $
 {const size_t l = buffer ▷ length;
  const ReadOnlyBytes r = makeReadOnlyBytesBuffer(l);
  char * p = r.data;
  ArenaTreefe(c, buffer.string) p = stpcpy(p, c ▷ key);
  return r;
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //Tmake$ //Tadd //TaddFormat //TaddReadOnlyBytes //Tlength //Tstring //Tfree
 {$ s = make$();
    s ▷ add("Hello"); assert(s ▷ length == 5);
    s ▷ add(" ");
    s ▷ add("world");
  ReadOnlyBytes r = s ▷ readOnlyBytes;
  assert(r ▷ equalsString("Hello world"));
  int i = 0;
    s ▷ addFormat(" - %d ", ++i);
    s ▷ addReadOnlyBytes(r);
    s ▷ addFormat(" - %d",   ++i);
  ReadOnlyBytes p = s ▷ readOnlyBytes;
  assert(p ▷ equalsString("Hello world - 1 Hello world - 2"));
  p ▷ free; r ▷ free; s ▷ free;
 }

void test1()                                                                    //Tequals //TequalsString //Tcontains //TcontainsString //TreadOnlyBytes //Tsubstring //TsubstringEquals
 {$ a = make$();   $ b = make$(); $ c = make$();
    a ▷ add("ab");   b ▷ add("a");  c ▷ add("aba");
    a ▷ add("c");    b ▷ add("bc"); c ▷ add("bc");

  assert( a ▷ equals         (b));
  assert( a ▷ equalsString   ("abc"));
  assert(!a ▷ equals         (c));
  assert( c ▷ contains       (a));
  assert(!a ▷ contains       (c));
  assert( c ▷ containsString("bab"));

  assert( c ▷ substringEquals(1, 0, ""));
  assert( c ▷ substringEquals(1, 1, "b"));
  assert( c ▷ substringEquals(1, 2, "ba"));
  assert( c ▷ substringEquals(1, 3, "bab"));
  assert( c ▷ substringEquals(1, 4, "babc"));
  assert( c ▷ substringEquals(1, 5, "babc"));

  assert( c ▷ substringEquals(5, 0, ""));
  assert( c ▷ substringEquals(5, 1, ""));

  if (1)
   {char buffer[16];
    c ▷ substring(1, 3, buffer);
    assert(!strcmp(buffer, "bab"));
   }

  ReadOnlyBytes A = a ▷ readOnlyBytes, B = b ▷ readOnlyBytes;
  assert(A ▷ equals(B));

  a ▷ free; b ▷ free; c ▷ free; A ▷ free; B ▷ free;
 }

void test2()                                                                    //TaddChar //TaddNewLine
 {$ a = make$();
    a ▷ addChar('a');
    a ▷ addNewLine;
  assert( a ▷ equalsString   ("a\n"));

  a ▷ free;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/stringBuffer/stringBuffer
