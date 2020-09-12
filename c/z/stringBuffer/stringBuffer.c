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
  $ buffer = new$(string: string);
  return buffer;
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

//D1 Statistics                                                                 // Statistics on the contents of the $.

static size_t length                                                              // Length of the string held in the buffer
 (const $ buffer)                                                               // $
 {size_t length = 0;
  ArenaTreefe(c, buffer.string) length += strlen(c ▷ key);
  return length;
 }

static int equals_$_$                                                           // Checks whether two $ are equal.
 (const $ a,                                                                    // First $
  const $ b)                                                                    // Second $
 {if (a ▷ length != b ▷ length) return 0;
  const ReadOnlyBytes A = a ▷ string, B = b ▷ string;
  const int r = A ▷ equals(B);
  A ▷ free; B ▷ free;
  return r;
 }

static int equalsString_$_string                                                // Checks whether a $ is equal to a specified zero terminated string.
 (const $            buffer,                                                    // $
  const char * const string)                                                    // String
 {const size_t l = strlen(string);
  if (buffer ▷ length != l) return 0;
  const ReadOnlyBytes b = buffer ▷ string;
  const int r = !strncmp(b.data, string, l);
  b ▷ free;
  return r;
 }

static int contains_$_$                                                         // Checks whether the first $ contains the second $
 (const $ a,                                                                    // First $
  const $ b)                                                                    // Second $
 {if (a ▷ length < b ▷ length) return 0;
  const ReadOnlyBytes A = a ▷ string, B = b ▷ string;
  const int r = !!strstr(A.data, B.data);
  A ▷ free; B ▷ free;
  return r;
 }

static int containsString_$_$                                                   // Checks whether a $ contains a specified zero terminated string.
 (const $            buffer,                                                    // $
  const char * const string)                                                    // String
 {const size_t l = strlen(string);
  if (buffer ▷ length < l) return 0;
  const ReadOnlyBytes b = buffer ▷ string;
  const int r = !!strstr(b.data, string);
  b ▷ free;
  return r;
 }

//D1 String                                                                     // Make into a string

static ReadOnlyBytes string_$                                                   // Create a read only bytes string from the $.
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
    s ▷ add("Hello");
    s ▷ add(" ");
    s ▷ add("world");
  ReadOnlyBytes r = s ▷ string;
  assert(r ▷ equalsString("Hello world"));
  int i = 0;
    s ▷ addFormat(" - %d ", ++i);
    s ▷ addReadOnlyBytes(r);
    s ▷ addFormat(" - %d",   ++i);
  ReadOnlyBytes p = s ▷ string;
  assert(p ▷ equalsString("Hello world - 1 Hello world - 2"));
  p ▷ free; r ▷ free; s ▷ free;
 }

void test1()                                                                    //Tequals //TequalsString //Tcontains //TcontainsString
 {$ a = make$();   $ b = make$(); $ c = make$();
    a ▷ add("ab");   b ▷ add("a");  c ▷ add("aba");
    a ▷ add("c");    b ▷ add("bc"); c ▷ add("bc");

  assert( a ▷ equals         (b));
  assert( a ▷ equalsString   ("abc"));
  assert(!a ▷ equals         (c));
  assert( c ▷ contains       (a));
  assert(!a ▷ contains       (c));
  assert( c ▷ containsString("bab"));

  a ▷ free; b ▷ free; c ▷ free;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/stringBuffer/stringBuffer
