/*
 * AED2 - Trabalho 3 - Analise de dados em formato de grafos
 * Joao Marcelo Franca Batista
 *
 * O programa le uma rede (lista de arestas no formato do SNAP: uma aresta
 * "u v" por linha, linhas com '#' ou '%' sao comentarios), monta um grafo
 * NAO DIRIGIDO e NAO PONDERADO e calcula tres medidas de centralidade para
 * todos os vertices:
 *
 *   - Grau: numero de arestas incidentes no vertice.
 *       grau_norm(v) = grau(v) / (n - 1)
 *
 *   - Closeness (proximidade): baseada na distancia media de v ate os demais.
 *       dist_media(v) = soma das distancias de v ate os vertices alcancaveis
 *                       / (r - 1), onde r = vertices alcancaveis (contando v)
 *       closeness(v)  = (r - 1) / soma das distancias, multiplicado por
 *                       (r - 1) / (n - 1) (correcao de Wasserman e Faust para
 *                       grafos desconexos; em grafo conexo r = n e o fator e 1)
 *     Quanto MENOR a distancia media, MAIOR o closeness (mais central).
 *     As distancias sao calculadas com uma busca em largura (BFS) a partir de
 *     cada vertice: como as arestas nao tem peso, a BFS ja da o caminho minimo.
 *
 *   - Betweenness (intermediacao): para cada par (s, t), fracao dos caminhos
 *     minimos de s a t que passam por v, somada sobre todos os pares.
 *     Calculada com o algoritmo de Brandes (2001): uma BFS por vertice conta
 *     os caminhos minimos (sigma) e, percorrendo os vertices em ordem
 *     decrescente de distancia, acumula a "dependencia" (delta) de cada um.
 *     Da o mesmo resultado da definicao, mas em O(V*E) em vez de O(V^3).
 *       betweenness_norm(v) = betweenness(v) / ((n - 1)(n - 2) / 2)
 *
 * Itens do enunciado (menu):
 *   A) le o arquivo e gera o grafo
 *   B) plot do grafo (exporta CSV e chama o script Python/NetworkX; os mesmos
 *      CSV podem ser abertos no Gephi)
 *   C) lista do grau de todos os vertices em ordem crescente
 *   D) lista do closeness de todos os vertices em ordem crescente
 *   E) lista do betweenness de todos os vertices em ordem crescente
 *   F) pesquisa um vertice (ou uma aresta) e informa se foi encontrado
 *   G) tempo de execucao das metricas
 * As listas completas sao gravadas em ranking_grau.txt, ranking_closeness.txt
 * e ranking_betweenness.txt; na tela aparecem o inicio, o fim e o top 10.
 *
 * Estruturas de dados (TADs):
 *   - Grafo: LISTA DE ADJACENCIA, implementada com um vetor dinamico por
 *     vertice (dobra de tamanho quando enche). Redes reais sao esparsas (no
 *     ego-Facebook a densidade e ~1%), entao a matriz de adjacencia gastaria
 *     memoria O(V^2) quase toda com zeros e cada BFS custaria O(V^2).
 *   - Tabela hash (enderecamento aberto, sondagem linear, metodo da
 *     multiplicacao): converte o ID que aparece no arquivo (que pode nao ser
 *     sequencial) no indice interno 0..n-1. Tambem e usada na busca (item F).
 *   - Merge sort (estavel) para ordenar os rankings; empates sao desfeitos
 *     pelo ID do vertice.
 *
 * Arestas repetidas e lacos (u u) sao ignorados. Se o arquivo for de um grafo
 * dirigido, cada arco vira uma aresta nao dirigida.
 *
 * Uso:
 *   gcc joaomarcelo_francabatista_algoritmo.c -o trabalho3 -O2
 *   ./trabalho3 [arquivo]           (menu interativo)
 *   ./trabalho3 [arquivo] --auto    (roda tudo sem menu e exporta os CSV)
 * Sem argumento, le "joaomarcelo_francabatista_base.txt".
 *
 * Fontes consultadas (algoritmos classicos, implementados a partir delas):
 *  - U. Brandes, "A faster algorithm for betweenness centrality", Journal of
 *    Mathematical Sociology, 25(2), 2001.
 *  - L. C. Freeman, "Centrality in social networks: conceptual
 *    clarification", Social Networks, 1(3), 1978.
 *  - S. Wasserman e K. Faust, Social Network Analysis, Cambridge, 1994.
 *  - Cormen et al., Introduction to Algorithms, cap. 11 (hash) e 22 (BFS).
 *  - N. Ziviani, Projeto de Algoritmos (grafos, hash, ordenacao).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ARQUIVO_PADRAO "joaomarcelo_francabatista_base.txt"
#define LIMITE_TELA 60       /* ate esse n a lista inteira e impressa na tela */
#define PONTAS_TELA 15       /* acima disso: 15 primeiros e 15 ultimos */
#define TOP_K 10             /* top 10 do maior para o menor */
#define REPETICOES_TEMPO 3   /* repeticoes de cada metrica no item G */
#define MAX_LINHA 4096

/* ======================================================================
 * Tempo
 * ====================================================================== */
double agora(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (double)t.tv_sec + (double)t.tv_nsec * 1e-9;
}

