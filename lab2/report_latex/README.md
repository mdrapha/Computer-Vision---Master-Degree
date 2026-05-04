# Relatorio LaTeX

Arquivos principais:

- `main.tex`: relatorio em LaTeX
- `figures/`: figuras usadas no texto

Antes de compilar a versao final:

1. Abrir `main.tex`.
2. Atualizar as macros no topo do arquivo:
   - `\VisualCorrect`
   - `\VisualPartial`
   - `\VisualIncorrect`
3. Conferir se quer manter o preset `sensitive` como principal no texto final.

Os valores da tabela obrigatoria devem vir da inspecao visual das 100 imagens, usando como apoio:

- `../results/balanced/classification_template.csv`
- `../results/sensitive/classification_template.csv`
- `../results/strict/classification_template.csv`

Compilacao tipica:

```bash
pdflatex main.tex
pdflatex main.tex
```

Se for subir no Prism/Overleaf, basta enviar a pasta `report_latex`.
