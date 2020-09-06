//------------------------------------------------------------------------------
// C
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

typedef float aaa(double d) AAA;

float aaa(double d)
 {return (float)d;
 }

typeof(aaa) bbb
 {return (float)d;
 }

int main(int argc, char **argv)
 {return 0;
 }