/* ======================================================================
 * TAD Tabela Hash: ID original (long long) -> indice interno (int)
 * Enderecamento aberto com sondagem linear. Tamanho potencia de 2 e funcao
 * hash pelo metodo da multiplicacao (constante de Knuth, 2^64 / phi).
 * Fator de carga mantido <= 0,5: ao passar disso, a tabela dobra (rehash).
 * ====================================================================== */
typedef struct {
    long long chave;
    int valor;
    char ocupado;
} Slot;

typedef struct {
    Slot *t;
    int bits;        /* capacidade = 2^bits */
    int cap;
    int qtd;
} Hash;

static unsigned int hashPosicao(const Hash *h, long long chave) {
    unsigned long long x = (unsigned long long)chave * 11400714819323198485ULL;
    return (unsigned int)(x >> (64 - h->bits));
}

Hash *hashCriar(int bits) {
    Hash *h = malloc(sizeof(Hash));
    h->bits = bits;
    h->cap = 1 << bits;
    h->qtd = 0;
    h->t = calloc((size_t)h->cap, sizeof(Slot));
    return h;
}

void hashLiberar(Hash *h) {
    free(h->t);
    free(h);
}

/* devolve o indice associado a chave, ou -1 se ela nao estiver na tabela */
int hashBuscar(const Hash *h, long long chave) {
    unsigned int i = hashPosicao(h, chave);
    while (h->t[i].ocupado) {
        if (h->t[i].chave == chave) return h->t[i].valor;
        i = (i + 1) & (unsigned int)(h->cap - 1);   /* sondagem linear */
    }
    return -1;
}

static void hashInserirSemCrescer(Hash *h, long long chave, int valor) {
    unsigned int i = hashPosicao(h, chave);
    while (h->t[i].ocupado) {
        if (h->t[i].chave == chave) { h->t[i].valor = valor; return; }
        i = (i + 1) & (unsigned int)(h->cap - 1);
    }
    h->t[i].chave = chave;
    h->t[i].valor = valor;
    h->t[i].ocupado = 1;
    h->qtd++;
}

static void hashDobrar(Hash *h) {
    Slot *antiga = h->t;
    int capAntiga = h->cap;
    h->bits++;
    h->cap = 1 << h->bits;
    h->qtd = 0;
    h->t = calloc((size_t)h->cap, sizeof(Slot));
    for (int i = 0; i < capAntiga; i++)
        if (antiga[i].ocupado) hashInserirSemCrescer(h, antiga[i].chave, antiga[i].valor);
    free(antiga);
}

void hashInserir(Hash *h, long long chave, int valor) {
    if ((h->qtd + 1) * 2 > h->cap) hashDobrar(h);   /* fator de carga <= 0,5 */
    hashInserirSemCrescer(h, chave, valor);
}

/* ======================================================================
 * TAD Grafo: lista de adjacencia (vetor dinamico de vizinhos por vertice)
 * ====================================================================== */
typedef struct {
    int n;                  /* numero de vertices */
    long long m;            /* numero de arestas (nao dirigidas) */
    int capVertices;        /* capacidade alocada para vertices */
    int **viz;              /* viz[v][0..grau[v]-1] = vizinhos de v */
    int *grau;
    int *capViz;            /* capacidade do vetor de vizinhos de cada vertice */
    long long *idOriginal;  /* indice interno -> ID do arquivo */
    Hash *mapa;             /* ID do arquivo -> indice interno */
    long long linhasLidas, lacosIgnorados, repetidasIgnoradas, linhasInvalidas;
} Grafo;

Grafo *grafoCriar(void) {
    Grafo *g = calloc(1, sizeof(Grafo));
    g->capVertices = 1024;
    g->viz = malloc((size_t)g->capVertices * sizeof(int *));
    g->grau = malloc((size_t)g->capVertices * sizeof(int));
    g->capViz = malloc((size_t)g->capVertices * sizeof(int));
    g->idOriginal = malloc((size_t)g->capVertices * sizeof(long long));
    g->mapa = hashCriar(11);
    return g;
}

void grafoLiberar(Grafo *g) {
    if (!g) return;
    for (int v = 0; v < g->n; v++) free(g->viz[v]);
    free(g->viz);
    free(g->grau);
    free(g->capViz);
    free(g->idOriginal);
    hashLiberar(g->mapa);
    free(g);
}

/* devolve o indice do vertice com esse ID; cria o vertice se ainda nao existe */
int obterVertice(Grafo *g, long long id) {
    int v = hashBuscar(g->mapa, id);
    if (v >= 0) return v;

    if (g->n == g->capVertices) {   /* dobra os vetores de vertices */
        g->capVertices *= 2;
        g->viz = realloc(g->viz, (size_t)g->capVertices * sizeof(int *));
        g->grau = realloc(g->grau, (size_t)g->capVertices * sizeof(int));
        g->capViz = realloc(g->capViz, (size_t)g->capVertices * sizeof(int));
        g->idOriginal = realloc(g->idOriginal, (size_t)g->capVertices * sizeof(long long));
    }
    v = g->n++;
    g->grau[v] = 0;
    g->capViz[v] = 4;
    g->viz[v] = malloc(4 * sizeof(int));
    g->idOriginal[v] = id;
    hashInserir(g->mapa, id, v);
    return v;
}

/* percorre a lista do vertice de menor grau: O(min(grau(u), grau(v))) */
int existeAresta(const Grafo *g, int u, int v) {
    if (g->grau[u] > g->grau[v]) { int t = u; u = v; v = t; }
    for (int i = 0; i < g->grau[u]; i++)
        if (g->viz[u][i] == v) return 1;
    return 0;
}

