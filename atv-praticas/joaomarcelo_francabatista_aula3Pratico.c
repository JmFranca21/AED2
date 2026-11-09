/*
 * AED2 - Atividade Pratica - Aula 3
 * Joao Marcelo Franca Batista
 *
 * Questao 1: executar Shellsort, Heapsort, Quicksort recursivo e Quicksort
 * iterativo em vetores de 100.000 elementos (aleatorio, crescente e
 * decrescente) e comparar o tempo de execucao de cada algoritmo. O tempo de
 * cada combinacao (algoritmo x cenario) e a MEDIA de 10 execucoes sobre o
 * mesmo vetor. Medicao de tempo com clock_gettime() (time.h).
 *
 * Escolhas da atividade:
 *  - Shellsort: sequencia de Knuth para o h (h = 3h + 1: 1, 4, 13, 40, ...).
 *  - Quicksort: pivo = mediana de tres (primeiro, do meio e ultimo elemento).
 *    O Quicksort iterativo usa exatamente o mesmo pivo e a mesma funcao de
 *    particao do recursivo; so troca a pilha de chamadas por uma pilha
 *    explicita.
 *
 * Fontes consultadas (algoritmos classicos, implementados a partir deles):
 *  - Shellsort e sequencia de h: D. Knuth, The Art of Computer Programming,
 *    vol. 3 (Sorting and Searching).
 *  - Heapsort: Cormen et al., Introduction to Algorithms (cap. 6).
 *  - Quicksort (particao de Hoare e mediana de tres): C. A. R. Hoare,
 *    "Quicksort" (1962); Cormen et al., Introduction to Algorithms (cap. 7).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100000        /* tamanho de cada vetor de teste */
#define VALOR_MAX 1000000 /* valor maximo sorteado (exclusivo) */
#define EXECUCOES 10    /* quantidade de execucoes para calcular a media */
#define NUM_ALGORITMOS 4
#define NUM_CENARIOS 3
#define TAM_PILHA 64    /* suficiente: sempre empilhamos a metade MAIOR e seguimos na menor */

