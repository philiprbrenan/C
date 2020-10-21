//------------------------------------------------------------------------------
// Draw text using Cairo
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <math.h>
#include <cairo.h>
#include <cairo-ft.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <colour.c>
#include <rectangle.c>
#include <stringBuffer.c>
#include <utilities.c>
#define SIZE 2000

//D1 Structures                                                                 // Structures describing an Arena Tree
#ifndef $_included
#define $_included

typedef struct $Font                                                            // Details of a font
 {const struct ProtoTypes_$Font *proto;                                         // Methods associated with an arena tree
  StringBuffer        file;                                                     // Name of the file containing the font
  FT_Face             freeTypeFontFace;                                         // Free type description of font
  cairo_font_face_t * cairoFontFace;                                            // Cairo dscription of font
 } $Font;

typedef struct $Image                                                           // Create an image
 {const struct ProtoTypes_$Image *proto;                                        // Methods associated with an arena tree
  cairo_t           * cr;                                                       // Cairo
  cairo_surface_t   * surface;                                                  // Surface on which we are drawing
  FT_Library          freeTypeLibrary;                                          // Free Type library
  int width, height;                                                            // Dimensions of the surface
  const StringBuffer  out;                                                      // The output image file

  $Font sansMonoBold;                                                           // Mono fonts
  $Font sansMono;

  $Font sansBold;                                                               // Sans fonts
  $Font sansBoldItalic;
  $Font sansItalic;
  $Font sans;

  $Font serifBold;                                                              // Serif fonts
  $Font serifBoldItalic;
  $Font serifItalic;
  $Font serif;
  double fontAscent, fontDescent, fontHeight;                                   // Metrics for the current font
 } $Image;

typedef struct $TabList                                                         // A tabbed list
 {const struct ProtoTypes_$TabList *proto;                                        // Methods associated with an arena tree
 } $TabList;

#include <$$_prototypes.h>

//D1 Constructors                                                               // Construct text in an image

$Image make$Image                                                               // Create an image by drawing on a surface
 (void (*draw)($Image i),                                                       // Draw routine
  int x,                                                                        // Dimension of image in x
  int y,                                                                        // Dimension of image in y
  const char * const imageFile,                                                 // Name of output file
  const char * const expected)                                                  // Part of the expected digest of the image produced
 {char fontFile[128]; strcpy(fontFile, developmentMode() ?                      // Font file
  "/usr/share/fonts/truetype/noto/NotoSansMono-Regular.ttf":
     "/home/runner/work/C/C/fonts/NotoSansMono-Regular.ttf");

  cairo_surface_t * surface = cairo_image_surface_create                        // Cairo
   (CAIRO_FORMAT_ARGB32, x, y);
  cairo_t         *    cr   = cairo_create (surface);
  cairo_set_antialias (cr, CAIRO_ANTIALIAS_BEST);

  iFile ◁ makeStringBufferFromString(imageFile);                                // Image file name

  i ◀ new $Image(out: iFile, cr: cr, surface: surface, width: x, height: y);    // Create image

  i.sansBoldItalic  = make$Font("NotoSans-BoldItalic.ttf" );
  i.sansBold        = make$Font("NotoSans-Bold.ttf"       );
  i.sansItalic      = make$Font("NotoSans-Italic.ttf"     );
  i.sans            = make$Font("NotoSans-Regular.ttf"    );

  i.sansMonoBold    = make$Font("NotoSansMono-Bold.ttf"   );
  i.sansMono        = make$Font("NotoSansMono-Regular.ttf");

  i.serifBoldItalic = make$Font("NotoSerif-BoldItalic.ttf");
  i.serifBold       = make$Font("NotoSerif-Bold.ttf"      );
  i.serifItalic     = make$Font("NotoSerif-Italic.ttf"    );
  i.serif           = make$Font("NotoSerif-Regular.ttf"   );

  e1 ◁ FT_Init_FreeType(&i.freeTypeLibrary);                                    // Initialize FreeType library
  if (e1) printStackBackTraceAndExit(1, "Error %d in free type\n", e1);

  i ▷ font(i.sansMono);                                                         // Default font
  i ▷ clearWhite;                                                               // Clear display
  i ▷ rgb(0,0,0);                                                               // Default drawing colour
  i ▷ lineWidth(2);                                                             // Default stroke width

  draw(i);                                                                      // Draw

  cairo_destroy(cr);                                                            // Save results
  cairo_surface_write_to_png(surface, imageFile);
  cairo_surface_destroy (surface);

  i ▷ assert$Result(expected);                                                  // Check results
  return i;
 } // make$Image