static void adicionarVizinho(Grafo *g, int u, int v) {
    if (g->grau[u] == g->capViz[u]) {
        g->capViz[u] *= 2;
        g->viz[u] = realloc(g->viz[u], (size_t)g->capViz[u] * sizeof(int));
    }
    g->viz[u][g->grau[u]++] = v;
}

/* grafo nao dirigido: guarda v na lista de u e u na lista de v.
 * Devolve 1 se inseriu, 0 se era laco ou aresta repetida. */
int inserirAresta(Grafo *g, int u, int v) {
    if (u == v) { g->lacosIgnorados++; return 0; }
    if (existeAresta(g, u, v)) { g->repetidasIgnoradas++; return 0; }
    adicionarVizinho(g, u, v);
    adicionarVizinho(g, v, u);
    g->m++;
    return 1;
}

/* le dois inteiros da linha (separados por espaco, tab, virgula ou ';');
 * colunas extras (ex.: peso, tempo) sao ignoradas */
static int lerDoisInteiros(const char *s, long long *a, long long *b) {
    char *fim;
    while (*s == ' ' || *s == '\t') s++;
    if (*s == '#' || *s == '%' || *s == '\n' || *s == '\r' || *s == '\0') return -1;
    *a = strtoll(s, &fim, 10);
    if (fim == s) return 0;
    s = fim;
    while (*s == ' ' || *s == '\t' || *s == ',' || *s == ';') s++;
    *b = strtoll(s, &fim, 10);
    if (fim == s) return 0;
    return 1;
}

/* Item A: le o arquivo e monta o grafo */
Grafo *lerArquivo(const char *caminho) {
    FILE *f = fopen(caminho, "r");
    if (!f) return NULL;
    Grafo *g = grafoCriar();
    char linha[MAX_LINHA];
    while (fgets(linha, sizeof(linha), f)) {
        long long a, b;
        int r = lerDoisInteiros(linha, &a, &b);
        if (r < 0) continue;                       /* comentario ou linha vazia */
        if (r == 0) { g->linhasInvalidas++; continue; }
        g->linhasLidas++;
        int u = obterVertice(g, a);
        int v = obterVertice(g, b);
        inserirAresta(g, u, v);
    }
    fclose(f);
    return g;
}

/* ======================================================================
 * Busca em largura (BFS)
 * Preenche dist[] (passos a partir de s; -1 = inalcancavel) e fila[] com os
 * vertices na ordem em que foram visitados (ordem nao decrescente de
 * distancia). Devolve quantos vertices foram alcancados (incluindo s).
 * ====================================================================== */
int bfs(const Grafo *g, int s, int *dist, int *fila) {
    for (int v = 0; v < g->n; v++) dist[v] = -1;
    int ini = 0, fim = 0;
    dist[s] = 0;
    fila[fim++] = s;
    while (ini < fim) {
        int v = fila[ini++];
        for (int i = 0; i < g->grau[v]; i++) {
            int w = g->viz[v][i];
            if (dist[w] < 0) {
                dist[w] = dist[v] + 1;
                fila[fim++] = w;
            }
        }
    }
    return fim;
}

int contarComponentes(const Grafo *g, int *maior) {
    int *dist = malloc((size_t)g->n * sizeof(int));
    int *fila = malloc((size_t)g->n * sizeof(int));
    char *visto = calloc((size_t)g->n, 1);
    int comp = 0;
    *maior = 0;
    for (int s = 0; s < g->n; s++) {
        if (visto[s]) continue;
        int r = bfs(g, s, dist, fila);
        for (int i = 0; i < r; i++) visto[fila[i]] = 1;
        if (r > *maior) *maior = r;
        comp++;
    }
    free(dist); free(fila); free(visto);
    return comp;
}

/* ======================================================================
 * Metricas de centralidade
 * ====================================================================== */
typedef struct {
    double *grau, *grauNorm;
    double *closeness, *distMedia;
    double *betweenness, *betweennessNorm;
    int temGrau, temCloseness, temBetweenness;
    double tGrau, tCloseness, tBetweenness, tOrdenacao;
} Metricas;

Metricas *metricasCriar(int n) {
    Metricas *M = calloc(1, sizeof(Metricas));
    M->grau = malloc((size_t)n * sizeof(double));
    M->grauNorm = malloc((size_t)n * sizeof(double));
    M->closeness = malloc((size_t)n * sizeof(double));
    M->distMedia = malloc((size_t)n * sizeof(double));
    M->betweenness = malloc((size_t)n * sizeof(double));
    M->betweennessNorm = malloc((size_t)n * sizeof(double));
    return M;
}

void metricasLiberar(Metricas *M) {
    if (!M) return;
    free(M->grau); free(M->grauNorm);
    free(M->closeness); free(M->distMedia);
    free(M->betweenness); free(M->betweennessNorm);
    free(M);
}

/* Grau: O(V), basta ler o tamanho da lista de cada vertice */
void calcularGrau(const Grafo *g, double *grau, double *grauNorm) {
    for (int v = 0; v < g->n; v++) {
        grau[v] = g->grau[v];
        grauNorm[v] = g->n > 1 ? (double)g->grau[v] / (g->n - 1) : 0.0;
    }
}

