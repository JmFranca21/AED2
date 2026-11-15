/*
 * AED2 - Atividade Pratica - Aula 6
 * Joao Marcelo Franca Batista
 *
 * Questao 1: inserir 10.000 elementos aleatorios em tres estruturas e comparar:
 *   a) VETOR ALEATORIO (insercao no fim; busca linear);
 *   b) VETOR ORDENADO (cada elemento e inserido na posicao correta, deslocando
 *      os maiores; busca binaria);
 *   c) ARVORE BINARIA DE BUSCA (sem balanceamento).
 * Sao medidos: o tempo de insercao dos 10.000 elementos, o tempo de busca do
 * elemento 50 e o tempo de busca do elemento 5000 (em cada estrutura).
 * Questao 2: altura das subarvores esquerda e direita da arvore binaria.
 *
 * Escolhas da atividade:
 *  - Os elementos sao uma PERMUTACAO aleatoria dos valores 0 a 9999 (embaralhada
 *    com Fisher-Yates, seed 42). Assim nao ha chaves repetidas e os elementos
 *    50 e 5000 existem de fato nas tres estruturas. Os tres recebem exatamente
 *    os mesmos valores, na mesma ordem.
 *  - Tempo de insercao = MEDIA de 10 execucoes (a estrutura e refeita do zero).
 *  - Uma unica busca dura poucos nanossegundos (menos que a precisao do
 *    relogio). Por isso cada busca e repetida varias vezes e o tempo mostrado
 *    e o de UMA busca (tempo total / repeticoes).
 *  - Altura medida em NIVEIS: uma subarvore vazia tem altura 0 e um unico no
 *    tem altura 1. A altura da arvore e 1 + max(esquerda, direita).
 *
 * Medicao de tempo com clock_gettime() (time.h).
 *
 * Fontes consultadas (algoritmos classicos, implementados a partir deles):
 *  - Cormen et al., Introduction to Algorithms, cap. 12 (arvores binarias de
 *    busca) e cap. 2 (insercao ordenada e busca).
 *  - N. Ziviani, Projeto de Algoritmos (pesquisa binaria e arvores de pesquisa).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 10000             /* elementos inseridos */
#define EXECUCOES 10        /* execucoes para a media do tempo de insercao */
#define REP_LINEAR 20000    /* repeticoes da busca linear (vetor aleatorio) */
#define REP_RAPIDA 2000000  /* repeticoes da busca binaria e da busca na arvore */
#define BUSCA_A 50
#define BUSCA_B 5000

/* ---------- Vetor aleatorio: insercao no fim, busca linear ---------- */
int vetorAleatorio[N];

int buscaLinear(const int v[], int n, int chave) {
    for (int i = 0; i < n; i++) {
        if (v[i] == chave) return i;
    }
    return -1;
}

/* ---------- Vetor ordenado: insercao ordenada, busca binaria ---------- */
int vetorOrdenado[N];

/* insere mantendo o vetor ordenado: desloca para a direita quem e maior */
void inserirOrdenado(int v[], int *n, int chave) {
    int i = *n - 1;
    while (i >= 0 && v[i] > chave) {
        v[i + 1] = v[i];
        i--;
    }
    v[i + 1] = chave;
    (*n)++;
}

int buscaBinaria(const int v[], int n, int chave) {
    int ini = 0, fim = n - 1;
    while (ini <= fim) {
        int meio = ini + (fim - ini) / 2;
        if (v[meio] == chave) return meio;
        if (v[meio] < chave) ini = meio + 1;
        else fim = meio - 1;
    }
    return -1;
}

/* ---------- Arvore binaria de busca (sem balanceamento) ---------- */
typedef struct No {
    int chave;
    struct No *esq, *dir;
} No;

