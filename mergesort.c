#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Função para mesclar dois subarrays
void merge(int arr[], int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Arrays temporários
    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    // Copia os dados para os arrays temporários
    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    // Mescla os arrays temporários de volta no array principal
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copia os elementos restantes de L[], se existirem
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copia os elementos restantes de R[], se existirem
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    // Mostra o array inteiro após a mesclagem
    printf("Mesclando: ");
    for (int x = left; x <= right; x++) {
        printf("%d ", arr[x]);
    }
    printf("\n");

    // Libera a memória alocada para os arrays temporários
    free(L);
    free(R);
}

// Função recursiva para dividir e ordenar o array
void mergeSort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        // Divide e ordena a metade esquerda
        mergeSort(arr, left, mid);

        // Divide e ordena a metade direita
        mergeSort(arr, mid + 1, right);

        // Mescla as duas metades
        merge(arr, left, mid, right);
    }
}

// Função para gerar números aleatórios no array
void gerarNumerosAleatorios(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 100; // Gera números entre 0 e 99
    }
}

int main() {
    int n;
    printf("Digite o número de elementos: ");
    scanf("%d", &n);

    // Aloca o array dinamicamente
    int *arr = (int *)malloc(n * sizeof(int));
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

    // Marca o tempo antes de iniciar o Merge Sort
    clock_t inicio = clock();
    mergeSort(arr, 0, n - 1);
    // Marca o tempo após a execução do Merge Sort
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
