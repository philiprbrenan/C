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
  $Font fonts[0];                                                               // Fonts
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

enum $Fonts {$Font_sansMonoBold    = 0,
             $Font_sansMono        = 1,
             $Font_sansBold        = 2,
             $Font_sansBoldItalic  = 3,
             $Font_sansItalic      = 4,
             $Font_sans            = 5,
             $Font_serifBold       = 6,
             $Font_serifBoldItalic = 7,
             $Font_serifItalic     = 8,
             $Font_serif           = 9,
             $Font_end             =10};


typedef struct $CompactList                                                     // A compact list of options that the user can cursor to, point to, scroll to choose an option.
 {const struct ProtoTypes_$CompactList *proto;                                  // Methods associated with an arena tree
  $Image * image;                                                               //I Image to draw in
  int pointerˣ, pointerʸ;                                                       //I Coordinates of the pointer in window pixels
  Colour frameColour;                                                           //I Colour of frame
  Colour possibilityColour;                                                     //I Colour of text used to show a possibility
  Colour pointedColour;                                                         //I Colour of back ground of current possibility
  Colour textEnteredSoFarColour;                                                //I Colour of the text entered so far
  ArenaList list;                                                               //I The possibilities to display
  ArenaListNode cursorEntry;                                                    //I The current possibility under the cursor
  char textEnteredSoFar[17];                                                    // Prefix text entered so far to narrow the possibilities
  size_t textEnteredSoFarLength;                                                // Length of text entered so far
  enum $Fonts textEnteredSoFarFont;                                             //I Font for text entered so far
  enum $Fonts possibilityFont;                                                  //I Font for remaining possibilities
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
 } $CompactList;

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

  i ▷ font($Font_sansMono);                                                     // Default font
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

