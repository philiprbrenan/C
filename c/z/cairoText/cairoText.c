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
 } $Image;

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

  i ▷ setFont(i.sansMono);                                                      // Default font
  i ▷ clearWhite;                                                               // Clear display

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
 ($Image i)                                                                     // $Image
 {i.out ▷ free;

  $Font *fonts[] =
   {&i.sansBoldItalic, &i.sansBold, &i.sansItalic,      &i.sans,
    &i.sansMonoBold,   &i.sansMono, &i.serifBoldItalic, &i.serifBold,
    &i.serifItalic,    &i.serif};

  for($Font **f = fonts; *f; ++f)                                               // Free any fonts that were loaded
   {$Font F = **f;
    F ▷ free;
   }

  FT_Done_FreeType(i.freeTypeLibrary);                                          // Finished with FreeType library
 }

static void free_$Font                                                          // Free a font if it has been loaded
 ($Font font)                                                                   // $Font
 {if (font.cairoFontFace)
   {cairo_font_face_destroy(font.cairoFontFace);
    FT_Done_Face           (font.freeTypeFontFace);
    font.file ▷ free;
   }
 }

//D1 Methods                                                                    // Methods that operate on an $

static void setFont                                                             // Start using a font
 ($Image i,                                                                     // $Image
  $Font font)                                                                   // Font to use
 {if (!font.cairoFontFace)                                                      // Load the font if this font has not yet been loaded.
   {makeLocalCopyOfStringBuffer(ff, l, font.file);
    e2 ◁ FT_New_Face(i.freeTypeLibrary, ff, 0, &font.freeTypeFontFace);
    if (e2 == FT_Err_Unknown_File_Format) printStackBackTraceAndExit
     (1, "FontFace not supported: %s\n", ff);
    else if (e2) printStackBackTraceAndExit
     (1, "FontFace failed: %d %s\n", e2, ff);

    font.cairoFontFace = cairo_ft_font_face_create_for_ft_face(font.freeTypeFontFace, 0);
   }

  cairo_set_font_face (i.cr, font.cairoFontFace);                               // Set the font as the currently active one
 }

