/*
 * AED2 - Atividade Pratica - Aula 9
 * Joao Marcelo Franca Batista
 *
 * Questao 1: simular uma ARVORE B, inserir 100.000 elementos e imprimir:
 *   a) o total de elementos (chaves) em cada no;
 *   b) o tempo de execucao para inserir todos os elementos;
 *   c) a altura da arvore.
 *
 * Implementacao: arvore B de grau minimo T, no algoritmo de Cormen (cap. 18):
 * todo no, exceto a raiz, tem entre T-1 e 2T-1 chaves; todo no tem no maximo
 * 2T filhos; todas as folhas ficam na mesma profundidade. A insercao desce da
 * raiz ate uma folha e DIVIDE, na descida, todo no cheio que encontra (a chave
 * do meio sobe para o pai); se a raiz esta cheia, cria uma nova raiz (unico
 * jeito de a arvore crescer em altura). Principal escolha: T = 50 (ate 99
 * chaves por no, semelhante a um no de indice que ocupa uma pagina de disco).
 *
 * Escolhas da atividade:
 *  - Os 100.000 elementos sao uma PERMUTACAO aleatoria dos valores 1 a 100000
 *    (Fisher-Yates, seed 42), sem chaves repetidas.
 *  - Tempo de insercao = MEDIA de 10 execucoes (a arvore e refeita do zero).
 *  - Altura medida em NIVEIS (so a raiz = 1 nivel) e tambem em arestas.
 *  - Como a arvore tem muitos nos, a lista completa de "chaves por no" so e
 *    impressa com o argumento --todos; por padrao e impresso um resumo por
 *    nivel (quantos nos, minimo, media e maximo de chaves por no, e quantos
 *    nos tem cada quantidade de chaves) e a lista completa dos nos dos
 *    niveis de cima (raiz e seus filhos).
 *  - Extra: o mesmo experimento com outros valores de T e com insercao em
 *    ordem crescente, para comparar a altura, o numero de nos e a ocupacao.
 *
 * Medicao de tempo com clock_gettime() (time.h).
 *
 * Fontes consultadas (algoritmo classico, implementado a partir dela):
 *  - Cormen et al., Introduction to Algorithms, cap. 18 (arvores B:
 *    B-TREE-INSERT, B-TREE-INSERT-NONFULL e B-TREE-SPLIT-CHILD).
 *  - R. Bayer e E. McCreight, "Organization and maintenance of large ordered
 *    indexes" (1972), artigo original da arvore B.
 *  - N. Ziviani, Projeto de Algoritmos (arvores B).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define N 100000            /* elementos inseridos */
#define T_PRINCIPAL 50      /* grau minimo da arvore principal */
#define EXECUCOES 10        /* execucoes para a media do tempo de insercao */
#define MAX_NIVEIS 32

typedef struct No {
    int n;                  /* quantidade de chaves no no */
    int folha;              /* 1 se e folha */
    int *chaves;            /* 2T-1 posicoes */
    struct No **filhos;     /* 2T posicoes (so usadas em nos internos) */
} No;

typedef struct {
    int t;
    No *raiz;
    long numNos;
} ArvoreB;

No *criarNo(ArvoreB *a, int folha) {
    No *x = malloc(sizeof(No));
    x->n = 0;
    x->folha = folha;
    x->chaves = malloc((size_t)(2 * a->t - 1) * sizeof(int));
    x->filhos = folha ? NULL : malloc((size_t)(2 * a->t) * sizeof(No *));
    a->numNos++;
    return x;
}

ArvoreB *criarArvore(int t) {
    ArvoreB *a = malloc(sizeof(ArvoreB));
    a->t = t;
    a->numNos = 0;
    a->raiz = criarNo(a, 1);
    return a;
}

void liberarNo(No *x) {
    if (!x->folha) {
        for (int i = 0; i <= x->n; i++) liberarNo(x->filhos[i]);
        free(x->filhos);
    }
    free(x->chaves);
    free(x);
}

