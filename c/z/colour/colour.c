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

//D1 Structures                                                                 // Structures describing an $.

#ifndef $_included_$
#define $_included_$
typedef struct $                                                                // $
 {const struct ProtoTypes_$ *proto;                                             // Prototypes for methods
  const double r;                                                               // Red
  const double g;                                                               // Green
  const double b;                                                               // Blue
  const double a;                                                               // Alpha
 } $;

#include <$$_prototypes.h>

//D1 Colours                                                                    // Make a colour

static $ make$                                                                  // Make a $
  (const double r,                                                              // Red
   const double g,                                                              // Green
   const double b,                                                              // Blue
   const double a)                                                              // Alpha
 {return new $(r: r, g: g, b: b, a: a);
 }

#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //Tmake$ //Tequals //Theight  //Twidth //Tclose$ //Tmake$Point //Tcontains //TcontainsPoint //Tpoint //Tvalid
 {c ◁ make$(0, 1, 0, 1);

  ✓  c.r == 0;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/rectangle/rectangle
