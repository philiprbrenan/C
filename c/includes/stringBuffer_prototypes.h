static StringBuffer makeStringBuffer
 ();
static void free_StringBuffer
 (const StringBuffer buffer);
static void add
 (const StringBuffer buffer,
  const char * const string);
static void addReadOnlyBytes
 (const StringBuffer             buffer,
  const ReadOnlyBytes string);
static void addFormat
 (const StringBuffer buffer,
  const char * const format,
  ...);
static size_t length
 (const StringBuffer buffer);
static int equals_StringBuffer_StringBuffer
 (const StringBuffer a,
  const StringBuffer b);
static int equalsString_StringBuffer_string
 (const StringBuffer            buffer,
  const char * const string);
static int contains_StringBuffer_StringBuffer
 (const StringBuffer a,
  const StringBuffer b);
static int containsString_StringBuffer_StringBuffer
 (const StringBuffer            buffer,
  const char * const string);
static ReadOnlyBytes string_StringBuffer
 (const StringBuffer buffer);
struct ProtoTypes_StringBuffer {
  void  (*add)(                                                                 // Concatenate a string
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const string);                                                 // Zero terminated string
  void  (*addFormat)(                                                           // Add a formatted string
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const format,                                                  // Format
     ...);                                                                      // Strings
  void  (*addReadOnlyBytes)(                                                    // Concatenate a read only bytes
    const StringBuffer buffer,                                                  // StringBuffer
    const ReadOnlyBytes string);                                                // Read only bytes
  int  (*containsString)(                                                       // Checks whether a StringBuffer contains a specified zero terminated string.
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const string);                                                 // String
  int  (*contains)(                                                             // Checks whether the first StringBuffer contains the second StringBuffer
    const StringBuffer a,                                                       // First StringBuffer
    const StringBuffer b);                                                      // Second StringBuffer
  int  (*equalsString)(                                                         // Checks whether a StringBuffer is equal to a specified zero terminated string.
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const string);                                                 // String
  int  (*equals)(                                                               // Checks whether two StringBuffer are equal.
    const StringBuffer a,                                                       // First StringBuffer
    const StringBuffer b);                                                      // Second StringBuffer
  void  (*free)(                                                                // Free a StringBuffer
    const StringBuffer buffer);                                                 // StringBuffer
  size_t  (*length)(                                                            // Length of the string held in the buffer
    const StringBuffer buffer);                                                 // StringBuffer
  ReadOnlyBytes  (*string)(                                                     // Create a read only bytes string from the StringBuffer.
    const StringBuffer buffer);                                                 // StringBuffer
 } const ProtoTypes_StringBuffer =
{add, addFormat, addReadOnlyBytes, containsString_StringBuffer_StringBuffer, contains_StringBuffer_StringBuffer, equalsString_StringBuffer_string, equals_StringBuffer_StringBuffer, free_StringBuffer, length, string_StringBuffer};
StringBuffer newStringBuffer(StringBuffer allocator) {return allocator;}

