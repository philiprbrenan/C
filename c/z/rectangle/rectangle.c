//------------------------------------------------------------------------------
// Rectangles
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
/*
 Operations on rectangles that are parallel to the coordinate axes.
*/
#define _GNU_SOURCE
#include <math.h>
#include <cairo.h>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an $.

#ifndef $_included_$
#define $_included_$
typedef enum type$                                                              // Valid, point, vertical, horizontal, invalid rectangle
 {type$_valid, type$_point, type$_vertical, type$_horizontal, type$_invalid
 } type$;

typedef struct $                                                                // $
 {const struct ProtoTypes_$ *proto;                                             // Prototypes for methods
  const double x, y, X, Y;                                                      // Coordinates of two opposing corners with lower x and y first
  const type$  t;                                                               //Type of rectangle
 } $;

typedef struct $Pair                                                            // A pair of smaller rectangles resulting from fission of a larger rectangle
 {const struct ProtoTypes_$Pair *proto;                                         // Prototypes for methods
  const $ source, a, b;                                                         // The source rectangle and the fission pair.
  const double d;                                                               // The splitting distance
  const int valid;                                                              // True if this pair is valid
 } $Pair;
#endif

#ifndef $_included
#define $_included
#include <$$_prototypes.h>

$ make$                                                                         // Make a $
 (double x,                                                                     // Low x
  double y,                                                                     // Low y
  double X,                                                                     // High x
  double Y)                                                                     // High y
 {cx ◁ close$(x, X); cy ◁ close$(y, Y);
  type$              T = type$_valid;
  if      (cx && cy) T = type$_point;
  else if (cx)       T = type$_horizontal;
  else if (cy)       T = type$_vertical;

  return new$(x: x, y: y, X: X, Y: Y, t: T);
 }

$ make$WH                                                                       // Make a $ with a specified width and height
 (double x,                                                                     // Low x
  double y,                                                                     // Low y
  double w,                                                                     // Width
  double h)                                                                     // Height
 {return make$(x, y, x + w, y + h);
 }

$ makeCopy$                                                                     // Make a copy of a $
 (const $ r)                                                                    // $
 {return new$(x: r.x, y: r.y, X: r.X, Y: r.Y, t: r.t);
 }

$ makeInvalid$()                                                                // Make an invalid $
 {return new$(t: type$_invalid);
 }

$ makePoint$                                                                    // Make a $ point
 (double x,                                                                     // x coordinate of point
  double y)                                                                     // y coordinate of point
 {return make$(x, y, x, y);
 }

$ makeZero$()                                                                   // Make a zero $
 {return make$(0,0,0,0);
 }

$ makeUnit$()                                                                   // Make a unit $
 {return make$(0,0,1,1);
 }

//D1 Dimensions                                                                 // Dimensions of the rectangle

static double width_double_$                                                    // Width
 (const $ * rectangle)                                                          // $
 {return fabs(rectangle->X - rectangle->x);
 }

static double height_double_$                                                   // Height
 (const $ * rectangle)                                                          // $
 {return fabs(rectangle->Y - rectangle->y);
 }

static $ center_$_$                                                             // Return the point rectangle at the center of another rectangle
 (const $ * rectangle)                                                          // $
 {return makePoint$((rectangle->x + rectangle->X) / 2,
                    (rectangle->y + rectangle->Y) / 2);
 }

//D1 Transformations                                                            // Transform $

static $ translate_$_$_double_double                                            // Translate the specified rectangle by the specified amounts
 ($    *  r,                                                                    // $
  double  x,                                                                    // x translation
  double  y)                                                                    // y translation
 {return make$(r->x + x, r->y + y, r->X + x, r->Y + y);
 }

static $ scaleLow_$_$_double_double                                             // Scale from the low corner
 ($    * r,                                                                     // $
  double x,                                                                     // x translation
  double y)                                                                     // y translation
 {w ◁ r ▶ width; h ◁ r ▶ height;
  return make$(r->x, r->y, r->x + w * x, r->y + h * y);
 }

static $ scaleHigh_$_$_double_double                                            // Scale from the high corner
 ($    * r,                                                                     // $
  double x,                                                                     // x translation
  double y)                                                                     // y translation
 {w ◁ r ▶ width; h ◁ r ▶ height;
  return make$(r->X - w * x, r->Y - h * y, r->X, r->Y);
 }

