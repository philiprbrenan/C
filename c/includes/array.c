//------------------------------------------------------------------------------
// Return a variable size array
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
int main(void)
 {typedef struct S
   {char c[4];
   } S;

  S a = {c: "aaa"};
  printf("AAAA %s\n", a.c);

  char * bb = "bbbb";
  S b = {c: {bb}};
  printf("BBBB %s\n", b.c);

  return 0;
 }
