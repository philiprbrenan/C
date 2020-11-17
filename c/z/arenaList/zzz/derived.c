#line 1 "zzz/source.c"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
 {  char a[12]; char b[12]; char c[12];
     strcpy(a, "aaaa");
     strcpy(b, "bbbb");
    const typeof( stpcpy(c, a)) A =  stpcpy(c, a); const typeof( stpcpy(A, b)) B =  stpcpy(A, b);
    const typeof( strstr(c, b)) C =  strstr(c, b);

  assert( !strcmp(a, "aaaa"));
  assert( !strcmp(b, "bbbb"));
  assert(  strcmp(b, a));
  assert( !strcmp(c, "aaaabbbb"));
  assert( !strcmp(A, b));
  assert( !strcmp(B, ""));
  assert( !strcmp(C, b));

  printf("success\n");
//◉success
//◉
 }
