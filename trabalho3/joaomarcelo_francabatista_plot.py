"""
AED2 - Trabalho 3 - Plot do grafo (item B)
Joao Marcelo Franca Batista

Le os arquivos gerados pelo programa em C (opcao B ou modo --auto):
  grafo_vertices.csv  -> Id, grau, closeness, betweenness... (metricas calculadas em C)
  grafo_arestas.csv   -> Source, Target
e desenha o grafo com NetworkX + matplotlib. As metricas NAO sao recalculadas
aqui: o tamanho e a cor dos vertices usam os valores calculados pelo programa.

Saidas:
  grafo_plot.png      -> grafo com tamanho = betweenness e cor = closeness
  grafo_metricas.png  -> tres paineis, um por metrica
O layout (Fruchterman-Reingold / spring_layout) e salvo em grafo_layout.csv e
reaproveitado nas proximas execucoes, porque e a parte mais lenta do plot.

Uso: python joaomarcelo_francabatista_plot.py [--sem-janela]
Requer: pip install networkx matplotlib scipy
"""
import csv
import os
import sys

import matplotlib

if "--sem-janela" in sys.argv:
    matplotlib.use("Agg")
import matplotlib.pyplot as plt
import networkx as nx

ARQ_VERTICES = "grafo_vertices.csv"
ARQ_ARESTAS = "grafo_arestas.csv"
ARQ_LAYOUT = "grafo_layout.csv"


def ler_grafo():
    G = nx.Graph()
    metricas = {}
    with open(ARQ_VERTICES, newline="") as f:
        for r in csv.DictReader(f):
            v = int(r["Id"])
            G.add_node(v)
            metricas[v] = {k: float(r[k]) for k in ("grau", "closeness", "betweenness_norm")}
    with open(ARQ_ARESTAS, newline="") as f:
        for r in csv.DictReader(f):
            G.add_edge(int(r["Source"]), int(r["Target"]))
    return G, metricas


def calcular_layout(G):
    if os.path.exists(ARQ_LAYOUT):
        pos = {}
        with open(ARQ_LAYOUT, newline="") as f:
            for r in csv.DictReader(f):
                pos[int(r["Id"])] = (float(r["x"]), float(r["y"]))
        if set(pos) == set(G.nodes()):
            print("Layout reaproveitado de", ARQ_LAYOUT)
            return pos
    print(f"Calculando o layout de {G.number_of_nodes()} vertices (pode levar alguns segundos)...")
    pos = nx.spring_layout(G, seed=42, iterations=60, k=1.5 / max(1, G.number_of_nodes()) ** 0.5)
    with open(ARQ_LAYOUT, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["Id", "x", "y"])
        for v, (x, y) in pos.items():
            w.writerow([v, x, y])
    return pos


def tamanhos(G, valores, minimo, maximo):
    # escala min-max: o menor valor vira o menor tamanho e o maior, o maior
    vmin = min(valores[v] for v in G.nodes())
    vmax = max(valores[v] for v in G.nodes())
    faixa = (vmax - vmin) or 1.0
    return [minimo + (maximo - minimo) * ((valores[v] - vmin) / faixa) for v in G.nodes()]


def desenhar(ax, G, pos, cores, tam, titulo, cmap):
    n = G.number_of_nodes()
    nx.draw_networkx_edges(G, pos, ax=ax, width=0.3 if n > 500 else 0.8,
                           alpha=0.08 if n > 500 else 0.4, edge_color="#555555")
    nos = nx.draw_networkx_nodes(G, pos, ax=ax, node_size=tam, node_color=cores,
                                 cmap=cmap, linewidths=0)
    if n <= 60:
        nx.draw_networkx_labels(G, pos, ax=ax, font_size=8)
    ax.set_title(titulo, fontsize=11)
    ax.set_aspect("equal")
    ax.axis("off")
    return nos


def main():
    if not (os.path.exists(ARQ_VERTICES) and os.path.exists(ARQ_ARESTAS)):
        print("Arquivos CSV nao encontrados. Rode o programa em C (opcao B) primeiro.")
        sys.exit(1)
    G, met = ler_grafo()
    n_total = G.number_of_nodes()
    nota = ""
    if n_total > 500 and not nx.is_connected(G):
        # em grafos grandes e desconexos, vertices isolados e componentes
        # pequenas ficam longe do resto e "achatam" o desenho
        maior = max(nx.connected_components(G), key=len)
        G = G.subgraph(maior).copy()
        nota = f" (maior componente: {G.number_of_nodes()} de {n_total} vertices)"
        print("Desenhando apenas a maior componente conexa" + nota)
    pos = calcular_layout(G)
    n, m = G.number_of_nodes(), G.number_of_edges()
    grande = n > 500

    grau = {v: met[v]["grau"] for v in G}
    clo = {v: met[v]["closeness"] for v in G}
    bet = {v: met[v]["betweenness_norm"] for v in G}

    # Figura principal: tamanho = betweenness, cor = closeness
    fig, ax = plt.subplots(figsize=(11, 9))
    tam = tamanhos(G, bet, 3 if grande else 60, 400 if grande else 1200)
    nos = desenhar(ax, G, pos, [clo[v] for v in G], tam,
                   f"Grafo (n = {n}, m = {m}){nota}\ntamanho = betweenness, cor = closeness", "viridis")
    fig.colorbar(nos, ax=ax, shrink=0.7, label="closeness")
    top = sorted(bet, key=bet.get, reverse=True)[:5]
    for v in top:
        ax.annotate(str(v), pos[v], fontsize=9, fontweight="bold", color="#b00000",
                    xytext=(4, 4), textcoords="offset points")
    fig.tight_layout()
    fig.savefig("grafo_plot.png", dpi=200)
    print("Gravado grafo_plot.png (top 5 de betweenness em vermelho:", top, ")")

    # Tres paineis, um por metrica (para o relatorio)
    fig2, axs = plt.subplots(1, 3, figsize=(18, 6.5))
    for ax2, (nome, val, cmap) in zip(axs, [("Grau", grau, "plasma"),
                                            ("Closeness", clo, "viridis"),
                                            ("Betweenness (normalizado)", bet, "inferno")]):
        nos2 = desenhar(ax2, G, pos, [val[v] for v in G],
                        tamanhos(G, val, 2 if grande else 40, 150 if grande else 700), nome, cmap)
        fig2.colorbar(nos2, ax=ax2, shrink=0.6, orientation="horizontal", pad=0.02)
    fig2.tight_layout()
    fig2.savefig("grafo_metricas.png", dpi=200)
    print("Gravado grafo_metricas.png")

    if "--sem-janela" not in sys.argv:
        plt.show()


if __name__ == "__main__":
    main()
