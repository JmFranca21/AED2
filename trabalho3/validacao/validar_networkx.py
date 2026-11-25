"""Confere grafo_vertices.csv (saida do programa em C) contra o NetworkX.

Uso: rode o programa com --auto no arquivo e depois
    python validar_networkx.py <arquivo de arestas>
Imprime o erro maximo de cada metrica e o tempo que o NetworkX levou.
"""
import csv
import sys
import time

import networkx as nx

arquivo = sys.argv[1]
G = nx.Graph()
with open(arquivo) as f:
    for linha in f:
        if linha.startswith(("#", "%")) or not linha.strip():
            continue
        a, b = linha.split()[:2]
        G.add_node(int(a)); G.add_node(int(b))
        if a != b:
            G.add_edge(int(a), int(b))

t = {}
t0 = time.perf_counter(); deg = nx.degree_centrality(G); t["grau"] = time.perf_counter() - t0
t0 = time.perf_counter(); clo = nx.closeness_centrality(G); t["closeness"] = time.perf_counter() - t0
t0 = time.perf_counter(); bet = nx.betweenness_centrality(G, normalized=True); t["betweenness"] = time.perf_counter() - t0

c = {}
with open("grafo_vertices.csv") as f:
    for r in csv.DictReader(f):
        c[int(r["Id"])] = r

assert set(c) == set(G.nodes()), "conjunto de vertices diferente"
erro = {"grau": 0.0, "closeness": 0.0, "betweenness": 0.0}
for v in G.nodes():
    erro["grau"] = max(erro["grau"], abs(float(c[v]["grau_norm"]) - deg[v]))
    erro["closeness"] = max(erro["closeness"], abs(float(c[v]["closeness"]) - clo[v]))
    erro["betweenness"] = max(erro["betweenness"], abs(float(c[v]["betweenness_norm"]) - bet[v]))

print(f"n = {G.number_of_nodes()}, m = {G.number_of_edges()}")
for k in erro:
    print(f"{k:12s} erro maximo = {erro[k]:.2e}   tempo NetworkX = {t[k]:.3f} s")
top = sorted(bet, key=bet.get, reverse=True)[:5]
print("top-5 betweenness NetworkX:", top)
