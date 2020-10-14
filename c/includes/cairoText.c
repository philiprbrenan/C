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
 (CairoTextImage * i)                                                                     // CairoTextImage
 {i->out.proto->free(&i->out);

  CairoTextFont *fonts[] =
   {&i->sansBoldItalic, &i->sansBold, &i->sansItalic,      &i->sans,
    &i->sansMonoBold,   &i->sansMono, &i->serifBoldItalic, &i->serifBold,
    &i->serifItalic,    &i->serif};

  for(CairoTextFont **f = fonts; *f; ++f)                                               // Free any fonts that were loaded
   {CairoTextFont F = **f;
    F.proto->free(&F);
   }

  FT_Done_FreeType(i->freeTypeLibrary);                                          // Finished with FreeType library
 }

static void free_CairoTextFont                                                          // Free a font if it has been loaded
 (CairoTextFont * font)                                                                   // CairoTextFont
 {if (font->cairoFontFace)
   {cairo_font_face_destroy(font->cairoFontFace);
    FT_Done_Face           (font->freeTypeFontFace);
    font->file.proto->free(&font->file);
   }
 }

//D1 Text                                                                       // Work with text

static void font_CairoText                                                              // Start using a font
 (CairoTextImage * i,                                                                     // CairoTextImage
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

  cairo_set_font_face (i->cr, font.cairoFontFace);                               // Set the font as the currently active one

  i->proto->fontMetrics(i);
 }

static void fontMetrics_CairoText                                                       // Load the metrics of the current font
 (CairoTextImage * i)                                                                     // CairoTextImage
 {cairo_font_extents_t fontExtents;
  cairo_font_extents (i->cr, &fontExtents);

  i->fontAscent  = fontExtents.ascent;                                           // Ascent from base line
  i->fontDescent = fontExtents.descent;                                          // Descent from base line
  i->fontHeight  = i->fontAscent + i->fontDescent;                                 // Interline height
 }

static void fontSize_CairoText                                                          // Set the size of a font
 (CairoTextImage * i,                                                                     // CairoTextImage
  int    size)                                                                  // Size
 {cairo_set_font_size (i->cr, size);                                             // Set font size
  i->proto->fontMetrics(i);
say("BBBBB %f\n", i->fontHeight);
 }

static double textAdvance_CairoText                                                     // Get the width of the specified text
 (CairoTextImage * i,                                                                     // CairoTextImage
  char * s)                                                                     // String
 {const typeof(i->cr) cr = i->cr;
  cairo_text_extents_t textExtents;
  cairo_text_extents (cr, s, &textExtents);
  return textExtents.x_advance;
 }

static void text_CairoText                                                              // Draw text at the specified position using the current font, fonet size and colour
 (CairoTextImage * i,                                                                     // CairoTextImage
  double x,                                                                     // X position of text
  double y,                                                                     // Y position of the upper edge of the text - i.e. the text will be drawn below this value.
  const char * t)                                                               // The text to draw
 {cairo_move_to  (i->cr, x, y + i->fontAscent);
  cairo_show_text(i->cr, t);
 }

//D1 Colours                                                                    // Colours

static void clearWhite_CairoText                                                        // Clear the drawing surface to white
 (CairoTextImage * i)                                                         // CairoTextImage
 {const typeof(i->cr) cr = i->cr;
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint         (cr);
 }

static void colour_CairoText                                                            // Set the current colour
 (CairoTextImage * i,                                                                     // CairoTextImage
  Colour c)                                                                     // Colour
 {const typeof(i->cr) cr = i->cr;
  cairo_set_source_rgba(cr, c.r, c.g, c.b, c.a);
 }

static void rgb_CairoText                                                               // Set the current colour
 (CairoTextImage * i,                                                                     // CairoTextImage
  double r,                                                                     // Red
  double g,                                                                     // Green
  double b)                                                                     // Blue
 {cairo_set_source_rgb(i->cr, r, g, b);
 }

static void rgba_CairoText                                                              // Set the current colour
 (CairoTextImage * i,                                                                     // CairoTextImage
  double r,                                                                     // Red
  double g,                                                                     // Green
  double b,                                                                     // Blue
  double a)                                                                     // Alpha
 {cairo_set_source_rgba(i->cr, r, g, b, a);
 }

//D1 Clipping                                                                   // Set the clip area