/* Closeness: uma BFS por vertice -> O(V * (V + E)) */
void calcularCloseness(const Grafo *g, double *closeness, double *distMedia) {
    int n = g->n;
    int *dist = malloc((size_t)n * sizeof(int));
    int *fila = malloc((size_t)n * sizeof(int));
    for (int s = 0; s < n; s++) {
        int r = bfs(g, s, dist, fila);   /* r = alcancados, incluindo s */
        long long soma = 0;
        for (int i = 0; i < r; i++) soma += dist[fila[i]];
        if (r > 1 && soma > 0) {
            distMedia[s] = (double)soma / (r - 1);
            closeness[s] = ((double)(r - 1) / soma) * ((double)(r - 1) / (n - 1));
        } else {                          /* vertice isolado */
            distMedia[s] = 0.0;
            closeness[s] = 0.0;
        }
    }
    free(dist);
    free(fila);
}

/* Betweenness: algoritmo de Brandes -> O(V * E) em grafo nao ponderado.
 * Para cada origem s:
 *   1) BFS a partir de s contando sigma[w] = numero de caminhos minimos s->w
 *      (sigma[w] soma sigma[v] de todo v vizinho com dist[v] = dist[w] - 1);
 *   2) percorre os vertices da BFS de tras para frente (do mais distante para
 *      o mais proximo) acumulando a dependencia:
 *        delta[v] += sigma[v] / sigma[w] * (1 + delta[w])
 *      para cada v vizinho de w que esta um nivel antes (predecessor de w);
 *   3) soma delta[w] em betweenness[w] (w != s).
 * Como cada par (s, t) e contado duas vezes (s->t e t->s), divide por 2.
 * sigma e double porque o numero de caminhos minimos pode estourar um int. */
void calcularBetweenness(const Grafo *g, double *bc, double *bcNorm) {
    int n = g->n;
    int *dist = malloc((size_t)n * sizeof(int));
    int *fila = malloc((size_t)n * sizeof(int));
    double *sigma = malloc((size_t)n * sizeof(double));
    double *delta = malloc((size_t)n * sizeof(double));
    for (int v = 0; v < n; v++) bc[v] = 0.0;

    for (int s = 0; s < n; s++) {
        for (int v = 0; v < n; v++) { dist[v] = -1; sigma[v] = 0.0; delta[v] = 0.0; }
        int ini = 0, fim = 0;
        dist[s] = 0;
        sigma[s] = 1.0;
        fila[fim++] = s;

        /* etapa 1: BFS contando caminhos minimos */
        while (ini < fim) {
            int v = fila[ini++];
            for (int i = 0; i < g->grau[v]; i++) {
                int w = g->viz[v][i];
                if (dist[w] < 0) {
                    dist[w] = dist[v] + 1;
                    fila[fim++] = w;
                }
                if (dist[w] == dist[v] + 1) sigma[w] += sigma[v];
            }
        }

        /* etapa 2: acumulacao das dependencias, do mais distante para s */
        for (int k = fim - 1; k > 0; k--) {
            int w = fila[k];
            for (int i = 0; i < g->grau[w]; i++) {
                int v = g->viz[w][i];
                if (dist[v] == dist[w] - 1)
                    delta[v] += sigma[v] / sigma[w] * (1.0 + delta[w]);
            }
            bc[w] += delta[w];   /* fila[0] e o proprio s, que nao entra */
        }
    }

    double escala = n > 2 ? 2.0 / ((double)(n - 1) * (n - 2)) : 0.0;
    for (int v = 0; v < n; v++) {
        bc[v] /= 2.0;            /* grafo nao dirigido */
        bcNorm[v] = bc[v] * escala;
    }
    free(dist); free(fila); free(sigma); free(delta);
}

/* ======================================================================
 * Ordenacao: merge sort estavel de pares (indice, valor)
 * Ordem crescente de valor; empate -> menor ID original primeiro.
 * ====================================================================== */
typedef struct {
    int idx;
    double valor;
} Par;

static int vemAntes(const Par *a, const Par *b, const long long *ids) {
    if (a->valor != b->valor) return a->valor < b->valor;
    return ids[a->idx] <= ids[b->idx];
}

static void intercalar(Par *v, Par *aux, int ini, int meio, int fim, const long long *ids) {
    int i = ini, j = meio + 1, k = ini;
    while (i <= meio && j <= fim) {
        if (vemAntes(&v[i], &v[j], ids)) aux[k++] = v[i++];
        else aux[k++] = v[j++];
    }
    while (i <= meio) aux[k++] = v[i++];
    while (j <= fim) aux[k++] = v[j++];
    for (k = ini; k <= fim; k++) v[k] = aux[k];
}

void mergeSort(Par *v, Par *aux, int ini, int fim, const long long *ids) {
    if (ini >= fim) return;
    int meio = ini + (fim - ini) / 2;
    mergeSort(v, aux, ini, meio, ids);
    mergeSort(v, aux, meio + 1, fim, ids);
    intercalar(v, aux, ini, meio, fim, ids);
}

/* devolve o vetor de pares ordenado em ordem crescente (quem chama libera) */
Par *ordenarCrescente(const Grafo *g, const double *valores, double *tempo) {
    int n = g->n;
    Par *p = malloc((size_t)n * sizeof(Par));
    Par *aux = malloc((size_t)n * sizeof(Par));
    for (int v = 0; v < n; v++) { p[v].idx = v; p[v].valor = valores[v]; }
    double t0 = agora();
    mergeSort(p, aux, 0, n - 1, g->idOriginal);
    if (tempo) *tempo = agora() - t0;
    free(aux);
    return p;
}

