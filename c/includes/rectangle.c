#line 1 "/home/phil/c/z/rectangle/rectangle.c"
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

//D1 Structures                                                                 // Structures describing an Rectangle.

#ifndef Rectangle_included_Rectangle
#define Rectangle_included_Rectangle
typedef enum typeRectangle                                                              // Valid, point, vertical, horizontal, invalid rectangle
 {typeRectangle_valid, typeRectangle_point, typeRectangle_vertical, typeRectangle_horizontal, typeRectangle_invalid
 } typeRectangle;

typedef struct Rectangle                                                                // Rectangle
 {const struct ProtoTypes_Rectangle *proto;                                             // Prototypes for methods
  const double x, y, X, Y;                                                      // Coordinates of two opposing corners with lower x and y first
  const typeRectangle  t;                                                               //Type of rectangle
 } Rectangle;

typedef struct RectanglePair                                                            // A pair of smaller rectangles resulting from fission of a larger rectangle
 {const struct ProtoTypes_RectanglePair *proto;                                         // Prototypes for methods
  const Rectangle source, a, b;                                                         // The source rectangle and the fission pair.
  const double d;                                                               // The splitting distance
  const int valid;                                                              // True if this pair is valid
 } RectanglePair;
#endif

#ifndef Rectangle_included
#define Rectangle_included
#include <rectangle_prototypes.h>

Rectangle makeRectangle                                                                         // Make a Rectangle
 (double x,                                                                     // Low x
  double y,                                                                     // Low y
  double X,                                                                     // High x
  double Y)                                                                     // High y
 {const typeof(closeRectangle(x, X)) cx = closeRectangle(x, X); const typeof(closeRectangle(y, Y)) cy = closeRectangle(y, Y);
  typeRectangle              T = typeRectangle_valid;
  if      (cx && cy) T = typeRectangle_point;
  else if (cx)       T = typeRectangle_horizontal;
  else if (cy)       T = typeRectangle_vertical;

  return newRectangle(({struct Rectangle t = {x: x, y: y, X: X, Y: Y, t: T, proto: &ProtoTypes_Rectangle}; t;}));
 }

Rectangle makeRectangleWH                                                                       // Make a Rectangle with a specified width and height
 (double x,                                                                     // Low x
  double y,                                                                     // Low y
  double w,                                                                     // Width
  double h)                                                                     // Height
 {return makeRectangle(x, y, x + w, y + h);
 }

Rectangle makeCopyRectangle                                                                     // Make a copy of a Rectangle
 (const Rectangle r)                                                                    // Rectangle
 {return newRectangle(({struct Rectangle t = {x: r.x, y: r.y, X: r.X, Y: r.Y, t: r.t, proto: &ProtoTypes_Rectangle}; t;}));
 }

Rectangle makeInvalidRectangle()                                                                // Make an invalid Rectangle
 {return newRectangle(({struct Rectangle t = {t: typeRectangle_invalid, proto: &ProtoTypes_Rectangle}; t;}));
 }

Rectangle makePointRectangle                                                                    // Make a Rectangle point
 (double x,                                                                     // x coordinate of point
  double y)                                                                     // y coordinate of point
 {return makeRectangle(x, y, x, y);
 }

Rectangle makeZeroRectangle()                                                                   // Make a zero Rectangle
 {return makeRectangle(0,0,0,0);
 }

Rectangle makeUnitRectangle()                                                                   // Make a unit Rectangle
 {return makeRectangle(0,0,1,1);
 }

//D1 Dimensions                                                                 // Dimensions of the rectangle

static double width_double_Rectangle                                                    // Width
 (const Rectangle * rectangle)                                                          // Rectangle
 {return fabs(rectangle->X - rectangle->x);
 }

static double height_double_Rectangle                                                   // Height
 (const Rectangle * rectangle)                                                          // Rectangle
 {return fabs(rectangle->Y - rectangle->y);
 }

static Rectangle center_Rectangle_Rectangle                                                             // Return the point rectangle at the center of another rectangle
 (const Rectangle * rectangle)                                                          // Rectangle
 {return makePointRectangle((rectangle->x + rectangle->X) / 2,
                    (rectangle->y + rectangle->Y) / 2);
 }

