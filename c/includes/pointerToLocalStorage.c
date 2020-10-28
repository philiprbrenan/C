#line 1 "/home/phil/c/z/z/inline/pointerToLocalStorage.c"
#include <stdio.h>
int * aaa() {return ({int a = 2;        &a;});} // no warning
int * bbb() {         int b = 3; return &b;}    //    warning

int main(void)
 {int *a = aaa(), *b = bbb();
  fprintf(stderr, "AAAA %d\n", *a);
  fprintf(stderr, "BBBB %d\n", *b);
  return 0;
 }
// AAAA 3
