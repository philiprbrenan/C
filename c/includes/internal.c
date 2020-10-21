#line 1 "/home/phil/c/z/z/functions/internal.c"
//------------------------------------------------------------------------------
// C
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
int main(void)
 {int i = 0;
  typedef void (*fff)(void);
  typedef void ggg(void);
  fff aaa(void)
   {void aaa(void) {const int j = i; printf("i=%d\n", j);}
    return aaa;;
   }
  fff a = aaa();
  ++i;
  fff b = aaa();
  a();
  b();

  ggg a(){}
  return 0;
 }