//D1 Transformations                                                            // Transform Rectangle

static Rectangle translate_Rectangle_Rectangle_double_double                                            // Translate the specified rectangle by the specified amounts
 (Rectangle    *  r,                                                                    // Rectangle
  double  x,                                                                    // x translation
  double  y)                                                                    // y translation
 {return makeRectangle(r->x + x, r->y + y, r->X + x, r->Y + y);
 }

static Rectangle scaleLow_Rectangle_Rectangle_double_double                                             // Scale from the low corner
 (Rectangle    * r,                                                                     // Rectangle
  double x,                                                                     // x translation
  double y)                                                                     // y translation
 {const typeof(r->proto->width(r)) w = r->proto->width(r); const typeof(r->proto->height(r)) h = r->proto->height(r);
  return makeRectangle(r->x, r->y, r->x + w * x, r->y + h * y);
 }

static Rectangle scaleHigh_Rectangle_Rectangle_double_double                                            // Scale from the high corner
 (Rectangle    * r,                                                                     // Rectangle
  double x,                                                                     // x translation
  double y)                                                                     // y translation
 {const typeof(r->proto->width(r)) w = r->proto->width(r); const typeof(r->proto->height(r)) h = r->proto->height(r);
  return makeRectangle(r->X - w * x, r->Y - h * y, r->X, r->Y);
 }

static Rectangle scaleCenter_Rectangle_Rectangle_double_double                                          // Scale from the center
 (Rectangle    * r,                                                                     // Rectangle
  double x,                                                                     // x translation
  double y)                                                                     // y translation
 {const typeof(r->proto->width(r) / 2) w = r->proto->width(r) / 2; const typeof(r->proto->height(r) / 2) h = r->proto->height(r) / 2;
  return makeRectangle(r->x + w * (1 - x), r->y + h * (1 - y),
               r->X + w * (x - 1), r->Y + h * (y - 1));
 }

//D1 Statistics                                                                 // Statistics describing a rectangle

static int closeRectangle                                                               // Equality to one part in a million being sufficient for our purposes
 (const double a,                                                               // First
  const double b)                                                               // Second
 {const typeof(1e-6) d = 1e-6;                                                                     // Accuracy
  return fabs(a - b) < d;
 }

static int equals_int_Rectangle_Rectangle                                                       // Equality to one part in a million being sufficient for our purposes
 (const Rectangle * r,                                                                  // Rectangle first
  const Rectangle   R)                                                                  // Rectangle second
 {const typeof(r->proto->close(r, R)) a = r->proto->close(r, R);
  if (a) return 1;                                                              // Close enough
  say("Rectangles not equal:\n");
  say("have: x:%f, y:%f, width:%f, height:%f valid: %s\n", r->x, r->y, r->proto->width(r), r->proto->height(r), r->proto->validAsString(r));
  say("want: x:%f, y:%f, width:%f, height:%f valid: %s\n", r->x, r->y, R.proto->width(&R), R.proto->height(&R), R.proto->validAsString(&R));
  return 0;                                                                     // Not close enough
 }

static int close_int_Rectangle_Rectangle                                                        // Confirm two rectangles are equal
 (const Rectangle * r,                                                                  // Rectangle first
  const Rectangle   R)                                                                  // Rectangle second
 {return closeRectangle(r->x, R.x) && closeRectangle(r->y, R.y) &&
         closeRectangle(r->X, R.X) && closeRectangle(r->Y, R.Y);
 }

static int containsPoint_int_Rectangle_double_double                                    // Check whether the specified Rectangle contains the specified coordinates
 (const  Rectangle * rectangle,                                                         // Rectangle
  double x,                                                                     // x coordinate of point
  double y)                                                                     // y coordinate of point
 {return x >= rectangle->x && y >= rectangle->y &&
         x <= rectangle->X && y <= rectangle->Y;
 }

