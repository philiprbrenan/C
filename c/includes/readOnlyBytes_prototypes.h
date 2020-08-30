ReadOnlyBytes newReadOnlyBytes
 (const char  *data,

  const size_t length,
  const enum   ReadOnlyBytes_allocator allocator);
ReadOnlyBytes newReadOnlyBytesError
 (const int errNo);
ReadOnlyBytes newReadOnlyBytesFromString
 (const char * string,
  const size_t length);
ReadOnlyBytes newReadOnlyBytesFromAllocatedString
 (const char * string,
  const size_t length);
ReadOnlyBytes newReadOnlyBytesFromFormat
 (const char * format, ...);
ReadOnlyBytes newReadOnlyBytesFromFile
 (FileName file);
static void free_ReadOnlyBytes
 (const ReadOnlyBytes r);
static size_t length_ReadOnlyBytes
 (const ReadOnlyBytes r);
static char *data_ReadOnlyBytes
 (const ReadOnlyBytes r);
static int errNo_ReadOnlyBytes
 (const ReadOnlyBytes r);
static ReadOnlyBytes substring_string_ReadOnlyBytes_sizet_sizet
 (const ReadOnlyBytes      r,
  const size_t start,
  const size_t length);
static void writeFile_ReadOnlyBytes_string
 (const ReadOnlyBytes        r,
  const FileName fileName);
static int writeOpenFile_ReadOnlyBytes_string
 (const ReadOnlyBytes r,
  FILE   *f);
static int equals_ReadOnlyBytes_ReadOnlyBytes
 (const ReadOnlyBytes p,
  const ReadOnlyBytes q);
static int equalsString_ReadOnlyBytes_zeroString
 (const ReadOnlyBytes     r,
  const char *s);
static size_t b2SumW8_ReadOnlyBytes
 (const ReadOnlyBytes r);
struct ProtoTypes_ReadOnlyBytes {
  size_t  (*b2SumW8)(                                                           // Get a BLAKE2 digest for a file represented as two hex digits.
    const ReadOnlyBytes r);                                                     // Description of read only sequence of bytes
  char * (*data)(                                                               // Data field of the sequence
    const ReadOnlyBytes r);                                                     // Description of a sequence of read only bytes
  int  (*equalsString)(                                                         // Compare a read only byte sequence with a zero terminated string
    const ReadOnlyBytes r,                                                      // Description of read only sequence of bytes
    const char * s);                                                            // Zero terminated string
  int  (*equals)(                                                               // Compare two read only byte sequences
    const ReadOnlyBytes p,                                                      // Description of first read only sequence of bytes
    const ReadOnlyBytes q);                                                     // Description second read only sequence of bytes
  int  (*errNo)(                                                                // Error number of the sequence if any
    const ReadOnlyBytes r);                                                     // Description of a sequence of read only bytes
  void  (*free)(                                                                // Free any resources associated with a read only byte sequence
    const ReadOnlyBytes r);                                                     // Description of a read only sequence of bytes
  size_t  (*length)(                                                            // Length of the sequence
    const ReadOnlyBytes r);                                                     // Description of a sequence of read only bytes
  ReadOnlyBytes  (*substring)(                                                  // Describe a sub-string of the specified sequence
    const ReadOnlyBytes r,                                                      // Description of a sequence of read only bytes
    const size_t start,                                                         // Start position of the sub-string
    const size_t length);                                                       // Length of the sub-string
  void  (*writeFile)(                                                           // Write a read only byte sequence to the specified file.
    const ReadOnlyBytes r,                                                      // Description of a read only sequence of bytes
    const FileName fileName);                                                   // Name of the file
  int  (*writeOpenFile)(                                                        // Write a read only byte sequence to the specified file descriptor.
    const ReadOnlyBytes r,                                                      // Description of a read only sequence of bytes
    FILE   * f);                                                                // File descriptor
 } const ProtoTypes_ReadOnlyBytes =
{b2SumW8_ReadOnlyBytes, data_ReadOnlyBytes, equalsString_ReadOnlyBytes_zeroString, equals_ReadOnlyBytes_ReadOnlyBytes, errNo_ReadOnlyBytes, free_ReadOnlyBytes, length_ReadOnlyBytes, substring_string_ReadOnlyBytes_sizet_sizet, writeFile_ReadOnlyBytes_string, writeOpenFile_ReadOnlyBytes_string};