static void assert$ImageFile                                                    //P Check that the digest of an image file contains the specified string
 (char *             imageFile,                                                 // Image file name
  const char * const digest)                                                    // Expected digest
 {      c ◁ makeStringBufferFromString("b2sum ");
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
 ($Image i,                                                                     // $Image
  const char * const digest)                                                    // Expected digest
 {makeLocalCopyOfStringBuffer(s, l, i.out);
  assert$ImageFile(s, digest);
 }

static void save_$_string                                                       // Save a copy of the drawing surface to the specified file
 ($Image             i,                                                         // $Image
  char *             imageFile,                                                 // Image file name
  const char * const digest)                                                    // Expected digest
 {cairo_surface_write_to_png(i.surface, imageFile);
  assert$ImageFile(imageFile, digest);
 }

static void clearWhite_$                                                        // Clear the drawing surface to white
 ($Image             i)                                                         // $Image
 {cr ◁ i.cr;
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint         (cr);
 }

//D1 Shapes                                                                     // Draw shapes

static void leftArrow                                                           // Draw a left pointing arrow in the specified rectangle with a linear gradient starting and ending with the specified colours
 ($Image    i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle to draw arrow in
  Colour    s,                                                                  // Start colour
  Colour    f)                                                                  // Finish colour
 {cr ◀ i.cr;

  cairo_save(cr);
  cairo_pattern_t *lg = cairo_pattern_create_linear(r.x, r.y, r.X, r.y);
  cairo_pattern_add_color_stop_rgba(lg, 0, s.r, s.g, s.b, s.a);
  cairo_pattern_add_color_stop_rgba(lg, 1, f.r, f.g, f.b, f.a);

  cairo_move_to    (cr, r.x, r.y + r ▷ height / 2);
  cairo_line_to    (cr, r.X, r.y);
  cairo_line_to    (cr, r.X, r.Y);
  cairo_close_path (cr);

  cairo_set_source (cr, lg);
  cairo_fill       (cr);
  cairo_pattern_destroy(lg);
  cairo_restore(cr);
 }

static void leftArrowWithCircle                                                 // Draw a left pointing arrow with a central circle cut out
 ($Image    i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle to draw arrow in
  Colour    s,                                                                  // Start colour
  Colour    f)                                                                  // Finish colour
 {cr ◀ i.cr;

  cairo_save(cr);
  cairo_pattern_t *lg = cairo_pattern_create_linear(r.x, r.y, r.X, r.y);
  cairo_pattern_add_color_stop_rgba(lg, 0, s.r, s.g, s.b, s.a);
  cairo_pattern_add_color_stop_rgba(lg, 1, f.r, f.g, f.b, f.a);

  cairo_move_to        (cr, r.x, r.y + r ▷ height / 2);
  cairo_line_to        (cr, r.X, r.y);
  cairo_line_to        (cr, r.X, r.Y);
  cairo_close_path     (cr);

  w ◁ r ▷ width; h ◁ r ▷ height;
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
 ($Image    i,                                                                  // Image
  Rectangle r,                                                                  // Rectangle to draw arrow in
  Colour    s,                                                                  // Start colour
  Colour    f)                                                                  // Finish colour
 {cr ◀ i.cr;

  cairo_save(cr);
  cairo_pattern_t *lg = cairo_pattern_create_linear(r.x, r.y, r.X, r.y);
  cairo_pattern_add_color_stop_rgba(lg, 0, s.r, s.g, s.b, s.a);
  cairo_pattern_add_color_stop_rgba(lg, 1, f.r, f.g, f.b, f.a);

  cairo_move_to    (cr, r.X, r.y + r ▷ height / 2);
  cairo_line_to    (cr, r.x, r.y);
  cairo_line_to    (cr, r.x, r.Y);
  cairo_close_path (cr);

  cairo_set_source (cr, lg);
  cairo_fill       (cr);
  cairo_pattern_destroy(lg);
  cairo_restore(cr);
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TcreateImage
 {void draw($Image i)                                                           // Draw some text
   {cr ◀ i.cr;
    cairo_set_font_size (cr, 20);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_font_extents_t fontExtents;
    cairo_font_extents (cr, &fontExtents);

    const char * const text = "Hello World How is it whirling?";
    for(size_t j = 0; j < 100; ++j)
     {cairo_move_to(cr, 0, (j + 1) * fontExtents.height);
      for(size_t i = 0; i < strlen(text); ++i)
       {char c[2]; c[0] = text[i]; c[1] = 0;
        cairo_show_text(cr, c);
       }
     }
   }

  i ◁ make$Image(draw, 2000, 2000, "$0.png", "ab32");                           // Create image containing some text and check its digest
  i ▷ free;
 }

void test1()                                                                    // Linear gradient
 {void draw($Image i)
   {Colour red   = makeColour(1,0,0,1);
    Colour blue  = makeColour(0,0,1,1);

    w ◁ i.width; h ◁ i.height;
    r ◁ makeRectangleWH(w/4, 0, w/4, h/2);
    s ◁ r ▷ translate(w/2, 0);
    i ▷ leftArrowWithCircle(r, red, blue);
    i ▷ rightArrow(s, red, blue);
   }

  i ◁ make$Image(draw, 1000, 2000, "$1.png", "a");
  i ▷ free;
 }

void test2()                                                                    // Text table
 {void draw($Image i)
   {//Colour white = makeColour(1,1,1,1);
    //Colour black = makeColour(0,0,0,1);
    Colour red   = makeColour(1,0,0,1);
    //Colour green = makeColour(0,1,0,1);
    Colour blue  = makeColour(0,0,1,1);

    table ◁ makeArenaList();

    r1 ◁ table ▷ node("aaaa", 4); r1 ▷ putTreeLast;

    w ◁ i.width; h ◁ i.height;
    r ◁ makeRectangleWH(w/4, 0, w/4, h/2);
    s ◁ r ▷ translate(w/2, 0);
    i ▷ leftArrowWithCircle(r, red, blue);
    i ▷ rightArrow(s, red, blue);
   }

  i ◁ make$Image(draw, 1000, 2000, "$1.png", "a");
  i ▷ free;
 }

int main (void)
 {void (*tests[])(void) = {test0, test1, 0};
  run_tests("$", 1, tests);
  return 0;
}
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/g/cairoText/cairoText
