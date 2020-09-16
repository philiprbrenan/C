static void clear_surface
 (cairo_surface_t     * surface);
static gboolean configure_event_cb
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



static gboolean draw_cb
 (GtkWidget * widget,

  cairo_t   * cr,

  gpointer    surface)

 {cairo_surface_t *s = *(cairo_surface_t **)surface;

  cairo_set_source_surface(cr, s, 0, 0);

  cairo_paint             (cr);

  return FALSE;

  if (0) widget = widget;

 }



static void drawBrush
 (GtkWidget       * widget,

  cairo_surface_t * surface,

  gdouble           x,

  gdouble           y)

 {cairo_t *cr;



  cr = cairo_create(surface);

  cairo_rectangle  (cr, x - 3, y - 3, 6, 6);

  cairo_fill       (cr);

  cairo_destroy    (cr);



  gtk_widget_queue_draw_area (widget, x - 3, y - 3, 6, 6);
 }



static gboolean button_press_event_cb
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



static gboolean motion_notify_event_cb
 (GtkWidget      *widget,

  GdkEventMotion *event,

  gpointer        surface)

 {cairo_surface_t *s = *(cairo_surface_t **)surface;



  if (event->state & GDK_BUTTON1_MASK) drawBrush(widget, s, event->x, event->y);
  return TRUE;

 }



static void close_window
 (GtkWidget      * widget,

  gpointer         surface)

 {cairo_surface_t *s = *(cairo_surface_t **)surface;

  cairo_surface_destroy(s);

  if (0) widget = widget;

 }



static void activate
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



  connect("draw",                G_CALLBACK(draw_cb));
  connect("configure-event",     G_CALLBACK(configure_event_cb));

  connect("motion-notify-event", G_CALLBACK(motion_notify_event_cb));
  connect("button-press-event",  G_CALLBACK(button_press_event_cb));



  gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area);
static gboolean draw_cb
 (GtkWidget * widget,

  cairo_t   * cr,

  gpointer    surface)

 {cairo_surface_t *s = *(cairo_surface_t **)surface;

  cairo_set_source_surface(cr, s, 0, 0);

  cairo_paint             (cr);

  return FALSE;

  if (0) widget = widget;

 }



static void drawBrush
 (GtkWidget       * widget,

  cairo_surface_t * surface,

  gdouble           x,

  gdouble           y)

 {cairo_t *cr;



  cr = cairo_create(surface);

  cairo_rectangle  (cr, x - 3, y - 3, 6, 6);

  cairo_fill       (cr);

  cairo_destroy    (cr);



  gtk_widget_queue_draw_area (widget, x - 3, y - 3, 6, 6);
 }



static gboolean button_press_event_cb
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



static gboolean motion_notify_event_cb
 (GtkWidget      *widget,

  GdkEventMotion *event,

  gpointer        surface)

 {cairo_surface_t *s = *(cairo_surface_t **)surface;



  if (event->state & GDK_BUTTON1_MASK) drawBrush(widget, s, event->x, event->y);
  return TRUE;

 }



static void close_window
 (GtkWidget      * widget,

  gpointer         surface)

 {cairo_surface_t *s = *(cairo_surface_t **)surface;

  cairo_surface_destroy(s);

  if (0) widget = widget;

 }



static void activate
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



  connect("draw",                G_CALLBACK(draw_cb));
  connect("configure-event",     G_CALLBACK(configure_event_cb));

  connect("motion-notify-event", G_CALLBACK(motion_notify_event_cb));
  connect("button-press-event",  G_CALLBACK(button_press_event_cb));



  gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area);
static void drawBrush
 (GtkWidget       * widget,

  cairo_surface_t * surface,

  gdouble           x,

  gdouble           y)

 {cairo_t *cr;



  cr = cairo_create(surface);

  cairo_rectangle  (cr, x - 3, y - 3, 6, 6);

  cairo_fill       (cr);

  cairo_destroy    (cr);



  gtk_widget_queue_draw_area (widget, x - 3, y - 3, 6, 6);
 }



static gboolean button_press_event_cb
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



static gboolean motion_notify_event_cb
 (GtkWidget      *widget,

  GdkEventMotion *event,

  gpointer        surface)

 {cairo_surface_t *s = *(cairo_surface_t **)surface;



  if (event->state & GDK_BUTTON1_MASK) drawBrush(widget, s, event->x, event->y);
  return TRUE;

 }



static void close_window
 (GtkWidget      * widget,

  gpointer         surface)

 {cairo_surface_t *s = *(cairo_surface_t **)surface;

  cairo_surface_destroy(s);

  if (0) widget = widget;

 }



static void activate
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



  connect("draw",                G_CALLBACK(draw_cb));
  connect("configure-event",     G_CALLBACK(configure_event_cb));

  connect("motion-notify-event", G_CALLBACK(motion_notify_event_cb));
  connect("button-press-event",  G_CALLBACK(button_press_event_cb));



  gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area);
static gboolean button_press_event_cb
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



static gboolean motion_notify_event_cb
 (GtkWidget      *widget,

  GdkEventMotion *event,

  gpointer        surface)

 {cairo_surface_t *s = *(cairo_surface_t **)surface;



  if (event->state & GDK_BUTTON1_MASK) drawBrush(widget, s, event->x, event->y);
  return TRUE;

 }



static void close_window
 (GtkWidget      * widget,

  gpointer         surface)

 {cairo_surface_t *s = *(cairo_surface_t **)surface;

  cairo_surface_destroy(s);

  if (0) widget = widget;

 }



static void activate
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



  connect("draw",                G_CALLBACK(draw_cb));
  connect("configure-event",     G_CALLBACK(configure_event_cb));

  connect("motion-notify-event", G_CALLBACK(motion_notify_event_cb));
  connect("button-press-event",  G_CALLBACK(button_press_event_cb));



  gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area);
static gboolean motion_notify_event_cb
 (GtkWidget      *widget,

  GdkEventMotion *event,

  gpointer        surface)

 {cairo_surface_t *s = *(cairo_surface_t **)surface;



  if (event->state & GDK_BUTTON1_MASK) drawBrush(widget, s, event->x, event->y);
  return TRUE;

 }



static void close_window
 (GtkWidget      * widget,

  gpointer         surface)

 {cairo_surface_t *s = *(cairo_surface_t **)surface;

  cairo_surface_destroy(s);

  if (0) widget = widget;

 }



static void activate
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



  connect("draw",                G_CALLBACK(draw_cb));
  connect("configure-event",     G_CALLBACK(configure_event_cb));

  connect("motion-notify-event", G_CALLBACK(motion_notify_event_cb));
  connect("button-press-event",  G_CALLBACK(button_press_event_cb));



  gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area);
static void close_window
 (GtkWidget      * widget,

  gpointer         surface)

 {cairo_surface_t *s = *(cairo_surface_t **)surface;

  cairo_surface_destroy(s);

  if (0) widget = widget;

 }



static void activate
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



  connect("draw",                G_CALLBACK(draw_cb));
  connect("configure-event",     G_CALLBACK(configure_event_cb));

  connect("motion-notify-event", G_CALLBACK(motion_notify_event_cb));
  connect("button-press-event",  G_CALLBACK(button_press_event_cb));



  gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area);
static void activate
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



  connect("draw",                G_CALLBACK(draw_cb));
  connect("configure-event",     G_CALLBACK(configure_event_cb));

  connect("motion-notify-event", G_CALLBACK(motion_notify_event_cb));
  connect("button-press-event",  G_CALLBACK(button_press_event_cb));



  gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area);
