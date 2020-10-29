static CairoTextFont makeCairoTextFont
 (char * fontFile);
static CairoTextCompactList makeCairoTextCompactList
 (CairoTextImage  * image,
  ArenaList list,
  Rectangle drawTable);
static void free__CairoTextImage
 (CairoTextImage * i);
static void free__CairoTextFont
 (CairoTextFont * font);
static void font__CairoText_enum
 (CairoTextImage    * i,
  enum CairoTextFonts fontNumber);
static void fontMetrics__CairoText
 (CairoTextImage * i);
static void fontSize__CairoText_size
 (CairoTextImage * i,
  int      size);
static double textAdvance__CairoText_string
 (CairoTextImage * i,
  char   * s);
static void text__CairoText_double_double_string
 (CairoTextImage     * i,
  double       x,
  double       y,
  const char * t);
static void textLine__CairoText_double_double_string
 (CairoTextImage     * i,
  double       x,
  double       y,
  const char * t);
static void textFit__CairoText_rectangle_int_int_int_string
 (CairoTextImage     * i,
  Rectangle    rc,
  const int    jX,
  const int    jY,
  const int    outline,
  const char * text);
static void clearWhite__CairoText
 (CairoTextImage * i);
static void colour__CairoText_colour
 (CairoTextImage * i,
  Colour   c);
static void rgb__CairoText_double_double_double
 (CairoTextImage * i,
  double   r,
  double   g,
  double   b);
static void rgba__CairoText_double_double_double_double
 (CairoTextImage * i,
  double   r,
  double   g,
  double   b,
  double   a);
static void save__CairoText
 (CairoTextImage * i);
static void restore__CairoText
 (CairoTextImage * i);
static void move__CairoText_double_double
 (CairoTextImage * i,
  double   x,
  double   y);
static void line__CairoText_double_double
 (CairoTextImage * i,
  double   x,
  double   y);
static void close__CairoText
 (CairoTextImage * i);
static void fill__CairoText
 (CairoTextImage * i);
static void fillPreserve__CairoText
 (CairoTextImage * i);
static void stroke__CairoText
 (CairoTextImage * i);
static void strokePreserve__CairoText
 (CairoTextImage * i);
static void lineWidth__CairoText_double
 (CairoTextImage * i,
  double width);
static void clip__CairoText_rectangle
 (CairoTextImage  * i,
  Rectangle r);
static void leftArrow__CairoText_rectangle_colour_colour
 (CairoTextImage  * i,
  Rectangle r,
  Colour    s,
  Colour    f);
static void leftArrowWithCircle__CairoText_rectangle_colour_colour
 (CairoTextImage  * i,
  Rectangle r,
  Colour    s,
  Colour    f);
static void rightArrow__CairoText_rectangle_colour_colour
 (CairoTextImage  * i,
  Rectangle r,
  Colour    s,
  Colour    f);
static void rectangle__CairoText_rectangle_colour
 (CairoTextImage  * i,
  Rectangle r,
  Colour    c);
static void rectangleWH__CairoText_double_double_double_double
 (CairoTextImage * i,
  double   x,
  double   y,
  double   w,
  double   h);
static void rectangleLine__CairoText_rectangle_colour_colour
 (CairoTextImage  * i,
  Rectangle r,
  Colour    c);
static void assertCairoTextImageFile
 (char       *       imageFile,
  const char * const digest);
static void assertCairoTextResult
 (CairoTextImage     *       i,
  const char * const digest);
static void saveAsPng_CairoText_string
 (CairoTextImage     *       i,
  char       *       imageFile,
  const char * const digest);
static void addChar__CairoTextCompactList
 (CairoTextCompactList * c,
  char           a);
static void removeChar__CairoTextCompactList
 (CairoTextCompactList * c);
static void scrollPageDown__CairoTextCompactList
 (CairoTextCompactList * c);
static void scrollPageUp__CairoTextCompactList
 (CairoTextCompactList * c);
static void scrollToCursor__CairoTextCompactList
 (CairoTextCompactList * c);
static void arrowLeft__CairoTextCompactList
 (CairoTextCompactList * c);
static void arrowRight__CairoTextCompactList
 (CairoTextCompactList * c);
static void arrowUp__CairoTextCompactList
 (CairoTextCompactList * c);
static void arrowDown__CairoTextCompactList
 (CairoTextCompactList * c);
static void draw__CairoTextCompactList
 (CairoTextCompactList * compactList);
static void layout__CairoTextCompactList
 (CairoTextCompactList * compactList);
static void drawOrLayout__CairoTextCompactList
 (CairoTextCompactList * compactList,
  int            drawing);
