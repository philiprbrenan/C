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

//D1 Statistics                                                                 // Statistics on the contents of the StringBuffer.

static size_t length                                                              // Length of the string held in the buffer
 (const StringBuffer buffer)                                                               // StringBuffer
 {size_t length = 0;
  ArenaTreefe(c, buffer.string) length += strlen(c.proto->key(c));
  return length;
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

void test0()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer //Terrors //TmakeStringBufferParseFromFile
 {StringBuffer s = makeStringBuffer();
    s.proto->add(s, "Hello");
    s.proto->add(s, " ");
    s.proto->add(s, "world");
  ReadOnlyBytes r = s.proto->string(s);
  assert(r.proto->equalsString(r, "Hello world"));
    r.proto->free(r);
    s.proto->free(s);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, 0};
  run_tests("StringBuffer", 1, tests);
  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/stringBuffer/stringBuffer
