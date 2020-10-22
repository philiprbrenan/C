#line 1 "/home/phil/c/z/cairoText/cairoText.c"
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
#ifndef CairoText_included
#define CairoText_included

typedef struct CairoTextFont                                                            // Details of a font
 {const struct ProtoTypes_CairoTextFont *proto;                                         // Methods associated with an arena tree
  StringBuffer        file;                                                     // Name of the file containing the font
  FT_Face             freeTypeFontFace;                                         // Free type description of font
  cairo_font_face_t * cairoFontFace;                                            // Cairo dscription of font
 } CairoTextFont;

typedef struct CairoTextImage                                                           // Create an image
 {const struct ProtoTypes_CairoTextImage *proto;                                        // Methods associated with an arena tree
  cairo_t           * cr;                                                       // Cairo
  cairo_surface_t   * surface;                                                  // Surface on which we are drawing
  FT_Library          freeTypeLibrary;                                          // Free Type library
  int width, height;                                                            // Dimensions of the surface
  const StringBuffer  out;                                                      // The output image file

  CairoTextFont sansMonoBold;                                                           // Mono fonts
  CairoTextFont sansMono;

  CairoTextFont sansBold;                                                               // Sans fonts
  CairoTextFont sansBoldItalic;
  CairoTextFont sansItalic;
  CairoTextFont sans;

  CairoTextFont serifBold;                                                              // Serif fonts
  CairoTextFont serifBoldItalic;
  CairoTextFont serifItalic;
  CairoTextFont serif;
  double fontAscent, fontDescent, fontHeight;                                   // Metrics for the current font
 } CairoTextImage;

typedef struct CairoTextTabList                                                         // A tabbed list
 {const struct ProtoTypes_CairoTextTabList *proto;                                        // Methods associated with an arena tree
 } CairoTextTabList;

#include <cairoText_prototypes.h>

//D1 Constructors                                                               // Construct text in an image

CairoTextImage makeCairoTextImage                                                               // Create an image by drawing on a surface
 (void (*draw)(CairoTextImage i),                                                       // Draw routine
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

  const typeof(makeStringBufferFromString(imageFile)) iFile = makeStringBufferFromString(imageFile);                                // Image file name

  typeof(newCairoTextImage(({struct CairoTextImage t = {out: iFile, cr: cr, surface: surface, width: x, height: y, proto: &ProtoTypes_CairoTextImage}; t;}))) i = newCairoTextImage(({struct CairoTextImage t = {out: iFile, cr: cr, surface: surface, width: x, height: y, proto: &ProtoTypes_CairoTextImage}; t;}));    // Create image

  i.sansBoldItalic  = makeCairoTextFont("NotoSans-BoldItalic.ttf" );
  i.sansBold        = makeCairoTextFont("NotoSans-Bold.ttf"       );
  i.sansItalic      = makeCairoTextFont("NotoSans-Italic.ttf"     );
  i.sans            = makeCairoTextFont("NotoSans-Regular.ttf"    );

  i.sansMonoBold    = makeCairoTextFont("NotoSansMono-Bold.ttf"   );
  i.sansMono        = makeCairoTextFont("NotoSansMono-Regular.ttf");

  i.serifBoldItalic = makeCairoTextFont("NotoSerif-BoldItalic.ttf");
  i.serifBold       = makeCairoTextFont("NotoSerif-Bold.ttf"      );
  i.serifItalic     = makeCairoTextFont("NotoSerif-Italic.ttf"    );
  i.serif           = makeCairoTextFont("NotoSerif-Regular.ttf"   );

  const typeof(FT_Init_FreeType(&i.freeTypeLibrary)) e1 = FT_Init_FreeType(&i.freeTypeLibrary);                                    // Initialize FreeType library
  if (e1) printStackBackTraceAndExit(1, "Error %d in free type\n", e1);

  i.proto->font(&i, i.sansMono);                                                         // Default font
  i.proto->clearWhite(&i);                                                               // Clear display
  i.proto->rgb(&i, 0,0,0);                                                               // Default drawing colour
  i.proto->lineWidth(&i, 2);                                                             // Default stroke width

  draw(i);                                                                      // Draw

  cairo_destroy(cr);                                                            // Save results
  cairo_surface_write_to_png(surface, imageFile);
  cairo_surface_destroy (surface);

  i.proto->assertCairoTextResult(&i, expected);                                                  // Check results
  return i;
 } // makeCairoTextImage

