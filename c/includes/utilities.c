#line 1 "/home/phil/c/z/utilities/utilities.c"
//------------------------------------------------------------------------------
// Useful functions
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef Utilities_included
#define Utilities_included
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>

#define say(format...) ({fprintf(stdout, format); fprintf(stdout, "\n"); fflush(stdout);}) // Say something on stdout using the specified format
#define err(format...) ({fprintf(stderr, format); fprintf(stderr, "\n"); fflush(stderr);}) // Say something on stderr using the specified format

// Create a local string with the specified name from the specified format assuming that most of the time it will have less than the specified number of characters (N).
#define lsprintf(a,N,format...) char a##c[N+1]; const size_t a##l = snprintf(a##c, N, format); char a[a##l+1]; if (a##l > N) sprintf(a, format); else memcpy(a, a##c, a##l+1);
//Faster, but does not pass valgrind
//#define lsprintf(a,N,format...) char a##c[N+1]; const size_t a##l = snprintf(a##c, N, format); char *a = a##c; if (a##l > N) {char a##d[a##l+1]; a = a##d;sprintf(a, format);}

//D1 Errors                                                                     // Methods for reporting errors

void printStackBackTraceDetails()                                               //P Print a stack back trace.
 {const size_t N = 100;
  void *array[N];
  char **strings;
  typeof(backtrace(array, N)) size = backtrace(array, N);
  strings = backtrace_symbols(array, size);
  for (typeof(size) i = 2; i < size; i++) fprintf(stderr, "%s\n", strings[i]);  // Ignore first two strings as they will be this routine plus its immediate caller
  free(strings);
 }

void __attribute__ ((unused)) printStackBackTrace                               // Print a message followed by a stack back trace.
 (const char *format,                                                           // Message format
  ...)                                                                          // Message elements
 {va_list arg;
  va_start(arg, format);
  vfprintf(stderr, format, arg);                                                // Print message
  va_end(arg);
  printStackBackTraceDetails();                                                 // Print stack back trace.
 }

void __attribute__ ((noreturn,unused)) printStackBackTraceAndExit               // Print a message followed by a stack back trace the exit with the specified code
 (const int   code,                                                             // Exit code
  const char *format,                                                           // Message format
  ...)                                                                          // Message elements
 {va_list arg;
  va_start(arg, format);
  vfprintf(stderr, format, arg);                                                // Print message
  va_end(arg);
  printStackBackTraceDetails();                                                 // Print stack back trace.
  exit(code);
 }

void __attribute__ ((unused)) assertNot                                         // Invert the condition in assert
 (const int   condition)                                                        // Condition to test
 {assert(!condition);
 }

size_t __attribute__ ((unused)) topOfStack()                                    // Return the address of the current top of the stack
 {char a;
  return (size_t)(&a-0);
 }

//D1 Numbers                                                                    // Numerical methods

size_t __attribute__ ((const,unused)) nextPowerOfTwo                            // The next power of two greater than or equal to the specified number
 (const size_t n)                                                               // Number
 {for(size_t i = 0; i <= 8*sizeof(n); ++i) if (1u << i >= n) return 1<<i;       // First containing power of two
  printStackBackTraceAndExit(1, "%lu too big\n", n);                            // Too big
  return 0;
 }

size_t __attribute__ ((const,unused)) exponentOfNextPowerOfTwo                  // The exponent of next power of two greater than or equal to the specified number
 (const size_t n)                                                               // Number
 {for(size_t i = 0; i <= 8*sizeof(n); ++i) if (1u << i >= n) return i;          // Exponent
  printStackBackTraceAndExit(1, "%lu too big\n", n);                            // Too big
  return 0;
 }

size_t __attribute__ ((const,unused)) nextMultipleOf4                           // Round up to the next multiple of 4
 (const size_t n)                                                               // Number for which next power of two is required
 {const char r = n % 4;
  if (r == 0) return n;
  return n + 4 - r;
 }

//D1 Allocation                                                                 // Methods for allocating storage

