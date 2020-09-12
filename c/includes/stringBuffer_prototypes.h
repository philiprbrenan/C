static StringBuffer makeStringBuffer
 ();
static void free_StringBuffer
 (const StringBuffer buffer);
static void add
 (const StringBuffer buffer,
  const char * const string);
static size_t length
 (const StringBuffer buffer);
static ReadOnlyBytes string_StringBuffer
 (const StringBuffer buffer);
struct ProtoTypes_StringBuffer {
  void  (*add)(                                                                 // Concatenate a string
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const string);                                                 // Zero terminated string
  void  (*free)(                                                                // Free a StringBuffer
    const StringBuffer buffer);                                                 // StringBuffer
  size_t  (*length)(                                                            // Length of the string held in the buffer
    const StringBuffer buffer);                                                 // StringBuffer
  ReadOnlyBytes  (*string)(                                                     // Create a read only bytes string from the StringBuffer.
    const StringBuffer buffer);                                                 // StringBuffer
 } const ProtoTypes_StringBuffer =
{add, free_StringBuffer, length, string_StringBuffer};
StringBuffer newStringBuffer(StringBuffer allocator) {return allocator;}

