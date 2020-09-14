#line 1 "/home/phil/c/z/stringBuffer/stringBuffer.c"
//------------------------------------------------------------------------------
// String buffer
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef StringBuffer_included
#define StringBuffer_included
#include <readOnlyBytes.c>
#include <arenaTree.c>
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
  return newStringBuffer(({struct StringBuffer t = {string: string, proto: &ProtoTypes_StringBuffer}; t;}));
 }

static void free_StringBuffer                                                              // Free a StringBuffer
 (const StringBuffer buffer)                                                               // StringBuffer
 {buffer.string.proto->free(buffer.string);
 }

//D1 Concatenate                                                                // Concatenate various items

static void add_StringBuffer_string                                                        // Concatenate a string
 (const StringBuffer buffer,                                                               // StringBuffer
  const char * const string)                                                    // Zero terminated string
 {const ArenaTreeNode s = buffer.string.proto->node(buffer.string, string);
  s.proto->putTreeLast(s);
 }

static void addReadOnlyBytes_StringBuffer_readOnlyBytes                                    // Concatenate a read only bytes
 (const StringBuffer             buffer,                                                   // StringBuffer
  const ReadOnlyBytes string)                                                   // Read only bytes
 {const ArenaTreeNode s = buffer.string.proto->noden(buffer.string, string.data, string.length);
  s.proto->putTreeLast(s);
 }

static void addVaFormat_StringBuffer_string_va                                             // Add a variadic argument formatted string
 (const StringBuffer buffer,                                                               // StringBuffer
  const char * const format,                                                    // Format
  va_list va1)                                                                  // Variable argument list
 {va_list va2; va_copy(va2, va1);                                               // We might need to make two passes through the variable arguments
  const int N = 1024;                                                           // Guess a reasonable size                                                                               //dx=const Xml x =  for the output string
  char data[N];                                                                 // Space on stack for first attempt
  const int L = vsnprintf(data, N, format, va1);                                // First attempt
  if (N > L + 1)                                                                // Success on the first attempt
   {const ArenaTreeNode s = buffer.string.proto->noden(buffer.string, data, L);
                        s.proto->putTreeLast(s);
   }
  else                                                                          // Second attempt
   {char data[L+1];
    vsnprintf(data, L+1, format, va2);
    const ArenaTreeNode s = buffer.string.proto->noden(buffer.string, data, L);
                        s.proto->putTreeLast(s);
   }
 }


static void addFormat_StringBuffer_strings                                                 // Add a formatted string
 (const StringBuffer buffer,                                                               // StringBuffer
  const char * const format,                                                    // Format
  ...)                                                                          // Strings
 {va_list va;
  va_start(va, format);
  buffer.proto->addVaFormat(buffer, format, va);
  va_end(va);
 }

static void addChar_StringBuffer_char                                                      // Add the specified character
 (const StringBuffer    buffer,                                                            // StringBuffer
  const char c)                                                                 // Character to add
 {const char s[2] = {c, 0};
  buffer.proto->add(buffer, s);
 }

static void addNewLine_StringBuffer                                                        // Add a new line
 (const StringBuffer buffer)                                                               // StringBuffer
 {buffer.proto->add(buffer, "\n");
 }

static void addSingleQuote_StringBuffer                                                    // Add a single quote
 (const StringBuffer buffer)                                                               // StringBuffer
 {buffer.proto->add(buffer, "'");
 }

static void addDoubleQuote_StringBuffer                                                    // Add a double quote
 (const StringBuffer buffer)                                                               // StringBuffer
 {buffer.proto->add(buffer, "\"");
 }

static void addQuotedNewLine_StringBuffer                                                  // Add a quoted new line
 (const StringBuffer buffer)                                                               // StringBuffer
 {buffer.proto->add(buffer, "\\n");
 }

//D1 Statistics                                                                 // Statistics on the contents of the StringBuffer.

static size_t length_StringBuffer                                                          // Length of the string held in the buffer
 (const StringBuffer buffer)                                                               // StringBuffer
 {size_t length = 0;
  ArenaTreefe(c, buffer.string) length += strlen(c.proto->key(c));
  return length;
 }

static int equals_StringBuffer_StringBuffer                                                           // Checks whether two StringBuffer are equal.
 (const StringBuffer a,                                                                    // First StringBuffer
  const StringBuffer b)                                                                    // Second StringBuffer
 {const size_t la = a.proto->length(a), lb = b.proto->length(b);
  if (la != lb) return 0;                                                       // Cannot be equal if they do not have the same length
  char A[la+1], B[lb+1]; a.proto->string(a, A); b.proto->string(b, B);                          // Use space on the stack to hold copies of the strings to be compared
  return !strcmp(A, B);
 }

static int equalsString_StringBuffer_string                                                // Checks whether a StringBuffer is equal to a specified zero terminated string.
 (const StringBuffer            buffer,                                                    // StringBuffer
  const char * const string)                                                    // String
 {const size_t l = strlen(string);
  if (buffer.proto->length(buffer) != l) return 0;
  char B[l+1]; buffer.proto->string(buffer, B);
  return !strcmp(B, string);
 }

static int contains_StringBuffer_StringBuffer                                                         // Checks whether the first StringBuffer contains the second StringBuffer
 (const StringBuffer a,                                                                    // First StringBuffer
  const StringBuffer b)                                                                    // Second StringBuffer
 {const size_t la = a.proto->length(a), lb = b.proto->length(b);
  if (la < lb) return 0;                                                        // Cannot be contained in a shorter string
  char A[la+1], B[lb+1]; a.proto->string(a, A); b.proto->string(b, B);
  return !!strstr(A, B);
 }