struct ProtoTypes_CairoTextCompactList {
  void  (*addChar)(                                                             // Add a character to the selection prefix
    CairoTextCompactList * c,                                                   // Compact list
    char a);                                                                    // Character to add
  void  (*arrowDown)(                                                           // Move the entry under the cursor one click down
    CairoTextCompactList * c);                                                  // Compact list
  void  (*arrowLeft)(                                                           // Move the entry under the cursor one click to the left
    CairoTextCompactList * c);                                                  // Compact list
  void  (*arrowRight)(                                                          // Move the entry under the cursor one click to the right
    CairoTextCompactList * c);                                                  // Compact list
  void  (*arrowUp)(                                                             // Move the entry under the cursor one click up
    CairoTextCompactList * c);                                                  // Compact list
  void  (*drawOrLayout)(                                                        // Draw or layout a compact list
    CairoTextCompactList * compactList,                                         // Compact list
    int drawing);                                                               // Drawing if true - else trial layout
  void  (*draw)(                                                                // Draw a compact list
    CairoTextCompactList * compactList);                                        // Compact list
  void  (*layout)(                                                              // Layout a compact list
    CairoTextCompactList * compactList);                                        // Compact list
  void  (*removeChar)(                                                          // Remove the latest character from the selection prefix
    CairoTextCompactList * c);                                                  // Compact list
  void  (*scrollPageDown)(                                                      // Scroll a compact list down one page
    CairoTextCompactList * c);                                                  // Compact list
  void  (*scrollPageUp)(                                                        // Scroll a compact list up one page
    CairoTextCompactList * c);                                                  // Compact list
  void  (*scrollToCursor)(                                                      // Scroll to bring the entry under the cursor into view
    CairoTextCompactList * c);                                                  // Compact list
 } const ProtoTypes_CairoTextCompactList =
{addChar__CairoTextCompactList, arrowDown__CairoTextCompactList, arrowLeft__CairoTextCompactList, arrowRight__CairoTextCompactList, arrowUp__CairoTextCompactList, drawOrLayout__CairoTextCompactList, draw__CairoTextCompactList, layout__CairoTextCompactList, removeChar__CairoTextCompactList, scrollPageDown__CairoTextCompactList, scrollPageUp__CairoTextCompactList, scrollToCursor__CairoTextCompactList};
CairoTextCompactList newCairoTextCompactList(CairoTextCompactList allocator) {return allocator;}

struct ProtoTypes_CairoTextFont {
  void  (*free)(                                                                // Free a font if it has been loaded
    CairoTextFont * font);                                                      // CairoTextFont
 } const ProtoTypes_CairoTextFont =
{free__CairoTextFont};
CairoTextFont newCairoTextFont(CairoTextFont allocator) {return allocator;}

