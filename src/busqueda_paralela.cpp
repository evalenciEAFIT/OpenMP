#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <omp.h>

class BuscadorParalelo {
private:
    std::vector<int> array;
    size_t tamano;
    int objetivo;

    // Inicializa el array con valores aleatorios y establece el objetivo
    void inicializarArray() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 1000000);

        for (auto& elemento : array) {
            elemento = dis(gen);
        }
        // Asegurarse de que el objetivo esté en el array
        objetivo = array[tamano / 2];
    }

public:
    // Constructor: inicializa el array con un tamaño dado
    BuscadorParalelo(size_t t) : tamano(t), array(t) {
        inicializarArray();
    }

    // Realiza la búsqueda de forma secuencial
    int buscarSecuencial() {
        for (size_t i = 0; i < tamano; ++i) {
            if (array[i] == objetivo) {
                return i;
            }
        }
        return -1;
    }

    // Realiza la búsqueda de forma paralela
    int buscarParalelo() {
        int indice_encontrado = -1;
        bool encontrado = false;

        // Usamos #pragma omp parallel para crear un equipo de hilos
        #pragma omp parallel
        {
            // Usamos #pragma omp for para distribuir las iteraciones entre los hilos
            // Justificación: Permite que múltiples hilos busquen simultáneamente en diferentes
            // partes del array, potencialmente encontrando el objetivo más rápido
            #pragma omp for
            for (size_t i = 0; i < tamano; ++i) {
                // Si ya se encontró el objetivo, los demás hilos pueden terminar
                if (encontrado) continue;
                if (array[i] == objetivo) {
                    // Usamos #pragma omp critical para asegurar que solo un hilo
                    // actualice las variables compartidas a la vez
                    // Justificación: Evita condiciones de carrera al actualizar
                    // las variables 'encontrado' e 'indice_encontrado'
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

        return indice_encontrado;
    }

    // Ejecuta y mide el tiempo de las versiones secuencial y paralela
    void ejecutar() {
        auto inicio = std::chrono::high_resolution_clock::now();
        int resultadoSecuencial = buscarSecuencial();
        auto fin = std::chrono::high_resolution_clock::now();
        auto duracionSecuencial = std::chrono::duration_cast<std::chrono::microseconds>(fin - inicio).count();

        inicio = std::chrono::high_resolution_clock::now();
        int resultadoParalelo = buscarParalelo();
        fin = std::chrono::high_resolution_clock::now();
        auto duracionParalelo = std::chrono::duration_cast<std::chrono::microseconds>(fin - inicio).count();

        std::cout << "Tamaño del array: " << tamano << std::endl;
        std::cout << "Objetivo: " << objetivo << std::endl;
        std::cout << "Índice encontrado (secuencial): " << resultadoSecuencial << std::endl;
        std::cout << "Índice encontrado (paralelo): " << resultadoParalelo << std::endl;
        std::cout << "Tiempo de ejecución (secuencial): " << duracionSecuencial << " microsegundos" << std::endl;
        std::cout << "Tiempo de ejecución (paralelo): " << duracionParalelo << " microsegundos" << std::endl;
        std::cout << "Aceleración: " << static_cast<double>(duracionSecuencial) / duracionParalelo << "x" << std::endl;
    }
};

int main() {
    const size_t TAMANO_ARRAY = 100000000;
    BuscadorParalelo buscador(TAMANO_ARRAY);
    buscador.ejecutar();
    return 0;
}