/*
 * AED2 - Atividade Pratica - Aula 11
 * Joao Marcelo Franca Batista
 *
 * Questao 1: implementacao de tabela hash com m = 10000 posicoes e funcao
 * hash da divisao, h(k) = k mod m, com duas formas de resolver colisoes:
 *   a) ENCADEAMENTO (lista ligada em cada posicao da tabela);
 *   b) ENDERECAMENTO ABERTO com SONDAGEM LINEAR,
 *      h(k, i) = (k mod m + i) mod m.
 * Sao inseridos 10000 elementos aleatorios no intervalo [0, 10000] (mesmos
 * elementos nas duas tabelas) e medido o tempo de insercao de cada tabela.
 * Depois e pesquisado o elemento 999 em cada tabela. Medicao de tempo com
 * clock_gettime() (time.h).
 *
 * Observacoes:
 *  - Como sao 10000 elementos para 10000 posicoes, chaves repetidas sao
 *    inseridas normalmente (a tabela de enderecamento aberto fica 100% cheia).
 *  - O tempo de insercao e a MEDIA de 10 execucoes (a tabela e refeita do zero
 *    a cada execucao).
 *  - Uma unica busca dura poucos nanossegundos, menos do que a precisao do
 *    relogio. Por isso a busca do 999 e repetida 10000 vezes e o tempo
 *    mostrado e o de UMA busca (tempo total / 10000).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define M 10000             /* numero de posicoes das tabelas */
#define N 10000             /* quantidade de elementos inseridos */
#define VALOR_MAX 10001     /* valores sorteados de 0 a 10000 */
#define CHAVE_BUSCA 999     /* elemento pesquisado */
#define EXECUCOES 10        /* execucoes para a media do tempo de insercao */
#define REPETICOES_BUSCA 10000
#define VAZIO -1            /* marca de posicao livre (enderecamento aberto) */

/* ---------- Hash com ENCADEAMENTO (funcao hash da divisao) ---------- */
typedef struct No {
    int chave;
    struct No *prox;
} No;

No *tabelaEnc[M];

int hashDivisao(int chave) {
    return chave % M;
}

/* insere no INICIO da lista da posicao (custo O(1)) */
void inserirEncadeamento(int chave) {
    int pos = hashDivisao(chave);
    No *novo = malloc(sizeof(No));
    novo->chave = chave;
    novo->prox = tabelaEnc[pos];
    tabelaEnc[pos] = novo;
}

int buscarEncadeamento(int chave) {
    for (No *p = tabelaEnc[hashDivisao(chave)]; p != NULL; p = p->prox) {
        if (p->chave == chave) return 1;
    }
    return 0;
}

void limparEncadeamento(void) {
    for (int i = 0; i < M; i++) {
        No *p = tabelaEnc[i];
        while (p != NULL) {
            No *prox = p->prox;
            free(p);
            p = prox;
        }
        tabelaEnc[i] = NULL;
    }
}

/* ---------- Hash com ENDERECAMENTO ABERTO (sondagem linear) ---------- */
int tabelaAb[M];

void limparEnderecamento(void) {
    for (int i = 0; i < M; i++) tabelaAb[i] = VAZIO;
}

/* devolve quantas posicoes foram testadas, ou -1 se a tabela estiver cheia */
int inserirEnderecamento(int chave) {
    for (int i = 0; i < M; i++) {
        int pos = (hashDivisao(chave) + i) % M;
        if (tabelaAb[pos] == VAZIO) {
            tabelaAb[pos] = chave;
            return i + 1;
        }
    }
    return -1;
}

/* para ao achar a chave, ao achar posicao livre ou depois de testar M posicoes
 * (a tabela cheia nao tem posicao livre, entao sem esse limite o laco nao para) */
int buscarEnderecamento(int chave) {
    for (int i = 0; i < M; i++) {
        int pos = (hashDivisao(chave) + i) % M;
        if (tabelaAb[pos] == chave) return 1;
        if (tabelaAb[pos] == VAZIO) return 0;
    }
    return 0;
}

/* ---------- Funcoes auxiliares (nao fazem parte da comparacao) ---------- */
/* rand() pode ter RAND_MAX = 32767 (ex.: MinGW/Windows); juntar duas chamadas
 * da 30 bits e garante que todos os valores de 0 a 10000 possam sair. */
int valorAleatorio(void) {
    return (((rand() & 0x7FFF) << 15) | (rand() & 0x7FFF)) % VALOR_MAX;
}

