# Trabalho 3 — Análise de dados em formato de grafos

Enunciado: [`enunciados-atvs/Trabalho 3 - Analise de dados em formato de grafos.pdf`](../enunciados-atvs/Trabalho%203%20-%20Analise%20de%20dados%20em%20formato%20de%20grafos.pdf). Entrega em 07/12/2026 às 12:00, com apresentação obrigatória em aula.

## Arquivos

| Arquivo | O que é |
|---|---|
| `joaomarcelo_francabatista_algoritmo.c` | Programa em C: leitura, TAD grafo (lista de adjacência), tabela hash, grau, closeness (BFS), betweenness (Brandes), merge sort, busca, tempos e menu A–G |
| `joaomarcelo_francabatista_plot.py` | Plot do grafo (item B) com NetworkX + matplotlib, usando as métricas calculadas em C |
| `joaomarcelo_francabatista_base.txt` | Dataset ego-Facebook (SNAP) — *a adicionar* |
| `validacao/validar_networkx.py` | Confere os resultados do C contra o NetworkX (não faz parte da entrega) |
| `validacao/karate.txt` | Karate Club (34 vértices), usado como teste |

## Como rodar

```bash
gcc joaomarcelo_francabatista_algoritmo.c -o trabalho3 -O2
./trabalho3                                   # menu; lê joaomarcelo_francabatista_base.txt
./trabalho3 validacao/karate.txt              # menu com outro arquivo
./trabalho3 joaomarcelo_francabatista_base.txt --auto   # roda tudo sem menu
```

Para o plot (opção B do menu, ou direto): `pip install networkx matplotlib scipy` e depois `python joaomarcelo_francabatista_plot.py`. O layout é calculado na primeira vez (cerca de 1 minuto para 4 mil vértices) e salvo em `grafo_layout.csv`, de modo que as execuções seguintes são instantâneas. Os arquivos `grafo_vertices.csv` e `grafo_arestas.csv` também abrem no Gephi.

### Saídas
- `ranking_grau.txt`, `ranking_closeness.txt`, `ranking_betweenness.txt`: listas completas em ordem crescente.
- `grafo_vertices.csv`, `grafo_arestas.csv`: grafo e métricas (formato Gephi).
- `grafo_plot.png`, `grafo_metricas.png`: figuras.

## Decisões de implementação

- **Lista de adjacência** (vetor dinâmico por vértice) em vez de matriz: no ego-Facebook a densidade é de cerca de 1%. A matriz desperdiçaria memória O(V²) e faria cada BFS custar O(V²).
- **Tabela hash** (endereçamento aberto, sondagem linear, método da multiplicação, fator de carga ≤ 0,5) para mapear o ID do arquivo no índice interno. Também é usada na busca do item F, que mostra o tempo em comparação com a busca sequencial.
- **Caminho mínimo por BFS**: o grafo não tem pesos, então a BFS já dá as distâncias mínimas em O(V+E). Dijkstra seria desnecessário e Floyd-Warshall custaria O(V³).
- **Closeness** = (r−1)/Σd × (r−1)/(n−1), com a correção de Wasserman–Faust para grafos desconexos, igual à do NetworkX. A distância média, que é a definição do slide (menor = mais central), também é impressa.
- **Betweenness** pelo algoritmo de Brandes (2001), O(V·E), dividido por 2 por ser não dirigido. Também é impresso normalizado por (n−1)(n−2)/2. A contagem de caminhos (σ) é `double` para não estourar.
- **Ordenação**: merge sort estável; empates são desfeitos pelo ID.
- Laços e arestas repetidas são ignorados. Um arquivo dirigido vira não dirigido.

## Validação (contra o NetworkX)

| Grafo | n | m | Erro máximo (grau / closeness / betweenness) | C, `-O2` (closeness / betweenness) | NetworkX (closeness / betweenness) |
|---|---|---|---|---|---|
| Karate Club | 34 | 78 | ~5·10⁻¹¹ nas três | < 1 ms | < 5 ms |
| Sintético (power-law cluster, seed 42) | 4.040 | 87.932 | ~5·10⁻¹¹ nas três | 1,0 s / 4,8 s | 38,4 s / 139,7 s |

O erro de ~10⁻¹¹ é só o arredondamento das 10 casas decimais do CSV. No Karate, o betweenness do vértice 1 dá 231,07, o valor conhecido da literatura. Os tempos são do ambiente de desenvolvimento; os do relatório serão medidos no notebook (i5-1035G1).
