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

//D1 Structures                                                                 // Structures describing an $.

#ifndef $_included_$
#define $_included_$
typedef struct $                                                                // $
 {const struct ProtoTypes_$ *proto;                                             // Prototypes for methods
  ArenaList     string;                                                         // String being built
 } $;
#endif

#ifndef $_included
#define $_included
#include <$$_prototypes.h>
#define $fe( child, parent) for(ArenaListNode child = parent.string ▷ first; child ▷ valid; child = child ▷ next) // Each child in a parent from first to last
#define $fer(child, parent) for(ArenaListNode child = parent.string ▷ last ; child ▷ valid; child = child ▷ prev) // Each child in a parent from last to first
#define makeLocalCopyOf$(string,length,buffer) const size_t length = length_StringBuffer(buffer); char string[length+1]; string_StringBuffer_string(buffer, string); // Load a string buffer into locally defined string/length variables.

$ make$                                                                         // Make a $
 ()                                                                             //
 {ArenaList string = makeArenaList();
  return new$(string: string);
 }

$ make$FromString                                                               // Make a $ from a string
 (const char * const string)                                                    // String
 {b ◁ make$();
  b ▷ add(string);
  return b;
 }

$ make$FromVaFormat                                                             // Make a string buffer from a variadic argument formatted string
 (const char * const format,                                                    // Format
  ...)                                                                          // Variable argument list
 {va_list va;
  va_start(va, format);
  s ◁ make$();
  s ▷ addVaFormat(format, va);
  return s;
 }

static void free_$                                                              // Free a $
 (const $ buffer)                                                               // $
 {buffer.string ▷ free;
 }

//D1 Concatenate                                                                // Concatenate various items to a string buffer.

static void add_$_string                                                        // Concatenate a string
 (const $ buffer,                                                               // $
  const char * const string)                                                    // Zero terminated string
 {s ◁ buffer.string ▷ node(string, strlen(string));
  s ▷ putTreeLast;
 }

static void addn_$_string                                                       // Concatenate a string of known length
 (const $            buffer,                                                    // $
  const char * const string,                                                    // String
  const size_t       length)                                                    // String length
 {s ◁ buffer.string ▷ node(string, length);
  s ▷ putTreeLast;
 }

static void add$_$_$                                                            // Add a string buffer
 (const $ buffer,                                                               // Target $
  const $ add)                                                                  // Source $
 {makeLocalCopyOf$(a, n, add);                                                  // Make a local copy of the buffer to be added in case the source and target buffers are the same
  buffer ▷ addn(a, n);                                                          // Add copy of source to target
 }

static void addVaFormat_$_string_va                                             // Add a variadic argument formatted string
 (const $ buffer,                                                               // $
  const char * const format,                                                    // Format
  va_list va1)                                                                  // Variable argument list
 {va_list va2; va_copy(va2, va1);                                               // We might need to make two passes through the variable arguments
  const int N = 1024;                                                           // Guess a reasonable size                                                                               //dx=const Xml x =  for the output string
  char data[N];                                                                 // Space on stack for first attempt
  const int L = vsnprintf(data, N, format, va1);                                // First attempt
  if (N > L) buffer ▷ addn(data, L);                                            // Success on the first attempt
  else                                                                          // Second attempt
   {char data[L+1];
    vsnprintf(data, L+1, format, va2);
    buffer ▷ addn(data, L);                                                     // Success on the first attempt
   }
 }

static void addFormat_$_strings                                                 // Add a formatted string
 (const $ buffer,                                                               // $
  const char * const format,                                                    // Format
  ...)                                                                          // Strings
 {va_list va;
  va_start(va, format);
  buffer ▷ addVaFormat(format, va);
  va_end(va);
 }

static void addChar_$_char                                                      // Add the specified character
 (const $    buffer,                                                            // $
  const char c)                                                                 // Character to add
 {const char s[2] = {c, 0};
  buffer ▷ add(s);
 }

static void addSpaces                                                           // Add zero or more spaces
 (const $      buffer,                                                          // $
  const size_t spaces)                                                          // Number of spaces to add
 {char s[spaces]; memset(s, ' ', spaces);
  buffer ▷ addn(s, spaces);
 }

static void addNewLine_$                                                        // Add a new line
 (const $ buffer)                                                               // $
 {buffer ▷ add("\n");
 }

static void addSingleQuote_$                                                    // Add a single quote
 (const $ buffer)                                                               // $
 {buffer ▷ add("'");
 }

static void addDoubleQuote_$                                                    // Add a double quote
 (const $ buffer)                                                               // $
 {buffer ▷ add("\"");
 }

static void addQuotedNewLine_$                                                  // Add a quoted new line
 (const $ buffer)                                                               // $
 {buffer ▷ add("\\n");
 }

