static double width_double_Rectangle
 (const Rectangle * rectangle);
static double height_double_Rectangle
 (const Rectangle * rectangle);
static Rectangle center_Rectangle_Rectangle
 (const Rectangle * rectangle);
static Rectangle translate_Rectangle_Rectangle_double_double
 (Rectangle    *  r,
  double  x,
  double  y);
static Rectangle scaleLow_Rectangle_Rectangle_double_double
 (Rectangle    * r,
  double x,
  double y);
static Rectangle scaleHigh_Rectangle_Rectangle_double_double
 (Rectangle    * r,
  double x,
  double y);
static Rectangle scaleCenter_Rectangle_Rectangle_double_double
 (Rectangle    * r,
  double x,
  double y);
static int closeRectangle
 (const double a,
  const double b);
static int equals_int_Rectangle_Rectangle
 (const Rectangle * r,
  const Rectangle   R);
static int close_int_Rectangle_Rectangle
 (const Rectangle * r,
  const Rectangle   R);
static int containsPoint_int_Rectangle_double_double
 (const  Rectangle * rectangle,
  double x,
  double y);
static int contains_int_Rectangle_Rectangle
 (const  Rectangle * r,
  const  Rectangle   p);
static int containsACorner_int_Rectangle_Rectangle
 (const  Rectangle * r,
  const  Rectangle   p);
static int valid_int_Rectangle
 (const  Rectangle * r);
static int point_int_Rectangle
 (const  Rectangle * r);
static double area_double__Rectangle
 (const  Rectangle * r);
static  Rectangle unionWith_Rectangle_Rectangle_Rectangle
 (const Rectangle * r,
  const Rectangle   p);
static Rectangle intersection_Rectangle_Rectangle_Rectangle
 (const  Rectangle * r,
  const  Rectangle   p);
static double intersectionArea_double__Rectangle_Rectangle
 (const  Rectangle * r,
  const  Rectangle   p);
static  RectanglePair left_RectanglePair_Rectangle_double
 (const Rectangle    * r,
  const double d);
static  RectanglePair right_RectanglePair_Rectangle_double
 (const Rectangle    * r,
  const double d);
static  RectanglePair down_RectanglePair_Rectangle_double
 (const Rectangle    * r,
  const double d);
static  RectanglePair up_RectanglePair_Rectangle_double
 (const Rectangle    * r,
  const double d);
static int valid_int_RectanglePair
 (const RectanglePair * p);
static char * validAsString_Rectangle
 (const Rectangle * r);
static void dump_Rectangle
 (const Rectangle * r);