static CairoTextFont makeCairoTextFont                                                          // Record a font file description
 (char * fontFile)                                                              // Font file base name
 {StringBuffer f = makeStringBuffer();
  f.proto->add(&f, developmentMode() ? "/usr/share/fonts/truetype/noto/" :               // Location of font
                              "/home/runner/work/C/C/fonts/");
  f.proto->add(&f, fontFile);
  f.proto->join(&f);
  return newCairoTextFont(({struct CairoTextFont t = {file: f, proto: &ProtoTypes_CairoTextFont}; t;}));
 }

//D1 Free                                                                       // Free cairo and free type resources associated with the image

static void free_CairoTextImage                                                         // Free an image
 (CairoTextImage * i)                                                                   // CairoTextImage
 {i->out.proto->free(&i->out);

  CairoTextFont *fonts[] =
   {&i->sansBoldItalic, &i->sansBold, &i->sansItalic,      &i->sans,
    &i->sansMonoBold,   &i->sansMono, &i->serifBoldItalic, &i->serifBold,
    &i->serifItalic,    &i->serif, 0};

  for(CairoTextFont **f = fonts; *f; ++f) {CairoTextFont *F = *f; F->proto->free(F);}                    // Free any fonts that were loaded


  FT_Done_FreeType(i->freeTypeLibrary);                                         // Finished with FreeType library
 }

static void free_CairoTextFont                                                          // Free a font if it has been loaded
 (CairoTextFont * font)                                                                 // CairoTextFont
 {if (font->cairoFontFace)
   {cairo_font_face_destroy(font->cairoFontFace);
    FT_Done_Face           (font->freeTypeFontFace);
    font->file.proto->free(&font->file);
   }
 }

//D1 Text                                                                       // Work with text

static void font_CairoText                                                              // Start using a font
 (CairoTextImage * i,                                                                   // CairoTextImage
  CairoTextFont font)                                                                   // Font to use
 {if (!font.cairoFontFace)                                                      // Load the font if this font has not yet been loaded.
   {makeLocalCopyOfStringBuffer(ff, l, font.file);
    const typeof(FT_New_Face(i->freeTypeLibrary, ff, 0, &font.freeTypeFontFace)) e2 = FT_New_Face(i->freeTypeLibrary, ff, 0, &font.freeTypeFontFace);
    if (e2 == FT_Err_Unknown_File_Format) printStackBackTraceAndExit
     (1, "FontFace not supported: %s\n", ff);
    else if (e2) printStackBackTraceAndExit
     (1, "FontFace failed: %d %s\n", e2, ff);

    font.cairoFontFace = cairo_ft_font_face_create_for_ft_face(font.freeTypeFontFace, 0);
   }

  cairo_set_font_face (i->cr, font.cairoFontFace);                              // Set the font as the currently active one

  i->proto->fontMetrics(i);
 }

static void fontMetrics_CairoText                                                       // Load the metrics of the current font
 (CairoTextImage * i)                                                                   // CairoTextImage
 {cairo_font_extents_t fontExtents;
  cairo_font_extents (i->cr, &fontExtents);

  i->fontAscent  = fontExtents.ascent;                                          // Ascent from base line
  i->fontDescent = fontExtents.descent;                                         // Descent from base line
  i->fontHeight  = i->fontAscent + i->fontDescent;                              // Interline height
 }

static void fontSize_CairoText                                                          // Set the size of a font
 (CairoTextImage * i,                                                                   // CairoTextImage
  int    size)                                                                  // Size
 {cairo_set_font_size(i->cr, size);                                             // Set font size
  i->proto->fontMetrics(i);
 }

static double textAdvance_CairoText                                                     // Get the width of the specified text
 (CairoTextImage * i,                                                                   // CairoTextImage
  char * s)                                                                     // String
 {const typeof(i->cr) cr = i->cr;
  cairo_text_extents_t textExtents;
  cairo_text_extents (cr, s, &textExtents);
  return textExtents.x_advance;
 }

static void text_CairoText                                                              // Draw text at the specified position using the current font, fonet size and colour
 (CairoTextImage * i,                                                                   // CairoTextImage
  double x,                                                                     // X position of text
  double y,                                                                     // Y position of the upper edge of the text - i.e. the text will be drawn below this value.
  const char * t)                                                               // The text to draw
 {cairo_move_to  (i->cr, x, y + i->fontAscent);
  cairo_show_text(i->cr, t);
 }

