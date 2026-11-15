/*
 * AED2 - Atividade Pratica - Aula 8
 * Joao Marcelo Franca Batista
 *
 * Questao 1: inserir 100.000 elementos aleatorios em uma ARVORE VERMELHO-PRETO
 * e medir o tempo de insercao e o tempo de busca dos elementos 50 e 50.000
 * (o enunciado escreve "50.000" no texto e "5000" na tabela; foi usado 50.000,
 * que e o valor do texto). Para comparar (item d), o MESMO programa refaz as
 * medicoes das aulas anteriores com os mesmos 100.000 elementos: vetor
 * aleatorio (busca linear), vetor ordenado (busca binaria), arvore binaria de
 * busca sem balanceamento e arvore AVL. Assim os cinco tempos saem da mesma
 * execucao, na mesma maquina, e com o mesmo tamanho (nas aulas 6 e 7 o
 * tamanho era 10.000).
 * Questao 2: altura das subarvores esquerda e direita da arvore vermelho-preto,
 * comparadas com as da arvore binaria e da AVL.
 *
 * Escolhas da atividade (iguais as das aulas 6 e 7):
 *  - Os elementos sao uma PERMUTACAO aleatoria dos valores 0 a 99999
 *    (Fisher-Yates, seed 42), sem chaves repetidas; 50 e 50000 existem.
 *  - Tempo de insercao = MEDIA de 10 execucoes (3 execucoes no vetor ordenado,
 *    que leva varios segundos por execucao com 100.000 elementos). Cada busca
 *    e repetida varias vezes e o tempo mostrado e o de UMA busca: na busca
 *    linear, a media das repeticoes; nas demais (que levam dezenas de ns), o
 *    MENOR tempo medio entre 5 lotes de 400.000 buscas, porque com 100.000
 *    elementos o ruido do sistema/cache faz uma media unica oscilar muito.
 *  - Altura em NIVEIS: subarvore vazia = 0, um unico no = 1.
 *
 * Arvore vermelho-preto (propriedades): (1) todo no e vermelho ou preto; (2) a
 * raiz e preta; (3) as folhas (NIL) sao pretas; (4) um no vermelho so tem
 * filhos pretos; (5) todos os caminhos de um no ate as folhas abaixo dele tem
 * o mesmo numero de nos pretos. Disso sai altura <= 2 log2(n + 1). A insercao
 * e a de uma arvore binaria de busca, pintando o novo no de vermelho, seguida
 * de um conserto (recolorir e/ou rotacionar) que restaura as propriedades.
 *
 * Medicao de tempo com clock_gettime() (time.h).
 *
 * Fontes consultadas (algoritmos classicos, implementados a partir deles):
 *  - Cormen et al., Introduction to Algorithms, cap. 13 (arvores
 *    vermelho-preto: rotacoes, RB-INSERT e RB-INSERT-FIXUP, com sentinela NIL).
 *  - R. Sedgewick e K. Wayne, Algorithms (propriedades e altura das arvores
 *    vermelho-preto).
 *  - Codigo da AVL e da arvore binaria: o mesmo da Aula 7.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define N 100000            /* elementos inseridos */
#define EXECUCOES 10        /* execucoes para a media do tempo de insercao */
#define EXEC_ORDENADO 3     /* execucoes do vetor ordenado (cada uma leva varios segundos) */
#define REP_LINEAR 2000     /* repeticoes da busca linear (vetor aleatorio) */
#define REP_RAPIDA 2000000  /* total de buscas rapidas = LOTES * REP_LOTE */
#define LOTES 5             /* buscas rapidas: menor media entre LOTES lotes de REP_LOTE buscas */
#define REP_LOTE 400000
#define BUSCA_A 50
#define BUSCA_B 50000

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

/* ---------- Arvore vermelho-preto (CLRS, com sentinela NIL) ---------- */
#define VERMELHO 0
#define PRETO 1

typedef struct NoRB {
    int chave;
    int cor;
    struct NoRB *esq, *dir, *pai;
} NoRB;

