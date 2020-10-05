#line 1 "/home/phil/c/z/colour/colour.c"
//------------------------------------------------------------------------------
// Colour
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
/*
 Operations on colours
*/
#define _GNU_SOURCE
#include <math.h>
#include <utilities.c>

//D1 Structures                                                                 // Structures describing an Colour.

#ifndef Colour_included_Colour
#define Colour_included_Colour
typedef struct Colour                                                                // Colour
 {const struct ProtoTypes_Colour *proto;                                             // Prototypes for methods
  const double r;                                                               // Red
  const double g;                                                               // Green
  const double b;                                                               // Blue
  const double a;                                                               // Alpha
 } Colour;

#include <colour_prototypes.h>

//D1 Colours                                                                    // Make a colour

static Colour makeColour                                                                  // Make a Colour
  (const double r,                                                              // Red
   const double g,                                                              // Green
   const double b,                                                              // Blue
   const double a)                                                              // Alpha
 {return newColour(({struct Colour t = {r: r, g: g, b: b, a: a, proto: &ProtoTypes_Colour}; t;}));
 }

#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TmakeColour //Tequals //Theight  //Twidth //TcloseColour //TmakeColourPoint //Tcontains //TcontainsPoint //Tpoint //Tvalid
 {const typeof(makeColour(0, 1, 0, 1)) c = makeColour(0, 1, 0, 1);

  assert(  c.r == 0);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, 0};
  run_tests("Colour", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/rectangle/rectangle
