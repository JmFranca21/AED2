"""
AED2 - Trabalho 3 - Analise da rede ego-Facebook (complementar ao programa em C)
Joao Marcelo Franca Batista

O programa em C calcula as metricas; este script usa a saida dele para
interpretar a rede a partir de como os dados foram coletados.

Como a ego-Facebook foi montada (documentacao do SNAP / McAuley e Leskovec,
2012): 10 usuarios responderam a uma pesquisa por um aplicativo do Facebook.
Para cada um deles (o "ego") foram coletados os amigos e as amizades entre
esses amigos. O arquivo facebook_combined.txt e a uniao dessas 10 "redes ego",
com o ego ligado a todos os seus amigos. Os IDs dos egos sao os nomes dos
arquivos .edges do dataset original: 0, 107, 348, 414, 686, 698, 1684, 1912,
3437 e 3980.

Entradas (geradas pelo programa em C, opcao B ou --auto):
  grafo_vertices.csv, grafo_arestas.csv, grafo_layout.csv (do plot)
Saidas:
  analise_rede.txt        -> numeros usados no relatorio
  grafo_egos.png          -> grafo colorido pela rede de cada ego, pontes em preto
  distribuicao_grau.png   -> distribuicao do grau (escala log-log)

Uso: python joaomarcelo_francabatista_analise.py
Requer: pip install networkx matplotlib numpy
"""
import collections
import csv
import itertools

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import networkx as nx
import numpy as np

EGOS = [0, 107, 348, 414, 686, 698, 1684, 1912, 3437, 3980]
saida = []


def p(*args):
    linha = " ".join(str(a) for a in args)
    print(linha)
    saida.append(linha)


def ler():
    G = nx.Graph()
    met = {}
    with open("grafo_vertices.csv", newline="") as f:
        for r in csv.DictReader(f):
            v = int(r["Id"])
            G.add_node(v)
            met[v] = {k: float(r[k]) for k in ("grau", "closeness", "dist_media",
                                                 "betweenness", "betweenness_norm")}
    with open("grafo_arestas.csv", newline="") as f:
        for r in csv.DictReader(f):
            G.add_edge(int(r["Source"]), int(r["Target"]))
    pos = {}
    with open("grafo_layout.csv", newline="") as f:
        for r in csv.DictReader(f):
            pos[int(r["Id"])] = (float(r["x"]), float(r["y"]))
    return G, met, pos