static $Font make$Font                                                          // Record a font file description
 (char * fontFile)                                                              // Font file base name
 {StringBuffer f = makeStringBuffer();
  f ▷ add(developmentMode() ? "/usr/share/fonts/truetype/noto/" :               // Location of font
                              "/home/runner/work/C/C/fonts/");
  f ▷ add(fontFile);
  f ▷ join;
  return new $Font(file: f);
 }

//D1 Free                                                                       // Free cairo and free type resources associated with the image

static void free_$Image                                                         // Free an image
 ($Image * i)                                                                   // $Image
 {i->out ▷ free;

  $Font *fonts[] =
   {&i->sansBoldItalic, &i->sansBold, &i->sansItalic,      &i->sans,
    &i->sansMonoBold,   &i->sansMono, &i->serifBoldItalic, &i->serifBold,
    &i->serifItalic,    &i->serif, 0};

  for($Font **f = fonts; *f; ++f) {$Font *F = *f; F ▶ free;}                    // Free any fonts that were loaded


  FT_Done_FreeType(i->freeTypeLibrary);                                         // Finished with FreeType library
 }

static void free_$Font                                                          // Free a font if it has been loaded
 ($Font * font)                                                                 // $Font
 {if (font->cairoFontFace)
   {cairo_font_face_destroy(font->cairoFontFace);
    FT_Done_Face           (font->freeTypeFontFace);
    font->file ▷ free;
   }
 }

//D1 Text                                                                       // Work with text

static void font_$                                                              // Start using a font
 ($Image * i,                                                                   // $Image
  $Font font)                                                                   // Font to use
 {if (!font.cairoFontFace)                                                      // Load the font if this font has not yet been loaded.
   {makeLocalCopyOfStringBuffer(ff, l, font.file);
    e2 ◁ FT_New_Face(i->freeTypeLibrary, ff, 0, &font.freeTypeFontFace);
    if (e2 == FT_Err_Unknown_File_Format) printStackBackTraceAndExit
     (1, "FontFace not supported: %s\n", ff);
    else if (e2) printStackBackTraceAndExit
     (1, "FontFace failed: %d %s\n", e2, ff);

    font.cairoFontFace = cairo_ft_font_face_create_for_ft_face(font.freeTypeFontFace, 0);
   }

  cairo_set_font_face (i->cr, font.cairoFontFace);                              // Set the font as the currently active one

  i ▶ fontMetrics;
 }

static void fontMetrics_$                                                       // Load the metrics of the current font
 ($Image * i)                                                                   // $Image
 {cairo_font_extents_t fontExtents;
  cairo_font_extents (i->cr, &fontExtents);

  i->fontAscent  = fontExtents.ascent;                                          // Ascent from base line
  i->fontDescent = fontExtents.descent;                                         // Descent from base line
  i->fontHeight  = i->fontAscent + i->fontDescent;                              // Interline height
 }

static void fontSize_$                                                          // Set the size of a font
 ($Image * i,                                                                   // $Image
  int    size)                                                                  // Size
 {cairo_set_font_size(i->cr, size);                                             // Set font size
  i ▶ fontMetrics;
 }

static double textAdvance_$                                                     // Get the width of the specified text
 ($Image * i,                                                                   // $Image
  char * s)                                                                     // String
 {cr ◁ i->cr;
  cairo_text_extents_t textExtents;
  cairo_text_extents (cr, s, &textExtents);
  return textExtents.x_advance;
 }