static int contains_int_Rectangle_Rectangle                                                     // Check whether the specified Rectangle contains all the corners of another Rectangle
 (const  Rectangle * r,                                                                 // Containing Rectangle
  const  Rectangle   p)                                                                 // Other Rectangle
 {return r->proto->containsPoint(r, p.x, p.y) && r->proto->containsPoint(r, p.X, p.Y);
 }

static int containsACorner_int_Rectangle_Rectangle                                              // Check whether the specified Rectangle contains any of the corners of another Rectangle
 (const  Rectangle * r,                                                                 // Containing Rectangle
  const  Rectangle   p)                                                                 // Other Rectangle
 {return r->proto->containsPoint(r, p.x, p.y) || r->proto->containsPoint(r, p.x, p.Y) ||
         r->proto->containsPoint(r, p.X, p.y) || r->proto->containsPoint(r, p.X, p.Y);
 }

static int valid_int_Rectangle                                                          // Check whether the specified Rectangle is valid.
 (const  Rectangle * r)                                                                 // Rectangle
 {return r->t != typeRectangle_invalid;
 }

static int point_int_Rectangle                                                          // Check whether the specified Rectangle is a point.
 (const  Rectangle * r)                                                                 // Rectangle
 {return r->t == typeRectangle_point;
 }

static double area_double__Rectangle                                                    // Area of a Rectangle - return 0 for an invalid Rectangle
 (const  Rectangle * r)                                                                 // Rectangle
 {if (!r->proto->valid(r)) return 0.0;                                                   // Invalid rectangle has no area
  const typeof(r->proto->width(r)) w = r->proto->width(r);                                                                // Dimensions
  const typeof(r->proto->height(r)) h = r->proto->height(r);
  return w * h;                                                                 // Area
 }

//D1 Union and Intersection                                                     // Form Rectangle from the union and  intersection of other Rectangle

static  Rectangle unionWith_Rectangle_Rectangle_Rectangle                                                       // Form union of two Rectangle: the smallest Rectangle that contains both of them
 (const Rectangle * r,                                                                  // Containing Rectangle
  const Rectangle   p)                                                                  // Other Rectangle
 {return makeRectangle(r->x < p.x ? r->x : p.x, r->y < p.y ? r->y : p.y,
               r->X > p.X ? r->X : p.X, r->Y > p.Y ? r->Y : p.Y);
 }

static Rectangle intersection_Rectangle_Rectangle_Rectangle                                                     // Return a valid Rectangle from the intersection of the specified Rectangle else return an invalid Rectangle
 (const  Rectangle * r,                                                                 // Containing Rectangle
  const  Rectangle   p)                                                                 // Other Rectangle
 {if (p.proto->containsPoint(&p, r->x, r->y) || p.proto->containsPoint(&p, r->X, r->Y) ||         // Check that one rectangle overlaps the other
      r->proto->containsPoint(r, p.x, p.y) || r->proto->containsPoint(r, p.X, p.Y))
   {return makeRectangle(r->x > p.x ? r->x : p.x, r->y > p.y ? r->y : p.y,              // Overlap
                 r->X < p.X ? r->X : p.X, r->Y < p.Y ? r->Y : p.Y);
   }
  else return makeInvalidRectangle();                                                   // No intersection
 }

static double intersectionArea_double__Rectangle_Rectangle                                      // Area of the intersection between two Rectangle
 (const  Rectangle * r,                                                                 // Containing Rectangle
  const  Rectangle   p)                                                                 // Other Rectangle
 {const typeof(r->proto->intersection(r, p)) o = r->proto->intersection(r, p);                                                      // Overlap
  return o.proto->area(&o);
 }

//D1 Fission                                                                    // Split a Rectangle into two smaller Rectangle

static  RectanglePair left_RectanglePair_Rectangle_double                                               // Split the Rectangle vertically the specified distance from the left hand side
 (const Rectangle    * r,                                                               // Rectangle
  const double d)                                                               // Distance
 {const typeof(r->proto->width(r)) w = r->proto->width(r); const typeof(r->proto->height(r)) h = r->proto->height(r);
  if (d <= w)
   {const typeof(makeRectangleWH(r->x,   r->y,     d, h)) a = makeRectangleWH(r->x,   r->y,     d, h);
    const typeof(makeRectangleWH(r->x+d, r->y, w - d, h)) b = makeRectangleWH(r->x+d, r->y, w - d, h);
    return newRectanglePair(({struct RectanglePair t = {source: *r, a: a, b: b, d: d, valid: 1, proto: &ProtoTypes_RectanglePair}; t;}));                   // Valid split
   }
  return   newRectanglePair(({struct RectanglePair t = {source: *r,             d: d, valid: 0, proto: &ProtoTypes_RectanglePair}; t;}));                   // Invalid split
 }