static $CompactList make$CompactList                                            // Define a compact list
 ($Image  * image,                                                              // Image in which to draw the compact list
  ArenaList list,                                                               // Arena list of possibilities
  Rectangle drawTable)                                                          // Rectangle defining drawing area for the list
 {CairoTextCompactList l = newCairoTextCompactList();
  l.image                  = image;
  l.frameColour            ≞ makeColour(0,0,0,1);
  l.possibilityColour      ≞ makeColour(0,0,0,1);
  l.pointedColour          ≞ makeColour(0,0,1,0.3);
  l.textEnteredSoFarColour ≞ makeColour(0,1,0,1);
  l.textEnteredSoFarColour ≞ makeColour(0,1,0,1);
  l.list                   ≞ list;
  l.cursorEntry            ≞ list ▷ first;
  l.textEnteredSoFarLength = 0;
  l.textEnteredSoFarFont   ≞ $Font_sansItalic;
  l.possibilityFont        ≞ $Font_serif;
  l.possibilityFontSize    = 100;
  l.drawTable              ≞ drawTable;
  return l;
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
 ($Image    * i,                                                                // $Image
  enum $Fonts fontNumber)                                                       // Font to use
 {font ◀ &i->fonts[fontNumber];
  if (!font->cairoFontFace)                                                     // Load the font if this font has not yet been loaded.
   {makeLocalCopyOfStringBuffer(ff, l, font->file);
    e2 ◁ FT_New_Face(i->freeTypeLibrary, ff, 0, &font->freeTypeFontFace);
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

//D1 Widgets                                                                    // Draw widgets

static void draw__$CompactList                                                  // Draw a compact list
 ($CompactList * compactList,                                                   // Compact list
  int            drawing)                                                       // Drawing if true - else trial layout
 {cl ◁ compactList;
  i  ◀ cl->image;

  if (drawing) i ▶ clip(cl->drawTable);                                         // Clip the drawing area to prevent text appearing outside it

  if (drawing)                                                                  // Show text entered so far
   {i ▶ colour(cl->textEnteredSoFarColour);
    i ▶ font  (cl->textEnteredSoFarFont);
    l ◁ cl->textEnteredSoFarLength;
    char t[l+1]; strncpy(t, cl->textEnteredSoFar, l); t[l] = 0;
    i ▶ textFit(cl->drawTable, 0, 0, 0, t);
   }

  i ▶ font    (cl->possibilityFont);                                            // Font for remaining possibilities
  i ▶ fontSize(cl->possibilityFontSize);                                        // Size of text for possibilities
  i ▶ colour  (cl->possibilityColour);                                          // Colour of text showing possibility

  double x = cl->drawTable.X, y = cl->drawTable.y - i->fontHeight;              // At the end of the previous line
  cursorEntryIndex ◀ 0ul;                                                       // Whether we have drawn the cursor entry yet.
  N ◁ cl->list ▷ countChildren;                                                 // Maximum number of entries
  size_t    entryOffset    [N+1]; memset(entryOffset, 0, sizeof(entryOffset));  // Offset for each entry
  Rectangle entryRectangles[N+1];                                               // Rectangle for each entry up to next tab stop
  firstOffset ◀ 0ul;

  ArenaListfeⁱ(word, cl->list)                                                  // Each word
   {if (wordⁱ >= cl->startAtOffset)                                             // Words in the scrolled to area
     {makeLocalCopyOfArenaListKey(K, L, word);
      if (L <= cl->textEnteredSoFarLength) continue;                            // Word shorter than prefix entered so far
      if (strncmp(K, cl->textEnteredSoFar, cl->textEnteredSoFarLength))continue;// Word does not match prefix entered so far
      k ◁ &K[cl->textEnteredSoFarLength];                                       // Skip text entered so far
      a ◁ i ▶ textAdvance(k);                                                   // Width of text
      if (x + a > cl->drawTable.X) {x = cl->drawTable.x; y += i->fontHeight;}   // Move to next line if necessary
      offset ◁ word.offset;                                                     // Offset of current entry

      r ◁ makeRectangleWH(x, y, a, i->fontHeight);                              // Rectangle in which to draw the text

      if (cl->drawTable ▷ contains(r))                                          // Draw visible elements
       {if (offset == cl->cursorEntry.offset)                                   // Reached entry under cursor
         {i ▶ rectangle(r, cl->pointedColour);                                  // Background colour of entry containing the cursor
          cursorEntryIndex = wordⁱ;                                             // Show that the cursor entry has now been drawn
         }
        if (r ▷ containsPoint(cl->pointerˣ, cl->pointerʸ))                      // Offset of item containing pointer
         {cl->pointerOffset = offset;
         }
        if (drawing) i ▶ text(x, y, k);                                         // Draw the remaining text of the entry
        if (!firstOffset) cl->firstOffset = firstOffset = offset;               // First offset visible in drawing area
        cl->lastOffset = offset;                                                // Last offset visible in drawing area
       }

      w ◁ i->fontHeight * ceil(a / i->fontHeight);                              // Width of current entry including move to next tab stop
      if      (!cursorEntryIndex)              cl->cursorPrevOffset = offset;   // Entry just before cursor
      else if  (cursorEntryIndex + 1 == wordⁱ) cl->cursorNextOffset = offset;   // Entry just after cursor
      R ◁ makeRectangleWH(x, y, w, i->fontHeight);                              // Rectangle containing entry
      entryRectangles[wordⁱ] ≞ R;                                               // Rectangle containing this entry up to the next tab stop
      entryOffset            [wordⁱ] = offset;                                  // Offset for this entry
      x += w;                                                                   // Move to next entry position
     }
   }

  bestAreaUp ◀ 0.0; bestAreaDown ◀ 0.0;                                         // Locate the rectangles that match the best for up and down arrows
  cer  ◀ entryRectangles[cursorEntryIndex];
  cerd ◁ cer ▷ translate(0, +i->fontHeight);
  ceru ◁ cer ▷ translate(0, -i->fontHeight);

  for(size_t i = 1; i <= N; ++i)                                                // Each entry
   {o ◁ entryOffset[i];                                                         // Offset of this entry
    if (o)
     {    r ◁ entryRectangles[i];                                               // Rectangle for entry
      d ◁ r ▷ intersectionArea(cerd);                                           // Area of overlap with down rectangle
      u ◁ r ▷ intersectionArea(ceru);                                           // Area of overlap with up rectangle
      if (d > bestAreaDown) {bestAreaDown = d; cl->cursorDownOffset = o;}       // Better down
      if (u > bestAreaUp)   {bestAreaUp   = u; cl->cursorUpOffset   = o;}       // Better up
     }
   }

  i ▶ rectangleLine(cl->drawTable, cl->frameColour);                            // Frame the drawn area
 } // draw

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

    i ▷ font    ($Font_serif);                                                  // Font
    i ▷ fontSize(100);                                                          // Size of text
    i ▷ colour  (black);                                                        // Colour of text

    drawTable ◁ makeRectangleWH(500, 500, 500, 800);                            // Drawing area
    i ▷ clip(drawTable);

    size_t x = drawTable.x, y = drawTable.y;
    ArenaListfe(col, table)                                                     // Each column
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
   {drawing ◁ 1;                                                                // Drawing if true else doing a trial layout
    pointerˣ ◁ 717ul; pointerʸ ◁ 717ul;                                         // Current pointer coordinates
               frameColour ◁ makeColour(0,0,0,1);                               // Colours
         possibilityColour ◁ makeColour(0,0,0,1);
             pointedColour ◁ makeColour(0,0,1,0.3);
    textEnteredSoFarColour ◁ makeColour(0,1,0,1);

    list ◁ makeArenaListFromWords("aaaa qqbbbb qqcc qqdd qqee qqff qqgggg qqhh qqiii qqjj qqkkk qql mmmm nn oo ppppp qq rrrr s tttttt uuuu v wwwwww xxx yy zz");
    cursorEntry            ◁ list ▷ findFirst("qqee");                          // Cursor entry
    textEnteredSoFar       ◁ "qqx";                                             // Assume the user has entered some text to narrow the possibilities displayed
    textEnteredSoFarLength ◁ 2ul;                                               // Length of text entered so far
    textEnteredSoFarFont   ◁ $Font_sansItalic;                                  // Font for text entered so far
    possibilityFont        ◁ $Font_serif;                                       // Font for remaining possibilities
    possibilityFontSize    ◁ 100ul;                                             // Font size for entries
    startAtOffset          ◁ 2ul;                                               // Start drawing at this entry
    firstOffset            ◀ 0ul;                                               // First visible entry
    lastOffset             ◀ 0ul;                                               // Last visible entry
    cursorPrevOffset       ◀ 0ul;                                               // Offset of entry preceding entry containing cursor
    cursorNextOffset       ◀ 0ul;                                               // Offset of entry following entry containing cursor
    cursorUpOffset         ◀ 0ul;                                               // Offset of entry above entry containing cursor
    cursorDownOffset       ◀ 0ul;                                               // Offset of entry below entry containing cursor
    pointerOffset          ◀ 0ul;                                               // Offset of entry containing cursor
    drawTable              ◁ makeRectangleWH(500, 500, 500, 500);               // Drawing area

    if (drawing) i ▷ clip(drawTable);                                           // Clip the drawing area to prevent text appearing outside it

    if (drawing)                                                                // Show text entered so far
     {i ▷ colour(textEnteredSoFarColour);
      i ▷ font  (textEnteredSoFarFont);
      l ◁ textEnteredSoFarLength;
      char t[l+1]; strncpy(t, textEnteredSoFar, l); t[l] = 0;
      i ▷ textFit(drawTable, 0, 0, 0, t);
     }

    i ▷ font    (possibilityFont);                                              // Font for remaining possibilities
    i ▷ fontSize(possibilityFontSize);                                          // Size of text for possibilities
    i ▷ colour  (possibilityColour);                                            // Colour of text showing possibility

    double x = drawTable.X, y = drawTable.y - i.fontHeight;                     // At the end of the previous line
    cursorEntryIndex ◀ 0ul;                                                     // Whether we have drawn the cursor entry yet.
    N ◁ list ▷ countChildren;                                                   // Maximum number of entries
    size_t    entryOffset    [N+1]; memset(entryOffset, 0, sizeof(entryOffset));// Offset for each entry
    Rectangle entryRectangles[N+1];                                             // Rectangle for each entry up to next tab stop

    ArenaListfeⁱ(word, list)                                                    // Each word
     {if (wordⁱ >= startAtOffset)                                               // Words in the scrolled to area
       {makeLocalCopyOfArenaListKey(K, L, word);
        if (L <= textEnteredSoFarLength) continue;                              // Word shorter than prefix entered so far
        if (strncmp(K, textEnteredSoFar, textEnteredSoFarLength)) continue;     // Word does not match prefix entered so far
        k ◁ &K[textEnteredSoFarLength];                                         // Skip text entered so far
        a ◁ i ▷ textAdvance(k);                                                 // Width of text
        if (x + a > drawTable.X) {x = drawTable.x; y += i.fontHeight;}          // Move to next line if necessary
        offset ◁ word.offset;                                                   // Offset of current entry

        r ◁ makeRectangleWH(x, y, a, i.fontHeight);                             // Rectangle in which to draw the text

        if (drawTable ▷ contains(r))                                            // Draw visible elements
         {if (offset == cursorEntry.offset)                                     // Reached entry under cursor
           {i ▷ rectangle(r, pointedColour);                                    // Background colour of entry containing the cursor
            cursorEntryIndex = wordⁱ;                                           // Show that the cursor entry has now been drawn
           }
          if (r ▷ containsPoint(pointerˣ, pointerʸ)) pointerOffset = offset;    // Offset of item containing pointer
          if (drawing) i ▷ text(x, y, k);                                       // Draw the remaining text of the entry
          if (!firstOffset) firstOffset = offset;                               // First offset visible in drawing area
          lastOffset = offset;                                                  // Last offset visible in drawing area
         }

        w ◁ i.fontHeight * ceil(a / i.fontHeight);                              // Width of current entry including move to next tab stop
        if      (!cursorEntryIndex)              cursorPrevOffset = offset;     // Entry just before cursor
        else if  (cursorEntryIndex + 1 == wordⁱ) cursorNextOffset = offset;     // Entry just after cursor
        R ◁ makeRectangleWH(x, y, w, i.fontHeight);                             // Rectangle containing entry
        memcpy(&entryRectangles[wordⁱ], &R, sizeof(R));                         // Rectangle containing this entry up to the next tab stop
        entryOffset            [wordⁱ] = offset;                                // Offset for this entry
        x += w;                                                                 // Move to next entry position
       }
     }

    bestAreaUp ◀ 0.0; bestAreaDown ◀ 0.0;                                       // Locate the rectangles that match the best for up and down arrows
    cer  ◀ entryRectangles[cursorEntryIndex];
    cerd ◁ cer ▷ translate(0, +i.fontHeight);
    ceru ◁ cer ▷ translate(0, -i.fontHeight);

    for(size_t i = 1; i <= N; ++i)                                              // Each entry
     {o ◁ entryOffset[i];                                                       // Offset of this entry
      if (o)
       {    r ◁ entryRectangles[i];                                             // Rectangle for entry
        d ◁ r ▷ intersectionArea(cerd);                                         // Area of overlap with down rectangle
        u ◁ r ▷ intersectionArea(ceru);                                         // Area of overlap with up rectangle
        if (d > bestAreaDown) {bestAreaDown = d; cursorDownOffset = o;}         // Better down
        if (u > bestAreaUp)   {bestAreaUp   = u; cursorUpOffset   = o;}         // Better up
       }
     }

    i ▷ rectangleLine(drawTable, frameColour);                                  // Frame the drawn area

    if (1)                                                                      // Check results
     {f ◁ list ▷ offset(firstOffset);        ✓ f ▷ equalsString("qqbbbb");
      l ◁ list ▷ offset(lastOffset);         ✓ l ▷ equalsString("qqhh");
      c ◁ list ▷ offset(cursorEntry.offset); ✓ c ▷ equalsString("qqee");
      p ◁ list ▷ offset(cursorPrevOffset);   ✓ p ▷ equalsString("qqdd");
      n ◁ list ▷ offset(cursorNextOffset);   ✓ n ▷ equalsString("qqff");
      d ◁ list ▷ offset(cursorDownOffset);   ✓ d ▷ equalsString("qqgggg");
      u ◁ list ▷ offset(cursorUpOffset);     ✓ u ▷ equalsString("qqbbbb");
      P ◁ list ▷ offset(pointerOffset);      ✓ P ▷ equalsString("qqee");
     }
   }

  i ◀ make$Image(draw, 2000, 2000, "$4.png", "9449");
  i ▷ free;
 }

void test5()                                                                    // Text table using tab stops
 {void draw($Image i)
   {list ◁ makeArenaListFromWords("aaaa qqbbbb qqcc qqdd qqee qqff qqgggg qqhh qqiii qqjj qqkkk qql mmmm nn oo ppppp qq rrrr s tttttt uuuu v wwwwww xxx yy zz");
    drawTable ◁ makeRectangleWH(500, 500, 500, 500);                            // Drawing area
    cl ◀ make$CompactList(&i, list, drawTable);                                 // Create compact list
    cl.cursorEntry ≞ list ▷ findFirst("qqee");                                  // Cursor entry
    cl.pointerˣ = 717; cl.pointerʸ = 717;                                       // Current pointer coordinates
    strcpy(cl.textEnteredSoFar, "qqx");                                         // Assume the user has entered some text to narrow the possibilities displayed
    cl.textEnteredSoFarLength = 2;                                              // Assume the user has entered some text to narrow the possibilities displayed
    cl.possibilityFontSize = 100;                                               // Font size for entries

    cl ▷ draw(1);

    if (1)                                                                      // Check results
     {f ◁ list ▷ offset(cl.firstOffset);        ✓ f ▷ equalsString("qqbbbb");
      l ◁ list ▷ offset(cl.lastOffset);         ✓ l ▷ equalsString("qqhh");
      c ◁ list ▷ offset(cl.cursorEntry.offset); ✓ c ▷ equalsString("qqee");
      p ◁ list ▷ offset(cl.cursorPrevOffset);   ✓ p ▷ equalsString("qqdd");
      n ◁ list ▷ offset(cl.cursorNextOffset);   ✓ n ▷ equalsString("qqff");
      d ◁ list ▷ offset(cl.cursorDownOffset);   ✓ d ▷ equalsString("qqgggg");
      u ◁ list ▷ offset(cl.cursorUpOffset);     ✓ u ▷ equalsString("qqbbbb");
      P ◁ list ▷ offset(cl.pointerOffset);      ✓ P ▷ equalsString("qqee");
     }
   }

  i ◀ make$Image(draw, 2000, 2000, "$5.png", "9449");
  i ▷ free;
 }

int main (void)
 {void (*tests[])(void) = {test0, test1, test2, test3, test4,
                           test5, 0};
  run_tests("$", 1, tests);
  return 0;
}
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/g/cairoText/cairoText
