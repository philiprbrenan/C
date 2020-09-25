static void draw_text
 (cairo_t * cr,

  char    * text)

 {cairo_font_extents_t fontExtents;

  cairo_font_extents (cr, &fontExtents);



  cairo_move_to      (cr, 0, fontExtents.height);

  for(size_t j = 0; j < 100; ++j)

   {for(size_t i = 0; i < strlen(text); ++i)

     {
      char c[2]; c[0] = text[i]; c[1] = 0;

      cairo_show_text    (cr, c);


     }

    cairo_move_to        (cr, 0, (j + 1) * fontExtents.height);

   }

 }

#endif




#if __INCLUDE_LEVEL__ == 0



void createImage
 (void (*draw)(cairo_t *cr),

  int x,

  int y,

  const char * const imageFile,

  const char * const fontFile)

 {cairo_surface_t * surface = cairo_image_surface_create
   (CAIRO_FORMAT_ARGB32, x, y);

  cairo_t         *    cr   = cairo_create (surface);

  cairo_set_antialias (cr, CAIRO_ANTIALIAS_BEST);

  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);

  cairo_paint         (cr);



  FT_Library freeTypeLibrary;
  e1 ◁ FT_Init_FreeType(&freeTypeLibrary);

  if (e1) printStackBackTraceAndExit(1, "Error %d in free type\n", e1);



  FT_Face freeTypeFontFace;

  e2 ◁ FT_New_Face(freeTypeLibrary, fontFile, 0, &freeTypeFontFace);

  if (e2 == FT_Err_Unknown_File_Format) printStackBackTraceAndExit

   (1, "FontFace not supported: %s\n", fontFile);

  else if (e2) printStackBackTraceAndExit

   (1, "FontFace failed: %d\n", e2);



  cairoFontFace ◁ cairo_ft_font_face_create_for_ft_face(freeTypeFontFace, 0);

  cairo_set_font_face (cr, cairoFontFace);



  draw(cr);


  cairo_destroy(cr);
  cairo_font_face_destroy(cairoFontFace);

  cairo_surface_write_to_png(surface, imageFile);

  cairo_surface_destroy (surface);



  FT_Done_Face    (freeTypeFontFace);

  FT_Done_FreeType(freeTypeLibrary);

 }



void assertResult
 (const char * const imageFile,

  const char * const digest)

 {c ◁ makeStringBufferFromVaFormat("b2sum %s", imageFile);

  c ▷ system;

  assert(c ▷ containsString(digest));

  c ▷ free;

 }



void test0()

 {const char * const imageFile = "cairo.png";


   void draw(cairo_t *cr)

   {cairo_translate     (cr, 10, 10);

    cairo_set_font_size (cr, 20);

    cairo_set_source_rgb(cr, 0, 0, 0);

    draw_text(cr, "Hello World How is it whirling?");

   }



  createImage(draw, 2000, 2000, imageFile, "/usr/share/fonts/truetype/noto/NotoSansMono-Regular.ttf");

  assertResult(imageFile, "1f5601b609a7ebfdd");

 }



int main (void)

 {void (*tests[])(void) = {test0, 0};

  run_tests("CairoText", 1, tests);

  return 0;

}

#endif


;