void liberarArvore(ArvoreB *a) {
    liberarNo(a->raiz);
    free(a);
}

/* divide o filho cheio x->filhos[i]: a chave do meio sobe para x */
void dividirFilho(ArvoreB *a, No *x, int i) {
    int t = a->t;
    No *y = x->filhos[i];               /* filho cheio, com 2t-1 chaves */
    No *z = criarNo(a, y->folha);       /* novo irmao a direita */
    z->n = t - 1;
    for (int j = 0; j < t - 1; j++) z->chaves[j] = y->chaves[j + t];
    if (!y->folha) {
        for (int j = 0; j < t; j++) z->filhos[j] = y->filhos[j + t];
    }
    y->n = t - 1;
    for (int j = x->n; j > i; j--) x->filhos[j + 1] = x->filhos[j];
    x->filhos[i + 1] = z;
    for (int j = x->n - 1; j >= i; j--) x->chaves[j + 1] = x->chaves[j];
    x->chaves[i] = y->chaves[t - 1];    /* chave do meio sobe */
    x->n++;
}

void inserirNaoCheio(ArvoreB *a, No *x, int k) {
    int i = x->n - 1;
    if (x->folha) {
        while (i >= 0 && k < x->chaves[i]) {
            x->chaves[i + 1] = x->chaves[i];
            i--;
        }
        x->chaves[i + 1] = k;
        x->n++;
        return;
    }
    while (i >= 0 && k < x->chaves[i]) i--;
    i++;
    if (x->filhos[i]->n == 2 * a->t - 1) {      /* desce so em no nao cheio */
        dividirFilho(a, x, i);
        if (k > x->chaves[i]) i++;
    }
    inserirNaoCheio(a, x->filhos[i], k);
}

void inserir(ArvoreB *a, int k) {
    No *r = a->raiz;
    if (r->n == 2 * a->t - 1) {                 /* raiz cheia: a arvore cresce */
        No *s = criarNo(a, 0);
        s->filhos[0] = r;
        a->raiz = s;
        dividirFilho(a, s, 0);
        inserirNaoCheio(a, s, k);
    } else {
        inserirNaoCheio(a, r, k);
    }
}

int buscar(const No *x, int k) {
    while (1) {
        int i = 0;
        while (i < x->n && k > x->chaves[i]) i++;
        if (i < x->n && k == x->chaves[i]) return 1;
        if (x->folha) return 0;
        x = x->filhos[i];
    }
}

/* ---------- Estatisticas e verificacao ---------- */
typedef struct {
    long nos[MAX_NIVEIS];
    long chaves[MAX_NIVEIS];
    int minimo[MAX_NIVEIS], maximo[MAX_NIVEIS];
    long *hist[MAX_NIVEIS];     /* hist[nivel][q] = quantos nos tem q chaves */
    int niveis;
    int ok;
} Estat;

void coletar(const ArvoreB *a, const No *x, int nivel, int raiz, Estat *e, int *profFolha) {
    int t = a->t;
    if (nivel >= e->niveis) e->niveis = nivel + 1;
    e->nos[nivel]++;
    e->chaves[nivel] += x->n;
    if (x->n < e->minimo[nivel]) e->minimo[nivel] = x->n;
    if (x->n > e->maximo[nivel]) e->maximo[nivel] = x->n;
    e->hist[nivel][x->n]++;
    /* propriedades da arvore B de Cormen */
    if (x->n > 2 * t - 1) e->ok = 0;
    if (!raiz && x->n < t - 1) e->ok = 0;
    for (int i = 0; i + 1 < x->n; i++) if (x->chaves[i] >= x->chaves[i + 1]) e->ok = 0;
    if (x->folha) {
        if (*profFolha < 0) *profFolha = nivel;
        else if (*profFolha != nivel) e->ok = 0;    /* todas as folhas no mesmo nivel */
    } else {
        for (int i = 0; i <= x->n; i++) coletar(a, x->filhos[i], nivel + 1, 0, e, profFolha);
    }
}