/* posicao de v no ranking do MAIOR para o menor (1 = mais central) */
int posicaoDecrescente(const Grafo *g, const double *valores, int v) {
    int pos = 1;
    for (int u = 0; u < g->n; u++)
        if (valores[u] > valores[v]) pos++;
    return pos;
}

/* ======================================================================
 * Impressao dos rankings
 * ====================================================================== */
static void imprimirLinha(const Grafo *g, int pos, const Par *p,
                          const double *extra, int valorInteiro) {
    if (valorInteiro)
        printf("  %6d  %12lld  %14.0f", pos, g->idOriginal[p->idx], p->valor);
    else
        printf("  %6d  %12lld  %14.8f", pos, g->idOriginal[p->idx], p->valor);
    if (extra) printf("  %14.8f", extra[p->idx]);
    printf("\n");
}

/* imprime a lista em ordem crescente (inteira se n <= LIMITE_TELA, senao o
 * inicio e o fim), o top 10 do maior para o menor e grava a lista completa */
void imprimirRanking(const Grafo *g, const double *valores, const double *extra,
                     const char *titulo, const char *nomeValor, const char *nomeExtra,
                     int valorInteiro, const char *arquivo, double *tempoOrdenacao) {
    int n = g->n;
    Par *p = ordenarCrescente(g, valores, tempoOrdenacao);

    printf("\n=== %s - ordem crescente (%d vertices) ===\n", titulo, n);
    printf("  %6s  %12s  %14s", "pos", "vertice", nomeValor);
    if (extra) printf("  %14s", nomeExtra);
    printf("\n");
    if (n <= LIMITE_TELA) {
        for (int i = 0; i < n; i++) imprimirLinha(g, i + 1, &p[i], extra, valorInteiro);
    } else {
        for (int i = 0; i < PONTAS_TELA; i++) imprimirLinha(g, i + 1, &p[i], extra, valorInteiro);
        printf("  %6s  ... %d linhas omitidas (lista completa em %s) ...\n",
               "", n - 2 * PONTAS_TELA, arquivo);
        for (int i = n - PONTAS_TELA; i < n; i++) imprimirLinha(g, i + 1, &p[i], extra, valorInteiro);
    }

    int k = n < TOP_K ? n : TOP_K;
    printf("\n--- Top %d (do maior para o menor) ---\n", k);
    /* percorre o vetor crescente de tras para frente; dentro de um bloco de
     * valores empatados imprime do menor ID para o maior */
    int impressos = 0, i = n - 1;
    while (impressos < k && i >= 0) {
        int j = i;
        while (j > 0 && p[j - 1].valor == p[i].valor) j--;
        for (int t = j; t <= i && impressos < k; t++, impressos++)
            imprimirLinha(g, impressos + 1, &p[t], extra, valorInteiro);
        i = j - 1;
    }

    FILE *f = fopen(arquivo, "w");
    if (f) {
        fprintf(f, "# %s - ordem crescente\n# posicao vertice %s%s%s\n",
                titulo, nomeValor, extra ? " " : "", extra ? nomeExtra : "");
        for (int i = 0; i < n; i++) {
            fprintf(f, "%d %lld %.10f", i + 1, g->idOriginal[p[i].idx], p[i].valor);
            if (extra) fprintf(f, " %.10f", extra[p[i].idx]);
            fprintf(f, "\n");
        }
        fclose(f);
        printf("(lista completa gravada em %s)\n", arquivo);
    }
    free(p);
}

/* ======================================================================
 * Calculo das metricas (com tempo) - chamadas pelos itens C, D, E e G
 * ====================================================================== */
void garantirGrau(const Grafo *g, Metricas *M) {
    if (M->temGrau) return;
    double t0 = agora();
    calcularGrau(g, M->grau, M->grauNorm);
    M->tGrau = agora() - t0;
    M->temGrau = 1;
}

void garantirCloseness(const Grafo *g, Metricas *M) {
    if (M->temCloseness) return;
    printf("Calculando closeness (%d BFS)...\n", g->n);
    double t0 = agora();
    calcularCloseness(g, M->closeness, M->distMedia);
    M->tCloseness = agora() - t0;
    M->temCloseness = 1;
}

void garantirBetweenness(const Grafo *g, Metricas *M) {
    if (M->temBetweenness) return;
    printf("Calculando betweenness (Brandes, %d BFS)...\n", g->n);
    double t0 = agora();
    calcularBetweenness(g, M->betweenness, M->betweennessNorm);
    M->tBetweenness = agora() - t0;
    M->temBetweenness = 1;
}

/* ======================================================================
 * Itens do menu
 * ====================================================================== */
