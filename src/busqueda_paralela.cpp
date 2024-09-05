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
        std::uniform_int_distribution<> dis(1, 100000000);

        for (auto& elemento : array) {
            elemento = dis(gen);
        }
        // Asegurarse de que el objetivo esté en el array
        //size_t indice_objetivo = tamano / 2;
        //objetivo = array[indice_objetivo];
        objetivo = dis(gen);
        std::cout << "Elemento a buscar: " << objetivo << std::endl;
    }

public:
    // Constructor: inicializa el array con un tamaño dado
    BuscadorParalelo(size_t t) : tamano(t), array(t) {
        inicializarArray();
    }

    // Método de búsqueda secuencial tradicional
    int buscarSecuencial() {
        for (size_t i = 0; i < tamano; ++i) {
            if (array[i] == objetivo) {
                return i;
            }
        }
        return -1; // Retorna -1 si no se encuentra el elemento
    }

    // Método de búsqueda paralela usando OpenMP
    int buscarParalelo() {
        int indice_encontrado = -1;
        bool encontrado = false;

        // Crea un equipo de hilos
        #pragma omp parallel
        {
            // Distribuye las iteraciones del bucle entre los hilos
            // Justificación: Permite que múltiples hilos busquen en diferentes partes del array simultáneamente
            #pragma omp for
            for (size_t i = 0; i < tamano; ++i) {
                // Si ya se encontró el elemento, los demás hilos pueden terminar
                if (encontrado) continue;
                if (array[i] == objetivo) {
                    // Sección crítica para actualizar las variables compartidas
                    // Justificación: Evita condiciones de carrera al actualizar 'encontrado' e 'indice_encontrado'
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

    // Método para ejecutar y comparar las versiones secuencial y paralela
    void ejecutar() {
        // Ejecución y medición del tiempo para la versión secuencial
        auto inicio = std::chrono::high_resolution_clock::now();
        int resultadoSecuencial = buscarSecuencial();
        auto fin = std::chrono::high_resolution_clock::now();
        auto duracionSecuencial = std::chrono::duration_cast<std::chrono::microseconds>(fin - inicio).count();

        // Ejecución y medición del tiempo para la versión paralela
        inicio = std::chrono::high_resolution_clock::now();
        int resultadoParalelo = buscarParalelo();
        fin = std::chrono::high_resolution_clock::now();
        auto duracionParalelo = std::chrono::duration_cast<std::chrono::microseconds>(fin - inicio).count();

        // Impresión de resultados
        std::cout << "Tamaño del array: " << tamano << std::endl;
        std::cout << "Índice encontrado (secuencial): "  
                  << " ( Array["<< resultadoSecuencial <<"]: " << (resultadoSecuencial != -1 ? std::to_string(array[resultadoSecuencial]) : "no encontrado") << " )" << std::endl;
        std::cout << "Índice encontrado (paralelo):   "  
                  << " ( Array["<< resultadoParalelo <<"]: " << (resultadoParalelo != -1 ? std::to_string(array[resultadoParalelo]) : "no encontrado") << " )" << std::endl;
        std::cout << "Tiempo de ejecución (secuencial): " << duracionSecuencial << " microsegundos" << std::endl;
        std::cout << "Tiempo de ejecución (paralelo):   " << duracionParalelo << " microsegundos" << std::endl;
        std::cout << "Aceleración: " << static_cast<double>(duracionSecuencial) / duracionParalelo << "x" << std::endl;
    }
};

int main() {
    const size_t TAMANO_ARRAY = 100000000; // Tamaño del array para la búsqueda
    BuscadorParalelo buscador(TAMANO_ARRAY);
    buscador.ejecutar();
    return 0;
}