No *inserirArvore(No *raiz, int chave) {
    No *novo = malloc(sizeof(No));
    novo->chave = chave;
    novo->esq = novo->dir = NULL;
    if (raiz == NULL) return novo;

    No *p = raiz;
    while (1) {
        if (chave < p->chave) {
            if (p->esq == NULL) { p->esq = novo; break; }
            p = p->esq;
        } else {
            if (p->dir == NULL) { p->dir = novo; break; }
            p = p->dir;
        }
    }
    return raiz;
}

int buscaArvore(const No *raiz, int chave) {
    while (raiz != NULL) {
        if (chave == raiz->chave) return 1;
        raiz = (chave < raiz->chave) ? raiz->esq : raiz->dir;
    }
    return 0;
}

/* quantos nos a busca visita ate achar a chave (comparacoes feitas) */
int comparacoesArvore(const No *raiz, int chave) {
    int c = 0;
    while (raiz != NULL) {
        c++;
        if (chave == raiz->chave) break;
        raiz = (chave < raiz->chave) ? raiz->esq : raiz->dir;
    }
    return c;
}

int comparacoesBinaria(const int v[], int n, int chave) {
    int ini = 0, fim = n - 1, c = 0;
    while (ini <= fim) {
        int meio = ini + (fim - ini) / 2;
        c++;
        if (v[meio] == chave) break;
        if (v[meio] < chave) ini = meio + 1;
        else fim = meio - 1;
    }
    return c;
}

int altura(const No *no) {
    if (no == NULL) return 0;
    int e = altura(no->esq);
    int d = altura(no->dir);
    return 1 + (e > d ? e : d);
}

/* menor altura (em niveis) possivel para uma arvore binaria com n nos: 2^h - 1 >= n */
int alturaMinima(int n) {
    int h = 0;
    while (((1L << h) - 1) < n) h++;
    return h;
}

void liberarArvore(No *no) {
    if (no == NULL) return;
    liberarArvore(no->esq);
    liberarArvore(no->dir);
    free(no);
}

