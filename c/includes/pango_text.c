#line 1 "/home/phil/c/g/pango/pango_text.c"
//------------------------------------------------------------------------------
// Draw text using Pango
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <math.h>
#include <pango/pangocairo.h>
#include <stringBuffer.c>
#include <utilities.c>
#define SIZE 2000

//D1 Structures                                                                 // Structures describing an Arena Tree.
#ifndef Pango_text_included
#define Pango_text_included

typedef struct Pango_textText                                                            // Draw some text
 {const struct ProtoTypes_Pango_text *proto;                                             // Methods associated with an arena tree
  cairo_t *cr;                                                                  // The cairo context to use
//pango_layout_t *layout;                                                       // The pango layout to use
  char * font;                                                                  // String describing the font to use.
 } Pango_text;

//D1 Constructors                                                               // Construct a new Arena tree.

//static Pango_text makePango_text                                                                  // Create a new arena tree
// ()                                                                             // ArenaTree allocator
// {return newPango_textText(({struct Pango_textText t = {proto: &ProtoTypes_Pango_textText};   t;}));                                                             // Arena tree we are creating
// }

static PangoRectangle draw_text
 (cairo_t * cr,
  char    * fontString,
  char    * text)
 {PangoLayout          * layout;
  PangoFontDescription * font;

  layout = pango_cairo_create_layout(cr);                                       // Create a PangoLayout, set the font and text

  font = pango_font_description_from_string(fontString);
  pango_layout_set_font_description(layout, font);
  pango_font_description_free(font);

  pango_layout_set_text(layout, text, -1);
  pango_cairo_show_layout(cr, layout);

  PangoRectangle extents;                                                       // Bound previous letter
  pango_layout_get_pixel_extents(layout, 0, &extents);
  cairo_rectangle(cr, extents.x, extents.y, extents.width, extents.height);
  cairo_stroke(cr);

  g_object_unref (layout);

  return extents;
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void createImage
 (void (*draw)(cairo_t *cr))
 {cairo_surface_t * surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, SIZE, SIZE);
  cairo_t         *    cr   = cairo_create (surface);
  cairo_set_antialias (cr, CAIRO_ANTIALIAS_BEST);
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint         (cr);
  draw                (cr);
  cairo_destroy       (cr);
  cairo_surface_write_to_png(surface, "pango.png");
  cairo_surface_destroy (surface);
}

void test0()
 {void draw(cairo_t *cr)
   {cairo_translate(cr, 10, 10);
    cairo_set_source_rgb(cr, 0, 1.0, 1.0);
    draw_text(cr, "Noto Bold 400", "World");
   }
  createImage(draw);
  const typeof(makeStringBufferFromString("b2sum pango.png")) c = makeStringBufferFromString("b2sum pango.png");
  c.proto->system(c);
  assert(c.proto->containsString(c, "fade60d"));
 }

int main (void)
 {void (*tests[])(void) = {test0, 0};
  run_tests("Pango_text", 1, tests);
  return 0;
}
#endif