static $ scaleCenter_$_$_double_double                                          // Scale from the center
 ($    * r,                                                                     // $
  double x,                                                                     // x translation
  double y)                                                                     // y translation
 {w ◁ r ▶ width / 2; h ◁ r ▶ height / 2;
  return make$(r->x + w * (1 - x), r->y + h * (1 - y),
               r->X + w * (x - 1), r->Y + h * (y - 1));
 }

//D1 Statistics                                                                 // Statistics describing a rectangle

static int close$                                                               // Equality to one part in a million being sufficient for our purposes
 (const double a,                                                               // First
  const double b)                                                               // Second
 {d ◁ 1e-6;                                                                     // Accuracy
  return fabs(a - b) < d;
 }

static int equals_int_$_$                                                       // Equality to one part in a million being sufficient for our purposes
 (const $ * r,                                                                  // $ first
  const $   R)                                                                  // $ second
 {a ◁ r ▶ close(R);
  if (a) return 1;                                                              // Close enough
  say("Rectangles not equal:\n");
  say("have: x:%f, y:%f, width:%f, height:%f valid: %s\n", r->x, r->y, r ▶ width, r ▶ height, r ▶ validAsString);
  say("want: x:%f, y:%f, width:%f, height:%f valid: %s\n", r->x, r->y, R ▷ width, R ▷ height, R ▷ validAsString);
  return 0;                                                                     // Not close enough
 }

static int close_int_$_$                                                        // Confirm two rectangles are equal
 (const $ * r,                                                                  // $ first
  const $   R)                                                                  // $ second
 {return close$(r->x, R.x) && close$(r->y, R.y) &&
         close$(r->X, R.X) && close$(r->Y, R.Y);
 }

static int containsPoint_int_$_double_double                                    // Check whether the specified $ contains the specified coordinates
 (const  $ * rectangle,                                                         // $
  double x,                                                                     // x coordinate of point
  double y)                                                                     // y coordinate of point
 {return x >= rectangle->x && y >= rectangle->y &&
         x <= rectangle->X && y <= rectangle->Y;
 }

static int contains_int_$_$                                                     // Check whether the specified $ contains all the corners of another $
 (const  $ * r,                                                                 // Containing $
  const  $   p)                                                                 // Other $
 {return r ▶ containsPoint(p.x, p.y) && r ▶ containsPoint(p.X, p.Y);
 }

static int containsACorner_int_$_$                                              // Check whether the specified $ contains any of the corners of another $
 (const  $ * r,                                                                 // Containing $
  const  $   p)                                                                 // Other $
 {return r ▶ containsPoint(p.x, p.y) || r ▶ containsPoint(p.x, p.Y) ||
         r ▶ containsPoint(p.X, p.y) || r ▶ containsPoint(p.X, p.Y);
 }

static int valid_int_$                                                          // Check whether the specified $ is valid.
 (const  $ * r)                                                                 // $
 {return r->t != type$_invalid;
 }

static int point_int_$                                                          // Check whether the specified $ is a point.
 (const  $ * r)                                                                 // $
 {return r->t == type$_point;
 }

static double area_double__$                                                    // Area of a $ - return 0 for an invalid $
 (const  $ * r)                                                                 // $
 {if (!r ▶ valid) return 0.0;                                                   // Invalid rectangle has no area
  w ◁ r ▶ width;                                                                // Dimensions
  h ◁ r ▶ height;
  return w * h;                                                                 // Area
 }

//D1 Union and Intersection                                                     // Form $ from the union and  intersection of other $

static  $ unionWith_$_$_$                                                       // Form union of two $: the smallest $ that contains both of them
 (const $ * r,                                                                  // Containing $
  const $   p)                                                                  // Other $
 {return make$(r->x < p.x ? r->x : p.x, r->y < p.y ? r->y : p.y,
               r->X > p.X ? r->X : p.X, r->Y > p.Y ? r->Y : p.Y);
 }

static $ intersection_$_$_$                                                     // Return a valid $ from the intersection of the specified $ else return an invalid $
 (const  $ * r,                                                                 // Containing $
  const  $   p)                                                                 // Other $
 {if (p ▷ containsPoint(r->x, r->y) || p ▷ containsPoint(r->X, r->Y) ||         // Check that one rectangle overlaps the other
      r ▶ containsPoint(p.x, p.y) || r ▶ containsPoint(p.X, p.Y))
   {return make$(r->x > p.x ? r->x : p.x, r->y > p.y ? r->y : p.y,              // Overlap
                 r->X < p.X ? r->X : p.X, r->Y < p.Y ? r->Y : p.Y);
   }
  else return makeInvalid$();                                                   // No intersection
 }