static void clip_CairoText                                                              // Set the clip area
 (CairoTextImage   *  i,                                                                  // CairoTextImage
  Rectangle r)                                                                  // Rectangle
 {const typeof(i->cr) cr = i->cr;
  cairo_rectangle(cr, r.x, r.y, r.proto->width(&r), r.proto->height(&r));
  cairo_clip     (cr);
 }

//D1 Shapes                                                                     // Draw shapes

static void leftArrow                                                           // Draw a left pointing arrow in the specified rectangle with a linear gradient starting and ending with the specified colours
 (CairoTextImage   * i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle to draw arrow in
  Colour    s,                                                                  // Start colour
  Colour    f)                                                                  // Finish colour
 {typeof(i->cr) cr = i->cr;

  cairo_save(cr);
  cairo_pattern_t *lg = cairo_pattern_create_linear(r.x, r.y, r.X, r.y);
  cairo_pattern_add_color_stop_rgba(lg, 0, s.r, s.g, s.b, s.a);
  cairo_pattern_add_color_stop_rgba(lg, 1, f.r, f.g, f.b, f.a);

  cairo_move_to    (cr, r.x, r.y + r.proto->height(&r) / 2);
  cairo_line_to    (cr, r.X, r.y);
  cairo_line_to    (cr, r.X, r.Y);
  cairo_close_path (cr);

  cairo_set_source (cr, lg);
  cairo_fill       (cr);
  cairo_pattern_destroy(lg);
  cairo_restore(cr);
 }

static void leftArrowWithCircle                                                 // Draw a left pointing arrow with a central circle cut out
 (CairoTextImage   *  i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle to draw arrow in
  Colour    s,                                                                  // Start colour
  Colour    f)                                                                  // Finish colour
 {typeof(i->cr) cr = i->cr;

  cairo_save(cr);
  cairo_pattern_t *lg = cairo_pattern_create_linear(r.x, r.y, r.X, r.y);
  cairo_pattern_add_color_stop_rgba(lg, 0, s.r, s.g, s.b, s.a);
  cairo_pattern_add_color_stop_rgba(lg, 1, f.r, f.g, f.b, f.a);

  cairo_move_to        (cr, r.x, r.y + r.proto->height(&r) / 2);
  cairo_line_to        (cr, r.X, r.y);
  cairo_line_to        (cr, r.X, r.Y);
  cairo_close_path     (cr);

  const typeof(r.proto->width(&r)) w = r.proto->width(&r); const typeof(r.proto->height(&r)) h = r.proto->height(&r);
  cairo_new_sub_path   (cr);
  cairo_arc            (cr, r.x + w * 7 / 12, r.y + h / 2, w / 3, 0,  2 * M_PI);
  cairo_close_path     (cr);

  cairo_set_source     (cr, lg);
  cairo_set_fill_rule  (cr, CAIRO_FILL_RULE_EVEN_ODD);
  cairo_fill           (cr);
  cairo_pattern_destroy(lg);
  cairo_restore(cr);
 }

static void rightArrow                                                          // Draw a right pointing arrow in the specified rectangle with a linear gradient starting and ending with the specified colours
 (CairoTextImage   *  i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle to draw arrow in
  Colour    s,                                                                  // Start colour
  Colour    f)                                                                  // Finish colour
 {typeof(i->cr) cr = i->cr;

  cairo_save(cr);
  cairo_pattern_t *lg = cairo_pattern_create_linear(r.x, r.y, r.X, r.y);
  cairo_pattern_add_color_stop_rgba(lg, 0, s.r, s.g, s.b, s.a);
  cairo_pattern_add_color_stop_rgba(lg, 1, f.r, f.g, f.b, f.a);

  cairo_move_to    (cr, r.X, r.y + r.proto->height(&r) / 2);
  cairo_line_to    (cr, r.x, r.y);
  cairo_line_to    (cr, r.x, r.Y);
  cairo_close_path (cr);

  cairo_set_source (cr, lg);
  cairo_fill       (cr);
  cairo_pattern_destroy(lg);
  cairo_restore(cr);
 }

//D1 Output                                                                     // Write the image and check the results

