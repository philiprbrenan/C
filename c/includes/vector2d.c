#line 1 "/home/phil/c/z/vector2d/vector2d.c"
//------------------------------------------------------------------------------
// Vector2ds in 2d
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <math.h>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Vector2d.

#ifndef Vector2d_included_Vector2d
#define Vector2d_included_Vector2d
typedef struct Vector2d                                                                // Vector2d
 {const struct ProtoTypes_Vector2d *proto;                                             // Prototypes for methods
  const double x;                                                               // x coordinate
  const double y;                                                               // y coordinate
 } Vector2d;

static const double Vector2dnearness = 1e-6;                                           // Definition of near

#include <vector2d_prototypes.h>

//D1 Vectors                                                                    // Vector2ds in 2 dimensions

Vector2d makeVector2d                                                                         // Make a Vector2d
  (const double x,                                                              // x
   const double y)                                                              // y
 {return newVector2d(({struct Vector2d t = {x: x, y: y, proto: &ProtoTypes_Vector2d}; t;}));
 }

Vector2d Vector2dZ()                                                                          // Zero Vector2d
 {return newVector2d(({struct Vector2d t = {x: 0, y: 0, proto: &ProtoTypes_Vector2d}; t;}));
 }

Vector2d Vector2dX()                                                                          // Unit X Vector2d
 {return newVector2d(({struct Vector2d t = {x: 1, y: 0, proto: &ProtoTypes_Vector2d}; t;}));
 }

Vector2d Vector2dY()                                                                          // Unit Y Vector2d
 {return newVector2d(({struct Vector2d t = {x: 0, y: 1, proto: &ProtoTypes_Vector2d}; t;}));
 }

//D1 Nearness                                                                   // Check the nearness of Vector2d and scalars

static int Vector2dzero                                                                // Check a scalar is near zero
 (double s)                                                                     // Scalar
 {return fabs(s) < Vector2dnearness;
 }

static int Vector2done                                                                 // Check a scalar is near one
 (double s)                                                                     // Scalar
 {return fabs(s - 1) < Vector2dnearness;
 }

static int Vector2dnear                                                                // Check two scalars are near each other
 (double o,                                                                     // First
  double p)                                                                     // Second
 {return fabs(p - o) < Vector2dnearness;
 }

//D1 Methods                                                                    // Vector2d methods.

static int zero_int_Vector2d                                                           // Whether the specified Vector2d is equal to zero within the accuracy of floating point arithmetic
 (Vector2d o)                                                                          // Vector2d
 {return Vector2dnear(o.x, 0) && Vector2dnear(o.y, 0);
 }

static int eq_int_Vector2d                                                             // Whether two vectors are equal to within the accuracy of floating point arithmetic
 (Vector2d o,                                                                          // First
  Vector2d p)                                                                          // Second
 {return Vector2dnear(o.x, p.x) && Vector2dnear(o.y, p.y);
 }

static void dump_Vector2d                                                              //P Print a Vector2d to stderr
 (Vector2d o)                                                                          // Vector2d
 {lsprintf(d, 32, "(%f,%f)\n", o.x, o.y);
  say("%s", d);
 }

static Vector2d clone_Vector2d_Vector2d                                                              // Clone a Vector2d
 (Vector2d o)                                                                          // First
 {return newVector2d(({struct Vector2d t = {x: o.x, y: o.y, proto: &ProtoTypes_Vector2d}; t;}));
 }

static Vector2d plus_Vector2d_Vector2d_Vector2d                                                             // Add the first Vector2d to the second Vector2d and return the result.
 (Vector2d o,                                                                          // First Vector2d
  Vector2d p)                                                                          // Second Vector2d
 {return newVector2d(({struct Vector2d t = {x: o.x + p.x, y: o.y + p.y, proto: &ProtoTypes_Vector2d}; t;}));
 }

static Vector2d minus_Vector2d_Vector2d_Vector2d                                                            // Subtract the second Vector2d from the first Vector2d and return the result.
 (Vector2d o,                                                                          // First Vector2d
  Vector2d p)                                                                          // Second Vector2d
 {return newVector2d(({struct Vector2d t = {x: o.x - p.x, y: o.y - p.y, proto: &ProtoTypes_Vector2d}; t;}));
 }

static Vector2d multiply_Vector2d_Vector2d_double                                                    // Multiply the specified vector by the specified scalar and return the result.
 (Vector2d      o,                                                                     // Vector2d
  double s)                                                                     // Scalar
 {return newVector2d(({struct Vector2d t = {x: o.x * s, y: o.y * s, proto: &ProtoTypes_Vector2d}; t;}));
 }

static Vector2d divide_Vector2d_Vector2d_double                                                      // Divide the specified vector by the specified scalar and return the result.
 (Vector2d      o,                                                                     // Vector2d
  double s)                                                                     // Scalar
 {return newVector2d(({struct Vector2d t = {x: o.x / s, y: o.y / s, proto: &ProtoTypes_Vector2d}; t;}));
 }