//D1 Join and Split                                                             // Join all the sub strings in the strin buffer into one string. Split the contents of the string buffer on spaces or new lines.

static size_t count_$                                                           // Count the number of sub strings in the buffer
 (const $ buffer)                                                               // $
 {return buffer.string ▷ countChildren;                                         // Count the number of sub strings
 }

static void join_$                                                              // Join in place - join all the sub strings in the specified string buffer and replace them with one string.
 (const $ old)                                                                  // $
 {C ◁ old.string ▷ countChildren;                                               // Number of sub strings
  if (C <= 1) return;                                                           // Already joined
  makeLocalCopyOf$(b, N, old);                                                  // Local copy of string
  new ◁ make$();                                                                // New string buffer
  new ▷ add(b);                                                                 // Load new buffer from joined string
  old.string ▷ swap(new.string);                                                // Swap arenas so that the old becomes the new
  new ▷ free;                                                                   // Free old arena
 }

static void splitLines                                                          // Split the specified $ on any new line characters and return the split text as a new $
 (const $ string)                                                               // $
 {r ◁ make$();                                                                  // Results
  makeLocalCopyOf$(s, N, string);                                               // Local copy of string
  size_t i = 0, j = 0;                                                          // Line start and end
  void save()                                                                   // Save a string
   {r ▷ addn(s+j, i - j + 1);
    j = i+1;
   }
  for(; i < N; ++i) if (s[i] == '\n') save();
  if (  i > j) {--i; save();}
  string.string ▷ swap(r.string);                                               // Swap arenas so that the old becomes the new
  r ▷ free;
 }

static void splitWords                                                          // Split the specified $ into words delimited by spaces and return the split text as a new $
 (const $ string)                                                               // $
 {r ◁ make$();                                                                  // Results
  makeLocalCopyOf$(s, n, string);                                               // Local copy of string
  size_t i = 0, j = 0;                                                          // Line start and end
  void save()                                                                   // Save a string
   {if (i > j) r ▷ addn(s+j, i - j);
    j = i+1;
   }
  for(; i < n; ++i) if (isspace(s[i])) save();
  save();
  string.string ▷ swap(r.string);                                               // Swap arenas so that the old becomes the new
  r ▷ free;
 }

//D1 Statistics                                                                 // Statistics on the contents of the $.

