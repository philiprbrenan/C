#line 1 "/home/phil/c/z/stringBuffer/stringBuffer.c"
//------------------------------------------------------------------------------
// String buffer
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef StringBuffer_included
#define StringBuffer_included
#include <arenaTree.c>
#include <readOnlyBytes.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Tree.

typedef struct StringBuffer                                                                // StringBuffer
 {const struct ProtoTypes_StringBuffer *proto;                                             // Prototypes for methods
  ArenaTree string;                                                             // String being built
 } StringBuffer;

#include <stringBuffer_prototypes.h>

static StringBuffer makeStringBuffer                                                                  // Make a StringBuffer
 ()                                                                             //
 {ArenaTree string = makeArenaTree();
  StringBuffer buffer = newStringBuffer(({struct StringBuffer t = {string: string, proto: &ProtoTypes_StringBuffer}; t;}));
  return buffer;
 }

static void free_StringBuffer                                                              // Free a StringBuffer
 (const StringBuffer buffer)                                                               // StringBuffer
 {buffer.string.proto->free(buffer.string);
 }

//D1 Concatenate                                                                // Concatenate various items

static void add                                                                 // Concatenate a string
 (const StringBuffer buffer,                                                               // StringBuffer
  const char * const string)                                                    // Zero terminated string
 {const ArenaTreeNode s = buffer.string.proto->node(buffer.string, string);
  s.proto->putTreeLast(s);
 }

static void addReadOnlyBytes                                                    // Concatenate a read only bytes
 (const StringBuffer             buffer,                                                   // StringBuffer
  const ReadOnlyBytes string)                                                   // Read only bytes
 {const ArenaTreeNode s = buffer.string.proto->noden(buffer.string, string.data, string.length);
  s.proto->putTreeLast(s);
 }

static void addFormat                                                           // Add a formatted string
 (const StringBuffer buffer,                                                               // StringBuffer
  const char * const format,                                                    // Format
  ...)                                                                          // Strings
 {va_list va;
  va_start(va, format);
  char * data; const int length = vasprintf(&data, format, va);                 // Allocate and format output string
  va_end(va);
  const ArenaTreeNode s = buffer.string.proto->noden(buffer.string, data, length);
  s.proto->putTreeLast(s);
  free(data);
 }

//D1 Statistics                                                                 // Statistics on the contents of the StringBuffer.

static size_t length                                                              // Length of the string held in the buffer
 (const StringBuffer buffer)                                                               // StringBuffer
 {size_t length = 0;
  ArenaTreefe(c, buffer.string) length += strlen(c.proto->key(c));
  return length;
 }

static int equals_StringBuffer_StringBuffer                                                           // Checks whether two StringBuffer are equal.
 (const StringBuffer a,                                                                    // First StringBuffer
  const StringBuffer b)                                                                    // Second StringBuffer
 {if (a.proto->length(a) != b.proto->length(b)) return 0;
  const ReadOnlyBytes A = a.proto->string(a), B = b.proto->string(b);
  const int r = A.proto->equals(A, B);
  A.proto->free(A); B.proto->free(B);
  return r;
 }

static int equalsString_StringBuffer_string                                                // Checks whether a StringBuffer is equal to a specified zero terminated string.
 (const StringBuffer            buffer,                                                    // StringBuffer
  const char * const string)                                                    // String
 {const size_t l = strlen(string);
  if (buffer.proto->length(buffer) != l) return 0;
  const ReadOnlyBytes b = buffer.proto->string(buffer);
  const int r = !strncmp(b.data, string, l);
  b.proto->free(b);
  return r;
 }

static int contains_StringBuffer_StringBuffer                                                         // Checks whether the first StringBuffer contains the second StringBuffer
 (const StringBuffer a,                                                                    // First StringBuffer
  const StringBuffer b)                                                                    // Second StringBuffer
 {if (a.proto->length(a) < b.proto->length(b)) return 0;
  const ReadOnlyBytes A = a.proto->string(a), B = b.proto->string(b);
  const int r = !!strstr(A.data, B.data);
  A.proto->free(A); B.proto->free(B);
  return r;
 }

static int containsString_StringBuffer_StringBuffer                                                   // Checks whether a StringBuffer contains a specified zero terminated string.
 (const StringBuffer            buffer,                                                    // StringBuffer
  const char * const string)                                                    // String
 {const size_t l = strlen(string);
  if (buffer.proto->length(buffer) < l) return 0;
  const ReadOnlyBytes b = buffer.proto->string(buffer);
  const int r = !!strstr(b.data, string);
  b.proto->free(b);
  return r;
 }

//D1 String                                                                     // Make into a string

static ReadOnlyBytes string_StringBuffer                                                   // Create a read only bytes string from the StringBuffer.
 (const StringBuffer buffer)                                                               // StringBuffer
 {const size_t l = buffer.proto->length(buffer);
  const ReadOnlyBytes r = makeReadOnlyBytesBuffer(l);
  char * p = r.data;
  ArenaTreefe(c, buffer.string) p = stpcpy(p, c.proto->key(c));
  return r;
 }

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TmakeStringBuffer //Tadd //TaddFormat //TaddReadOnlyBytes //Tlength //Tstring //Tfree
 {StringBuffer s = makeStringBuffer();
    s.proto->add(s, "Hello");
    s.proto->add(s, " ");
    s.proto->add(s, "world");
  ReadOnlyBytes r = s.proto->string(s);
  assert(r.proto->equalsString(r, "Hello world"));
  int i = 0;
    s.proto->addFormat(s, " - %d ", ++i);
    s.proto->addReadOnlyBytes(s, r);
    s.proto->addFormat(s, " - %d",   ++i);
  ReadOnlyBytes p = s.proto->string(s);
  assert(p.proto->equalsString(p, "Hello world - 1 Hello world - 2"));
  p.proto->free(p); r.proto->free(r); s.proto->free(s);
 }

void test1()                                                                    //Tequals //TequalsString //Tcontains //TcontainsString
 {StringBuffer a = makeStringBuffer();   StringBuffer b = makeStringBuffer(); StringBuffer c = makeStringBuffer();
    a.proto->add(a, "ab");   b.proto->add(b, "a");  c.proto->add(c, "aba");
    a.proto->add(a, "c");    b.proto->add(b, "bc"); c.proto->add(c, "bc");

  assert( a.proto->equals(a, b));
  assert( a.proto->equalsString(a, "abc"));
  assert(!a.proto->equals(a, c));
  assert( c.proto->contains(c, a));
  assert(!a.proto->contains(a, c));
  assert( c.proto->containsString(c, "bab"));

  a.proto->free(a); b.proto->free(b); c.proto->free(c);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, 0};
  run_tests("StringBuffer", 1, tests);
  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/stringBuffer/stringBuffer
