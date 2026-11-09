# Status das atividades — AED2

Controle do que já foi entregue e do que ainda está pendente em cada atividade.
Atualizado a cada commit.

Legenda: ✅ concluída · 🟡 em andamento (falta algo) · ⬜ enunciado ainda não recebido

## Resumo

| Aula | Tema | Prática (`.c` + `.pdf`) | Teórica (`.pdf`) |
|------|------|-------------------------|------------------|
| 1 | Recursivo x iterativo (primo) | ✅ | ✅ |
| 2 | Bubble Sort x Insertion Sort | ✅ | ✅ |
| 3 | Ordenação n log n (Shellsort, Heapsort, Quicksort, Mergesort) | 🟡 código pronto, falta o PDF | ✅ |
| 4 | Counting Sort e Radix Sort | ✅ | ✅ |

## Pendências detalhadas

### Aula 3 — Prática (🟡)

- ✅ Código: `atv-praticas/joaomarcelo_francabatista_aula3Pratico.c`
  - Shellsort (h de Knuth), Heapsort, Quicksort recursivo e iterativo (pivô: mediana de três).
  - Vetores de 100.000 elementos (aleatório, crescente, decrescente), seed 42, média de 10 execuções.
  - Corretude verificada (todos os algoritmos ordenam os 3 vetores) e sem erros de memória (ASan/UBSan).
- ⬜ Rodar o código **no computador de uso habitual** (os tempos do relatório precisam ser medidos lá, como nas aulas anteriores):
  ```bash
  gcc joaomarcelo_francabatista_aula3Pratico.c -o aula3 -O0
  ./aula3
  ```
- ⬜ Enviar a saída do programa (tabela de tempos) para montar o relatório.
- ⬜ Gerar `atv-praticas/joaomarcelo_francabatista_aula3Pratico.pdf` com: especificações da máquina, metodologia, tabela de tempos, resposta de "qual o melhor algoritmo n log n" e análise de complexidade (O/Θ/Ω).
- ⬜ Atualizar o README com a linha do PDF e fazer o commit final.

### Aula 3 — Teórica (✅)

- ✅ `atv-teoricas/joaomarcelo_francabatista_aula3Teorico.pdf`: Shellsort (h = 3, 2, 1), Quicksort (pivô: mediana de três), Heapsort e Mergesort passo a passo em {22, 35, 2, 1, 13, 7}, mais como funciona o Mergesort e comparação com o Quicksort.

## Padrão seguido em todas as atividades

- Prática: `joaomarcelo_francabatista_aula{N}Pratico.c` + `.pdf` (relatório de 1 página) em `atv-praticas/`.
- Teórica: `joaomarcelo_francabatista_aula{N}Teorico.pdf` em `atv-teoricas/`.
- Código em C sem acentos, `srand(42)`, tempo com `clock_gettime(CLOCK_MONOTONIC)` e nota de utilização de IA no final.
- Cada atividade é listada no `README.md`.
