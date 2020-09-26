#line 1 "/home/phil/c/g/cairoText/cairoText.c"
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

typedef struct CairoTextImage                                                           // Create an image
 {const struct ProtoTypes_CairoTextImage *proto;                                        // Methods associated with an arena tree
  cairo_t           * cr;                                                       // Cairo
  const StringBuffer  out;                                                      // The output image file
 } CairoTextImage;

#include <cairoText_prototypes.h>

//D1 Constructors                                                               // Construct text in an image

CairoTextImage createCairoTextImage                                                             // Create an image by drawing on a surface
 (void (*draw)(CairoTextImage i),                                                       // Draw routine
  int x,                                                                        // Dimension of image in x
  int y,                                                                        // Dimension of image in y
  const char * const imageFile,                                                 // Name of output file
  const char * const expected)                                                  // Part of the expected digest of the image produced
 {char fontFile[128]; strcpy(fontFile, developmentMode() ?                      // Font file
  "/usr/share/fonts/truetype/noto/NotoSansMono-Regular.ttf":
  "fonts/NotoSansMono-Regular.ttf");

  cairo_surface_t * surface = cairo_image_surface_create                        // Cairo
   (CAIRO_FORMAT_ARGB32, x, y);
  cairo_t         *    cr   = cairo_create (surface);
  cairo_set_antialias (cr, CAIRO_ANTIALIAS_BEST);
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint         (cr);

  FT_Library freeTypeLibrary;                                                   // Free type
  const typeof(FT_Init_FreeType(&freeTypeLibrary)) e1 = FT_Init_FreeType(&freeTypeLibrary);
  if (e1) printStackBackTraceAndExit(1, "Error %d in free type\n", e1);

  FT_Face freeTypeFontFace;
  const typeof(FT_New_Face(freeTypeLibrary, fontFile, 0, &freeTypeFontFace)) e2 = FT_New_Face(freeTypeLibrary, fontFile, 0, &freeTypeFontFace);
  if (e2 == FT_Err_Unknown_File_Format) printStackBackTraceAndExit
   (1, "FontFace not supported: %s\n", fontFile);
  else if (e2) printStackBackTraceAndExit
   (1, "FontFace failed: %d %s\n", e2, fontFile);

  const typeof(cairo_ft_font_face_create_for_ft_face(freeTypeFontFace, 0)) cairoFontFace = cairo_ft_font_face_create_for_ft_face(freeTypeFontFace, 0);   // Load free type into cairo
  cairo_set_font_face (cr, cairoFontFace);

  const typeof(makeStringBufferFromString(imageFile)) iFile = makeStringBufferFromString(imageFile);                                // Image file name
  const typeof(newCairoTextImage(({struct CairoTextImage t = {out: iFile, cr: cr, proto: &ProtoTypes_CairoTextImage}; t;}))) i = newCairoTextImage(({struct CairoTextImage t = {out: iFile, cr: cr, proto: &ProtoTypes_CairoTextImage}; t;}));

  draw(i);                                                                      // Draw

  cairo_destroy(cr);                                                            // Save results
  cairo_font_face_destroy(cairoFontFace);
  cairo_surface_write_to_png(surface, imageFile);
  cairo_surface_destroy (surface);

  FT_Done_Face    (freeTypeFontFace);                                           // Free free type
  FT_Done_FreeType(freeTypeLibrary);

  i.proto->assertResult(i, expected);                                                   // Check results
  return i;
 }

//D1 Methods                                                                    // Methods that operate on an CairoText

static void free_CairoText                                                              // Free an image
 (CairoTextImage i)                                                                     // CairoTextImage
 {i.out.proto->free(i.out);
 }

static void assertResult                                                        // Check the image via a digest
 (CairoTextImage i,                                                                     // CairoTextImage
  const char * const digest)                                                    // Expected digest
 {      const typeof(makeStringBufferFromString("b2sum ")) c = makeStringBufferFromString("b2sum ");
        c.proto->addStringBuffer(c, i.out);
        c.proto->system(c);
  if (! c.proto->containsString(c, digest))
   {makeLocalCopyOfStringBuffer(d, l, c);
    say("\nHave: %s\nWant: %s\n", d, digest);
    assert(0);
   }
  c.proto->free(c);
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

  const typeof(createCairoTextImage(draw, 2000, 2000, "cairo.png", "ab32")) i = createCairoTextImage(draw, 2000, 2000, "cairo.png", "ab32");                      // Create image containing some text and check its digest
  i.proto->free(i);
 }

int main (void)
 {void (*tests[])(void) = {test0, 0};
  run_tests("CairoText", 1, tests);
  return 0;
}
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/g/cairoText/cairoText
