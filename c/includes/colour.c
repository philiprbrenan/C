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

typedef struct ColourPale                                                            // Colour Pale colours
 {const struct ProtoTypes_ColourPale *proto;                                         // Prototypes for methods
  const size_t N;
  const Colour p[0];
  const Colour p1, p2, p3, p4, p5, p6;
 } ColourPale;

#include <colour_prototypes.h>

//D1 Colours                                                                    // Make a colour

static Colour makeColour                                                                  // Make a Colour
  (const double r,                                                              // Red
   const double g,                                                              // Green
   const double b,                                                              // Blue
   const double a)                                                              // Alpha
 {return newColour(({struct Colour t = {r: r, g: g, b: b, a: a, proto: &ProtoTypes_Colour}; t;}));
 }

static ColourPale makeColourPale()                                                        // An array of pale colours
 {const typeof(1.00) o = 1.00; const typeof(0.90) 𝗽 = 0.90; const typeof((o + 𝗽) / 2) 𝗾 = (o + 𝗽) / 2;
  const typeof(makeColour(𝗽, o, o, o)) p1 = makeColour(𝗽, o, o, o);
  const typeof(makeColour(o, 𝗽, o, o)) p2 = makeColour(o, 𝗽, o, o);
  const typeof(makeColour(o, o, 𝗽, o)) p3 = makeColour(o, o, 𝗽, o);
  const typeof(makeColour(𝗾, 𝗾, o, o)) p4 = makeColour(𝗾, 𝗾, o, o);
  const typeof(makeColour(o, 𝗾, 𝗾, o)) p5 = makeColour(o, 𝗾, 𝗾, o);
  const typeof(makeColour(𝗾, o, 𝗾, o)) p6 = makeColour(𝗾, o, 𝗾, o);
  return newColourPale(({struct ColourPale t = {N: 6, p1: p1, p2: p2, p3: p3, p4: p4, p5: p5, p6: p6, proto: &ProtoTypes_ColourPale}; t;}));
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
