#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define N 1000 // Tamaño de las matrices

void matrix_multiply_serial(double A[N][N], double B[N][N], double C[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void matrix_multiply_parallel(double A[N][N], double B[N][N], double C[N][N]) {
    // Directiva de OpenMP para paralelizar el bucle externo
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            C[i][j] = 0;
            for (int k = 0; k < N; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    double (*A)[N] = malloc(sizeof(double[N][N]));
    double (*B)[N] = malloc(sizeof(double[N][N]));
    double (*C_serial)[N] = malloc(sizeof(double[N][N]));
    double (*C_parallel)[N] = malloc(sizeof(double[N][N]));

    clock_t start, end;
    double cpu_time_used;

    // Inicialización de matrices
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = (double)rand() / RAND_MAX;
            B[i][j] = (double)rand() / RAND_MAX;
        }
    }

    // Multiplicación de matrices serial
    start = clock();
    matrix_multiply_serial(A, B, C_serial);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecución (serial): %f segundos\n", cpu_time_used);

    // Multiplicación de matrices paralela
    start = clock();
    matrix_multiply_parallel(A, B, C_parallel);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Tiempo de ejecución (paralelo): %f segundos\n", cpu_time_used);

    // Verificación de resultados
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (C_serial[i][j] != C_parallel[i][j]) {
                printf("Error: los resultados no coinciden en [%d][%d]\n", i, j);
                goto cleanup;
            }
        }
    }

    printf("Los resultados coinciden.\n");

cleanup:
    // Liberación de memoria
    free(A);
    free(B);
    free(C_serial);
    free(C_parallel);

    return 0;
}