# Status das atividades — AED2

Controle do que já foi entregue e do que ainda está pendente em cada atividade.
Atualizado a cada commit.

Legenda: ✅ concluída · 🟡 em andamento (falta algo) · ⬜ enunciado ainda não recebido

## Resumo

| Aula | Tema | Prática (`.c` + `.pdf`) | Teórica (`.pdf`) |
|------|------|-------------------------|------------------|
| 1 | Recursivo x iterativo (primo) | ✅ | ✅ |
| 2 | Bubble Sort x Insertion Sort | ✅ | ✅ |
| 3 | Ordenação n log n (Shellsort, Heapsort, Quicksort, Mergesort) | ✅ | ✅ |
| 4 | Counting Sort e Radix Sort | ✅ | ✅ |
| 5 | Ordenação externa (intercalação balanceada, polifásica, quicksort externo) | ✅ | ✅ |
| 6 | Métodos de pesquisa (vetor, busca binária, árvore binária) | ✅ | ✅ |
| 7 | Árvore AVL | ✅ | ✅ |
| 8 | Árvore vermelho-preto | ✅ | ✅ |
| 9 | Árvore B | ✅ | ✅ |
| 10 | Árvores B+ e B* | — (sem prática no enunciado) | ✅ |
| 11 | Tabelas Hash | ✅ | ✅ |
| 12 | Grafos: BFS e DFS | ✅ | ✅ |

## Trabalhos

| Trabalho | Tema | Entrega | Status |
|----------|------|---------|--------|
| 3 | Análise de dados em formato de grafos (grau, closeness, betweenness) | 07/12/2026 12:00 + apresentação | 🟡 código, dataset, figuras e validação prontos; faltam o relatório IEEE e o vídeo |

Os enunciados de todas as aulas estão em `enunciados-atvs/`.

## Como foram feitos (aulas 3 a 12)

- **Práticas:** o código foi compilado com `gcc -O0` (as aulas 7, 8 e 9 precisam de `-lm`) e executado neste computador (ASUS VivoBook X515JF, Intel i5-1035G1, 8 GB, Windows 11, gcc 13.2.0). Os tempos dos PDFs são de uma execução de referência; o programa foi repetido mais vezes e o PDF informa a faixa de variação. Em todas as práticas há verificação de corretude (as estruturas/ordenações são conferidas antes de medir o tempo).
- **Teóricas com passo a passo (aulas 5, 6, 7, 8, 9, 10 e 12):** os desenhos e passos foram gerados por simuladores (BST, AVL, vermelho-preto, árvores B/B+/B*, ordenação externa, grafos) testados com milhares de operações aleatórias, e os códigos citados nos PDFs (remoção em BST, percursos iterativos, remoção em AVL) foram compilados e testados em C.
- **Aula 5 (prática):** cria arquivos binários temporários na pasta onde é executada e os apaga ao final.
- **Aula 8 (prática):** leva cerca de 1 minuto para rodar (inserção ordenada de 100.000 elementos em vetor).

## Observações sobre os enunciados

- Aula 11: o enunciado cita o arquivo de código como `nome_sobrenome_aula14`, provavelmente erro de digitação; segue o padrão `aula11Pratico`.
- Aula 8: o enunciado escreve "50.000" no texto e "5000" na tabela; foi usado 50.000. Também cita `aula08Pratico`; no repositório segue o padrão `aula8Pratico`.
- Aula 9: o enunciado pede só o `.c` (nomeado `aula9Pratico`); o PDF com a análise é um complemento.
- Aula 12 (prática): o enunciado pede só o código; o PDF com a análise é um complemento.
- Aula 10: o enunciado só tem a parte teórica.

## Padrão seguido em todas as atividades

- Prática: `joaomarcelo_francabatista_aula{N}Pratico.c` + `.pdf` (relatório) em `atv-praticas/`.
- Teórica: `joaomarcelo_francabatista_aula{N}Teorico.pdf` em `atv-teoricas/`.
- Código em C sem acentos, `srand(42)`, tempo com `clock_gettime(CLOCK_MONOTONIC)` e nota de utilização de IA no final.
- Cada atividade é listada no `README.md`.
