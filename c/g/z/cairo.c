//------------------------------------------------------------------------------
// Cairo
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <cairo-xlib.h>
#include <utilities.c>

static void clear_surface                                                       // Paint the entire surface white
 (cairo_surface_t     * surface)                                                // Bitmap in which we actually draw
 {
  cairo_t *             cr = cairo_create(surface);
  say("AAA %d %d", cairo_xlib_surface_get_width(surface), cairo_xlib_surface_get_height(surface));
  cairo_set_antialias  (cr, CAIRO_ANTIALIAS_BEST);
  cairo_set_source_rgb (cr, 1, 1, 1);
  cairo_paint          (cr);
  cairo_rectangle      (cr, 200, 200, 200, 200);
  cairo_set_source_rgba(cr, 0.5, 1.0, 0.5, 0.5);
  cairo_set_line_width (cr, 10);
  cairo_stroke         (cr);
  cairo_translate      (cr, 100, 100);
  cairo_arc            (cr, 500, 500, 400, 0, 7);
  cairo_set_source_rgba(cr, 1, 0.5, 0.5, 0.5);
  cairo_fill           (cr);
  cairo_destroy        (cr);
}

static gboolean configure_event_cb                                              // Create a new surface of the appropriate size to store our scribbles
 (GtkWidget         *widget,
  GdkEventConfigure *event,
  gpointer           surface)
 {cairo_surface_t *s = *(cairo_surface_t **)surface =
    gdk_window_create_similar_surface
     (gtk_widget_get_window(widget),
      CAIRO_CONTENT_COLOR,
      gtk_widget_get_allocated_width (widget),
      gtk_widget_get_allocated_height(widget));

  clear_surface(s);
  return TRUE;
  if (0) event = event;
 }

static gboolean draw_cb                                                         // Copy drawn surface to widget being displayed
 (GtkWidget * widget,
  cairo_t   * cr,
  gpointer    surface)
 {cairo_surface_t *s = *(cairo_surface_t **)surface;
  cairo_set_source_surface(cr, s, 0, 0);
  cairo_paint             (cr);
  return FALSE;
  if (0) widget = widget;
 }

static void drawBrush                                                           // Draw a rectangle in to the bitmap
 (GtkWidget       * widget,
  cairo_surface_t * surface,
  gdouble           x,
  gdouble           y)
 {cairo_t *cr;

  cr = cairo_create(surface);
  cairo_rectangle  (cr, x - 3, y - 3, 6, 6);
  cairo_fill       (cr);
  cairo_destroy    (cr);

  gtk_widget_queue_draw_area (widget, x - 3, y - 3, 6, 6);                      // Request a redraw of the affected area so that draw gets called - with suitable clipping.
 }

static gboolean button_press_event_cb                                           // Button press
 (GtkWidget      *widget,
  GdkEventButton *event,
  gpointer        surface)
 {cairo_surface_t *s = *(cairo_surface_t **)surface;

  if (event->button == GDK_BUTTON_PRIMARY)
   {drawBrush(widget, s, event->x, event->y);
   }
  else if (event->button == GDK_BUTTON_SECONDARY)
   {clear_surface(s);
    gtk_widget_queue_draw(widget);
   }

  return TRUE;
}

static gboolean motion_notify_event_cb                                          // Motion
 (GtkWidget      *widget,
  GdkEventMotion *event,
  gpointer        surface)
 {cairo_surface_t *s = *(cairo_surface_t **)surface;

  if (event->state & GDK_BUTTON1_MASK) drawBrush(widget, s, event->x, event->y);// Draw a rectangle at the specified location
  return TRUE;
 }

static void close_window                                                        // Clean up when the app main window is destroyed
 (GtkWidget      * widget,
  gpointer         surface)
 {cairo_surface_t *s = *(cairo_surface_t **)surface;
  cairo_surface_destroy(s);
  if (0) widget = widget;
 }

static void activate                                                            // Create the widget hierarchy
 (GtkApplication * app,
  gpointer         surface)
 {GtkWidget * window       = gtk_application_window_new(app);
  GtkWidget * frame        = gtk_frame_new(NULL);;
  GtkWidget * drawing_area = gtk_drawing_area_new();

  gtk_window_set_title(GTK_WINDOW(window), "Drawing Area");

  gtk_container_set_border_width(GTK_CONTAINER(window), 0);
  gtk_container_add             (GTK_CONTAINER(window), frame);
  gtk_container_add             (GTK_CONTAINER(frame),  drawing_area);

  gtk_widget_set_size_request(drawing_area, 2000, 1000);

  g_signal_connect(window, "destroy", G_CALLBACK(close_window), surface);

  void connect(char * event, void * cb)
   {g_signal_connect(drawing_area, event, cb, surface);
   }

  connect("draw",                G_CALLBACK(draw_cb));                          // Signals used to handle the backing surface
  connect("configure-event",     G_CALLBACK(configure_event_cb));
  connect("motion-notify-event", G_CALLBACK(motion_notify_event_cb));           // Event signals
  connect("button-press-event",  G_CALLBACK(button_press_event_cb));

  gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area)       // Receive events not normally passed to a drawing area
                                     | GDK_BUTTON_PRESS_MASK
                                     | GDK_POINTER_MOTION_MASK);

  gtk_widget_show_all (window);
 }

int main()                                                                      // Create GTK application to draw on a surface
 {GtkApplication  * app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  cairo_surface_t * surface;                                                    // The surface we will draw on once the main window has been created
  g_signal_connect(app, "activate", G_CALLBACK(activate), &surface);            // Build the window structure
  g_application_run(G_APPLICATION(app), 0, 0);                                  // Run the application
  g_object_unref(app);                                                          // Free app resources by lowering reference count
  return 0;
 }
