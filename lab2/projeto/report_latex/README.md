# Relatorio LaTeX

Arquivos principais:

- `main.tex`: relatorio em LaTeX
- `figures/`: figuras usadas no texto

Antes de compilar a versao final:

1. Abrir `main.tex`.
2. Conferir se as figuras desejadas estao presentes na pasta `figures/`.

Os valores da tabela obrigatoria devem vir da inspecao visual das 100 imagens, usando como apoio:

- `../results_manual/balanced/classification_template.csv`
- `../results_manual/balanced/classification_summary.txt`

Compilacao tipica:

```bash
pdflatex main.tex
pdflatex main.tex
```

Se for subir no Prism/Overleaf, basta enviar a pasta `report_latex`.