static  RectanglePair right_RectanglePair_Rectangle_double                                              // Split the Rectangle vertically the specified distance from the right hand side
 (const Rectangle    * r,                                                               // Rectangle
  const double d)                                                               // Distance
 {const typeof(r->proto->width(r)) w = r->proto->width(r); const typeof(r->proto->height(r)) h = r->proto->height(r);
  if (d <= w)
   {const typeof(makeRectangleWH(r->x,     r->y, w - d, h)) a = makeRectangleWH(r->x,     r->y, w - d, h);
    const typeof(makeRectangleWH(r->x+w-d, r->y, d,     h)) b = makeRectangleWH(r->x+w-d, r->y, d,     h);
    return newRectanglePair(({struct RectanglePair t = {source: *r, a: a, b: b, d: d, valid: 1, proto: &ProtoTypes_RectanglePair}; t;}));                   // Valid split
   }
  return   newRectanglePair(({struct RectanglePair t = {source: *r,             d: d, valid: 0, proto: &ProtoTypes_RectanglePair}; t;}));                   // Invalid split
 }

static  RectanglePair down_RectanglePair_Rectangle_double                                               // Split the Rectangle horizontally the specified distance down from the low corner: remember that y increases down the page
 (const Rectangle    * r,                                                               // Rectangle
  const double d)                                                               // Distance
 {const typeof(r->proto->width(r)) w = r->proto->width(r); const typeof(r->proto->height(r)) h = r->proto->height(r);
  if (d <= w)
   {const typeof(makeRectangleWH(r->x, r->y,   w, d)) a = makeRectangleWH(r->x, r->y,   w, d);
    const typeof(makeRectangleWH(r->x, r->y+d, w, h - d)) b = makeRectangleWH(r->x, r->y+d, w, h - d);
    return newRectanglePair(({struct RectanglePair t = {source: *r, a: a, b: b, d: d, valid: 1, proto: &ProtoTypes_RectanglePair}; t;}));                   // Valid split
   }
  return   newRectanglePair(({struct RectanglePair t = {source: *r,             d: d, valid: 0, proto: &ProtoTypes_RectanglePair}; t;}));                   // Invalid split
 }

static  RectanglePair up_RectanglePair_Rectangle_double                                                 // Split the Rectangle vertically the specified distance up from the high corner: remember that y increases down the page
 (const Rectangle    * r,                                                               // Rectangle
  const double d)                                                               // Distance
 {const typeof(r->proto->width(r)) w = r->proto->width(r); const typeof(r->proto->height(r)) h = r->proto->height(r);
  if (d <= w)
   {const typeof(makeRectangleWH(r->x, r->y,     w, h - d)) a = makeRectangleWH(r->x, r->y,     w, h - d);
    const typeof(makeRectangleWH(r->x, r->y+h-d, w,     d)) b = makeRectangleWH(r->x, r->y+h-d, w,     d);
    return newRectanglePair(({struct RectanglePair t = {source: *r, a: a, b: b, d: d, valid: 1, proto: &ProtoTypes_RectanglePair}; t;}));                   // Valid split
   }
  return   newRectanglePair(({struct RectanglePair t = {source: *r,             d: d, valid: 0, proto: &ProtoTypes_RectanglePair}; t;}));                   // Invalid split
 }

static int valid_int_RectanglePair                                                      // Confirm that a split was valid
 (const RectanglePair * p)                                                              // RectanglePair
 {return p->valid;
 }

//D1 Read and Write                                                             // Read/write a string buffer from/to named and temporary files.