static void assertCairoTextImageFile                                                    //P Check that the digest of an image file contains the specified string
 (char *             imageFile,                                                 // Image file name
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
 (CairoTextImage * i,                                                                     // CairoTextImage
  const char * const digest)                                                    // Expected digest
 {makeLocalCopyOfStringBuffer(s, l, i->out);
  assertCairoTextImageFile(s, digest);
 }

static void save_CairoText_string                                                       // Save a copy of the drawing surface to the specified file
 (CairoTextImage   *  i,                                                                  // CairoTextImage
  char *    imageFile,                                                          // Image file name
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
say("AAAA %f\n", i.fontHeight);
    i.proto->rgb(&i, 0, 0, 0);

    cairo_set_source_rgb(i.cr, 0.5, 0.5, 0.5);
    cairo_rectangle(i.cr, 0, 0, 1000, 1000);
    cairo_fill(i.cr);

    const typeof("Hello World How is it whirling?") text = "Hello World How is it whirling?";

    cairo_move_to       (i.cr, 1000, 1000);
    cairo_set_source_rgb(i.cr, 1, 0, 0);
    i.proto->text(&i, 1000, 100 * i.fontHeight, text);

    for(size_t j = 0; j < 100; ++j) i.proto->text(&i, 0, j * i.fontHeight, text);
   }

  typeof(makeCairoTextImage(draw, 2000, 2000, "CairoText0.png", "ab32")) i = makeCairoTextImage(draw, 2000, 2000, "CairoText0.png", "ab32");                           // Create image containing some text and check its digest
  i.proto->free(&i);
 }

void test1()                                                                    // Linear gradient
 {void draw(CairoTextImage i)
   {Colour red   = makeColour(1,0,0,1);
    Colour blue  = makeColour(0,0,1,1);

    const typeof(i.width) w = i.width; const typeof(i.height) h = i.height;
    typeof(makeRectangleWH(w/4, 0, w/4, h/2)) r = makeRectangleWH(w/4, 0, w/4, h/2);
    const typeof(r.proto->translate(&r, w/2, 0)) s = r.proto->translate(&r, w/2, 0);
    i.proto->leftArrowWithCircle(&i, r, red, blue);
    i.proto->rightArrow(&i, s, red, blue);
   }

  typeof(makeCairoTextImage(draw, 1000, 2000, "CairoText1.png", "a")) i = makeCairoTextImage(draw, 1000, 2000, "CairoText1.png", "a");
  i.proto->free(&i);
 }

void test2()                                                                    // Text table
 {void draw(CairoTextImage i)
   {Colour black = makeColour(0,0,0,1);

    const typeof(makeArenaListFromLinesOfWords("aaaa bbbb cc d\n a bb ccc dddd\n a b c d")) table = makeArenaListFromLinesOfWords("aaaa bbbb cc d\n a bb ccc dddd\n a b c d");

    i.proto->font(&i, i.serif);                                                          // Font
    i.proto->fontSize(&i, 100);                                                          // Size of text
    i.proto->colour(&i, black);                                                        // Colour of text

    const typeof(i.cr) cr = i.cr;
    cairo_font_extents_t fontExtents;
    cairo_font_extents (cr, &fontExtents);

    const typeof(fontExtents.ascent) A = fontExtents.ascent;                                                     // Descent from base line
    const typeof(fontExtents.descent) D = fontExtents.descent;                                                    // Descent from base line
    const typeof(A + D) H = A + D;                                                                  // Interline height

    const typeof(makeRectangleWH(500, 500, 500, 800)) drawTable = makeRectangleWH(500, 500, 500, 800);
    i.proto->clip(&i, drawTable);

    size_t x = drawTable.x, y = drawTable.y;
    ArenaListFe(col, table)
     {typeof(0ul) max = 0ul;
      cairo_move_to(cr, x, y = drawTable.y);
      ArenaListfe(row, col)
       {makeLocalCopyOfArenaListKey(k, l, row);
        cairo_move_to  (cr, x, y + A);
        cairo_show_text(cr, k);
        y += H;
        const typeof(i.proto->textAdvance(&i, k)) a = i.proto->textAdvance(&i, k);
        if (max < a) max = a;
       }
      x += max + fontExtents.descent;
     }
   }

  typeof(makeCairoTextImage(draw, 2000, 2000, "CairoText2.png", "a")) i = makeCairoTextImage(draw, 2000, 2000, "CairoText2.png", "a");
  i.proto->free(&i);
 }

int main (void)
 {void (*tests[])(void) = {test0, test1, test2, 0};
  run_tests("CairoText", 1, tests);
  return 0;
}
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/g/cairoText/cairoText
