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

  i.proto->setFont(i, i.sansMono);                                                      // Default font
  i.proto->clearWhite(i);                                                               // Clear display

  draw(i);                                                                      // Draw

  cairo_destroy(cr);                                                            // Save results
  cairo_surface_write_to_png(surface, imageFile);
  cairo_surface_destroy (surface);

  i.proto->assertCairoTextResult(i, expected);                                                  // Check results
  return i;
 } // makeCairoTextImage

static CairoTextFont makeCairoTextFont                                                          // Record a font file description
 (char * fontFile)                                                              // Font file base name
 {StringBuffer f = makeStringBuffer();
  f.proto->add(f, developmentMode() ? "/usr/share/fonts/truetype/noto/" :               // Location of font
                              "/home/runner/work/C/C/fonts/");
  f.proto->add(f, fontFile);
  f.proto->join(f);
  return newCairoTextFont(({struct CairoTextFont t = {file: f, proto: &ProtoTypes_CairoTextFont}; t;}));
 }

//D1 Free                                                                       // Free cairo and free type resources associated with the image

static void free_CairoTextImage                                                         // Free an image
 (CairoTextImage i)                                                                     // CairoTextImage
 {i.out.proto->free(i.out);

  CairoTextFont *fonts[] =
   {&i.sansBoldItalic, &i.sansBold, &i.sansItalic,      &i.sans,
    &i.sansMonoBold,   &i.sansMono, &i.serifBoldItalic, &i.serifBold,
    &i.serifItalic,    &i.serif};

  for(CairoTextFont **f = fonts; *f; ++f)                                               // Free any fonts that were loaded
   {CairoTextFont F = **f;
    F.proto->free(F);
   }

  FT_Done_FreeType(i.freeTypeLibrary);                                          // Finished with FreeType library
 }

static void free_CairoTextFont                                                          // Free a font if it has been loaded
 (CairoTextFont font)                                                                   // CairoTextFont
 {if (font.cairoFontFace)
   {cairo_font_face_destroy(font.cairoFontFace);
    FT_Done_Face           (font.freeTypeFontFace);
    font.file.proto->free(font.file);
   }
 }

//D1 Methods                                                                    // Methods that operate on an CairoText

static void setFont                                                             // Start using a font
 (CairoTextImage i,                                                                     // CairoTextImage
  CairoTextFont font)                                                                   // Font to use
 {if (!font.cairoFontFace)                                                      // Load all the available fonts into cairo
   {makeLocalCopyOfStringBuffer(ff, l, font.file);
    const typeof(FT_New_Face(i.freeTypeLibrary, ff, 0, &font.freeTypeFontFace)) e2 = FT_New_Face(i.freeTypeLibrary, ff, 0, &font.freeTypeFontFace);
    if (e2 == FT_Err_Unknown_File_Format) printStackBackTraceAndExit
     (1, "FontFace not supported: %s\n", ff);
    else if (e2) printStackBackTraceAndExit
     (1, "FontFace failed: %d %s\n", e2, ff);

    font.cairoFontFace = cairo_ft_font_face_create_for_ft_face(font.freeTypeFontFace, 0);
   }
  cairo_set_font_face (i.cr, font.cairoFontFace);
 }

static void assertCairoTextImageFile                                                    //P Check that the digest of an image file contains the specified string
 (char *             imageFile,                                                 // Image file name
  const char * const digest)                                                    // Expected digest
 {      const typeof(makeStringBufferFromString("b2sum ")) c = makeStringBufferFromString("b2sum ");
        c.proto->add(c, imageFile);
        c.proto->system(c);
  if (! c.proto->containsString(c, digest))
   {makeLocalCopyOfStringBuffer(d, l, c);
    say("\nImage file: %s\n",   imageFile);
    say("Have: %s\nWant: %s\n", d, digest);
    assert(0);
   }
  c.proto->free(c);
 }

static void assertCairoTextResult                                                       // Check the image via a digest
 (CairoTextImage i,                                                                     // CairoTextImage
  const char * const digest)                                                    // Expected digest
 {makeLocalCopyOfStringBuffer(s, l, i.out);
  assertCairoTextImageFile(s, digest);
 }

static void save_CairoText_string                                                       // Save a copy of the drawing surface to the specified file
 (CairoTextImage             i,                                                         // CairoTextImage
  char *             imageFile,                                                 // Image file name
  const char * const digest)                                                    // Expected digest
 {cairo_surface_write_to_png(i.surface, imageFile);
  assertCairoTextImageFile(imageFile, digest);
 }

static void clearWhite_CairoText                                                        // Clear the drawing surface to white
 (CairoTextImage             i)                                                         // CairoTextImage
 {const typeof(i.cr) cr = i.cr;
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint         (cr);
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TcreateImage
 {void draw(CairoTextImage i)                                                           // Draw some text
   {typeof(i.cr) cr = i.cr;
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

  const typeof(makeCairoTextImage(draw, 2000, 2000, "CairoText.png", "ab32")) i = makeCairoTextImage(draw, 2000, 2000, "CairoText.png", "ab32");                        // Create image containing some text and check its digest
  i.proto->free(i);
 }

int main (void)
 {void (*tests[])(void) = {test0, 0};
  run_tests("CairoText", 1, tests);
  return 0;
}
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/g/cairoText/cairoText
