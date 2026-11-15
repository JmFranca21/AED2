/*
 * AED2 - Atividade Pratica - Aula 12
 * Joao Marcelo Franca Batista
 *
 * Questao 1: labirinto representado por uma matriz de caracteres N x M:
 *   'S' = ponto de partida, 'E' = saida, '.' = caminho livre, '#' = parede.
 * Movimentos permitidos: apenas as 4 direcoes ortogonais (cima, baixo,
 * esquerda, direita), sem diagonais e sem sair da grade.
 *
 * O programa:
 *   1) constroi uma estrutura de GRAFO a partir da matriz: cada coordenada
 *      livre (S, E e '.') e um VERTICE; dois vertices sao ligados por uma
 *      ARESTA quando suas coordenadas sao vizinhas na horizontal ou na vertical
 *      (grafo nao dirigido e nao ponderado, ou seja, cada passo custa 1);
 *   2) executa uma BUSCA EM LARGURA (BFS) a partir de S. Como o grafo nao e
 *      ponderado, a BFS visita os vertices em ordem crescente de distancia e,
 *      portanto, o primeiro caminho que ela encontra ate E e o MENOR (em
 *      numero de passos);
 *   3) reconstroi o caminho seguindo o "pai" de cada vertice de E ate S,
 *      imprime a sequencia de coordenadas (linha, coluna), com indices a
 *      partir de 0, e o tamanho do caminho (numero de passos). Se E nao for
 *      alcancado, informa que o labirinto e insoluvel.
 *
 * Representacao do grafo: lista de adjacencia. Como em uma grade cada vertice
 * tem no maximo 4 vizinhos, a lista de cada vertice cabe em um vetor de 4
 * posicoes (adj[v][0..grau[v]-1]), sem precisar de uma alocacao por aresta.
 * A ordem dos vizinhos e cima, baixo, esquerda, direita: quando existem varios
 * caminhos de mesmo tamanho minimo, esta ordem decide qual deles e impresso.
 *
 * O programa roda 4 labirintos de teste (o exemplo do enunciado, um insoluvel e
 * dois casos de borda) e, no fim, um teste de desempenho em um labirinto
 * grande gerado aleatoriamente (seed 42). Opcionalmente, um labirinto pode ser
 * lido de um arquivo passado como argumento: a primeira linha tem N e M e as
 * N linhas seguintes tem as M colunas do mapa.
 * Medicao de tempo com clock_gettime() (time.h).
 *
 * Fontes consultadas (algoritmo classico, implementado a partir dela):
 *  - Cormen et al., Introduction to Algorithms, cap. 22 (busca em largura,
 *    vetor de pais e reconstrucao do caminho minimo em grafos nao ponderados).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_VIZINHOS 4
#define SEM_PAI -1
#define NAO_VISITADO -1
#define PRINT_LIMITE 40   /* labirintos maiores que isso nao tem o caminho impresso */

/* deslocamentos das 4 direcoes ortogonais: cima, baixo, esquerda, direita */
const int DLINHA[MAX_VIZINHOS]  = { -1, 1,  0, 0 };
const int DCOLUNA[MAX_VIZINHOS] = {  0, 0, -1, 1 };

typedef struct {
    int N, M;             /* dimensoes da matriz */
    const char *mapa;     /* matriz N x M guardada por linhas */
    int numVertices;
    int *idCelula;        /* idCelula[l*M + c] = id do vertice, ou -1 se for parede */
    int *linha, *coluna;  /* coordenada de cada vertice */
    int (*adj)[MAX_VIZINHOS]; /* lista de adjacencia de cada vertice */
    int *grau;            /* quantos vizinhos cada vertice tem */
    int origem, saida;    /* ids dos vertices S e E (-1 se nao existirem) */
    long numArestas;      /* arestas nao dirigidas (cada uma conta uma vez) */
} Grafo;

int celulaLivre(char c) {
    return c == '.' || c == 'S' || c == 'E';
}

