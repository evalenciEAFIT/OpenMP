#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <omp.h>

class MultiplicadorMatrices {
private:
    std::vector<std::vector<double>> matrizA;
    std::vector<std::vector<double>> matrizB;
    std::vector<std::vector<double>> resultado;
    size_t tamano;

    // Inicializa una matriz con valores aleatorios
    void inicializarMatriz(std::vector<std::vector<double>>& matriz) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        for (auto& fila : matriz) {
            for (auto& elemento : fila) {
                elemento = dis(gen);
            }
        }
    }

public:
    // Constructor: inicializa las matrices con un tamaño dado
    MultiplicadorMatrices(size_t n) : tamano(n),
        matrizA(n, std::vector<double>(n)),
        matrizB(n, std::vector<double>(n)),
        resultado(n, std::vector<double>(n, 0.0)) {
        inicializarMatriz(matrizA);
        inicializarMatriz(matrizB);
    }

    // Realiza la multiplicación de matrices de forma secuencial
    void multiplicarSecuencial() {
        for (size_t i = 0; i < tamano; ++i) {
            for (size_t j = 0; j < tamano; ++j) {
                resultado[i][j] = 0.0;
                for (size_t k = 0; k < tamano; ++k) {
                    resultado[i][j] += matrizA[i][k] * matrizB[k][j];
                }
            }
        }
    }

    // Realiza la multiplicación de matrices de forma paralela
    void multiplicarParalelo() {
        // Usamos #pragma omp parallel for collapse(2) para paralelizar los dos bucles externos
        // Justificación: Esto permite una paralelización más granular, distribuyendo
        // el trabajo de manera más eficiente entre los hilos disponibles
        #pragma omp parallel for collapse(2)
        for (size_t i = 0; i < tamano; ++i) {
            for (size_t j = 0; j < tamano; ++j) {
                resultado[i][j] = 0.0;
                for (size_t k = 0; k < tamano; ++k) {
                    resultado[i][j] += matrizA[i][k] * matrizB[k][j];
                }
            }
        }
    }

    // Verifica si el resultado de la multiplicación paralela es correcto
    bool verificarResultado() {
        std::vector<std::vector<double>> resultadoSecuencial(tamano, std::vector<double>(tamano, 0.0));
        for (size_t i = 0; i < tamano; ++i) {
            for (size_t j = 0; j < tamano; ++j) {
                for (size_t k = 0; k < tamano; ++k) {
                    resultadoSecuencial[i][j] += matrizA[i][k] * matrizB[k][j];
                }
            }
        }
        return resultadoSecuencial == resultado;
    }

    // Ejecuta y mide el tiempo de las versiones secuencial y paralela
    void ejecutar() {
        auto inicio = std::chrono::high_resolution_clock::now();
        multiplicarSecuencial();
        auto fin = std::chrono::high_resolution_clock::now();
        auto duracionSecuencial = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        inicio = std::chrono::high_resolution_clock::now();
        multiplicarParalelo();
        fin = std::chrono::high_resolution_clock::now();
        auto duracionParalelo = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        std::cout << "Tamaño de la matriz: " << tamano << "x" << tamano << std::endl;
        std::cout << "Tiempo de ejecución (secuencial): " << duracionSecuencial << " ms" << std::endl;
        std::cout << "Tiempo de ejecución (paralelo): " << duracionParalelo << " ms" << std::endl;
        std::cout << "Aceleración: " << static_cast<double>(duracionSecuencial) / duracionParalelo << "x" << std::endl;
        std::cout << "Resultado correcto: " << (verificarResultado() ? "Sí" : "No") << std::endl;
    }
};

int main() {
    const size_t TAMANO_MATRIZ = 1000;
    MultiplicadorMatrices multiplicador(TAMANO_MATRIZ);
    multiplicador.ejecutar();
    return 0;
}