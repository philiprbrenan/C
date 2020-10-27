#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
 {typedef struct S
   {int a;
    int b;
   } S;
  S s;
         s.a = 1;
  assert(s.a == 1);
         s.b = 2;
  assert(s.a == 1);
  assert(s.b == 2);
  printf("AAAA %d %d\n", s.a, s.b);
  S t;
  memcpy(&t, ({typeof(t) s2 = s; (void *)&s2;}), sizeof(t));
  assert(t.a == 1);
  assert(t.b == 2);
  printf("BBBB %d %d\n", t.a, t.b);
  printf("CCCC %d\n", memcmp(&t, &s, sizeof(t)));
  assert(! memcmp(({typeof(s) t = s; (void *)&t;}), ({typeof(s) s = t; (void *)&s;}), sizeof(s)));
  memset(&t, 0,                                sizeof(t));
  //assert(!memcmp(({typeof(s) t = s; (void *)&t;}), ({typeof(s) s = t; (void *)&s;}), sizeof(s)));
  printf("success\n");
 }
