/*
 * AED2 - Atividade Pratica - Aula 4
 * Joao Marcelo Franca Batista
 *
 * Questao 1: gerar um vetor de 1000 datas aleatorias (dia 1-31, mes 1-12,
 * ano 2000-2024) e ordena-las com Radix Sort. O Radix Sort e feito em 3
 * passadas (dia, mes, ano - do menos para o mais significativo), e cada
 * passada usa um "sub-algoritmo" de ordenacao por campo. Para visualizar a
 * efetividade, uma versao usa um sub-algoritmo ESTAVEL (Counting Sort) e a
 * outra usa um sub-algoritmo INSTAVEL (Selection Sort). Medicao de tempo com
 * clock_gettime() (time.h).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000 /* quantidade de datas geradas */

/*
 * TAD Data: agrupar dia, mes e ano numa unica struct e a melhor estrategia
 * aqui porque os 3 campos pertencem a mesma informacao (uma data). Assim o
 * vetor tem 1000 "blocos" de dados, e ordenar o vetor move a data inteira de
 * uma vez, sem o risco de desalinhar dia/mes/ano como aconteceria se
 * usassemos 3 vetores paralelos.
 */
typedef struct {
    int dia;
    int mes;
    int ano;
} Data;

void trocarData(Data *a, Data *b) {
    Data temp = *a;
    *a = *b;
    *b = temp;
}

/* ---------- Counting Sort por campo (ESTAVEL) ---------- */
/* usado como sub-rotina do Radix Sort "estavel" */
void countingSortPorDia(Data v[], int n) {
    int count[32] = {0}; /* dia vai de 1 a 31 */
    Data *saida = malloc(n * sizeof(Data));

    for (int i = 0; i < n; i++) count[v[i].dia]++;
    for (int k = 1; k <= 31; k++) count[k] += count[k - 1];
    for (int i = n - 1; i >= 0; i--) {
        saida[--count[v[i].dia]] = v[i];
    }
    for (int i = 0; i < n; i++) v[i] = saida[i];

    free(saida);
}

void countingSortPorMes(Data v[], int n) {
    int count[13] = {0}; /* mes vai de 1 a 12 */
    Data *saida = malloc(n * sizeof(Data));

    for (int i = 0; i < n; i++) count[v[i].mes]++;
    for (int k = 1; k <= 12; k++) count[k] += count[k - 1];
    for (int i = n - 1; i >= 0; i--) {
        saida[--count[v[i].mes]] = v[i];
    }
    for (int i = 0; i < n; i++) v[i] = saida[i];

    free(saida);
}

void countingSortPorAno(Data v[], int n) {
    int count[26] = {0}; /* ano vai de 2000 a 2024 -> indice = ano - 2000 (0 a 24) */
    Data *saida = malloc(n * sizeof(Data));

    for (int i = 0; i < n; i++) count[v[i].ano - 2000]++;
    for (int k = 1; k <= 24; k++) count[k] += count[k - 1];
    for (int i = n - 1; i >= 0; i--) {
        saida[--count[v[i].ano - 2000]] = v[i];
    }
    for (int i = 0; i < n; i++) v[i] = saida[i];

    free(saida);
}

/* Radix Sort "estavel": 3 passadas de Counting Sort, do dia (menos
 * significativo) ate o ano (mais significativo). */
void radixSortEstavel(Data v[], int n) {
    countingSortPorDia(v, n);
    countingSortPorMes(v, n);
    countingSortPorAno(v, n);
}

/* ---------- Selection Sort por campo (INSTAVEL) ---------- */
/* usado como sub-rotina do Radix Sort "instavel" */
void selectionSortPorDia(Data v[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int indiceMenor = i;
        for (int j = i + 1; j < n; j++) {
            if (v[j].dia < v[indiceMenor].dia) indiceMenor = j;
        }
        if (indiceMenor != i) trocarData(&v[i], &v[indiceMenor]);
    }
}

