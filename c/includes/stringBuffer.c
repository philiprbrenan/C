#line 1 "/home/phil/c/z/stringBuffer/stringBuffer.c"
//------------------------------------------------------------------------------
// String buffer - a string made of a list of sub strings.
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
/*
 A string made up of a list of sub strings.
*/
#define _GNU_SOURCE
#include <ctype.h>
#include <arenaList.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an StringBuffer.

#ifndef StringBuffer_included_StringBuffer
#define StringBuffer_included_StringBuffer
typedef struct StringBuffer                                                                // StringBuffer
 {const struct ProtoTypes_StringBuffer *proto;                                             // Prototypes for methods
  ArenaList     string;                                                         // String being built
 } StringBuffer;
#endif

#ifndef StringBuffer_included
#define StringBuffer_included
#include <stringBuffer_prototypes.h>
#define StringBufferfe( child, parent) for(ArenaListNode child = parent.string.proto->first(parent.string); child.proto->valid(child); child = child.proto->next(child)) // Each child in a parent from first to last
#define StringBufferfer(child, parent) for(ArenaListNode child = parent.string.proto->last(parent.string) ; child.proto->valid(child); child = child.proto->prev(child)) // Each child in a parent from last to first
#define makeLocalCopyOfStringBuffer(string,length,buffer) const size_t length = length_StringBuffer(buffer); char string[length+1]; string_StringBuffer_string(buffer, string); // Load a string buffer into locally defined string/length variables.