NoRB sentinela;                 /* NIL: unica folha, sempre preta */
NoRB *NIL = &sentinela;

long rbRotacoes = 0;            /* contadores so para analise */
long rbRecolorizacoes = 0;

void iniciarRB(void) {
    sentinela.cor = PRETO;
    sentinela.chave = -1;
    sentinela.esq = sentinela.dir = sentinela.pai = &sentinela;
}

void rotacaoEsquerdaRB(NoRB **raiz, NoRB *x) {
    NoRB *y = x->dir;
    x->dir = y->esq;
    if (y->esq != NIL) y->esq->pai = x;
    y->pai = x->pai;
    if (x->pai == NIL) *raiz = y;
    else if (x == x->pai->esq) x->pai->esq = y;
    else x->pai->dir = y;
    y->esq = x;
    x->pai = y;
    rbRotacoes++;
}

void rotacaoDireitaRB(NoRB **raiz, NoRB *y) {
    NoRB *x = y->esq;
    y->esq = x->dir;
    if (x->dir != NIL) x->dir->pai = y;
    x->pai = y->pai;
    if (y->pai == NIL) *raiz = x;
    else if (y == y->pai->esq) y->pai->esq = x;
    else y->pai->dir = x;
    x->dir = y;
    y->pai = x;
    rbRotacoes++;
}

/* restaura as propriedades depois de inserir o no vermelho z */
void consertarInsercaoRB(NoRB **raiz, NoRB *z) {
    while (z->pai->cor == VERMELHO) {
        if (z->pai == z->pai->pai->esq) {
            NoRB *tio = z->pai->pai->dir;
            if (tio->cor == VERMELHO) {                 /* caso 1: tio vermelho -> recolore */
                z->pai->cor = PRETO;
                tio->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                z = z->pai->pai;
                rbRecolorizacoes++;
            } else {
                if (z == z->pai->dir) {                 /* caso 2: vira o caso 3 */
                    z = z->pai;
                    rotacaoEsquerdaRB(raiz, z);
                }
                z->pai->cor = PRETO;                    /* caso 3: recolore e rotaciona */
                z->pai->pai->cor = VERMELHO;
                rotacaoDireitaRB(raiz, z->pai->pai);
            }
        } else {                                        /* simetrico: pai e filho direito */
            NoRB *tio = z->pai->pai->esq;
            if (tio->cor == VERMELHO) {
                z->pai->cor = PRETO;
                tio->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                z = z->pai->pai;
                rbRecolorizacoes++;
            } else {
                if (z == z->pai->esq) {
                    z = z->pai;
                    rotacaoDireitaRB(raiz, z);
                }
                z->pai->cor = PRETO;
                z->pai->pai->cor = VERMELHO;
                rotacaoEsquerdaRB(raiz, z->pai->pai);
            }
        }
    }
    (*raiz)->cor = PRETO;                               /* a raiz e sempre preta */
}

void inserirRB(NoRB **raiz, int chave) {
    NoRB *pai = NIL, *x = *raiz;
    while (x != NIL) {
        pai = x;
        if (chave < x->chave) x = x->esq;
        else if (chave > x->chave) x = x->dir;
        else return;                                    /* chave repetida: ignora */
    }
    NoRB *z = malloc(sizeof(NoRB));
    z->chave = chave;
    z->cor = VERMELHO;
    z->esq = z->dir = NIL;
    z->pai = pai;
    if (pai == NIL) *raiz = z;
    else if (chave < pai->chave) pai->esq = z;
    else pai->dir = z;
    consertarInsercaoRB(raiz, z);
}

int buscaRB(const NoRB *n, int chave) {
    while (n != NIL) {
        if (chave == n->chave) return 1;
        n = (chave < n->chave) ? n->esq : n->dir;
    }
    return 0;
}

int comparacoesRB(const NoRB *n, int chave) {
    int c = 0;
    while (n != NIL) {
        c++;
        if (chave == n->chave) break;
        n = (chave < n->chave) ? n->esq : n->dir;
    }
    return c;
}