Estat estatisticas(const ArvoreB *a) {
    Estat e;
    memset(&e, 0, sizeof(e));
    for (int i = 0; i < MAX_NIVEIS; i++) {
        e.minimo[i] = 1 << 30;
        e.hist[i] = calloc((size_t)(2 * a->t), sizeof(long));
    }
    e.ok = 1;
    int profFolha = -1;
    coletar(a, a->raiz, 0, 1, &e, &profFolha);
    return e;
}

void liberarEstat(Estat *e) {
    for (int i = 0; i < MAX_NIVEIS; i++) free(e->hist[i]);
}

/* percurso em ordem: confere que as chaves saem crescentes e conta quantas sao */
long emOrdem(const No *x, int *ultima, int *ok) {
    long c = 0;
    for (int i = 0; i < x->n; i++) {
        if (!x->folha) c += emOrdem(x->filhos[i], ultima, ok);
        if (x->chaves[i] <= *ultima) *ok = 0;
        *ultima = x->chaves[i];
        c++;
    }
    if (!x->folha) c += emOrdem(x->filhos[x->n], ultima, ok);
    return c;
}

/* lista as quantidades de chaves dos nos de um nivel (da esquerda para a direita) */
void listarNivel(const No *x, int nivel, int alvo, int *conta) {
    if (nivel == alvo) {
        if (*conta % 20 == 0) printf("\n    ");
        printf("%d ", x->n);
        (*conta)++;
        return;
    }
    if (x->folha) return;
    for (int i = 0; i <= x->n; i++) listarNivel(x->filhos[i], nivel + 1, alvo, conta);
}