struct ProtoTypes_Rectangle {
  double  (*area)(                                                              // Area of a Rectangle - return 0 for an invalid Rectangle
    const  Rectangle * r);                                                      // Rectangle
  Rectangle  (*center)(                                                         // Return the point rectangle at the center of another rectangle
    const Rectangle * rectangle);                                               // Rectangle
  int  (*close)(                                                                // Confirm two rectangles are equal
    const Rectangle * r,                                                        // Rectangle first
    const Rectangle R);                                                         // Rectangle second
  int  (*containsACorner)(                                                      // Check whether the specified Rectangle contains any of the corners of another Rectangle
    const  Rectangle * r,                                                       // Containing Rectangle
    const  Rectangle p);                                                        // Other Rectangle
  int  (*containsPoint)(                                                        // Check whether the specified Rectangle contains the specified coordinates
    const  Rectangle * rectangle,                                               // Rectangle
    double x,                                                                   // x coordinate of point
    double y);                                                                  // y coordinate of point
  int  (*contains)(                                                             // Check whether the specified Rectangle contains all the corners of another Rectangle
    const  Rectangle * r,                                                       // Containing Rectangle
    const  Rectangle p);                                                        // Other Rectangle
  RectanglePair  (*down)(                                                       // Split the Rectangle horizontally the specified distance down from the low corner: remember that y increases down the page
    const Rectangle    * r,                                                     // Rectangle
    const double d);                                                            // Distance
  void  (*dump)(                                                                // Write a Rectangle to stderr->
    const Rectangle * r);                                                       // Rectangle
  int  (*equals)(                                                               // Equality to one part in a million being sufficient for our purposes
    const Rectangle * r,                                                        // Rectangle first
    const Rectangle R);                                                         // Rectangle second
  double  (*height)(                                                            // Height
    const Rectangle * rectangle);                                               // Rectangle
  double  (*intersectionArea)(                                                  // Area of the intersection between two Rectangle
    const  Rectangle * r,                                                       // Containing Rectangle
    const  Rectangle p);                                                        // Other Rectangle
  Rectangle  (*intersection)(                                                   // Return a valid Rectangle from the intersection of the specified Rectangle else return an invalid Rectangle
    const  Rectangle * r,                                                       // Containing Rectangle
    const  Rectangle p);                                                        // Other Rectangle
  RectanglePair  (*left)(                                                       // Split the Rectangle vertically the specified distance from the left hand side
    const Rectangle    * r,                                                     // Rectangle
    const double d);                                                            // Distance
  int  (*point)(                                                                // Check whether the specified Rectangle is a point.
    const  Rectangle * r);                                                      // Rectangle
  RectanglePair  (*right)(                                                      // Split the Rectangle vertically the specified distance from the right hand side
    const Rectangle    * r,                                                     // Rectangle
    const double d);                                                            // Distance
  Rectangle  (*scaleCenter)(                                                    // Scale from the center
    Rectangle    * r,                                                           // Rectangle
    double x,                                                                   // x translation
    double y);                                                                  // y translation
  Rectangle  (*scaleHigh)(                                                      // Scale from the high corner
    Rectangle    * r,                                                           // Rectangle
    double x,                                                                   // x translation
    double y);                                                                  // y translation
  Rectangle  (*scaleLow)(                                                       // Scale from the low corner
    Rectangle    * r,                                                           // Rectangle
    double x,                                                                   // x translation
    double y);                                                                  // y translation
  Rectangle  (*translate)(                                                      // Translate the specified rectangle by the specified amounts
    Rectangle    * r,                                                           // Rectangle
    double x,                                                                   // x translation
    double y);                                                                  // y translation
  Rectangle  (*unionWith)(                                                      // Form union of two Rectangle: the smallest Rectangle that contains both of them
    const Rectangle * r,                                                        // Containing Rectangle
    const Rectangle p);                                                         // Other Rectangle
  RectanglePair  (*up)(                                                         // Split the Rectangle vertically the specified distance up from the high corner: remember that y increases down the page
    const Rectangle    * r,                                                     // Rectangle
    const double d);                                                            // Distance
  char *  (*validAsString)(                                                     // Validity of rectangle as a string
    const Rectangle * r);                                                       // Rectangle
  int  (*valid)(                                                                // Check whether the specified Rectangle is valid.
    const  Rectangle * r);                                                      // Rectangle
  double  (*width)(                                                             // Width
    const Rectangle * rectangle);                                               // Rectangle
 } const ProtoTypes_Rectangle =
{area_double__Rectangle, center_Rectangle_Rectangle, close_int_Rectangle_Rectangle, containsACorner_int_Rectangle_Rectangle, containsPoint_int_Rectangle_double_double, contains_int_Rectangle_Rectangle, down_RectanglePair_Rectangle_double, dump_Rectangle, equals_int_Rectangle_Rectangle, height_double_Rectangle, intersectionArea_double__Rectangle_Rectangle, intersection_Rectangle_Rectangle_Rectangle, left_RectanglePair_Rectangle_double, point_int_Rectangle, right_RectanglePair_Rectangle_double, scaleCenter_Rectangle_Rectangle_double_double, scaleHigh_Rectangle_Rectangle_double_double, scaleLow_Rectangle_Rectangle_double_double, translate_Rectangle_Rectangle_double_double, unionWith_Rectangle_Rectangle_Rectangle, up_RectanglePair_Rectangle_double, validAsString_Rectangle, valid_int_Rectangle, width_double_Rectangle};
Rectangle newRectangle(Rectangle allocator) {return allocator;}

struct ProtoTypes_RectanglePair {
  int  (*valid)(                                                                // Confirm that a split was valid
    const RectanglePair * p);                                                   // RectanglePair
 } const ProtoTypes_RectanglePair =
{valid_int_RectanglePair};
RectanglePair newRectanglePair(RectanglePair allocator) {return allocator;}

