#include <iostream>
#include <iomanip>
#include <chrono>
#include <random>
#include <cmath>
#include <omp.h>

class SimuladorMonteCarlo {
private:
    long long numPuntos;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

public:
    // Constructor: inicializa el generador de números aleatorios
    SimuladorMonteCarlo(long long puntos) : numPuntos(puntos), gen(std::random_device{}()), dis(-1.0, 1.0) {}

    // Método secuencial para estimar Pi
    double simularSecuencial() {
        long long dentroCirculo = 0;

        for (long long i = 0; i < numPuntos; ++i) {
            // Genera puntos aleatorios en el cuadrado [-1,1] x [-1,1]
            double x = dis(gen);
            double y = dis(gen);
            // Verifica si el punto está dentro del círculo unitario
            if (x*x + y*y <= 1.0) {
                ++dentroCirculo;
            }
        }

        // Calcula la estimación de Pi
        return 4.0 * dentroCirculo / numPuntos;
    }

    // Método paralelo para estimar Pi usando OpenMP
    double simularParalelo() {
        long long dentroCirculo = 0;

        // Crea un equipo de hilos
        #pragma omp parallel
        {
            // Variables locales para cada hilo
            long long dentroCirculoLocal = 0;
            std::mt19937 genLocal(std::random_device{}());
            std::uniform_real_distribution<> disLocal(-1.0, 1.0);

            // Distribuye las iteraciones entre los hilos
            // Justificación: Cada iteración es independiente, permitiendo una paralelización eficiente
            #pragma omp for
            for (long long i = 0; i < numPuntos; ++i) {
                double x = disLocal(genLocal);
                double y = disLocal(genLocal);
                if (x*x + y*y <= 1.0) {
                    ++dentroCirculoLocal;
                }
            }

            // Suma atómica para evitar condiciones de carrera
            // Justificación: Múltiples hilos necesitan actualizar la variable compartida de forma segura
            #pragma omp atomic
            dentroCirculo += dentroCirculoLocal;
        }

        return 4.0 * dentroCirculo / numPuntos;
    }

    // Método para ejecutar y comparar las versiones secuencial y paralela
    void ejecutar() {
        std::cout << std::fixed << std::setprecision(10);

        // Ejecución y medición del tiempo para la versión secuencial
        auto inicio = std::chrono::high_resolution_clock::now();
        double piSecuencial = simularSecuencial();
        auto fin = std::chrono::high_resolution_clock::now();
        auto duracionSecuencial = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        // Ejecución y medición del tiempo para la versión paralela
        inicio = std::chrono::high_resolution_clock::now();
        double piParalelo = simularParalelo();
        fin = std::chrono::high_resolution_clock::now();
        auto duracionParalelo = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        double piReal = M_PI;

        // Impresión de resultados
        std::cout << "Número de puntos: " << numPuntos << std::endl;
        std::cout << "Valor real de Pi: " << piReal << std::endl;
        std::cout << std::endl;

        std::cout << "Resultado secuencial:" << std::endl;
        std::cout << "Pi estimado: " << piSecuencial << std::endl;
        std::cout << "Error absoluto: " << std::abs(piSecuencial - piReal) << std::endl;
        std::cout << "Error relativo: " << std::abs(piSecuencial - piReal) / piReal * 100 << "%" << std::endl;
        std::cout << "Tiempo de ejecución: " << duracionSecuencial << " ms" << std::endl;
        std::cout << std::endl;

        std::cout << "Resultado paralelo:" << std::endl;
        std::cout << "Pi estimado: " << piParalelo << std::endl;
        std::cout << "Error absoluto: " << std::abs(piParalelo - piReal) << std::endl;
        std::cout << "Error relativo: " << std::abs(piParalelo - piReal) / piReal * 100 << "%" << std::endl;
        std::cout << "Tiempo de ejecución: " << duracionParalelo << " ms" << std::endl;
        std::cout << std::endl;

        // Cálculo y muestra de la aceleración obtenida con la versión paralela
        std::cout << "Aceleración: " << static_cast<double>(duracionSecuencial) / duracionParalelo << "x" << std::endl;
    }
};

int main() {
    const long long NUM_PUNTOS = 1000000000;
    SimuladorMonteCarlo simulador(NUM_PUNTOS);
    simulador.ejecutar();
    return 0;
}