static void text_$                                                              // Draw text at the specified position using the current font, fonet size and colour
 ($Image * i,                                                                   // $Image
  double x,                                                                     // X position of text
  double y,                                                                     // Y position of the upper edge of the text - i.e. the text will be drawn below this value.
  const char * t)                                                               // The text to draw
 {cairo_move_to  (i->cr, x, y + i->fontAscent);
  cairo_show_text(i->cr, t);
 }

static void textLine_$                                                          // Outline text at the specified position using the current font, fonet size and colour
 ($Image * i,                                                                   // $Image
  double x,                                                                     // X position of text
  double y,                                                                     // Y position of the upper edge of the text - i.e. the text will be drawn below this value.
  const char * t)                                                               // The text to draw
 {cr ◁ i->cr;
  cairo_move_to  (cr, x, y + i->fontAscent);
  cairo_text_path(cr, t);
  cairo_stroke   (cr);
 }

static void textFit_$                                                           // Draw text so that it fills a rectangle in one dimension and is justified as specified in the other dimension.
 ($Image     * i,                                                               // $Image
  Rectangle    rc,                                                              // Rectangle in which to draw text
  const int    jX,                                                              // < 0 justify left, > 0 justify right,  0 : center
  const int    jY,                                                              // < 0 justify top,  > 0 justify bottom, 0 : center
  const int    line,                                                            // 0 : fill, 1 - outline
  const char * text)                                                            // The text to draw
 {cr ◁ i->cr;
  base ◁ 1000.0;                                                                // Size to scale from
  i ▶ save;
  i ▶ move(0, 0);
  i ▶ fontSize(base);                                                           // Set font size
  cairo_text_extents_t textExtents;
  cairo_text_extents (cr, text, &textExtents);
  rx ◁ textExtents.width  / rc ▷ width;
  ry ◁ textExtents.height / rc ▷ height;
  r  ◁ rx > ry ? rx : ry;
  tw ◁ textExtents.width  / r;                                                  // Scaled width of text
  th ◁ textExtents.height / r;                                                  // Scaled height of text
  i ▶ fontSize(base / r);                                                       // Set font size
  dx ◁ rc ▷ width  - tw;                                                        // Left over in x
  dy ◁ rc ▷ height - th;                                                        // Left over in y
  jx ◁ jX < 0 ? 0 : jX > 0 ? dx : dx / 2;
  jy ◁ jY < 0 ? 0 : jY > 0 ? dy : dy / 2;
  xb ◁ textExtents.x_bearing / r;
  yb ◁ textExtents.y_bearing / r;
  x ◁ rc.x + jx - xb;
  y ◁ rc.y + jy - yb;
  if (line) i ▶ textLine (x, y - i->fontAscent, text);                           // Outline or fill the text
  else      i ▶ text     (x, y - i->fontAscent, text);
  i ▶ restore;
 }

//D1 Colours                                                                    // Colours

static void clearWhite_$                                                        // Clear the drawing surface to white
 ($Image * i)                                                                   // $Image
 {cr ◁ i->cr;
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint         (cr);
 }

static void colour_$                                                            // Set the current colour
 ($Image * i,                                                                   // $Image
  Colour c)                                                                     // Colour
 {cr ◁ i->cr;
  cairo_set_source_rgba(cr, c.r, c.g, c.b, c.a);
 }

static void rgb_$                                                               // Set the current colour
 ($Image * i,                                                                   // $Image
  double r,                                                                     // Red
  double g,                                                                     // Green
  double b)                                                                     // Blue
 {cairo_set_source_rgb(i->cr, r, g, b);
 }

static void rgba_$                                                              // Set the current colour
 ($Image * i,                                                                   // $Image
  double r,                                                                     // Red
  double g,                                                                     // Green
  double b,                                                                     // Blue
  double a)                                                                     // Alpha
 {cairo_set_source_rgba(i->cr, r, g, b, a);
 }

//D1 Drawing                                                                    // Draw objects

static void save_$                                                              // Save the drawing context
 ($Image * i)                                                                   // $Image
 {cairo_save(i->cr);
 }