void * __attribute__ ((unused)) alloc                                           // Allocate a block of memory that is the next power of two in size
 (const size_t N)                                                               // Amount of memory requested
 {const typeof(nextPowerOfTwo(N)) n = nextPowerOfTwo(N);                                                        // Round up allocations to reduce fragmentation
  void  *r = malloc(n);                                                         // Allocate arena
  memset(r, -1, n);                                                             // Clear memory to avoid complaints from valgrind
  return r;                                                                     // Return memory address
 }

//D1 System commands                                                            // Execute system commands

void execSystemCommand                                                          // Execute a command via the shell
 (const char *format, ...)                                                      // Format of command
 {va_list arg;
  va_start(arg, format);
  char *c;
  vasprintf(&c, format, arg);
  va_end(arg);
  system(c);
  free  (c);
 }

//D1 Files                                                                      // Read, write and other operations on files

ssize_t __attribute__ ((unused)) readFile                                       // Map the specified file then pass its location and length to the specified reader function.  Return non negative on success, negative on failure after writing a stack back trace.
 (const char * const file,                                                      // File name
  ssize_t (*reader)(char * location, size_t length))                            // Function to process the mapped file. Return non negative on success, negative on failure: this value will be returned to the caller. The file is mapped into an area at least one byte larger than the file size so that the content of the file appears to have a trailing zero.
 {struct stat buf;
  if (stat(file, &buf))                                                         // Get file size
   {printStackBackTrace("Unable to stat file because %m, file:\n%s\n", file);
    return -1;
   }
  const typeof(buf.st_size) size = buf.st_size;                                                           // Length of file content

  const typeof(open(file, O_RDONLY)) i = open(file, O_RDONLY);                                                     // Open file for read
  if (i < 0)
   {printStackBackTrace("Unable to open file because %m, file:\n%s\n", file);
    return -2;
   }

  char * const s = mmap(NULL, size + 1, PROT_READ, MAP_PRIVATE, i, 0);          // Add a terminating zero byte - mmap sets unused bytes beyond the file to zero giving us a zero terminated string.
  if (!s)                                                                       // Unsuccessful map
   {printStackBackTrace("Unable to map file because %m, file:\n%s\n", file);
    close(i);
    return -3;
   }
  const typeof(reader(s, size)) r = reader(s, size);                                                          // Read data per caller

  close(i);                                                                     // Close file
  return r;
 }

ssize_t __attribute__ ((unused)) makeTemporaryFileWithContent                   // Create a temporary file with the specified base name and extension then load it via the specified function.  Return non negative on success, negative on failure after writing a stack back trace.
 (const char * const fileName,                                                  // Base name and extension of the file
  ssize_t (*writer)(int d, char * fileName))                                    // Function to write to a file handle. Return non negative on success, negative on failure: this value will be returned to the caller.
 {lsprintf(dir, 1024, "/tmp/%d/", getpid());                                    // Create a temporary folder for this process
  const typeof(mkdir(dir, S_IRWXU)) m = mkdir(dir, S_IRWXU);                                                      // Create folder
  if (m && errno != EEXIST)                                                     // Its ok if the temporary folder already exists
   {printStackBackTrace
     ("Unable to create directory because %m, directory:\n%s\n", dir);
    return -1;
   }

  lsprintf(file, 1024, "%s%s", dir, fileName);                                  // File name
  const typeof(open(file, O_WRONLY|O_CREAT, S_IRWXU)) o = open(file, O_WRONLY|O_CREAT, S_IRWXU);                                    // Create file
  if (o < 0)
   {printStackBackTrace("Unable to open file because %m, file:\n%s\n", file);
    return -2;
   }
  const typeof(writer(o, file)) r = writer(o, file);                                                          // Write data per caller

  close(o);                                                                     // Close temporary file
  return r;
 }

ssize_t __attribute__ ((unused)) writeFile                                      // Write to the specified file via the specified function. Returns non negative  on success, negative on failure.
 (const char * const file,                                                      // Base name and extension of the file
  ssize_t (*writer)(int d))                                                     // Function to write to a file handle. Return non negative on success, negative on failure: this value will be returned to the caller.
 {const typeof(open(file, O_WRONLY|O_CREAT, S_IRWXU)) o = open(file, O_WRONLY|O_CREAT, S_IRWXU);                                    // Create file
  if (o < 0)
   {printStackBackTrace("Unable to open file because %m, file:\n%s\n", file);
    return -1;
   }

  const typeof(writer(o)) r = writer(o);                                                                // Write data per caller

  close(o);                                                                     // Close temporary file
  return r;
 }