void itemA(Grafo **pg, Metricas **pM, const char *arquivo) {
    grafoLiberar(*pg);
    metricasLiberar(*pM);
    *pg = NULL;
    *pM = NULL;

    printf("\n=== A) Leitura do arquivo e construcao do grafo ===\n");
    printf("Arquivo: %s\n", arquivo);
    double t0 = agora();
    Grafo *g = lerArquivo(arquivo);
    double t = agora() - t0;
    if (!g) {
        printf("Nao foi possivel abrir o arquivo \"%s\".\n", arquivo);
        return;
    }
    *pg = g;
    *pM = metricasCriar(g->n > 0 ? g->n : 1);
    if (g->n == 0) {
        printf("O arquivo nao tem nenhuma aresta valida.\n");
        return;
    }

    int maior;
    int comp = contarComponentes(g, &maior);
    int gMin = g->grau[0], gMax = g->grau[0];
    for (int v = 1; v < g->n; v++) {
        if (g->grau[v] < gMin) gMin = g->grau[v];
        if (g->grau[v] > gMax) gMax = g->grau[v];
    }
    double densidade = g->n > 1 ? 2.0 * g->m / ((double)g->n * (g->n - 1)) : 0.0;

    printf("Linhas de aresta lidas:      %lld\n", g->linhasLidas);
    printf("Lacos ignorados:             %lld\n", g->lacosIgnorados);
    printf("Arestas repetidas ignoradas: %lld\n", g->repetidasIgnoradas);
    if (g->linhasInvalidas) printf("Linhas invalidas ignoradas:  %lld\n", g->linhasInvalidas);
    printf("Vertices (n):                %d\n", g->n);
    printf("Arestas (m):                 %lld\n", g->m);
    printf("Densidade 2m/(n(n-1)):       %.6f (%.4f%%)\n", densidade, 100.0 * densidade);
    printf("Grau medio 2m/n:             %.4f\n", 2.0 * g->m / g->n);
    printf("Grau minimo / maximo:        %d / %d\n", gMin, gMax);
    printf("Componentes conexas:         %d (maior com %d vertices)\n", comp, maior);
    printf("Memoria - lista de adjacencia: %.2f MB | matriz de adjacencia (1 byte/celula): %.2f MB\n",
           (2.0 * g->m * sizeof(int)) / (1024.0 * 1024.0),
           ((double)g->n * g->n) / (1024.0 * 1024.0));
    printf("Tempo de leitura e construcao: %.6f s\n", t);

    int k = g->n < 5 ? g->n : 5;
    printf("Exemplo - vizinhos dos %d primeiros vertices lidos:\n", k);
    for (int v = 0; v < k; v++) {
        printf("  %lld (grau %d):", g->idOriginal[v], g->grau[v]);
        for (int i = 0; i < g->grau[v] && i < 10; i++) printf(" %lld", g->idOriginal[g->viz[v][i]]);
        if (g->grau[v] > 10) printf(" ...");
        printf("\n");
    }
}

/* grava os CSV (formato aceito pelo Gephi: Id/Label e Source/Target/Type) */
int exportarCSV(const Grafo *g, const Metricas *M) {
    FILE *fv = fopen("grafo_vertices.csv", "w");
    FILE *fa = fopen("grafo_arestas.csv", "w");
    if (!fv || !fa) {
        if (fv) fclose(fv);
        if (fa) fclose(fa);
        return 0;
    }
    fprintf(fv, "Id,Label,grau,grau_norm,closeness,dist_media,betweenness,betweenness_norm\n");
    for (int v = 0; v < g->n; v++)
        fprintf(fv, "%lld,%lld,%.0f,%.10f,%.10f,%.10f,%.10f,%.10f\n",
                g->idOriginal[v], g->idOriginal[v], M->grau[v], M->grauNorm[v],
                M->closeness[v], M->distMedia[v], M->betweenness[v], M->betweennessNorm[v]);
    fprintf(fa, "Source,Target,Type\n");
    for (int u = 0; u < g->n; u++)
        for (int i = 0; i < g->grau[u]; i++) {
            int v = g->viz[u][i];
            if (u < v) fprintf(fa, "%lld,%lld,Undirected\n", g->idOriginal[u], g->idOriginal[v]);
        }
    fclose(fv);
    fclose(fa);
    return 1;
}

void itemB(const Grafo *g, Metricas *M) {
    printf("\n=== B) Plot do grafo ===\n");
    garantirGrau(g, M);
    garantirCloseness(g, M);
    garantirBetweenness(g, M);
    if (!exportarCSV(g, M)) {
        printf("Erro ao gravar os arquivos CSV.\n");
        return;
    }
    printf("Gravados: grafo_vertices.csv (metricas) e grafo_arestas.csv.\n");
    printf("Esses arquivos podem ser importados no Gephi (Data Laboratory > Import Spreadsheet).\n");
    printf("Chamando o script de plot (NetworkX + matplotlib)...\n");
    fflush(stdout);
#ifdef _WIN32
    int r = system("python joaomarcelo_francabatista_plot.py");
#else
    int r = system("python3 joaomarcelo_francabatista_plot.py");
#endif
    if (r != 0)
        printf("Nao foi possivel executar o script de plot (verifique se Python, networkx "
               "e matplotlib estao instalados).\n");
}

void itemC(const Grafo *g, Metricas *M) {
    garantirGrau(g, M);
    imprimirRanking(g, M->grau, M->grauNorm, "C) Grau dos vertices", "grau", "grau_norm",
                    1, "ranking_grau.txt", &M->tOrdenacao);
    printf("Tempo do calculo do grau: %.6f s | ordenacao (merge sort): %.6f s\n",
           M->tGrau, M->tOrdenacao);
}

void itemD(const Grafo *g, Metricas *M) {
    garantirCloseness(g, M);
    imprimirRanking(g, M->closeness, M->distMedia, "D) Closeness dos vertices", "closeness",
                    "dist_media", 0, "ranking_closeness.txt", &M->tOrdenacao);
    printf("Tempo do calculo do closeness: %.6f s | ordenacao (merge sort): %.6f s\n",
           M->tCloseness, M->tOrdenacao);
}

