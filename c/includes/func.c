#line 1 "/home/phil/c/z/z/typedef/func.c"
//------------------------------------------------------------------------------
// C
// Philip R Brenan at gmail dot com, Appa Apps Ltd. Inc., 2020
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

typedef char (*fn_type) (void);

char aaa(fn_type a) {return a();}


int main(void)
 {char bbb(void) {return 'b';}
  fn_type ccc {return 'c';}
  printf("AAAA %c\n", aaa(bbb));
  return 0;
 }
