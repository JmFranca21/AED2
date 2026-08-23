/*
 * AED2 - Atividade Pratica - Aula 1
 * Joao Marcelo Franca Batista
 *
 * Questao 1: algoritmos recursivo e iterativo para verificar se um numero e
 * primo, aplicados a um vetor de 1000 numeros aleatorios para encontrar o
 * maior primo. Medicao de tempo com clock_gettime() (time.h) e comparacao.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000          /* tamanho do vetor */
#define VALOR_MAX 100000 /* valor maximo sorteado para cada elemento */

/* ---------- Versao ITERATIVA ---------- */
int ehPrimoIterativo(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;

    for (int i = 3; (long)i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

/* ---------- Versao RECURSIVA ---------- */
int ehPrimoRecursivoAux(int n, int divisor) {
    if ((long)divisor * divisor > n) return 1; /* nenhum divisor encontrado */
    if (n % divisor == 0) return 0;
    return ehPrimoRecursivoAux(n, divisor + 2);
}

int ehPrimoRecursivo(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    return ehPrimoRecursivoAux(n, 3);
}

/* calcula a diferenca em segundos entre dois instantes de clock_gettime */
double diffSegundos(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

int main(void) {
    int vetor[N];
    struct timespec inicio, fim;

    srand(42); /* seed fixa: garante o MESMO vetor nas duas execucoes */
    for (int i = 0; i < N; i++) {
        vetor[i] = (rand() % VALOR_MAX) + 1;
    }

    /* ---------- Execucao ITERATIVA (uma unica varredura) ---------- */
    int maiorPrimoIter = -1;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (int i = 0; i < N; i++) {
        if (ehPrimoIterativo(vetor[i]) && vetor[i] > maiorPrimoIter) {
            maiorPrimoIter = vetor[i];
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempoIter = diffSegundos(inicio, fim);

    /* ---------- Execucao RECURSIVA (uma unica varredura) ---------- */
    int maiorPrimoRec = -1;
    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (int i = 0; i < N; i++) {
        if (ehPrimoRecursivo(vetor[i]) && vetor[i] > maiorPrimoRec) {
            maiorPrimoRec = vetor[i];
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempoRec = diffSegundos(inicio, fim);

    printf("Vetor de %d elementos (seed fixa = 42), valores entre 1 e %d\n\n", N, VALOR_MAX);

    printf("== Algoritmo ITERATIVO ==\n");
    printf("Maior primo encontrado: %d\n", maiorPrimoIter);
    printf("Tempo de execucao (1 varredura): %.9f s\n\n", tempoIter);

    printf("== Algoritmo RECURSIVO ==\n");
    printf("Maior primo encontrado: %d\n", maiorPrimoRec);
    printf("Tempo de execucao (1 varredura): %.9f s\n\n", tempoRec);

    if (tempoIter < tempoRec)
        printf("Resultado: o algoritmo ITERATIVO foi mais rapido.\n");
    else if (tempoRec < tempoIter)
        printf("Resultado: o algoritmo RECURSIVO foi mais rapido.\n");
    else
        printf("Resultado: tempos praticamente iguais.\n");

    return 0;
}

/*
 * Nota de utilizacao de IA: uma LLM (Claude) foi utilizada para auxiliar na
 * formatacao correta e na inclusao de comentarios ao longo do codigo.
 * Alem das partes de impressao na tela (printf's) para melhor visualizacao
 * no momento de execucao.
 */
