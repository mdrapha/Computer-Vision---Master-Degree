/**
 * Exercicio 5 - Binarizacao com limiar fixo
 * Limiar fixo: 128
 */

#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat cinza = cv::imread("../../imagem.jpg", cv::IMREAD_GRAYSCALE);

    if (cinza.empty()) {
        std::cerr << "Erro: nao foi possivel carregar a imagem '../../imagem.jpg'" << std::endl;
        return -1;
    }

    // Binarizacao manual com limiar fixo = 128
    int limiar = 128;
    cv::Mat binaria = cv::Mat::zeros(cinza.size(), cinza.type());

    for (int i = 0; i < cinza.rows; i++) {
        for (int j = 0; j < cinza.cols; j++) {
            if (cinza.at<uchar>(i, j) >= limiar) {
                binaria.at<uchar>(i, j) = 255;
            } else {
                binaria.at<uchar>(i, j) = 0;
            }
        }
    }

    std::cout << "=== Binarizacao com Limiar Fixo ===" << std::endl;
    std::cout << "Limiar utilizado: " << limiar << std::endl;

    // Exibe as imagens
    cv::namedWindow("Exercicio 5 - Original (Cinza)", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Exercicio 5 - Binaria (limiar=128)", cv::WINDOW_AUTOSIZE);
    cv::imshow("Exercicio 5 - Original (Cinza)", cinza);
    cv::imshow("Exercicio 5 - Binaria (limiar=128)", binaria);

    // Salva as saidas
    cv::imwrite("original_cinza.jpg", cinza);
    cv::imwrite("binaria.jpg", binaria);

    std::cout << "\nPressione qualquer tecla na janela para sair..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