int alturaRB(const NoRB *n) {
    if (n == NIL) return 0;
    int e = alturaRB(n->esq), d = alturaRB(n->dir);
    return 1 + (e > d ? e : d);
}

/* confere ordem de busca e as propriedades 4 e 5; devolve a altura negra da
 * subarvore (nos pretos ate a folha, contando a folha) ou -1 se algo falhar */
int verificarRB(const NoRB *n, int min, int max) {
    if (n == NIL) return 1;
    if (n->chave < min || n->chave > max) return -1;
    if (n->cor == VERMELHO && (n->esq->cor == VERMELHO || n->dir->cor == VERMELHO)) return -1;
    int e = verificarRB(n->esq, min, n->chave - 1);
    int d = verificarRB(n->dir, n->chave + 1, max);
    if (e < 0 || d < 0 || e != d) return -1;
    return e + (n->cor == PRETO ? 1 : 0);
}

/* quantos nos vermelhos existem (so para analise) */
long contarVermelhos(const NoRB *n) {
    if (n == NIL) return 0;
    return (n->cor == VERMELHO) + contarVermelhos(n->esq) + contarVermelhos(n->dir);
}

void liberarRB(NoRB *n) {
    if (n == NIL) return;
    liberarRB(n->esq);
    liberarRB(n->dir);
    free(n);
}

/* mede UMA busca rapida: roda LOTES lotes de REP_LOTE buscas e guarda o menor
 * tempo medio por busca (o menor lote e o menos atrapalhado por outros
 * processos e pelo sistema operacional) */