/* ---------- Funcoes auxiliares (nao fazem parte da medicao) ---------- */
double diffSegundos(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

int sorteio(int limite) {
    return (((rand() & 0x7FFF) << 15) | (rand() & 0x7FFF)) % limite;
}

static int elementos[N];

void relatorioPrincipal(const ArvoreB *a, int todos) {
    Estat e = estatisticas(a);
    int t = a->t;
    int ult = 0, ok = 1;
    long total = emOrdem(a->raiz, &ult, &ok);

    printf("Arvore B de grau minimo T = %d (cada no tem de %d a %d chaves, exceto a raiz; ate %d filhos)\n",
           t, t - 1, 2 * t - 1, 2 * t);
    printf("Chaves na arvore: %ld | nos: %ld | em ordem crescente: %s | propriedades da arvore B: %s\n\n",
           total, a->numNos, ok ? "SIM" : "NAO", e.ok ? "OK" : "ERRO");

    printf("c) ALTURA: %d niveis (%d arestas da raiz ate uma folha)\n", e.niveis, e.niveis - 1);
    printf("   limite teorico: h <= log_T((n+1)/2) = %.2f arestas\n\n", log((total + 1) / 2.0) / log((double)t));

    printf("a) Chaves por no, resumo por nivel:\n");
    printf("   %-6s %8s %10s %10s %10s %12s\n", "nivel", "nos", "minimo", "media", "maximo", "chaves");
    for (int i = 0; i < e.niveis; i++) {
        printf("   %-6d %8ld %10d %10.1f %10d %12ld%s\n", i, e.nos[i], e.minimo[i],
               (double)e.chaves[i] / e.nos[i], e.maximo[i], e.chaves[i],
               i == 0 ? "   (raiz)" : (i == e.niveis - 1 ? "   (folhas)" : ""));
    }
    printf("\n   Chaves em cada no do(s) nivel(is) de cima:\n");
    for (int i = 0; i < e.niveis - 1; i++) {
        printf("   nivel %d (%ld no(s)):", i, e.nos[i]);
        int conta = 0;
        listarNivel(a->raiz, 0, i, &conta);
        printf("\n");
    }
    printf("\n   Folhas: quantos nos tem cada quantidade de chaves (so as quantidades que aparecem):\n   ");
    int ult2 = e.niveis - 1;
    for (int q = 0; q < 2 * t; q++) {
        if (e.hist[ult2][q] > 0) printf("[%d chaves: %ld nos] ", q, e.hist[ult2][q]);
    }
    printf("\n");
    if (todos) {
        printf("\n   Lista completa (todos os nos, por nivel):\n");
        for (int i = 0; i < e.niveis; i++) {
            printf("   nivel %d:", i);
            int conta = 0;
            listarNivel(a->raiz, 0, i, &conta);
            printf("\n");
        }
    }
    liberarEstat(&e);
}

int main(int argc, char *argv[]) {
    int todos = (argc > 1 && strcmp(argv[1], "--todos") == 0);
    struct timespec t0, t1;

    /* permutacao aleatoria de 1..N (Fisher-Yates), seed fixa */
    srand(42);
    for (int i = 0; i < N; i++) elementos[i] = i + 1;
    for (int i = N - 1; i > 0; i--) {
        int j = sorteio(i + 1);
        int tmp = elementos[i];
        elementos[i] = elementos[j];
        elementos[j] = tmp;
    }

    printf("Arvore B: insercao de %d elementos (permutacao aleatoria de 1 a %d, seed fixa = 42)\n\n", N, N);

    /* ---------- b) tempo de insercao (media de EXECUCOES execucoes) ---------- */
    double soma = 0.0;
    ArvoreB *a = NULL;
    for (int e = 0; e < EXECUCOES; e++) {
        if (a) liberarArvore(a);
        a = criarArvore(T_PRINCIPAL);
        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (int i = 0; i < N; i++) inserir(a, elementos[i]);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        soma += diffSegundos(t0, t1);
    }
    printf("b) TEMPO para inserir os %d elementos (media de %d execucoes): %.6f s\n\n", N, EXECUCOES, soma / EXECUCOES);

    int achou = 1;
    for (int i = 1; i <= N; i++) if (!buscar(a->raiz, i)) achou = 0;
    if (buscar(a->raiz, 0) || buscar(a->raiz, N + 1)) achou = 0;
    printf("Verificacao: todas as chaves de 1 a %d encontradas e as inexistentes nao: %s\n\n", N, achou ? "SIM" : "NAO");

    relatorioPrincipal(a, todos);
    liberarArvore(a);

    /* ---------- Extra: outros graus minimos e insercao em ordem crescente ---------- */
    printf("\n=== Extra: efeito do grau minimo T e da ordem de insercao (media de 5 execucoes) ===\n");
    printf("%-4s %-10s %8s %8s %10s %12s %14s\n", "T", "ordem", "niveis", "nos", "chaves/no", "ocupacao", "tempo insercao");
    int graus[] = { 2, 5, 10, 50, 100, 500 };
    for (int g = 0; g < 6; g++) {
        for (int modo = 0; modo < 2; modo++) {              /* 0 = aleatoria, 1 = crescente */
            double s = 0;
            ArvoreB *b = NULL;
            for (int e = 0; e < 5; e++) {
                if (b) liberarArvore(b);
                b = criarArvore(graus[g]);
                clock_gettime(CLOCK_MONOTONIC, &t0);
                for (int i = 0; i < N; i++) inserir(b, modo == 0 ? elementos[i] : i + 1);
                clock_gettime(CLOCK_MONOTONIC, &t1);
                s += diffSegundos(t0, t1);
            }
            Estat es = estatisticas(b);
            printf("%-4d %-10s %8d %8ld %10.1f %11.1f%% %12.6f s%s\n", graus[g], modo == 0 ? "aleatoria" : "crescente",
                   es.niveis, b->numNos, (double)N / b->numNos,
                   100.0 * N / ((double)b->numNos * (2 * graus[g] - 1)), s / 5, es.ok ? "" : "  ERRO");
            liberarEstat(&es);
            liberarArvore(b);
        }
    }
    return 0;
}

/*
 * Nota de utilizacao de IA: uma LLM (Claude) foi utilizada para auxiliar na
 * formatacao correta e na inclusao de comentarios ao longo do codigo.
 * Alem das partes de impressao na tela (printf's) para melhor visualizacao
 * no momento de execucao.
 */
