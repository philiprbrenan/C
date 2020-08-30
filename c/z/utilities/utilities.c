//------------------------------------------------------------------------------
// Useful functions
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef $
#define $
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

#define say(args...) fprintf(stderr, args)                                      // Say something

//D1 Errors                                                                     // Methods for reporting errors

void __attribute__ ((noreturn,unused)) printStackBackTrace                      // Print a message followed by a stack back trace then exit
 (const char *format, ...)                                                      // Optional format
 {va_list arg;
  const size_t N = 100;
  void *array[N];
  char **strings;
  const size_t size = backtrace(array, N);
  strings = backtrace_symbols(array, size);
  if (format)                                                                   // Write title
   {va_start(arg, format);
    vfprintf(stderr, format, arg);
    va_end(arg);
   }
  for (size_t i = 1; i < size; i++) fprintf(stderr, "%s\n", strings[i]);        // Ignore first string as it will be this routine
  free(strings);
  exit(1);
 }

//D1 Numbers                                                                    // Numerical methods

size_t __attribute__ ((const,unused)) nextPowerOfTwo                            // Find the containing power of two and return its exponent base 2.
 (const size_t n)                                                               // Number for which next power of two is required
 {for(size_t i = 0; i <= 8*sizeof(n); ++i) if (1u << i >= n) return 1<<i;       // First containing power
  printStackBackTrace("%lu too big\n", n);
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
 {const size_t n = nextPowerOfTwo(N);
  void  *r = malloc(n);                                                         // Allocate arena
  memset(r, -1, n);                                                             // Clear memory
  return r;                                                                     // Return memory address
 }

//D1 System commands                                                            // Execute system commands

void $_system                                                                   // Execute a command via the shell
 (const char *format, ...)                                                      // Format of command
 {va_list arg;
  va_start(arg, format);
  char *c;
  vasprintf(&c, format, arg);
  va_end(arg);
  system(c);                                                                    // Execute b2sum to calculate digest with specified width
  free  (c);
 }

//D1 Testing                                                                    // Methods for testing other methods

int run_tests                                                                   // Run tests
 (char     *title,                                                              // Title of the tests
  const int repetitions,                                                        // Number of times to test
  void    (*tests[])(void))                                                     // Zero terminated array of tests functions
 {clock_t start = clock();                                                      // Start time
  int n = 0;
  for(int j = 0; j < repetitions; ++j)                                          // Test a few times
   {n = 0;
    for(int i = 0; tests[i]; ++i, ++n)                                          // Tests
     {say("%d", i);
      tests[i]();
      say(".");
     }
    say("done\n");
   }

  say("SUCCESS: All %d %s tests passed successfully in %lu microseconds\n",     // Run time
      n, title,
     (long)(((double) (clock() - start)) / CLOCKS_PER_SEC * 1000000.0));

  return n;
 }
#endif

#if __INCLUDE_LEVEL__ == 0
void test1()                                                                    //TnextPowerOfTwo //TnextMultipleOf4
 {assert(nextPowerOfTwo(0) == 1);
  assert(nextPowerOfTwo(1) == 1);
  assert(nextPowerOfTwo(2) == 2);
  assert(nextPowerOfTwo(3) == 4);
  assert(nextPowerOfTwo(4) == 4);
  assert(nextPowerOfTwo(5) == 8);

  assert(nextMultipleOf4(0) == 0);
  assert(nextMultipleOf4(1) == 4);
  assert(nextMultipleOf4(2) == 4);
  assert(nextMultipleOf4(3) == 4);
  assert(nextMultipleOf4(4) == 4);
  assert(nextMultipleOf4(5) == 8);
 }

void test2()                                                                    //Talloc
 {free(alloc(4));
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test1, test2, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/utilities/utilities