#define MEDIR_RAPIDA(destino, chamada)                                        \
    do {                                                                      \
        double melhor = 1e9;                                                  \
        for (int lote = 0; lote < LOTES; lote++) {                            \
            clock_gettime(CLOCK_MONOTONIC, &t0);                              \
            for (int r = 0; r < REP_LOTE; r++) sink += (chamada);             \
            clock_gettime(CLOCK_MONOTONIC, &t1);                              \
            double tempo = diffSegundos(t0, t1) / REP_LOTE;                   \
            if (tempo < melhor) melhor = tempo;                               \
        }                                                                     \
        (destino) = melhor;                                                   \
    } while (0)

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
    static int elementos[N]; /* estatico: 100.000 inteiros nao cabem com folga na pilha */

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
    double somaVet = 0, somaOrd = 0, somaArv = 0, somaAVL = 0, somaRB = 0;
    No *raiz = NULL;
    NoAVL *avl = NULL;
    NoRB *rb = NIL;
    iniciarRB();
    int nOrd = 0;

    for (int e = 0; e < EXECUCOES; e++) {
        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (int i = 0; i < N; i++) vetorAleatorio[i] = elementos[i];
        clock_gettime(CLOCK_MONOTONIC, &t1);
        somaVet += diffSegundos(t0, t1);

        if (e < EXEC_ORDENADO) {                 /* so algumas execucoes: cada uma leva segundos */
            nOrd = 0;
            clock_gettime(CLOCK_MONOTONIC, &t0);
            for (int i = 0; i < N; i++) inserirOrdenado(vetorOrdenado, &nOrd, elementos[i]);
            clock_gettime(CLOCK_MONOTONIC, &t1);
            somaOrd += diffSegundos(t0, t1);
        }

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

        liberarRB(rb);
        rb = NIL;
        rbRotacoes = rbRecolorizacoes = 0; /* conta so a ultima execucao */
        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (int i = 0; i < N; i++) inserirRB(&rb, elementos[i]);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        somaRB += diffSegundos(t0, t1);
    }
    double insVet = somaVet / EXECUCOES, insOrd = somaOrd / EXEC_ORDENADO;
    double insArv = somaArv / EXECUCOES, insAVL = somaAVL / EXECUCOES, insRB = somaRB / EXECUCOES;

    /* ---------- Verificacao de corretude ---------- */
    int ok = 1;
    for (int i = 0; i < N; i++) {
        if (vetorOrdenado[i] != i) ok = 0;                       /* ordenado = 0,1,2,... */
        if (buscaLinear(vetorAleatorio, N, i) < 0) ok = 0;
        if (buscaBinaria(vetorOrdenado, N, i) < 0) ok = 0;
        if (!buscaArvore(raiz, i)) ok = 0;
        if (!buscaAVL(avl, i)) ok = 0;
        if (!buscaRB(rb, i)) ok = 0;
    }
    int alturaVerificada = verificarAVL(avl, 0, N - 1);
    if (alturaVerificada < 0 || alturaVerificada != avl->h) ok = 0; /* propriedade AVL */
    int alturaNegra = verificarRB(rb, 0, N - 1);
    if (alturaNegra < 0 || rb->cor != PRETO) ok = 0;               /* propriedades vermelho-preto */
    if (buscaLinear(vetorAleatorio, N, N + 1) >= 0 || buscaBinaria(vetorOrdenado, N, N + 1) >= 0 ||
        buscaArvore(raiz, N + 1) || buscaAVL(avl, N + 1) || buscaRB(rb, N + 1)) ok = 0; /* elemento inexistente nao pode ser achado */

    /* ---------- Busca dos elementos 50 e 50000 ---------- */
    int chaves[2] = { BUSCA_A, BUSCA_B };
    double bVet[2], bOrd[2], bArv[2], bAVL[2], bRB[2];
    int posVet[2];
    volatile int sink = 0;

    for (int k = 0; k < 2; k++) {
        posVet[k] = buscaLinear(vetorAleatorio, N, chaves[k]);

        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (int r = 0; r < REP_LINEAR; r++) sink += buscaLinear(vetorAleatorio, N, chaves[k]);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        bVet[k] = diffSegundos(t0, t1) / REP_LINEAR;

        MEDIR_RAPIDA(bOrd[k], buscaBinaria(vetorOrdenado, N, chaves[k]));

        MEDIR_RAPIDA(bArv[k], buscaArvore(raiz, chaves[k]));

        MEDIR_RAPIDA(bAVL[k], buscaAVL(avl, chaves[k]));

        MEDIR_RAPIDA(bRB[k], buscaRB(rb, chaves[k]));
    }

    /* ---------- Alturas ---------- */
    int hEsq = altura(raiz->esq), hDir = altura(raiz->dir);
    int aEsq = alturaAVL(avl->esq), aDir = alturaAVL(avl->dir);
    int rEsq = alturaRB(rb->esq), rDir = alturaRB(rb->dir);

    printf("%d elementos (permutacao aleatoria de 0 a %d, seed fixa = 42)\n", N, N - 1);
    printf("Raiz da arvore binaria: %d | raiz da AVL: %d | raiz da vermelho-preto: %d\n",
           raiz->chave, avl->chave, rb->chave);
    printf("Verificacao de corretude (todas as chaves achadas; propriedades AVL e vermelho-preto): %s\n", ok ? "OK" : "ERRO");
    printf("Insercao = media de %d execucoes. Busca = tempo de UMA busca: busca linear = media de %d repeticoes;\n"
           "demais = menor media entre %d lotes de %d buscas\n\n",
           EXECUCOES, REP_LINEAR, LOTES, REP_LOTE);

    printf("%-34s %16s %16s %16s\n", "Estrutura", "Insercao 100.000", "Busca do 50", "Busca do 50000");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("%-34s %13.6f s %13.1f ns %13.1f ns\n", "Vetor aleatorio (busca linear)", insVet, bVet[0] * 1e9, bVet[1] * 1e9);
    printf("%-34s %13.6f s %13.1f ns %13.1f ns\n", "Vetor ordenado (busca binaria)", insOrd, bOrd[0] * 1e9, bOrd[1] * 1e9);
    printf("%-34s %13.6f s %13.1f ns %13.1f ns\n", "Arvore binaria de busca", insArv, bArv[0] * 1e9, bArv[1] * 1e9);
    printf("%-34s %13.6f s %13.1f ns %13.1f ns\n", "Arvore binaria AVL", insAVL, bAVL[0] * 1e9, bAVL[1] * 1e9);
    printf("%-34s %13.6f s %13.1f ns %13.1f ns\n", "Arvore vermelho-preto", insRB, bRB[0] * 1e9, bRB[1] * 1e9);
    printf("\nPosicao do 50 e do 50000 no vetor aleatorio (busca linear testa pos+1 elementos): %d e %d\n",
           posVet[0], posVet[1]);

    printf("Comparacoes ate achar o 50 / o 50000: linear %d / %d, binaria %d / %d, arvore %d / %d, AVL %d / %d, vermelho-preto %d / %d\n",
           posVet[0] + 1, posVet[1] + 1,
           comparacoesBinaria(vetorOrdenado, N, BUSCA_A), comparacoesBinaria(vetorOrdenado, N, BUSCA_B),
           comparacoesArvore(raiz, BUSCA_A), comparacoesArvore(raiz, BUSCA_B),
           comparacoesAVL(avl, BUSCA_A), comparacoesAVL(avl, BUSCA_B),
           comparacoesRB(rb, BUSCA_A), comparacoesRB(rb, BUSCA_B));

    printf("\n%-34s %22s %22s %16s\n", "Altura", "subarvore esquerda", "subarvore direita", "arvore");
    printf("%-34s %22d %22d %16d\n", "Arvore binaria de busca", hEsq, hDir, altura(raiz));
    printf("%-34s %22d %22d %16d\n", "Arvore binaria AVL", aEsq, aDir, avl->h);
    printf("%-34s %22d %22d %16d\n", "Arvore vermelho-preto", rEsq, rDir, alturaRB(rb));
    printf("Altura minima possivel para %d nos: %d | limite da AVL: %.1f | limite da vermelho-preto: %.1f\n",
           N, alturaMinima(N), 1.44 * log2(N + 2.0), 2.0 * log2(N + 1.0));
    printf("Rotacoes na construcao da AVL: %ld simples + %ld duplas\n", rotacoesSimples, rotacoesDuplas);
    printf("Vermelho-preto: %ld rotacoes e %ld recolorizacoes de 'caso 1' na construcao | altura negra: %d | nos vermelhos: %ld (%.1f%%)\n",
           rbRotacoes, rbRecolorizacoes, alturaNegra - 1, contarVermelhos(rb), 100.0 * contarVermelhos(rb) / N);

    liberarAVL(avl);
    liberarArvore(raiz);
    liberarRB(rb);
    rb = NIL;

    /* ---------- Extra: chaves inseridas em ORDEM CRESCENTE ----------
     * A arvore binaria sem balanceamento viraria uma lista (altura N, insercao
     * O(n^2)), por isso so as duas arvores balanceadas sao comparadas aqui. */
    NoAVL *avlOrd = NULL;
    NoRB *rbOrd = NIL;
    rotacoesSimples = rotacoesDuplas = 0;
    rbRotacoes = rbRecolorizacoes = 0;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < N; i++) avlOrd = inserirAVL(avlOrd, i);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double insAVLOrd = diffSegundos(t0, t1);
    clock_gettime(CLOCK_MONOTONIC, &t0);
    for (int i = 0; i < N; i++) inserirRB(&rbOrd, i);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double insRBOrd = diffSegundos(t0, t1);

    printf("\nChaves 0 a %d inseridas em ordem crescente (1 execucao):\n", N - 1);
    printf("%-34s insercao %9.6f s | altura %d | rotacoes: %ld simples + %ld duplas\n", "Arvore binaria AVL",
           insAVLOrd, avlOrd->h, rotacoesSimples, rotacoesDuplas);
    printf("%-34s insercao %9.6f s | altura %d | rotacoes: %ld\n", "Arvore vermelho-preto",
           insRBOrd, alturaRB(rbOrd), rbRotacoes);

    liberarAVL(avlOrd);
    liberarRB(rbOrd);
    return 0;
}

/*
 * Nota de utilizacao de IA: uma LLM (Claude) foi utilizada para auxiliar na
 * formatacao correta e na inclusao de comentarios ao longo do codigo.
 * Alem das partes de impressao na tela (printf's) para melhor visualizacao
 * no momento de execucao.
 */
