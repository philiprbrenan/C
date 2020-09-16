#include <math.h>
#include <pango/pangocairo.h>
#define SIZE 2000

static void
draw_text
 (cairo_t *cr)
 {PangoLayout          * layout;
  PangoFontDescription * desc;

  cairo_translate(cr, 10, -400);

  layout = pango_cairo_create_layout(cr);                                        // Create a PangoLayout, set the font and text

  pango_layout_set_text(layout, "Cy", -1);
  desc = pango_font_description_from_string("Noto Sans Bold 1200");
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);
  cairo_set_source_rgb(cr, 1, 0, 0);

  pango_cairo_show_layout(cr, layout);

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
