#line 1 "/home/phil/c/g/z/pango2.c"
#define _GNU_SOURCE
#include <math.h>
#include <pango/pangocairo.h>
#include <utilities.c>
#define SIZE 2000

const size_t height = 400;

static void
draw_text
 (cairo_t *cr)
 {PangoLayout          * layout;
  PangoFontDescription * font;

  layout = pango_cairo_create_layout(cr);                                       // Create a PangoLayout, set the font and text

  lsprintf(fontDescription, 256, "Noto Bold 400");                    // Create font
  font = pango_font_description_from_string(fontDescription);
  //pango_font_description_set_absolute_size(font, height);

  pango_layout_set_font_description(layout, font);
  pango_font_description_free(font);

  cairo_translate(cr, 10, 10);                                                  // Draw a letter and bound it
  cairo_set_source_rgb(cr, 1, 0, 0);
  pango_layout_set_text(layout, "W", -1);
  pango_cairo_show_layout(cr, layout);

  PangoRectangle extents;                                                       // Bound previous letter
  pango_layout_get_pixel_extents(layout, 0, &extents);
  cairo_rectangle(cr, extents.x, extents.y, extents.width, extents.height);
  cairo_stroke(cr);

  cairo_translate(cr, extents.width, 0);                                        // Draw letter and bound it
  cairo_set_source_rgb(cr, 0, 1, 0);
  pango_layout_set_text(layout, "ither", -1);
  pango_cairo_show_layout(cr, layout);

  pango_layout_get_pixel_extents(layout, 0, &extents);
  cairo_rectangle(cr, extents.x, extents.y, extents.width, extents.height);
  cairo_stroke(cr);

  g_object_unref (layout);
 }

int main (void)
 {cairo_t *cr;
  cairo_surface_t *surface;

  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, SIZE, SIZE);
  cr      = cairo_create (surface);
  cairo_set_antialias (cr, CAIRO_ANTIALIAS_BEST);
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint  (cr);
  draw_text    (cr);
  cairo_destroy(cr);

  cairo_surface_write_to_png(surface, "pango.png");
  cairo_surface_destroy (surface);

  return 0;
}
