#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat cinza = cv::imread("../../imagem.jpg", cv::IMREAD_GRAYSCALE);

    if (cinza.empty()) {
        std::cerr << "Erro: nao foi possivel carregar a imagem '../../imagem.jpg'" << std::endl;
        return -1;
    }

    std::cout << "=== Leitura do Valor de um Pixel ===" << std::endl;
    std::cout << "Imagem carregada: " << cinza.cols << "x" << cinza.rows << std::endl;
    std::cout << "Faixa valida: x=[0, " << cinza.cols - 1 << "], y=[0, " << cinza.rows - 1 << "]" << std::endl;

    // Exibe a imagem
    cv::namedWindow("Exercicio 3 - Imagem em Cinza", cv::WINDOW_AUTOSIZE);
    cv::imshow("Exercicio 3 - Imagem em Cinza", cinza);

    // Loop interativo para consultar pixels
    int x, y;
    while (true) {
        std::cout << "\nDigite as coordenadas (x y) ou (-1 -1) para sair: ";
        std::cin >> x >> y;

        if (x == -1 && y == -1) {
            break;
        }

        if (x < 0 || x >= cinza.cols || y < 0 || y >= cinza.rows) {
            std::cerr << "Coordenadas fora dos limites!" << std::endl;
            continue;
        }

        int intensidade = static_cast<int>(cinza.at<uchar>(y, x));
        std::cout << "Intensidade no pixel (" << x << ", " << y << ") = " << intensidade
                  << " (de 0 a 255)" << std::endl;
    }

    cv::destroyAllWindows();
    return 0;
}