void itemE(const Grafo *g, Metricas *M) {
    garantirBetweenness(g, M);
    imprimirRanking(g, M->betweenness, M->betweennessNorm, "E) Betweenness dos vertices",
                    "betweenness", "betw_norm", 0, "ranking_betweenness.txt", &M->tOrdenacao);
    printf("Tempo do calculo do betweenness: %.6f s | ordenacao (merge sort): %.6f s\n",
           M->tBetweenness, M->tOrdenacao);
}

/* Item F: pesquisa um vertice (1 numero) ou uma aresta (2 numeros) */
void itemF(const Grafo *g, const Metricas *M, const char *entrada) {
    long long a, b;
    char *fim;
    const char *s = entrada;
    a = strtoll(s, &fim, 10);
    if (fim == s) { printf("Entrada invalida.\n"); return; }
    s = fim;
    b = strtoll(s, &fim, 10);
    int ehAresta = (fim != s);

    printf("\n=== F) Pesquisa no grafo ===\n");
    if (!ehAresta) {
        /* busca pela tabela hash: O(1) em media */
        const int REP_HASH = 1000000, REP_SEQ = 1000;
        volatile int sink = 0;
        double t0 = agora();
        for (int r = 0; r < REP_HASH; r++) sink += hashBuscar(g->mapa, a);
        double tHash = (agora() - t0) / REP_HASH;
        /* comparacao: busca sequencial no vetor de IDs, O(n) */
        int achouSeq = -1;
        t0 = agora();
        for (int r = 0; r < REP_SEQ; r++) {
            achouSeq = -1;
            for (int v = 0; v < g->n; v++)
                if (g->idOriginal[v] == a) { achouSeq = v; break; }
            sink += achouSeq;
        }
        double tSeq = (agora() - t0) / REP_SEQ;
        (void)sink;

        int v = hashBuscar(g->mapa, a);
        if (v < 0) {
            printf("Vertice %lld NAO foi encontrado no grafo.\n", a);
        } else {
            printf("Vertice %lld ENCONTRADO (indice interno %d).\n", a, v);
            printf("  grau: %d | vizinhos:", g->grau[v]);
            for (int i = 0; i < g->grau[v] && i < 15; i++) printf(" %lld", g->idOriginal[g->viz[v][i]]);
            if (g->grau[v] > 15) printf(" ... (+%d)", g->grau[v] - 15);
            printf("\n");
            if (M->temGrau)
                printf("  grau_norm: %.6f (posicao %d de %d, do maior para o menor)\n",
                       M->grauNorm[v], posicaoDecrescente(g, M->grau, v), g->n);
            if (M->temCloseness)
                printf("  closeness: %.6f | dist. media: %.4f (posicao %d)\n", M->closeness[v],
                       M->distMedia[v], posicaoDecrescente(g, M->closeness, v));
            if (M->temBetweenness)
                printf("  betweenness: %.4f | normalizado: %.6f (posicao %d)\n", M->betweenness[v],
                       M->betweennessNorm[v], posicaoDecrescente(g, M->betweenness, v));
        }
        printf("  Tempo medio por busca: tabela hash %.1f ns | busca sequencial %.1f ns\n",
               tHash * 1e9, tSeq * 1e9);
    } else {
        int u = hashBuscar(g->mapa, a);
        int v = hashBuscar(g->mapa, b);
        if (u < 0) printf("Vertice %lld NAO foi encontrado no grafo.\n", a);
        if (v < 0) printf("Vertice %lld NAO foi encontrado no grafo.\n", b);
        if (u < 0 || v < 0) { printf("Logo a aresta (%lld, %lld) nao existe.\n", a, b); return; }
        if (existeAresta(g, u, v)) printf("Aresta (%lld, %lld) ENCONTRADA.\n", a, b);
        else printf("Aresta (%lld, %lld) NAO foi encontrada.\n", a, b);
        /* bonus: distancia (caminho minimo) entre os dois pela BFS */
        int *dist = malloc((size_t)g->n * sizeof(int));
        int *fila = malloc((size_t)g->n * sizeof(int));
        bfs(g, u, dist, fila);
        if (dist[v] >= 0) printf("  Distancia (caminho minimo pela BFS) entre eles: %d\n", dist[v]);
        else printf("  Nao ha caminho entre eles (componentes diferentes).\n");
        free(dist);
        free(fila);
    }
}

