#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Incluimos la biblioteca OpenMP si está disponible
#ifdef _OPENMP
#include <omp.h>
#endif

#define NUM_STEPS 1000000000

// Función para calcular Pi sin usar OpenMP
double calculate_pi(int num_steps) {
    double pi = 0.0;
    double step = 1.0 / (double) num_steps;
    
    for (int i = 0; i < num_steps; i++) {
        double x = (i + 0.5) * step;
        pi += 4.0 / (1.0 + x * x);
    }
    
    return pi * step;
}

// Función para calcular Pi usando OpenMP
double calculate_pi_OpenMP(int num_steps) {
    double pi = 0.0;
    double step = 1.0 / (double) num_steps;
    
    // Inicio de la región paralela
    #pragma omp parallel
    {
        // Cada hilo tendrá su propia suma parcial
        double sum = 0.0;
        
        // Distribuye las iteraciones del bucle entre los hilos
        #pragma omp for
        for (int i = 0; i < num_steps; i++) {
            double x = (i + 0.5) * step;
            sum += 4.0 / (1.0 + x * x);
        }
        
        // Suma atómica para evitar condiciones de carrera
        #pragma omp atomic
        pi += sum;
    }
    
    return pi * step;
}

int main() {
    clock_t start, end;
    double cpu_time_used;

    // Versión sin OpenMP
    start = clock();
    double pi_serial = calculate_pi(NUM_STEPS);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Pi (serial): %.10f\n", pi_serial);
    printf("Tiempo (serial): %f segundos\n", cpu_time_used);

    // Versión con OpenMP
    start = clock();
    double pi_parallel = calculate_pi_OpenMP(NUM_STEPS);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Pi (paralelo): %.10f\n", pi_parallel);
    printf("Tiempo (paralelo): %f segundos\n", cpu_time_used);

    return 0;
}

/* 
Explicación del uso de OpenMP y las bibliotecas:

1. Inclusión de bibliotecas:
   - <stdio.h>: Para funciones de entrada/salida estándar.
   - <stdlib.h>: Para funciones de propósito general.
   - <time.h>: Para medir el tiempo de ejecución.
   - <omp.h>: Biblioteca de OpenMP (incluida condicionalmente).

2. Uso de OpenMP:
   - #pragma omp parallel: Crea un equipo de hilos para ejecutar el bloque siguiente.
   - #pragma omp for: Distribuye las iteraciones del bucle entre los hilos disponibles.
   - #pragma omp atomic: Asegura que la actualización de 'pi' sea atómica, evitando condiciones de carrera.

3. Compilación:
   Para compilar este código con soporte para OpenMP, use:
   gcc -fopenmp nombre_archivo.c -o nombre_ejecutable

   Si no desea usar OpenMP, simplemente omita la bandera -fopenmp:
   gcc nombre_archivo.c -o nombre_ejecutable

4. Ejecución:
   ./nombre_ejecutable

5. Control de hilos:
   Puede controlar el número de hilos utilizados por OpenMP mediante la variable de entorno OMP_NUM_THREADS:
   export OMP_NUM_THREADS=4 (para usar 4 hilos)

6. Rendimiento:
   - La versión con OpenMP debería ser más rápida en sistemas multi-núcleo.
   - Para problemas pequeños, la sobrecarga de crear hilos puede superar los beneficios.
   - El speedup dependerá del número de núcleos disponibles y la carga de trabajo.

7. Precisión:
   - Ambas versiones deberían producir resultados idénticos o muy similares.
   - Pequeñas diferencias pueden ocurrir debido al orden de las operaciones en punto flotante.
*/