static size_t length_$                                                          // Length of the string held in the buffer
 (const $ buffer)                                                               // $
 {size_t length = 0;
  ArenaListNode  root = buffer.string ▷ root;
  ArenaListfe(c, root) length += c ▷ length;
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
 {const size_t l = strlen(string);                                              // Length of comparison
  makeLocalCopyOf$(b, n, buffer);                                               // Local copy
  if (l != n) return 0;                                                         // Strings differ in length and so cannot be equal
  return !strncmp(b, string, l);                                                // Check strings are equal
 }

static int contains_$_$                                                         // Checks whether the first $ contains the second $
 (const $ a,                                                                    // First $
  const $ b)                                                                    // Second $
 {makeLocalCopyOf$(A, la, a);                                                   // Local copy of first
  makeLocalCopyOf$(B, lb, b);                                                   // Local copy of second
  if (la < lb) return 0;                                                        // Cannot be contained in a shorter string
  return !!strstr(A, B);
 }

static int containsString_$_$                                                   // Checks whether a $ contains a specified zero terminated string.
 (const $            buffer,                                                    // $
  const char * const string)                                                    // String
 {const size_t l = strlen(string);                                              // Length of string to be found
  makeLocalCopyOf$(b, lb, buffer);                                              // Local copy of string to be searched
  if (lb < l) return 0;                                                         // Cannot be contained in a shorter string
  return !!strstr(b, string);
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
  ArenaListNode  root = buffer.string ▷ root;
  ArenaListfe(c, root)
   {makeLocalCopyOfArenaListKey(k, l, c);
    p = memcpy(p, k, l); p += l;
   }
  *p = 0;
 }

static size_t substring_size_$_int_int_string                                   // Load a zero terminates string with a sub string of a $ and return the number of bytes loaded.
 (const $      buffer,                                                          // $
  const size_t start,                                                           // Offset to start of string
  const size_t length,                                                          // Length of sub string. The length of the zero terminate string to be loaded must be larger than this.
  char * const string)                                                          // String to load with enough space for the string and its terminating zero
 {makeLocalCopyOf$(s, l, buffer);                                               // Local copy
  *string        = 0;                                                           // Start with an empty output string
  if (start >= l) return 0;                                                     // Substring starts beyond the end of the string
  const size_t n = start + length < l ? length : l - start;                     // Amount we can copy
  strncpy(string, s + start, n);                                                // Copy out as much of the sub string as we can
  string[n] = 0;                                                                // Zero terminate sub string
  return n;                                                                     // Actual length of sub string
 }

static void apply_$_function                                                    // Apply a function to a string.
 (const $ string,                                                               // $
  void (*action)(char *string, size_t length))                                  // Action to apply
 {makeLocalCopyOf$(s, l, string);                                               // Local copy
  action(s, l);                                                                 // Perform action on string
 }

static  void system_$_$                                                         // Replace a $ containing a system command with the results of executing that command.
 (const $    command)                                                           // $ containing command to execute
 {t ◁ make$();                                                                  // Temporary file name for output
  ssize_t writer(int d, char *fileName) {t ▷ add(fileName); return 0; if(0)d=d;}// Function to write to a file handle. Return non negative on success, negative on failure: this value will be returned to the caller.
  makeTemporaryFileWithContent("exec.txt", writer);                             // Save temporary file name
  command ▷ add(" 1>");  command ▷ add$(t);  command ▷ add(" 2>&1");            // Command with output capture

  void execString(char * cmd, size_t length)                                    // Execute command in string
   {system(cmd);                                                                // Execute command
    if(0)length=length;
   }

  command ▷ apply(execString);                                                  // Execute command in string buffer
  r ◁ t ▷ readFile;                                                             // Load the results
  r.string ▷ swap(command.string);                                              // Overwrite the input command with the results
  t ▷ free; r ▷ free;                                                           // Free file name and command
 }

//D1 Read and Write                                                             // Read/write a string buffer from/to named and temporary files.

static ssize_t writeToFileHandle                                                //P Write a $ as a string to a file handle and return the non negative number of bytes written or a negative error.
 (const $            buffer,                                                    // $
  const int          d,                                                         // Base name of the file
  const char * const fileName)                                                  // The name of the file being written to
 {ssize_t N = 0;
  $fe(line, buffer)
   {makeLocalCopyOfArenaListKey(k, l, line);
    w ◁ write(d, k, l);
    if (w < 0 || (size_t)w != (size_t)l)
     {printStackBackTrace
       ("Unable to write file because: %m\nFile: %s\n", fileName);
      return -1;
     }
    else N += l;
   }
  return N;
 }

static void dump_$                                                              //P Write a $ to stderr.
 (const $ buffer)                                                               // $
 {say("Dump of $:\n");
  $fe(line, buffer)
   {makeLocalCopyOfArenaListKey(k, l, line);
    say("%s\n", k);
   }
 }

static void dumpHex_$                                                           //P Write a $ to stderr in hexadecimal.
 (const $ buffer)                                                               // $
 {const size_t L = 16;                                                          // Line length
  say("Dump of $ in hex:\n");
  size_t n = 0;
  $fe(line, buffer)
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
  if (!n) say("$ is empty\n");
 }

static  $ writeTemporaryFile_$_string                                           // Write a $ as a string to a temporary file with the specified base name and return the full name of the file created as a string buffer.
 (const $            buffer,                                                    // $ content to be written
  const char * const fileName)                                                  // Base name of the file
 {fullFileName ◁ make$();                                                       // String buffer to record full file name of file created

  ssize_t writer(int d, char *f)                                                // Write
   {if (buffer ▷ writeToFileHandle(d, f) >= 0) fullFileName  ▷ add(f);          // Record full file name of temporary file name created if content was written successfully
    return 0;                                                                   // Success is now determined by whether the temporary file name was created in the returned string buffer or not.
   }

  makeTemporaryFileWithContent(fileName, writer);                               // Create temporary file and write to it
  return fullFileName;
 }

static ssize_t writeFile_$_string                                               // Write a $ as a string to the specified file
 (const $            buffer,                                                    // $
  const char * const fileName)                                                  // Base name of the file
 {ssize_t writer(int d)                                                         // Write
   {return buffer ▷ writeToFileHandle(d, fileName);
   }
  return writeFile(fileName, writer);                                           // Write to file and return bytes written
 }

static void writeStderr_$                                                       //P Write a $ as a string to stderr
 (const $ buffer)                                                               // $
 {buffer ▷ writeToFileHandle(fileno(stderr), "stderr");
 }

static $ readFile_$_string                                                      // Read a file and returns its content as a string buffer.
 (const $ fileName)                                                             // Name of the file as the content of a string buffer
 {buffer ◁ make$();                                                             // Content will appear here
  ssize_t reader(char * location, size_t length)                                // File content
   {buffer ▷ addn(location, length);                                            // Place content in $
    return 0;                                                                   // Success is now determined by the content of the $
   }
  makeLocalCopyOf$(f, n, fileName);                                             // Local copy
  readFile(f, reader);                                                          // Read file
  return buffer;                                                                // Content read
 }

#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //Tmake$ //Tadd //TaddFormat //TaddStringBuffer //Tfree
 {s ◁ make$();
  s ▷ add("Hello"); assert(s ▷ length == 5);
  s ▷ add(" ");
  s ▷ addStringBuffer(s);
  s ▷ addFormat("%s", "World");
  assert(s ▷ equalsString("Hello Hello World"));
  s ▷ free;
 }

void test1()                                                                    //Tequals //TequalsString //Tcontains //TcontainsString //Tsubstring //TsubstringEquals //Tjoin //Tcount
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

  c ▷ join;
  assert(c ▷ count <= 1);
  assert(c ▷ equalsString("ababc"));

  a ▷ free; b ▷ free; c ▷ free; //A ▷ free; B ▷ free;
 }

