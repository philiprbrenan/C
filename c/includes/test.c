#line 1 "/home/phil/c/z/z/cuda/test.c"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

int main(void)
 {int N = 1<<24;
  float *x = malloc(N * sizeof(float)),
        *y = malloc(N * sizeof(float));
  //printf("Number of processors: %d\n", omp_get_num_procs());

  #pragma omp parallel for
  for (int i = 0; i < N; i++)
   {x[i] = 1.0f;
    y[i] = 2.0f;
   }

  #pragma omp parallel for
  for (int i = 0; i < N; ++i)
   {y[i] = x[i] + y[i];
   }

  #pragma omp parallel for
  for (int i = 0; i < N; i++)
   {assert(fabs(y[i]-3.0f) < 1e-6);
   }
  return 0;
 }
