#line 1 "/home/phil/c/z/z/openMP/testACC.c"
#include <stdio.h>

int main(void)
 {int n = 100, N = 1e7;
  double a[n];

  #pragma acc parallel loop copyout(a[0:n])
  for(int i = 0; i < n; ++i)
   {a[i] = i;
    for(int j = 1; j < N; ++j)
     {a[i] += 1.0 / (double)j;
     }
   }
  for(int i = 0; i < n; ++i) printf("%f ", a[i]); printf("\n");
  return 0;
 }
