#line 1 "/home/phil/c/z/stringBuffer/stringBuffer.c"
//------------------------------------------------------------------------------
// String buffer
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <ctype.h>
#include <arenaTree.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Tree.

#ifndef StringBuffer_included_StringBuffer
#define StringBuffer_included_StringBuffer
typedef struct StringBuffer                                                                // StringBuffer
 {const struct ProtoTypes_StringBuffer *proto;                                             // Prototypes for methods
  ArenaTree     string;                                                         // String being built
 } StringBuffer;
#endif

#ifndef StringBuffer_included
#define StringBuffer_included
#include <stringBuffer_prototypes.h>
#define StringBufferfe( child, parent) for(ArenaTreeNode child = parent.string.proto->first(parent.string); child.proto->valid(child); child = child.proto->next(child)) // Each child in a parent from first to last
#define StringBufferfer(child, parent) for(ArenaTreeNode child = parent.string.proto->last(parent.string) ; child.proto->valid(child); child = child.proto->prev(child)) // Each child in a parent from last to first

StringBuffer makeStringBuffer                                                                         // Make a StringBuffer
 ()                                                                             //
 {ArenaTree string = makeArenaTree();
  return newStringBuffer(({struct StringBuffer t = {string: string, proto: &ProtoTypes_StringBuffer}; t;}));
 }

StringBuffer makeStringBufferFromString                                                               // Make a StringBuffer from a string
 (const char * const string)                                                    // String
 {const typeof(makeStringBuffer()) b = makeStringBuffer();
  b.proto->add(b, string);
  return b;
 }

StringBuffer makeStringBufferFromVaFormat                                                             // Make a string buffer from a variadic argument formatted string
 (const char * const format,                                                    // Format
  ...)                                                                          // Variable argument list
 {va_list va;
  va_start(va, format);
  const typeof(makeStringBuffer()) s = makeStringBuffer();
  s.proto->addVaFormat(s, format, va);
  return s;
 }

static void free_StringBuffer                                                              // Free a StringBuffer
 (const StringBuffer buffer)                                                               // StringBuffer
 {buffer.string.proto->free(buffer.string);
 }

//D1 Concatenate                                                                // Concatenate various items to a string buffer.

static void add_StringBuffer_string                                                        // Concatenate a string
 (const StringBuffer buffer,                                                               // StringBuffer
  const char * const string)                                                    // Zero terminated string
 {const ArenaTreeNode s = buffer.string.proto->node(buffer.string, string);
  s.proto->putTreeLast(s);
 }

static void addn_StringBuffer_string                                                       // Concatenate a string of known length
 (const StringBuffer            buffer,                                                    // StringBuffer
  const char * const string,                                                    // String
  const size_t       length)                                                    // String length
 {const typeof(buffer.string.proto->noden(buffer.string, string, length)) s = buffer.string.proto->noden(buffer.string, string, length);
  s.proto->putTreeLast(s);
 }