void selectionSortPorMes(Data v[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int indiceMenor = i;
        for (int j = i + 1; j < n; j++) {
            if (v[j].mes < v[indiceMenor].mes) indiceMenor = j;
        }
        if (indiceMenor != i) trocarData(&v[i], &v[indiceMenor]);
    }
}

void selectionSortPorAno(Data v[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int indiceMenor = i;
        for (int j = i + 1; j < n; j++) {
            if (v[j].ano < v[indiceMenor].ano) indiceMenor = j;
        }
        if (indiceMenor != i) trocarData(&v[i], &v[indiceMenor]);
    }
}

/* Radix Sort "instavel": mesmas 3 passadas, mas com Selection Sort no lugar
 * do Counting Sort. Como o Selection Sort nao e estavel, a ordem conseguida
 * numa passada pode ser perdida na passada seguinte. */
void radixSortInstavel(Data v[], int n) {
    selectionSortPorDia(v, n);
    selectionSortPorMes(v, n);
    selectionSortPorAno(v, n);
}

/* Verifica se o vetor esta corretamente ordenado por (ano, mes, dia) */
int estaOrdenado(Data v[], int n) {
    for (int i = 0; i < n - 1; i++) {
        if (v[i].ano != v[i + 1].ano) {
            if (v[i].ano > v[i + 1].ano) return 0;
            continue;
        }
        if (v[i].mes != v[i + 1].mes) {
            if (v[i].mes > v[i + 1].mes) return 0;
            continue;
        }
        if (v[i].dia > v[i + 1].dia) return 0;
    }
    return 1;
}

void imprimirData(Data d) {
    printf("%02d/%02d/%04d", d.dia, d.mes, d.ano);
}

void imprimirPrimeiras(const char *titulo, Data v[], int quantidade) {
    printf("%s\n", titulo);
    for (int i = 0; i < quantidade; i++) {
        imprimirData(v[i]);
        printf("%s", (i < quantidade - 1) ? "  " : "\n");
    }
}

double diffSegundos(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

Data vetorOriginal[N];
Data bufferEstavel[N];
Data bufferInstavel[N];

int main(void) {
    struct timespec inicio, fim;

    srand(42); /* seed fixa: garante o MESMO vetor em toda execucao */
    for (int i = 0; i < N; i++) {
        vetorOriginal[i].dia = (rand() % 31) + 1;
        vetorOriginal[i].mes = (rand() % 12) + 1;
        vetorOriginal[i].ano = (rand() % 25) + 2000;
    }

    for (int i = 0; i < N; i++) bufferEstavel[i] = vetorOriginal[i];
    for (int i = 0; i < N; i++) bufferInstavel[i] = vetorOriginal[i];

    clock_gettime(CLOCK_MONOTONIC, &inicio);
    radixSortEstavel(bufferEstavel, N);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempoEstavel = diffSegundos(inicio, fim);

    clock_gettime(CLOCK_MONOTONIC, &inicio);
    radixSortInstavel(bufferInstavel, N);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempoInstavel = diffSegundos(inicio, fim);

    printf("Vetor de %d datas (seed fixa = 42)\n\n", N);

    imprimirPrimeiras("Primeiras 10 datas - Radix Sort ESTAVEL (Counting Sort por campo):", bufferEstavel, 10);
    printf("Esta corretamente ordenado? %s\n", estaOrdenado(bufferEstavel, N) ? "SIM" : "NAO");
    printf("Tempo de execucao: %.6f s\n\n", tempoEstavel);

    imprimirPrimeiras("Primeiras 10 datas - Radix Sort INSTAVEL (Selection Sort por campo):", bufferInstavel, 10);
    printf("Esta corretamente ordenado? %s\n", estaOrdenado(bufferInstavel, N) ? "SIM" : "NAO");
    printf("Tempo de execucao: %.6f s\n\n", tempoInstavel);

    if (tempoEstavel < tempoInstavel)
        printf("Resultado: a versao ESTAVEL (Counting Sort) foi mais rapida.\n");
    else if (tempoInstavel < tempoEstavel)
        printf("Resultado: a versao INSTAVEL (Selection Sort) foi mais rapida.\n");
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