static void restore_$                                                           // Restore the drawing context
 ($Image * i)                                                                   // $Image
 {cairo_restore(i->cr);
 }

static void move_$                                                              // Move to a position without drawing
 ($Image * i,                                                                   // $Image
  double x,                                                                     // X coordinate to move to
  double y)                                                                     // Y coordinate to move to
 {cairo_move_to(i->cr, x, y);
 }

static void line_$                                                              // Draw a line to the specified position from the current position
 ($Image * i,                                                                   // $Image
  double x,                                                                     // X coordinate to move to
  double y)                                                                     // Y coordinate to move to
 {cairo_line_to(i->cr, x, y);
 }

static void close_$                                                             // Close the current path
 ($Image * i)                                                                   // $Image
 {cairo_close_path(i->cr);
 }

static void fill_$                                                              // Fill the current path and delete the path.
 ($Image * i)                                                                   // $Image
 {cairo_fill(i->cr);
 }

static void fillPreserve_$                                                      // Fill the current path and keep the path.
 ($Image * i)                                                                   // $Image
 {cairo_fill_preserve(i->cr);
 }

static void stroke_$                                                            // Stroke the current path and delete the path.
 ($Image * i)                                                                   // $Image
 {cairo_stroke(i->cr);
 }

static void strokePreserve_$                                                    // Stroke the current path and keep the path.
 ($Image * i)                                                                   // $Image
 {cairo_stroke_preserve(i->cr);
 }

static void lineWidth_$                                                         // Set the stroke width
 ($Image * i,                                                                   // $Image
  double width)                                                                 // Width of lines
 {cairo_set_line_width (i->cr, width);
 }

//D1 Clipping                                                                   // Set the clip area

static void clip_$                                                              // Set the clip area
 ($Image  * i,                                                                  // $Image
  Rectangle r)                                                                  // Rectangle
 {cr ◁ i->cr;
  cairo_rectangle(cr, r.x, r.y, r ▷ width, r ▷ height);
  cairo_clip     (cr);
 }

//D1 Shapes                                                                     // Draw shapes

static void leftArrow                                                           // Draw a left pointing arrow in the specified rectangle with a linear gradient starting and ending with the specified colours
 ($Image  * i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle to draw arrow in
  Colour    s,                                                                  // Start colour
  Colour    f)                                                                  // Finish colour
 {cr ◀ i->cr;

  i ▶ save;
  cairo_pattern_t *lg = cairo_pattern_create_linear(r.x, r.y, r.X, r.y);
  cairo_pattern_add_color_stop_rgba(lg, 0, s.r, s.g, s.b, s.a);
  cairo_pattern_add_color_stop_rgba(lg, 1, f.r, f.g, f.b, f.a);

  i ▶ move(r.x, r.y + r ▷ height / 2);
  i ▶ line(r.X, r.y);
  i ▶ line(r.X, r.Y);
  i ▶ close;

  cairo_set_source (cr, lg);
  i ▶ fill;
  cairo_pattern_destroy(lg);
  i ▶ restore;
 }

static void leftArrowWithCircle                                                 // Draw a left pointing arrow with a central circle cut out
 ($Image  * i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle to draw arrow in
  Colour    s,                                                                  // Start colour
  Colour    f)                                                                  // Finish colour
 {cr ◀ i->cr;

  i ▶ save;
  cairo_pattern_t *lg = cairo_pattern_create_linear(r.x, r.y, r.X, r.y);
  cairo_pattern_add_color_stop_rgba(lg, 0, s.r, s.g, s.b, s.a);
  cairo_pattern_add_color_stop_rgba(lg, 1, f.r, f.g, f.b, f.a);

  i ▶ move(r.x, r.y + r ▷ height / 2);
  i ▶ line(r.X, r.y);
  i ▶ line(r.X, r.Y);
  i ▶ close;

  w ◁ r ▷ width; h ◁ r ▷ height;
  cairo_new_sub_path(cr);
  cairo_arc         (cr, r.x + w * 7 / 12, r.y + h / 2, w / 3, 0,  2 * M_PI);
  i ▶ close;

  cairo_set_source     (cr, lg);
  cairo_set_fill_rule  (cr, CAIRO_FILL_RULE_EVEN_ODD);
  i ▶ fill;
  cairo_pattern_destroy(lg);
  i ▶ restore;
 }

