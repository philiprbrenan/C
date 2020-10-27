#include <stdio.h>
int main(void)
 {int   a = 1;
   {int a = a; // Pointless
    printf("AAAA %d\n", a);
   }
  printf("BBBB %d\n", a);
 }
// AAAA 0
// BBBB 1

