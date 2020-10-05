//------------------------------------------------------------------------------
// $s in 2d
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <math.h>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an $.

#ifndef $_included_$
#define $_included_$
typedef struct $                                                                // $
 {const struct ProtoTypes_$ *proto;                                             // Prototypes for methods
  const double x;                                                               // x coordinate
  const double y;                                                               // y coordinate
 } $;

static const double $nearness = 1e-6;                                           // Definition of near

#include <$$_prototypes.h>

//D1 Vectors                                                                    // $s in 2 dimensions

$ make$                                                                         // Make a $
  (const double x,                                                              // x
   const double y)                                                              // y
 {return new $(x: x, y: y);
 }

$ $Z()                                                                          // Zero $
 {return new $(x: 0, y: 0);
 }

$ $X()                                                                          // Unit X $
 {return new $(x: 1, y: 0);
 }

$ $Y()                                                                          // Unit Y $
 {return new $(x: 0, y: 1);
 }

//D1 Nearness                                                                   // Check the nearness of $ and scalars

static int $zero                                                                // Check a scalar is near zero
 (double s)                                                                     // Scalar
 {return fabs(s) < $nearness;
 }

static int $one                                                                 // Check a scalar is near one
 (double s)                                                                     // Scalar
 {return fabs(s - 1) < $nearness;
 }

static int $near                                                                // Check two scalars are near each other
 (double o,                                                                     // First
  double p)                                                                     // Second
 {return fabs(p - o) < $nearness;
 }

//D1 Methods                                                                    // $ methods.

static int zero_int_$                                                           // Whether the specified $ is equal to zero within the accuracy of floating point arithmetic
 ($ o)                                                                          // $
 {return $near(o.x, 0) && $near(o.y, 0);
 }

static int eq_int_$                                                             // Whether two vectors are equal to within the accuracy of floating point arithmetic
 ($ o,                                                                          // First
  $ p)                                                                          // Second
 {return $near(o.x, p.x) && $near(o.y, p.y);
 }

static void dump_$                                                              //P Print a $ to stderr
 ($ o)                                                                          // $
 {lsprintf(d, 32, "(%f,%f)\n", o.x, o.y);
  say("%s", d);
 }

static $ clone_$_$                                                              // Clone a $
 ($ o)                                                                          // First
 {return new$(x: o.x, y: o.y);
 }

static $ plus_$_$_$                                                             // Add the first $ to the second $ and return the result.
 ($ o,                                                                          // First $
  $ p)                                                                          // Second $
 {return new $(x: o.x + p.x, y: o.y + p.y);
 }

static $ minus_$_$_$                                                            // Subtract the second $ from the first $ and return the result.
 ($ o,                                                                          // First $
  $ p)                                                                          // Second $
 {return new $(x: o.x - p.x, y: o.y - p.y);
 }

static $ multiply_$_$_double                                                    // Multiply the specified vector by the specified scalar and return the result.
 ($      o,                                                                     // $
  double s)                                                                     // Scalar
 {return new $(x: o.x * s, y: o.y * s);
 }

static $ divide_$_$_double                                                      // Divide the specified vector by the specified scalar and return the result.
 ($      o,                                                                     // $
  double s)                                                                     // Scalar
 {return new $(x: o.x / s, y: o.y / s);
 }

static double  l_double_$                                                       // Length of the specified $
 ($ o)                                                                          // $
 {return sqrt(o.x*o.x + o.y*o.y);
 }

static double  l2_double_$                                                      // Length squared of the specified $
 ($ o)                                                                          // $
 {return o.x*o.x + o.y*o.y;
 }

static double d_double_$_$                                                      // Distance between the points identified by the specified $
 ($ o,                                                                          // First $
  $ p)                                                                          // Second $
 {x ◁ o.x - p.x; y ◁ o.y - p.y;
  return sqrt(x * x + y * y);
 }

static double d2_double_$_$                                                     // Distance squared between the points identified by the specified $
 ($ o,                                                                          // First $
  $ p)                                                                          // Second $
 {x ◁ o.x - p.x; y ◁ o.y - p.y;
  return x * x + y * y;
 }

static $ n_$_$                                                                  // Return a normalized a copy of the specified $.
 ($ o)                                                                          // $
 {l ◁ o ▷ l;
  if ($near(l, 0)) printStackBackTrace("Cannot normalize a zero length vector\n");
  return make$(o.x / l, o.y / l);
 }

static double dot_double_$_$                                                    // Dot product of two vectors.
 ($ o,                                                                          // First $
  $ p)                                                                          // Second $
 {return o.x * p.x + o.y * p.y;
 }

static double area_double_$_$                                                   // Signed area of the parallelogram defined by the two vectors. The area is negative if the second vector appears to the right of the first if they are both placed at the origin and the observer stands against the z-axis in a left handed coordinate system.
 ($ o,                                                                          // First $
  $ p)                                                                          // Second $
 {return o.x * p.y - o.y * p.x;
 }

static double cosine_double_$_$                                                 // cos(angle between two vectors)
 ($ o,                                                                          // First $
  $ p)                                                                          // Second $
 {return o ▷ dot(p) / o ▷ l / p ▷ l;
 }

