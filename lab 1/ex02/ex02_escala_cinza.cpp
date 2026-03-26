#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat imagem = cv::imread("../../imagem.jpg", cv::IMREAD_COLOR);

    if (imagem.empty()) {
        std::cerr << "Erro: nao foi possivel carregar a imagem '../../imagem.jpg'" << std::endl;
        return -1;
    }

    // Converte para escala de cinza
    cv::Mat cinza;
    cv::cvtColor(imagem, cinza, cv::COLOR_BGR2GRAY);

    std::cout << "=== Conversao para Escala de Cinza ===" << std::endl;
    std::cout << "Imagem original: " << imagem.cols << "x" << imagem.rows
              << " (" << imagem.channels() << " canais)" << std::endl;
    std::cout << "Imagem cinza:    " << cinza.cols << "x" << cinza.rows
              << " (" << cinza.channels() << " canal)" << std::endl;

    // Exibe ambas as imagens
    cv::namedWindow("Exercicio 2 - Original", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Exercicio 2 - Escala de Cinza", cv::WINDOW_AUTOSIZE);
    cv::imshow("Exercicio 2 - Original", imagem);
    cv::imshow("Exercicio 2 - Escala de Cinza", cinza);

    // Salva a imagem convertida
    cv::imwrite("cinza.jpg", cinza);

    std::cout << "\nPressione qualquer tecla na janela para sair..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
