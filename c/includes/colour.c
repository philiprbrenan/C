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
 (const double r,                                                               // Red
  const double g,                                                               // Green
  const double b,                                                               // Blue
  const double a)                                                               // Alpha
 {return newColour(({struct Colour t = {r: r, g: g, b: b, a: a, proto: &ProtoTypes_Colour}; t;}));
 }

static void dump_Colour                                                              //P Dump a colour
  (Colour colour)                                                                    // Colour
 {say("colour(%d,%d,%d)", (int)(colour.r*10), (int)(colour.g*10), (int)(colour.b*10));
 }

static ColourPale makeColourPale()                                                        // An array of pale colours
 {const typeof(1.00) o = 1.00; const typeof(0.60) p = 0.60; const typeof((o + p) / 2) q = (o + p) / 2;
  const typeof(makeColour(p, o, o, o)) p1 = makeColour(p, o, o, o);
  const typeof(makeColour(o, p, o, o)) p2 = makeColour(o, p, o, o);
  const typeof(makeColour(o, o, p, o)) p3 = makeColour(o, o, p, o);
  const typeof(makeColour(q, q, o, o)) p4 = makeColour(q, q, o, o);
  const typeof(makeColour(o, q, q, o)) p5 = makeColour(o, q, q, o);
  const typeof(makeColour(q, o, q, o)) p6 = makeColour(q, o, q, o);
  return newColourPale(({struct ColourPale t = {N: 6, p1: p1, p2: p2, p3: p3, p4: p4, p5: p5, p6: p6, proto: &ProtoTypes_ColourPale}; t;}));
 }

#endif

//D1 Tests                                                                      // Tests
#if __INCLUDE_LEVEL__ == 0

void test0()                                                                    //TmakeColour //Tequals //Theight  //Twidth //TcloseColour //TmakeColourPoint //Tcontains //TcontainsPoint //Tpoint //Tvalid
 {const typeof(makeColour(0, 1, 0, 1)) c = makeColour(0, 1, 0, 1);

  assert(  c.r == 0);

  const typeof(makeColour(1, 0, 0, 1)) C = makeColour(1, 0, 0, 1);
  ({typeof(c) sourcesourcesource = C; memcpy((void *)&c, (void *)&sourcesourcesource, sizeof(c));});
  assert(  c.r == 1);
 }

int main(void)                                                                  // Run tests
 {void (*tests[])(void) = {test0, 0};
  run_tests("Colour", 1, tests);
  return 0;
 }
#endif
// valgrind --leak-check=full --leak-resolution=high --show-leak-kinds=definite  --track-origins=yes /home/phil/c/z/rectangle/rectangle
