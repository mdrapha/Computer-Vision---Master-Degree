#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Carrega a imagem colorida do disco
    cv::Mat imagem = cv::imread("../../imagem.jpg", cv::IMREAD_COLOR);

    if (imagem.empty()) {
        std::cerr << "Erro: nao foi possivel carregar a imagem '../../imagem.jpg'" << std::endl;
        return -1;
    }

    // Exibe informacoes no terminal
    std::cout << "=== Informacoes da Imagem ===" << std::endl;
    std::cout << "Largura (colunas): " << imagem.cols << " pixels" << std::endl;
    std::cout << "Altura  (linhas):  " << imagem.rows << " pixels" << std::endl;
    std::cout << "Numero de canais:  " << imagem.channels() << std::endl;
    std::cout << "Resolucao total:   " << imagem.cols << " x " << imagem.rows << std::endl;

    // Exibe a imagem em uma janela
    cv::namedWindow("Exercicio 1 - Imagem Original", cv::WINDOW_AUTOSIZE);
    cv::imshow("Exercicio 1 - Imagem Original", imagem);

    std::cout << "\nPressione qualquer tecla na janela para sair..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
