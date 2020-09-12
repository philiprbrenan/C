static size_t size
 (FileName file);
static void writeContentToOpenFile
 (const FileName   file,
  const int desc,
  const char * const content,
  const size_t length);
static FileName makeFileNameTemporaryWithContent
 (const char * const fileName,
  const char * const content,
  const size_t length);
static FileName makeFileNameTemporary
 (const char * const fileName);
static char * readFile_FileName
 (const FileName file);
static void writeFile_FileName_string_ssize
 (const FileName            file,
  const char * const content,
  const size_t length);
static void unlink_FileName
 (const FileName file);
static size_t b2SumW8_FileName
 (const FileName i);
static int equalsString_int_FileName_string
 (const FileName      file,
  const char * const expected);
static int containsString_int_FileName_string
 (const FileName      file,
  const char * const expected);
static size_t maxFileNameSize_FileNameFileName
 (const FileName file);
struct ProtoTypes_FileName {
  size_t  (*b2SumW8)(                                                           // Get a BLAKE2 digest for a file represented as two hex digits.
    const FileName i);                                                          // Name of file containing content for which we want a digest.
  int  (*containsString)(                                                       // Check that the content of the specified file contains the specified zero terminated string.
    const FileName file,                                                        // File
    const char * const expected);                                               // Zero terminated string of expected content
  int  (*equalsString)(                                                         // Check that the content of the specified file is equal to the specified zero terminated string.
    const FileName file,                                                        // File
    const char * const expected);                                               // Zero terminated string of expected content
  size_t  (*maxFileNameSize)(                                                   // Maximum size of a file name
    const FileName file);                                                       // File description
  char *  (*readFile)(                                                          // Return the content of a specified file as a string.
    const FileName file);                                                       // File name to read
  size_t  (*size)(                                                              // Get the size of a file in bytes
    FileName file);                                                             // Name of the file
  void  (*unlink)(                                                              // Unlink - delete - the specified file
    const FileName file);                                                       // File name to delete
  void  (*writeContentToOpenFile)(                                              // Write content to an open file descriptor unti it is either all written or an error occurs.
    const FileName file,                                                        // Name of the file
    const int desc,                                                             // Open file descriptor to write to
    const char * const content,                                                 // Content for the file
    const size_t length);                                                       // Length of content, or zero for a zero terminated string
  void  (*writeFile)(                                                           // Write the specified content to the named file.
    const FileName file,                                                        // File name to read
    const char * const content,                                                 // Content for the file
    const size_t length);                                                       // Length of content, or zero for a zero terminated string
 } const ProtoTypes_FileName =
{b2SumW8_FileName, containsString_int_FileName_string, equalsString_int_FileName_string, maxFileNameSize_FileNameFileName, readFile_FileName, size, unlink_FileName, writeContentToOpenFile, writeFile_FileName_string_ssize};
FileName newFileName(FileName allocator) {return allocator;}

