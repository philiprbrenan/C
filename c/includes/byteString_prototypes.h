static ByteString bytes_bytesAllocator_sizet_string_sizet
 (BytesAllocator allocator,
  size_t         length,
  string         str,
  size_t         strLength);
static size_t length_byteString
 (char *bytes);
static size_t used_byteString
 (char *bytes);
static size_t copy_bytes_into_string
 (ByteString  bytes,
  string str);
static ByteString concatenate_bytes_strings
 (BytesAllocator  allocator,
  ByteString  *bytes);
static int compare_int_bytes_string
 (ByteString  bytes,
  string str);
struct ProtoTypes_ByteString {
  int  (*compare)(                                                              // Compare the specified bytes with the specified string
    ByteString bytes,                                                           // Bytes
    string str);                                                                // Target string
  size_t  (*copy)(                                                              // Copy the specified bytes into the specified preallocated string and add a terminating zero. The string should be preallocated large enough to take the specified byte content and terminator.
    ByteString bytes,                                                           // Bytes
    string str);                                                                // Target string
 } const ProtoTypes_ByteString =
{compare_int_bytes_string, copy_bytes_into_string};
struct ProtoTypes_BytesAllocator {
  ByteString  (*bytes)(                                                         // Allocate a byte string of the specified length and optionally load it from a string
    BytesAllocator allocator,                                                   // Allocation method
    size_t length,                                                              // Length of allocation. If zero the string length is used.
    string str,                                                                 // String
    size_t strLength);                                                          // Length
  ByteString  (*concatenate)(                                                   // Concatenate zero or more ByteString.
    BytesAllocator allocator,                                                   // Bytes to concatenate
    ByteString  * bytes);                                                       // Bytes to concatenate
 } const ProtoTypes_BytesAllocator =
{bytes_bytesAllocator_sizet_string_sizet, concatenate_bytes_strings};