struct ProtoTypes_CairoTextImage {
  void  (*assertCairoTextResult)(                                               // Check the image via a digest
    CairoTextImage     * i,                                                     // CairoTextImage
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
  void  (*fillPreserve)(                                                        // Fill the current path and keep the path.
    CairoTextImage * i);                                                        // CairoTextImage
  void  (*fill)(                                                                // Fill the current path and delete the path.
    CairoTextImage * i);                                                        // CairoTextImage
  void  (*fontMetrics)(                                                         // Load the metrics of the current font
    CairoTextImage * i);                                                        // CairoTextImage
  void  (*fontSize)(                                                            // Set the size of a font
    CairoTextImage * i,                                                         // CairoTextImage
    int size);                                                                  // Size
  void  (*font)(                                                                // Start using a font
    CairoTextImage    * i,                                                      // CairoTextImage
    enum CairoTextFonts fontNumber);                                            // Font to use
  void  (*free)(                                                                // Free an image
    CairoTextImage * i);                                                        // CairoTextImage
  void  (*leftArrowWithCircle)(                                                 // Draw a left pointing arrow with a central circle cut out
    CairoTextImage  * i,                                                        // Image
    Rectangle r,                                                                // Rectangle to draw arrow in
    Colour s,                                                                   // Start colour
    Colour f);                                                                  // Finish colour
  void  (*leftArrow)(                                                           // Draw a left pointing arrow in the specified rectangle with a linear gradient starting and ending with the specified colours
    CairoTextImage  * i,                                                        // Image
    Rectangle r,                                                                // Rectangle to draw arrow in
    Colour s,                                                                   // Start colour
    Colour f);                                                                  // Finish colour
  void  (*lineWidth)(                                                           // Set the stroke width
    CairoTextImage * i,                                                         // CairoTextImage
    double width);                                                              // Width of lines
  void  (*line)(                                                                // Draw a line to the specified position from the current position
    CairoTextImage * i,                                                         // CairoTextImage
    double x,                                                                   // X coordinate to move to
    double y);                                                                  // Y coordinate to move to
  CairoTextCompactList  (*makeCairoTextCompactList)(                            // Define a compact list
    CairoTextImage  * image,                                                    // Image in which to draw the compact list
    ArenaList list,                                                             // Arena list of possibilities
    Rectangle drawTable);                                                       // Rectangle defining drawing area for the list
  void  (*move)(                                                                // Move to a position without drawing
    CairoTextImage * i,                                                         // CairoTextImage
    double x,                                                                   // X coordinate to move to
    double y);                                                                  // Y coordinate to move to
  void  (*rectangleLine)(                                                       // Draw a rectangle in outline
    CairoTextImage  * i,                                                        // Image
    Rectangle r,                                                                // Rectangle
    Colour c);                                                                  // Colour
  void  (*rectangleWH)(                                                         // Draw a rectangle using the last colour set
    CairoTextImage * i,                                                         // Image
    double x,                                                                   // Left x
    double y,                                                                   // Upper y
    double w,                                                                   // Width
    double h);                                                                  // Height
  void  (*rectangle)(                                                           // Draw a rectangle
    CairoTextImage  * i,                                                        // Image
    Rectangle r,                                                                // Rectangle
    Colour c);                                                                  // Colour
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
    CairoTextImage  * i,                                                        // Image
    Rectangle r,                                                                // Rectangle to draw arrow in
    Colour s,                                                                   // Start colour
    Colour f);                                                                  // Finish colour
  void  (*saveAsPng)(                                                           // Save a copy of the drawing surface to the specified file
    CairoTextImage     * i,                                                     // CairoTextImage
    char       * imageFile,                                                     // Image file name
    const char * const digest);                                                 // Expected digest
  void  (*save)(                                                                // Save the drawing context
    CairoTextImage * i);                                                        // CairoTextImage
  void  (*strokePreserve)(                                                      // Stroke the current path and keep the path.
    CairoTextImage * i);                                                        // CairoTextImage
  void  (*stroke)(                                                              // Stroke the current path and delete the path.
    CairoTextImage * i);                                                        // CairoTextImage
  double  (*textAdvance)(                                                       // Get the width of the specified text
    CairoTextImage * i,                                                         // CairoTextImage
    char   * s);                                                                // String
  void  (*textFit)(                                                             // Draw text so that it fills a rectangle in one dimension and is justified as specified in the other dimension.
    CairoTextImage     * i,                                                     // CairoTextImage
    Rectangle rc,                                                               // Rectangle in which to draw text
    const int jX,                                                               // < 0 justify left, > 0 justify right,  0 : center
    const int jY,                                                               // < 0 justify top,  > 0 justify bottom, 0 : center
    const int outline,                                                          // 0 : fill, 1 - outline
    const char * text);                                                         // The text to draw
  void  (*textLine)(                                                            // Outline text at the specified position using the current font, fonet size and colour
    CairoTextImage     * i,                                                     // CairoTextImage
    double x,                                                                   // X position of text
    double y,                                                                   // Y position of the upper edge of the text - i.e. the text will be drawn below this value.
    const char * t);                                                            // The text to draw
  void  (*text)(                                                                // Draw text at the specified position using the current font, fonet size and colour
    CairoTextImage     * i,                                                     // CairoTextImage
    double x,                                                                   // X position of text
    double y,                                                                   // Y position of the upper edge of the text - i.e. the text will be drawn below this value.
    const char * t);                                                            // The text to draw
 } const ProtoTypes_CairoTextImage =
{assertCairoTextResult, clearWhite__CairoText, clip__CairoText_rectangle, close__CairoText, colour__CairoText_colour, fillPreserve__CairoText, fill__CairoText, fontMetrics__CairoText, fontSize__CairoText_size, font__CairoText_enum, free__CairoTextImage, leftArrowWithCircle__CairoText_rectangle_colour_colour, leftArrow__CairoText_rectangle_colour_colour, lineWidth__CairoText_double, line__CairoText_double_double, makeCairoTextCompactList, move__CairoText_double_double, rectangleLine__CairoText_rectangle_colour_colour, rectangleWH__CairoText_double_double_double_double, rectangle__CairoText_rectangle_colour, restore__CairoText, rgb__CairoText_double_double_double, rgba__CairoText_double_double_double_double, rightArrow__CairoText_rectangle_colour_colour, saveAsPng_CairoText_string, save__CairoText, strokePreserve__CairoText, stroke__CairoText, textAdvance__CairoText_string, textFit__CairoText_rectangle_int_int_int_string, textLine__CairoText_double_double_string, text__CairoText_double_double_string};
CairoTextImage newCairoTextImage(CairoTextImage allocator) {return allocator;}