static void textLine_CairoText                                                          // Outline text at the specified position using the current font, fonet size and colour
 (CairoTextImage * i,                                                                   // CairoTextImage
  double x,                                                                     // X position of text
  double y,                                                                     // Y position of the upper edge of the text - i.e. the text will be drawn below this value.
  const char * t)                                                               // The text to draw
 {const typeof(i->cr) cr = i->cr;
  cairo_move_to  (cr, x, y + i->fontAscent);
  cairo_text_path(cr, t);
  cairo_stroke   (cr);
 }

static void textFit_CairoText                                                           // Draw text so that it fills a rectangle in one dimension and is justified as specified in the other dimension.
 (CairoTextImage     * i,                                                               // CairoTextImage
  Rectangle    rc,                                                              // Rectangle in which to draw text
  const int    jX,                                                              // < 0 justify left, > 0 justify right,  0 : center
  const int    jY,                                                              // < 0 justify top,  > 0 justify bottom, 0 : center
  const int    line,                                                            // 0 : fill, 1 - outline
  const char * text)                                                            // The text to draw
 {const typeof(i->cr) cr = i->cr;
  const typeof(1000.0) base = 1000.0;                                                                // Size to scale from
  i->proto->save(i);
  i->proto->move(i, 0, 0);
  i->proto->fontSize(i, base);                                                           // Set font size
  cairo_text_extents_t textExtents;
  cairo_text_extents (cr, text, &textExtents);
  const typeof(textExtents.width  / rc.proto->width(&rc)) rx = textExtents.width  / rc.proto->width(&rc);
  const typeof(textExtents.height / rc.proto->height(&rc)) ry = textExtents.height / rc.proto->height(&rc);
  const typeof(rx > ry ? rx : ry) r = rx > ry ? rx : ry;
  const typeof(textExtents.width  / r) tw = textExtents.width  / r;                                                  // Scaled width of text
  const typeof(textExtents.height / r) th = textExtents.height / r;                                                  // Scaled height of text
  i->proto->fontSize(i, base / r);                                                       // Set font size
  const typeof(rc.proto->width(&rc)  - tw) dx = rc.proto->width(&rc)  - tw;                                                        // Left over in x
  const typeof(rc.proto->height(&rc) - th) dy = rc.proto->height(&rc) - th;                                                        // Left over in y
  const typeof(jX < 0 ? 0 : jX > 0 ? dx : dx / 2) jx = jX < 0 ? 0 : jX > 0 ? dx : dx / 2;
  const typeof(jY < 0 ? 0 : jY > 0 ? dy : dy / 2) jy = jY < 0 ? 0 : jY > 0 ? dy : dy / 2;
  const typeof(textExtents.x_bearing / r) xb = textExtents.x_bearing / r;
  const typeof(textExtents.y_bearing / r) yb = textExtents.y_bearing / r;
  const typeof(rc.x + jx - xb) x = rc.x + jx - xb;
  const typeof(rc.y + jy - yb) y = rc.y + jy - yb;
  if (line) i->proto->textLine(i, x, y - i->fontAscent, text);                           // Outline or fill the text
  else      i->proto->text(i, x, y - i->fontAscent, text);
  i->proto->restore(i);
 }

//D1 Colours                                                                    // Colours

static void clearWhite_CairoText                                                        // Clear the drawing surface to white
 (CairoTextImage * i)                                                                   // CairoTextImage
 {const typeof(i->cr) cr = i->cr;
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint         (cr);
 }

static void colour_CairoText                                                            // Set the current colour
 (CairoTextImage * i,                                                                   // CairoTextImage
  Colour c)                                                                     // Colour
 {const typeof(i->cr) cr = i->cr;
  cairo_set_source_rgba(cr, c.r, c.g, c.b, c.a);
 }

static void rgb_CairoText                                                               // Set the current colour
 (CairoTextImage * i,                                                                   // CairoTextImage
  double r,                                                                     // Red
  double g,                                                                     // Green
  double b)                                                                     // Blue
 {cairo_set_source_rgb(i->cr, r, g, b);
 }

static void rgba_CairoText                                                              // Set the current colour
 (CairoTextImage * i,                                                                   // CairoTextImage
  double r,                                                                     // Red
  double g,                                                                     // Green
  double b,                                                                     // Blue
  double a)                                                                     // Alpha
 {cairo_set_source_rgba(i->cr, r, g, b, a);
 }

//D1 Drawing                                                                    // Draw objects

static void save_CairoText                                                              // Save the drawing context
 (CairoTextImage * i)                                                                   // CairoTextImage
 {cairo_save(i->cr);
 }

static void restore_CairoText                                                           // Restore the drawing context
 (CairoTextImage * i)                                                                   // CairoTextImage
 {cairo_restore(i->cr);
 }