/* ---------- Etapa 1: construir o grafo a partir da matriz ---------- */
Grafo *construirGrafo(const char *mapa, int N, int M) {
    Grafo *g = malloc(sizeof(Grafo));
    g->N = N;
    g->M = M;
    g->mapa = mapa;
    g->origem = g->saida = -1;
    g->numArestas = 0;

    g->idCelula = malloc((size_t)N * M * sizeof(int));

    /* 1a passada: cada celula livre vira um vertice (ids em ordem de leitura) */
    int n = 0;
    for (int l = 0; l < N; l++) {
        for (int c = 0; c < M; c++) {
            if (celulaLivre(mapa[l * M + c])) g->idCelula[l * M + c] = n++;
            else g->idCelula[l * M + c] = -1;
        }
    }
    g->numVertices = n;

    g->linha = malloc((size_t)n * sizeof(int));
    g->coluna = malloc((size_t)n * sizeof(int));
    g->grau = calloc((size_t)n, sizeof(int));
    g->adj = malloc((size_t)n * sizeof(*g->adj));

    /* 2a passada: guarda a coordenada de cada vertice e acha S e E */
    for (int l = 0; l < N; l++) {
        for (int c = 0; c < M; c++) {
            int id = g->idCelula[l * M + c];
            if (id < 0) continue;
            g->linha[id] = l;
            g->coluna[id] = c;
            if (mapa[l * M + c] == 'S') g->origem = id;
            if (mapa[l * M + c] == 'E') g->saida = id;
        }
    }

    /* 3a passada: arestas entre vizinhos livres (cima, baixo, esquerda, direita) */
    for (int v = 0; v < n; v++) {
        for (int d = 0; d < MAX_VIZINHOS; d++) {
            int l = g->linha[v] + DLINHA[d];
            int c = g->coluna[v] + DCOLUNA[d];
            if (l < 0 || l >= N || c < 0 || c >= M) continue; /* fora da grade */
            int w = g->idCelula[l * M + c];
            if (w < 0) continue;                              /* parede */
            g->adj[v][g->grau[v]++] = w;
        }
        g->numArestas += g->grau[v];
    }
    g->numArestas /= 2; /* cada aresta foi contada nos dois extremos */
    return g;
}

void liberarGrafo(Grafo *g) {
    free(g->idCelula);
    free(g->linha);
    free(g->coluna);
    free(g->grau);
    free(g->adj);
    free(g);
}

/* ---------- Etapa 2: busca em largura (BFS) a partir de S ---------- */
/* preenche dist[] (passos desde S) e pai[] (vertice anterior no caminho).
 * Devolve 1 se alcancou E, 0 caso contrario. Para assim que E e descoberto,
 * pois a distancia dele ja e a minima. */
int buscaEmLargura(const Grafo *g, int *dist, int *pai) {
    int n = g->numVertices;
    for (int v = 0; v < n; v++) {
        dist[v] = NAO_VISITADO;
        pai[v] = SEM_PAI;
    }

    int *fila = malloc((size_t)n * sizeof(int));
    int ini = 0, fim = 0;

    dist[g->origem] = 0;
    fila[fim++] = g->origem;

    int achou = 0;
    while (ini < fim && !achou) {
        int v = fila[ini++];
        for (int i = 0; i < g->grau[v]; i++) {
            int w = g->adj[v][i];
            if (dist[w] != NAO_VISITADO) continue; /* ja visitado */
            dist[w] = dist[v] + 1;
            pai[w] = v;
            fila[fim++] = w;
            if (w == g->saida) { achou = 1; break; }
        }
    }
    free(fila);
    return achou;
}

/* ---------- Etapa 3: reconstruir o caminho de S ate E ---------- */
/* grava em caminho[] os vertices de S ate E (em ordem) e devolve quantos sao */
int reconstruirCaminho(const Grafo *g, const int *pai, int *caminho) {
    int tamanho = 0;
    for (int v = g->saida; v != SEM_PAI; v = pai[v]) caminho[tamanho++] = v;
    /* o vetor saiu de E para S: inverte */
    for (int i = 0, j = tamanho - 1; i < j; i++, j--) {
        int tmp = caminho[i];
        caminho[i] = caminho[j];
        caminho[j] = tmp;
    }
    return tamanho;
}

