static CairoTextFont makeCairoTextFont
 (char * fontFile);
static void free_CairoTextImage
 (CairoTextImage i);
static void free_CairoTextFont
 (CairoTextFont font);
static void setFont
 (CairoTextImage i,
  CairoTextFont font);
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
static void leftArrow
 (CairoTextImage    i,
  Rectangle r,
  Colour    s,
  Colour    f);
static void leftArrowWithCircle
 (CairoTextImage    i,
  Rectangle r,
  Colour    s,
  Colour    f);
static void rightArrow
 (CairoTextImage    i,
  Rectangle r,
  Colour    s,
  Colour    f);
struct ProtoTypes_CairoTextFont {
  void  (*free)(                                                                // Free a font if it has been loaded
    CairoTextFont font);                                                        // CairoTextFont
 } const ProtoTypes_CairoTextFont =
{free_CairoTextFont};
CairoTextFont newCairoTextFont(CairoTextFont allocator) {return allocator;}

struct ProtoTypes_CairoTextImage {
  void  (*assertCairoTextResult)(                                               // Check the image via a digest
    CairoTextImage i,                                                           // CairoTextImage
    const char * const digest);                                                 // Expected digest
  void  (*clearWhite)(                                                          // Clear the drawing surface to white
    CairoTextImage i);                                                          // CairoTextImage
  void  (*free)(                                                                // Free an image
    CairoTextImage i);                                                          // CairoTextImage
  void  (*leftArrow)(                                                           // Draw a left pointing arrow in the specified rectangle with a linear gradient starting and ending with the specified colours
    CairoTextImage i,                                                           // Image
    Rectangle r,                                                                // Rectangle to draw arrow in
    Colour s,                                                                   // Start colour
    Colour f);                                                                  // Finish colour
  void  (*leftArrowWithCircle)(                                                 // Draw a left pointing arrow with a central circle cut out
    CairoTextImage i,                                                           // Image
    Rectangle r,                                                                // Rectangle to draw arrow in
    Colour s,                                                                   // Start colour
    Colour f);                                                                  // Finish colour
  void  (*rightArrow)(                                                          // Draw a right pointing arrow in the specified rectangle with a linear gradient starting and ending with the specified colours
    CairoTextImage i,                                                           // Image
    Rectangle r,                                                                // Rectangle to draw arrow in
    Colour s,                                                                   // Start colour
    Colour f);                                                                  // Finish colour
  void  (*save)(                                                                // Save a copy of the drawing surface to the specified file
    CairoTextImage i,                                                           // CairoTextImage
    char * imageFile,                                                           // Image file name
    const char * const digest);                                                 // Expected digest
  void  (*setFont)(                                                             // Start using a font
    CairoTextImage i,                                                           // CairoTextImage
    CairoTextFont font);                                                        // Font to use
 } const ProtoTypes_CairoTextImage =
{assertCairoTextResult, clearWhite_CairoText, free_CairoTextImage, leftArrow, leftArrowWithCircle, rightArrow, save_CairoText_string, setFont};
CairoTextImage newCairoTextImage(CairoTextImage allocator) {return allocator;}

