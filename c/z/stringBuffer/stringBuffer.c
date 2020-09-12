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

//D1 Statistics                                                                 // Statistics on the contents of the $.

static size_t length                                                              // Length of the string held in the buffer
 (const $ buffer)                                                               // $
 {size_t length = 0;
  ArenaTreefe(c, buffer.string) length += strlen(c ▷ key);
  return length;
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

void test0()                                                                    //TnewArenaTree //Tnew //Tfree //TputFirst //TputLast //Tfe //Tfer //Terrors //Tmake$ParseFromFile
 {$ s = make$();
    s ▷ add("Hello");
    s ▷ add(" ");
    s ▷ add("world");
  ReadOnlyBytes r = s ▷ string;
  assert(r ▷ equalsString("Hello world"));
    r ▷ free;
    s ▷ free;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/stringBuffer/stringBuffer