static double  l_double_Vector2d                                                       // Length of the specified Vector2d
 (Vector2d o)                                                                          // Vector2d
 {return sqrt(o.x*o.x + o.y*o.y);
 }

static double  l2_double_Vector2d                                                      // Length squared of the specified Vector2d
 (Vector2d o)                                                                          // Vector2d
 {return o.x*o.x + o.y*o.y;
 }

static double d_double_Vector2d_Vector2d                                                      // Distance between the points identified by the specified Vector2d
 (Vector2d o,                                                                          // First Vector2d
  Vector2d p)                                                                          // Second Vector2d
 {const typeof(o.x - p.x) x = o.x - p.x; const typeof(o.y - p.y) y = o.y - p.y;
  return sqrt(x * x + y * y);
 }

static double d2_double_Vector2d_Vector2d                                                     // Distance squared between the points identified by the specified Vector2d
 (Vector2d o,                                                                          // First Vector2d
  Vector2d p)                                                                          // Second Vector2d
 {const typeof(o.x - p.x) x = o.x - p.x; const typeof(o.y - p.y) y = o.y - p.y;
  return x * x + y * y;
 }

static Vector2d n_Vector2d_Vector2d                                                                  // Return a normalized a copy of the specified Vector2d.
 (Vector2d o)                                                                          // Vector2d
 {const typeof(o.proto->l(o)) l = o.proto->l(o);
  if (Vector2dnear(l, 0)) printStackBackTrace("Cannot normalize a zero length vector\n");
  return makeVector2d(o.x / l, o.y / l);
 }

static double dot_double_Vector2d_Vector2d                                                    // Dot product of two vectors.
 (Vector2d o,                                                                          // First Vector2d
  Vector2d p)                                                                          // Second Vector2d
 {return o.x * p.x + o.y * p.y;
 }

static double area_double_Vector2d_Vector2d                                                   // Signed area of the parallelogram defined by the two vectors. The area is negative if the second vector appears to the right of the first if they are both placed at the origin and the observer stands against the z-axis in a left handed coordinate system.
 (Vector2d o,                                                                          // First Vector2d
  Vector2d p)                                                                          // Second Vector2d
 {return o.x * p.y - o.y * p.x;
 }

static double cosine_double_Vector2d_Vector2d                                                 // cos(angle between two vectors)
 (Vector2d o,                                                                          // First Vector2d
  Vector2d p)                                                                          // Second Vector2d
 {return o.proto->dot(o, p) / o.proto->l(o) / p.proto->l(p);
 }

static double  sine_double_Vector2d_Vector2d                                                  // sin(angle between two vectors)
 (Vector2d o,                                                                          // First Vector2d
  Vector2d p)                                                                          // Second Vector2d
 {return o.proto->area(o, p) / o.proto->l(o) / p.proto->l(p);
 }

static double  angle_double_Vector2d_Vector2d                                                 // Angle in radians anticlockwise that the first vector must be rotated to point along the second vector normalized to the range: -pi to +pi.
 (Vector2d o,                                                                          // First Vector2d
  Vector2d p)                                                                          // Second Vector2d
 {const typeof(o.proto->cosine(o, p)) c = o.proto->cosine(o, p);
  const typeof(o.proto->sine(o, p)) s = o.proto->sine(o, p);
  const typeof(acos(c)) a = acos(c);
  return s > 0 ? a : -a;
 }

static double  smallestAngleToNormalPlane_double_Vector2d_Vector2d                            // The smallest angle between the second vector and a plane normal to the first vector.
 (Vector2d a,                                                                          // First Vector2d
  Vector2d b)                                                                          // Second Vector2d
 {const typeof(fabs(a.proto->angle(a, b))) r = fabs(a.proto->angle(a, b));
  const typeof(M_PI / 2) p = M_PI / 2;
  return r < p ? p - r : r - p;
 }

static Vector2d  r90_Vector2d_Vector2d                                                               // Rotate the specified Vector2d by 90 degrees anticlockwise.
 (Vector2d o)                                                                          // Vector2d
 {return makeVector2d(-o.y, o.x);
 }

static Vector2d  r180_Vector2d_Vector2d                                                              // Rotate the specified Vector2d by 180 degrees.
 (Vector2d o)                                                                          // Vector2d
 {return makeVector2d(-o.x, -o.y);
 }

static Vector2d  r270_Vector2d_Vector2d                                                              // Rotate the specified Vector2d by 270 degrees.
 (Vector2d o)                                                                          // Vector2d
 {return makeVector2d(o.y, -o.x);
 }

