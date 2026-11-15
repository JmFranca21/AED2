/*
 * AED2 - Atividade Pratica - Aula 7
 * Joao Marcelo Franca Batista
 *
 * Questao 1: inserir 10.000 elementos aleatorios em uma ARVORE AVL e medir o
 * tempo de insercao e o tempo de busca dos elementos 50 e 5000. Para comparar
 * (item d), o MESMO programa tambem refaz as medicoes da Aula 6 com os
 * mesmos elementos: vetor aleatorio (busca linear), vetor ordenado (busca
 * binaria) e arvore binaria de busca sem balanceamento. Assim os quatro
 * tempos saem da mesma execucao, na mesma maquina.
 * Questao 2: altura das subarvores esquerda e direita da arvore AVL (e, para
 * comparar, da arvore binaria sem balanceamento).
 *
 * Escolhas da atividade (iguais as da Aula 6):
 *  - Os elementos sao uma PERMUTACAO aleatoria dos valores 0 a 9999
 *    (Fisher-Yates, seed 42), sem chaves repetidas; 50 e 5000 existem.
 *  - Tempo de insercao = MEDIA de 10 execucoes. Cada busca e repetida varias
 *    vezes e o tempo mostrado e o de UMA busca (tempo total / repeticoes).
 *  - Altura em NIVEIS: subarvore vazia = 0, um unico no = 1.
 *
 * Arvore AVL: arvore binaria de busca que, apos cada insercao, confere o fator
 * de balanceamento (altura da esquerda - altura da direita) de cada no no
 * caminho de volta; se ele chega a +2 ou -2, rebalanceia com rotacao simples
 * (direita ou esquerda) ou dupla (esquerda-direita ou direita-esquerda).
 * Isso garante altura <= 1,44 log2(n + 2), logo busca e insercao em O(log n)
 * mesmo no pior caso.
 *
 * Medicao de tempo com clock_gettime() (time.h).
 *
 * Fontes consultadas (algoritmos classicos, implementados a partir deles):
 *  - Cormen et al., Introduction to Algorithms, cap. 12 (arvore binaria de
 *    busca) e Problema 13-3 (arvores AVL).
 *  - N. Ziviani, Projeto de Algoritmos (arvores binarias de pesquisa e AVL).
 *  - G. M. Adelson-Velsky e E. M. Landis, "An algorithm for the organization of
 *    information" (1962), artigo original da arvore AVL.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

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

/* ---------- Arvore AVL ---------- */
typedef struct NoAVL {
    int chave;
    int h;                      /* altura do no, em niveis (folha = 1) */
    struct NoAVL *esq, *dir;
} NoAVL;

long rotacoesSimples = 0;       /* contadores so para analise */
long rotacoesDuplas = 0;

int alturaAVL(const NoAVL *n) { return n ? n->h : 0; }

void atualizarAltura(NoAVL *n) {
    int e = alturaAVL(n->esq), d = alturaAVL(n->dir);
    n->h = 1 + (e > d ? e : d);
}

int fatorBalanceamento(const NoAVL *n) { return alturaAVL(n->esq) - alturaAVL(n->dir); }

NoAVL *rotacaoDireita(NoAVL *y) {
    NoAVL *x = y->esq;
    y->esq = x->dir;
    x->dir = y;
    atualizarAltura(y);
    atualizarAltura(x);
    return x;
}

NoAVL *rotacaoEsquerda(NoAVL *x) {
    NoAVL *y = x->dir;
    x->dir = y->esq;
    y->esq = x;
    atualizarAltura(x);
    atualizarAltura(y);
    return y;
}

NoAVL *balancear(NoAVL *n) {
    atualizarAltura(n);
    int fb = fatorBalanceamento(n);
    if (fb > 1) {                                   /* pesado a esquerda */
        if (fatorBalanceamento(n->esq) < 0) {       /* caso esquerda-direita */
            n->esq = rotacaoEsquerda(n->esq);
            rotacoesDuplas++;
        } else {
            rotacoesSimples++;
        }
        return rotacaoDireita(n);
    }
    if (fb < -1) {                                  /* pesado a direita */
        if (fatorBalanceamento(n->dir) > 0) {       /* caso direita-esquerda */
            n->dir = rotacaoDireita(n->dir);
            rotacoesDuplas++;
        } else {
            rotacoesSimples++;
        }
        return rotacaoEsquerda(n);
    }
    return n;
}

