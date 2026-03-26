# Exercício 7 — Redução de Resolução Espacial

## Resultados

| Imagem | Dimensão | Descrição |
|--------|----------|-----------|
| ![Original](original.jpg) | 512 × 512 | Imagem original |
| ![Reduzida](reduzida.jpg) | 256 × 256 | Após sub-amostragem (metade) |
| ![Reampliada](reampliada.jpg) | 512 × 512 | Após re-ampliação ao tamanho original |

## Comparação visual

A imagem reampliada apresenta **pixelização** evidente quando comparada à original. Bordas que na imagem original eram definidas tornam-se serrilhadas, e detalhes finos (texturas, gradientes sutis) são irrecuperavelmente perdidos.

Esse resultado ilustra o **Teorema da Amostragem de Nyquist-Shannon**: ao reduzir a taxa de amostragem (resolução) pela metade, frequências espaciais acima da nova frequência de Nyquist são perdidas. A re-ampliação apenas replica pixels existentes, não recria a informação descartada.

