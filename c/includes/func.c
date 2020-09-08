#line 1 "/home/phil/c/z/z/typedef/func.c"
//------------------------------------------------------------------------------
// C
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

typedef char * (*fn_type bbb) (char *);

char * aaa(char * a) {return a;}


int main(void)
 {fn_type a = aaa;
  printf("%s\n", a("a"));
  return 0;
 }