static void move_CairoText                                                              // Move to a position without drawing
 (CairoTextImage * i,                                                                   // CairoTextImage
  double x,                                                                     // X coordinate to move to
  double y)                                                                     // Y coordinate to move to
 {cairo_move_to(i->cr, x, y);
 }

static void line_CairoText                                                              // Draw a line to the specified position from the current position
 (CairoTextImage * i,                                                                   // CairoTextImage
  double x,                                                                     // X coordinate to move to
  double y)                                                                     // Y coordinate to move to
 {cairo_line_to(i->cr, x, y);
 }

static void close_CairoText                                                             // Close the current path
 (CairoTextImage * i)                                                                   // CairoTextImage
 {cairo_close_path(i->cr);
 }

static void fill_CairoText                                                              // Fill the current path and delete the path.
 (CairoTextImage * i)                                                                   // CairoTextImage
 {cairo_fill(i->cr);
 }

static void fillPreserve_CairoText                                                      // Fill the current path and keep the path.
 (CairoTextImage * i)                                                                   // CairoTextImage
 {cairo_fill_preserve(i->cr);
 }

static void stroke_CairoText                                                            // Stroke the current path and delete the path.
 (CairoTextImage * i)                                                                   // CairoTextImage
 {cairo_stroke(i->cr);
 }

static void strokePreserve_CairoText                                                    // Stroke the current path and keep the path.
 (CairoTextImage * i)                                                                   // CairoTextImage
 {cairo_stroke_preserve(i->cr);
 }

static void lineWidth_CairoText                                                         // Set the stroke width
 (CairoTextImage * i,                                                                   // CairoTextImage
  double width)                                                                 // Width of lines
 {cairo_set_line_width (i->cr, width);
 }

//D1 Clipping                                                                   // Set the clip area

static void clip_CairoText                                                              // Set the clip area
 (CairoTextImage  * i,                                                                  // CairoTextImage
  Rectangle r)                                                                  // Rectangle
 {const typeof(i->cr) cr = i->cr;
  cairo_rectangle(cr, r.x, r.y, r.proto->width(&r), r.proto->height(&r));
  cairo_clip     (cr);
 }

//D1 Shapes                                                                     // Draw shapes

static void leftArrow                                                           // Draw a left pointing arrow in the specified rectangle with a linear gradient starting and ending with the specified colours
 (CairoTextImage  * i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle to draw arrow in
  Colour    s,                                                                  // Start colour
  Colour    f)                                                                  // Finish colour
 {typeof(i->cr) cr = i->cr;

  i->proto->save(i);
  cairo_pattern_t *lg = cairo_pattern_create_linear(r.x, r.y, r.X, r.y);
  cairo_pattern_add_color_stop_rgba(lg, 0, s.r, s.g, s.b, s.a);
  cairo_pattern_add_color_stop_rgba(lg, 1, f.r, f.g, f.b, f.a);

  i->proto->move(i, r.x, r.y + r.proto->height(&r) / 2);
  i->proto->line(i, r.X, r.y);
  i->proto->line(i, r.X, r.Y);
  i->proto->close(i);

  cairo_set_source (cr, lg);
  i->proto->fill(i);
  cairo_pattern_destroy(lg);
  i->proto->restore(i);
 }

static void leftArrowWithCircle                                                 // Draw a left pointing arrow with a central circle cut out
 (CairoTextImage  * i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle to draw arrow in
  Colour    s,                                                                  // Start colour
  Colour    f)                                                                  // Finish colour
 {typeof(i->cr) cr = i->cr;

  i->proto->save(i);
  cairo_pattern_t *lg = cairo_pattern_create_linear(r.x, r.y, r.X, r.y);
  cairo_pattern_add_color_stop_rgba(lg, 0, s.r, s.g, s.b, s.a);
  cairo_pattern_add_color_stop_rgba(lg, 1, f.r, f.g, f.b, f.a);

  i->proto->move(i, r.x, r.y + r.proto->height(&r) / 2);
  i->proto->line(i, r.X, r.y);
  i->proto->line(i, r.X, r.Y);
  i->proto->close(i);

  const typeof(r.proto->width(&r)) w = r.proto->width(&r); const typeof(r.proto->height(&r)) h = r.proto->height(&r);
  cairo_new_sub_path(cr);
  cairo_arc         (cr, r.x + w * 7 / 12, r.y + h / 2, w / 3, 0,  2 * M_PI);
  i->proto->close(i);

  cairo_set_source     (cr, lg);
  cairo_set_fill_rule  (cr, CAIRO_FILL_RULE_EVEN_ODD);
  i->proto->fill(i);
  cairo_pattern_destroy(lg);
  i->proto->restore(i);
 }

