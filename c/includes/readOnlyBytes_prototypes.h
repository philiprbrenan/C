static ReadOnlyBytes makeReadOnlyBytes
 (char * const          data,
  size_t                length,
  const enum ReadOnlyBytesAllocator allocator);
static ReadOnlyBytes makeReadOnlyBytesError
 (const enum ReadOnlyBytesError  error,
  const char * const format,
  ...);
static ReadOnlyBytes makeReadOnlyBytesFromString
 (char * const string);
static ReadOnlyBytes makeReadOnlyBytesFromStringN
 (char * const string,
  const size_t length);
static ReadOnlyBytes makeReadOnlyBytesDup
 (const char * const string);
static ReadOnlyBytes makeReadOnlyBytesDupN
 (char * const string,
  const size_t length);
static ReadOnlyBytes makeReadOnlyBytesFromFormat
 (const char * const format,
  ...);
static ReadOnlyBytes makeReadOnlyBytesBuffer
 (const size_t length);
static ReadOnlyBytes makeReadOnlyBytesFromFile
 (const FileName file);
static void free_ReadOnlyBytes
 (const ReadOnlyBytes r);
static size_t length_ReadOnlyBytes
 (const ReadOnlyBytes r);
static char *data_ReadOnlyBytes
 (const ReadOnlyBytes r);
static ReadOnlyBytes substring_string_ReadOnlyBytes_sizet_sizet
 (const ReadOnlyBytes      r,
  const size_t start,
  const size_t length);
static FileName writeTemporaryFile_ReadOnlyBytes_string
 (const ReadOnlyBytes            r,
  const char * const fileName);
static void writeFile_ReadOnlyBytes_string
 (const ReadOnlyBytes        r,
  const FileName fileName);
static int writeOpenFile_ReadOnlyBytes_string
 (const  ReadOnlyBytes       r,
  FILE   * const f);
static int equals_ReadOnlyBytes_ReadOnlyBytes
 (const ReadOnlyBytes p,
  const ReadOnlyBytes q);
static int equalsString_ReadOnlyBytes_zeroString
 (const ReadOnlyBytes            r,
  const char * const s);
static int containsString_ReadOnlyBytes_zeroString
 (const ReadOnlyBytes            r,
  const char * const s);
static size_t b2SumW8_ReadOnlyBytes
 (const ReadOnlyBytes r);
static ArenaTree splitNewLine
 (const ReadOnlyBytes r);
static ArenaTree splitSpaces
 (const ReadOnlyBytes r);
struct ProtoTypes_ReadOnlyBytes {
  size_t  (*b2SumW8)(                                                           // Get a BLAKE2 digest for a file represented as two hex digits.
    const ReadOnlyBytes r);                                                     // Description of read only sequence of bytes
  int  (*containsString)(                                                       // Check that a read only byte sequence contains the specified zero terminated string
    const ReadOnlyBytes r,                                                      // Description of read only sequence of bytes
    const char * const s);                                                      // Zero terminated string
  char * (*data)(                                                               // Data field of the sequence
    const ReadOnlyBytes r);                                                     // Description of a sequence of read only bytes
  int  (*equalsString)(                                                         // Compare a read only byte sequence with a zero terminated string
    const ReadOnlyBytes r,                                                      // Description of read only sequence of bytes
    const char * const s);                                                      // Zero terminated string
  int  (*equals)(                                                               // Compare two read only byte sequences
    const ReadOnlyBytes p,                                                      // Description of first read only sequence of bytes
    const ReadOnlyBytes q);                                                     // Description second read only sequence of bytes
  void  (*free)(                                                                // Free any resources associated with a read only byte sequence
    const ReadOnlyBytes r);                                                     // Description of a read only sequence of bytes
  size_t  (*length)(                                                            // Length of the sequence
    const ReadOnlyBytes r);                                                     // Description of a sequence of read only bytes
  ArenaTree  (*splitNewLine)(                                                   // Split the specified ReadOnlyBytes on any new line characters
    const ReadOnlyBytes r);                                                     // Description of read only sequence of bytes
  ArenaTree  (*splitSpaces)(                                                    // Split the specified ReadOnlyBytes on any sequence of white space, removing the white space from the results
    const ReadOnlyBytes r);                                                     // Description of read only sequence of bytes
  ReadOnlyBytes  (*substring)(                                                  // Describe a sub-string of the specified sequence
    const ReadOnlyBytes r,                                                      // Description of a sequence of read only bytes
    const size_t start,                                                         // Start position of the sub-string
    const size_t length);                                                       // Length of the sub-string
  void  (*writeFile)(                                                           // Write a read only byte sequence to the specified file.
    const ReadOnlyBytes r,                                                      // Description of a read only sequence of bytes
    const FileName fileName);                                                   // Name of the file
  int  (*writeOpenFile)(                                                        // Write a read only byte sequence to the specified file descriptor.
    const  ReadOnlyBytes r,                                                     // Description of a read only sequence of bytes
    FILE   * const f);                                                          // File descriptor
  FileName  (*writeTemporaryFile)(                                              // Write a read only byte sequence to a temporary file with the specified base name.
    const ReadOnlyBytes r,                                                      // Description of a read only sequence of bytes
    const char * const fileName);                                               // Base name of the file
 } const ProtoTypes_ReadOnlyBytes =
{b2SumW8_ReadOnlyBytes, containsString_ReadOnlyBytes_zeroString, data_ReadOnlyBytes, equalsString_ReadOnlyBytes_zeroString, equals_ReadOnlyBytes_ReadOnlyBytes, free_ReadOnlyBytes, length_ReadOnlyBytes, splitNewLine, splitSpaces, substring_string_ReadOnlyBytes_sizet_sizet, writeFile_ReadOnlyBytes_string, writeOpenFile_ReadOnlyBytes_string, writeTemporaryFile_ReadOnlyBytes_string};
ReadOnlyBytes newReadOnlyBytes(ReadOnlyBytes allocator) {return allocator;}

