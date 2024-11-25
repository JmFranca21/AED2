#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Função Counting Sort
void countingSort(int arr[], int n, int max) {
    // Array de contagem
    int *count = (int *)calloc(max + 1, sizeof(int));
    int *output = (int *)malloc(n * sizeof(int));

    if (count == NULL || output == NULL) {
        printf("Erro ao alocar memória.\n");
        exit(1);
    }

    // Conta as ocorrências de cada elemento
    for (int i = 0; i < n; i++) {
        count[arr[i]]++;
    }

    // Atualiza o array de contagem para indicar as posições finais
    for (int i = 1; i <= max; i++) {
        count[i] += count[i - 1];
    }

    // Constrói o array ordenado
    for (int i = n - 1; i >= 0; i--) {
        output[count[arr[i]] - 1] = arr[i];
        count[arr[i]]--;
    }

    // Copia o array ordenado de volta para o original
    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }

    // Mostra o array de contagem (opcional)
    printf("Array de contagem: ");
    for (int i = 0; i <= max; i++) {
        printf("%d ", count[i]);
    }
    printf("\n");

    // Libera memória dos arrays auxiliares
    free(count);
    free(output);
}

// Função para gerar números aleatórios no array
void gerarNumerosAleatorios(int arr[], int n, int max) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % (max + 1); // Gera números entre 0 e max
    }
}

int main() {
    int n, max;
    printf("Digite o número de elementos: ");
    scanf("%d", &n);

    printf("Digite o valor máximo para os elementos: ");
    scanf("%d", &max);

    // Aloca o array dinamicamente
    int *arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL) {
        printf("Erro ao alocar memória.\n");
        return 1;
    }

    // Inicializa o gerador de números aleatórios com uma semente baseada no tempo atual
    srand(time(NULL));

    // Preenche o array com números aleatórios
    gerarNumerosAleatorios(arr, n, max);

    printf("Array gerado aleatoriamente:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Marca o tempo antes de iniciar o Counting Sort
    clock_t inicio = clock();
    countingSort(arr, n, max);
    // Marca o tempo após a execução do Counting Sort
    clock_t fim = clock();

    // Calcula o tempo de execução
    double tempo_execucao = ((double)(fim - inicio)) / CLOCKS_PER_SEC;

    printf("\nArray ordenado:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n\nTempo de execução: %.10f segundos\n", tempo_execucao);

    // Libera a memória alocada
    free(arr);

    return 0;
}