static int containsString_StringBuffer_StringBuffer                                                   // Checks whether a StringBuffer contains a specified zero terminated string.
 (const StringBuffer            buffer,                                                    // StringBuffer
  const char * const string)                                                    // String
 {const size_t l = strlen(string), lb = buffer.proto->length(buffer);
  if (lb < l) return 0;                                                         // Cannot be contained in a shorter string
  char B[lb+1]; buffer.proto->string(buffer, B);
  return !!strstr(B, string);
 }

static int substringEquals_int_StringBuffer_int_int_string                                 // Checks whether a sub string of the specified StringBuffer is equal to the specified zero terminated string.
 (const StringBuffer            buffer,                                                    // StringBuffer
  const size_t       start,                                                     // Offset to start of string
  const size_t       length,                                                    // Length of sub string. The length of the zero terminate string to be loaded must be larger than this.
  const char * const string)                                                    // Zero terminated string expected
 {const size_t l =   strlen(string), lb = buffer.proto->length(buffer);
  if (lb < l) return 0;                                                         // Cannot be contained in a shorter string
  char s[length+1]; buffer.proto->substring(buffer, start, length, s);
  return !strcmp(s, string);
 }

//D1 String                                                                     // Make into a string

static void string_StringBuffer_string                                                     // Load a zero terminated string from a StringBuffer.
 (const StringBuffer        buffer,                                                        // StringBuffer
  char  * const  string)                                                        // String to load with enough space for the string and its terminating zero
 {char  * p =    string;
  ArenaTreefe(c, buffer.string) p = stpcpy(p, c.proto->key(c));
  *p = 0;
 }

static size_t substring_size_StringBuffer_int_int_string                                   // Load a zero terminates string with a sub string of a StringBuffer and return the number of bytes loaded.
 (const StringBuffer      buffer,                                                          // StringBuffer
  const size_t start,                                                           // Offset to start of string
  const size_t length,                                                          // Length of sub string. The length of the zero terminate string to be loaded must be larger than this.
  char * const string)                                                          // String to load with enough space for the string and its terminating zero
 {const size_t l = buffer.proto->length(buffer);                                             // Length of source string
  *string        = 0;                                                           // Start with an empty output string
  if (start >= l) return 0;                                                     // Substring starts beyond the end of the string
  char s[l+1]; buffer.proto->string(buffer, s);                                              // Buffer as a single string
  const size_t n = start + length < l ? length : l - start;                     // Amount we can copy
  strncpy(string, s + start, n);                                                // Copy out as much of the sub string as we can
  string[n] = 0;                                                                // Zero terminate sub string
  return n;                                                                     // Actual length of sub string
 }

static ReadOnlyBytes readOnlyBytes_StringBuffer                                            // Create a read only bytes string from the StringBuffer.
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
    s.proto->add(s, "Hello"); assert(s.proto->length(s) == 5);
    s.proto->add(s, " ");
    s.proto->add(s, "world");
  ReadOnlyBytes r = s.proto->readOnlyBytes(s);
  assert(r.proto->equalsString(r, "Hello world"));
  int i = 0;
    s.proto->addFormat(s, " - %d ", ++i);
    s.proto->addReadOnlyBytes(s, r);
    s.proto->addFormat(s, " - %d",   ++i);
  ReadOnlyBytes p = s.proto->readOnlyBytes(s);
  assert(p.proto->equalsString(p, "Hello world - 1 Hello world - 2"));
  p.proto->free(p); r.proto->free(r); s.proto->free(s);
 }

void test1()                                                                    //Tequals //TequalsString //Tcontains //TcontainsString //TreadOnlyBytes //Tsubstring //TsubstringEquals
 {StringBuffer a = makeStringBuffer();   StringBuffer b = makeStringBuffer(); StringBuffer c = makeStringBuffer();
    a.proto->add(a, "ab");   b.proto->add(b, "a");  c.proto->add(c, "aba");
    a.proto->add(a, "c");    b.proto->add(b, "bc"); c.proto->add(c, "bc");

  assert( a.proto->equals(a, b));
  assert( a.proto->equalsString(a, "abc"));
  assert(!a.proto->equals(a, c));
  assert( c.proto->contains(c, a));
  assert(!a.proto->contains(a, c));
  assert( c.proto->containsString(c, "bab"));

  assert( c.proto->substringEquals(c, 1, 0, ""));
  assert( c.proto->substringEquals(c, 1, 1, "b"));
  assert( c.proto->substringEquals(c, 1, 2, "ba"));
  assert( c.proto->substringEquals(c, 1, 3, "bab"));
  assert( c.proto->substringEquals(c, 1, 4, "babc"));
  assert( c.proto->substringEquals(c, 1, 5, "babc"));

  assert( c.proto->substringEquals(c, 5, 0, ""));
  assert( c.proto->substringEquals(c, 5, 1, ""));

  if (1)
   {char buffer[16];
    c.proto->substring(c, 1, 3, buffer);
    assert(!strcmp(buffer, "bab"));
   }

  ReadOnlyBytes A = a.proto->readOnlyBytes(a), B = b.proto->readOnlyBytes(b);
  assert(A.proto->equals(A, B));

  a.proto->free(a); b.proto->free(b); c.proto->free(c); A.proto->free(A); B.proto->free(B);
 }

void test2()                                                                    //TaddChar //TaddNewLine
 {StringBuffer a = makeStringBuffer();
    a.proto->addChar(a, 'a');
    a.proto->addNewLine(a);
    a.proto->addSingleQuote(a);
    a.proto->addDoubleQuote(a);
  assert( a.proto->equalsString(a, "a\n'\""));

  a.proto->free(a);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, 0};
  run_tests("StringBuffer", 1, tests);
  return 0;
 }
#endif
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/stringBuffer/stringBuffer