/* ---------- Funcoes auxiliares (nao fazem parte da comparacao) ---------- */
double diffSegundos(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

/* rand() pode ter RAND_MAX = 32767 (MinGW/Windows): junta duas chamadas (30 bits) */
int sorteio(int limite) {
    return (((rand() & 0x7FFF) << 15) | (rand() & 0x7FFF)) % limite;
}

int main(void) {
    struct timespec t0, t1;
    int elementos[N];

    /* permutacao aleatoria de 0..N-1 (Fisher-Yates), seed fixa */
    srand(42);
    for (int i = 0; i < N; i++) elementos[i] = i;
    for (int i = N - 1; i > 0; i--) {
        int j = sorteio(i + 1);
        int tmp = elementos[i];
        elementos[i] = elementos[j];
        elementos[j] = tmp;
    }

    /* ---------- Insercao (media de EXECUCOES execucoes) ---------- */
    double somaVet = 0, somaOrd = 0, somaArv = 0;
    No *raiz = NULL;
    int nOrd = 0;

    for (int e = 0; e < EXECUCOES; e++) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (int i = 0; i < N; i++) vetorAleatorio[i] = elementos[i];
        clock_gettime(CLOCK_MONOTONIC, &t1);
        somaVet += diffSegundos(t0, t1);

        nOrd = 0;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (int i = 0; i < N; i++) inserirOrdenado(vetorOrdenado, &nOrd, elementos[i]);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        somaOrd += diffSegundos(t0, t1);

        liberarArvore(raiz);
        raiz = NULL;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (int i = 0; i < N; i++) raiz = inserirArvore(raiz, elementos[i]);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        somaArv += diffSegundos(t0, t1);
    }
    double insVet = somaVet / EXECUCOES, insOrd = somaOrd / EXECUCOES, insArv = somaArv / EXECUCOES;

    /* ---------- Verificacao de corretude ---------- */
    int ok = 1;
    for (int i = 0; i < N; i++) {
        if (vetorOrdenado[i] != i) ok = 0;                       /* ordenado = 0,1,2,... */
        if (buscaLinear(vetorAleatorio, N, i) < 0) ok = 0;
        if (buscaBinaria(vetorOrdenado, N, i) < 0) ok = 0;
        if (!buscaArvore(raiz, i)) ok = 0;
    }
    if (buscaLinear(vetorAleatorio, N, N + 1) >= 0 || buscaBinaria(vetorOrdenado, N, N + 1) >= 0 ||
        buscaArvore(raiz, N + 1)) ok = 0; /* elemento inexistente nao pode ser achado */

    /* ---------- Busca dos elementos 50 e 5000 ---------- */
    int chaves[2] = { BUSCA_A, BUSCA_B };
    double bVet[2], bOrd[2], bArv[2];
    int posVet[2];
    volatile int sink = 0;

    for (int k = 0; k < 2; k++) {
        posVet[k] = buscaLinear(vetorAleatorio, N, chaves[k]);

        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (int r = 0; r < REP_LINEAR; r++) sink += buscaLinear(vetorAleatorio, N, chaves[k]);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        bVet[k] = diffSegundos(t0, t1) / REP_LINEAR;

        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (int r = 0; r < REP_RAPIDA; r++) sink += buscaBinaria(vetorOrdenado, N, chaves[k]);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        bOrd[k] = diffSegundos(t0, t1) / REP_RAPIDA;

        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (int r = 0; r < REP_RAPIDA; r++) sink += buscaArvore(raiz, chaves[k]);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        bArv[k] = diffSegundos(t0, t1) / REP_RAPIDA;
    }

    /* ---------- Alturas ---------- */
    int hEsq = altura(raiz->esq), hDir = altura(raiz->dir);

    printf("%d elementos (permutacao aleatoria de 0 a %d, seed fixa = 42)\n", N, N - 1);
    printf("Raiz da arvore binaria: %d | Verificacao de corretude: %s\n", raiz->chave, ok ? "OK" : "ERRO");
    printf("Insercao = media de %d execucoes; busca = tempo de UMA busca (repetida %d / %d vezes)\n\n",
           EXECUCOES, REP_LINEAR, REP_RAPIDA);

    printf("%-34s %16s %16s %16s\n", "Estrutura", "Insercao 10.000", "Busca do 50", "Busca do 5000");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("%-34s %13.6f s %13.1f ns %13.1f ns\n", "Vetor aleatorio (busca linear)", insVet, bVet[0] * 1e9, bVet[1] * 1e9);
    printf("%-34s %13.6f s %13.1f ns %13.1f ns\n", "Vetor ordenado (busca binaria)", insOrd, bOrd[0] * 1e9, bOrd[1] * 1e9);
    printf("%-34s %13.6f s %13.1f ns %13.1f ns\n", "Arvore binaria de busca", insArv, bArv[0] * 1e9, bArv[1] * 1e9);
    printf("\nPosicao do 50 e do 5000 no vetor aleatorio (busca linear testa pos+1 elementos): %d e %d\n",
           posVet[0], posVet[1]);

    printf("Comparacoes ate achar o 50 / o 5000: busca linear %d / %d, busca binaria %d / %d, arvore %d / %d\n",
           posVet[0] + 1, posVet[1] + 1,
           comparacoesBinaria(vetorOrdenado, N, BUSCA_A), comparacoesBinaria(vetorOrdenado, N, BUSCA_B),
           comparacoesArvore(raiz, BUSCA_A), comparacoesArvore(raiz, BUSCA_B));

    printf("\nAltura da subarvore esquerda: %d | direita: %d | altura da arvore: %d (minimo possivel: %d)\n",
           hEsq, hDir, altura(raiz), alturaMinima(N));

    liberarArvore(raiz);
    return 0;
}

/*
 * Nota de utilizacao de IA: uma LLM (Claude) foi utilizada para auxiliar na
 * formatacao correta e na inclusao de comentarios ao longo do codigo.
 * Alem das partes de impressao na tela (printf's) para melhor visualizacao
 * no momento de execucao.
 */
