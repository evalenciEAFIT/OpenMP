#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>
#include <omp.h>

class SimuladorMonteCarlo {
private:
    long long numPuntos;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

public:
    // Constructor: inicializa el generador de números aleatorios
    SimuladorMonteCarlo(long long puntos) : numPuntos(puntos), gen(std::random_device{}()), dis(-1.0, 1.0) {}

    // Realiza la simulación de Monte Carlo de forma secuencial
    double simularSecuencial() {
        long long dentroCirculo = 0;

        for (long long i = 0; i < numPuntos; ++i) {
            double x = dis(gen);
            double y = dis(gen);
            if (x*x + y*y <= 1.0) {
                ++dentroCirculo;
            }
        }

        return 4.0 * dentroCirculo / numPuntos;
    }

    // Realiza la simulación de Monte Carlo de forma paralela
    double simularParalelo() {
        long long dentroCirculo = 0;

        // Usamos #pragma omp parallel para crear un equipo de hilos
        #pragma omp parallel
        {
            // Cada hilo tiene su propia copia local de estas variables
            long long dentroCirculoLocal = 0;
            std::mt19937 genLocal(std::random_device{}());
            std::uniform_real_distribution<> disLocal(-1.0, 1.0);

            // Usamos #pragma omp for para distribuir las iteraciones entre los hilos
            // Justificación: Permite que cada hilo procese un subconjunto de puntos de forma independiente
            #pragma omp for
            for (long long i = 0; i < numPuntos; ++i) {
                double x = disLocal(genLocal);
                double y = disLocal(genLocal);
                if (x*x + y*y <= 1.0) {
                    ++dentroCirculoLocal;
                }
            }

            // Usamos #pragma omp atomic para sumar de forma segura las contribuciones locales
            // Justificación: Evita condiciones de carrera al actualizar la variable compartida dentroCirculo
            #pragma omp atomic
            dentroCirculo += dentroCirculoLocal;
        }

        return 4.0 * dentroCirculo / numPuntos;
    }

    // Ejecuta y mide el tiempo de las versiones secuencial y paralela
    void ejecutar() {
        auto inicio = std::chrono::high_resolution_clock::now();
        double piSecuencial = simularSecuencial();
        auto fin = std::chrono::high_resolution_clock::now();
        auto duracionSecuencial = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        inicio = std::chrono::high_resolution_clock::now();
        double piParalelo = simularParalelo();
        fin = std::chrono::high_resolution_clock::now();
        auto duracionParalelo = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        std::cout << "Número de puntos: " << numPuntos << std::endl;
        std::cout << std::setprecision(15);
        std::cout << "Pi estimado (secuencial): " << piSecuencial << std::endl;
        std::cout << "Pi estimado (paralelo): " << piParalelo << std::endl;
        std::cout << "Tiempo de ejecución (secuencial): " << duracionSecuencial << " ms" << std::endl;
        std::cout << "Tiempo de ejecución (paralelo): " << duracionParalelo << " ms" << std::endl;
        std::cout << "Aceleración: " << static_cast<double>(duracionSecuencial) / duracionParalelo << "x" << std::endl;
    }
};

int main() {
    const long long NUM_PUNTOS = 1000000000;
    SimuladorMonteCarlo simulador(NUM_PUNTOS);
    simulador.ejecutar();
    return 0;
}