#include <stdio.h>
#include "bench.h"

VALT suma_est_fil(VALT A[N][N]);
VALT suma_est_col(VALT A[N][N]);

VALT suma_fil(VALT* A, size_t n);
VALT suma_col(VALT* A, size_t n);
VALT suma_rand(VALT* A, size_t n);

int mult_simple(const VALT * __restrict__ A, const VALT * __restrict__ B, VALT * __restrict__ C, size_t n);
int mult_fila(const VALT * __restrict__ A, const VALT * __restrict__ B, VALT * __restrict__ C, size_t n);
int mult_bl_simple(const VALT * __restrict__ A, const VALT * __restrict__ B, VALT * __restrict__ C, size_t n, size_t bl_sz);
int mult_bl_fila  (const VALT * __restrict__ A, const VALT * __restrict__ B, VALT * __restrict__ C, size_t n, size_t bl_sz);

void random_vector(VALT* a, size_t n) {
   for (unsigned int i = 0; i < n; i++) {
      a[i] = (float)rand() / (float)RAND_MAX;
   }
}

unsigned int* INDEX;

int main(int argc, char* argv[]) {

   // const char * fname;

   if (argc < 2) {
      printf("El programa recibe n, y nb: n es la dimensión de las matrices y nb es el tamaño de bloque\n");
      exit(1);
   }

   int n = atoi(argv[1]);
   int nb = atoi(argv[2]);

   srand(0);

   VALT* A = (VALT*) aligned_alloc(64, n * n * sizeof(VALT));
   VALT* B = (VALT*) aligned_alloc(64, n * n * sizeof(VALT));
   VALT* C = (VALT*) aligned_alloc(64, n * n * sizeof(VALT));

   // Precalculate random access
   INDEX = (unsigned int*) aligned_alloc(64, n * n * sizeof(unsigned int));
   for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
         INDEX[i * n + j] = rand() % n * n + rand() % n;
      }
   }

   random_vector(A, n * n);
   random_vector(B, n * n);

   struct timeb t_ini;
   struct timeb t_fin;

   VALT A_est[N][N];

   for (int i = 0; i < N; ++i)
      for (int j = 0; j < N; ++j)
         A_est[i][j] = (VALT) rand();

   BENCH_RUN(suma_est_fil(A_est), t_suma_est_fil, t_suma_est_fil_runs)
   BENCH_RUN(suma_est_col(A_est), t_suma_est_col, t_suma_est_col_runs)
   BENCH_RUN(suma_fil(A,n), t_suma_fil, t_suma_fil_runs)
   BENCH_RUN(suma_col(A,n), t_suma_col, t_suma_col_runs)
   BENCH_RUN(suma_rand(A,n), t_suma_rand, t_suma_rand_runs)

   printf("suma_est_fil: %.8f ms\truns:%d\n", t_suma_est_fil, t_suma_est_fil_runs );
   printf("suma_est_col: %.8f ms\truns:%d\n", t_suma_est_col, t_suma_est_col_runs );
   printf("suma_fila: %.8f ms\truns:%d\n", t_suma_fil, t_suma_fil_runs);
   printf("suma_col: %.8f ms\truns:%d\n", t_suma_col, t_suma_col_runs);
   printf("suma_rand: %.8f ms\truns:%d\n", t_suma_rand, t_suma_rand_runs);

   BENCH_RUN(mult_simple(A, B, C, n), t_mm_simple, t_mm_simple_runs)
   BENCH_RUN(mult_fila(A, B, C, n), t_mm_fila, t_mm_fila_runs)
   BENCH_RUN(mult_bl_simple(A, B, C, n, nb), t_mm_bl_simple, t_mm_bl_simple_runs)
   BENCH_RUN(mult_bl_fila(A, B, C, n, nb), t_mm_bl_fila, t_mm_bl_fila_runs)

   printf("mult_simple: %.2f ms, %.2f GFlops runs: %d\n", t_mm_simple, (((double)n / t_mm_simple ) * ((double)n / 1000.0)*((double)n / 1000.0)), t_mm_simple_runs);
   printf("mult_fila: %.2f ms, %.2f GFlops runs: %d\n", t_mm_fila, (((double)n / t_mm_fila) * ((double)n / 1000.0)*((double)n / 1000.0)), t_mm_fila_runs);
   printf("mult_bl_simple: %.2f ms, %.2f GFlops runs: %d\n", t_mm_bl_simple, (((double)n / t_mm_bl_simple)*((double)n / 1000.0) * ((double)n / 1000.0)), t_mm_bl_simple_runs);
   printf("mult_bl_fila: %.2f ms, %.2f GFlops runs: %d\n", t_mm_bl_fila, (((double)n / t_mm_bl_fila) * ((double)n / 1000.0) * ((double)n / 1000.0)), t_mm_bl_fila_runs );

   return 0;
}

