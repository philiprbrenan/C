//------------------------------------------------------------------------------
// String buffer
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <ctype.h>
#include <arenaTree.c>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Arena Tree.

#ifndef $_included_$
#define $_included_$
typedef struct $                                                                // $
 {const struct ProtoTypes_$ *proto;                                             // Prototypes for methods
  ArenaTree     string;                                                         // String being built
 } $;
#endif

#ifndef $_included
#define $_included
#include <$$_prototypes.h>
#define $fe( child, parent) for(ArenaTreeNode child = parent.string ▷ first; child ▷ valid; child = child ▷ next) // Each child in a parent from first to last
#define $fer(child, parent) for(ArenaTreeNode child = parent.string ▷ last ; child ▷ valid; child = child ▷ prev) // Each child in a parent from last to first

$ make$                                                                         // Make a $
 ()                                                                             //
 {ArenaTree string = makeArenaTree();
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
 {const ArenaTreeNode s = buffer.string ▷ node(string);
  s ▷ putTreeLast;
 }

static void addn_$_string                                                       // Concatenate a string of known length
 (const $            buffer,                                                    // $
  const char * const string,                                                    // String
  const size_t       length)                                                    // String length
 {s ◁ buffer.string ▷ noden(string, length);
  s ▷ putTreeLast;
 }

static void add$_$_$                                                            // Add a string buffer
 (const $ buffer,                                                               // Target $
  const $ add)                                                                  // Source $
 {N ◁ add ▷ length; char a[N+1]; add ▷ string(a);
  buffer ▷ add(a);
 }

static void addVaFormat_$_string_va                                             // Add a variadic argument formatted string
 (const $ buffer,                                                               // $
  const char * const format,                                                    // Format
  va_list va1)                                                                  // Variable argument list
 {va_list va2; va_copy(va2, va1);                                               // We might need to make two passes through the variable arguments
  const int N = 1024;                                                           // Guess a reasonable size                                                                               //dx=const Xml x =  for the output string
  char data[N];                                                                 // Space on stack for first attempt
  const int L = vsnprintf(data, N, format, va1);                                // First attempt
  if (N > L + 1)                                                                // Success on the first attempt
   {const ArenaTreeNode s = buffer.string ▷ noden(data, L);
                        s ▷ putTreeLast;
   }
  else                                                                          // Second attempt
   {char data[L+1];
    vsnprintf(data, L+1, format, va2);
    const ArenaTreeNode s = buffer.string ▷ noden(data, L);
                        s ▷ putTreeLast;
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
  N ◁ old ▷ length;                                                             // Length of joined string
  char b[N+1]; old ▷ string(b);
  new ◁ make$();                                                                // New string buffer
  new ▷ add(b);                                                                 // Load new buffer from joined string
  old.string ▷ swap(new.string);                                                // Swap arenas so that the old becomes the new
  new ▷ free;                                                                   // Free old arena
 }

static StringBuffer splitLines                                                  // Split the specified $ on any new line characters and return the split text as a new $
 (const $ string)                                                               // $
 {r ◁ make$();                                                                  // Results
  N ◁ string ▷ length;                                                          // Length of current string
  char s[N+1]; string ▷ string(s);                                              // Linearize current string
  size_t i = 0, j = 0;                                                          // Line start and end
  void save()                                                                   // Save a string
   {r ▷ addn(s+j, i - j + 1);
    j = i+1;
   }
  for(; i < N; ++i) if (s[i] == '\n') save();
  if (  i > j) save();
  return r;
 }

static StringBuffer splitWords                                                  // Split the specified $ into words delimited by spaces and return the split text as a new $
 (const $ string)                                                               // $
 {r ◁ make$();                                                                  // Results
  N ◁ string ▷ length;                                                          // Length of current string
  char s[N+1]; string ▷ string(s);                                              // Linearize current string
  size_t i = 0, j = 0;                                                          // Line start and end
  void save()                                                                   // Save a string
   {if (i > j) r ▷ addn(s+j, i - j);
    j = i+1;
   }
  for(; i < N; ++i) if (isspace(s[i])) save();
  save();
  return r;
 }

//D1 Statistics                                                                 // Statistics on the contents of the $.

static size_t length_$                                                          // Length of the string held in the buffer
 (const $ buffer)                                                               // $
 {size_t length = 0;
  ArenaTreeNode  root = buffer.string ▷ root;
  ArenaTreefe(c, root) length += strlen(c ▷ key);
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
  ArenaTreeNode  root = buffer.string ▷ root;
  ArenaTreefe(c, root) p = stpcpy(p, c ▷ key);
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

static void apply_$_function                                                    // Apply a function to a string.
 (const $ string,                                                               // $
  void (*action)(char *string, size_t length))                                  // Action to apply
 {length ◁ string ▷ length;                                                     // Length of string
  char s[length + 1]; string ▷ string(s);                                       // Place string on stack
  action(s, length);                                                            // Perform action on string
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
   {k ◁ line ▷ key;
    l ◁ strlen(k);
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
  N ◁ fileName ▷ length;                                                        // Length of file name
  char f[N+1]; fileName ▷ string(f);
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
  s  ◁ a ▷ splitLines;
  s1 ◁ s.string ▷ first; assert(s1 ▷ equalsString("a\n"));
  s2 ◁ s1 ▷ next;        assert(s2 ▷ equalsString("bb\n"));
  s3 ◁ s2 ▷ next;        assert(s3 ▷ equalsString("ccc\n"));
  s4 ◁ s3 ▷ next;        assert(s4 ▷ equalsString("dddd\n"));
  a  ▷ free; s ▷ free;
 }

void test4()                                                                    //TsplitWords
 {a  ◁ make$FromString("  a\nbb   ccc dddd\n\n  ");
  s  ◁ a ▷ splitWords;
  s1 ◁ s.string ▷ first; assert(s1 ▷ equalsString("a"));
  s2 ◁ s1 ▷ next;        assert(s2 ▷ equalsString("bb"));
  s3 ◁ s2 ▷ next;        assert(s3 ▷ equalsString("ccc"));
  s4 ◁ s3 ▷ next;        assert(s4 ▷ equalsString("dddd"));
  a  ▷ free; s ▷ free;
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

void test9()                                                                    //Tsystem //Tmake$VaFormat
 {a ◁ make$FromString("uname");
  a ▷ system;
  assert(a ▷ containsString("Linux"));
  a ▷ free;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4, test5,
                           test6, test7, test8, test9, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/stringBuffer/stringBuffer