static double intersectionArea_double__$_$                                      // Area of the intersection between two $
 (const  $ * r,                                                                 // Containing $
  const  $   p)                                                                 // Other $
 {o ◁ r ▶ intersection(p);                                                      // Overlap
  return o ▷ area;
 }

//D1 Fission                                                                    // Split a $ into two smaller $

static  $Pair left_$Pair_$_double                                               // Split the $ vertically the specified distance from the left hand side
 (const $    * r,                                                               // $
  const double d)                                                               // Distance
 {w ◁ r ▶ width; h ◁ r ▶ height;
  if (d <= w)
   {a ◁ make$WH(r->x, r->y, d, h);
    b ◁ make$WH(r->x+d, r->y, w - d, h);
    return new $Pair(source: *r, a: a, b: b, d: d, valid: 1);                   // Valid split
   }
  return   new $Pair(source: *r,             d: d, valid: 0);                   // Invalid split
 }

static  $Pair right_$Pair_$_double                                              // Split the $ vertically the specified distance from the right hand side
 (const $    * r,                                                               // $
  const double d)                                                               // Distance
 {w ◁ r ▶ width; h ◁ r ▶ height;
  if (d <= w)
   {a ◁ make$WH(r->x,     r->y, w - d, h);
    b ◁ make$WH(r->x+w-d, r->y, d,     h);
    return new $Pair(source: *r, a: a, b: b, d: d, valid: 1);                   // Valid split
   }
  return   new $Pair(source: *r,             d: d, valid: 0);                   // Invalid split
 }

static  $Pair down_$Pair_$_double                                               // Split the $ horizontally the specified distance down from the low corner: remember that y increases down the page
 (const $    * r,                                                               // $
  const double d)                                                               // Distance
 {w ◁ r ▶ width; h ◁ r ▶ height;
  if (d <= w)
   {a ◁ make$WH(r->x, r->y,   w, d);
    b ◁ make$WH(r->x, r->y+d, w, h - d);
    return new $Pair(source: *r, a: a, b: b, d: d, valid: 1);                   // Valid split
   }
  return   new $Pair(source: *r,             d: d, valid: 0);                   // Invalid split
 }

static  $Pair up_$Pair_$_double                                                 // Split the $ vertically the specified distance up from the high corner: remember that y increases down the page
 (const $    * r,                                                               // $
  const double d)                                                               // Distance
 {w ◁ r ▶ width; h ◁ r ▶ height;
  if (d <= w)
   {a ◁ make$WH(r->x, r->y,     w, h-d);
    b ◁ make$WH(r->x, r->y+h-d, w, d);
    return new $Pair(source: *r, a: a, b: b, d: d, valid: 1);                   // Valid split
   }
  return   new $Pair(source: *r,             d: d, valid: 0);                   // Invalid split
 }

static int valid_int_$Pair                                                      // Confirm that a split was valid
 (const $Pair * p)                                                              // $Pair
 {return p->valid;
 }

//D1 Read and Write                                                             // Read/write a string buffer from/to named and temporary files.

static char * validAsString_$                                                   // Validity of rectangle as a string
 (const $ * r)                                                                  // $
 {switch(r->t)
   {case type$_valid:      return "valid";
    case type$_point:      return "point";
    case type$_vertical:   return "vertical";
    case type$_horizontal: return "horizontal";
    default:               return "invalid";
   }
 }

static void dump_$                                                              //P Write a $ to stderr->
 (const $ * r)                                                                  // $
 {say("rectangle(x:%f, y:%f, width:%f, height:%f, type: %s)\n",
     r->x, r->y, r ▶ width, r ▶ height, r ▶ validAsString);
 }

#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //Tmake$ //Tequals //Theight  //Twidth //Tclose$ //Tmake$Point //Tcontains //TcontainsPoint //Tpoint //Tvalid
 {p ◁ makePoint$(2, 2); ✓ p ▷ point;
  q ◁ makePoint$(2,22);
  r ◁ make$(1,1,9,9);   ✓ p ▷ valid;
  R ◁ make$(1,1,9,9);

  ✓ !close$(0.9999,     1);
  ✓  close$(0.99999999, 1);
  ✓  close$(r ▷ width,  8);
  ✓  close$(R ▷ height, 8);

  ✓  r ▷ containsPoint( 2,  2);
  ✓ !r ▷ containsPoint( 2, 99);
  ✓ !r ▷ containsPoint(99,  2);

  ✓  r ▷ equals  (R);
  ✓  r ▷ contains(p);
  ✓ !r ▷ contains(q);
  ✓  q ▷ contains(q);
  ✓ !q ▷ contains(p);
 }