static void addStringBuffer_StringBuffer_StringBuffer                                                            // Add a string buffer
 (const StringBuffer buffer,                                                               // Target StringBuffer
  const StringBuffer add)                                                                  // Source StringBuffer
 {const size_t l = buffer.proto->count(buffer);
  char *s[l];
  size_t i = 0; StringBufferfe(line, add) s[i++] = line.proto->key(line);
  for(size_t i = 0; i < l; ++i) buffer.proto->add(buffer, s[i]);
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

//D1 Join and Split                                                             // Join all the sub strings in the strin buffer into one string. Split the contents of the string buffer on spaces or new lines.

static size_t count_StringBuffer                                                           // Count the number of sub strings in the buffer
 (const StringBuffer buffer)                                                               // StringBuffer
 {return buffer.string.proto->countChildren(buffer.string);                                         // Count the number of sub strings
 }

static void join_StringBuffer                                                              // Join in place - join all the sub strings in the specified string buffer and replace them with one string.
 (const StringBuffer old)                                                                  // StringBuffer
 {const typeof(old.string.proto->countChildren(old.string)) C = old.string.proto->countChildren(old.string);                                               // Number of sub strings
  if (C <= 1) return;                                                           // Already joined
  const typeof(old.proto->length(old)) N = old.proto->length(old);                                                             // Length of joined string
  char b[N+1]; old.proto->string(old, b);
  const typeof(makeStringBuffer()) new = makeStringBuffer();                                                                // New string buffer
  new.proto->add(new, b);                                                                 // Load new buffer from joined string
  old.string.proto->swap(old.string, new.string);                                                // Swap arenas so that the old becomes the new
  new.proto->free(new);                                                                   // Free old arena
 }

static StringBuffer splitLines                                                  // Split the specified StringBuffer on any new line characters and return the split text as a new StringBuffer
 (const StringBuffer string)                                                               // StringBuffer
 {const typeof(makeStringBuffer()) r = makeStringBuffer();                                                                  // Results
  const typeof(string.proto->length(string)) N = string.proto->length(string);                                                          // Length of current string
  char s[N+1]; string.proto->string(string, s);                                              // Linearize current string
  size_t i = 0, j = 0;                                                          // Line start and end
  void save()                                                                   // Save a string
   {r.proto->addn(r, s+j, i - j + 1);
    j = i+1;
   }
  for(; i < N; ++i) if (s[i] == '\n') save();
  if (  i > j) save();
  return r;
 }

static StringBuffer splitWords                                                  // Split the specified StringBuffer into words delimited by spaces and return the split text as a new StringBuffer
 (const StringBuffer string)                                                               // StringBuffer
 {const typeof(makeStringBuffer()) r = makeStringBuffer();                                                                  // Results
  const typeof(string.proto->length(string)) N = string.proto->length(string);                                                          // Length of current string
  char s[N+1]; string.proto->string(string, s);                                              // Linearize current string
  size_t i = 0, j = 0;                                                          // Line start and end
  void save()                                                                   // Save a string
   {if (i > j) r.proto->addn(r, s+j, i - j);
    j = i+1;
   }
  for(; i < N; ++i) if (isspace(s[i])) save();
  save();
  return r;
 }

//D1 Statistics                                                                 // Statistics on the contents of the StringBuffer.

static size_t length_StringBuffer                                                          // Length of the string held in the buffer
 (const StringBuffer buffer)                                                               // StringBuffer
 {size_t length = 0;
  ArenaTreeNode  root = buffer.string.proto->root(buffer.string);
  ArenaTreefe(c, root) length += strlen(c.proto->key(c));
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
  ArenaTreeNode  root = buffer.string.proto->root(buffer.string);
  ArenaTreefe(c, root) p = stpcpy(p, c.proto->key(c));
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

static void apply_StringBuffer_function                                                    // Apply a function to a string.
 (const StringBuffer string,                                                               // StringBuffer
  void (*action)(char *string, size_t length))                                  // Action to apply
 {const typeof(string.proto->length(string)) length = string.proto->length(string);                                                     // Length of string
  char s[length + 1]; string.proto->string(string, s);                                       // Place string on stack
  action(s, length);                                                            // Perform action on string
 }

static  void system_StringBuffer_StringBuffer                                                         // Replace a StringBuffer containing a system command with the results of executing that command.
 (const StringBuffer    command)                                                           // StringBuffer containing command to execute
 {const typeof(makeStringBuffer()) t = makeStringBuffer();                                                                  // Temporary file name for output
  ssize_t writer(int d, char *fileName) {t.proto->add(t, fileName); return 0; if(0)d=d;}// Function to write to a file handle. Return non negative on success, negative on failure: this value will be returned to the caller.
  makeTemporaryFileWithContent("exec.txt", writer);                             // Save temporary file name
  command.proto->add(command, " 1>"); command.proto->addStringBuffer(command, t); command.proto->add(command, " 2>&1");              // Command with output capture

  void execString(char * cmd, size_t length)                                    // Execute command in string
   {system(cmd);                                                                // Execute command
    if(0)length=length;
   }

  command.proto->apply(command, execString);                                                  // Execute command in string buffer
  const typeof(t.proto->readFile(t)) r = t.proto->readFile(t);                                                             // Load the results
  r.string.proto->swap(r.string, command.string);                                              // Overwrite the input command with the results
  t.proto->free(t); r.proto->free(r);                                                           // Free file name and command
 }

//D1 Read and Write                                                             // Read/write a string buffer from/to named and temporary files.

static ssize_t writeToFileHandle                                                //P Write a StringBuffer as a string to a file handle and return the non negative number of bytes written or a negative error.
 (const StringBuffer            buffer,                                                    // StringBuffer
  const int          d,                                                         // Base name of the file
  const char * const fileName)                                                  // The name of the file being written to
 {ssize_t N = 0;
  StringBufferfe(line, buffer)
   {const typeof(line.proto->key(line)) k = line.proto->key(line);
    const typeof(strlen(k)) l = strlen(k);
    const typeof(write(d, k, l)) w = write(d, k, l);
    if (w < 0 || (size_t)w != (size_t)l)
     {printStackBackTrace
       ("Unable to write file because: %m\nFile: %s\n", fileName);
      return -1;
     }
    else N += l;
   }
  return N;
 }

static  StringBuffer writeTemporaryFile_StringBuffer_string                                           // Write a StringBuffer as a string to a temporary file with the specified base name and return the full name of the file created as a string buffer.
 (const StringBuffer            buffer,                                                    // StringBuffer content to be written
  const char * const fileName)                                                  // Base name of the file
 {const typeof(makeStringBuffer()) fullFileName = makeStringBuffer();                                                       // String buffer to record full file name of file created

  ssize_t writer(int d, char *f)                                                // Write
   {if (buffer.proto->writeToFileHandle(buffer, d, f) >= 0) fullFileName.proto->add(fullFileName, f);          // Record full file name of temporary file name created if content was written successfully
    return 0;                                                                   // Success is now determined by whether the temporary file name was created in the returned string buffer or not.
   }

  makeTemporaryFileWithContent(fileName, writer);                               // Create temporary file and write to it
  return fullFileName;
 }

static ssize_t writeFile_StringBuffer_string                                               // Write a StringBuffer as a string to the specified file
 (const StringBuffer            buffer,                                                    // StringBuffer
  const char * const fileName)                                                  // Base name of the file
 {ssize_t writer(int d)                                                         // Write
   {return buffer.proto->writeToFileHandle(buffer, d, fileName);
   }
  return writeFile(fileName, writer);                                           // Write to file and return bytes written
 }

static void writeStderr_StringBuffer                                                       //P Write a StringBuffer as a string to stderr
 (const StringBuffer buffer)                                                               // StringBuffer
 {buffer.proto->writeToFileHandle(buffer, fileno(stderr), "stderr");
 }

static StringBuffer readFile_StringBuffer_string                                                      // Read a file and returns its content as a string buffer.
 (const StringBuffer fileName)                                                             // Name of the file as the content of a string buffer
 {const typeof(makeStringBuffer()) buffer = makeStringBuffer();                                                             // Content will appear here
  ssize_t reader(char * location, size_t length)                                // File content
   {buffer.proto->addn(buffer, location, length);                                            // Place content in StringBuffer
    return 0;                                                                   // Success is now determined by the content of the StringBuffer
   }
  const typeof(fileName.proto->length(fileName)) N = fileName.proto->length(fileName);                                                        // Length of file name
  char f[N+1]; fileName.proto->string(fileName, f);
  readFile(f, reader);                                                          // Read file
  return buffer;                                                                // Content read
 }

#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TmakeStringBuffer //Tadd //TaddFormat //TaddStringBuffer //Tfree
 {const typeof(makeStringBuffer()) s = makeStringBuffer();
  s.proto->add(s, "Hello"); assert(s.proto->length(s) == 5);
  s.proto->add(s, " ");
  s.proto->addStringBuffer(s, s);
  s.proto->addFormat(s, "%s", "World");
  assert(s.proto->equalsString(s, "Hello Hello World"));
  s.proto->free(s);
 }

void test1()                                                                    //Tequals //TequalsString //Tcontains //TcontainsString //Tsubstring //TsubstringEquals //Tjoin //Tcount
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

  c.proto->join(c);
  assert(c.proto->count(c) <= 1);
  assert(c.proto->equalsString(c, "ababc"));

  a.proto->free(a); b.proto->free(b); c.proto->free(c); //A.proto->free(A); B.proto->free(B);
 }

void test2()                                                                    //TaddChar //TaddNewLine //Tb2SumW8 //TaddDoubleQuote //TaddSingleQuote
 {const typeof(makeStringBuffer()) a = makeStringBuffer();
  a.proto->addChar(a, 'a');
  a.proto->addNewLine(a);
  a.proto->addSingleQuote(a);
  a.proto->addDoubleQuote(a);
  assert(a.proto->equalsString(a, "a\n'\""));
  a.proto->free(a);
 }

void test3()                                                                    //TsplitLines //TmakeStringBufferFromString
 {const typeof(makeStringBufferFromString("a\nbb\nccc\ndddd\n")) a = makeStringBufferFromString("a\nbb\nccc\ndddd\n");
  const typeof(a.proto->splitLines(a)) s = a.proto->splitLines(a);
  const typeof(s.string.proto->first(s.string)) s1 = s.string.proto->first(s.string); assert(s1.proto->equalsString(s1, "a\n"));
  const typeof(s1.proto->next(s1)) s2 = s1.proto->next(s1);        assert(s2.proto->equalsString(s2, "bb\n"));
  const typeof(s2.proto->next(s2)) s3 = s2.proto->next(s2);        assert(s3.proto->equalsString(s3, "ccc\n"));
  const typeof(s3.proto->next(s3)) s4 = s3.proto->next(s3);        assert(s4.proto->equalsString(s4, "dddd\n"));
  a.proto->free(a); s.proto->free(s);
 }

void test4()                                                                    //TsplitWords
 {const typeof(makeStringBufferFromString("  a\nbb   ccc dddd\n\n  ")) a = makeStringBufferFromString("  a\nbb   ccc dddd\n\n  ");
  const typeof(a.proto->splitWords(a)) s = a.proto->splitWords(a);
  const typeof(s.string.proto->first(s.string)) s1 = s.string.proto->first(s.string); assert(s1.proto->equalsString(s1, "a"));
  const typeof(s1.proto->next(s1)) s2 = s1.proto->next(s1);        assert(s2.proto->equalsString(s2, "bb"));
  const typeof(s2.proto->next(s2)) s3 = s2.proto->next(s2);        assert(s3.proto->equalsString(s3, "ccc"));
  const typeof(s3.proto->next(s3)) s4 = s3.proto->next(s3);        assert(s4.proto->equalsString(s4, "dddd"));
  a.proto->free(a); s.proto->free(s);
 }

void test5()                                                                    //TreadFile //TwriteTemporaryFile
 {const typeof("a\nbb\nccc\ndddd\n") c = "a\nbb\nccc\ndddd\n";
  const typeof(makeStringBufferFromString(c)) a = makeStringBufferFromString(c);
  const typeof(a.proto->writeTemporaryFile(a, "a.txt")) f = a.proto->writeTemporaryFile(a, "a.txt");
  const typeof(f.proto->readFile(f)) b = f.proto->readFile(f);
  assert(b.proto->equalsString(b, c));
  a.proto->free(a); b.proto->free(b); f.proto->free(f);
 }

void test6()                                                                    //TwriteFile //Tlength //Tstring //Taddn
 {const typeof("a.txt") F = "a.txt";
  const typeof("a\nbb\nccc\ndddd\n") c = "a\nbb\nccc\ndddd\n";

  const typeof(makeStringBuffer()) a = makeStringBuffer();
  a.proto->addn(a, c, strlen(c));

  a.proto->writeFile(a, F);

  const typeof(makeStringBuffer()) f = makeStringBuffer(); f.proto->add(f, F);
  const typeof(f.proto->readFile(f)) b = f.proto->readFile(f);
  assert(b.proto->equalsString(b, c));
  assert(b.proto->length(b) == strlen(c));

  char C[b.proto->length(b) + 1]; b.proto->string(b, C); assert(!strcmp(c, C));

  a.proto->free(a); b.proto->free(b); f.proto->free(f); unlink(F);
 }

void test7()                                                                    //TaddVaFormat //TaddQuotedNewLine
 {const typeof(makeStringBuffer()) a = makeStringBuffer();

  void add(char *   format, ...)
   {va_list  va;
    va_start(va,    format);
    a.proto->addVaFormat(a, format, va);
    va_end  (va);
   }

  add("%s %s", "Hello", "World");
  a.proto->addQuotedNewLine(a);

  assert(a.proto->equalsString(a, "Hello World\\n"));

  a.proto->free(a);
 }

void test8()                                                                    //Tapply
 {const char * const c = "Hello World";
  const typeof(makeStringBufferFromString(c)) a = makeStringBufferFromString(c);

  void check(char * s, size_t l)
   {assert(l == strlen(c));
    assert(!strcmp(c, s));
   }

  a.proto->free(a);
 }

void test9()                                                                    //Tsystem //TmakeStringBufferVaFormat
 {const typeof(makeStringBufferFromString("uname")) a = makeStringBufferFromString("uname");
  a.proto->system(a);
  a.proto->writeStderr(a);
  assert(a.proto->containsString(a, "Linux"));
  a.proto->free(a);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4, test5,
                           test6, test7, test8, test9, 0};
  run_tests("StringBuffer", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/stringBuffer/stringBuffer
