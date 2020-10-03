static void free_CairoText
 (CairoTextImage i);
static void assertCairoTextImageFile
 (char *             imageFile,
  const char * const digest);
static void assertCairoTextResult
 (CairoTextImage i,
  const char * const digest);
static void save_CairoText_string
 (CairoTextImage             i,
  char *             imageFile,
  const char * const digest);
static void clearWhite_CairoText
 (CairoTextImage             i);
struct ProtoTypes_CairoTextImage {
  void  (*assertCairoTextResult)(                                               // Check the image via a digest
    CairoTextImage i,                                                           // CairoTextImage
    const char * const digest);                                                 // Expected digest
  void  (*clearWhite)(                                                          // Clear the drawing surface to white
    CairoTextImage i);                                                          // CairoTextImage
  void  (*free)(                                                                // Free an image
    CairoTextImage i);                                                          // CairoTextImage
  void  (*save)(                                                                // Save a copy of the drawing surface to the specified file
    CairoTextImage i,                                                           // CairoTextImage
    char * imageFile,                                                           // Image file name
    const char * const digest);                                                 // Expected digest
 } const ProtoTypes_CairoTextImage =
{assertCairoTextResult, clearWhite_CairoText, free_CairoText, save_CairoText_string};
CairoTextImage newCairoTextImage(CairoTextImage allocator) {return allocator;}

