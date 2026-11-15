/*
 * AED2 - Atividade Pratica - Aula 5
 * Joao Marcelo Franca Batista
 *
 * Questao 1: executar dois algoritmos de ORDENACAO EXTERNA vistos em aula,
 * explicar como foram implementados, mostrar o tempo de execucao e dizer qual
 * foi mais rapido. Algoritmos escolhidos:
 *   (A) INTERCALACAO BALANCEADA DE VARIOS CAMINHOS (2F arquivos);
 *   (B) QUICKSORT EXTERNO (com area de pivos em memoria).
 *
 * Cenario: um arquivo binario com N registros (chave inteira + dado) em ordem
 * aleatoria, que precisa ser ordenado com NO MAXIMO M registros na memoria
 * principal (M e bem menor que N: o arquivo nao cabe na memoria).
 *
 * (A) Intercalacao balanceada de F caminhos (Ziviani, cap. 4):
 *   1) Formacao de blocos ordenados: le M registros por vez, ordena na memoria
 *      (qsort) e grava o bloco ("run") em um dos F arquivos de entrada, em
 *      rodizio.
 *   2) Intercalacao: a cada passagem, um bloco de cada um dos F arquivos de
 *      entrada e intercalado (com um heap minimo de F posicoes, mantendo so um
 *      registro de cada bloco na memoria) em um unico bloco maior, gravado em
 *      um dos F arquivos de saida, em rodizio. Terminada a passagem, entrada e
 *      saida trocam de papel. Repete ate restar um unico bloco.
 *   Cada passagem le e grava os N registros uma vez; o numero de passagens e
 *   ceil(log_F(numero de blocos)).
 *
 * (B) Quicksort externo (Ziviani, cap. 4): ordena o arquivo "no lugar", usando
 *   tres cabecas de arquivo: uma de leitura no inicio (ArqLi), uma de escrita
 *   no inicio (ArqEi) e uma de leitura/escrita no fim (ArqLEs). Mantem na
 *   memoria uma AREA ordenada de ate M registros. Le um registro alternando
 *   as pontas; se a chave e maior que o maior pivo ja descartado (Lsup), vai
 *   direto para a particao superior; se e menor que Linf, para a inferior; se
 *   cabe entre os dois, entra na area, e o maior ou o menor registro da area
 *   (o da particao menor, para equilibrar) e gravado e vira o novo limite.
 *   Ao fim, a area e gravada ordenada no meio. Depois ordena recursivamente a
 *   menor particao primeiro. Como as tres cabecas apontam para o mesmo arquivo,
 *   os arquivos sao abertos SEM buffer do C (setvbuf com _IONBF); sem isso,
 *   um buffer de leitura poderia guardar dados antigos de posicoes ja
 *   regravadas por outra cabeca.
 *
 * Escolhas da atividade:
 *  - N = 200.000 registros de 8 bytes (1,6 MB), M = 2.000 registros na memoria
 *    (1% do arquivo), F = 10 caminhos, seed 42, mesmas entradas nos dois.
 *  - Tempo = MEDIA de EXECUCOES execucoes (o arquivo e recriado a cada uma).
 *    Tempo total de relogio (CPU + disco), medido com clock_gettime().
 *  - Corretude: o arquivo de saida e lido e conferido (ordem nao decrescente,
 *    N registros e mesma soma das chaves e dos dados da entrada).
 *  - Sao contados os registros lidos e gravados de/para o disco (operacoes de
 *    E/S logicas), que e o que mais pesa em ordenacao externa.
 *
 * Fontes consultadas (algoritmos classicos, implementados a partir deles):
 *  - N. Ziviani, Projeto de Algoritmos com Implementacoes em Pascal e C,
 *    cap. 4 (Ordenacao: ordenacao externa, intercalacao balanceada de varios
 *    caminhos e quicksort externo).
 *  - D. Knuth, The Art of Computer Programming, vol. 3, secao 5.4 (ordenacao
 *    externa, intercalacao balanceada).
 *  - Cormen et al., Introduction to Algorithms (formacao de blocos ordenados e
 *    intercalacao de k vias com heap minimo).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#define N 200000            /* registros no arquivo */
#define M 2000              /* maximo de registros na memoria principal */
#define F 10                /* caminhos da intercalacao (F entradas + F saidas) */
#define EXECUCOES 3
#define ARQ_ENTRADA "aula5_entrada.bin"
#define ARQ_QUICK "aula5_quick.bin"
#define ARQ_FINAL "aula5_saida.bin"
#define MAX_BLOCOS (N / M + F + 2)