double diffSegundos(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

int elementos[N];

int main(void) {
    struct timespec inicio, fim;
    double somaInsEnc = 0.0, somaInsAb = 0.0;
    long colisoesAb = 0; /* posicoes testadas alem da primeira (ultima execucao) */

    srand(42); /* seed fixa: garante os MESMOS elementos em toda execucao */
    for (int i = 0; i < N; i++) elementos[i] = valorAleatorio();

    int quantos999 = 0;
    for (int i = 0; i < N; i++) {
        if (elementos[i] == CHAVE_BUSCA) quantos999++;
    }

    /* ---------- Insercao (media de EXECUCOES execucoes) ---------- */
    for (int e = 0; e < EXECUCOES; e++) {
        limparEncadeamento();
        clock_gettime(CLOCK_MONOTONIC, &inicio);
        for (int i = 0; i < N; i++) inserirEncadeamento(elementos[i]);
        clock_gettime(CLOCK_MONOTONIC, &fim);
        somaInsEnc += diffSegundos(inicio, fim);

        limparEnderecamento();
        colisoesAb = 0;
        clock_gettime(CLOCK_MONOTONIC, &inicio);
        for (int i = 0; i < N; i++) colisoesAb += inserirEnderecamento(elementos[i]) - 1;
        clock_gettime(CLOCK_MONOTONIC, &fim);
        somaInsAb += diffSegundos(inicio, fim);
    }
    double tempoInsEnc = somaInsEnc / EXECUCOES;
    double tempoInsAb = somaInsAb / EXECUCOES;

    /* ---------- Verificacao: todas as chaves inseridas sao encontradas ---------- */
    int tudoCerto = 1;
    for (int i = 0; i < N; i++) {
        if (!buscarEncadeamento(elementos[i]) || !buscarEnderecamento(elementos[i])) tudoCerto = 0;
    }

    /* ---------- Estatisticas das tabelas (so para analise) ---------- */
    int maiorLista = 0, posicoesVazias = 0;
    for (int i = 0; i < M; i++) {
        int tamanho = 0;
        for (No *p = tabelaEnc[i]; p != NULL; p = p->prox) tamanho++;
        if (tamanho > maiorLista) maiorLista = tamanho;
        if (tamanho == 0) posicoesVazias++;
    }

    /* ---------- Busca do elemento 999 (REPETICOES_BUSCA buscas) ---------- */
    volatile int achouEnc = 0, achouAb = 0; /* volatile: o resultado da busca e usado */

    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (int r = 0; r < REPETICOES_BUSCA; r++) achouEnc = buscarEncadeamento(CHAVE_BUSCA);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempoBuscaEnc = diffSegundos(inicio, fim) / REPETICOES_BUSCA;

    clock_gettime(CLOCK_MONOTONIC, &inicio);
    for (int r = 0; r < REPETICOES_BUSCA; r++) achouAb = buscarEnderecamento(CHAVE_BUSCA);
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempoBuscaAb = diffSegundos(inicio, fim) / REPETICOES_BUSCA;

    printf("Tabelas hash com m = %d posicoes, %d elementos aleatorios em [0, %d] (seed fixa = 42)\n",
           M, N, VALOR_MAX - 1);
    printf("O elemento %d aparece %d vez(es) entre os elementos inseridos\n", CHAVE_BUSCA, quantos999);
    printf("Verificacao (todas as chaves inseridas foram encontradas nas duas tabelas)? %s\n\n",
           tudoCerto ? "SIM" : "NAO");

    printf("== Hash por ENCADEAMENTO (funcao hash da divisao) ==\n");
    printf("Tempo de insercao (media de %d execucoes): %.6f s\n", EXECUCOES, tempoInsEnc);
    printf("Tempo de busca do %d (1 busca): %.1f ns   -> elemento %s\n", CHAVE_BUSCA,
           tempoBuscaEnc * 1e9, achouEnc ? "ENCONTRADO" : "NAO encontrado");
    printf("Maior lista: %d elementos | posicoes vazias: %d de %d\n\n", maiorLista, posicoesVazias, M);

    printf("== Hash por ENDERECAMENTO ABERTO (sondagem linear) ==\n");
    printf("Tempo de insercao (media de %d execucoes): %.6f s\n", EXECUCOES, tempoInsAb);
    printf("Tempo de busca do %d (1 busca): %.1f ns   -> elemento %s\n", CHAVE_BUSCA,
           tempoBuscaAb * 1e9, achouAb ? "ENCONTRADO" : "NAO encontrado");
    printf("Posicoes extras testadas na insercao (colisoes): %ld\n\n", colisoesAb);

    if (tempoInsEnc < tempoInsAb)
        printf("Insercao: o ENCADEAMENTO foi mais rapido.\n");
    else
        printf("Insercao: o ENDERECAMENTO ABERTO foi mais rapido.\n");
    if (tempoBuscaEnc < tempoBuscaAb)
        printf("Busca: o ENCADEAMENTO foi mais rapido.\n");
    else
        printf("Busca: o ENDERECAMENTO ABERTO foi mais rapido.\n");

    limparEncadeamento();
    return 0;
}

/*
 * Nota de utilizacao de IA: uma LLM (Claude) foi utilizada para auxiliar na
 * formatacao correta e na inclusao de comentarios ao longo do codigo.
 * Alem das partes de impressao na tela (printf's) para melhor visualizacao
 * no momento de execucao.
 */
