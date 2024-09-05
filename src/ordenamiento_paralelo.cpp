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
        std::uniform_int_distribution<> dis(1, 1000000000);

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

    // Implementación paralela del Merge Sort usando OpenMP
    void mergeSortParalelo(std::vector<int>& arr, size_t inicio, size_t fin) {
        if (inicio < fin) {
            size_t medio = inicio + (fin - inicio) / 2;

            // Crea tareas paralelas para ordenar las dos mitades del array
            // Justificación: Permite que diferentes hilos trabajen en diferentes partes del array simultáneamente
            #pragma omp task shared(arr)
            mergeSortParalelo(arr, inicio, medio);

            #pragma omp task shared(arr)
            mergeSortParalelo(arr, medio + 1, fin);

            // Espera a que las tareas hijas terminen antes de hacer el merge
            // Justificación: Asegura que ambas mitades estén ordenadas antes de mezclarlas
            #pragma omp taskwait
            merge(arr, inicio, medio, fin);
        }
    }

    // Imprime los primeros y últimos 10 elementos de un array
    void imprimirElementos(const std::vector<int>& arr, const std::string& mensaje) {
        std::cout << mensaje << std::endl;
        std::cout << "Primeros 10 elementos: ";
        for (size_t i = 0; i < 10 && i < arr.size(); ++i) {
            std::cout << arr[i] << " ";
        }
        std::cout << std::endl;

        std::cout << "Últimos 10 elementos: ";
        for (size_t i = (arr.size() > 10 ? arr.size() - 10 : 0); i < arr.size(); ++i) {
            std::cout << arr[i] << " ";
        }
        std::cout << std::endl;
    }

public:
    // Constructor: inicializa el array con un tamaño dado
    OrdenadorParalelo(size_t t) : tamano(t), array(t) {
        inicializarArray();
    }

    // Método para ordenar el array de forma secuencial
    void ordenarSecuencial() {
        std::vector<int> arrCopia = array;
        mergeSortSecuencial(arrCopia, 0, tamano - 1);
        imprimirElementos(arrCopia, "Resultado secuencial:");
    }

    // Método para ordenar el array de forma paralela
    void ordenarParalelo() {
        std::vector<int> arrCopia = array;
        // Crea un equipo de hilos y permite que un solo hilo inicie la tarea raíz
        // Justificación: Inicia la recursión paralela desde un único punto de entrada
        #pragma omp parallel
        {
            #pragma omp single
            mergeSortParalelo(arrCopia, 0, tamano - 1);
        }
        imprimirElementos(arrCopia, "Resultado paralelo:");
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

    // Método para ejecutar y comparar las versiones secuencial y paralela
    void ejecutar() {
        imprimirElementos(array, "Array original:");

        // Ejecución y medición del tiempo para la versión secuencial
        auto inicio = std::chrono::high_resolution_clock::now();
        ordenarSecuencial();
        auto fin = std::chrono::high_resolution_clock::now();
        auto duracionSecuencial = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        // Ejecución y medición del tiempo para la versión paralela
        inicio = std::chrono::high_resolution_clock::now();
        ordenarParalelo();
        fin = std::chrono::high_resolution_clock::now();
        auto duracionParalelo = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        // Impresión de resultados
        std::cout << "Tamaño del array: " << tamano << std::endl;
        std::cout << "Tiempo de ejecución (secuencial): " << duracionSecuencial << " ms" << std::endl;
        std::cout << "Tiempo de ejecución (paralelo): " << duracionParalelo << " ms" << std::endl;
        std::cout << "Aceleración: " << static_cast<double>(duracionSecuencial) / duracionParalelo << "x" << std::endl;
        std::cout << "Resultado correcto: " << (verificarResultado() ? "Sí" : "No") << std::endl;
    }
};

int main() {
    const size_t TAMANO_ARRAY = 10000000; // Tamaño del array para el ordenamiento
    OrdenadorParalelo ordenador(TAMANO_ARRAY);
    ordenador.ejecutar();
    return 0;
}