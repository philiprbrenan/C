static int Vector2dzero
 (double s);
static int Vector2done
 (double s);
static int Vector2dnear
 (double o,
  double p);
static int zero_int_Vector2d
 (Vector2d o);
static int eq_int_Vector2d
 (Vector2d o,
  Vector2d p);
static void dump_Vector2d
 (Vector2d o);
static Vector2d clone_Vector2d_Vector2d
 (Vector2d o);
static Vector2d plus_Vector2d_Vector2d_Vector2d
 (Vector2d o,
  Vector2d p);
static Vector2d minus_Vector2d_Vector2d_Vector2d
 (Vector2d o,
  Vector2d p);
static Vector2d multiply_Vector2d_Vector2d_double
 (Vector2d      o,
  double s);
static Vector2d divide_Vector2d_Vector2d_double
 (Vector2d      o,
  double s);
static double  l_double_Vector2d
 (Vector2d o);
static double  l2_double_Vector2d
 (Vector2d o);
static double d_double_Vector2d_Vector2d
 (Vector2d o,
  Vector2d p);
static double d2_double_Vector2d_Vector2d
 (Vector2d o,
  Vector2d p);
static Vector2d n_Vector2d_Vector2d
 (Vector2d o);
static double dot_double_Vector2d_Vector2d
 (Vector2d o,
  Vector2d p);
static double area_double_Vector2d_Vector2d
 (Vector2d o,
  Vector2d p);
static double cosine_double_Vector2d_Vector2d
 (Vector2d o,
  Vector2d p);
static double  sine_double_Vector2d_Vector2d
 (Vector2d o,
  Vector2d p);
static double  angle_double_Vector2d_Vector2d
 (Vector2d o,
  Vector2d p);
static double  smallestAngleToNormalPlane_double_Vector2d_Vector2d
 (Vector2d a,
  Vector2d b);
static Vector2d  r90_Vector2d_Vector2d
 (Vector2d o);
static Vector2d  r180_Vector2d_Vector2d
 (Vector2d o);
static Vector2d  r270_Vector2d_Vector2d
 (Vector2d o);
static Vector2d  swap_Vector2d_Vector2d
 (Vector2d o);
struct ProtoTypes_Vector2d {
  double   (*angle)(                                                            // Angle in radians anticlockwise that the first vector must be rotated to point along the second vector normalized to the range: -pi to +pi.
    Vector2d o,                                                                 // First Vector2d
    Vector2d p);                                                                // Second Vector2d
  double  (*area)(                                                              // Signed area of the parallelogram defined by the two vectors. The area is negative if the second vector appears to the right of the first if they are both placed at the origin and the observer stands against the z-axis in a left handed coordinate system.
    Vector2d o,                                                                 // First Vector2d
    Vector2d p);                                                                // Second Vector2d
  Vector2d  (*clone)(                                                           // Clone a Vector2d
    Vector2d o);                                                                // First
  double  (*cosine)(                                                            // cos(angle between two vectors)
    Vector2d o,                                                                 // First Vector2d
    Vector2d p);                                                                // Second Vector2d
  double  (*d2)(                                                                // Distance squared between the points identified by the specified Vector2d
    Vector2d o,                                                                 // First Vector2d
    Vector2d p);                                                                // Second Vector2d
  double  (*d)(                                                                 // Distance between the points identified by the specified Vector2d
    Vector2d o,                                                                 // First Vector2d
    Vector2d p);                                                                // Second Vector2d
  Vector2d  (*divide)(                                                          // Divide the specified vector by the specified scalar and return the result.
    Vector2d o,                                                                 // Vector2d
    double s);                                                                  // Scalar
  double  (*dot)(                                                               // Dot product of two vectors.
    Vector2d o,                                                                 // First Vector2d
    Vector2d p);                                                                // Second Vector2d
  void  (*dump)(                                                                // Print a Vector2d to stderr
    Vector2d o);                                                                // Vector2d
  int  (*eq)(                                                                   // Whether two vectors are equal to within the accuracy of floating point arithmetic
    Vector2d o,                                                                 // First
    Vector2d p);                                                                // Second
  double   (*l2)(                                                               // Length squared of the specified Vector2d
    Vector2d o);                                                                // Vector2d
  double   (*l)(                                                                // Length of the specified Vector2d
    Vector2d o);                                                                // Vector2d
  Vector2d  (*minus)(                                                           // Subtract the second Vector2d from the first Vector2d and return the result.
    Vector2d o,                                                                 // First Vector2d
    Vector2d p);                                                                // Second Vector2d
  Vector2d  (*multiply)(                                                        // Multiply the specified vector by the specified scalar and return the result.
    Vector2d o,                                                                 // Vector2d
    double s);                                                                  // Scalar
  Vector2d  (*n)(                                                               // Return a normalized a copy of the specified Vector2d.
    Vector2d o);                                                                // Vector2d
  Vector2d  (*plus)(                                                            // Add the first Vector2d to the second Vector2d and return the result.
    Vector2d o,                                                                 // First Vector2d
    Vector2d p);                                                                // Second Vector2d
  Vector2d   (*r180)(                                                           // Rotate the specified Vector2d by 180 degrees.
    Vector2d o);                                                                // Vector2d
  Vector2d   (*r270)(                                                           // Rotate the specified Vector2d by 270 degrees.
    Vector2d o);                                                                // Vector2d
  Vector2d   (*r90)(                                                            // Rotate the specified Vector2d by 90 degrees anticlockwise.
    Vector2d o);                                                                // Vector2d
  double   (*sine)(                                                             // sin(angle between two vectors)
    Vector2d o,                                                                 // First Vector2d
    Vector2d p);                                                                // Second Vector2d
  double   (*smallestAngleToNormalPlane)(                                       // The smallest angle between the second vector and a plane normal to the first vector.
    Vector2d a,                                                                 // First Vector2d
    Vector2d b);                                                                // Second Vector2d
  Vector2d   (*swap)(                                                           // Sap the components of the specified Vector2d and return the result.
    Vector2d o);                                                                // Vector2d
  int  (*zero)(                                                                 // Whether the specified Vector2d is equal to zero within the accuracy of floating point arithmetic
    Vector2d o);                                                                // Vector2d
 } const ProtoTypes_Vector2d =
{angle_double_Vector2d_Vector2d, area_double_Vector2d_Vector2d, clone_Vector2d_Vector2d, cosine_double_Vector2d_Vector2d, d2_double_Vector2d_Vector2d, d_double_Vector2d_Vector2d, divide_Vector2d_Vector2d_double, dot_double_Vector2d_Vector2d, dump_Vector2d, eq_int_Vector2d, l2_double_Vector2d, l_double_Vector2d, minus_Vector2d_Vector2d_Vector2d, multiply_Vector2d_Vector2d_double, n_Vector2d_Vector2d, plus_Vector2d_Vector2d_Vector2d, r180_Vector2d_Vector2d, r270_Vector2d_Vector2d, r90_Vector2d_Vector2d, sine_double_Vector2d_Vector2d, smallestAngleToNormalPlane_double_Vector2d_Vector2d, swap_Vector2d_Vector2d, zero_int_Vector2d};
Vector2d newVector2d(Vector2d allocator) {return allocator;}

