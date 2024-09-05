#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <omp.h>

class OrdenadorParalelo {
private:
    std::vector<int> array;
    size_t tamano;

    // Inicializa el array con valores aleatorios
    void inicializarArray() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 1000000);

        for (auto& elemento : array) {
            elemento = dis(gen);
        }
    }

    // Función auxiliar para mezclar dos subarrays ordenados
    void merge(std::vector<int>& arr, size_t inicio, size_t medio, size_t fin) {
        std::vector<int> temp(fin - inicio + 1);
        size_t i = inicio, j = medio + 1, k = 0;

        while (i <= medio && j <= fin) {
            if (arr[i] <= arr[j]) {
                temp[k++] = arr[i++];
            } else {
                temp[k++] = arr[j++];
            }
        }

        while (i <= medio) {
            temp[k++] = arr[i++];
        }

        while (j <= fin) {
            temp[k++] = arr[j++];
        }

        std::copy(temp.begin(), temp.end(), arr.begin() + inicio);
    }

    // Implementación secuencial del Merge Sort
    void mergeSortSecuencial(std::vector<int>& arr, size_t inicio, size_t fin) {
        if (inicio < fin) {
            size_t medio = inicio + (fin - inicio) / 2;
            mergeSortSecuencial(arr, inicio, medio);
            mergeSortSecuencial(arr, medio + 1, fin);
            merge(arr, inicio, medio, fin);
        }
    }

    // Implementación paralela del Merge Sort
    void mergeSortParalelo(std::vector<int>& arr, size_t inicio, size_t fin) {
        if (inicio < fin) {
            size_t medio = inicio + (fin - inicio) / 2;

            // Usamos #pragma omp task para crear tareas paralelas
            // Justificación: Permite una paralelización recursiva y dinámica del algoritmo,
            // aprovechando mejor los recursos en diferentes niveles de la recursión
            #pragma omp task shared(arr)
            mergeSortParalelo(arr, inicio, medio);

            #pragma omp task shared(arr)
            mergeSortParalelo(arr, medio + 1, fin);

            // Esperamos a que las tareas hijas terminen antes de hacer el merge
            #pragma omp taskwait
            merge(arr, inicio, medio, fin);
        }
    }

public:
    // Constructor: inicializa el array con un tamaño dado
    OrdenadorParalelo(size_t t) : tamano(t), array(t) {
        inicializarArray();
    }

    // Ordena el array de forma secuencial
    void ordenarSecuencial() {
        std::vector<int> arrCopia = array;
        mergeSortSecuencial(arrCopia, 0, tamano - 1);
    }

    // Ordena el array de forma paralela
    void ordenarParalelo() {
        std::vector<int> arrCopia = array;
        // Usamos #pragma omp parallel para crear un equipo de hilos
        // y #pragma omp single para que solo un hilo inicie la tarea raíz
        // Justificación: Esto permite que un solo hilo comience la recursión,
        // pero los hilos del equipo puedan ejecutar las subtareas en paralelo
        #pragma omp parallel
        {
            #pragma omp single
            mergeSortParalelo(arrCopia, 0, tamano - 1);
        }
    }

    // Verifica si el resultado del ordenamiento paralelo es correcto
    bool verificarResultado() {
        std::vector<int> arrOrdenado = array;
        std::sort(arrOrdenado.begin(), arrOrdenado.end());
        
        std::vector<int> arrParalelo = array;
        #pragma omp parallel
        {
            #pragma omp single
            mergeSortParalelo(arrParalelo, 0, tamano - 1);
        }

        return arrOrdenado == arrParalelo;
    }

    // Ejecuta y mide el tiempo de las versiones secuencial y paralela
    void ejecutar() {
        auto inicio = std::chrono::high_resolution_clock::now();
        ordenarSecuencial();
        auto fin = std::chrono::high_resolution_clock::now();
        auto duracionSecuencial = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        inicio = std::chrono::high_resolution_clock::now();
        ordenarParalelo();
        fin = std::chrono::high_resolution_clock::now();
        auto duracionParalelo = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        std::cout << "Tamaño del array: " << tamano << std::endl;
        std::cout << "Tiempo de ejecución (secuencial): " << duracionSecuencial << " ms" << std::endl;
        std::cout << "Tiempo de ejecución (paralelo): " << duracionParalelo << " ms" << std::endl;
        std::cout << "Aceleración: " << static_cast<double>(duracionSecuencial) / duracionParalelo << "x" << std::endl;
        std::cout << "Resultado correcto: " << (verificarResultado() ? "Sí" : "No") << std::endl;
    }
};

int main() {
    const size_t TAMANO_ARRAY = 10000000;
    OrdenadorParalelo ordenador(TAMANO_ARRAY);
    ordenador.ejecutar();
    return 0;
}