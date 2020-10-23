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

typedef struct $Pale                                                            // $ Pale colours
 {const struct ProtoTypes_$Pale *proto;                                         // Prototypes for methods
  const size_t N;
  const $ p[0];
  const $ p1, p2, p3, p4, p5, p6;
 } $Pale;

#include <$$_prototypes.h>

//D1 Colours                                                                    // Make a colour

static $ make$                                                                  // Make a $
 (const double r,                                                               // Red
  const double g,                                                               // Green
  const double b,                                                               // Blue
  const double a)                                                               // Alpha
 {return new $(r: r, g: g, b: b, a: a);
 }

static void dump_$                                                              //P Dump a colour
  ($ colour)                                                                    // Colour
 {say("colour(%d,%d,%d)", (int)(colour.r*10), (int)(colour.g*10), (int)(colour.b*10));
 }

static $Pale make$Pale()                                                        // An array of pale colours
 {o ◁ 1.00; p ◁ 0.60; q ◁ (o + p) / 2;
  p1 ◁ make$(p, o, o, o);
  p2 ◁ make$(o, p, o, o);
  p3 ◁ make$(o, o, p, o);
  p4 ◁ make$(q, q, o, o);
  p5 ◁ make$(o, q, q, o);
  p6 ◁ make$(q, o, q, o);
  return new $Pale(N: 6, p1: p1, p2: p2, p3: p3, p4: p4, p5: p5, p6: p6);
 }

#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //Tmake$ //Tequals //Theight  //Twidth //Tclose$ //Tmake$Point //Tcontains //TcontainsPoint //Tpoint //Tvalid
 {c ◁ make$(0, 1, 0, 1);

  ✓  c.r == 0;

  C ◁ make$(1, 0, 0, 1);
  c ≞ C;
  ✓  c.r == 1;
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, 0};
  run_tests("$", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/rectangle/rectangle
