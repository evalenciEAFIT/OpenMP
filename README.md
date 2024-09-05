# Guía de OpenMP: Ejemplos Avanzados en C++

## Tabla de Contenidos
1. [Introducción a OpenMP](#introducción-a-openmp)
2. [Directivas Principales de OpenMP](#directivas-principales-de-openmp)
3. [Funcionamiento Interno de OpenMP en C++](#funcionamiento-interno-de-openmp-en-c)
4. [Estructura del Repositorio](#estructura-del-repositorio)
5. [Requisitos](#requisitos)
6. [Compilación y Ejecución](#compilación-y-ejecución)
7. [Ejemplos](#ejemplos)
   - [Suma de Vectores](#1-suma-de-vectores)
   - [Cálculo de Pi](#2-cálculo-de-pi)
   - [Multiplicación de Matrices](#3-multiplicación-de-matrices)
   - [Búsqueda Paralela](#4-búsqueda-paralela)
   - [Ordenamiento Paralelo](#5-ordenamiento-paralelo)
   - [Procesamiento de Imágenes](#6-procesamiento-de-imágenes)
   - [Simulación de Monte Carlo](#7-simulación-de-monte-carlo)
8. [Consejos para el Uso Efectivo de OpenMP](#consejos-para-el-uso-efectivo-de-openmp)
9. [Recursos Adicionales](#recursos-adicionales)
10. [Conclusión](#conclusión)
11. [Licencia](#licencia)

## Introducción a OpenMP

OpenMP (Open Multi-Processing) es una API que soporta programación multiproceso de memoria compartida en múltiples plataformas. Permite añadir concurrencia a los programas escritos en C, C++ y Fortran de manera incremental, lo que facilita la paralelización de código existente.

Características principales de OpenMP:
- Modelo de memoria compartida
- Paralelismo incremental
- Portabilidad entre diferentes arquitecturas y compiladores
- Escalabilidad para aprovechar múltiples núcleos y procesadores

OpenMP utiliza el modelo de ejecución fork-join, donde un hilo principal (master thread) crea un equipo de hilos paralelos cuando encuentra una región paralela en el código.

## Directivas Principales de OpenMP

OpenMP se basa en directivas de compilador para especificar comportamiento paralelo. Las directivas principales son:

1. `#pragma omp parallel`: Crea un equipo de hilos.
2. `#pragma omp for`: Distribuye iteraciones de un bucle entre los hilos existentes.
3. `#pragma omp parallel for`: Combina la creación de hilos y la distribución de bucles.
4. `#pragma omp critical`: Define una sección crítica para acceso exclusivo.
5. `#pragma omp atomic`: Asegura operaciones atómicas en una sola variable.
6. `#pragma omp barrier`: Sincroniza todos los hilos en un punto.
7. `#pragma omp single`: Especifica que un bloque sea ejecutado por un solo hilo.
8. `#pragma omp task`: Define una unidad de trabajo que puede ser ejecutada asincrónicamente.

## Funcionamiento Interno de OpenMP en C++

Cuando se utiliza OpenMP, el compilador transforma las directivas en código C++ que utiliza las funciones de la biblioteca de tiempo de ejecución de OpenMP. Aquí se explica cómo algunas de las directivas principales se traducen internamente:

### 1. `#pragma omp parallel`

```cpp
// Código original con OpenMP
#pragma omp parallel
{
    // Código paralelo
}

// Transformación aproximada
void subrutina_paralela(void *tid) {
    // Código paralelo
}

int num_hilos = omp_get_num_threads();
omp_set_num_threads(num_hilos);
#pragma omp parallel
{
    int tid = omp_get_thread_num();
    subrutina_paralela(&tid);
}
```

### 2. `#pragma omp for`

```cpp
// Código original con OpenMP
#pragma omp for
for (int i = 0; i < n; i++) {
    // trabajo
}

// Transformación aproximada
{
    int chunk = n / omp_get_num_threads();
    int start = omp_get_thread_num() * chunk;
    int end = (omp_get_thread_num() == omp_get_num_threads() - 1) ? n : start + chunk;
    for (int i = start; i < end; i++) {
        // trabajo
    }
}
```

### 3. `#pragma omp critical`

```cpp
// Código original con OpenMP
#pragma omp critical
{
    // sección crítica
}

// Transformación aproximada
{
    omp_set_lock(&lock);
    {
        // sección crítica
    }
    omp_unset_lock(&lock);
}
```

### 4. `#pragma omp atomic`

```cpp
// Código original con OpenMP
#pragma omp atomic
x += 1;

// Transformación aproximada (usando instrucciones atómicas del hardware)
__atomic_fetch_add(&x, 1, __ATOMIC_SEQ_CST);
```

### 5. `#pragma omp task`

```cpp
// Código original con OpenMP
#pragma omp task
{
    // código de la tarea
}

// Transformación aproximada
{
    omp_task_t *nueva_tarea = omp_create_task(/* parámetros */);
    omp_set_task_data(nueva_tarea, /* datos */);
    omp_enqueue_task(nueva_tarea);
}
```

## Estructura del Repositorio

```
.
├── README.md
├── src/
│   ├── suma_vectores.cpp
│   ├── calculo_pi.cpp
│   ├── multiplicacion_matrices.cpp
│   ├── busqueda_paralela.cpp
│   ├── ordenamiento_paralelo.cpp
│   ├── procesamiento_imagenes.cpp
│   └── simulacion_montecarlo.cpp
└── LICENSE
```

## Requisitos

- Compilador C++ que soporte C++11 o superior
- OpenMP

## Compilación y Ejecución

Para compilar los ejemplos, use los siguientes comandos:

```bash
g++ -fopenmp -o suma_vectores src/suma_vectores.cpp
g++ -fopenmp -o calculo_pi src/calculo_pi.cpp
g++ -fopenmp -o multiplicacion_matrices src/multiplicacion_matrices.cpp
g++ -fopenmp -o busqueda_paralela src/busqueda_paralela.cpp
g++ -fopenmp -o ordenamiento_paralelo src/ordenamiento_paralelo.cpp
g++ -fopenmp -o procesamiento_imagenes src/procesamiento_imagenes.cpp
g++ -fopenmp -o simulacion_montecarlo src/simulacion_montecarlo.cpp
```

Para ejecutar los ejemplos:

```bash
./suma_vectores
./calculo_pi
./multiplicacion_matrices
./busqueda_paralela
./ordenamiento_paralelo
./procesamiento_imagenes
./simulacion_montecarlo
```

## Ejemplos

### 1. Suma de Vectores

**Archivo:** `src/suma_vectores.cpp`

**Descripción:** Este ejemplo demuestra cómo sumar dos vectores en paralelo.

**Directiva utilizada:** `#pragma omp parallel for`

**Justificación:** Se utiliza `parallel for` porque la suma de cada elemento es independiente de los demás, permitiendo una paralelización directa y eficiente del bucle.

**Pseudocódigo:**
```
función sumar_vectores(A, B, C, N)
    paralelo para i = 0 hasta N-1
        C[i] = A[i] + B[i]
    fin paralelo para
fin función
```

**Código clave:**
```cpp
#pragma omp parallel for
for (size_t i = 0; i < tamano; ++i) {
    resultado[i] = vectorA[i] + vectorB[i];
}
```

### 2. Cálculo de Pi

**Archivo:** `src/calculo_pi.cpp`

**Descripción:** Calcula π usando el método de integración numérica.

**Directivas utilizadas:** 
- `#pragma omp parallel`
- `#pragma omp for reduction(+:suma)`

**Justificación:** Se usa `parallel` para crear un equipo de hilos y `for reduction` para distribuir las iteraciones y combinar los resultados parciales de forma segura.

**Pseudocódigo:**
```
función calcular_pi(num_pasos)
    paso = 1.0 / num_pasos
    suma = 0.0
    
    paralelo
        suma_local = 0.0
        para i = id_hilo hasta num_pasos-1 con paso = num_hilos
            x = (i + 0.5) * paso
            suma_local += 4.0 / (1.0 + x*x)
        fin para
        
        suma += suma_local (de forma atómica)
    fin paralelo
    
    retornar paso * suma
fin función
```

**Código clave:**
```cpp
#pragma omp parallel
{
    #pragma omp for reduction(+:suma)
    for (long long i = 0; i < numPasos; i++) {
        double x = (i + 0.5) * paso;
        suma += 4.0 / (1.0 + x * x);
    }
}
```

### 3. Multiplicación de Matrices

**Archivo:** `src/multiplicacion_matrices.cpp`

**Descripción:** Multiplica dos matrices en paralelo.

**Directiva utilizada:** `#pragma omp parallel for collapse(2)`

**Justificación:** Se utiliza `parallel for` con `collapse(2)` para paralelizar los dos bucles externos, permitiendo una distribución más granular del trabajo.

**Pseudocódigo:**
```
función multiplicar_matrices(A, B, C, N)
    paralelo para i = 0 hasta N-1, j = 0 hasta N-1
        C[i][j] = 0
        para k = 0 hasta N-1
            C[i][j] += A[i][k] * B[k][j]
        fin para
    fin paralelo para
fin función
```

**Código clave:**
```cpp
#pragma omp parallel for collapse(2)
for (size_t i = 0; i < tamano; ++i) {
    for (size_t j = 0; j < tamano; ++j) {
        for (size_t k = 0; k < tamano; ++k) {
            resultado[i][j] += matrizA[i][k] * matrizB[k][j];
        }
    }
}
```

### 4. Búsqueda Paralela

**Archivo:** `src/busqueda_paralela.cpp`

**Descripción:** Realiza una búsqueda paralela en un array.

**Directivas utilizadas:**
- `#pragma omp parallel`
- `#pragma omp for`
- `#pragma omp critical`

**Justificación:** Se usa `parallel` para crear hilos, `for` para distribuir la búsqueda, y `critical` para actualizar de forma segura cuando se encuentra el objetivo.

**Pseudocódigo:**
```
función buscar_paralelo(array, N, objetivo)
    encontrado = falso
    índice = -1
    paralelo
        para i = 0 hasta N-1 en paralelo
            si array[i] == objetivo y no encontrado
                región crítica
                    si no encontrado
                        encontrado = verdadero
                        índice = i
                    fin si
                fin región crítica
            fin si
        fin para
    fin paralelo
    retornar índice
fin función
```

**Código clave:**
```cpp
#pragma omp parallel
{
    #pragma omp for
    for (size_t i = 0; i < tamano; ++i) {
        if (encontrado) continue;
        if (array[i] == objetivo) {
            #pragma omp critical
            {
                if (!encontrado) {
                    encontrado = true;
                    indice_encontrado = i;
                }
            }
        }
    }
}
```

### 5. Ordenamiento Paralelo

**Archivo:** `src/ordenamiento_paralelo.cpp`

**Descripción:** Implementa un algoritmo de merge sort paralelo.

**Directivas utilizadas:**
- `#pragma omp parallel`
- `#pragma omp single`
- `#pragma omp task`
- `#pragma omp taskwait`

**Justificación:** Se utiliza `task` para crear tareas recursivas que pueden ser ejecutadas en paralelo.

**Pseudocódigo:**
```
función merge_sort_paralelo(array, inicio, fin)
    si fin - inicio > 1
        medio = (inicio + fin) / 2
        paralelo
            single
                crear tarea
                    merge_sort_paralelo(array, inicio, medio)
                fin tarea
                crear tarea
                    merge_sort_paralelo(array, medio, fin)
                fin tarea
            fin single
            esperar tareas
        fin paralelo
        merge(array, inicio, medio, fin)
    fin si
fin función
```

**Código clave:**
```cpp
#pragma omp parallel
{
    #pragma omp single
    mergeSortParalelo(arrCopia, 0, tamano - 1);
}

// Dentro de mergeSortParalelo:
#pragma omp task shared(arr)
mergeSortParalelo(arr, inicio, medio);

#pragma omp task shared(arr)
mergeSortParalelo(arr, medio + 1, fin);

#pragma omp taskwait
merge(arr, inicio, medio, fin);
```

### 6. Procesamiento de Imágenes

**Archivo:** `src/procesamiento_imagenes.cpp`

**Descripción:** Aplica un filtro de desenfoque a una imagen.

**Directiva utilizada:** `#pragma omp parallel for collapse(2)`

**Justificación:** Se usa `parallel for` con `collapse(2)` para paralelizar el procesamiento de píxeles en ambas dimensiones de la imagen.

**Pseudocódigo:**
```
función aplicar_filtro(imagen, N, M)
    paralelo para i = 1 hasta N-2, j = 1 hasta M-2
        nueva_imagen[i][j] = promedio de los 9 píxeles alrededor de imagen[i][j]
    fin paralelo para
fin función
```

**Código clave:**
```cpp
#pragma omp parallel for collapse(2)
for (size_t i = 0; i < alto; ++i) {
    for (size_t j = 0; j < ancho; ++j) {
        imagenProcesada[i][j] = aplicarFiltro(i, j);
    }
}
```

### 7. Simulación de Monte Carlo

**Archivo:** `src/simulacion_montecarlo.cpp`

**Descripción:** Realiza una simulación de Monte Carlo para estimar π.

**Directivas utilizadas:**
- `#pragma omp parallel`
- `#pragma omp for`
- `#pragma omp atomic`

**Justificación:** Se usa `parallel` para crear hilos, `for` para distribuir la generación de puntos aleatorios, y `atomic` para actualizar de forma segura el contador global.

**Pseudocódigo:**
```
función estimar_pi(num_puntos)
    dentro_circulo = 0
    paralelo
        dentro_circulo_local = 0
        para i = 0 hasta num_puntos-1 en paralelo
            x = aleatorio entre -1 y 1
            y = aleatorio entre -1 y 1
            si x*x + y*y <= 1
                dentro_circulo_local += 1
            fin si
        fin para
        atómico
            dentro_circulo += dentro_circulo_local
        fin atómico
    fin paralelo
    retornar 4 * dentro_circulo / num_puntos
fin función
```

**Código clave:**
```cpp
#pragma omp parallel
{
    long long dentroCirculoLocal = 0;
    std::mt19937 genLocal(std::random_device{}());
    std::uniform_real_distribution<> disLocal(-1.0, 1.0);

    #pragma omp for
    for (long long i = 0; i < numPuntos; ++i) {
        double x = disLocal(genLocal);
        double y = disLocal(genLocal);
        if (x*x + y*y <= 1.0) {
            ++dentroCirculoLocal;
        }
    }

    #pragma omp atomic
    dentroCirculo += dentroCirculoLocal;
}
```

## Consejos para el Uso Efectivo de OpenMP

1. **Análisis de dependencias:** Examine cuidadosamente las dependencias de datos en su código antes de paralelizar. Asegúrese de que no haya conflictos de datos entre hilos.

2. **Granularidad:** Elija un tamaño de tarea apropiado. Tareas demasiado pequeñas pueden llevar a una sobrecarga de gestión de hilos, mientras que tareas demasiado grandes pueden resultar en un desequilibrio de carga.

3. **Equilibrio de carga:** Utilice planificación dinámica (`schedule(dynamic)`) cuando las iteraciones del bucle tengan cargas de trabajo variables.

4. **Reducción de overhead:** Evite crear y destruir hilos con frecuencia. Utilice regiones paralelas más grandes cuando sea posible.

5. **Uso de variables privadas:** Declare variables como privadas cuando cada hilo necesite su propia copia para evitar condiciones de carrera.

6. **Sincronización mínima:** Minimice el uso de barreras y secciones críticas, ya que pueden ser costosas en términos de rendimiento.

7. **Pruebas y mediciones:** Realice pruebas exhaustivas y mida el rendimiento para asegurarse de que la paralelización realmente mejora el rendimiento.

8. **Escalabilidad:** Diseñe sus algoritmos paralelos pensando en la escalabilidad, considerando cómo se comportarán con un número creciente de núcleos.

9. **Uso de herramientas de perfilado:** Utilice herramientas de perfilado específicas para OpenMP para identificar cuellos de botella y oportunidades de optimización.

10. **Familiarización con la arquitectura:** Conozca la arquitectura de su sistema (número de núcleos, topología de caché, etc.) para optimizar el rendimiento.

## Recursos Adicionales

- [Especificación oficial de OpenMP](https://www.openmp.org/specifications/): Documentación completa y actualizada de OpenMP.
- [Tutorial de OpenMP](https://computing.llnl.gov/tutorials/openMP/): Un excelente recurso para aprender OpenMP paso a paso.
- [Guía de referencia rápida de OpenMP](https://www.openmp.org/wp-content/uploads/OpenMP-4.0-C.pdf): Una referencia concisa de las directivas y cláusulas de OpenMP.
- [Intel OpenMP Runtime Library](https://www.openmprtl.org/): Documentación y recursos de la implementación de OpenMP de Intel.
- [GCC OpenMP Manual](https://gcc.gnu.org/onlinedocs/libgomp/): Manual de la implementación de OpenMP en GCC.
- [OpenMP in Visual C++](https://docs.microsoft.com/en-us/cpp/parallel/openmp/openmp-in-visual-cpp): Guía de uso de OpenMP en Visual Studio.