void trocar(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

/* ---------- Shellsort (h = sequencia de Knuth) ---------- */
void shellSort(int v[], int n) {
    int h = 1;
    while (h < n / 3) h = 3 * h + 1; /* maior h da sequencia 1, 4, 13, 40, ... abaixo de n/3 */

    for (; h >= 1; h /= 3) {
        /* Insertion Sort "com salto h" */
        for (int i = h; i < n; i++) {
            int chave = v[i];
            int j = i;
            while (j >= h && v[j - h] > chave) {
                v[j] = v[j - h];
                j -= h;
            }
            v[j] = chave;
        }
    }
}

/* ---------- Heapsort (max-heap) ---------- */
/* desce o elemento da posicao i ate restaurar a propriedade de max-heap */
void peneirar(int v[], int n, int i) {
    while (1) {
        int maior = i;
        int esq = 2 * i + 1;
        int dir = 2 * i + 2;

        if (esq < n && v[esq] > v[maior]) maior = esq;
        if (dir < n && v[dir] > v[maior]) maior = dir;
        if (maior == i) break;

        trocar(&v[i], &v[maior]);
        i = maior;
    }
}

void heapSort(int v[], int n) {
    /* 1) constroi o max-heap a partir do vetor */
    for (int i = n / 2 - 1; i >= 0; i--) peneirar(v, n, i);

    /* 2) leva o maior (raiz) para o fim e reconstroi o heap menor */
    for (int i = n - 1; i >= 1; i--) {
        trocar(&v[0], &v[i]);
        peneirar(v, i, 0);
    }
}

/* ---------- Quicksort: pivo e particao (compartilhados) ---------- */
/* pivo = mediana de tres valores: primeiro, do meio e ultimo do intervalo */
int medianaDeTres(int v[], int ini, int fim) {
    int a = v[ini];
    int b = v[ini + (fim - ini) / 2];
    int c = v[fim];

    if ((a <= b && b <= c) || (c <= b && b <= a)) return b;
    if ((b <= a && a <= c) || (c <= a && a <= b)) return a;
    return c;
}

/* particao de Hoare: ao final, v[ini..*j] <= pivo <= v[*i..fim] */
void particionar(int v[], int ini, int fim, int *i, int *j) {
    int pivo = medianaDeTres(v, ini, fim);
    int esq = ini;
    int dir = fim;

    while (esq <= dir) {
        while (v[esq] < pivo) esq++;
        while (v[dir] > pivo) dir--;
        if (esq <= dir) {
            trocar(&v[esq], &v[dir]);
            esq++;
            dir--;
        }
    }
    *i = esq;
    *j = dir;
}

/* ---------- Quicksort RECURSIVO ---------- */
void quickSortRecAux(int v[], int ini, int fim) {
    if (ini >= fim) return;

    int i, j;
    particionar(v, ini, fim, &i, &j);
    quickSortRecAux(v, ini, j);
    quickSortRecAux(v, i, fim);
}

void quickSortRecursivo(int v[], int n) {
    quickSortRecAux(v, 0, n - 1);
}

/* ---------- Quicksort ITERATIVO (pilha explicita, mesmo pivo) ---------- */
void quickSortIterativo(int v[], int n) {
    int pilhaIni[TAM_PILHA];
    int pilhaFim[TAM_PILHA];
    int topo = 0;

    pilhaIni[topo] = 0;
    pilhaFim[topo] = n - 1;
    topo++;

    while (topo > 0) {
        topo--;
        int ini = pilhaIni[topo];
        int fim = pilhaFim[topo];

        /* continua na metade MENOR e empilha a MAIOR: a pilha nunca passa de log2(n) */
        while (ini < fim) {
            int i, j;
            particionar(v, ini, fim, &i, &j);

            if (j - ini < fim - i) {
                /* esquerda (ini..j) e a menor: empilha a direita, segue na esquerda */
                if (i < fim) {
                    pilhaIni[topo] = i;
                    pilhaFim[topo] = fim;
                    topo++;
                }
                fim = j;
            } else {
                /* direita (i..fim) e a menor: empilha a esquerda, segue na direita */
                if (ini < j) {
                    pilhaIni[topo] = ini;
                    pilhaFim[topo] = j;
                    topo++;
                }
                ini = i;
            }
        }
    }
}

/* ---------- Funcoes auxiliares (nao fazem parte da comparacao) ---------- */
/* rand() pode ter RAND_MAX = 32767 (ex.: MinGW/Windows); juntar duas chamadas
 * da 30 bits e evita ter so ~32 mil valores distintos no vetor de 100.000. */
int valorAleatorio(void) {
    return (((rand() & 0x7FFF) << 15) | (rand() & 0x7FFF)) % VALOR_MAX;
}

int compararInt(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}

int estaOrdenado(int v[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (v[i] > v[i + 1]) return 0;
    }
    return 1;
}

double diffSegundos(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

/* vetores globais (evita alocar 100.000 inteiros na pilha de main) */
int vetorAleatorio[N];
int vetorCrescente[N];
int vetorDecrescente[N];
int buffer[N];

typedef void (*Ordenador)(int[], int);

/* roda o algoritmo EXECUCOES vezes sobre copias do mesmo vetor e devolve a
 * media do tempo. Se alguma execucao nao ordenar corretamente, avisa. */
double medirMedia(Ordenador ordenar, int original[], int n, const char *nome) {
    struct timespec inicio, fim;
    double soma = 0.0;

    for (int execucao = 0; execucao < EXECUCOES; execucao++) {
        for (int i = 0; i < n; i++) buffer[i] = original[i];

        clock_gettime(CLOCK_MONOTONIC, &inicio);
        ordenar(buffer, n);
        clock_gettime(CLOCK_MONOTONIC, &fim);
        soma += diffSegundos(inicio, fim);

        if (!estaOrdenado(buffer, n)) {
            printf("ERRO: %s nao ordenou corretamente o vetor!\n", nome);
            return -1.0;
        }
    }
    return soma / EXECUCOES;
}

int main(void) {
    const char *nomes[NUM_ALGORITMOS] = {
        "Shellsort (h de Knuth)",
        "Heapsort",
        "Quicksort recursivo",
        "Quicksort iterativo"
    };
    Ordenador algoritmos[NUM_ALGORITMOS] = {
        shellSort, heapSort, quickSortRecursivo, quickSortIterativo
    };
    const char *cenarios[NUM_CENARIOS] = { "Aleatorio", "Crescente", "Decrescente" };
    int *vetores[NUM_CENARIOS] = { vetorAleatorio, vetorCrescente, vetorDecrescente };
    double tempos[NUM_ALGORITMOS][NUM_CENARIOS];

    srand(42); /* seed fixa: garante os MESMOS vetores em toda execucao */

    /* vetor aleatorio */
    for (int i = 0; i < N; i++) vetorAleatorio[i] = valorAleatorio();
    /* vetor crescente: os MESMOS valores do aleatorio, apenas ordenados */
    for (int i = 0; i < N; i++) vetorCrescente[i] = vetorAleatorio[i];
    qsort(vetorCrescente, N, sizeof(int), compararInt);
    /* vetor decrescente: o crescente de tras para frente */
    for (int i = 0; i < N; i++) vetorDecrescente[i] = vetorCrescente[N - 1 - i];

    printf("Vetores de %d elementos (seed fixa = 42), valores entre 0 e %d\n", N, VALOR_MAX - 1);
    printf("Tempo = media de %d execucoes sobre o mesmo vetor\n\n", EXECUCOES);

    for (int a = 0; a < NUM_ALGORITMOS; a++) {
        for (int c = 0; c < NUM_CENARIOS; c++) {
            tempos[a][c] = medirMedia(algoritmos[a], vetores[c], N, nomes[a]);
        }
    }

    printf("%-24s %14s %14s %14s\n", "Algoritmo", cenarios[0], cenarios[1], cenarios[2]);
    printf("------------------------------------------------------------------\n");
    for (int a = 0; a < NUM_ALGORITMOS; a++) {
        printf("%-24s %12.6f s %12.6f s %12.6f s\n",
               nomes[a], tempos[a][0], tempos[a][1], tempos[a][2]);
    }
    printf("\n");

    for (int c = 0; c < NUM_CENARIOS; c++) {
        int melhor = 0;
        for (int a = 1; a < NUM_ALGORITMOS; a++) {
            if (tempos[a][c] < tempos[melhor][c]) melhor = a;
        }
        printf("Mais rapido no vetor %-11s: %s\n", cenarios[c], nomes[melhor]);
    }

    return 0;
}

/*
 * Nota de utilizacao de IA: uma LLM (Claude) foi utilizada para auxiliar na
 * formatacao correta e na inclusao de comentarios ao longo do codigo.
 * Alem das partes de impressao na tela (printf's) para melhor visualizacao
 * no momento de execucao.
 */
