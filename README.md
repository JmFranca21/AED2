# AED2 — Algoritmos e Estruturas de Dados 2

Repositório com as atividades da disciplina, organizado por tipo de exercício.

## Estrutura das pastas

### `atv-teoricas/`
Atividades teóricas (enunciados/respostas em PDF).
- `atv1-teorica.PDF` — Atividade teórica 1.

### `atv-praticas/`
Atividades práticas (exercícios de implementação/código). Cada exercício prático deve ser adicionado aqui.

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
