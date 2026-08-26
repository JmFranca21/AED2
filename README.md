# AED2 — Algoritmos e Estruturas de Dados 2

Repositório com as atividades da disciplina, organizado por tipo de exercício.

## Estrutura das pastas

### `atv-teoricas/`
Atividades teóricas (enunciados/respostas em PDF).
- `joaomarcelo_francabatista_aula1Teorico.PDF` — Atividade teórica da aula 1.

### `atv-praticas/`
Atividades práticas (exercícios de implementação/código). Cada atividade tem um `.c` com o código-fonte e um `.pdf` com o relatório (tempo de execução e análise de complexidade), seguindo o padrão `joaomarcelo_francabatista_aula{N}Pratico`.
- `joaomarcelo_francabatista_aula1Pratico.c` — código (primo recursivo x iterativo).
- `joaomarcelo_francabatista_aula1Pratico.pdf` — relatório da aula 1 (tempos de execução e análise de complexidade).
- `joaomarcelo_francabatista_aula2Pratico.c` — código (Bubble Sort x Insertion Sort).
- `joaomarcelo_francabatista_aula2Pratico.pdf` — relatório da aula 2 (tempos de execução e análise de complexidade).

### `estudos/`
Códigos de estudo pessoal em C, sem vínculo direto com uma atividade avaliativa específica (ex.: implementações de algoritmos de ordenação).
- `bubble_sort.c`
- `insertion_sort.c`
- `selection_sort.c`

> Os arquivos `.exe` gerados na compilação não são versionados (ver `.gitignore`).

## Como encontrar um exercício
1. Verifique se é uma atividade **teórica** (`atv-teoricas/`) ou **prática** (`atv-praticas/`).
2. Dentro da pasta, os arquivos são nomeados pelo tema ou número da atividade (ex.: `atv1-teorica.PDF`).
3. Códigos em C podem ser compilados individualmente, por exemplo:
   ```bash
   gcc estudos/bubble_sort.c -o bubble_sort
   ./bubble_sort
   ```
