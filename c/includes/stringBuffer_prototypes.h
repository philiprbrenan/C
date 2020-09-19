static StringBuffer makeStringBuffer
 ();
static StringBuffer makeStringBufferFromString
 (const char * const string);
static void free_StringBuffer
 (const StringBuffer buffer);
static void add_StringBuffer_string
 (const StringBuffer buffer,
  const char * const string);
static void addn_StringBuffer_string
 (const StringBuffer            buffer,
  const char * const string,
  const size_t        length);
static void addStringBuffer_StringBuffer_StringBuffer
 (const StringBuffer buffer,
  const StringBuffer add);
static void addVaFormat_StringBuffer_string_va
 (const StringBuffer buffer,
  const char * const format,
  va_list va1);
static void addFormat_StringBuffer_strings
 (const StringBuffer buffer,
  const char * const format,
  ...);
static void addChar_StringBuffer_char
 (const StringBuffer    buffer,
  const char c);
static void addNewLine_StringBuffer
 (const StringBuffer buffer);
static void addSingleQuote_StringBuffer
 (const StringBuffer buffer);
static void addDoubleQuote_StringBuffer
 (const StringBuffer buffer);
static void addQuotedNewLine_StringBuffer
 (const StringBuffer buffer);
static size_t count_StringBuffer
 (const StringBuffer buffer);
static void join_StringBuffer
 (const StringBuffer old);
static StringBuffer splitLines
 (const StringBuffer string);
static StringBuffer splitWords
 (const StringBuffer string);
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
static ssize_t writeToFileHandle
 (const StringBuffer            buffer,
  const int          d,
  const char * const fileName);
static  StringBuffer writeTemporaryFile_StringBuffer_string
 (const StringBuffer            buffer,
  const char * const fileName);
static ssize_t writeFile_StringBuffer_string
 (const StringBuffer            buffer,
  const char * const fileName);
static void writeStderr_StringBuffer
 (const StringBuffer buffer);
static StringBuffer readFile_StringBuffer_string
 (const StringBuffer fileName);
struct ProtoTypes_StringBuffer {
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
  void  (*addQuotedNewLine)(                                                    // Add a quoted new line
    const StringBuffer buffer);                                                 // StringBuffer
  void  (*addSingleQuote)(                                                      // Add a single quote
    const StringBuffer buffer);                                                 // StringBuffer
  void  (*addStringBuffer)(                                                     // Add a string buffer
    const StringBuffer buffer,                                                  // Target StringBuffer
    const StringBuffer add);                                                    // Source StringBuffer
  void  (*addVaFormat)(                                                         // Add a variadic argument formatted string
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const format,                                                  // Format
    va_list va1);                                                               // Variable argument list
  void  (*add)(                                                                 // Concatenate a string
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const string);                                                 // Zero terminated string
  void  (*addn)(                                                                // Concatenate a string of known length
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const string,                                                  // String
    const size_t length);                                                       // String length
  int  (*containsString)(                                                       // Checks whether a StringBuffer contains a specified zero terminated string.
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const string);                                                 // String
  int  (*contains)(                                                             // Checks whether the first StringBuffer contains the second StringBuffer
    const StringBuffer a,                                                       // First StringBuffer
    const StringBuffer b);                                                      // Second StringBuffer
  size_t  (*count)(                                                             // Count the number of sub strings in the buffer
    const StringBuffer buffer);                                                 // StringBuffer
  int  (*equalsString)(                                                         // Checks whether a StringBuffer is equal to a specified zero terminated string.
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const string);                                                 // String
  int  (*equals)(                                                               // Checks whether two StringBuffer are equal.
    const StringBuffer a,                                                       // First StringBuffer
    const StringBuffer b);                                                      // Second StringBuffer
  void  (*free)(                                                                // Free a StringBuffer
    const StringBuffer buffer);                                                 // StringBuffer
  void  (*join)(                                                                // Join in place - join all the sub strings in the specified string buffer and replace them with one string.
    const StringBuffer old);                                                    // StringBuffer
  size_t  (*length)(                                                            // Length of the string held in the buffer
    const StringBuffer buffer);                                                 // StringBuffer
  StringBuffer  (*readFile)(                                                    // Read a file and returns its content as a string buffer.
    const StringBuffer fileName);                                               // Name of the file as the content of a string buffer
  StringBuffer  (*splitLines)(                                                  // Split the specified StringBuffer on any new line characters and return the split text as a new StringBuffer
    const StringBuffer string);                                                 // StringBuffer
  StringBuffer  (*splitWords)(                                                  // Split the specified StringBuffer into words delimited by spaces and return the split text as a new StringBuffer
    const StringBuffer string);                                                 // StringBuffer
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
  ssize_t  (*writeFile)(                                                        // Write a StringBuffer as a string to the specified file
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const fileName);                                               // Base name of the file
  void  (*writeStderr)(                                                         // Write a StringBuffer as a string to stderr
    const StringBuffer buffer);                                                 // StringBuffer
  StringBuffer  (*writeTemporaryFile)(                                          // Write a StringBuffer as a string to a temporary file with the specified base name and return the full name of the file created as a string buffer.
    const StringBuffer buffer,                                                  // StringBuffer
    const char * const fileName);                                               // Base name of the file
  ssize_t  (*writeToFileHandle)(                                                // Write a StringBuffer as a string to a file handle and return the non negative number of bytes written or a negative error.
    const StringBuffer buffer,                                                  // StringBuffer
    const int d,                                                                // Base name of the file
    const char * const fileName);                                               // The name of the file being written to
 } const ProtoTypes_StringBuffer =
{addChar_StringBuffer_char, addDoubleQuote_StringBuffer, addFormat_StringBuffer_strings, addNewLine_StringBuffer, addQuotedNewLine_StringBuffer, addSingleQuote_StringBuffer, addStringBuffer_StringBuffer_StringBuffer, addVaFormat_StringBuffer_string_va, add_StringBuffer_string, addn_StringBuffer_string, containsString_StringBuffer_StringBuffer, contains_StringBuffer_StringBuffer, count_StringBuffer, equalsString_StringBuffer_string, equals_StringBuffer_StringBuffer, free_StringBuffer, join_StringBuffer, length_StringBuffer, readFile_StringBuffer_string, splitLines, splitWords, string_StringBuffer_string, substringEquals_int_StringBuffer_int_int_string, substring_size_StringBuffer_int_int_string, writeFile_StringBuffer_string, writeStderr_StringBuffer, writeTemporaryFile_StringBuffer_string, writeToFileHandle};
StringBuffer newStringBuffer(StringBuffer allocator) {return allocator;}

