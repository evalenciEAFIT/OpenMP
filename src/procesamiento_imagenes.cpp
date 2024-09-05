#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <omp.h>

class ProcesadorImagenes {
private:
    std::vector<std::vector<int>> imagen;
    std::vector<std::vector<int>> imagenProcesada;
    size_t ancho, alto;

    // Inicializa la imagen con valores aleatorios
    void inicializarImagen() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);

        for (auto& fila : imagen) {
            for (auto& pixel : fila) {
                pixel = dis(gen);
            }
        }
    }

    // Aplica un filtro de desenfoque a un pixel
    int aplicarFiltro(size_t x, size_t y) {
        int suma = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int nx = x + i;
                int ny = y + j;
                if (nx >= 0 && nx < alto && ny >= 0 && ny < ancho) {
                    suma += imagen[nx][ny];
                }
            }
        }
        return suma / 9;
    }

public:
    // Constructor: inicializa la imagen con un tamaño dado
    ProcesadorImagenes(size_t w, size_t h) : ancho(w), alto(h), 
        imagen(h, std::vector<int>(w)), imagenProcesada(h, std::vector<int>(w)) {
        inicializarImagen();
    }

    // Procesa la imagen de forma secuencial
    void procesarSecuencial() {
        for (size_t i = 0; i < alto; ++i) {
            for (size_t j = 0; j < ancho; ++j) {
                imagenProcesada[i][j] = aplicarFiltro(i, j);
            }
        }
    }

    // Procesa la imagen de forma paralela
    void procesarParalelo() {
        // Usamos #pragma omp parallel for collapse(2) para paralelizar los dos bucles anidados
        // Justificación: Esto permite una paralelización más granular, distribuyendo
        // el trabajo de procesamiento de píxeles entre los hilos de manera eficiente
        #pragma omp parallel for collapse(2)
        for (size_t i = 0; i < alto; ++i) {
            for (size_t j = 0; j < ancho; ++j) {
                imagenProcesada[i][j] = aplicarFiltro(i, j);
            }
        }
    }

    // Verifica si el resultado del procesamiento paralelo es correcto
    bool verificarResultado() {
        std::vector<std::vector<int>> resultadoSecuencial(alto, std::vector<int>(ancho));
        for (size_t i = 0; i < alto; ++i) {
            for (size_t j = 0; j < ancho; ++j) {
                resultadoSecuencial[i][j] = aplicarFiltro(i, j);
            }
        }
        return resultadoSecuencial == imagenProcesada;
    }

    // Ejecuta y mide el tiempo de las versiones secuencial y paralela
    void ejecutar() {
        auto inicio = std::chrono::high_resolution_clock::now();
        procesarSecuencial();
        auto fin = std::chrono::high_resolution_clock::now();
        auto duracionSecuencial = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        inicio = std::chrono::high_resolution_clock::now();
        procesarParalelo();
        fin = std::chrono::high_resolution_clock::now();
        auto duracionParalelo = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

        std::cout << "Tamaño de la imagen: " << ancho << "x" << alto << std::endl;
        std::cout << "Tiempo de ejecución (secuencial): " << duracionSecuencial << " ms" << std::endl;
        std::cout << "Tiempo de ejecución (paralelo): " << duracionParalelo << " ms" << std::endl;
        std::cout << "Aceleración: " << static_cast<double>(duracionSecuencial) / duracionParalelo << "x" << std::endl;
        std::cout << "Resultado correcto: " << (verificarResultado() ? "Sí" : "No") << std::endl;
    }
};

int main() {
    const size_t ANCHO = 5000;
    const size_t ALTO = 5000;
    ProcesadorImagenes procesador(ANCHO, ALTO);
    procesador.ejecutar();
    return 0;
}