void test2()                                                                    //TaddChar //TaddNewLine //Tb2SumW8 //TaddDoubleQuote //TaddSingleQuote
 {a ◁ make$();
  a ▷ addChar('a');
  a ▷ addNewLine;
  a ▷ addSingleQuote;
  a ▷ addDoubleQuote;
  assert(a ▷ equalsString("a\n'\""));
  a ▷ free;
 }

void test3()                                                                    //TsplitLines //Tmake$FromString
 {a  ◁ make$FromString("a\nbb\nccc\ndddd\n");
  a  ▷ splitLines;
  a1 ◁ a.string ▷ first; assert(a1 ▷ equalsString("a\n"));
  a2 ◁ a1 ▷ next;        assert(a2 ▷ equalsString("bb\n"));
  a3 ◁ a2 ▷ next;        assert(a3 ▷ equalsString("ccc\n"));
  a4 ◁ a3 ▷ next;        assert(a4 ▷ equalsString("dddd\n"));
  a  ▷ free;

  b  ◁ make$FromString("abc");
  b  ▷ splitLines;
  b1 ◁ b.string ▷ first;
  assert(b1 ▷ equalsString("abc"));
  b  ▷ free;
 }

void test4()                                                                    //TsplitWords
 {a  ◁ make$FromString("  a\nbb   ccc dddd\n\n  ");
  a  ▷ splitWords;
  a1 ◁ a.string ▷ first; assert(a1 ▷ equalsString("a"));
  a2 ◁ a1 ▷ next;        assert(a2 ▷ equalsString("bb"));
  a3 ◁ a2 ▷ next;        assert(a3 ▷ equalsString("ccc"));
  a4 ◁ a3 ▷ next;        assert(a4 ▷ equalsString("dddd"));
  a  ▷ free;
 }

void test5()                                                                    //TreadFile //TwriteTemporaryFile
 {c ◁ "a\nbb\nccc\ndddd\n";
  a ◁ make$FromString(c);
  f ◁ a ▷ writeTemporaryFile("a.txt");
  b ◁ f ▷ readFile;
  assert(b ▷ equalsString(c));
  a ▷ free; b ▷ free; f ▷ free;
 }

void test6()                                                                    //TwriteFile //Tlength //Tstring //Taddn
 {F ◁ "a.txt";
  c ◁ "a\nbb\nccc\ndddd\n";

  a ◁ make$();
  a ▷ addn(c, strlen(c));

  a ▷ writeFile(F);

  f ◁ make$(); f ▷ add(F);
  b ◁ f ▷ readFile;
  assert(b ▷ equalsString(c));
  assert(b ▷ length == strlen(c));

  char C[b ▷ length + 1]; b ▷ string(C); assert(!strcmp(c, C));

  a ▷ free; b ▷ free; f ▷ free; unlink(F);
 }

void test7()                                                                    //TaddVaFormat //TaddQuotedNewLine
 {a ◁ make$();

  void add(char *   format, ...)
   {va_list  va;
    va_start(va,    format);
    a ▷ addVaFormat(format, va);
    va_end  (va);
   }

  add("%s %s", "Hello", "World");
  a ▷ addQuotedNewLine;

  assert(a ▷ equalsString("Hello World\\n"));

  a ▷ free;
 }

void test8()                                                                    //Tapply
 {const char * const c = "Hello World";
  a ◁ make$FromString(c);
  void check(char * s, size_t l)
   {assert(l == strlen(c));
    assert(!strcmp(c, s));
   }

  a ▷ free;
 }

void test9()                                                                    //TaddSpaces
 {a ◁ make$();
  a ▷ add("a"); a ▷ addSpaces(2); a ▷ add("A");
  makeLocalCopyOf$(s, l, a);
  assert(!strcmp(s, "a  A"));
  a ▷ free;
 }

void test10()                                                                   //Tsystem //Tmake$VaFormat
 {a ◁ make$FromString("uname");
  a ▷ system;
  assert(a ▷ containsString("Linux"));
  a ▷ free;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4, test5,
                           test6, test7, test8, test9, test10, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/stringBuffer/stringBuffer
