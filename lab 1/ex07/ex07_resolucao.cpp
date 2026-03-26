/**
 * Exercicio 7 - Reducao de resolucao espacial
 * Reduz para metade, depois reamplia ao tamanho original.
 */

#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat original = cv::imread("../../imagem.jpg", cv::IMREAD_GRAYSCALE);

    if (original.empty()) {
        std::cerr << "Erro: nao foi possivel carregar a imagem '../../imagem.jpg'" << std::endl;
        return -1;
    }

    std::cout << "=== Reducao de Resolucao Espacial ===" << std::endl;
    std::cout << "Tamanho original: " << original.cols << "x" << original.rows << std::endl;

    // Reduz para metade (sub-amostragem)
    cv::Mat reduzida;
    cv::resize(original, reduzida, cv::Size(original.cols / 2, original.rows / 2),
               0, 0, cv::INTER_NEAREST);
    std::cout << "Tamanho reduzido: " << reduzida.cols << "x" << reduzida.rows << std::endl;

    // Amplia de volta ao tamanho original (re-amostragem)
    cv::Mat reampliada;
    cv::resize(reduzida, reampliada, cv::Size(original.cols, original.rows),
               0, 0, cv::INTER_NEAREST);
    std::cout << "Tamanho reampliado: " << reampliada.cols << "x" << reampliada.rows << std::endl;

    // Exibe as tres imagens
    cv::namedWindow("Exercicio 7 - Original", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Exercicio 7 - Reduzida (metade)", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Exercicio 7 - Reampliada", cv::WINDOW_AUTOSIZE);
    cv::imshow("Exercicio 7 - Original", original);
    cv::imshow("Exercicio 7 - Reduzida (metade)", reduzida);
    cv::imshow("Exercicio 7 - Reampliada", reampliada);

    // Salva as saidas
    cv::imwrite("original.jpg", original);
    cv::imwrite("reduzida.jpg", reduzida);
    cv::imwrite("reampliada.jpg", reampliada);

    std::cout << "\nPressione qualquer tecla na janela para sair..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