/* Item G: tempo de execucao das metricas (REPETICOES_TEMPO execucoes) */
void itemG(const Grafo *g, Metricas *M) {
    printf("\n=== G) Tempo de execucao das metricas (%d execucoes cada) ===\n", REPETICOES_TEMPO);
    printf("n = %d, m = %lld\n", g->n, g->m);
    double *a1 = malloc((size_t)g->n * sizeof(double));
    double *a2 = malloc((size_t)g->n * sizeof(double));
    const char *nomes[3] = { "Grau", "Closeness", "Betweenness" };
    const char *complex[3] = { "O(V)", "O(V(V+E))", "O(VE)" };
    printf("  %-12s %-11s %12s %12s %12s\n", "metrica", "complex.", "media (s)", "min (s)", "max (s)");
    for (int k = 0; k < 3; k++) {
        double soma = 0, mn = 1e30, mx = 0;
        for (int r = 0; r < REPETICOES_TEMPO; r++) {
            double t0 = agora();
            if (k == 0) calcularGrau(g, a1, a2);
            else if (k == 1) calcularCloseness(g, a1, a2);
            else calcularBetweenness(g, a1, a2);
            double t = agora() - t0;
            soma += t;
            if (t < mn) mn = t;
            if (t > mx) mx = t;
        }
        printf("  %-12s %-11s %12.6f %12.6f %12.6f\n", nomes[k], complex[k],
               soma / REPETICOES_TEMPO, mn, mx);
        /* guarda os resultados da ultima execucao como as metricas atuais */
        if (k == 0) { memcpy(M->grau, a1, g->n * sizeof(double)); memcpy(M->grauNorm, a2, g->n * sizeof(double)); M->temGrau = 1; M->tGrau = soma / REPETICOES_TEMPO; }
        if (k == 1) { memcpy(M->closeness, a1, g->n * sizeof(double)); memcpy(M->distMedia, a2, g->n * sizeof(double)); M->temCloseness = 1; M->tCloseness = soma / REPETICOES_TEMPO; }
        if (k == 2) { memcpy(M->betweenness, a1, g->n * sizeof(double)); memcpy(M->betweennessNorm, a2, g->n * sizeof(double)); M->temBetweenness = 1; M->tBetweenness = soma / REPETICOES_TEMPO; }
    }
    double tOrd;
    Par *p = ordenarCrescente(g, M->betweenness, &tOrd);
    free(p);
    printf("  %-12s %-11s %12.6f\n", "Ordenacao", "O(V log V)", tOrd);
    free(a1);
    free(a2);
}

/* ======================================================================
 * Programa principal
 * ====================================================================== */
static void lerLinha(char *buf, int tam) {
    fflush(stdout);
    if (!fgets(buf, tam, stdin)) { buf[0] = '0'; buf[1] = '\0'; return; }
    buf[strcspn(buf, "\r\n")] = '\0';
}

static int precisaGrafo(const Grafo *g) {
    if (!g || g->n == 0) {
        printf("Primeiro leia um arquivo valido (opcao A).\n");
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    const char *arquivo = argc > 1 ? argv[1] : ARQUIVO_PADRAO;
    int automatico = argc > 2 && strcmp(argv[2], "--auto") == 0;
    Grafo *g = NULL;
    Metricas *M = NULL;

    printf("AED2 - Trabalho 3 - Analise de dados em formato de grafos\n");
    printf("Joao Marcelo Franca Batista\n");

    if (automatico) {
        itemA(&g, &M, arquivo);
        if (precisaGrafo(g)) return 1;
        itemC(g, M);
        itemD(g, M);
        itemE(g, M);
        if (exportarCSV(g, M)) printf("\nGravados grafo_vertices.csv e grafo_arestas.csv\n");
        printf("\nResumo dos tempos: grau %.6f s | closeness %.6f s | betweenness %.6f s\n",
               M->tGrau, M->tCloseness, M->tBetweenness);
        grafoLiberar(g);
        metricasLiberar(M);
        return 0;
    }

    char op[MAX_LINHA];
    for (;;) {
        printf("\n--------------------------------------------------\n");
        printf("Arquivo: %s", arquivo);
        if (g) printf("  (n = %d, m = %lld)", g->n, g->m);
        printf("\n");
        printf("  A - Ler o arquivo e gerar o grafo\n");
        printf("  B - Plot do grafo\n");
        printf("  C - Grau dos vertices (ordem crescente)\n");
        printf("  D - Closeness dos vertices (ordem crescente)\n");
        printf("  E - Betweenness dos vertices (ordem crescente)\n");
        printf("  F - Pesquisar vertice (\"id\") ou aresta (\"id1 id2\")\n");
        printf("  G - Tempo de execucao das metricas\n");
        printf("  T - Trocar o arquivo de entrada\n");
        printf("  0 - Sair\n");
        printf("Opcao: ");
        lerLinha(op, sizeof(op));
        char c = op[0];
        if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');

        if (c == '0') break;
        else if (c == 'A') itemA(&g, &M, arquivo);
        else if (c == 'T') {
            static char novo[MAX_LINHA];
            printf("Novo arquivo: ");
            lerLinha(novo, sizeof(novo));
            if (novo[0]) arquivo = novo;
            itemA(&g, &M, arquivo);
        }
        else if (c == 'B') { if (!precisaGrafo(g)) itemB(g, M); }
        else if (c == 'C') { if (!precisaGrafo(g)) itemC(g, M); }
        else if (c == 'D') { if (!precisaGrafo(g)) itemD(g, M); }
        else if (c == 'E') { if (!precisaGrafo(g)) itemE(g, M); }
        else if (c == 'F') {
            if (precisaGrafo(g)) continue;
            char busca[MAX_LINHA];
            printf("Digite o ID do vertice (ou dois IDs para uma aresta): ");
            lerLinha(busca, sizeof(busca));
            itemF(g, M, busca);
        }
        else if (c == 'G') { if (!precisaGrafo(g)) itemG(g, M); }
        else printf("Opcao invalida.\n");
    }

    grafoLiberar(g);
    metricasLiberar(M);
    return 0;
}

/*
 * Nota de utilizacao de IA: uma LLM (Claude) foi utilizada para auxiliar na
 * formatacao correta e na inclusao de comentarios ao longo do codigo.
 * Alem das partes de impressao na tela (printf's) para melhor visualizacao
 * no momento de execucao.
 */