static void rightArrow                                                          // Draw a right pointing arrow in the specified rectangle with a linear gradient starting and ending with the specified colours
 (CairoTextImage  * i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle to draw arrow in
  Colour    s,                                                                  // Start colour
  Colour    f)                                                                  // Finish colour
 {typeof(i->cr) cr = i->cr;

  i->proto->save(i);
  cairo_pattern_t *lg = cairo_pattern_create_linear(r.x, r.y, r.X, r.y);
  cairo_pattern_add_color_stop_rgba(lg, 0, s.r, s.g, s.b, s.a);
  cairo_pattern_add_color_stop_rgba(lg, 1, f.r, f.g, f.b, f.a);

  i->proto->move(i, r.X, r.y + r.proto->height(&r) / 2);
  i->proto->line(i, r.x, r.y);
  i->proto->line(i, r.x, r.Y);
  i->proto->close(i);

  cairo_set_source (cr, lg);
  i->proto->fill(i);
  cairo_pattern_destroy(lg);
  i->proto->restore(i);
 }

static void rectangle                                                           // Draw a rectangle
 (CairoTextImage  * i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle
  Colour    c)                                                                  // Colour
 {typeof(i->cr) cr = i->cr;
  i->proto->save(i);
  i->proto->colour(i, c);
  cairo_rectangle(cr, r.x, r.y, r.proto->width(&r), r.proto->height(&r));
  i->proto->fill(i);
  i->proto->restore(i);
 }

static void rectangleLine                                                       // Draw a rectangle in outline
 (CairoTextImage  * i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle
  Colour    c)                                                                  // Colour
 {typeof(i->cr) cr = i->cr;
  i->proto->save(i);
  i->proto->colour(i, c);
  cairo_rectangle(cr, r.x, r.y, r.proto->width(&r), r.proto->height(&r));
  i->proto->stroke(i);
  i->proto->restore(i);
 }

//D1 Output                                                                     // Write the image and check the results

static void assertCairoTextImageFile                                                    //P Check that the digest of an image file contains the specified string
 (char       *       imageFile,                                                 // Image file name
  const char * const digest)                                                    // Expected digest
 {      typeof(makeStringBufferFromString("b2sum ")) c = makeStringBufferFromString("b2sum ");
        c.proto->add(&c, imageFile);
        c.proto->system(&c);

  if (! c.proto->containsString(&c, digest))
   {makeLocalCopyOfStringBuffer(d, l, c);
    say("\nImage file: %s\n",   imageFile);
    say("Have: %s\nWant: %s\n", d, digest);
    assert(0);
   }

  c.proto->free(&c);
 }

static void assertCairoTextResult                                                       // Check the image via a digest
 (CairoTextImage     *       i,                                                         // CairoTextImage
  const char * const digest)                                                    // Expected digest
 {makeLocalCopyOfStringBuffer(s, l, i->out);
  assertCairoTextImageFile(s, digest);
 }