NoAVL *inserirAVL(NoAVL *n, int chave) {
    if (n == NULL) {
        NoAVL *novo = malloc(sizeof(NoAVL));
        novo->chave = chave;
        novo->h = 1;
        novo->esq = novo->dir = NULL;
        return novo;
    }
    if (chave < n->chave) n->esq = inserirAVL(n->esq, chave);
    else if (chave > n->chave) n->dir = inserirAVL(n->dir, chave);
    else return n;                                  /* chave repetida: ignora */
    return balancear(n);
}

int buscaAVL(const NoAVL *n, int chave) {
    while (n != NULL) {
        if (chave == n->chave) return 1;
        n = (chave < n->chave) ? n->esq : n->dir;
    }
    return 0;
}

int comparacoesAVL(const NoAVL *n, int chave) {
    int c = 0;
    while (n != NULL) {
        c++;
        if (chave == n->chave) break;
        n = (chave < n->chave) ? n->esq : n->dir;
    }
    return c;
}

/* confere: ordem de busca, alturas gravadas corretas e |fator| <= 1 em todo no.
 * Devolve a altura real da subarvore, ou -1 se alguma propriedade falhar. */
int verificarAVL(const NoAVL *n, int min, int max) {
    if (n == NULL) return 0;
    if (n->chave < min || n->chave > max) return -1;
    int e = verificarAVL(n->esq, min, n->chave - 1);
    int d = verificarAVL(n->dir, n->chave + 1, max);
    if (e < 0 || d < 0) return -1;
    if (e - d > 1 || d - e > 1) return -1;
    int h = 1 + (e > d ? e : d);
    return (h == n->h) ? h : -1;
}

