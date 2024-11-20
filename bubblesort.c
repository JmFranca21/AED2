#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Função de Bubble Sort com print a cada iteração
void bubbleSort(int arr[], int n) {
    int i, j, temp;
    for (i = 0; i < n-1; i++) {
        for (j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                // Troca os elementos
                temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;

                // Mostra o array inteiro com destaque para os elementos trocados
                printf("Iteração (%d, %d): ", i, j);
                for (int k = 0; k < n; k++) {
                    if (k == j || k == j+1) {
                        printf("[%d] ", arr[k]); // Destaca os elementos trocados
                    } else {
                        printf("%d ", arr[k]);
                    }
                }
                printf("\n");
            }
        }
    }
}

// Função para gerar números aleatórios no array
void gerarNumerosAleatorios(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand(); // Gera números entre 1 e 100
    }
}

int main() {
    int n;
    printf("Digite o número de elementos: ");
    scanf("%d", &n);

    // Aloca o array dinamicamente
    int *arr = (int*)malloc(n * sizeof(int));
    if (arr == NULL) {
        printf("Erro ao alocar memória.\n");
        return 1;
    }

    // Inicializa o gerador de números aleatórios com uma semente baseada no tempo atual
    srand(time(NULL));

    // Preenche o array com números aleatórios
    gerarNumerosAleatorios(arr, n);

    printf("Array gerado aleatoriamente:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    // Marca o tempo antes de iniciar o Bubble Sort
    clock_t inicio = clock();
    bubbleSort(arr, n);
    // Marca o tempo após a execução do Bubble Sort
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