static char * validAsString_Rectangle                                                   // Validity of rectangle as a string
 (const Rectangle * r)                                                                  // Rectangle
 {switch(r->t)
   {case typeRectangle_valid:      return "valid";
    case typeRectangle_point:      return "point";
    case typeRectangle_vertical:   return "vertical";
    case typeRectangle_horizontal: return "horizontal";
    default:               return "invalid";
   }
 }

static void dump_Rectangle                                                              //P Write a Rectangle to stderr->
 (const Rectangle * r)                                                                  // Rectangle
 {say("rectangle(x:%f, y:%f, width:%f, height:%f, type: %s)\n",
     r->x, r->y, r->proto->width(r), r->proto->height(r), r->proto->validAsString(r));
 }

#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TmakeRectangle //Tequals //Theight  //Twidth //TcloseRectangle //TmakeRectanglePoint //Tcontains //TcontainsPoint //Tpoint //Tvalid
 {const typeof(makePointRectangle(2, 2)) p = makePointRectangle(2, 2); assert( p.proto->point(&p));
  const typeof(makePointRectangle(2,22)) q = makePointRectangle(2,22);
  const typeof(makeRectangle(1,1,9,9)) r = makeRectangle(1,1,9,9);   assert( p.proto->valid(&p));
  const typeof(makeRectangle(1,1,9,9)) R = makeRectangle(1,1,9,9);

  assert( !closeRectangle(0.9999,     1));
  assert(  closeRectangle(0.99999999, 1));
  assert(  closeRectangle(r.proto->width(&r),  8));
  assert(  closeRectangle(R.proto->height(&R), 8));

  assert(  r.proto->containsPoint(&r,  2,  2));
  assert( !r.proto->containsPoint(&r,  2, 99));
  assert( !r.proto->containsPoint(&r, 99,  2));

  assert(  r.proto->equals(&r, R));
  assert(  r.proto->contains(&r, p));
  assert( !r.proto->contains(&r, q));
  assert(  q.proto->contains(&q, q));
  assert( !q.proto->contains(&q, p));
 }

void test1()                                                                    //TmakeUnitRectangle //TmakeZeroRectangle //Tclose //TvalidAsString
 {const typeof(makeUnitRectangle()) i = makeUnitRectangle(); assert( i.proto->valid(&i)); assert( i.proto->equals(&i, makeRectangle(0,0,1,1)));
  const typeof(makeZeroRectangle()) z = makeZeroRectangle(); assert( z.proto->point(&z)); assert( z.proto->equals(&z, makeRectangle(0,0,0,0)));
  assert( !i.proto->close(&i, z));
  assert( !strcmp(i.proto->validAsString(&i), "valid"));
 }

void test2()                                                                    //TunionWith //Tintersection
 {const typeof(makeRectangle(2,2,6,4)) a = makeRectangle(2,2,6,4);
  const typeof(makeRectangle(0,0,9,9)) b = makeRectangle(0,0,9,9);

  const typeof(makeUnitRectangle()) i = makeUnitRectangle(); assert( i.proto->valid(&i)); assert( i.proto->equals(&i, makeRectangle(0,0,1,1)));
  const typeof(makeZeroRectangle()) z = makeZeroRectangle(); assert( z.proto->point(&z)); assert( z.proto->close(&z, makeRectangle(0,0,0,0)));

  const typeof(a.proto->center(&a)) cen = a.proto->center(&a);           assert( cen.proto->equals(&cen, makeRectangle(4,3,4,3)));

  const typeof(a.proto->unionWith(&a, b)) uab = a.proto->unionWith(&a, b);     assert( uab.proto->equals(&uab, b));
  const typeof(b.proto->unionWith(&b, a)) uba = b.proto->unionWith(&b, a);     assert( uba.proto->equals(&uba, b));

  const typeof(a.proto->intersection(&a, b)) iab = a.proto->intersection(&a, b);  assert( iab.proto->valid(&iab)); iab.proto->equals(&iab, a);
  const typeof(b.proto->intersection(&b, a)) iba = b.proto->intersection(&b, a);  assert( iba.proto->valid(&iba)); iba.proto->equals(&iba, a);
  const typeof(a.proto->intersection(&a, i)) iai = a.proto->intersection(&a, i);  assert(!iai.proto->valid(&iai));
 }