static double  sine_double_$_$                                                  // sin(angle between two vectors)
 ($ o,                                                                          // First $
  $ p)                                                                          // Second $
 {return o ▷ area(p) / o ▷ l / p ▷ l;
 }

static double  angle_double_$_$                                                 // Angle in radians anticlockwise that the first vector must be rotated to point along the second vector normalized to the range: -pi to +pi.
 ($ o,                                                                          // First $
  $ p)                                                                          // Second $
 {c ◁ o ▷ cosine(p);
  s ◁ o ▷ sine(p);
  a ◁ acos(c);
  return s > 0 ? a : -a;
 }

static double  smallestAngleToNormalPlane_double_$_$                            // The smallest angle between the second vector and a plane normal to the first vector.
 ($ a,                                                                          // First $
  $ b)                                                                          // Second $
 {r ◁ fabs(a ▷ angle(b));
  p ◁ M_PI / 2;
  return r < p ? p - r : r - p;
 }

static $  r90_$_$                                                               // Rotate the specified $ by 90 degrees anticlockwise.
 ($ o)                                                                          // $
 {return make$(-o.y, o.x);
 }

static $  r180_$_$                                                              // Rotate the specified $ by 180 degrees.
 ($ o)                                                                          // $
 {return make$(-o.x, -o.y);
 }

static $  r270_$_$                                                              // Rotate the specified $ by 270 degrees.
 ($ o)                                                                          // $
 {return make$(o.y, -o.x);
 }

static $  swap_$_$                                                              // Sap the components of the specified $ and return the result.
 ($ o)                                                                          // $
 {return make$(o.y, o.x);
 }
#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //Tmake$ //Teq //T$near //T$zero //T$one //Tswap //Tclone //Tplus //Tmultiply //Td //Td2 //Tl //Tl2 //Tdot //Tr90 //Tr180 //Tr270 //Tsine //Tcosine //Tarea //Tangle //TsmallestAngleToNormalPlane //Tn //Tzero //Tdivide //Tminus
 {z ◁ $Z(); x ◁ $X(); y ◁ $Y(); 𝘅 ◁ x ▷ r180; 𝘆 ◁ y ▷ r180;

  ✓ z ▷ zero;  ✓ x ▷ eq(make$(1, 0)); ✓ y ▷ eq(make$(0, 1)); ✓ z ▷ eq(x ▷ minus(x));

  ✓ y ▷ eq(x ▷ swap);
  ✓ x ▷ eq(y ▷ swap);
  ✓ x ▷ eq(x ▷ clone);
  ✓ y ▷ eq(y ▷ clone);

  x3 ◁ x ▷ multiply(3); y4 ◁ y ▷ multiply(4); h5 ◁ x3 ▷ plus(y4);
  ✓ $near(h5 ▷ l      ,  5);
  ✓ $near(h5 ▷ l2     , 25);
  ✓ $near(z  ▷ d (h5) ,  5);
  ✓ $near(z  ▷ d2(h5) , 25);

    𝗵5 ◁ h5 ▷ divide(5);
  ✓ 𝗵5 ▷ eq(h5 ▷ n);

  ✓ $zero(x ▷ dot(y));
  ✓ $zero(y ▷ dot(x));
  ✓ $one (x ▷ dot(x));
  ✓ $one (y ▷ dot(y));

  ✓ $near(x3 ▷ dot(h5), x3 ▷ l2);
  ✓ $near(y4 ▷ dot(h5), y4 ▷ l2);

  ✓ x ▷ eq(𝘆 ▷ r90);  ✓ y ▷ eq(x ▷ r90);  ✓ 𝘅 ▷ eq(y ▷ r90);  ✓ 𝘆 ▷ eq(𝘅 ▷ r90);
  ✓ x ▷ eq(𝘅 ▷ r180); ✓ y ▷ eq(𝘆 ▷ r180); ✓ 𝘅 ▷ eq(x ▷ r180); ✓ 𝘆 ▷ eq(y ▷ r180);
  ✓ x ▷ eq(y ▷ r270); ✓ y ▷ eq(𝘅 ▷ r270); ✓ 𝘅 ▷ eq(𝘆 ▷ r270); ✓ 𝘆 ▷ eq(x ▷ r270);

  ✓ $one (x ▷ cosine(x));
  ✓ $one (y ▷ cosine(y));
  ✓ $zero(x ▷ cosine(y));
  ✓ $zero(y ▷ cosine(x));

  ✓ $zero(x ▷ sine(x));
  ✓ $zero(y ▷ sine(y));
  ✓ $one (x ▷ sine(y));
  ✓ $near(y ▷ sine(x), -1);


  xy ◁ make$(1, 1);  ✓ xy.x == 1;  ✓ xy.y == 1;

  ✓ $near(-sqrt(1.0/2.0), xy ▷ sine(x));
  ✓ $near(+sqrt(1.0/2.0), xy ▷ sine(y));
  ✓ $near(-2,             xy ▷ area(x ▷ multiply(2)));
  ✓ $near(+2,             xy ▷ area(y ▷ multiply(2)));

  ✓ $near(x ▷ angle(xy), + M_PI / 4);
  ✓ $near(y ▷ angle(xy), - M_PI / 4);

  ✓ $near(y ▷ smallestAngleToNormalPlane(xy), M_PI / 4);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/vector2D/vector2D
