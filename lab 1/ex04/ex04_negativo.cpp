/**
 * Exercicio 4 - Negativo da imagem
 * Formula: novo = 255 - antigo
 */

#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat cinza = cv::imread("../../imagem.jpg", cv::IMREAD_GRAYSCALE);

    if (cinza.empty()) {
        std::cerr << "Erro: nao foi possivel carregar a imagem '../../imagem.jpg'" << std::endl;
        return -1;
    }

    // Gera o negativo: novo = 255 - antigo
    cv::Mat negativo = cv::Mat::zeros(cinza.size(), cinza.type());

    for (int i = 0; i < cinza.rows; i++) {
        for (int j = 0; j < cinza.cols; j++) {
            negativo.at<uchar>(i, j) = 255 - cinza.at<uchar>(i, j);
        }
    }

    std::cout << "=== Negativo da Imagem ===" << std::endl;
    std::cout << "Transformacao aplicada: novo = 255 - antigo" << std::endl;

    // Exibe as imagens
    cv::namedWindow("Exercicio 4 - Original (Cinza)", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Exercicio 4 - Negativo", cv::WINDOW_AUTOSIZE);
    cv::imshow("Exercicio 4 - Original (Cinza)", cinza);
    cv::imshow("Exercicio 4 - Negativo", negativo);

    // Salva as saidas
    cv::imwrite("original_cinza.jpg", cinza);
    cv::imwrite("negativo.jpg", negativo);

    std::cout << "\nPressione qualquer tecla na janela para sair..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
