#line 1 "/home/phil/c/z/z/openMP/testMP.c"
#include <stdio.h>
#include <utilities.c>
#include <omp.h>

int main(void)
 {int n = 1000, N = 1e6;
  omp_set_num_threads(n);
  double a[n];

//execSystemCommand("nvidia-smi");
//execSystemCommand("nvprof ./test");
//#pragma omp target
  printf("Number of processors: %d\n", omp_get_num_procs());
  printf("Number of devices:    %d\n", omp_get_num_devices());

  #pragma omp parallel
   {int i = omp_get_thread_num();
    a[i] = 0;
   }

  #pragma omp parallel for
  for  (int i = 0; i < n; ++i)
   {a[i] = i;
    for(int j = 1; j < N; ++j)
     {a[i] += 1.0 / (double)j;
     }
   }
  for(int i = 0; i < n; ++i) printf("%f ", a[i]); printf("\n");
  return 0;
 }
