//------------------------------------------------------------------------------
// File names
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#ifndef $_included
#define $_included
#include <unistd.h>
#include <utilities.c>

typedef struct $                                                                // File names (of limited length)
 {const struct ProtoTypes_$ *proto;                                             // Methods
  char name[256];
 } $;

#include <$$_prototypes.h>                                                      // Arena tree prototypes now that the relevant structures have been declared

//D1 Files                                                                      // Methods for processing files

static size_t size                                                              // Get the size of a file in bytes
 ($ file)                                                                       // Name of the file
 {struct stat buf;
  if (!stat(file.name, &buf)) return buf.st_size;
  return 0;
 }

static void writeContentToOpenFile                                              //P Write content to an open file descriptor unti it is either all written or an error occurs.
 (const $   file,                                                               // Name of the file
  const int desc,                                                               // Open file descriptor to write to
  const char *content,                                                          // Content for the file
  const size_t length)                                                          // Length of content, or zero for a zero terminated string
 {if (content)                                                                  // Write content if any
   {size_t l = length ? : strlen(content);                                      // Content length
    while(l > 0)                                                                // Keep writing until error or all written
     {const ssize_t w = write(desc, content, l);                                // Write content
      if (w < 1)                                                                // Write failed for some reason
       {printStackBackTrace
          ("Unable to write to file because %m, file:\n%s\n", file.name);
       }
      l -= w;
     }
   }
 }

$ new$TemporaryWithContent                                                      //C Create a temporary file with the specified base name and extension preloaded with the specified content.
 (const char *fileName,                                                         // Base name and extension of the file
  const char *content,                                                          // Content for the file
  const size_t length)                                                          // Length of content, or zero for a zero terminated string
 {$ f = new$("");                                                               // Create file name with a yet to be determined name

  if (strlen(fileName) + 16 > f ▷ maxFileNameSize)                              // Check file name length
   {printStackBackTrace("File name too big: %s\n", fileName);
   }
  sprintf(f.name, "/tmp/%d/", getpid());                                        // Length checked above
  const int m = mkdir(f.name, S_IRWXU);                                         // Create folder

  if (m && errno != EEXIST)                                                     // Create a temporary folder for this process
   {printStackBackTrace
     ("Unable to create directory because %m, directory:\n%s\n", f.name);
   }

  strcat(f.name, fileName);                                                     // Length checked above
  const int o = open(f.name, O_WRONLY|O_CREAT, S_IRWXU);                        // Create file
  if (o < 0)
   {printStackBackTrace
     ("Unable to open file because %m, file:\n%s\n", f.name);
   }

  f ▷ writeContentToOpenFile(o, content, length);                               // Write content

  close(o);                                                                     // Close temporary file
  return f;
 }

$ new$Temporary                                                                 //C Create an empty temporary file with the specified base name and extension .
 (const char *fileName)                                                         // Base name and extension of the file
 {return new$TemporaryWithContent(fileName, "", 0);
 }

$ new$                                                                          //C Create a new  file name from a string - no corresponding file is actually created.
 (const char *name)                                                         // Base name and extension of the file
 {$ f = {proto: &ProtoTypes_$};
  const size_t l = sizeof(f.name);
  if (strlen(name) > l) printStackBackTrace
   ("File name too long, must be less than %lu bytes\n", l);
  strncpy(f.name, name, l);
  return f;
 }

static char * readFile_$                                                        // Return the content of a specified file as a string.
 (const $ file)                                                                 // File name to read
 {const ssize_t l = file ▷ size;
  if (l < 0) return 0; else if (l == 0) return strdup("");                      // Unsuccessful allocation or empty file
  const int   d = open(file.name, O_RDONLY);
  const char *s = mmap(NULL, l + 1, PROT_READ, MAP_PRIVATE, d, 0);              // Add space for a terminating zero byte - mmap sets unused bytes beyond the file to zero giving us a zero terminated string.

  if (!s)                                                                       // Unsuccessful map
   {close(d);
    return 0;
   }

  char *r = strndup(s, l);                                                      // Place the string on the heap
  close(d);                                                                     // Close the file: the map is private so we no long need the underlying file

  return r;
 }

static void writeFile_$_string_ssize                                            // Write the specified content to the named file.
 (const $      file,                                                            // File name to read
  const char * content,                                                         // Content for the file
  const size_t length)                                                          // Length of content, or zero for a zero terminated string
 {const int o = open(file.name, O_WRONLY | O_CREAT, S_IRWXU);                   // Open file for write, creating it if necessary
  file ▷ writeContentToOpenFile(o, content, length);
  close(o);                                                                     // Close the file: the map is private so we no long need the underlying file
 }

static void unlink_$                                                            // Unlink - delete - the specified file
 (const $      file)                                                            // File name to delete
 {unlink(file.name);
 }

static size_t b2SumW8_$                                                         // Get a BLAKE2 digest for a file represented as two hex digits.
 (const $ i)                                                                    // Name of file containing content for which we want a digest.
 {const $ o = new$Temporary("o.txt");                                           // File to receive output
  Utilities_system("b2sum -l 8 < %s > %s", i.name, o.name);                     // Execute Blake command via shell
  char * const r = o ▷ readFile;                                                // Read results from command
  const size_t d = strtol(r, NULL, 16);                                         // Convert result from hex string to integer
  free(r);                                                                      // Free read string
  return d;                                                                     // Return digest
 }

static size_t maxFileNameSize_$FileName                                         // Maximum size of a file name
 (const $ file)                                                                 // File description
 {return sizeof(file.name) - 1;                                                 // Room for terminating zero
 }
#endif

#if __INCLUDE_LEVEL__ == 0
void test0()                                                                    //Tb2SumW8 //Tnew$TemporaryWithContent //Tnew$Temporary //TreadFile //TmaxFileNameSize //TnewFileName //Tsize //TwriteFile //Tunlink
 {$ f = new$TemporaryWithContent("aaa.c", "aaaa", 0);
  assert(f ▷ maxFileNameSize == 255);
  assert(f ▷ b2SumW8 == 0x8f);

  char *s = f ▷ readFile;
  assert(!strcmp(s, "aaaa"));
  assert(f ▷ size == 4);
  free(s);

  const $ F = new$(f.name);
          F ▷ writeFile("bbbb", 0);
  assert( F ▷ size == 4);
  assert( F ▷ b2SumW8 == 0x12);
          F ▷ unlink;
  assert(!F ▷ size);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/utilities/utilities
