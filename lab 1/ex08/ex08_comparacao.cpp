/**
 * Exercicio 8 - Comparacao entre amostragem e quantizacao
 * Produz tres imagens: resolucao reduzida, poucos niveis, e ambos combinados.
 */

#include <opencv2/opencv.hpp>
#include <iostream>

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

cv::Mat reduzir_resolucao(const cv::Mat& imagem, int fator) {
    cv::Mat reduzida, reampliada;
    cv::resize(imagem, reduzida,
               cv::Size(imagem.cols / fator, imagem.rows / fator),
               0, 0, cv::INTER_NEAREST);
    cv::resize(reduzida, reampliada,
               cv::Size(imagem.cols, imagem.rows),
               0, 0, cv::INTER_NEAREST);
    return reampliada;
}

int main() {
    cv::Mat original = cv::imread("../../imagem.jpg", cv::IMREAD_GRAYSCALE);

    if (original.empty()) {
        std::cerr << "Erro: nao foi possivel carregar a imagem '../../imagem.jpg'" << std::endl;
        return -1;
    }

    std::cout << "=== Comparacao: Amostragem vs Quantizacao ===" << std::endl;
    std::cout << "Imagem original: " << original.cols << "x" << original.rows
              << ", 256 niveis" << std::endl;

    // 1. Apenas resolucao reduzida (fator 4x)
    cv::Mat img_amostragem = reduzir_resolucao(original, 4);

    // 2. Apenas quantizacao (8 niveis)
    cv::Mat img_quantizacao = quantizar(original, 8);

    // 3. Ambos os efeitos combinados
    cv::Mat img_combinada = quantizar(reduzir_resolucao(original, 4), 8);

    // Exibe todas as imagens
    cv::namedWindow("Ex8 - Original", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Ex8 - Amostragem (res/4)", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Ex8 - Quantizacao (8 niveis)", cv::WINDOW_AUTOSIZE);
    cv::namedWindow("Ex8 - Combinado", cv::WINDOW_AUTOSIZE);

    cv::imshow("Ex8 - Original", original);
    cv::imshow("Ex8 - Amostragem (res/4)", img_amostragem);
    cv::imshow("Ex8 - Quantizacao (8 niveis)", img_quantizacao);
    cv::imshow("Ex8 - Combinado", img_combinada);

    // Salva as imagens
    cv::imwrite("original.jpg", original);
    cv::imwrite("amostragem.jpg", img_amostragem);
    cv::imwrite("quantizacao_8niveis.jpg", img_quantizacao);
    cv::imwrite("combinado.jpg", img_combinada);

    std::cout << "\nPressione qualquer tecla na janela para sair..." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
