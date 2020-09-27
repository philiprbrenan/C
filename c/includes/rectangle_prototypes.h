static double width_double_Rectangle
 (const Rectangle rectangle);
static double height_double_Rectangle
 (const Rectangle rectangle);
static Rectangle center_Rectangle_Rectangle
 (const Rectangle rectangle);
static Rectangle translate_Rectangle_Rectangle_double_double
 (Rectangle       r,
  double  x,
  double  y);
static Rectangle scaleLow_Rectangle_Rectangle_double_double
 (Rectangle      r,
  double x,
  double y);
static Rectangle scaleHigh_Rectangle_Rectangle_double_double
 (Rectangle      r,
  double x,
  double y);
static Rectangle scaleCenter_Rectangle_Rectangle_double_double
 (Rectangle      r,
  double x,
  double y);
static int closeRectangle
 (const double a,
  const double b);
static int equals_int_Rectangle_Rectangle
 (const Rectangle r,
  const Rectangle R);
static int close_int_Rectangle_Rectangle
 (const Rectangle r,
  const Rectangle R);
static int containsPoint_int_Rectangle_double_double
 (const  Rectangle rectangle,
  double x,
  double y);
static int contains_int_Rectangle_Rectangle
 (const  Rectangle r,
  const  Rectangle p);
static int valid_int_Rectangle
 (const  Rectangle r);
static int point_int_Rectangle
 (const  Rectangle r);
static  Rectangle unionWith_Rectangle_Rectangle_Rectangle
 (const Rectangle r,
  const Rectangle p);
static Rectangle intersection_Rectangle_Rectangle_Rectangle
 (const  Rectangle r,
  const  Rectangle p);
static char * validAsString_Rectangle
 (const Rectangle r);
static void dump_Rectangle
 (const Rectangle r);
struct ProtoTypes_Rectangle {
  Rectangle  (*center)(                                                         // Return the point rectangle at the center of another rectangle
    const Rectangle rectangle);                                                 // Rectangle
  int  (*close)(                                                                // Confirm two rectangles are equal
    const Rectangle r,                                                          // Rectangle first
    const Rectangle R);                                                         // Rectangle second
  int  (*containsPoint)(                                                        // Check whether the specified Rectangle contains the specified coordinates
    const  Rectangle rectangle,                                                 // Rectangle
    double x,                                                                   // x coordinate of point
    double y);                                                                  // y coordinate of point
  int  (*contains)(                                                             // Check whether the specified Rectangle contains another Rectangle
    const  Rectangle r,                                                         // Containing Rectangle
    const  Rectangle p);                                                        // Other Rectangle
  void  (*dump)(                                                                // Write a Rectangle to stderr.
    const Rectangle r);                                                         // Rectangle
  int  (*equals)(                                                               // Equality to one part in a million being sufficient for our purposes
    const Rectangle r,                                                          // Rectangle first
    const Rectangle R);                                                         // Rectangle second
  double  (*height)(                                                            // Height
    const Rectangle rectangle);                                                 // Rectangle
  Rectangle  (*intersection)(                                                   // Return a valid rectangle from the intersection of the specified rectangles else return an invalid rectangle
    const  Rectangle r,                                                         // Containing Rectangle
    const  Rectangle p);                                                        // Other Rectangle
  int  (*point)(                                                                // Check whether the specified Rectangle is a point.
    const  Rectangle r);                                                        // Rectangle
  Rectangle  (*scaleCenter)(                                                    // Scale from the center
    Rectangle r,                                                                // Rectangle
    double x,                                                                   // x translation
    double y);                                                                  // y translation
  Rectangle  (*scaleHigh)(                                                      // Scale from the high corner
    Rectangle r,                                                                // Rectangle
    double x,                                                                   // x translation
    double y);                                                                  // y translation
  Rectangle  (*scaleLow)(                                                       // Scale from the low corner
    Rectangle r,                                                                // Rectangle
    double x,                                                                   // x translation
    double y);                                                                  // y translation
  Rectangle  (*translate)(                                                      // Translate the specified rectangle by the specified amounts
    Rectangle r,                                                                // Rectangle
    double x,                                                                   // x translation
    double y);                                                                  // y translation
  Rectangle  (*unionWith)(                                                      // Form union of two rectangles: the smallest rectangle that contains both of them
    const Rectangle r,                                                          // Containing Rectangle
    const Rectangle p);                                                         // Other Rectangle
  char *  (*validAsString)(                                                     // Validity of rectangle as a string
    const Rectangle r);                                                         // Rectangle
  int  (*valid)(                                                                // Check whether the specified Rectangle is valid.
    const  Rectangle r);                                                        // Rectangle
  double  (*width)(                                                             // Width
    const Rectangle rectangle);                                                 // Rectangle
 } const ProtoTypes_Rectangle =
{center_Rectangle_Rectangle, close_int_Rectangle_Rectangle, containsPoint_int_Rectangle_double_double, contains_int_Rectangle_Rectangle, dump_Rectangle, equals_int_Rectangle_Rectangle, height_double_Rectangle, intersection_Rectangle_Rectangle_Rectangle, point_int_Rectangle, scaleCenter_Rectangle_Rectangle_double_double, scaleHigh_Rectangle_Rectangle_double_double, scaleLow_Rectangle_Rectangle_double_double, translate_Rectangle_Rectangle_double_double, unionWith_Rectangle_Rectangle_Rectangle, validAsString_Rectangle, valid_int_Rectangle, width_double_Rectangle};
Rectangle newRectangle(Rectangle allocator) {return allocator;}

