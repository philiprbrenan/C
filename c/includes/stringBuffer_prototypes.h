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
static void addChar_StringBuffer
 (const StringBuffer    buffer,
  const char c);
static void addNewLine
 (const StringBuffer buffer);
static void addSingleQuote
 (const StringBuffer buffer);
static void addDoubleQuote
 (const StringBuffer buffer);
static size_t length_StringBuffer
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
static int substringEquals_int_StringBuffer_int_int_string
 (const StringBuffer            buffer,
  const size_t       start,
  const size_t       length,
  const char * const string);
static void string_StringBuffer_string
 (const StringBuffer        buffer,
  char  * const  string);
static size_t substring_size_StringBuffer_int_int_string
 (const StringBuffer      buffer,
  const size_t start,
  const size_t length,
  char * const string);
static ReadOnlyBytes readOnlyBytes_StringBuffer
 (const StringBuffer buffer);
struct ProtoTypes_StringBuffer {
  void  (*add)(                                                                 // Concatenate a string
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const string);                                                 // Zero terminated string
  void  (*addChar)(                                                             // Add the specified character
    const StringBuffer buffer,                                                  // StringBuffer
    const char c);                                                              // Character to add
  void  (*addDoubleQuote)(                                                      // Add a double quote
    const StringBuffer buffer);                                                 // StringBuffer
  void  (*addFormat)(                                                           // Add a formatted string
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const format,                                                  // Format
     ...);                                                                      // Strings
  void  (*addNewLine)(                                                          // Add a new line
    const StringBuffer buffer);                                                 // StringBuffer
  void  (*addReadOnlyBytes)(                                                    // Concatenate a read only bytes
    const StringBuffer buffer,                                                  // StringBuffer
    const ReadOnlyBytes string);                                                // Read only bytes
  void  (*addSingleQuote)(                                                      // Add a single quote
    const StringBuffer buffer);                                                 // StringBuffer
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
  ReadOnlyBytes  (*readOnlyBytes)(                                              // Create a read only bytes string from the StringBuffer.
    const StringBuffer buffer);                                                 // StringBuffer
  void  (*string)(                                                              // Load a zero terminated string from a StringBuffer.
    const StringBuffer buffer,                                                  // StringBuffer
    char  * const string);                                                      // String to load with enough space for the string and its terminating zero
  int  (*substringEquals)(                                                      // Checks whether a sub string of the specified StringBuffer is equal to the specified zero terminated string.
    const StringBuffer buffer,                                                  // StringBuffer
    const size_t start,                                                         // Offset to start of string
    const size_t length,                                                        // Length of sub string. The length of the zero terminate string to be loaded must be larger than this.
    const char * const string);                                                 // Zero terminated string expected
  size_t  (*substring)(                                                         // Load a zero terminates string with a sub string of a StringBuffer and return the number of bytes loaded.
    const StringBuffer buffer,                                                  // StringBuffer
    const size_t start,                                                         // Offset to start of string
    const size_t length,                                                        // Length of sub string. The length of the zero terminate string to be loaded must be larger than this.
    char * const string);                                                       // String to load with enough space for the string and its terminating zero
 } const ProtoTypes_StringBuffer =
{add, addChar_StringBuffer, addDoubleQuote, addFormat, addNewLine, addReadOnlyBytes, addSingleQuote, containsString_StringBuffer_StringBuffer, contains_StringBuffer_StringBuffer, equalsString_StringBuffer_string, equals_StringBuffer_StringBuffer, free_StringBuffer, length_StringBuffer, readOnlyBytes_StringBuffer, string_StringBuffer_string, substringEquals_int_StringBuffer_int_int_string, substring_size_StringBuffer_int_int_string};
StringBuffer newStringBuffer(StringBuffer allocator) {return allocator;}

