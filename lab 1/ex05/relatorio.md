# Exercício 5 — Binarização com Limiar Fixo

## Resultados

| Imagem | Descrição |
|--------|-----------|
| ![Original em cinza](original_cinza.jpg) | Imagem original em escala de cinza |
| ![Imagem binarizada](binaria.jpg) | Imagem após binarização com limiar 128 |

A imagem binarizada divide a cena em duas classes: regiões com intensidade acima do limiar (branco) e abaixo (preto). O limiar $T = 128$ corresponde ao ponto médio da faixa [0, 255], o que pode não ser ótimo para todas as imagens. Para cenas com distribuição de intensidade desbalanceada, um limiar adaptativo (e.g., método de Otsu) produziria uma segmentação mais significativa.