void test1()                                                                    //TmakeUnit$ //TmakeZero$ //Tclose //TvalidAsString
 {i ◁ makeUnit$(); ✓ i ▷ valid; ✓ i ▷ equals(make$(0,0,1,1));
  z ◁ makeZero$(); ✓ z ▷ point; ✓ z ▷ equals(make$(0,0,0,0));
  ✓ !i ▷ close(z);
  ✓ !strcmp(i ▷ validAsString, "valid");
 }

void test2()                                                                    //TunionWith //Tintersection
 {a ◁ make$(2,2,6,4);
  b ◁ make$(0,0,9,9);

  i ◁ makeUnit$(); ✓ i ▷ valid; ✓ i ▷ equals(make$(0,0,1,1));
  z ◁ makeZero$(); ✓ z ▷ point; ✓ z ▷ close (make$(0,0,0,0));

  cen ◁ a ▷ center;           ✓ cen ▷ equals(make$(4,3,4,3));

  uab ◁ a ▷ unionWith(b);     ✓ uab ▷ equals(b);
  uba ◁ b ▷ unionWith(a);     ✓ uba ▷ equals(b);

  iab ◁ a ▷ intersection(b);  ✓ iab ▷ valid; iab ▷ equals(a);
  iba ◁ b ▷ intersection(a);  ✓ iba ▷ valid; iba ▷ equals(a);
  iai ◁ a ▷ intersection(i);  ✓!iai ▷ valid;
 }

void test3()                                                                    //Tcenter //Ttranslate //TscaleLow //TscaleHigh //TscaleCenter
 {a ◀ makeUnit$();

  b ◀ a ▷ scaleLow   ( 2,   4);  ✓ b ▷ equals(make$WH( 0,  0, 2, 4));
  c ◀ b ▷ translate  (-1,  -2);  ✓ c ▷ equals(make$WH(-1, -2, 2, 4));
  z ◁ makeZero$();               ✓ z ▷ equals(c ▷ center);
  d ◀ c ▷ scaleHigh  (0.5,0.5);  ✓ d ▷ equals(make$WH( 0,  0, 1, 2));
  e ◁ d ▷ scaleCenter(1,    1);  ✓ e ▷ equals(d);
  f ◁ d ▷ scaleCenter(3,    3);  ✓ f ▷ equals(make$WH(-1, -2, 3, 6));
 }

void test4()                                                                    //Tleft //Tright //Tdown //Tup
 {  a ◁ make$WH (0, 0,  10, 10);
    p ◁ a ▷ left(2);
  ✓ p ▷ valid;
  ✓ p.a ▷ equals(make$WH(0, 0, 2, 10));
  ✓ p.b ▷ equals(make$WH(2, 0, 8, 10));

    q ◁ a ▷ right(8);
  ✓ q ▷ valid;
  ✓ q.a ▷ equals(make$WH(0, 0, 2, 10));
  ✓ q.b ▷ equals(make$WH(2, 0, 8, 10));

    s ◁ a ▷ down(2);
  ✓ s ▷ valid;
  ✓ s.a ▷ equals(make$WH(0, 0, 10, 2));
  ✓ s.b ▷ equals(make$WH(0, 2, 10, 8));

    t ◁ a ▷ up(8);
  ✓ t ▷ valid;
  ✓ t.a ▷ equals(make$WH(0, 0, 10, 2));
  ✓ t.b ▷ equals(make$WH(0, 2, 10, 8));
 }

void test5()                                                                    //TcontainsACorner
 {   a ◁ make$WH (10, 10,  10, 10);
     b ◁ make$WH (15, 15,  10, 10);
  ✓ !a ▷ contains(b);
  ✓  a ▷ containsACorner(b);
 }

void test6()                                                                    //Tarea //TintersectionArea
 {   a ◁ make$WH (10, 10,  10, 10);
     b ◁ make$WH (15, 15,  20, 10);
  ✓  100 == a ▷ area;
  ✓  200 == b ▷ area;
  ✓   25 == a ▷ intersectionArea(b);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4, test5,
                           test6, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/rectangle/rectangle