static void rightArrow                                                          // Draw a right pointing arrow in the specified rectangle with a linear gradient starting and ending with the specified colours
 ($Image  * i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle to draw arrow in
  Colour    s,                                                                  // Start colour
  Colour    f)                                                                  // Finish colour
 {cr ◀ i->cr;

  i ▶ save;
  cairo_pattern_t *lg = cairo_pattern_create_linear(r.x, r.y, r.X, r.y);
  cairo_pattern_add_color_stop_rgba(lg, 0, s.r, s.g, s.b, s.a);
  cairo_pattern_add_color_stop_rgba(lg, 1, f.r, f.g, f.b, f.a);

  i ▶ move(r.X, r.y + r ▷ height / 2);
  i ▶ line(r.x, r.y);
  i ▶ line(r.x, r.Y);
  i ▶ close;

  cairo_set_source (cr, lg);
  i ▶ fill;
  cairo_pattern_destroy(lg);
  i ▶ restore;
 }

static void rectangle                                                           // Draw a rectangle
 ($Image  * i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle
  Colour    c)                                                                  // Colour
 {cr ◀ i->cr;
  i ▶ save;
  i ▶ colour(c);
  cairo_rectangle(cr, r.x, r.y, r ▷ width, r ▷ height);
  i ▶ fill;
  i ▶ restore;
 }

static void rectangleLine                                                       // Draw a rectangle in outline
 ($Image  * i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle
  Colour    c)                                                                  // Colour
 {cr ◀ i->cr;
  i ▶ save;
  i ▶ colour(c);
  cairo_rectangle(cr, r.x, r.y, r ▷ width, r ▷ height);
  i ▶ stroke;
  i ▶ restore;
 }

//D1 Output                                                                     // Write the image and check the results

static void assert$ImageFile                                                    //P Check that the digest of an image file contains the specified string
 (char       *       imageFile,                                                 // Image file name
  const char * const digest)                                                    // Expected digest
 {      c ◀ makeStringBufferFromString("b2sum ");
        c ▷ add(imageFile);
        c ▷ system;

  if (! c ▷ containsString(digest))
   {makeLocalCopyOfStringBuffer(d, l, c);
    say("\nImage file: %s\n",   imageFile);
    say("Have: %s\nWant: %s\n", d, digest);
    assert(0);
   }

  c ▷ free;
 }

static void assert$Result                                                       // Check the image via a digest
 ($Image     *       i,                                                         // $Image
  const char * const digest)                                                    // Expected digest
 {makeLocalCopyOfStringBuffer(s, l, i->out);
  assert$ImageFile(s, digest);
 }