typedef struct {
    int chave;
    int dado;               /* informacao associada (acompanha a chave) */
} Registro;

long lidos = 0, gravados = 0;   /* contadores de E/S logica */

int compararReg(const void *a, const void *b) {
    int x = ((const Registro *)a)->chave, y = ((const Registro *)b)->chave;
    return (x > y) - (x < y);
}

double diffSegundos(struct timespec inicio, struct timespec fim) {
    return (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

int sorteio(int limite) {
    return (((rand() & 0x7FFF) << 15) | (rand() & 0x7FFF)) % limite;
}

/* cria o arquivo de entrada com N registros aleatorios (seed fixa) */
void criarEntrada(const char *nome) {
    FILE *f = fopen(nome, "wb");
    srand(42);
    for (int i = 0; i < N; i++) {
        Registro r = { sorteio(10 * N), i };
        fwrite(&r, sizeof(Registro), 1, f);
    }
    fclose(f);
}

void copiarArquivo(const char *origem, const char *destino) {
    FILE *a = fopen(origem, "rb"), *b = fopen(destino, "wb");
    static Registro buf[4096];
    size_t n;
    while ((n = fread(buf, sizeof(Registro), 4096, a)) > 0) fwrite(buf, sizeof(Registro), n, b);
    fclose(a);
    fclose(b);
}

/* confere o arquivo ordenado: ordem, quantidade e somas de chaves/dados */
int conferir(const char *nome, long long somaChaves, long long somaDados) {
    FILE *f = fopen(nome, "rb");
    Registro r;
    int ultima = INT_MIN;
    long cont = 0;
    long long sc = 0, sd = 0;
    int ok = 1;
    while (fread(&r, sizeof(Registro), 1, f) == 1) {
        if (r.chave < ultima) ok = 0;
        ultima = r.chave;
        sc += r.chave;
        sd += r.dado;
        cont++;
    }
    fclose(f);
    return ok && cont == N && sc == somaChaves && sd == somaDados;
}

/* =====================================================================
 * (A) Intercalacao balanceada de F caminhos
 * ===================================================================== */
typedef struct {
    FILE *arq;
    int numBlocos;
    int tamBloco[MAX_BLOCOS];   /* quantos registros tem cada bloco do arquivo */
} Fita;

static Registro memoria[M];     /* a memoria principal permitida */

int semBuffer = 0;              /* 1 = abre as fitas sem buffer do C (mesmo regime de E/S do quicksort externo) */

FILE *abrir(const char *nome, const char *modo) {
    FILE *f = fopen(nome, modo);
    if (semBuffer) setvbuf(f, NULL, _IONBF, 0);
    return f;
}

void nomeFita(char *dest, int conjunto, int i) {
    sprintf(dest, "aula5_fita_%c%d.bin", 'a' + conjunto, i);
}

/* heap minimo de ate F registros (chave) com o indice da fita de origem */
typedef struct { Registro reg; int fita; } ItemHeap;

void descer(ItemHeap h[], int n, int i) {
    while (1) {
        int menor = i, e = 2 * i + 1, d = 2 * i + 2;
        if (e < n && h[e].reg.chave < h[menor].reg.chave) menor = e;
        if (d < n && h[d].reg.chave < h[menor].reg.chave) menor = d;
        if (menor == i) return;
        ItemHeap t = h[i]; h[i] = h[menor]; h[menor] = t;
        i = menor;
    }
}

void subir(ItemHeap h[], int i) {
    while (i > 0 && h[(i - 1) / 2].reg.chave > h[i].reg.chave) {
        ItemHeap t = h[i]; h[i] = h[(i - 1) / 2]; h[(i - 1) / 2] = t;
        i = (i - 1) / 2;
    }
}

/* ordena ARQ_ENTRADA em ARQ_FINAL; devolve o numero de passagens de intercalacao */
int intercalacaoBalanceada(int *numBlocosIniciais) {
    Fita fitas[2][F];
    char nome[64];
    int entrada = 0;                    /* conjunto atualmente usado como entrada */

    /* 1) formacao de blocos ordenados, distribuidos em rodizio no conjunto 0 */
    for (int i = 0; i < F; i++) {
        nomeFita(nome, 0, i);
        fitas[0][i].arq = abrir(nome, "wb");
        fitas[0][i].numBlocos = 0;
    }
    FILE *ent = abrir(ARQ_ENTRADA, "rb");
    int blocos = 0;
    size_t n;
    while ((n = fread(memoria, sizeof(Registro), M, ent)) > 0) {
        lidos += (long)n;
        qsort(memoria, n, sizeof(Registro), compararReg);
        Fita *f = &fitas[0][blocos % F];
        fwrite(memoria, sizeof(Registro), n, f->arq);
        gravados += (long)n;
        f->tamBloco[f->numBlocos++] = (int)n;
        blocos++;
    }
    fclose(ent);
    for (int i = 0; i < F; i++) fclose(fitas[0][i].arq);
    *numBlocosIniciais = blocos;

    /* 2) passagens de intercalacao: F blocos de entrada -> 1 bloco de saida */
    int passagens = 0;
    while (blocos > 1) {
        int saida = 1 - entrada;
        for (int i = 0; i < F; i++) {
            nomeFita(nome, entrada, i);
            fitas[entrada][i].arq = abrir(nome, "rb");
            nomeFita(nome, saida, i);
            fitas[saida][i].arq = abrir(nome, "wb");
            fitas[saida][i].numBlocos = 0;
        }
        int proximo[F] = {0};           /* proximo bloco a intercalar de cada fita */
        int restantes = blocos, destino = 0, novosBlocos = 0;

        while (restantes > 0) {
            ItemHeap heap[F];
            int nh = 0, falta[F];
            for (int i = 0; i < F; i++) {   /* um bloco de cada fita que ainda tem */
                if (proximo[i] < fitas[entrada][i].numBlocos) {
                    falta[i] = fitas[entrada][i].tamBloco[proximo[i]];
                    fread(&heap[nh].reg, sizeof(Registro), 1, fitas[entrada][i].arq);
                    lidos++;
                    heap[nh].fita = i;
                    nh++;
                    restantes--;
                }
            }
            for (int i = nh / 2 - 1; i >= 0; i--) descer(heap, nh, i);   /* monta o heap */

            Fita *s = &fitas[saida][destino];
            int total = 0;
            while (nh > 0) {
                fwrite(&heap[0].reg, sizeof(Registro), 1, s->arq);      /* menor chave */
                gravados++;
                total++;
                int i = heap[0].fita;
                if (--falta[i] > 0) {                                    /* repoe da mesma fita */
                    fread(&heap[0].reg, sizeof(Registro), 1, fitas[entrada][i].arq);
                    lidos++;
                    descer(heap, nh, 0);
                } else {                                                 /* bloco acabou */
                    proximo[i]++;
                    heap[0] = heap[--nh];
                    descer(heap, nh, 0);
                }
            }
            s->tamBloco[s->numBlocos++] = total;
            destino = (destino + 1) % F;
            novosBlocos++;
        }
        for (int i = 0; i < F; i++) { fclose(fitas[entrada][i].arq); fclose(fitas[saida][i].arq); }
        blocos = novosBlocos;
        entrada = saida;
        passagens++;
    }

    /* o unico bloco restante esta em uma fita do conjunto "entrada": vira o resultado */
    for (int i = 0; i < F; i++) {
        nomeFita(nome, entrada, i);
        if (fitas[entrada][i].numBlocos == 1) {
            remove(ARQ_FINAL);
            rename(nome, ARQ_FINAL);
        }
    }
    for (int c = 0; c < 2; c++) {               /* apaga as fitas temporarias */
        for (int i = 0; i < F; i++) {
            nomeFita(nome, c, i);
            remove(nome);
        }
    }
    return passagens;
}

/* =====================================================================
 * (B) Quicksort externo (area de pivos em memoria)
 * ===================================================================== */
static Registro area[M];                /* area de pivos: ordenada, ate M registros */

/* as tres "cabecas" de arquivo (sem buffer do C: ver comentario no cabecalho) */
FILE *ArqLi, *ArqEi, *ArqLEs;

void leSup(FILE *arq, Registro *r, int *Ls, int *ondeLer) {
    fseek(arq, (long)(*Ls) * (long)sizeof(Registro), SEEK_SET);
    fread(r, sizeof(Registro), 1, arq);
    lidos++;
    (*Ls)--;
    *ondeLer = 0;
}

void leInf(FILE *arq, Registro *r, int *Li, int *ondeLer) {
    fseek(arq, (long)(*Li) * (long)sizeof(Registro), SEEK_SET);
    fread(r, sizeof(Registro), 1, arq);
    lidos++;
    (*Li)++;
    *ondeLer = 1;
}

void escreveMax(FILE *arq, Registro r, int *Es) {
    fseek(arq, (long)(*Es) * (long)sizeof(Registro), SEEK_SET);
    fwrite(&r, sizeof(Registro), 1, arq);
    gravados++;
    (*Es)--;
}

void escreveMin(FILE *arq, Registro r, int *Ei) {
    fwrite(&r, sizeof(Registro), 1, arq);   /* ArqEi ja esta posicionada em Ei */
    gravados++;
    (*Ei)++;
}

/* insere na area mantendo-a ordenada por chave (insercao com deslocamento) */
void inserirArea(Registro r, int *nrArea) {
    int i = *nrArea - 1;
    while (i >= 0 && area[i].chave > r.chave) {
        area[i + 1] = area[i];
        i--;
    }
    area[i + 1] = r;
    (*nrArea)++;
}

Registro retiraPrimeiro(int *nrArea) {
    Registro r = area[0];
    memmove(&area[0], &area[1], (size_t)(*nrArea - 1) * sizeof(Registro));
    (*nrArea)--;
    return r;
}

Registro retiraUltimo(int *nrArea) {
    return area[--(*nrArea)];
}

/* particiona o trecho [Esq, Dir] do arquivo; ao final, [Esq..*i] <= area <= [*j..Dir] */
void particao(int Esq, int Dir, int *i, int *j) {
    int Ls = Dir, Es = Dir, Li = Esq, Ei = Esq;
    int Linf = INT_MIN, Lsup = INT_MAX;
    int nrArea = 0, ondeLer = 1;
    Registro ultLido, R;

    fseek(ArqLi, (long)Li * (long)sizeof(Registro), SEEK_SET);
    fseek(ArqEi, (long)Ei * (long)sizeof(Registro), SEEK_SET);
    *i = Esq - 1;
    *j = Dir + 1;

    while (Ls >= Li) {
        if (nrArea < M - 1) {                       /* primeiro enche a area */
            if (ondeLer) leSup(ArqLEs, &ultLido, &Ls, &ondeLer);
            else leInf(ArqLi, &ultLido, &Li, &ondeLer);
            inserirArea(ultLido, &nrArea);
            continue;
        }
        if (Ls == Es) leSup(ArqLEs, &ultLido, &Ls, &ondeLer);     /* nao sobrescrever o nao lido */
        else if (Li == Ei) leInf(ArqLi, &ultLido, &Li, &ondeLer);
        else if (ondeLer) leSup(ArqLEs, &ultLido, &Ls, &ondeLer);
        else leInf(ArqLi, &ultLido, &Li, &ondeLer);

        if (ultLido.chave > Lsup) {                 /* maior que todos os pivos: particao superior */
            *j = Es;
            escreveMax(ArqLEs, ultLido, &Es);
            continue;
        }
        if (ultLido.chave < Linf) {                 /* menor que todos os pivos: particao inferior */
            *i = Ei;
            escreveMin(ArqEi, ultLido, &Ei);
            continue;
        }
        inserirArea(ultLido, &nrArea);              /* fica entre Linf e Lsup: entra na area */
        if (Ei - Esq < Dir - Es) {                  /* particao inferior esta menor: ela cresce */
            R = retiraPrimeiro(&nrArea);            /* grava o MENOR da area na particao inferior */
            escreveMin(ArqEi, R, &Ei);
            Linf = R.chave;
        } else {                                    /* senao, a superior cresce */
            R = retiraUltimo(&nrArea);              /* grava o MAIOR da area na particao superior */
            escreveMax(ArqLEs, R, &Es);
            Lsup = R.chave;
        }
    }
    while (Ei <= Es) {                              /* despeja a area ordenada no meio */
        R = retiraPrimeiro(&nrArea);
        escreveMin(ArqEi, R, &Ei);
    }
}

void quicksortExterno(int Esq, int Dir) {
    if (Dir - Esq < 1) return;
    int i, j;
    particao(Esq, Dir, &i, &j);
    if (i - Esq < Dir - j) {                        /* ordena a menor particao primeiro */
        quicksortExterno(Esq, i);
        quicksortExterno(j, Dir);
    } else {
        quicksortExterno(j, Dir);
        quicksortExterno(Esq, i);
    }
}

/* ordena ARQ_QUICK no lugar */
void quickExterno(void) {
    ArqLi = fopen(ARQ_QUICK, "r+b");
    ArqEi = fopen(ARQ_QUICK, "r+b");
    ArqLEs = fopen(ARQ_QUICK, "r+b");
    setvbuf(ArqLi, NULL, _IONBF, 0);
    setvbuf(ArqEi, NULL, _IONBF, 0);
    setvbuf(ArqLEs, NULL, _IONBF, 0);
    quicksortExterno(0, N - 1);
    fclose(ArqLi);
    fclose(ArqEi);
    fclose(ArqLEs);
}

int main(void) {
    struct timespec t0, t1;
    long long somaChaves = 0, somaDados = 0;
    double somaA = 0, somaB = 0, somaA2 = 0;
    long lidosA = 0, gravadosA = 0, lidosB = 0, gravadosB = 0;
    int passagens = 0, blocosIniciais = 0;
    int okA = 1, okB = 1, okA2 = 1;

    printf("Ordenacao externa: %d registros de %d bytes (%.1f MB), memoria de %d registros (%.1f%% do arquivo)\n",
           N, (int)sizeof(Registro), N * sizeof(Registro) / 1e6, M, 100.0 * M / N);
    printf("Media de %d execucoes; tempo total de relogio (CPU + disco); seed fixa = 42\n\n", EXECUCOES);

    criarEntrada(ARQ_ENTRADA);
    {   /* somas de referencia da entrada, para conferir que nenhum registro se perdeu */
        FILE *f = fopen(ARQ_ENTRADA, "rb");
        Registro r;
        while (fread(&r, sizeof(Registro), 1, f) == 1) { somaChaves += r.chave; somaDados += r.dado; }
        fclose(f);
    }

    for (int e = 0; e < EXECUCOES; e++) {
        /* (A) intercalacao balanceada, com buffer do C (uso normal) */
        semBuffer = 0;
        lidos = gravados = 0;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        passagens = intercalacaoBalanceada(&blocosIniciais);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        somaA += diffSegundos(t0, t1);
        lidosA = lidos;
        gravadosA = gravados;
        okA = okA && conferir(ARQ_FINAL, somaChaves, somaDados);

        /* (A') a mesma intercalacao SEM buffer: mesmo regime de E/S do quicksort externo */
        semBuffer = 1;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        intercalacaoBalanceada(&blocosIniciais);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        somaA2 += diffSegundos(t0, t1);
        okA2 = okA2 && conferir(ARQ_FINAL, somaChaves, somaDados);
        semBuffer = 0;

        /* (B) quicksort externo (a copia do arquivo nao entra no tempo) */
        copiarArquivo(ARQ_ENTRADA, ARQ_QUICK);
        lidos = gravados = 0;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        quickExterno();
        clock_gettime(CLOCK_MONOTONIC, &t1);
        somaB += diffSegundos(t0, t1);
        lidosB = lidos;
        gravadosB = gravados;
        okB = okB && conferir(ARQ_QUICK, somaChaves, somaDados);
    }

    double tA = somaA / EXECUCOES, tB = somaB / EXECUCOES, tA2 = somaA2 / EXECUCOES;
    printf("(A) Intercalacao balanceada de %d caminhos (%d arquivos)\n", F, 2 * F);
    printf("    blocos iniciais: %d de ate %d registros | passagens de intercalacao: %d\n", blocosIniciais, M, passagens);
    printf("    registros lidos: %ld | gravados: %ld\n", lidosA, gravadosA);
    printf("    tempo (media de %d execucoes): %.6f s | resultado ordenado e completo: %s\n\n", EXECUCOES, tA, okA ? "SIM" : "NAO");

    printf("(B) Quicksort externo (area de %d registros)\n", M);
    printf("    registros lidos: %ld | gravados: %ld\n", lidosB, gravadosB);
    printf("    tempo (media de %d execucoes): %.6f s | resultado ordenado e completo: %s\n\n", EXECUCOES, tB, okB ? "SIM" : "NAO");

    printf("(A') Mesma intercalacao balanceada, mas SEM buffer do C (mesmo regime de E/S do quicksort externo)\n");
    printf("    tempo (media de %d execucoes): %.6f s | resultado ordenado e completo: %s\n\n", EXECUCOES, tA2, okA2 ? "SIM" : "NAO");

    printf("Mais rapido (uso normal, A x B): %s (%.1fx)\n", tA < tB ? "INTERCALACAO BALANCEADA" : "QUICKSORT EXTERNO",
           tA < tB ? tB / tA : tA / tB);
    printf("Mais rapido (mesmo regime de E/S sem buffer, A' x B): %s (%.1fx)\n", tA2 < tB ? "INTERCALACAO BALANCEADA" : "QUICKSORT EXTERNO",
           tA2 < tB ? tB / tA2 : tA2 / tB);

    remove(ARQ_ENTRADA);
    remove(ARQ_QUICK);
    remove(ARQ_FINAL);
    return 0;
}
