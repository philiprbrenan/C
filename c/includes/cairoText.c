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
  CairoTextFont fonts[0];                                                               // Fonts
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

enum CairoTextFonts {CairoTextFont_sansMonoBold    = 0,
             CairoTextFont_sansMono        = 1,
             CairoTextFont_sansBold        = 2,
             CairoTextFont_sansBoldItalic  = 3,
             CairoTextFont_sansItalic      = 4,
             CairoTextFont_sans            = 5,
             CairoTextFont_serifBold       = 6,
             CairoTextFont_serifBoldItalic = 7,
             CairoTextFont_serifItalic     = 8,
             CairoTextFont_serif           = 9,
             CairoTextFont_end             =10};


typedef struct CairoTextCompactList                                                     // A compact list of options that the user can cursor to, point to, scroll to choose an option.
 {const struct ProtoTypes_CairoTextCompactList *proto;                                  // Methods associated with an arena tree
  CairoTextImage * image;                                                               //I Image to draw in
  int pointerˣ, pointerʸ;                                                       //I Coordinates of the pointer in window pixels
  Colour frameColour;                                                           //I Colour of frame
  Colour possibilityColour;                                                     //I Colour of text used to show a possibility
  Colour pointedColour;                                                         //I Colour of back ground of current possibility
  Colour textEnteredSoFarColour;                                                //I Colour of the text entered so far
  ArenaList list;                                                               //I The possibilities to display
  ArenaListNode cursorEntry;                                                    //I The current possibility under the cursor
  char textEnteredSoFar[17];                                                    // Prefix text entered so far to narrow the possibilities
  size_t textEnteredSoFarLength;                                                // Length of text entered so far
  enum CairoTextFonts textEnteredSoFarFont;                                             //I Font for text entered so far
  enum CairoTextFonts possibilityFont;                                                  //I Font for remaining possibilities
  size_t possibilityFontSize;                                                   //I Font size for entries
  size_t startAtOffset;                                                         //I Start drawing at this entry
  size_t firstOffset;                                                           //O First visible entry
  size_t lastOffset;                                                            //O Last visible entry
  size_t cursorPrevOffset;                                                      //O Offset of entry preceding entry containing cursor
  size_t cursorNextOffset;                                                      //O Offset of entry following entry containing cursor
  size_t cursorUpOffset;                                                        //O Offset of entry above entry containing cursor
  size_t cursorDownOffset;                                                      //O Offset of entry below entry containing cursor
  size_t pointerOffset;                                                         //O Offset of entry containing cursor
  Rectangle drawTable;                                                          //I Drawing area
 } CairoTextCompactList;

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

  i.proto->font(&i, CairoTextFont_sansMono);                                                     // Default font
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