static Vector2d  swap_Vector2d_Vector2d                                                              // Sap the components of the specified Vector2d and return the result.
 (Vector2d o)                                                                          // Vector2d
 {return makeVector2d(o.y, o.x);
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TmakeVector2d //Teq //TVector2dnear //TVector2dzero //TVector2done //Tswap //Tclone //Tplus //Tmultiply //Td //Td2 //Tl //Tl2 //Tdot //Tr90 //Tr180 //Tr270 //Tsine //Tcosine //Tarea //Tangle //TsmallestAngleToNormalPlane //Tn //Tzero //Tdivide //Tminus
 {const typeof(Vector2dZ()) z = Vector2dZ(); const typeof(Vector2dX()) x = Vector2dX(); const typeof(Vector2dY()) y = Vector2dY(); const typeof(x.proto->r180(x)) 𝘅 = x.proto->r180(x); const typeof(y.proto->r180(y)) 𝘆 = y.proto->r180(y);

  assert( z.proto->zero(z));  assert( x.proto->eq(x, makeVector2d(1, 0))); assert( y.proto->eq(y, makeVector2d(0, 1))); assert( z.proto->eq(z, x.proto->minus(x, x)));

  assert( y.proto->eq(y, x.proto->swap(x)));
  assert( x.proto->eq(x, y.proto->swap(y)));
  assert( x.proto->eq(x, x.proto->clone(x)));
  assert( y.proto->eq(y, y.proto->clone(y)));

  const typeof(x.proto->multiply(x, 3)) x3 = x.proto->multiply(x, 3); const typeof(y.proto->multiply(y, 4)) y4 = y.proto->multiply(y, 4); const typeof(x3.proto->plus(x3, y4)) h5 = x3.proto->plus(x3, y4);
  assert( Vector2dnear(h5.proto->l(h5)      ,  5));
  assert( Vector2dnear(h5.proto->l2(h5)     , 25));
  assert( Vector2dnear(z.proto->d(z, h5) ,  5));
  assert( Vector2dnear(z.proto->d2(z, h5) , 25));

    const typeof(h5.proto->divide(h5, 5)) 𝗵5 = h5.proto->divide(h5, 5);
  assert( 𝗵5.proto->eq(𝗵5, h5.proto->n(h5)));

  assert( Vector2dzero(x.proto->dot(x, y)));
  assert( Vector2dzero(y.proto->dot(y, x)));
  assert( Vector2done (x.proto->dot(x, x)));
  assert( Vector2done (y.proto->dot(y, y)));

  assert( Vector2dnear(x3.proto->dot(x3, h5), x3.proto->l2(x3)));
  assert( Vector2dnear(y4.proto->dot(y4, h5), y4.proto->l2(y4)));

  assert( x.proto->eq(x, 𝘆.proto->r90(𝘆)));  assert( y.proto->eq(y, x.proto->r90(x)));  assert( 𝘅.proto->eq(𝘅, y.proto->r90(y)));  assert( 𝘆.proto->eq(𝘆, 𝘅.proto->r90(𝘅)));
  assert( x.proto->eq(x, 𝘅.proto->r180(𝘅))); assert( y.proto->eq(y, 𝘆.proto->r180(𝘆))); assert( 𝘅.proto->eq(𝘅, x.proto->r180(x))); assert( 𝘆.proto->eq(𝘆, y.proto->r180(y)));
  assert( x.proto->eq(x, y.proto->r270(y))); assert( y.proto->eq(y, 𝘅.proto->r270(𝘅))); assert( 𝘅.proto->eq(𝘅, 𝘆.proto->r270(𝘆))); assert( 𝘆.proto->eq(𝘆, x.proto->r270(x)));

  assert( Vector2done (x.proto->cosine(x, x)));
  assert( Vector2done (y.proto->cosine(y, y)));
  assert( Vector2dzero(x.proto->cosine(x, y)));
  assert( Vector2dzero(y.proto->cosine(y, x)));

  assert( Vector2dzero(x.proto->sine(x, x)));
  assert( Vector2dzero(y.proto->sine(y, y)));
  assert( Vector2done (x.proto->sine(x, y)));
  assert( Vector2dnear(y.proto->sine(y, x), -1));


  const typeof(makeVector2d(1, 1)) xy = makeVector2d(1, 1);  assert( xy.x == 1);  assert( xy.y == 1);

  assert( Vector2dnear(-sqrt(1.0/2.0), xy.proto->sine(xy, x)));
  assert( Vector2dnear(+sqrt(1.0/2.0), xy.proto->sine(xy, y)));
  assert( Vector2dnear(-2,             xy.proto->area(xy, x.proto->multiply(x, 2))));
  assert( Vector2dnear(+2,             xy.proto->area(xy, y.proto->multiply(y, 2))));

  assert( Vector2dnear(x.proto->angle(x, xy), + M_PI / 4));
  assert( Vector2dnear(y.proto->angle(y, xy), - M_PI / 4));

  assert( Vector2dnear(y.proto->smallestAngleToNormalPlane(y, xy), M_PI / 4));
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, 0};
  run_tests("Vector2d", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/vector2D/vector2D
