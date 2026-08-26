/*
 * AED2 - Atividade Pratica - Aula 2
 * Joao Marcelo Franca Batista
 *
 * Questao 1: implementacao do Bubble Sort (algoritmo desenvolvido em sala) e
 * comparacao do tempo de execucao com o Insertion Sort (outro algoritmo
 * apresentado na Aula 2), em 4 cenarios de teste com vetores de 10000
 * elementos. Medicao de tempo com clock_gettime() (time.h).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10000          /* tamanho de cada vetor de teste */
#define VALOR_MAX 100000 /* valor maximo sorteado para os inteiros */
#define CHAR_ASCII_MIN 33  /* '!' - inicio do intervalo de caracteres imprimiveis usados */
#define CHAR_ASCII_MAX 126 /* '~' - fim do intervalo de caracteres imprimiveis usados */

/* ---------- Bubble Sort (algoritmo desenvolvido em sala) ---------- */
void bubbleSortInt(int v[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (v[j] > v[j + 1]) {
                int troca = v[j];
                v[j] = v[j + 1];
                v[j + 1] = troca;
            }
        }
    }
}

void bubbleSortChar(char v[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (v[j] > v[j + 1]) {
                char troca = v[j];
                v[j] = v[j + 1];
                v[j + 1] = troca;
            }
        }
    }
}

/* ---------- Insertion Sort (algoritmo usado para comparacao) ---------- */
void insertionSortInt(int v[], int n) {
    for (int i = 1; i < n; i++) {
        int chave = v[i];
        int j = i - 1;
        while (j >= 0 && v[j] > chave) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = chave;
    }
}

void insertionSortChar(char v[], int n) {
    for (int i = 1; i < n; i++) {
        char chave = v[i];
        int j = i - 1;
        while (j >= 0 && v[j] > chave) {
            v[j + 1] = v[j];
            j--;
        }
        v[j + 1] = chave;
    }
}

/* ---------- Funcoes auxiliares (nao fazem parte da comparacao) ---------- */
int compararInt(const void *a, const void *b) {
    return (*(const int *)a) - (*(const int *)b);
}

int compararChar(const void *a, const void *b) {
    return (*(const char *)a) - (*(const char *)b);
}

double diffSegundos(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

/* vetores globais de teste (evita alocar tudo na pilha de main) */
int vetorIntAleatorio[N];
int vetorIntCrescente[N];
char vetorCharAleatorio[N];
char vetorCharCrescente[N];
int bufferInt[N];
char bufferChar[N];

void testarInt(const char *nomeTeste, int original[], int n) {
    struct timespec inicio, fim;

    for (int i = 0; i < n; i++) bufferInt[i] = original[i];
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    bubbleSortInt(bufferInt, n);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempoBubble = diffSegundos(inicio, fim);

    for (int i = 0; i < n; i++) bufferInt[i] = original[i];
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    insertionSortInt(bufferInt, n);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempoInsertion = diffSegundos(inicio, fim);

    printf("== %s ==\n", nomeTeste);
    printf("Bubble Sort:    %.6f s\n", tempoBubble);
    printf("Insertion Sort: %.6f s\n\n", tempoInsertion);
}

void testarChar(const char *nomeTeste, char original[], int n) {
    struct timespec inicio, fim;

    for (int i = 0; i < n; i++) bufferChar[i] = original[i];
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    bubbleSortChar(bufferChar, n);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempoBubble = diffSegundos(inicio, fim);

    for (int i = 0; i < n; i++) bufferChar[i] = original[i];
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    insertionSortChar(bufferChar, n);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempoInsertion = diffSegundos(inicio, fim);

    printf("== %s ==\n", nomeTeste);
    printf("Bubble Sort:    %.6f s\n", tempoBubble);
    printf("Insertion Sort: %.6f s\n\n", tempoInsertion);
}

int main(void) {
    srand(42); /* seed fixa: garante os MESMOS vetores em toda execucao */

    /* vetor de inteiros aleatorios */
    for (int i = 0; i < N; i++) {
        vetorIntAleatorio[i] = rand() % VALOR_MAX;
    }
    /* vetor de inteiros crescente: os MESMOS valores acima, apenas ordenados */
    for (int i = 0; i < N; i++) vetorIntCrescente[i] = vetorIntAleatorio[i];
    qsort(vetorIntCrescente, N, sizeof(int), compararInt);

    /* vetor de caracteres aleatorios (imprimiveis, de '!' a '~') */
    for (int i = 0; i < N; i++) {
        vetorCharAleatorio[i] = CHAR_ASCII_MIN + (rand() % (CHAR_ASCII_MAX - CHAR_ASCII_MIN + 1));
    }
    /* vetor de caracteres crescente: os MESMOS valores acima, apenas ordenados */
    for (int i = 0; i < N; i++) vetorCharCrescente[i] = vetorCharAleatorio[i];
    qsort(vetorCharCrescente, N, sizeof(char), compararChar);

    printf("Vetores de %d elementos (seed fixa = 42)\n\n", N);

    testarInt("Inteiros aleatorios", vetorIntAleatorio, N);
    testarInt("Inteiros crescentes", vetorIntCrescente, N);
    testarChar("Caracteres aleatorios", vetorCharAleatorio, N);
    testarChar("Caracteres crescentes", vetorCharCrescente, N);

    return 0;
}

/*
 * Nota de utilizacao de IA: uma LLM (Claude) foi utilizada para auxiliar na
 * formatacao correta e na inclusao de comentarios ao longo do codigo.
 * Alem das partes de impressao na tela (printf's) para melhor visualizacao
 * no momento de execucao.
 */