//D1 Testing                                                                    // Methods for testing other methods

static int developmentMode()                                                    // Test whether we are local or on github
 {return !strcmp(getenv("HOME"), "/home/phil");
 }

long microSecondsSince                                                          // Microseconds since the specified time
 (clock_t start)                                                                // Start time
 {return (long)(((double) (clock() - start)) / CLOCKS_PER_SEC * 1000000.0);
 }

int run_tests                                                                   // Run tests
 (char     *title,                                                              // Title of the tests
  const int repetitions,                                                        // Number of times to test
  void    (*tests[])(void))                                                     // Zero terminated array of tests functions
 {clock_t start = clock();                                                      // Start time
  int n = 0;
  for(int j = 0; j < repetitions; ++j)                                          // Test a few times
   {n = 0;
    say("\n");
    for(int i = 0; tests[i]; ++i, ++n)                                          // Tests
     {printf("Test: %2d ", i);
      tests[i]();
      say(" - done");
     }
   }

  say("SUCCESS: All %d %s tests passed successfully in %lu microseconds",       // Run time
      n, title, microSecondsSince(start));

  return n;
 }
#endif

#if __INCLUDE_LEVEL__ == 0
void test1()                                                                    //TnextPowerOfTwo //TnextMultipleOf4 //TexponentOfNextPowerOfTwo
 {assert( nextPowerOfTwo(0) == 1);
  assert( nextPowerOfTwo(1) == 1);
  assert( nextPowerOfTwo(2) == 2);
  assert( nextPowerOfTwo(3) == 4);
  assert( nextPowerOfTwo(4) == 4);
  assert( nextPowerOfTwo(5) == 8);

  assert( exponentOfNextPowerOfTwo(0) == 0);
  assert( exponentOfNextPowerOfTwo(1) == 0);
  assert( exponentOfNextPowerOfTwo(2) == 1);
  assert( exponentOfNextPowerOfTwo(3) == 2);
  assert( exponentOfNextPowerOfTwo(4) == 2);
  assert( exponentOfNextPowerOfTwo(5) == 3);

  assert( nextMultipleOf4(0) == 0);
  assert( nextMultipleOf4(1) == 4);
  assert( nextMultipleOf4(2) == 4);
  assert( nextMultipleOf4(3) == 4);
  assert( nextMultipleOf4(4) == 4);
  assert( nextMultipleOf4(5) == 8);
 }

void test2()                                                                    //Talloc
 {free(alloc(4));
  char a[4]; a[0] = 'a'; a[1] = 'b'; a[2] = 0;
  lsprintf(aa, 2, "%s%s", a, a);
  assert( !strcmp(aa, "abab"));
   lsprintf(bb, 5, "%s%s", a, a);
  assert( !strcmp(bb, "abab"));
 }

void test3()                                                                    //TreadFile //TwriteFile
 {const typeof("abcd") data = "abcd";                                                                // String to write then read
  char *file;                                                                   // File written to

  ssize_t w(int d, char *f)                                                     // Write
   {file = strdup(f);
    return write(d, data, strlen(data));
   }
  assert( makeTemporaryFileWithContent("a.txt", w) >= 0);

  ssize_t r(char * location, size_t length)                                     // Read
   {assert( length == strlen(data));
    assert( !strcmp(data, location));
    return 1;
   }

  readFile(file, r);
  free(file);
 }

void test4()                                                                    //TtopOfStack
 {size_t a, b, c;

  void function(size_t * p, int depth)
   {char a[100]; memset(a, 0, sizeof(a));
    if (!depth) *p = topOfStack(); else function(p, depth-1);
   }

  function(&a,1); function(&b, 5); function(&c, 9);
  assert( a > b); assert( b > c);                                                             // The stack grows downward
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test1, test2, test3, test4, 0};
  run_tests("Utilities", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/utilities/utilities
