/**
 * Exercicio 6 - Reducao de niveis de cinza (quantizacao)
 * Gera versoes com 128, 64, 16 e 4 niveis de cinza.
 */

#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

// Funcao que quantiza a imagem para N niveis de cinza
cv::Mat quantizar(const cv::Mat& imagem, int niveis) {
    cv::Mat resultado = imagem.clone();
    int fator = 256 / niveis;

    for (int i = 0; i < resultado.rows; i++) {
        for (int j = 0; j < resultado.cols; j++) {
            uchar valor = resultado.at<uchar>(i, j);
            uchar quantizado = static_cast<uchar>((valor / fator) * fator + fator / 2);
            if (quantizado > 255) quantizado = 255;
            resultado.at<uchar>(i, j) = quantizado;
        }
    }
    return resultado;
}

int main() {
    cv::Mat cinza = cv::imread("../../imagem.jpg", cv::IMREAD_GRAYSCALE);

    if (cinza.empty()) {
        std::cerr << "Erro: nao foi possivel carregar a imagem '../../imagem.jpg'" << std::endl;
        return -1;
    }

    int niveis[] = {128, 64, 16, 4};

    std::cout << "=== Reducao de Niveis de Cinza (Quantizacao) ===" << std::endl;
    std::cout << "Imagem original: 256 niveis de cinza" << std::endl;

    // Exibe a original
    cv::namedWindow("Exercicio 6 - Original (256 niveis)", cv::WINDOW_AUTOSIZE);
    cv::imshow("Exercicio 6 - Original (256 niveis)", cinza);
    cv::imwrite("original_256niveis.jpg", cinza);

    // Gera, exibe e salva cada versao quantizada
    for (int n : niveis) {
        cv::Mat quantizada = quantizar(cinza, n);

        std::string titulo = "Exercicio 6 - " + std::to_string(n) + " niveis";
        cv::namedWindow(titulo, cv::WINDOW_AUTOSIZE);
        cv::imshow(titulo, quantizada);

        std::string arquivo = "quantizada_" + std::to_string(n) + "niveis.jpg";
        cv::imwrite(arquivo, quantizada);
        std::cout << "Gerada imagem com " << n << " niveis" << std::endl;
    }

    std::cout << "\nPressione qualquer tecla na janela para sair..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