def main():
    G, met, pos = ler()
    n = G.number_of_nodes()
    nao_egos = [v for v in G if v not in EGOS]
    egos_de = {v: [e for e in EGOS if G.has_edge(v, e)] for v in nao_egos}

    p("=== 1. As 10 redes ego ===")
    p("ego | amigos no grafo | amigos so dele | amigos em comum com outros egos")
    for e in EGOS:
        amigos = [v for v in G[e] if v not in EGOS]
        exclusivos = sum(1 for v in amigos if len(egos_de[v]) == 1)
        p(f"{e:5d} | {G.degree(e):5d} | {exclusivos:5d} | {len(amigos) - exclusivos:4d}")
    p("amizades diretas entre egos:",
      [(a, b) for a, b in itertools.combinations(EGOS, 2) if G.has_edge(a, b)])
    sem = sum(1 for v in nao_egos if not egos_de[v])
    um = sum(1 for v in nao_egos if len(egos_de[v]) == 1)
    varios = [v for v in nao_egos if len(egos_de[v]) >= 2]
    p(f"nao-egos: {len(nao_egos)} | amigos de nenhum ego: {sem} | de 1 ego: {um} "
      f"({100 * um / len(nao_egos):.1f}%) | de 2+ egos: {len(varios)}")
    p("pares de egos com amigos em comum:")
    for a, b in itertools.combinations(EGOS, 2):
        k = len((set(G[a]) & set(G[b])) - set(EGOS))
        if k:
            p(f"   {a}-{b}: {k}")

    p("\n=== 2. Comunidades (Louvain, NetworkX) x redes ego ===")
    com = nx.community.louvain_communities(G, seed=42)
    p(f"{len(com)} comunidades, modularidade {nx.community.modularity(G, com):.3f}")

    def dono(v):
        if v in EGOS:
            return v
        return egos_de[v][0] if len(egos_de[v]) == 1 else "varios"

    for c in sorted(com, key=len, reverse=True):
        cont = collections.Counter(dono(v) for v in c)
        d, k = cont.most_common(1)[0]
        p(f"   {len(c):4d} vertices: {100 * k / len(c):5.1f}% da rede do ego {d}  {dict(cont.most_common(3))}")

    p("\n=== 3. Vertices centrais e de quais egos sao amigos ===")
    for nome in ("grau", "closeness", "betweenness"):
        top = sorted(met, key=lambda v: -met[v][nome])[:10]
        p(nome + ":")
        for v in top:
            papel = "EGO" if v in EGOS else f"amigo de {egos_de[v]}"
            p(f"   {v:5d}  grau {int(met[v]['grau']):4d}  {papel}")
    soma_bet = sum(met[v]["betweenness"] for v in G)
    bet_pontes = sum(met[v]["betweenness"] for v in varios)
    bet_egos = sum(met[v]["betweenness"] for v in EGOS)
    p(f"betweenness total: egos {100 * bet_egos / soma_bet:.1f}% | pontes (amigos de 2+ egos, "
      f"{len(varios)} vertices) {100 * bet_pontes / soma_bet:.1f}% | demais {100 - 100 * (bet_egos + bet_pontes) / soma_bet:.1f}%")

    p("\n=== 4. Distribuicao do grau e mundo pequeno ===")
    deg = np.array([d for _, d in G.degree()])
    p(f"grau <= 10: {100 * (deg <= 10).mean():.1f}% | <= 25: {100 * (deg <= 25).mean():.1f}% | "
      f">= 100: {(deg >= 100).sum()} | >= 200: {(deg >= 200).sum()}")
    dens = 2 * G.number_of_edges() / (n * (n - 1))
    cl = nx.average_clustering(G)
    p(f"agrupamento medio {cl:.4f} | esperado num grafo aleatorio de mesma densidade {dens:.4f} "
      f"| razao {cl / dens:.1f}x")
    p(f"triangulos: {sum(nx.triangles(G).values()) // 3}")
    p(f"distancia media (media das dist_media do C): {np.mean([met[v]['dist_media'] for v in G]):.3f}")

    with open("analise_rede.txt", "w") as f:
        f.write("\n".join(saida) + "\n")

    # ---------- Figura: grafo colorido pela rede de cada ego ----------
    cores = plt.get_cmap("tab10").colors
    cor_ego = {e: cores[i] for i, e in enumerate(EGOS)}
    fig, ax = plt.subplots(figsize=(11, 9))
    nx.draw_networkx_edges(G, pos, ax=ax, width=0.3, alpha=0.06, edge_color="#555555")
    simples = [v for v in nao_egos if len(egos_de[v]) == 1]
    nx.draw_networkx_nodes(G, pos, nodelist=simples, ax=ax, node_size=4, linewidths=0,
                           node_color=[cor_ego[egos_de[v][0]] for v in simples])
    nx.draw_networkx_nodes(G, pos, nodelist=varios, ax=ax, node_size=14, linewidths=0,
                           node_color="black")
    nx.draw_networkx_nodes(G, pos, nodelist=EGOS, ax=ax, node_size=220, node_shape="*",
                           node_color=[cor_ego[e] for e in EGOS], edgecolors="black", linewidths=0.8)
    deslocamento = {0: (-48, -16)}   # evita sobrepor o rotulo do ego 1684
    for e in EGOS:
        ax.annotate(f"ego {e}", pos[e], fontsize=9, fontweight="bold",
                    xytext=deslocamento.get(e, (6, 6)), textcoords="offset points")
    for v in (58, 1085, 567):
        ax.annotate(str(v), pos[v], fontsize=9, color="#b00000", fontweight="bold",
                    xytext=(5, -10), textcoords="offset points")
    from matplotlib.lines import Line2D
    leg = [Line2D([0], [0], marker="o", color="w", markerfacecolor=cor_ego[e], markersize=7,
                  label=f"amigos do ego {e}") for e in EGOS]
    leg.append(Line2D([0], [0], marker="o", color="w", markerfacecolor="black", markersize=7,
                      label="amigos de 2 ou mais egos (pontes)"))
    ax.legend(handles=leg, loc="lower left", fontsize=8, frameon=False, ncol=2)
    ax.set_title("ego-Facebook: cada cor é a rede de amigos de um dos 10 usuários pesquisados")
    ax.set_aspect("equal")
    ax.axis("off")
    fig.tight_layout()
    fig.savefig("grafo_egos.png", dpi=200, bbox_inches="tight", pad_inches=0.05)

    # ---------- Figura: distribuicao do grau (acumulada, log-log) ----------
    fig2, ax2 = plt.subplots(figsize=(6, 4.2))
    ks = np.sort(deg)
    frac = 1.0 - np.arange(len(ks)) / len(ks)      # fracao com grau >= k
    ax2.loglog(ks, frac, "-", color="#1f77b4", lw=1.6, label="todos os vértices")
    for e in EGOS:
        k = G.degree(e)
        ax2.loglog([k], [(deg >= k).mean()], "*", color="#d62728", markersize=10)
        if k >= 300:
            ax2.annotate(str(e), (k, (deg >= k).mean()), fontsize=7, xytext=(4, 3),
                         textcoords="offset points")
    ax2.plot([], [], "*", color="#d62728", markersize=10, label="os 10 egos")
    ax2.axvline(np.median(deg), color="gray", lw=0.8, ls="--")
    ax2.annotate(f"mediana = {int(np.median(deg))}", (np.median(deg), 0.02), fontsize=8,
                 xytext=(4, 0), textcoords="offset points", color="gray")
    ax2.set_xlabel("k (amigos dentro da rede)")
    ax2.set_ylabel("fração de vértices com grau ≥ k")
    ax2.set_title("Distribuição acumulada do grau", fontsize=10)
    ax2.legend(fontsize=8, frameon=False)
    fig2.tight_layout()
    fig2.savefig("distribuicao_grau.png", dpi=200)
    p("\nGravados analise_rede.txt, grafo_egos.png e distribuicao_grau.png")


if __name__ == "__main__":
    main()
