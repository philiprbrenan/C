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
#ifndef $_included
#define $_included

typedef struct $Image                                                           // Create an image
 {const struct ProtoTypes_$Image *proto;                                        // Methods associated with an arena tree
  cairo_t           * cr;                                                       // Cairo
  const StringBuffer  out;                                                      // The output image file
 } $Image;

#include <$$_prototypes.h>

//D1 Constructors                                                               // Construct text in an image

$Image create$Image                                                             // Create an image by drawing on a surface
 (void (*draw)($Image i),                                                       // Draw routine
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
  e1 ◁ FT_Init_FreeType(&freeTypeLibrary);
  if (e1) printStackBackTraceAndExit(1, "Error %d in free type\n", e1);

  FT_Face freeTypeFontFace;
  e2 ◁ FT_New_Face(freeTypeLibrary, fontFile, 0, &freeTypeFontFace);
  if (e2 == FT_Err_Unknown_File_Format) printStackBackTraceAndExit
   (1, "FontFace not supported: %s\n", fontFile);
  else if (e2) printStackBackTraceAndExit
   (1, "FontFace failed: %d %s\n", e2, fontFile);

  cairoFontFace ◁ cairo_ft_font_face_create_for_ft_face(freeTypeFontFace, 0);   // Load free type into cairo
  cairo_set_font_face (cr, cairoFontFace);

  iFile ◁ makeStringBufferFromString(imageFile);                                // Image file name
  i ◁ new $Image(out: iFile, cr: cr);

  draw(i);                                                                      // Draw

  cairo_destroy(cr);                                                            // Save results
  cairo_font_face_destroy(cairoFontFace);
  cairo_surface_write_to_png(surface, imageFile);
  cairo_surface_destroy (surface);

  FT_Done_Face    (freeTypeFontFace);                                           // Free free type
  FT_Done_FreeType(freeTypeLibrary);

  i ▷ assertResult(expected);                                                   // Check results
  return i;
 }

//D1 Methods                                                                    // Methods that operate on an $

static void free_$                                                              // Free an image
 ($Image i)                                                                     // $Image
 {i.out ▷ free;
 }

static void assertResult                                                        // Check the image via a digest
 ($Image i,                                                                     // $Image
  const char * const digest)                                                    // Expected digest
 {      c ◁ makeStringBufferFromString("b2sum ");
        c ▷ addStringBuffer(i.out);
        c ▷ system;
  if (! c ▷ containsString(digest))
   {makeLocalCopyOfStringBuffer(d, l, c);
    say("\nHave: %s\nWant: %s\n", d, digest);
    assert(0);
   }
  c ▷ free;
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

  i ◁ create$Image(draw, 2000, 2000, "cairo.png", "ab32");                      // Create image containing some text and check its digest
  i ▷ free;
 }

int main (void)
 {void (*tests[])(void) = {test0, 0};
  run_tests("$", 1, tests);
  return 0;
}
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/g/cairoText/cairoText