static void saveAsPng_$_string                                                  // Save a copy of the drawing surface to the specified file
 ($Image     *       i,                                                         // $Image
  char       *       imageFile,                                                 // Image file name
  const char * const digest)                                                    // Expected digest
 {cairo_surface_write_to_png(i->surface, imageFile);
  assert$ImageFile(imageFile, digest);
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TcreateImage
 {void draw($Image i)                                                           // Draw some text
   {i ▷ fontSize(20);
    i ▷ rgb(1, 0, 0);

    text ◁ "Hello World How is it whirling?";

    for(size_t j = 0; j < 100; ++j) i ▷ text(0, j * i.fontHeight, text);
   }

  i ◀ make$Image(draw, 2000, 2000, "$0.png", "8915");                           // Create image containing some text and check its digest
  i ▷ free;
 }

void test1()                                                                    // Linear gradient
 {red  ◁ makeColour(1,0,0,1);
  blue ◁ makeColour(0,0,1,1);
  void draw($Image i)
   {w ◁ i.width; h ◁ i.height;
    r ◀ makeRectangleWH    (w/4, 0, w/4, h/2);
    s ◁ r ▷ translate      (w/2, 0);
    i ▷ leftArrowWithCircle(r, red, blue);
    i ▷ rightArrow         (s, red, blue);
   }

  i ◀ make$Image(draw, 1000, 2000, "$1.png", "55d2");
  i ▷ free;
 }

void test2()                                                                    // Text filling rectangles
 {black ◁ makeColour(0,0,0,1);
  void draw($Image i)
   {w ◁ i.width; h ◁ i.height;
    r ◁ makeRectangleWH(w/4, h/4, w/2, h/2);
    i ▷ rgb(1,0,0);
    i ▷ textFit(r, -1, 0, 0, "1");
    i ▷ rgb(0,1,0);
    i ▷ textFit(r,  0, 0, 0, "2");
    i ▷ rgb(0,0,1);
    i ▷ textFit(r, +1, 0, 0, "3");
    i ▷ rgb(1,1,0);
    i ▷ textFit(r, 0, -1, 0, "Hello");
    i ▷ rgb(1,0,1);
    i ▷ textFit(r, 0,  0, 0, "Hello");
    i ▷ rgb(0,1,1);
    i ▷ textFit(r, 0, +1, 0, "Hello");
    i ▷ rectangleLine(r, black);
   }

  i ◀ make$Image(draw, 2000, 2000, "$2.png", "8185");
  i ▷ free;
 }

void test3()                                                                    // Text table
 {void draw($Image i)
   {black ◁ makeColour(0,0,0,1);

    table ◁ makeArenaListFromLinesOfWords("aaaa bbbb cc d\n a bb ccc dddd\n a b c d");

    i ▷ font    (i.serif);                                                      // Font
    i ▷ fontSize(100);                                                          // Size of text
    i ▷ colour  (black);                                                        // Colour of text

    drawTable ◁ makeRectangleWH(500, 500, 500, 800);                            // Drawing area
    i ▷ clip(drawTable);

    size_t x = drawTable.x, y = drawTable.y;
    ArenaListFe(col, table)                                                     // Each column
     {max ◀ 0ul;
      y = drawTable.y;
      ArenaListfe(row, col)                                                     // Each row
       {makeLocalCopyOfArenaListKey(k, l, row);
        i ▷ text(x, y, k);
        y += i.fontHeight;
        a ◁ i ▷ textAdvance(k);
        if (max < a) max = a;
       }
      x += max + i.fontDescent;
     }
   }

  i ◀ make$Image(draw, 2000, 2000, "$3.png", "ba23");
  i ▷ free;
 }

void test4()                                                                    // Text table using tab stops
 {void draw($Image i)
   {  frameColour ◁ makeColour(0,0,0,1);
       textColour ◁ makeColour(0,0,0,1);
    pointedColour ◁ makeColour(0,0,1,0.3);
    textEnteredSoFarColour ◁ makeColour(0,1,0,1);

    list ◁ makeArenaListFromWords("aaaa qqbbbb qqcc qqdd qqee qqff qqgggg qqhh qqiii qqjj qqkkk qql mmmm nn oo ppppp qq rrrr s tttttt uuuu v wwwwww xxx yy zz");
    textEnteredSoFar ◁ "qqx";                                                   // Assume the user has entered some text to narrow the possibilities displayed
    textEnteredSoFarLength ◁ 2ul;                                                 // length of text entered so far
    px ◀ 717ul; py ◀ 717ul;                                                     // Current pointer coordinates
    startAtEntry      ◁ 2ul;                                                    // Start drawing at this entry
    firstEntry        ◀ 0ul;                                                    // First visible entry
    lastEntry         ◀ 0ul;                                                    // Last visible entry
    clickedEntry      ◀ 0ul;                                                    // Entry containing pointer coordinates
    clickedEntryIndex ◀ 0ul;                                                    // Index of entry containing pointer
    clickedPrevEntry  ◀ 0ul;                                                    // Offset of entry preceding entry containing pointer
    clickedNextEntry  ◀ 0ul;                                                    // Offset of entry following entry containing pointer
    clickedUpEntry    ◀ 0ul;                                                    // Offset of entry above entry containing pointer
    clickedDownEntry  ◀ 0ul;                                                    // Offset of entry below entry containing pointer

    drawTable ◁ makeRectangleWH(500, 500, 500, 500);                            // Drawing area
    i ▷ clip(drawTable);

    if (1)                                                                      // Show text entered so far
     {i ▷ colour (textEnteredSoFarColour);
      i ▷ font   (i.sansItalic);
      l ◁ textEnteredSoFarLength;
      char t[l+1]; strncpy(t, textEnteredSoFar, l); t[l] = 0;
      i ▷ textFit(drawTable, 0, 0, 0, t);
     }

    i ▷ font    (i.serif);                                                      // Font
    i ▷ fontSize(100);                                                          // Size of text
    i ▷ colour  (textColour);                                                   // Colour of text
    H ◁ i.fontHeight;                                                           // Font height - we should use heoght not ascent but height is actually too much

    double x = drawTable.X, y = drawTable.y - H;                                // At the end of the previous line

    ArenaListfec(word, list)                                                    // Each word
     {if (wordⁱ >= startAtEntry)                                                // Words in the scrolled to area
       {makeLocalCopyOfArenaListKey(K, L, word);
        if (L <= textEnteredSoFarLength) continue;                              // Word shorter than prefix entered so far
        k ◁ &K[textEnteredSoFarLength];                                         // Allow for text entered so far
        offset ◁ word.offset;                                                   // Offset of current entry
        a ◁ i ▷ textAdvance(k);                                                 // Width of text
        if (x + a > drawTable.X) {x = drawTable.x; y += H;}                     // Move to next line if necessary

        r ◁ makeRectangleWH(x, y, a, H);                                        // Rectangle in which to draw the text

        if (drawTable ▷ contains(r))
         {if (r ▷ containsPoint(px, py))
           {clickedEntry      = offset;
            clickedEntryIndex = wordⁱ;
            i ▷ rectangle(r, pointedColour);
           }

          if (!clickedEntry)                       clickedPrevEntry = offset;
          else if (wordⁱ == clickedEntryIndex + 1) clickedNextEntry = offset;

          i ▷ text(x, y, k);
          if (!firstEntry) firstEntry = offset;
          lastEntry = offset;
         }

        w ◁ H * ceil(a / H);                                                    // Width of entry including move to next tab stop
        if (1)                                                                  // Entries above and below
         {r ◁ makeRectangleWH(x, y, w, H);                                      // Rectangle in which to draw the entry
          if (r ▷ containsPoint(px, py - H)) clickedUpEntry   = offset;
          if (r ▷ containsPoint(px, py + H)) clickedDownEntry = offset;
         }
        x += w;                                                                 // Move to next entry
       }
     }

    i ▷ rectangleLine(drawTable, frameColour);                                  // Frame the drawn area

    if (0)                                                                      // Check results
     {f ◁ list ▷ offset(firstEntry);       ✓ f ▷ equalsString("qqbbbb");
      l ◁ list ▷ offset(lastEntry);        ✓ l ▷ equalsString("qqhh");
      c ◁ list ▷ offset(clickedEntry);     ✓ c ▷ equalsString("qqee");
      p ◁ list ▷ offset(clickedPrevEntry); ✓ p ▷ equalsString("qqdd");
      n ◁ list ▷ offset(clickedNextEntry); ✓ n ▷ equalsString("qqff");
      d ◁ list ▷ offset(clickedDownEntry); ✓ d ▷ equalsString("qqgggg");
      u ◁ list ▷ offset(clickedUpEntry);   ✓ u ▷ equalsString("qqbbbb");
     }
   }

  i ◀ make$Image(draw, 2000, 2000, "$4.png", "9449");
  i ▷ free;
 }

int main (void)
 {void (*tests[])(void) = {test0, test1, test2, test3, test4, 0};
  run_tests("$", 1, tests);
  return 0;
}
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/g/cairoText/cairoText