/* ---------- Funcoes auxiliares: resolver, imprimir, medir ---------- */
double diffSegundos(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

/* resolve um labirinto: constroi o grafo, roda a BFS e imprime o resultado.
 * Devolve o numero de passos do menor caminho, ou -1 se for insoluvel
 * (ou -2 se o mapa nao tiver exatamente um S e um E). */
int resolverLabirinto(const char *nome, const char *mapa, int N, int M, int imprimir) {
    int numS = 0, numE = 0;
    for (int i = 0; i < N * M; i++) {
        if (mapa[i] == 'S') numS++;
        if (mapa[i] == 'E') numE++;
    }
    if (imprimir) printf("=== %s (%d x %d) ===\n", nome, N, M);
    if (numS != 1 || numE != 1) {
        if (imprimir) printf("Mapa invalido: precisa ter exatamente um 'S' e um 'E' (tem %d e %d).\n\n", numS, numE);
        return -2;
    }

    Grafo *g = construirGrafo(mapa, N, M);
    int *dist = malloc((size_t)g->numVertices * sizeof(int));
    int *pai = malloc((size_t)g->numVertices * sizeof(int));
    int resultado;

    if (imprimir) {
        printf("Grafo: %d vertices (celulas livres), %ld arestas\n", g->numVertices, g->numArestas);
    }

    if (!buscaEmLargura(g, dist, pai)) {
        if (imprimir) printf("Labirinto INSOLUVEL: nao existe caminho de S ate E.\n\n");
        resultado = -1;
    } else {
        int *caminho = malloc((size_t)g->numVertices * sizeof(int));
        int tam = reconstruirCaminho(g, pai, caminho);
        resultado = tam - 1; /* passos = coordenadas - 1 */

        if (imprimir) {
            if (N <= PRINT_LIMITE && M <= PRINT_LIMITE) {
                printf("Caminho (linha, coluna), do inicio ao fim:\n");
                for (int i = 0; i < tam; i++) {
                    printf("(%d,%d)%s", g->linha[caminho[i]], g->coluna[caminho[i]],
                           i < tam - 1 ? " -> " : "\n");
                }
            }
            printf("Tamanho do caminho: %d passos (%d coordenadas, contando S e E)\n", resultado, tam);

            if (N <= PRINT_LIMITE && M <= PRINT_LIMITE) {
                /* desenha o labirinto com o caminho marcado por '*' */
                char *desenho = malloc((size_t)N * M);
                memcpy(desenho, mapa, (size_t)N * M);
                for (int i = 1; i < tam - 1; i++) {
                    desenho[g->linha[caminho[i]] * M + g->coluna[caminho[i]]] = '*';
                }
                printf("Labirinto com o caminho marcado:\n");
                for (int l = 0; l < N; l++) printf("  %.*s\n", M, desenho + l * M);
                free(desenho);
            }
            printf("\n");
        }
        free(caminho);
    }

    free(dist);
    free(pai);
    liberarGrafo(g);
    return resultado;
}

/* copia um labirinto dado como vetor de strings para uma matriz plana */
char *montarMapa(const char *linhas[], int N, int M) {
    char *mapa = malloc((size_t)N * M);
    for (int l = 0; l < N; l++) memcpy(mapa + l * M, linhas[l], (size_t)M);
    return mapa;
}

/* le o labirinto de um arquivo: "N M" e depois N linhas de M caracteres */
char *lerArquivo(const char *caminhoArquivo, int *N, int *M) {
    FILE *f = fopen(caminhoArquivo, "r");
    if (f == NULL) return NULL;
    if (fscanf(f, "%d %d", N, M) != 2 || *N <= 0 || *M <= 0) { fclose(f); return NULL; }
    char *mapa = malloc((size_t)(*N) * (*M));
    char linha[4096];
    for (int l = 0; l < *N; l++) {
        if (fscanf(f, "%4095s", linha) != 1 || (int)strlen(linha) != *M) {
            free(mapa);
            fclose(f);
            return NULL;
        }
        memcpy(mapa + l * (*M), linha, (size_t)(*M));
    }
    fclose(f);
    return mapa;
}

/* ---------- Teste de desempenho: labirinto grande aleatorio ---------- */
#define BENCH_N 2000
#define BENCH_M 2000
#define BENCH_PAREDES 25    /* % de celulas que viram parede */
#define BENCH_EXECUCOES 5

void testeDesempenho(void) {
    char *mapa = malloc((size_t)BENCH_N * BENCH_M);
    for (int i = 0; i < BENCH_N * BENCH_M; i++) {
        mapa[i] = (rand() % 100 < BENCH_PAREDES) ? '#' : '.';
    }
    mapa[0] = 'S';
    mapa[BENCH_N * BENCH_M - 1] = 'E';

    printf("=== Teste de desempenho: labirinto aleatorio %d x %d, %d%% de paredes (seed 42) ===\n",
           BENCH_N, BENCH_M, BENCH_PAREDES);

    struct timespec t0, t1;
    double somaConstrucao = 0.0, somaBusca = 0.0;
    int passos = -1;
    int numVertices = 0;
    long numArestas = 0;

    for (int e = 0; e < BENCH_EXECUCOES; e++) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        Grafo *g = construirGrafo(mapa, BENCH_N, BENCH_M);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        somaConstrucao += diffSegundos(t0, t1);
        numVertices = g->numVertices;
        numArestas = g->numArestas;

        int *dist = malloc((size_t)g->numVertices * sizeof(int));
        int *pai = malloc((size_t)g->numVertices * sizeof(int));
        clock_gettime(CLOCK_MONOTONIC, &t0);
        int achou = buscaEmLargura(g, dist, pai);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        somaBusca += diffSegundos(t0, t1);

        if (achou) {
            int *caminho = malloc((size_t)g->numVertices * sizeof(int));
            passos = reconstruirCaminho(g, pai, caminho) - 1;
            free(caminho);
        } else {
            passos = -1;
        }
        free(dist);
        free(pai);
        liberarGrafo(g);
    }

    printf("Grafo: %d vertices, %ld arestas\n", numVertices, numArestas);
    printf("Resultado: %s\n", passos >= 0 ? "caminho encontrado" : "labirinto insoluvel");
    if (passos >= 0) printf("Menor caminho: %d passos\n", passos);
    printf("Tempo para construir o grafo (media de %d execucoes): %.6f s\n", BENCH_EXECUCOES,
           somaConstrucao / BENCH_EXECUCOES);
    printf("Tempo da BFS (media de %d execucoes): %.6f s\n", BENCH_EXECUCOES,
           somaBusca / BENCH_EXECUCOES);
    free(mapa);
}