StringBuffer makeStringBuffer                                                                         // Make a StringBuffer
 ()                                                                             //
 {ArenaList string = makeArenaList();
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
 {const typeof(buffer.string.proto->node(buffer.string, string, strlen(string))) s = buffer.string.proto->node(buffer.string, string, strlen(string));
  s.proto->putTreeLast(s);
 }

static void addn_StringBuffer_string                                                       // Concatenate a string of known length
 (const StringBuffer            buffer,                                                    // StringBuffer
  const char * const string,                                                    // String
  const size_t       length)                                                    // String length
 {const typeof(buffer.string.proto->node(buffer.string, string, length)) s = buffer.string.proto->node(buffer.string, string, length);
  s.proto->putTreeLast(s);
 }

static void addStringBuffer_StringBuffer_StringBuffer                                                            // Add a string buffer
 (const StringBuffer buffer,                                                               // Target StringBuffer
  const StringBuffer add)                                                                  // Source StringBuffer
 {makeLocalCopyOfStringBuffer(a, n, add);                                                  // Make a local copy of the buffer to be added in case the source and target buffers are the same
  buffer.proto->addn(buffer, a, n);                                                          // Add copy of source to target
 }

static void addVaFormat_StringBuffer_string_va                                             // Add a variadic argument formatted string
 (const StringBuffer buffer,                                                               // StringBuffer
  const char * const format,                                                    // Format
  va_list va1)                                                                  // Variable argument list
 {va_list va2; va_copy(va2, va1);                                               // We might need to make two passes through the variable arguments
  const int N = 1024;                                                           // Guess a reasonable size                                                                               //dx=const Xml x =  for the output string
  char data[N];                                                                 // Space on stack for first attempt
  const int L = vsnprintf(data, N, format, va1);                                // First attempt
  if (N > L) buffer.proto->addn(buffer, data, L);                                            // Success on the first attempt
  else                                                                          // Second attempt
   {char data[L+1];
    vsnprintf(data, L+1, format, va2);
    buffer.proto->addn(buffer, data, L);                                                     // Success on the first attempt
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

static void addSpaces                                                           // Add zero or more spaces
 (const StringBuffer      buffer,                                                          // StringBuffer
  const size_t spaces)                                                          // Number of spaces to add
 {char s[spaces]; memset(s, ' ', spaces);
  buffer.proto->addn(buffer, s, spaces);
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
 {const typeof(old.string.proto->countChildren(old.string)) N = old.string.proto->countChildren(old.string);                                               // Number of sub strings
  if (N <= 1) return;                                                           // Already joined
  makeLocalCopyOfStringBuffer(b, l, old);                                                  // Local copy of string
  const typeof(makeStringBuffer()) new = makeStringBuffer();                                                                // New string buffer
  new.proto->addn(new, b, l);                                                             // Load new buffer from joined string
  old.proto->swap(old, new);                                                              // Swap arenas so that the old becomes the new
 }

static void joinWith_StringBuffer                                                          // Join in place - join all the sub strings in the specified string buffer with the specified string and replace the string buffer with the result.
 (const StringBuffer old,                                                                  // StringBuffer
  const char * const string)                                                    // String to separate the items being joined
 {const typeof(strlen(string)) S = strlen(string);                                                           // Length of joining string
  if (S < 1) {old.proto->join(old); return;}                                              // Empty joining string
  const typeof(old.string.proto->countChildren(old.string)) N = old.string.proto->countChildren(old.string);                                               // Number of sub strings
  if (N <= 1) return;                                                           // Already joined
  const typeof(makeStringBuffer()) new = makeStringBuffer();                                                                // New string buffer
  typeof(0ul) n = 0ul;
  StringBufferfe(word, old)
   {makeLocalCopyOfArenaListKey(w, l, word);                                    // Local copy of word
    new.proto->addn(new, w, l);                                                            // Copy word
    if (++n < N) new.proto->addn(new, string, S);                                         // Add joining string
   }
  new.proto->join(new);                                                                   // Join
  old.proto->swap(old, new);                                                              // Swap arenas so that the old becomes the new
 }

static void splitLines                                                          // Split the specified StringBuffer on any new line characters and return the split text as a new StringBuffer
 (const StringBuffer string)                                                               // StringBuffer
 {const typeof(makeStringBuffer()) r = makeStringBuffer();                                                                  // Results
  makeLocalCopyOfStringBuffer(s, N, string);                                               // Local copy of string
  size_t i = 0, j = 0;                                                          // Line start and end
  void save()                                                                   // Save a string
   {r.proto->addn(r, s+j, i - j + 1);
    j = i+1;
   }
  for(; i < N; ++i) if (s[i] == '\n') save();
  if (  i > j) {--i; save();}
  string.proto->swap(string, r);                                                             // Swap arenas so that the old becomes the new
 }

static void splitWords                                                          // Split the specified StringBuffer into words delimited by spaces and return the split text as a new StringBuffer
 (const StringBuffer string)                                                               // StringBuffer
 {const typeof(makeStringBuffer()) r = makeStringBuffer();                                                                  // Results
  makeLocalCopyOfStringBuffer(s, n, string);                                               // Local copy of string
  size_t i = 0, j = 0;                                                          // Line start and end
  void save()                                                                   // Save a string
   {if (i > j) r.proto->addn(r, s+j, i - j);
    j = i+1;
   }
  for(; i < n; ++i) if (isspace(s[i])) save();
  save();
  string.proto->swap(string, r);                                                             // Swap arenas so that the old becomes the new
 }

//D1 Statistics                                                                 // Statistics on the contents of the StringBuffer.

static size_t length_StringBuffer                                                          // Length of the string held in the buffer
 (const StringBuffer buffer)                                                               // StringBuffer
 {size_t length = 0;
  ArenaListNode  root = buffer.string.proto->root(buffer.string);
  ArenaListfe(c, root) length += c.proto->length(c);
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
 {const size_t l = strlen(string);                                              // Length of comparison
  makeLocalCopyOfStringBuffer(b, n, buffer);                                               // Local copy
  if (l != n) return 0;                                                         // Strings differ in length and so cannot be equal
  return !strncmp(b, string, l);                                                // Check strings are equal
 }

static int contains_StringBuffer_StringBuffer                                                         // Checks whether the first StringBuffer contains the second StringBuffer
 (const StringBuffer a,                                                                    // First StringBuffer
  const StringBuffer b)                                                                    // Second StringBuffer
 {makeLocalCopyOfStringBuffer(A, la, a);                                                   // Local copy of first
  makeLocalCopyOfStringBuffer(B, lb, b);                                                   // Local copy of second
  if (la < lb) return 0;                                                        // Cannot be contained in a shorter string
  return !!strstr(A, B);
 }

static int containsString_StringBuffer_StringBuffer                                                   // Checks whether a StringBuffer contains a specified zero terminated string.
 (const StringBuffer            buffer,                                                    // StringBuffer
  const char * const string)                                                    // String
 {const size_t l = strlen(string);                                              // Length of string to be found
  makeLocalCopyOfStringBuffer(b, lb, buffer);                                              // Local copy of string to be searched
  if (lb < l) return 0;                                                         // Cannot be contained in a shorter string
  return !!strstr(b, string);
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
  ArenaListNode  root = buffer.string.proto->root(buffer.string);
  ArenaListfe(c, root)
   {makeLocalCopyOfArenaListKey(k, l, c);
    p = memcpy(p, k, l); p += l;
   }
  *p = 0;
 }

static size_t substring_size_StringBuffer_int_int_string                                   // Load a zero terminates string with a sub string of a StringBuffer and return the number of bytes loaded.
 (const StringBuffer      buffer,                                                          // StringBuffer
  const size_t start,                                                           // Offset to start of string
  const size_t length,                                                          // Length of sub string. The length of the zero terminate string to be loaded must be larger than this.
  char * const string)                                                          // String to load with enough space for the string and its terminating zero
 {makeLocalCopyOfStringBuffer(s, l, buffer);                                               // Local copy
  *string        = 0;                                                           // Start with an empty output string
  if (start >= l) return 0;                                                     // Substring starts beyond the end of the string
  const size_t n = start + length < l ? length : l - start;                     // Amount we can copy
  strncpy(string, s + start, n);                                                // Copy out as much of the sub string as we can
  string[n] = 0;                                                                // Zero terminate sub string
  return n;                                                                     // Actual length of sub string
 }

static void apply_StringBuffer_function                                                    // Apply a function to a string.
 (const StringBuffer string,                                                               // StringBuffer
  void (*action)(char *string, size_t length))                                  // Action to apply
 {makeLocalCopyOfStringBuffer(s, l, string);                                               // Local copy
  action(s, l);                                                                 // Perform action on string
 }

static  void system_StringBuffer_StringBuffer                                                         // Replace a StringBuffer containing a system command with the results of executing that command.
 (const StringBuffer    command)                                                           // StringBuffer containing command to execute
 {const typeof(makeStringBuffer()) t = makeStringBuffer();                                                                  // Temporary file name for output
  ssize_t writer(int d, char *fileName) {t.proto->add(t, fileName); return 0; if(0)d=d;}// Function to write to a file handle. Return non negative on success, negative on failure: this value will be returned to the caller.
  makeTemporaryFileWithContent("exec.txt", writer);                             // Save temporary file name
  command.proto->add(command, " 1>");  command.proto->addStringBuffer(command, t);  command.proto->add(command, " 2>&1");            // Command with output capture

  void execString(char * cmd, size_t length)                                    // Execute command in string
   {system(cmd);                                                                // Execute command
    if(0)length=length;
   }

  command.proto->apply(command, execString);                                                  // Execute command in string buffer
  t.proto->readFile(t);                                                             // Load the results
  command.proto->swap(command, t);                                                            // Overwrite the input command with the results
 }

//D1 Read and Write                                                             // Read/write a string buffer from/to named and temporary files.

static ssize_t writeToFileHandle                                                //P Write a StringBuffer as a string to a file handle and return the non negative number of bytes written or a negative error.
 (const StringBuffer            buffer,                                                    // StringBuffer
  const int          d,                                                         // Base name of the file
  const char * const fileName)                                                  // The name of the file being written to
 {ssize_t N = 0;
  StringBufferfe(line, buffer)
   {makeLocalCopyOfArenaListKey(k, l, line);
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

static void dump_StringBuffer                                                              //P Write a StringBuffer to stderr.
 (const StringBuffer buffer)                                                               // StringBuffer
 {say("Dump of StringBuffer:\n");
  StringBufferfe(line, buffer)
   {makeLocalCopyOfArenaListKey(k, l, line);
    say("%s\n", k);
   }
 }

static void dumpHex_StringBuffer                                                           //P Write a StringBuffer to stderr in hexadecimal.
 (const StringBuffer buffer)                                                               // StringBuffer
 {const size_t L = 16;                                                          // Line length
  say("Dump of StringBuffer in hex:\n");
  size_t n = 0;
  StringBufferfe(line, buffer)
   {makeLocalCopyOfArenaListKey(k, l, line);
    char a[L+1]; memset(a, 0, L);
    for(size_t i = 0; i < l; ++i)
     {if (i && i % 4  == 0)  say("  ");
      say("%02x ", (unsigned int)k[i]);
      a[i % L] = isspace(k[i]) ? ' ' : k[i];
      if (i && i % 16 == 0) {say("=%s=\n", a); memset(a, 0, L);}
      ++n;
     }
    if (a[0]) say("=%s=\n", a);
   }
  if (!n) say("StringBuffer is empty\n");
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

static void readFile_StringBuffer_string                                                   // Read a file and returns its content as a string buffer.
 (const StringBuffer fileName)                                                             // Name of the file as the content of a string buffer
 {const typeof(makeStringBuffer()) buffer = makeStringBuffer();                                                             // Content will appear here
  ssize_t reader(char * location, size_t length)                                // File content
   {buffer.proto->addn(buffer, location, length);                                            // Place content in StringBuffer
    return 0;                                                                   // Success is now determined by the content of the StringBuffer
   }
  makeLocalCopyOfStringBuffer(f, n, fileName);                                             // Local copy
  readFile(f, reader);                                                          // Read file
  fileName.proto->swap(fileName, buffer);                                                      // Replace file name with contents of file name
 }

//D1 Swap                                                                       // Swap two string buffers

static void swap_StringBuffer                                                              // Swap two string buffers and free the new one so that the old one is renewed.
 (const StringBuffer old,                                                                  // StringBuffer Old
  const StringBuffer new)                                                                  // StringBuffer New
 {old.string.proto->swap(old.string, new.string);                                                // Swap arenas so that the old becomes the new
  new.proto->free(new);
 }

#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TmakeStringBuffer //Tadd //TaddFormat //TaddStringBuffer //Tfree
 {const typeof(makeStringBuffer()) s = makeStringBuffer();
  s.proto->add(s, "Hello"); assert( s.proto->length(s) == 5);
  s.proto->add(s, " ");
  s.proto->addStringBuffer(s, s);
  s.proto->addFormat(s, "%s", "World");
  assert( s.proto->equalsString(s, "Hello Hello World"));
  s.proto->free(s);
 }

void test1()                                                                    //Tequals //TequalsString //Tcontains //TcontainsString //Tsubstring //TsubstringEquals //Tjoin //Tcount //TjoinWith
 {StringBuffer a = makeStringBuffer();   StringBuffer b = makeStringBuffer(); StringBuffer c = makeStringBuffer();
    a.proto->add(a, "ab");   b.proto->add(b, "a");  c.proto->add(c, "aba");
    a.proto->add(a, "c");    b.proto->add(b, "bc"); c.proto->add(c, "bc");

  assert(  a.proto->equals(a, b));
  assert(  a.proto->equalsString(a, "abc"));
  assert( !a.proto->equals(a, c));
  assert(  c.proto->contains(c, a));
  assert( !a.proto->contains(a, c));
  assert(  c.proto->containsString(c, "bab"));

  assert(  c.proto->substringEquals(c, 1, 0, ""));
  assert(  c.proto->substringEquals(c, 1, 1, "b"));
  assert(  c.proto->substringEquals(c, 1, 2, "ba"));
  assert(  c.proto->substringEquals(c, 1, 3, "bab"));
  assert(  c.proto->substringEquals(c, 1, 4, "babc"));
  assert(  c.proto->substringEquals(c, 1, 5, "babc"));

  assert(  c.proto->substringEquals(c, 5, 0, ""));
  assert(  c.proto->substringEquals(c, 5, 1, ""));

  if (1)
   {char buffer[16];
    c.proto->substring(c, 1, 3, buffer);
    assert( !strcmp(buffer, "bab"));
   }

  c.proto->join(c);
  assert( c.proto->count(c) == 1);
  assert( c.proto->equalsString(c, "ababc"));

  b.proto->joinWith(b, "--");
  assert( b.proto->count(b) == 1);
  assert( b.proto->equalsString(b, "a--bc"));

  a.proto->free(a); b.proto->free(b); c.proto->free(c);
 }

void test2()                                                                    //TaddChar //TaddNewLine //Tb2SumW8 //TaddDoubleQuote //TaddSingleQuote
 {const typeof(makeStringBuffer()) a = makeStringBuffer();
  a.proto->addChar(a, 'a');
  a.proto->addNewLine(a);
  a.proto->addSingleQuote(a);
  a.proto->addDoubleQuote(a);
  assert( a.proto->equalsString(a, "a\n'\""));
  a.proto->free(a);
 }

void test3()                                                                    //TsplitLines //TmakeStringBufferFromString
 {const typeof(makeStringBufferFromString("a\nbb\nccc\ndddd\n")) a = makeStringBufferFromString("a\nbb\nccc\ndddd\n");
  a.proto->splitLines(a);
  const typeof(a.string.proto->first(a.string)) a1 = a.string.proto->first(a.string); assert( a1.proto->equalsString(a1, "a\n"));
  const typeof(a1.proto->next(a1)) a2 = a1.proto->next(a1);        assert( a2.proto->equalsString(a2, "bb\n"));
  const typeof(a2.proto->next(a2)) a3 = a2.proto->next(a2);        assert( a3.proto->equalsString(a3, "ccc\n"));
  const typeof(a3.proto->next(a3)) a4 = a3.proto->next(a3);        assert( a4.proto->equalsString(a4, "dddd\n"));
  a.proto->free(a);

  const typeof(makeStringBufferFromString("abc")) b = makeStringBufferFromString("abc");
  b.proto->splitLines(b);
  const typeof(b.string.proto->first(b.string)) b1 = b.string.proto->first(b.string);
  assert( b1.proto->equalsString(b1, "abc"));
  b.proto->free(b);
 }

void test4()                                                                    //TsplitWords
 {const typeof(makeStringBufferFromString("  a\nbb   ccc dddd\n\n  ")) a = makeStringBufferFromString("  a\nbb   ccc dddd\n\n  ");
  a.proto->splitWords(a);
  const typeof(a.string.proto->first(a.string)) a1 = a.string.proto->first(a.string); assert( a1.proto->equalsString(a1, "a"));
  const typeof(a1.proto->next(a1)) a2 = a1.proto->next(a1);        assert( a2.proto->equalsString(a2, "bb"));
  const typeof(a2.proto->next(a2)) a3 = a2.proto->next(a2);        assert( a3.proto->equalsString(a3, "ccc"));
  const typeof(a3.proto->next(a3)) a4 = a3.proto->next(a3);        assert( a4.proto->equalsString(a4, "dddd"));
  a.proto->free(a);
 }

void test5()                                                                    //TreadFile //TwriteTemporaryFile
 {  const typeof("a\nbb\nccc\ndddd\n") c = "a\nbb\nccc\ndddd\n";
    const typeof(makeStringBufferFromString(c)) a = makeStringBufferFromString(c);
    const typeof(a.proto->writeTemporaryFile(a, "a.txt")) f = a.proto->writeTemporaryFile(a, "a.txt");
    f.proto->readFile(f);
  assert( f.proto->equalsString(f, c));
  a.proto->free(a); f.proto->free(f);
 }

void test6()                                                                    //TwriteFile //Tlength //Tstring //Taddn
 {  const typeof("a.txt") F = "a.txt";
    const typeof("a\nbb\nccc\ndddd\n") c = "a\nbb\nccc\ndddd\n";

    const typeof(makeStringBuffer()) a = makeStringBuffer();
    a.proto->addn(a, c, strlen(c));

    a.proto->writeFile(a, F);

    const typeof(makeStringBuffer()) f = makeStringBuffer(); f.proto->add(f, F);
    f.proto->readFile(f);
  assert( f.proto->equalsString(f, c));
  assert( f.proto->length(f) == strlen(c));

  char C[f.proto->length(f) + 1]; f.proto->string(f, C); assert( !strcmp(c, C));

  a.proto->free(a); f.proto->free(f); unlink(F);
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

  assert( a.proto->equalsString(a, "Hello World\\n"));

  a.proto->free(a);
 }

void test8()                                                                    //Tapply
 {const char * const c = "Hello World";
  const typeof(makeStringBufferFromString(c)) a = makeStringBufferFromString(c);
  void check(char * s, size_t l)
   {assert( l == strlen(c));
    assert( !strcmp(c, s));
   }

  a.proto->free(a);
 }

void test9()                                                                    //TaddSpaces
 {const typeof(makeStringBuffer()) a = makeStringBuffer();
  a.proto->add(a, "a"); a.proto->addSpaces(a, 2); a.proto->add(a, "A");
  makeLocalCopyOfStringBuffer(s, l, a);
  assert( !strcmp(s, "a  A"));
  a.proto->free(a);
 }

void test10()                                                                   //Tsystem //TmakeStringBufferVaFormat
 {const typeof(makeStringBufferFromString("uname")) a = makeStringBufferFromString("uname");
  a.proto->system(a);
  assert( a.proto->containsString(a, "Linux"));
  a.proto->free(a);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4, test5,
                           test6, test7, test8, test9, test10, 0};
  run_tests("StringBuffer", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/stringBuffer/stringBuffer