void liberarAVL(NoAVL *n) {
    if (n == NULL) return;
    liberarAVL(n->esq);
    liberarAVL(n->dir);
    free(n);
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
    double somaVet = 0, somaOrd = 0, somaArv = 0, somaAVL = 0;
    No *raiz = NULL;
    NoAVL *avl = NULL;
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

        liberarAVL(avl);
        avl = NULL;
        rotacoesSimples = rotacoesDuplas = 0; /* conta so a ultima execucao */
        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (int i = 0; i < N; i++) avl = inserirAVL(avl, elementos[i]);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        somaAVL += diffSegundos(t0, t1);
    }
    double insVet = somaVet / EXECUCOES, insOrd = somaOrd / EXECUCOES;
    double insArv = somaArv / EXECUCOES, insAVL = somaAVL / EXECUCOES;

    /* ---------- Verificacao de corretude ---------- */
    int ok = 1;
    for (int i = 0; i < N; i++) {
        if (vetorOrdenado[i] != i) ok = 0;                       /* ordenado = 0,1,2,... */
        if (buscaLinear(vetorAleatorio, N, i) < 0) ok = 0;
        if (buscaBinaria(vetorOrdenado, N, i) < 0) ok = 0;
        if (!buscaArvore(raiz, i)) ok = 0;
        if (!buscaAVL(avl, i)) ok = 0;
    }
    int alturaVerificada = verificarAVL(avl, 0, N - 1);
    if (alturaVerificada < 0 || alturaVerificada != avl->h) ok = 0; /* propriedade AVL */
    if (buscaLinear(vetorAleatorio, N, N + 1) >= 0 || buscaBinaria(vetorOrdenado, N, N + 1) >= 0 ||
        buscaArvore(raiz, N + 1) || buscaAVL(avl, N + 1)) ok = 0; /* elemento inexistente nao pode ser achado */

    /* ---------- Busca dos elementos 50 e 5000 ---------- */
    int chaves[2] = { BUSCA_A, BUSCA_B };
    double bVet[2], bOrd[2], bArv[2], bAVL[2];
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

        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (int r = 0; r < REP_RAPIDA; r++) sink += buscaAVL(avl, chaves[k]);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        bAVL[k] = diffSegundos(t0, t1) / REP_RAPIDA;
    }

    /* ---------- Alturas ---------- */
    int hEsq = altura(raiz->esq), hDir = altura(raiz->dir);
    int aEsq = alturaAVL(avl->esq), aDir = alturaAVL(avl->dir);

    printf("%d elementos (permutacao aleatoria de 0 a %d, seed fixa = 42)\n", N, N - 1);
    printf("Raiz da arvore binaria: %d | raiz da AVL: %d | Verificacao de corretude (inclui propriedade AVL): %s\n",
           raiz->chave, avl->chave, ok ? "OK" : "ERRO");
    printf("Insercao = media de %d execucoes; busca = tempo de UMA busca (repetida %d / %d vezes)\n\n",
           EXECUCOES, REP_LINEAR, REP_RAPIDA);

    printf("%-34s %16s %16s %16s\n", "Estrutura", "Insercao 10.000", "Busca do 50", "Busca do 5000");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("%-34s %13.6f s %13.1f ns %13.1f ns\n", "Vetor aleatorio (busca linear)", insVet, bVet[0] * 1e9, bVet[1] * 1e9);
    printf("%-34s %13.6f s %13.1f ns %13.1f ns\n", "Vetor ordenado (busca binaria)", insOrd, bOrd[0] * 1e9, bOrd[1] * 1e9);
    printf("%-34s %13.6f s %13.1f ns %13.1f ns\n", "Arvore binaria de busca", insArv, bArv[0] * 1e9, bArv[1] * 1e9);
    printf("%-34s %13.6f s %13.1f ns %13.1f ns\n", "Arvore binaria AVL", insAVL, bAVL[0] * 1e9, bAVL[1] * 1e9);
    printf("\nPosicao do 50 e do 5000 no vetor aleatorio (busca linear testa pos+1 elementos): %d e %d\n",
           posVet[0], posVet[1]);

    printf("Comparacoes ate achar o 50 / o 5000: busca linear %d / %d, busca binaria %d / %d, arvore %d / %d, AVL %d / %d\n",
           posVet[0] + 1, posVet[1] + 1,
           comparacoesBinaria(vetorOrdenado, N, BUSCA_A), comparacoesBinaria(vetorOrdenado, N, BUSCA_B),
           comparacoesArvore(raiz, BUSCA_A), comparacoesArvore(raiz, BUSCA_B),
           comparacoesAVL(avl, BUSCA_A), comparacoesAVL(avl, BUSCA_B));

    printf("\n%-34s %22s %22s %16s\n", "Altura", "subarvore esquerda", "subarvore direita", "arvore");
    printf("%-34s %22d %22d %16d\n", "Arvore binaria de busca", hEsq, hDir, altura(raiz));
    printf("%-34s %22d %22d %16d\n", "Arvore binaria AVL", aEsq, aDir, avl->h);
    printf("Altura minima possivel para %d nos: %d | limite teorico da AVL: %.1f\n",
           N, alturaMinima(N), 1.44 * log2(N + 2.0));
    printf("Rotacoes na construcao da AVL: %ld simples + %ld duplas\n", rotacoesSimples, rotacoesDuplas);

    liberarAVL(avl);
    liberarArvore(raiz);

    /* ---------- Pior caso: chaves inseridas em ORDEM CRESCENTE (extra) ----------
     * A arvore binaria sem balanceamento vira uma lista (altura N); a AVL nao. */
    No *raizOrd = NULL;
    NoAVL *avlOrd = NULL;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < N; i++) raizOrd = inserirArvore(raizOrd, i);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double insArvOrd = diffSegundos(t0, t1);
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < N; i++) avlOrd = inserirAVL(avlOrd, i);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double insAVLOrd = diffSegundos(t0, t1);

    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int r = 0; r < 2000; r++) sink += buscaArvore(raizOrd, BUSCA_B);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double buscaArvOrd = diffSegundos(t0, t1) / 2000;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int r = 0; r < REP_RAPIDA; r++) sink += buscaAVL(avlOrd, BUSCA_B);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double buscaAVLOrd = diffSegundos(t0, t1) / REP_RAPIDA;

    printf("\nPior caso (chaves 0 a %d inseridas em ordem crescente, 1 execucao):\n", N - 1);
    printf("%-34s insercao %9.6f s | busca do 5000 %9.1f ns | altura %d\n", "Arvore binaria de busca",
           insArvOrd, buscaArvOrd * 1e9, altura(raizOrd));
    printf("%-34s insercao %9.6f s | busca do 5000 %9.1f ns | altura %d\n", "Arvore binaria AVL",
           insAVLOrd, buscaAVLOrd * 1e9, avlOrd->h);

    liberarArvore(raizOrd);
    liberarAVL(avlOrd);
    return 0;
}

/*
 * Nota de utilizacao de IA: uma LLM (Claude) foi utilizada para auxiliar na
 * formatacao correta e na inclusao de comentarios ao longo do codigo.
 * Alem das partes de impressao na tela (printf's) para melhor visualizacao
 * no momento de execucao.
 */
