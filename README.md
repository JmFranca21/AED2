# AED2 — Algoritmos e Estruturas de Dados 2

Repositório com as atividades da disciplina, organizado por tipo de exercício.

> O andamento de cada atividade (concluídas e pendentes) está em [`STATUS.md`](STATUS.md).

## Estrutura das pastas

### `enunciados-atvs/`
Enunciados (PDF) enviados pela professora para todas as atividades, práticas e teóricas, das aulas 3 a 12.

### `atv-teoricas/`
Atividades teóricas (resolução em PDF).
- `joaomarcelo_francabatista_aula1Teorico.PDF` — Atividade teórica da aula 1.
- `joaomarcelo_francabatista_aula2Teorico.PDF` — Atividade teórica da aula 2.
- `joaomarcelo_francabatista_aula3Teorico.pdf` — Atividade teórica da aula 3 (Shellsort, Quicksort, Heapsort e Mergesort passo a passo).
- `joaomarcelo_francabatista_aula4Teorico.pdf` — Atividade teórica da aula 4 (Counting Sort, Radix Sort e eficiência dos algoritmos de ordenação).
- `joaomarcelo_francabatista_aula5Teorico.pdf` — Atividade teórica da aula 5 (intercalação balanceada de vários caminhos, intercalação polifásica com heap mínimo e quicksort externo passo a passo).
- `joaomarcelo_francabatista_aula6Teorico.pdf` — Atividade teórica da aula 6 (inserção e remoção em árvore binária de busca, comutatividade da eliminação, algoritmo de remoção completo e percurso iterativo).
- `joaomarcelo_francabatista_aula7Teorico.pdf` — Atividade teórica da aula 7 (inserção em árvore binária e AVL com as rotações passo a passo, remoção do 362 e código de remoção em AVL).
- `joaomarcelo_francabatista_aula8Teorico.pdf` — Atividade teórica da aula 8 (inserção e remoção passo a passo em árvore vermelho-preto e diferenças entre AVL e vermelho-preto).
- `joaomarcelo_francabatista_aula9Teorico.pdf` — Atividade teórica da aula 9 (árvore B com t = 2: inserção de 1 a 10 e remoção do 8 e do 2, regras de Cormen).
- `joaomarcelo_francabatista_aula10Teorico.pdf` — Atividade teórica da aula 10 (árvores B+ e B* com t = 2: inserção de 1 a 10 e remoção do 8 e do 2).
- `joaomarcelo_francabatista_aula11Teorico.pdf` — Atividade teórica da aula 11 (Tabelas Hash: encadeamento, método da multiplicação e endereçamento aberto com sondagem linear, quadrática e hash duplo).
- `joaomarcelo_francabatista_aula12Teorico.pdf` — Atividade teórica da aula 12 (DFS e BFS no grafo do enunciado, ciclos, componentes e quando usar cada busca).

### `atv-praticas/`
Atividades práticas (exercícios de implementação/código). Cada atividade tem um `.c` com o código-fonte e um `.pdf` com o relatório (especificações da máquina, tempo de execução e análise de complexidade), seguindo o padrão `joaomarcelo_francabatista_aula{N}Pratico`.
- Aula 1: `aula1Pratico.c` / `.pdf` — primo recursivo x iterativo.
- Aula 2: `aula2Pratico.c` / `.pdf` — Bubble Sort x Insertion Sort.
- Aula 3: `aula3Pratico.c` / `.pdf` — Shellsort x Heapsort x Quicksort recursivo e iterativo, em vetores de 100.000 elementos.
- Aula 4: `aula4Pratico.c` / `.pdf` — Radix Sort de datas: Counting Sort estável x Selection Sort instável por campo.
- Aula 5: `aula5Pratico.c` / `.pdf` — ordenação externa: intercalação balanceada de 10 caminhos x quicksort externo, em arquivo de 200.000 registros com memória de 2.000.
- Aula 6: `aula6Pratico.c` / `.pdf` — vetor aleatório x vetor ordenado com busca binária x árvore binária de busca (10.000 elementos) e altura das subárvores.
- Aula 7: `aula7Pratico.c` / `.pdf` — árvore AVL (10.000 elementos) comparada com as estruturas da aula 6.
- Aula 8: `aula8Pratico.c` / `.pdf` — árvore vermelho-preto (100.000 elementos) comparada com vetores, árvore binária e AVL.
- Aula 9: `aula9Pratico.c` / `.pdf` — árvore B com 100.000 elementos (chaves por nó, tempo e altura).
- Aula 11: `aula11Pratico.c` / `.pdf` — Tabela Hash com m = 10000: encadeamento x endereçamento aberto com sondagem linear.
- Aula 12: `aula12Pratico.c` / `.pdf` — menor caminho em labirinto com grafo e busca em largura (BFS).

### `estudos/`
Códigos de estudo pessoal em C, sem vínculo direto com uma atividade avaliativa específica (ex.: implementações de algoritmos de ordenação).
- `bubble_sort.c`
- `insertion_sort.c`
- `selection_sort.c`

> Os arquivos `.exe` gerados na compilação não são versionados (ver `.gitignore`).

## Como encontrar um exercício
1. Verifique se é uma atividade **teórica** (`atv-teoricas/`) ou **prática** (`atv-praticas/`).
2. Dentro da pasta, os arquivos são nomeados pelo número da aula (ex.: `joaomarcelo_francabatista_aula7Pratico.c`).
3. Códigos em C podem ser compilados individualmente, por exemplo:
   ```bash
   gcc atv-praticas/joaomarcelo_francabatista_aula3Pratico.c -o aula3 -O0
   ./aula3
   ```
   As aulas 7, 8 e 9 usam `math.h` e precisam de `-lm` ao final do comando de compilação.