static void saveAsPng_CairoText_string                                                  // Save a copy of the drawing surface to the specified file
 (CairoTextImage     *       i,                                                         // CairoTextImage
  char       *       imageFile,                                                 // Image file name
  const char * const digest)                                                    // Expected digest
 {cairo_surface_write_to_png(i->surface, imageFile);
  assertCairoTextImageFile(imageFile, digest);
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TcreateImage
 {void draw(CairoTextImage i)                                                           // Draw some text
   {i.proto->fontSize(&i, 20);
    i.proto->rgb(&i, 1, 0, 0);

    const typeof("Hello World How is it whirling?") text = "Hello World How is it whirling?";

    for(size_t j = 0; j < 100; ++j) i.proto->text(&i, 0, j * i.fontHeight, text);
   }

  typeof(makeCairoTextImage(draw, 2000, 2000, "CairoText0.png", "8915")) i = makeCairoTextImage(draw, 2000, 2000, "CairoText0.png", "8915");                           // Create image containing some text and check its digest
  i.proto->free(&i);
 }

void test1()                                                                    // Linear gradient
 {const typeof(makeColour(1,0,0,1)) red = makeColour(1,0,0,1);
  const typeof(makeColour(0,0,1,1)) blue = makeColour(0,0,1,1);
  void draw(CairoTextImage i)
   {const typeof(i.width) w = i.width; const typeof(i.height) h = i.height;
    typeof(makeRectangleWH    (w/4, 0, w/4, h/2)) r = makeRectangleWH    (w/4, 0, w/4, h/2);
    const typeof(r.proto->translate(&r, w/2, 0)) s = r.proto->translate(&r, w/2, 0);
    i.proto->leftArrowWithCircle(&i, r, red, blue);
    i.proto->rightArrow(&i, s, red, blue);
   }

  typeof(makeCairoTextImage(draw, 1000, 2000, "CairoText1.png", "55d2")) i = makeCairoTextImage(draw, 1000, 2000, "CairoText1.png", "55d2");
  i.proto->free(&i);
 }

void test2()                                                                    // Text filling rectangles
 {const typeof(makeColour(0,0,0,1)) black = makeColour(0,0,0,1);
  void draw(CairoTextImage i)
   {const typeof(i.width) w = i.width; const typeof(i.height) h = i.height;
    const typeof(makeRectangleWH(w/4, h/4, w/2, h/2)) r = makeRectangleWH(w/4, h/4, w/2, h/2);
    i.proto->rgb(&i, 1,0,0);
    i.proto->textFit(&i, r, -1, 0, 0, "1");
    i.proto->rgb(&i, 0,1,0);
    i.proto->textFit(&i, r,  0, 0, 0, "2");
    i.proto->rgb(&i, 0,0,1);
    i.proto->textFit(&i, r, +1, 0, 0, "3");
    i.proto->rgb(&i, 1,1,0);
    i.proto->textFit(&i, r, 0, -1, 0, "Hello");
    i.proto->rgb(&i, 1,0,1);
    i.proto->textFit(&i, r, 0,  0, 0, "Hello");
    i.proto->rgb(&i, 0,1,1);
    i.proto->textFit(&i, r, 0, +1, 0, "Hello");
    i.proto->rectangleLine(&i, r, black);
   }

  typeof(makeCairoTextImage(draw, 2000, 2000, "CairoText2.png", "8185")) i = makeCairoTextImage(draw, 2000, 2000, "CairoText2.png", "8185");
  i.proto->free(&i);
 }

void test3()                                                                    // Text table
 {void draw(CairoTextImage i)
   {const typeof(makeColour(0,0,0,1)) black = makeColour(0,0,0,1);

    const typeof(makeArenaListFromLinesOfWords("aaaa bbbb cc d\n a bb ccc dddd\n a b c d")) table = makeArenaListFromLinesOfWords("aaaa bbbb cc d\n a bb ccc dddd\n a b c d");

    i.proto->font(&i, i.serif);                                                      // Font
    i.proto->fontSize(&i, 100);                                                          // Size of text
    i.proto->colour(&i, black);                                                        // Colour of text

    const typeof(makeRectangleWH(500, 500, 500, 800)) drawTable = makeRectangleWH(500, 500, 500, 800);                            // Drawing area
    i.proto->clip(&i, drawTable);

    size_t x = drawTable.x, y = drawTable.y;
    ArenaListFe(col, table)                                                     // Each column
     {typeof(0ul) max = 0ul;
      y = drawTable.y;
      ArenaListfe(row, col)                                                     // Each row
       {makeLocalCopyOfArenaListKey(k, l, row);
        i.proto->text(&i, x, y, k);
        y += i.fontHeight;
        const typeof(i.proto->textAdvance(&i, k)) a = i.proto->textAdvance(&i, k);
        if (max < a) max = a;
       }
      x += max + i.fontDescent;
     }
   }

  typeof(makeCairoTextImage(draw, 2000, 2000, "CairoText3.png", "ba23")) i = makeCairoTextImage(draw, 2000, 2000, "CairoText3.png", "ba23");
  i.proto->free(&i);
 }

void test4()                                                                    // Text table using tab stops
 {void draw(CairoTextImage i)
   {  const typeof(makeColour(0,0,0,1)) frameColour = makeColour(0,0,0,1);
       const typeof(makeColour(0,0,0,1)) textColour = makeColour(0,0,0,1);
    const typeof(makeColour(0,0,1,0.3)) pointedColour = makeColour(0,0,1,0.3);
    const typeof(makeColour(0,1,0,1)) textEnteredSoFarColour = makeColour(0,1,0,1);

    const typeof(makeArenaListFromWords("aaaa qqbbbb qqcc qqdd qqee qqff qqgggg qqhh qqiii qqjj qqkkk qql mmmm nn oo ppppp qq rrrr s tttttt uuuu v wwwwww xxx yy zz")) list = makeArenaListFromWords("aaaa qqbbbb qqcc qqdd qqee qqff qqgggg qqhh qqiii qqjj qqkkk qql mmmm nn oo ppppp qq rrrr s tttttt uuuu v wwwwww xxx yy zz");
    const typeof("qqx") textEnteredSoFar = "qqx";                                                   // Assume the user has entered some text to narrow the possibilities displayed
    const typeof(2ul) textEnteredSoFarLength = 2ul;                                               // length of text entered so far
    typeof(717ul) px = 717ul; typeof(717ul) py = 717ul;                                                     // Current pointer coordinates
    const typeof(2ul) startAtOffset = 2ul;                                                   // Start drawing at this entry
    typeof(0ul) firstOffset = 0ul;                                                   // First visible entry
    typeof(0ul) lastOffset = 0ul;                                                   // Last visible entry
    const typeof(list.proto->findFirst(&list, "qqee")) cursorEntry = list.proto->findFirst(&list, "qqee");                              // Cursor entry
    const typeof(cursorEntry.offset) cursorOffset = cursorEntry.offset;                                    // Offset of entry containing cursor
    typeof(0ul) cursorPrevOffset = 0ul;                                                   // Offset of entry preceding entry containing cursor
    typeof(0ul) cursorNextOffset = 0ul;                                                   // Offset of entry following entry containing cursor
    typeof(0ul) cursorUpOffset = 0ul;                                                   // Offset of entry above entry containing cursor
    typeof(0ul) cursorDownOffset = 0ul;                                                   // Offset of entry below entry containing cursor
    typeof(0ul) pointerOffset = 0ul;                                                   // Offset of entry containing cursor

    const typeof(makeRectangleWH(500, 500, 500, 500)) drawTable = makeRectangleWH(500, 500, 500, 500);                            // Drawing area
    i.proto->clip(&i, drawTable);

    if (1)                                                                      // Show text entered so far
     {i.proto->colour(&i, textEnteredSoFarColour);
      i.proto->font(&i, i.sansItalic);
      const typeof(textEnteredSoFarLength) l = textEnteredSoFarLength;
      char t[l+1]; strncpy(t, textEnteredSoFar, l); t[l] = 0;
      i.proto->textFit(&i, drawTable, 0, 0, 0, t);
     }

    i.proto->font(&i, i.serif);                                                      // Font for remaining possibilities
    i.proto->fontSize(&i, 100);                                                          // Size of text
    i.proto->colour(&i, textColour);                                                   // Colour of text
    const typeof(i.fontHeight) H = i.fontHeight;                                                           // Font height - we should use heoght not ascent but height is actually too much

    double x = drawTable.X, y = drawTable.y - H;                                // At the end of the previous line
    typeof(0ul) cursorEntryIndex = 0ul;                                                       // Whether we have drawn the cursor entry yet.
    const typeof(list.proto->countChildren(&list)) N = list.proto->countChildren(&list);                                                   // Maximum number of entries
    size_t    entryOffset    [N+1]; memset(entryOffset, 0, sizeof(entryOffset));// Offset for each entry
    Rectangle entryRectangles[N+1];                                             // Rectangle for each entry up to next tab stop

    ArenaListfec(word, list)                                                    // Each word
     {if (wordⁱ >= startAtOffset)                                               // Words in the scrolled to area
       {makeLocalCopyOfArenaListKey(K, L, word);
        if (L <= textEnteredSoFarLength) continue;                              // Word shorter than prefix entered so far
        if (strncmp(K, textEnteredSoFar, textEnteredSoFarLength)) continue;    // Word does not match prefix entered so far
        const typeof(&K[textEnteredSoFarLength]) k = &K[textEnteredSoFarLength];                                         // Skip text entered so far
        const typeof(i.proto->textAdvance(&i, k)) a = i.proto->textAdvance(&i, k);                                                 // Width of text
        if (x + a > drawTable.X) {x = drawTable.x; y += H;}                     // Move to next line if necessary
        const typeof(word.offset) offset = word.offset;                                                   // Offset of current entry

        const typeof(makeRectangleWH(x, y, a, H)) r = makeRectangleWH(x, y, a, H);                                        // Rectangle in which to draw the text

        if (drawTable.proto->contains(&drawTable, r))                                            // Draw visible elements
         {if (offset == cursorOffset)                                           // Reached entry under cursor
           {i.proto->rectangle(&i, r, pointedColour);                                    // Background colour of entry containing the cursor
            cursorEntryIndex = wordⁱ;                                           // Show that the cursor entry has now been drawn
           }
          if (r.proto->containsPoint(&r, px, py)) pointerOffset = offset;                // Offset of item containing pointer
          i.proto->text(&i, x, y, k);                                                    // Draw the remaining text of the entry
          if (!firstOffset) firstOffset = offset;                               // First offset visible in drawing area
          lastOffset = offset;                                                  // Last offset visible in drawing area
         }

        const typeof(H * ceil(a / H)) w = H * ceil(a / H);                                                    // Width of current entry including move to next tab stop
        if      (!cursorEntryIndex)              cursorPrevOffset = offset;     // Entry just before cursor
        else if  (cursorEntryIndex + 1 == wordⁱ) cursorNextOffset = offset;     // Entry just after cursor
        const typeof(makeRectangleWH(x, y, w, H)) R = makeRectangleWH(x, y, w, H);
        memcpy(&entryRectangles[wordⁱ], &R, sizeof(R));                         // Rectangle containing this entry up to the next tab stop
        entryOffset    [wordⁱ] = offset;                                        // Offset for this entry
        x += w;                                                                 // Move to next entry position
       }
     }

    typeof(0.0) bestAreaUp = 0.0; typeof(0.0) bestAreaDown = 0.0;                                       // Locate the rectangles that match the best for up and down arrows
    typeof(entryRectangles[cursorEntryIndex]) cer = entryRectangles[cursorEntryIndex];
    const typeof(cer.proto->translate(&cer, 0, +H)) cerd = cer.proto->translate(&cer, 0, +H);
    const typeof(cer.proto->translate(&cer, 0, -H)) ceru = cer.proto->translate(&cer, 0, -H);

    for(size_t i = 1; i <= N; ++i)                                              // Each entry
     {const typeof(entryOffset[i]) o = entryOffset[i];                                                       // Offset of this entry
      if (o)
       {    const typeof(entryRectangles[i]) r = entryRectangles[i];                                             // Rectangle for entry
        const typeof(r.proto->intersectionArea(&r, cerd)) d = r.proto->intersectionArea(&r, cerd);                                         // Area of overlap with down rectangle
        const typeof(r.proto->intersectionArea(&r, ceru)) u = r.proto->intersectionArea(&r, ceru);                                         // Area of overlap with up rectangle
        if (d > bestAreaDown) {bestAreaDown = d; cursorDownOffset = o;}         // Better down
        if (u > bestAreaUp)   {bestAreaUp   = u; cursorUpOffset   = o;}         // Better up
       }
     }

    i.proto->rectangleLine(&i, drawTable, frameColour);                                  // Frame the drawn area

    if (1)                                                                      // Check results
     {const typeof(list.proto->offset(&list, firstOffset)) f = list.proto->offset(&list, firstOffset);      assert( f.proto->equalsString(&f, "qqbbbb"));
      const typeof(list.proto->offset(&list, lastOffset)) l = list.proto->offset(&list, lastOffset);       assert( l.proto->equalsString(&l, "qqhh"));
      const typeof(list.proto->offset(&list, cursorOffset)) c = list.proto->offset(&list, cursorOffset);     assert( c.proto->equalsString(&c, "qqee"));
      const typeof(list.proto->offset(&list, cursorPrevOffset)) p = list.proto->offset(&list, cursorPrevOffset); assert( p.proto->equalsString(&p, "qqdd"));
      const typeof(list.proto->offset(&list, cursorNextOffset)) n = list.proto->offset(&list, cursorNextOffset); assert( n.proto->equalsString(&n, "qqff"));
      const typeof(list.proto->offset(&list, cursorDownOffset)) d = list.proto->offset(&list, cursorDownOffset); assert( d.proto->equalsString(&d, "qqgggg"));
      const typeof(list.proto->offset(&list, cursorUpOffset)) u = list.proto->offset(&list, cursorUpOffset);   assert( u.proto->equalsString(&u, "qqbbbb"));
      const typeof(list.proto->offset(&list, pointerOffset)) P = list.proto->offset(&list, pointerOffset);    assert( P.proto->equalsString(&P, "qqee"));
     }
   }

  typeof(makeCairoTextImage(draw, 2000, 2000, "CairoText4.png", "9449")) i = makeCairoTextImage(draw, 2000, 2000, "CairoText4.png", "9449");
  i.proto->free(&i);
 }

int main (void)
 {void (*tests[])(void) = {test0, test1, test2, test3, test4, 0};
  run_tests("CairoText", 1, tests);
  return 0;
}
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/g/cairoText/cairoText
