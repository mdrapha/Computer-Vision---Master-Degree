# Laboratorio 02 - Deteccao de Pratos com Transformada de Hough

Implementacao em `C++17` com `OpenCV` para detectar pratos em imagens de alimentos com uma implementacao propria da Transformada de Hough para circulos.

## Estrutura

- `src/main.cpp`: fluxo principal do experimento.
- `src/config.*`: parametros, presets e parsing de linha de comando.
- `src/plate_detector.*`: pre-processamento e deteccao.
- `src/io_utils.*`: leitura de diretorio, anotacoes e exportacao de CSV.
- `src/review_labels.cpp`: mini app para classificacao visual final.

## Compilacao

```bash
cmake -S . -B build
cmake --build build --config Release
```

## Execucao

```bash
./build/lab02_hough_pratos --input-dir sel_data --output-dir results_manual --sweep
```

## Saidas

Para cada execucao, o programa gera:

- imagens anotadas com o circulo selecionado;
- `detections.csv` com centro, raio, score e numero de candidatos;
- `classification_template.csv` para apoiar a avaliacao visual exigida no relatorio.

## Mini app de classificacao visual

Depois de gerar as imagens anotadas, use o visualizador para rotular manualmente:

```bash
./build/review_labels --images-dir results_manual/sensitive/images --csv results_manual/sensitive/classification_template.csv
```

Controles principais:

- `1`: marcar como `correct`
- `2`: marcar como `partial`
- `3`: marcar como `incorrect`
- `0`: voltar para `pending`
- `n` ou seta direita: avancar
- `p` ou seta esquerda: voltar
- `j`: ir para a proxima imagem pendente
- `s`: salvar
- `q` ou `Esc`: sair salvando

Ao sair, o programa atualiza o CSV e gera `classification_summary.txt` com os totais.