static CairoTextCompactList makeCairoTextCompactList                                            // Define a compact list
 (CairoTextImage  * image,                                                              // Image in which to draw the compact list
  ArenaList list,                                                               // Arena list of possibilities
  Rectangle drawTable)                                                          // Rectangle defining drawing area for the list
 {CairoTextCompactList l = newCairoTextCompactList(({struct CairoTextCompactList t = {proto: &ProtoTypes_CairoTextCompactList};   t;}));
  l.image                  = image;
memcpy(&  l.frameColour            , ({typeof(  l.frameColour            ) s =  makeColour(0,0,0,1); (void *)&s;}), sizeof(  l.frameColour            ));
memcpy(&  l.possibilityColour      , ({typeof(  l.possibilityColour      ) s =  makeColour(0,0,0,1); (void *)&s;}), sizeof(  l.possibilityColour      ));
memcpy(&  l.pointedColour          , ({typeof(  l.pointedColour          ) s =  makeColour(0,0,1,0.3); (void *)&s;}), sizeof(  l.pointedColour          ));
memcpy(&  l.textEnteredSoFarColour , ({typeof(  l.textEnteredSoFarColour ) s =  makeColour(0,1,0,1); (void *)&s;}), sizeof(  l.textEnteredSoFarColour ));
memcpy(&  l.textEnteredSoFarColour , ({typeof(  l.textEnteredSoFarColour ) s =  makeColour(0,1,0,1); (void *)&s;}), sizeof(  l.textEnteredSoFarColour ));
memcpy(&  l.list                   , ({typeof(  l.list                   ) s =  list; (void *)&s;}), sizeof(  l.list                   ));
memcpy(&  l.cursorEntry            , ({typeof(  l.cursorEntry            ) s =  list.proto->first(&list); (void *)&s;}), sizeof(  l.cursorEntry            ));
  l.textEnteredSoFarLength = 0;
memcpy(&  l.textEnteredSoFarFont   , ({typeof(  l.textEnteredSoFarFont   ) s =  CairoTextFont_sansItalic; (void *)&s;}), sizeof(  l.textEnteredSoFarFont   ));
memcpy(&  l.possibilityFont        , ({typeof(  l.possibilityFont        ) s =  CairoTextFont_serif; (void *)&s;}), sizeof(  l.possibilityFont        ));
  l.possibilityFontSize    = 100;
memcpy(&  l.drawTable              , ({typeof(  l.drawTable              ) s =  drawTable; (void *)&s;}), sizeof(  l.drawTable              ));
  return l;
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
 (CairoTextImage    * i,                                                                // CairoTextImage
  enum CairoTextFonts fontNumber)                                                       // Font to use
 {typeof(&i->fonts[fontNumber]) font = &i->fonts[fontNumber];
  if (!font->cairoFontFace)                                                     // Load the font if this font has not yet been loaded.
   {makeLocalCopyOfStringBuffer(ff, l, font->file);
    const typeof(FT_New_Face(i->freeTypeLibrary, ff, 0, &font->freeTypeFontFace)) e2 = FT_New_Face(i->freeTypeLibrary, ff, 0, &font->freeTypeFontFace);
    if (e2 == FT_Err_Unknown_File_Format) printStackBackTraceAndExit
     (1, "FontFace not supported: %s\n", ff);
    else if (e2) printStackBackTraceAndExit
     (1, "FontFace failed: %d %s\n", e2, ff);

    font->cairoFontFace =
      cairo_ft_font_face_create_for_ft_face(font->freeTypeFontFace, 0);
   }

if (i->fonts[fontNumber].cairoFontFace == 0)
 {printStackBackTrace("BBBB Bad font face\n");
 }

  cairo_set_font_face (i->cr, font->cairoFontFace);                              // Set the font as the currently active one

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

//D1 Widgets                                                                    // Draw widgets

static void draw__CairoTextCompactList                                                  // Draw a compact list
 (CairoTextCompactList * compactList,                                                   // Compact list
  int            drawing)                                                       // Drawing if true - else trial layout
 {const typeof(compactList) cl = compactList;
  typeof(cl->image) i = cl->image;

  if (drawing) i->proto->clip(i, cl->drawTable);                                         // Clip the drawing area to prevent text appearing outside it

  if (drawing)                                                                  // Show text entered so far
   {i->proto->colour(i, cl->textEnteredSoFarColour);
    i->proto->font(i, cl->textEnteredSoFarFont);
    const typeof(cl->textEnteredSoFarLength) l = cl->textEnteredSoFarLength;
    char t[l+1]; strncpy(t, cl->textEnteredSoFar, l); t[l] = 0;
    i->proto->textFit(i, cl->drawTable, 0, 0, 0, t);
   }

  i->proto->font(i, cl->possibilityFont);                                            // Font for remaining possibilities
  i->proto->fontSize(i, cl->possibilityFontSize);                                        // Size of text for possibilities
  i->proto->colour(i, cl->possibilityColour);                                          // Colour of text showing possibility

  double x = cl->drawTable.X, y = cl->drawTable.y - i->fontHeight;              // At the end of the previous line
  typeof(0ul) cursorEntryIndex = 0ul;                                                       // Whether we have drawn the cursor entry yet.
  const typeof(cl->list.proto->countChildren(&cl->list)) N = cl->list.proto->countChildren(&cl->list);                                                 // Maximum number of entries
  size_t    entryOffset    [N+1]; memset(entryOffset, 0, sizeof(entryOffset));  // Offset for each entry
  Rectangle entryRectangles[N+1];                                               // Rectangle for each entry up to next tab stop
  typeof(0ul) firstOffset = 0ul;

  ArenaListfeⁱ(word, cl->list)                                                  // Each word
   {if (wordⁱ >= cl->startAtOffset)                                             // Words in the scrolled to area
     {makeLocalCopyOfArenaListKey(K, L, word);
      if (L <= cl->textEnteredSoFarLength) continue;                            // Word shorter than prefix entered so far
      if (strncmp(K, cl->textEnteredSoFar, cl->textEnteredSoFarLength))continue;// Word does not match prefix entered so far
      const typeof(&K[cl->textEnteredSoFarLength]) k = &K[cl->textEnteredSoFarLength];                                       // Skip text entered so far
      const typeof(i->proto->textAdvance(i, k)) a = i->proto->textAdvance(i, k);                                                   // Width of text
      if (x + a > cl->drawTable.X) {x = cl->drawTable.x; y += i->fontHeight;}   // Move to next line if necessary
      const typeof(word.offset) offset = word.offset;                                                     // Offset of current entry

      const typeof(makeRectangleWH(x, y, a, i->fontHeight)) r = makeRectangleWH(x, y, a, i->fontHeight);                              // Rectangle in which to draw the text

      if (cl->drawTable.proto->contains(&cl->drawTable, r))                                          // Draw visible elements
       {if (offset == cl->cursorEntry.offset)                                   // Reached entry under cursor
         {i->proto->rectangle(i, r, cl->pointedColour);                                  // Background colour of entry containing the cursor
          cursorEntryIndex = wordⁱ;                                             // Show that the cursor entry has now been drawn
         }
        if (r.proto->containsPoint(&r, cl->pointerˣ, cl->pointerʸ))                      // Offset of item containing pointer
         {cl->pointerOffset = offset;
         }
        if (drawing) i->proto->text(i, x, y, k);                                         // Draw the remaining text of the entry
        if (!firstOffset) cl->firstOffset = firstOffset = offset;               // First offset visible in drawing area
        cl->lastOffset = offset;                                                // Last offset visible in drawing area
       }

      const typeof(i->fontHeight * ceil(a / i->fontHeight)) w = i->fontHeight * ceil(a / i->fontHeight);                              // Width of current entry including move to next tab stop
      if      (!cursorEntryIndex)              cl->cursorPrevOffset = offset;   // Entry just before cursor
      else if  (cursorEntryIndex + 1 == wordⁱ) cl->cursorNextOffset = offset;   // Entry just after cursor
      const typeof(makeRectangleWH(x, y, w, i->fontHeight)) R = makeRectangleWH(x, y, w, i->fontHeight);                              // Rectangle containing entry
memcpy(&      entryRectangles[wordⁱ] , ({typeof(      entryRectangles[wordⁱ] ) s =  R; (void *)&s;}), sizeof(      entryRectangles[wordⁱ] ));                                               // Rectangle containing this entry up to the next tab stop
      entryOffset            [wordⁱ] = offset;                                  // Offset for this entry
      x += w;                                                                   // Move to next entry position
     }
   }

  typeof(0.0) bestAreaUp = 0.0; typeof(0.0) bestAreaDown = 0.0;                                         // Locate the rectangles that match the best for up and down arrows
  typeof(entryRectangles[cursorEntryIndex]) cer = entryRectangles[cursorEntryIndex];
  const typeof(cer.proto->translate(&cer, 0, +i->fontHeight)) cerd = cer.proto->translate(&cer, 0, +i->fontHeight);
  const typeof(cer.proto->translate(&cer, 0, -i->fontHeight)) ceru = cer.proto->translate(&cer, 0, -i->fontHeight);

  for(size_t i = 1; i <= N; ++i)                                                // Each entry
   {const typeof(entryOffset[i]) o = entryOffset[i];                                                         // Offset of this entry
    if (o)
     {    const typeof(entryRectangles[i]) r = entryRectangles[i];                                               // Rectangle for entry
      const typeof(r.proto->intersectionArea(&r, cerd)) d = r.proto->intersectionArea(&r, cerd);                                           // Area of overlap with down rectangle
      const typeof(r.proto->intersectionArea(&r, ceru)) u = r.proto->intersectionArea(&r, ceru);                                           // Area of overlap with up rectangle
      if (d > bestAreaDown) {bestAreaDown = d; cl->cursorDownOffset = o;}       // Better down
      if (u > bestAreaUp)   {bestAreaUp   = u; cl->cursorUpOffset   = o;}       // Better up
     }
   }

  i->proto->rectangleLine(i, cl->drawTable, cl->frameColour);                            // Frame the drawn area
 } // draw

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

    i.proto->font(&i, CairoTextFont_serif);                                                  // Font
    i.proto->fontSize(&i, 100);                                                          // Size of text
    i.proto->colour(&i, black);                                                        // Colour of text

    const typeof(makeRectangleWH(500, 500, 500, 800)) drawTable = makeRectangleWH(500, 500, 500, 800);                            // Drawing area
    i.proto->clip(&i, drawTable);

    size_t x = drawTable.x, y = drawTable.y;
    ArenaListfe(col, table)                                                     // Each column
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
   {const typeof(1) drawing = 1;                                                                // Drawing if true else doing a trial layout
    const typeof(717ul) pointerˣ = 717ul; const typeof(717ul) pointerʸ = 717ul;                                         // Current pointer coordinates
               const typeof(makeColour(0,0,0,1)) frameColour = makeColour(0,0,0,1);                               // Colours
         const typeof(makeColour(0,0,0,1)) possibilityColour = makeColour(0,0,0,1);
             const typeof(makeColour(0,0,1,0.3)) pointedColour = makeColour(0,0,1,0.3);
    const typeof(makeColour(0,1,0,1)) textEnteredSoFarColour = makeColour(0,1,0,1);

    const typeof(makeArenaListFromWords("aaaa qqbbbb qqcc qqdd qqee qqff qqgggg qqhh qqiii qqjj qqkkk qql mmmm nn oo ppppp qq rrrr s tttttt uuuu v wwwwww xxx yy zz")) list = makeArenaListFromWords("aaaa qqbbbb qqcc qqdd qqee qqff qqgggg qqhh qqiii qqjj qqkkk qql mmmm nn oo ppppp qq rrrr s tttttt uuuu v wwwwww xxx yy zz");
    const typeof(list.proto->findFirst(&list, "qqee")) cursorEntry = list.proto->findFirst(&list, "qqee");                          // Cursor entry
    const typeof("qqx") textEnteredSoFar = "qqx";                                             // Assume the user has entered some text to narrow the possibilities displayed
    const typeof(2ul) textEnteredSoFarLength = 2ul;                                               // Length of text entered so far
    const typeof(CairoTextFont_sansItalic) textEnteredSoFarFont = CairoTextFont_sansItalic;                                  // Font for text entered so far
    const typeof(CairoTextFont_serif) possibilityFont = CairoTextFont_serif;                                       // Font for remaining possibilities
    const typeof(100ul) possibilityFontSize = 100ul;                                             // Font size for entries
    const typeof(2ul) startAtOffset = 2ul;                                               // Start drawing at this entry
    typeof(0ul) firstOffset = 0ul;                                               // First visible entry
    typeof(0ul) lastOffset = 0ul;                                               // Last visible entry
    typeof(0ul) cursorPrevOffset = 0ul;                                               // Offset of entry preceding entry containing cursor
    typeof(0ul) cursorNextOffset = 0ul;                                               // Offset of entry following entry containing cursor
    typeof(0ul) cursorUpOffset = 0ul;                                               // Offset of entry above entry containing cursor
    typeof(0ul) cursorDownOffset = 0ul;                                               // Offset of entry below entry containing cursor
    typeof(0ul) pointerOffset = 0ul;                                               // Offset of entry containing cursor
    const typeof(makeRectangleWH(500, 500, 500, 500)) drawTable = makeRectangleWH(500, 500, 500, 500);               // Drawing area

    if (drawing) i.proto->clip(&i, drawTable);                                           // Clip the drawing area to prevent text appearing outside it

    if (drawing)                                                                // Show text entered so far
     {i.proto->colour(&i, textEnteredSoFarColour);
      i.proto->font(&i, textEnteredSoFarFont);
      const typeof(textEnteredSoFarLength) l = textEnteredSoFarLength;
      char t[l+1]; strncpy(t, textEnteredSoFar, l); t[l] = 0;
      i.proto->textFit(&i, drawTable, 0, 0, 0, t);
     }

    i.proto->font(&i, possibilityFont);                                              // Font for remaining possibilities
    i.proto->fontSize(&i, possibilityFontSize);                                          // Size of text for possibilities
    i.proto->colour(&i, possibilityColour);                                            // Colour of text showing possibility

    double x = drawTable.X, y = drawTable.y - i.fontHeight;                     // At the end of the previous line
    typeof(0ul) cursorEntryIndex = 0ul;                                                     // Whether we have drawn the cursor entry yet.
    const typeof(list.proto->countChildren(&list)) N = list.proto->countChildren(&list);                                                   // Maximum number of entries
    size_t    entryOffset    [N+1]; memset(entryOffset, 0, sizeof(entryOffset));// Offset for each entry
    Rectangle entryRectangles[N+1];                                             // Rectangle for each entry up to next tab stop

    ArenaListfeⁱ(word, list)                                                    // Each word
     {if (wordⁱ >= startAtOffset)                                               // Words in the scrolled to area
       {makeLocalCopyOfArenaListKey(K, L, word);
        if (L <= textEnteredSoFarLength) continue;                              // Word shorter than prefix entered so far
        if (strncmp(K, textEnteredSoFar, textEnteredSoFarLength)) continue;     // Word does not match prefix entered so far
        const typeof(&K[textEnteredSoFarLength]) k = &K[textEnteredSoFarLength];                                         // Skip text entered so far
        const typeof(i.proto->textAdvance(&i, k)) a = i.proto->textAdvance(&i, k);                                                 // Width of text
        if (x + a > drawTable.X) {x = drawTable.x; y += i.fontHeight;}          // Move to next line if necessary
        const typeof(word.offset) offset = word.offset;                                                   // Offset of current entry

        const typeof(makeRectangleWH(x, y, a, i.fontHeight)) r = makeRectangleWH(x, y, a, i.fontHeight);                             // Rectangle in which to draw the text

        if (drawTable.proto->contains(&drawTable, r))                                            // Draw visible elements
         {if (offset == cursorEntry.offset)                                     // Reached entry under cursor
           {i.proto->rectangle(&i, r, pointedColour);                                    // Background colour of entry containing the cursor
            cursorEntryIndex = wordⁱ;                                           // Show that the cursor entry has now been drawn
           }
          if (r.proto->containsPoint(&r, pointerˣ, pointerʸ)) pointerOffset = offset;    // Offset of item containing pointer
          if (drawing) i.proto->text(&i, x, y, k);                                       // Draw the remaining text of the entry
          if (!firstOffset) firstOffset = offset;                               // First offset visible in drawing area
          lastOffset = offset;                                                  // Last offset visible in drawing area
         }

        const typeof(i.fontHeight * ceil(a / i.fontHeight)) w = i.fontHeight * ceil(a / i.fontHeight);                              // Width of current entry including move to next tab stop
        if      (!cursorEntryIndex)              cursorPrevOffset = offset;     // Entry just before cursor
        else if  (cursorEntryIndex + 1 == wordⁱ) cursorNextOffset = offset;     // Entry just after cursor
        const typeof(makeRectangleWH(x, y, w, i.fontHeight)) R = makeRectangleWH(x, y, w, i.fontHeight);                             // Rectangle containing entry
        memcpy(&entryRectangles[wordⁱ], &R, sizeof(R));                         // Rectangle containing this entry up to the next tab stop
        entryOffset            [wordⁱ] = offset;                                // Offset for this entry
        x += w;                                                                 // Move to next entry position
       }
     }

    typeof(0.0) bestAreaUp = 0.0; typeof(0.0) bestAreaDown = 0.0;                                       // Locate the rectangles that match the best for up and down arrows
    typeof(entryRectangles[cursorEntryIndex]) cer = entryRectangles[cursorEntryIndex];
    const typeof(cer.proto->translate(&cer, 0, +i.fontHeight)) cerd = cer.proto->translate(&cer, 0, +i.fontHeight);
    const typeof(cer.proto->translate(&cer, 0, -i.fontHeight)) ceru = cer.proto->translate(&cer, 0, -i.fontHeight);

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
     {const typeof(list.proto->offset(&list, firstOffset)) f = list.proto->offset(&list, firstOffset);        assert( f.proto->equalsString(&f, "qqbbbb"));
      const typeof(list.proto->offset(&list, lastOffset)) l = list.proto->offset(&list, lastOffset);         assert( l.proto->equalsString(&l, "qqhh"));
      const typeof(list.proto->offset(&list, cursorEntry.offset)) c = list.proto->offset(&list, cursorEntry.offset); assert( c.proto->equalsString(&c, "qqee"));
      const typeof(list.proto->offset(&list, cursorPrevOffset)) p = list.proto->offset(&list, cursorPrevOffset);   assert( p.proto->equalsString(&p, "qqdd"));
      const typeof(list.proto->offset(&list, cursorNextOffset)) n = list.proto->offset(&list, cursorNextOffset);   assert( n.proto->equalsString(&n, "qqff"));
      const typeof(list.proto->offset(&list, cursorDownOffset)) d = list.proto->offset(&list, cursorDownOffset);   assert( d.proto->equalsString(&d, "qqgggg"));
      const typeof(list.proto->offset(&list, cursorUpOffset)) u = list.proto->offset(&list, cursorUpOffset);     assert( u.proto->equalsString(&u, "qqbbbb"));
      const typeof(list.proto->offset(&list, pointerOffset)) P = list.proto->offset(&list, pointerOffset);      assert( P.proto->equalsString(&P, "qqee"));
     }
   }

  typeof(makeCairoTextImage(draw, 2000, 2000, "CairoText4.png", "9449")) i = makeCairoTextImage(draw, 2000, 2000, "CairoText4.png", "9449");
  i.proto->free(&i);
 }

void test5()                                                                    // Text table using tab stops
 {void draw(CairoTextImage i)
   {const typeof(makeArenaListFromWords("aaaa qqbbbb qqcc qqdd qqee qqff qqgggg qqhh qqiii qqjj qqkkk qql mmmm nn oo ppppp qq rrrr s tttttt uuuu v wwwwww xxx yy zz")) list = makeArenaListFromWords("aaaa qqbbbb qqcc qqdd qqee qqff qqgggg qqhh qqiii qqjj qqkkk qql mmmm nn oo ppppp qq rrrr s tttttt uuuu v wwwwww xxx yy zz");
    const typeof(makeRectangleWH(500, 500, 500, 500)) drawTable = makeRectangleWH(500, 500, 500, 500);                            // Drawing area
    typeof(makeCairoTextCompactList(&i, list, drawTable)) cl = makeCairoTextCompactList(&i, list, drawTable);                                 // Create compact list
memcpy(&    cl.cursorEntry , ({typeof(    cl.cursorEntry ) s =  list.proto->findFirst(&list, "qqee"); (void *)&s;}), sizeof(    cl.cursorEntry ));                                  // Cursor entry
    cl.pointerˣ = 717; cl.pointerʸ = 717;                                       // Current pointer coordinates
    strcpy(cl.textEnteredSoFar, "qqx");                                         // Assume the user has entered some text to narrow the possibilities displayed
    cl.textEnteredSoFarLength = 2;                                              // Assume the user has entered some text to narrow the possibilities displayed
    cl.possibilityFontSize = 100;                                               // Font size for entries

    cl.proto->draw(&cl, 1);

    if (1)                                                                      // Check results
     {const typeof(list.proto->offset(&list, cl.firstOffset)) f = list.proto->offset(&list, cl.firstOffset);        assert( f.proto->equalsString(&f, "qqbbbb"));
      const typeof(list.proto->offset(&list, cl.lastOffset)) l = list.proto->offset(&list, cl.lastOffset);         assert( l.proto->equalsString(&l, "qqhh"));
      const typeof(list.proto->offset(&list, cl.cursorEntry.offset)) c = list.proto->offset(&list, cl.cursorEntry.offset); assert( c.proto->equalsString(&c, "qqee"));
      const typeof(list.proto->offset(&list, cl.cursorPrevOffset)) p = list.proto->offset(&list, cl.cursorPrevOffset);   assert( p.proto->equalsString(&p, "qqdd"));
      const typeof(list.proto->offset(&list, cl.cursorNextOffset)) n = list.proto->offset(&list, cl.cursorNextOffset);   assert( n.proto->equalsString(&n, "qqff"));
      const typeof(list.proto->offset(&list, cl.cursorDownOffset)) d = list.proto->offset(&list, cl.cursorDownOffset);   assert( d.proto->equalsString(&d, "qqgggg"));
      const typeof(list.proto->offset(&list, cl.cursorUpOffset)) u = list.proto->offset(&list, cl.cursorUpOffset);     assert( u.proto->equalsString(&u, "qqbbbb"));
      const typeof(list.proto->offset(&list, cl.pointerOffset)) P = list.proto->offset(&list, cl.pointerOffset);      assert( P.proto->equalsString(&P, "qqee"));
     }
   }

  typeof(makeCairoTextImage(draw, 2000, 2000, "CairoText5.png", "9449")) i = makeCairoTextImage(draw, 2000, 2000, "CairoText5.png", "9449");
  i.proto->free(&i);
 }

int main (void)
 {void (*tests[])(void) = {test0, test1, test2, test3, test4,
                           test5, 0};
  run_tests("CairoText", 1, tests);
  return 0;
}
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/g/cairoText/cairoText
