#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

class SumadorVectores {
private:
    std::vector<double> vectorA;
    std::vector<double> vectorB;
    std::vector<double> resultado;
    size_t tamano;

    // Inicializa un vector con valores aleatorios
    void inicializarVector(std::vector<double>& vec) {
        for (auto& elemento : vec) {
            elemento = static_cast<double>(rand()) / RAND_MAX;
        }
    }

public:
    // Constructor: inicializa los vectores con un tamaño dado
    SumadorVectores(size_t t) : tamano(t), vectorA(t), vectorB(t), resultado(t) {
        inicializarVector(vectorA);
        inicializarVector(vectorB);
    }

    // Realiza la suma de vectores de forma secuencial
    void sumarSecuencial() {
        for (size_t i = 0; i < tamano; ++i) {
            resultado[i] = vectorA[i] + vectorB[i];
        }
    }

    // Realiza la suma de vectores de forma paralela
    void sumarParalelo() {
        // Usamos #pragma omp parallel for para paralelizar el bucle
        // Justificación: Cada iteración del bucle es independiente,
        // lo que permite una paralelización sencilla y eficiente
        #pragma omp parallel for
        for (size_t i = 0; i < tamano; ++i) {
            resultado[i] = vectorA[i] + vectorB[i];
        }
    }

    // Verifica si el resultado de la suma paralela es correcto
    bool verificarResultado() {
        std::vector<double> resultadoSecuencial(tamano);
        for (size_t i = 0; i < tamano; ++i) {
            resultadoSecuencial[i] = vectorA[i] + vectorB[i];
        }
        //std::cout << "suma secuencial = " << resultadoSecuencial << " | " << resultado << " = suma paralela" << std::endl;
        return resultadoSecuencial == resultado;
    }

    // Ejecuta y mide el tiempo de las versiones secuencial y paralela
    void ejecutar() {
        auto inicio = std::chrono::high_resolution_clock::now();
        sumarSecuencial();
        auto fin = std::chrono::high_resolution_clock::now();
        auto duracionSecuencial = std::chrono::duration_cast<std::chrono::microseconds>(fin - inicio).count();

        inicio = std::chrono::high_resolution_clock::now();
        sumarParalelo();
        fin = std::chrono::high_resolution_clock::now();
        auto duracionParalelo = std::chrono::duration_cast<std::chrono::microseconds>(fin - inicio).count();

        std::cout << "Tamaño del vector: " << tamano << std::endl;
        std::cout << "Tiempo de ejecución (secuencial): " << duracionSecuencial << " microsegundos" << std::endl;
        std::cout << "Tiempo de ejecución (paralelo): " << duracionParalelo << " microsegundos" << std::endl;
        std::cout << "Aceleración: " << static_cast<double>(duracionSecuencial) / duracionParalelo << "x" << std::endl;
        std::cout << "Resultado correcto: " << (verificarResultado() ? "Sí" : "No") << std::endl;
    }
};

int main() {
    const size_t TAMANO_VECTOR = 10000000;
    SumadorVectores sumador(TAMANO_VECTOR);
    sumador.ejecutar();
    return 0;
}