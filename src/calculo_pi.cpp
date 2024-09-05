#include <iostream>
#include <chrono>
#include <iomanip>
#include <omp.h>

class CalculadorPi {
private:
    long long numPasos;

public:
    // Constructor: inicializa el número de pasos para el cálculo
    CalculadorPi(long long pasos) : numPasos(pasos) {}

    // Calcula Pi de forma secuencial usando el método de integración numérica
    double calcularSecuencial() {
        double paso = 1.0 / static_cast<double>(numPasos);
        double suma = 0.0;

        for (long long i = 0; i < numPasos; i++) {
            double x = (i + 0.5) * paso;
            suma += 4.0 / (1.0 + x * x);
        }

        return paso * suma;
    }

    // Calcula Pi de forma paralela usando OpenMP
    double calcularParalelo() {
        double paso = 1.0 / static_cast<double>(numPasos);
        double suma = 0.0;

        // Usamos #pragma omp parallel para crear un equipo de hilos
        #pragma omp parallel
        {
            // Usamos #pragma omp for reduction(+:suma) para paralelizar el bucle
            // y realizar una reducción segura de la variable suma
            // Justificación: Esto permite que cada hilo compute una parte de la suma
            // y luego combina los resultados de forma segura y eficiente
            #pragma omp for reduction(+:suma)
            for (long long i = 0; i < numPasos; i++) {
                double x = (i + 0.5) * paso;
                suma += 4.0 / (1.0 + x * x);
            }
        }

        return paso * suma;
    }

    // Ejecuta y mide el tiempo de las versiones secuencial y paralela
    void ejecutar() {
        auto inicio = std::chrono::high_resolution_clock::now();
        double piSecuencial = calcularSecuencial();
        auto fin = std::chrono::high_resolution_clock::now();
        auto duracionSecuencial = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        inicio = std::chrono::high_resolution_clock::now();
        double piParalelo = calcularParalelo();
        fin = std::chrono::high_resolution_clock::now();
        auto duracionParalelo = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        std::cout << "Número de pasos: " << numPasos << std::endl;
        std::cout << std::setprecision(15);
        std::cout << "Pi (secuencial): " << piSecuencial << std::endl;
        std::cout << "Pi (paralelo):   " << piParalelo << std::endl;
        std::cout << "Tiempo de ejecución (secuencial): " << duracionSecuencial << " ms" << std::endl;
        std::cout << "Tiempo de ejecución (paralelo): " << duracionParalelo << " ms" << std::endl;
        std::cout << "Aceleración: " << static_cast<double>(duracionSecuencial) / duracionParalelo << "x" << std::endl;
    }
};

int main() {
    const long long NUM_PASOS = 1000000000;
    CalculadorPi calculador(NUM_PASOS);
    calculador.ejecutar();
    return 0;
}