#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <vector>

#define debug(x, y) printf(x, y)
/* Este é um algoritmo de multiplicação de matrizes que usa threads produtor-consumidor para melhorar o desempenho. O algoritmo é dividido em três partes principais:

    Preparação: Alocação de memória para as matrizes a serem multiplicadas e inicialização dos vetores de threads produtoras/consumidoras.
    Criação de threads produtoras/consumidoras: Nesta fase, as threads são criadas para executar o cálculo da multiplicação de matrizes. As threads produtoras preenchem uma fila com linhas das matrizes a serem multiplicadas, e as threads consumidoras retiram essas linhas da fila e executam o cálculo da multiplicação. Cada thread consome uma linha de A e uma linha correspondente de C.
    Espera pelo término das threads: O programa espera que todas as threads terminem antes de continuar a execução.

A parte principal do algoritmo é a multiplicação de matrizes, que é realizada pelas threads consumidoras. Cada thread consome uma linha da matriz A e uma linha correspondente da matriz C. A multiplicação de matrizes é feita seguindo a fórmula clássica: C[i][j] = soma(A[i][k]*B[k][j]) para todos os valores possíveis de k.
 */

#define TAMANHOMATRIZ 3 // Numero de linhas das matrizes
#define NTHREADS 8  // Numero de threads consumidoras utilizadas

long unsigned int currentThreads;

using namespace std;
mutex mtx;
/*
 * */

struct Dados
{
    long unsigned int **B;// B é referencia pra matriz inteira linha x coluna
    std::vector<long unsigned int*> A, C;//Vetor de referencias para linhas da matriz A e C que essa thread consumirá
    long unsigned int threadId;

    void adicionarLinhas(long unsigned int *a, long unsigned int *c)//atualizar conteúdo da thread
    {
        A.push_back(a);
        C.push_back(c);
    }
    long unsigned int* getA()
    {
        long unsigned int* a;
        a = A.back();
        A.pop_back();
        return a;
    }
    long unsigned int* getC()
    {
        long unsigned int* c;
        c = C.back();
        C.pop_back();
        return c;
    }
    Dados(long unsigned int **b, long unsigned int t)
    {
        B = b;
        threadId = t;
    }
    Dados()// O compulador pediu então implementei
    {
        B = NULL;
        threadId = 0;
    }
};

void printaMat(long unsigned int **mat, long unsigned n, const char* titulo)
{
    printf("%s\n", titulo);
    for(int i = 0; i < n; i++)
    {
        for(int j = 0; j < n; j++)
            printf("%5ld ", mat[i][j]);
        printf("\n");
    }
}

void *multiplicaLinha(void *dados)
{
    Dados *ptr = (Dados *) dados;
    long unsigned int *a, *c;//ponteiros temporarios

    while(!(ptr->A.empty()) && !(ptr->C.empty()))//enquanto tiver algo na fila dessa thread
    {
        a = ptr->getA();//pega uma linha de A
        c = ptr->getC();//pega uma linha correspondente em C
        for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)//executa o calculo normalmente
        {
            for(int j = 0; j < TAMANHOMATRIZ; j++)
                c[i] += a[j] * ptr->B[i][j];
        }
    }
    printf("thread#%02lu: concluída\n", ptr->threadId);
    mtx.lock();
    currentThreads--;//decrementa 1 do número global de threads abertas, já que essa será fechada
    mtx.unlock();
    pthread_exit(NULL);
}

int main()
{
    try
    {
        pthread_t threads[NTHREADS];
        Dados data[NTHREADS];
        long unsigned int **a, **b, **c;

        // Alocação e inicialização das variáveis a serem utilizadas
        a = new long unsigned int*[TAMANHOMATRIZ];
        b = new long unsigned int*[TAMANHOMATRIZ];
        c = new long unsigned int*[TAMANHOMATRIZ];

        //insere o vetor b no contexto de todas as threads
        for(long unsigned int i = 0; i < NTHREADS; i++)
            data[i] = Dados(b, i+1);

        // Preenchimento das matrizes a serem multiplicadas
        for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
        {
            a[i] = new long unsigned int[TAMANHOMATRIZ];
            b[i] = new long unsigned int[TAMANHOMATRIZ];
            c[i] = new long unsigned int[TAMANHOMATRIZ]{0};//iniciados preenchidos com zeros

            for(int j = 0; j < TAMANHOMATRIZ; j++)
                    a[i][j] += i+j;

            //distribui as linhas entre as threads utilizando o resto da divisão da linha atual pela quantidade de threads
            data[i % NTHREADS].adicionarLinhas(a[i], c[i]);
        }
        for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)//calculando a transposta de B
            for(int j = 0; j < TAMANHOMATRIZ; j++)
                    b[j][i] += i+j;
        //
        // Criando as threads
        for(long unsigned int i = 0; i < NTHREADS; i++)
        {
            if(pthread_create(&threads[i], NULL, multiplicaLinha, (void *)&data[i]) != 0)
                throw("Erro ao criar thread i");
            mtx.lock();
            currentThreads++;// Incrementa o contador global de threads abertas
            mtx.unlock();
        }

        // Imprimindo o andamento
        printf("esperando encerramento das threads abertas\n");
        while(currentThreads > 0)
        {
            usleep(0.1);//só liberará o fluxo do código uma vez que todas as threads tenham sido executadas até o fim
        }
        printf("threads concluidas\n");

        // Imprimindo o resultado
        printaMat(c, TAMANHOMATRIZ, "\n\tResultado:");
        // desalocando memória
        for(int i = 0; i < TAMANHOMATRIZ; i++)
        {
            free(a[i]);
            free(b[i]);
            free(c[i]);
        }
        free(a);
        free(b);
        free(c);
        return 0;
    }catch(const exception &e)
    {
        cerr<<e.what()<<endl;
    }
    return 0;
}
