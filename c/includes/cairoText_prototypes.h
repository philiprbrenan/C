static CairoTextFont makeCairoTextFont
 (char * fontFile);
static void free_CairoTextImage
 (CairoTextImage * i);
static void free_CairoTextFont
 (CairoTextFont * font);
static void font_CairoText
 (CairoTextImage * i,
  CairoTextFont font);
static void fontMetrics_CairoText
 (CairoTextImage * i);
static void fontSize_CairoText
 (CairoTextImage * i,
  int    size);
static double textAdvance_CairoText
 (CairoTextImage * i,
  char * s);
static void text_CairoText
 (CairoTextImage * i,
  double x,
  double y,
  const char * t);
static void clearWhite_CairoText
 (CairoTextImage * i);
static void colour_CairoText
 (CairoTextImage * i,
  Colour c);
static void rgb_CairoText
 (CairoTextImage * i,
  double r,
  double g,
  double b);
static void rgba_CairoText
 (CairoTextImage * i,
  double r,
  double g,
  double b,
  double a);
static void save_CairoText
 (CairoTextImage * i);
static void restore_CairoText
 (CairoTextImage * i);
static void move_CairoText
 (CairoTextImage * i,
  double   x,
  double   y);
static void line_CairoText
 (CairoTextImage * i,
  double   x,
  double   y);
static void close_CairoText
 (CairoTextImage * i);
static void fill_CairoText
 (CairoTextImage * i);
static void stroke_CairoText
 (CairoTextImage * i);
static void clip_CairoText
 (CairoTextImage  * i,
  Rectangle r);
static void leftArrow
 (CairoTextImage  * i,
  Rectangle r,
  Colour    s,
  Colour    f);
static void leftArrowWithCircle
 (CairoTextImage  * i,
  Rectangle r,
  Colour    s,
  Colour    f);
static void rightArrow
 (CairoTextImage   *  i,
  Rectangle r,
  Colour    s,
  Colour    f);
static void assertCairoTextImageFile
 (char *             imageFile,
  const char * const digest);
static void assertCairoTextResult
 (CairoTextImage * i,
  const char * const digest);
static void saveAsPng_CairoText_string
 (CairoTextImage   *  i,
  char *    imageFile,
  const char * const digest);
struct ProtoTypes_CairoTextFont {
  void  (*free)(                                                                // Free a font if it has been loaded
    CairoTextFont * font);                                                      // CairoTextFont
 } const ProtoTypes_CairoTextFont =
{free_CairoTextFont};
CairoTextFont newCairoTextFont(CairoTextFont allocator) {return allocator;}

struct ProtoTypes_CairoTextImage {
  void  (*assertCairoTextResult)(                                               // Check the image via a digest
    CairoTextImage * i,                                                         // CairoTextImage
    const char * const digest);                                                 // Expected digest
  void  (*clearWhite)(                                                          // Clear the drawing surface to white
    CairoTextImage * i);                                                        // CairoTextImage
  void  (*clip)(                                                                // Set the clip area
    CairoTextImage  * i,                                                        // CairoTextImage
    Rectangle r);                                                               // Rectangle
  void  (*close)(                                                               // Close the current path
    CairoTextImage * i);                                                        // CairoTextImage
  void  (*colour)(                                                              // Set the current colour
    CairoTextImage * i,                                                         // CairoTextImage
    Colour c);                                                                  // Colour
  void  (*fill)(                                                                // Fill the current path
    CairoTextImage * i);                                                        // CairoTextImage
  void  (*fontMetrics)(                                                         // Load the metrics of the current font
    CairoTextImage * i);                                                        // CairoTextImage
  void  (*fontSize)(                                                            // Set the size of a font
    CairoTextImage * i,                                                         // CairoTextImage
    int size);                                                                  // Size
  void  (*font)(                                                                // Start using a font
    CairoTextImage * i,                                                         // CairoTextImage
    CairoTextFont font);                                                        // Font to use
  void  (*free)(                                                                // Free an image
    CairoTextImage * i);                                                        // CairoTextImage
  void  (*leftArrow)(                                                           // Draw a left pointing arrow in the specified rectangle with a linear gradient starting and ending with the specified colours
    CairoTextImage  * i,                                                        // Image
    Rectangle r,                                                                // Rectangle to draw arrow in
    Colour s,                                                                   // Start colour
    Colour f);                                                                  // Finish colour
  void  (*leftArrowWithCircle)(                                                 // Draw a left pointing arrow with a central circle cut out
    CairoTextImage  * i,                                                        // Image
    Rectangle r,                                                                // Rectangle to draw arrow in
    Colour s,                                                                   // Start colour
    Colour f);                                                                  // Finish colour
  void  (*line)(                                                                // Draw a line to the specified position from the current position
    CairoTextImage * i,                                                         // CairoTextImage
    double x,                                                                   // X coordinate to move to
    double y);                                                                  // Y coordinate to move to
  void  (*move)(                                                                // Move to a position without drawing
    CairoTextImage * i,                                                         // CairoTextImage
    double x,                                                                   // X coordinate to move to
    double y);                                                                  // Y coordinate to move to
  void  (*restore)(                                                             // Restore the drawing context
    CairoTextImage * i);                                                        // CairoTextImage
  void  (*rgb)(                                                                 // Set the current colour
    CairoTextImage * i,                                                         // CairoTextImage
    double r,                                                                   // Red
    double g,                                                                   // Green
    double b);                                                                  // Blue
  void  (*rgba)(                                                                // Set the current colour
    CairoTextImage * i,                                                         // CairoTextImage
    double r,                                                                   // Red
    double g,                                                                   // Green
    double b,                                                                   // Blue
    double a);                                                                  // Alpha
  void  (*rightArrow)(                                                          // Draw a right pointing arrow in the specified rectangle with a linear gradient starting and ending with the specified colours
    CairoTextImage   * i,                                                       // Image
    Rectangle r,                                                                // Rectangle to draw arrow in
    Colour s,                                                                   // Start colour
    Colour f);                                                                  // Finish colour
  void  (*saveAsPng)(                                                           // Save a copy of the drawing surface to the specified file
    CairoTextImage   * i,                                                       // CairoTextImage
    char * imageFile,                                                           // Image file name
    const char * const digest);                                                 // Expected digest
  void  (*save)(                                                                // Save the drawing context
    CairoTextImage * i);                                                        // CairoTextImage
  void  (*stroke)(                                                              // Stroke the current path
    CairoTextImage * i);                                                        // CairoTextImage
  double  (*textAdvance)(                                                       // Get the width of the specified text
    CairoTextImage * i,                                                         // CairoTextImage
    char * s);                                                                  // String
  void  (*text)(                                                                // Draw text at the specified position using the current font, fonet size and colour
    CairoTextImage * i,                                                         // CairoTextImage
    double x,                                                                   // X position of text
    double y,                                                                   // Y position of the upper edge of the text - i.e. the text will be drawn below this value.
    const char * t);                                                            // The text to draw
 } const ProtoTypes_CairoTextImage =
{assertCairoTextResult, clearWhite_CairoText, clip_CairoText, close_CairoText, colour_CairoText, fill_CairoText, fontMetrics_CairoText, fontSize_CairoText, font_CairoText, free_CairoTextImage, leftArrow, leftArrowWithCircle, line_CairoText, move_CairoText, restore_CairoText, rgb_CairoText, rgba_CairoText, rightArrow, saveAsPng_CairoText_string, save_CairoText, stroke_CairoText, textAdvance_CairoText, text_CairoText};
CairoTextImage newCairoTextImage(CairoTextImage allocator) {return allocator;}