void test3()                                                                    //Tcenter //Ttranslate //TscaleLow //TscaleHigh //TscaleCenter
 {typeof(makeUnitRectangle()) a = makeUnitRectangle();

  typeof(a.proto->scaleLow(&a,  2,   4)) b = a.proto->scaleLow(&a,  2,   4);  assert( b.proto->equals(&b, makeRectangleWH( 0,  0, 2, 4)));
  typeof(b.proto->translate(&b, -1,  -2)) c = b.proto->translate(&b, -1,  -2);  assert( c.proto->equals(&c, makeRectangleWH(-1, -2, 2, 4)));
  const typeof(makeZeroRectangle()) z = makeZeroRectangle();               assert( z.proto->equals(&z, c.proto->center(&c)));
  typeof(c.proto->scaleHigh(&c, 0.5,0.5)) d = c.proto->scaleHigh(&c, 0.5,0.5);  assert( d.proto->equals(&d, makeRectangleWH( 0,  0, 1, 2)));
  const typeof(d.proto->scaleCenter(&d, 1,    1)) e = d.proto->scaleCenter(&d, 1,    1);  assert( e.proto->equals(&e, d));
  const typeof(d.proto->scaleCenter(&d, 3,    3)) f = d.proto->scaleCenter(&d, 3,    3);  assert( f.proto->equals(&f, makeRectangleWH(-1, -2, 3, 6)));
 }

void test4()                                                                    //Tleft //Tright //Tdown //Tup
 {  const typeof(makeRectangleWH (0, 0,  10, 10)) a = makeRectangleWH (0, 0,  10, 10);
    const typeof(a.proto->left(&a, 2)) p = a.proto->left(&a, 2);
  assert( p.proto->valid(&p));
  assert( p.a.proto->equals(&p.a, makeRectangleWH(0, 0, 2, 10)));
  assert( p.b.proto->equals(&p.b, makeRectangleWH(2, 0, 8, 10)));

    const typeof(a.proto->right(&a, 8)) q = a.proto->right(&a, 8);
  assert( q.proto->valid(&q));
  assert( q.a.proto->equals(&q.a, makeRectangleWH(0, 0, 2, 10)));
  assert( q.b.proto->equals(&q.b, makeRectangleWH(2, 0, 8, 10)));

    const typeof(a.proto->down(&a, 2)) s = a.proto->down(&a, 2);
  assert( s.proto->valid(&s));
  assert( s.a.proto->equals(&s.a, makeRectangleWH(0, 0, 10, 2)));
  assert( s.b.proto->equals(&s.b, makeRectangleWH(0, 2, 10, 8)));

    const typeof(a.proto->up(&a, 8)) t = a.proto->up(&a, 8);
  assert( t.proto->valid(&t));
  assert( t.a.proto->equals(&t.a, makeRectangleWH(0, 0, 10, 2)));
  assert( t.b.proto->equals(&t.b, makeRectangleWH(0, 2, 10, 8)));
 }

void test5()                                                                    //TcontainsACorner
 {   const typeof(makeRectangleWH (10, 10,  10, 10)) a = makeRectangleWH (10, 10,  10, 10);
     const typeof(makeRectangleWH (15, 15,  10, 10)) b = makeRectangleWH (15, 15,  10, 10);
  assert( !a.proto->contains(&a, b));
  assert(  a.proto->containsACorner(&a, b));
 }

void test6()                                                                    //Tarea //TintersectionArea
 {   const typeof(makeRectangleWH (10, 10,  10, 10)) a = makeRectangleWH (10, 10,  10, 10);
     const typeof(makeRectangleWH (15, 15,  20, 10)) b = makeRectangleWH (15, 15,  20, 10);
  assert(  100 == a.proto->area(&a));
  assert(  200 == b.proto->area(&b));
  assert(   25 == a.proto->intersectionArea(&a, b));
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, test1, test2, test3, test4, test5,
                           test6, 0};
  run_tests("Rectangle", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/rectangle/rectangle
