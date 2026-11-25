# Trabalho 3 — Análise de dados em formato de grafos

Enunciado: [`enunciados-atvs/Trabalho 3 - Analise de dados em formato de grafos.pdf`](../enunciados-atvs/Trabalho%203%20-%20Analise%20de%20dados%20em%20formato%20de%20grafos.pdf). Entrega em 07/12/2026 às 12:00, com apresentação obrigatória em aula.

## Arquivos

| Arquivo | O que é |
|---|---|
| `joaomarcelo_francabatista_algoritmo.c` | Programa em C: leitura, TAD grafo (lista de adjacência), tabela hash, grau, closeness (BFS), betweenness (Brandes), merge sort, busca, tempos, menu A–G e opção extra H (experimento de remoção) |
| `joaomarcelo_francabatista_plot.py` | Plot do grafo (item B) com NetworkX + matplotlib, usando as métricas calculadas em C |
| `joaomarcelo_francabatista_analise.py` | Análise da rede: redes ego, pontes, comunidades (Louvain), distribuição do grau; gera `analise_rede.txt`, `grafo_egos.png` e `distribuicao_grau.png` |
| `joaomarcelo_francabatista_base.txt` | Dataset ego-Facebook do SNAP (`facebook_combined.txt`): 4.039 vértices, 88.234 arestas |
| `grafo_layout.csv` | Posições já calculadas dos vértices (cache do plot, deixa a opção B instantânea) |
| `resultados/` | Figuras, rankings completos, saídas do programa, validação e tempos no ego-Facebook |
| `relatorio/` | Relatório IEEE (LaTeX, classe IEEEtran): `joaomarcelo_francabatista_relatorio.pdf` + `.tex` + figuras (abre no Overleaf) |
| `medir_tempos_windows.bat` | Dois cliques: compila com o gcc do Windows (`-O2` e `-O0`), roda o item G e grava `tempos_windows.txt` |
| `validacao/validar_networkx.py` | Confere os resultados do C contra o NetworkX (não faz parte da entrega) |
| `validacao/karate.txt` | Karate Club (34 vértices), usado como teste |

## Como rodar

```bash
gcc joaomarcelo_francabatista_algoritmo.c -o trabalho3 -O2
./trabalho3                                   # menu; lê joaomarcelo_francabatista_base.txt
./trabalho3 validacao/karate.txt              # menu com outro arquivo
./trabalho3 joaomarcelo_francabatista_base.txt --auto   # roda tudo sem menu
```

Para o plot (opção B do menu, ou direto): `pip install networkx matplotlib scipy` e depois `python joaomarcelo_francabatista_plot.py`. O layout leva cerca de 1 minuto para 4 mil vértices; para o ego-Facebook ele já está salvo em `grafo_layout.csv`, então o plot sai na hora (para outro dataset, apague o arquivo). Os arquivos `grafo_vertices.csv` e `grafo_arestas.csv` também abrem no Gephi.

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
| **ego-Facebook** | 4.039 | 88.234 | ~5·10⁻¹¹ nas três | 0,69 s / 2,14 s | 25,6 s / 81,1 s |

O erro de ~10⁻¹¹ é só o arredondamento das 10 casas decimais do CSV. No Karate, o betweenness do vértice 1 dá 231,07, o valor conhecido da literatura. Os tempos são do ambiente de desenvolvimento; os do relatório serão medidos no notebook (i5-1035G1).

## Resultados no ego-Facebook

Grafo conexo (1 componente), densidade 1,08%, grau médio 43,69, grau mínimo 1 e máximo 1.045.

| # | Grau | Closeness | Betweenness (norm.) |
|---|---|---|---|
| 1 | 107 (1045) | 107 (0,4597) | 107 (0,4805) |
| 2 | 1684 (792) | 58 (0,3974) | 1684 (0,3378) |
| 3 | 1912 (755) | 428 (0,3948) | 3437 (0,2361) |
| 4 | 3437 (547) | 563 (0,3939) | 1912 (0,2293) |
| 5 | 0 (347) | 1684 (0,3936) | 1085 (0,1490) |

O vértice 107 lidera as três métricas. No plot, os vértices de maior betweenness aparecem como pontes entre as comunidades. Busca do item F: tabela hash ~1–2 ns por busca; busca sequencial ~40 ns (ID no início) a ~1.440 ns (ID inexistente).

## Tempos no notebook (i5-1035G1)

Média de 3 execuções (item G), no ego-Facebook. Medidos na VM Linux do app Claude no notebook (2 vCPU, Ubuntu 22.04, gcc 11.4); arquivo `resultados/tempos_notebook_vm_linux.txt`.

| Métrica | Complexidade | `-O2` | `-O0` |
|---|---|---|---|
| Leitura e construção | O(V + E) | 0,012 s | 0,026 s |
| Grau | O(V) | 0,00002 s | 0,00003 s |
| Closeness | O(V(V+E)) | 1,06 s | 3,28 s |
| Betweenness | O(V·E) | 2,73 s | 8,80 s |
| Ordenação (merge sort) | O(V log V) | 0,0008 s | 0,0013 s |

Para os tempos nativos no Windows 11 (gcc 13.2, padrão das outras práticas), basta dar dois cliques em `medir_tempos_windows.bat`.

## Análise da rede: o que os dados significam

**O que é o grafo.** São 10 usuários do Facebook que responderam a uma pesquisa em 2012 (os *egos*: 0, 107, 348, 414, 686, 698, 1684, 1912, 3437 e 3980), os amigos deles e as amizades entre esses amigos (McAuley e Leskovec, 2012). Cada vértice é um usuário anonimizado e cada aresta é uma amizade. É a união de 10 redes pessoais, não uma amostra aleatória do Facebook: o grau é o número de amigos *dentro dessas redes*, e os egos têm grau alto por construção.

**Principais conclusões** (`resultados/analise_rede.txt`, figura `resultados/grafo_egos.png`):
- A rede são 10 grupos quase isolados, um por ego. 97,2% das pessoas são amigas de um único ego; só 113 são amigas de 2 ou mais. O Louvain acha 16 comunidades (modularidade 0,835), 14 delas com 97% ou mais de vértices de um só ego.
- **Grau** aponta os populares de cada grupo: os 5 primeiros são egos, e os 5 seguintes têm mais de 230 amigos dentro de uma única rede ego.
- **Closeness** aponta quem está perto de vários grupos. O 58 tem 12 amigos, mas é amigo de 4 egos e é o 2º em closeness.
- **Betweenness** aponta as pontes. Egos e pontes (3% dos vértices) concentram 93,6% do betweenness. O 1085 é o *único* amigo em comum entre 107 e 3437.
- **Remoção (opção H):** tirar os 10 de maior betweenness separa 20% da rede (as redes do 3437 e do 686); tirar os 10 de maior grau, 2%; tirar 10 aleatórios, nada.
- **Mundo pequeno:** distância média de 3,69, diâmetro 8 e agrupamento 0,606, 56 vezes o de um grafo aleatório de mesma densidade.