int main(int argc, char *argv[]) {
    /* se foi passado um arquivo, resolve so esse labirinto */
    if (argc > 1) {
        int N, M;
        char *mapa = lerArquivo(argv[1], &N, &M);
        if (mapa == NULL) {
            printf("Nao foi possivel ler o labirinto do arquivo '%s'.\n", argv[1]);
            return 1;
        }
        resolverLabirinto(argv[1], mapa, N, M, 1);
        free(mapa);
        return 0;
    }

    srand(42); /* seed fixa: o labirinto do teste de desempenho e sempre o mesmo */

    /* 1) exemplo do enunciado */
    const char *exemplo[] = { "S.#..", "#.#.#", ".....", "###.#", "....E" };
    char *m1 = montarMapa(exemplo, 5, 5);
    resolverLabirinto("Exemplo do enunciado", m1, 5, 5, 1);

    /* 2) labirinto sem solucao: a coluna 2 e a linha 3 isolam o S */
    const char *insoluvel[] = { "S.#..", "#.#.#", "..#..", "###.#", "....E" };
    char *m2 = montarMapa(insoluvel, 5, 5);
    resolverLabirinto("Labirinto insoluvel", m2, 5, 5, 1);

    /* 3) caso de borda: S e E lado a lado */
    const char *vizinhos[] = { "SE" };
    char *m3 = montarMapa(vizinhos, 1, 2);
    resolverLabirinto("S e E vizinhos", m3, 1, 2, 1);

    /* 4) varios caminhos de mesmo tamanho: a BFS devolve um dos menores */
    const char *aberto[] = { "S....", ".....", ".....", ".....", "....E" };
    char *m4 = montarMapa(aberto, 5, 5);
    resolverLabirinto("Grade aberta (varios caminhos minimos)", m4, 5, 5, 1);

    testeDesempenho();

    free(m1);
    free(m2);
    free(m3);
    free(m4);
    return 0;
}

/*
 * Nota de utilizacao de IA: uma LLM (Claude) foi utilizada para auxiliar na
 * formatacao correta e na inclusao de comentarios ao longo do codigo.
 * Alem das partes de impressao na tela (printf's) para melhor visualizacao
 * no momento de execucao.
 */