VALT suma_est_fil(VALT A[N][N]) {
   VALT sum = 0;

   for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++)
         sum += A[i][j];
   }

   return sum;
}

VALT suma_est_col(VALT A[N][N]) {
   VALT sum = 0;

   for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++)
         sum += A[j][i];
   }

   return sum;
}

VALT suma_fil(VALT *A, size_t n) {
   VALT sum = 0;

   for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++)
         sum += A[i * n + j];
   }

   return sum;
}

VALT suma_col(VALT *A, size_t n) {
   VALT sum = 0;

   for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++)
         sum += A[j * n + i];
   }

   return sum;
}

VALT suma_rand(VALT *A, size_t n) {
   VALT sum = 0;

   for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++)
         sum += A[INDEX[i * n + j]];
   }

   return sum;
}

int mult_simple(const VALT * __restrict__ A, const VALT * __restrict__ B, VALT * __restrict__ C, size_t n) {
   for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
         for (int k = 0; k < n; k++) {
            double temp = A[i * n + k] * B[k * n + j];
            if (k != 0) {
               temp += C[i * n + j];
            }
            C[i * n + j] = temp;
         }
      }
   }

   return 0;
}

int mult_fila(const VALT * __restrict__ A, const VALT * __restrict__ B, VALT * __restrict__ C, size_t n) {
   for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
         for (int k = 0; k < n; k++) {
            double temp = A[i * n + j] * B[j * n + k];
            if (j != 0) {
               temp += C[i * n + k];
            }
            C[i * n + k] = temp;
         }
      }
   }

   return 0;
}

int mult_bl_simple(const VALT * __restrict__ A, const VALT * __restrict__ B, VALT * __restrict__ C, size_t n, size_t bl_sz) {
   for (int bi = 0; bi < n; bi += bl_sz) {
      for (int bj = 0; bj < n; bj += bl_sz) {
         for (int i = bi; i < bi + bl_sz && i < n; ++i) {
            for (int j = bj; j < bj + bl_sz && j < n; ++j) {
               for (int k = 0; k < n; ++k) {
                  double temp = A[i * n + k] * B[k * n + j];
                  if (k != 0) {
                     temp += C[i * n + j];
                  }
                  C[i * n + j] = temp;
               }
            }
         }
      }
   }

   return 0;
}

int mult_bl_fila(const VALT * __restrict__ A, const VALT * __restrict__ B, VALT * __restrict__ C, size_t n, size_t bl_sz) {
   for (int bi = 0; bi < n; bi += bl_sz) {
      for (int bj = 0; bj < n; bj += bl_sz) {
         for (int i = bi; i < bi + bl_sz && i < n; ++i) {
            for (int j = bj; j < bj + bl_sz && j < n; ++j) {
               for (int k = 0; k < n; ++k) {
                  double temp = A[i * n + j] * B[j * n + k];
                  if (j != 0) {
                     temp += C[i * n + k];
                  }
                  C[i * n + k] = temp;
               }
            }
         }
      }
   }

   return 0;
}
