#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <vector>

#define debug(x, y) printf(x, y)
/* este é um algoritmo de multiplicação de matrizes que usa threads produtor-consumidor para melhorar o desempenho. o algoritmo é dividido em três partes principais:

    preparação: alocação de memória para as matrizes a serem multiplicadas e inicialização dos vetores de threads produtoras/consumidoras.
    criação de threads produtoras/consumidoras: nesta fase, as threads são criadas para executar o cálculo da multiplicação de matrizes. as threads produtoras preenchem uma fila com linhas das matrizes a serem multiplicadas, e as threads consumidoras retiram essas linhas da fila e executam o cálculo da multiplicação. cada thread consome uma linha de a e uma linha correspondente de c.
    espera pelo término das threads: o programa espera que todas as threads terminem antes de continuar a execução.

a parte principal do algoritmo é a multiplicação de matrizes, que é realizada pelas threads consumidoras. cada thread consome uma linha da matriz a e uma linha correspondente da matriz c. a multiplicação de matrizes é feita seguindo a fórmula clássica: c[i][j] = soma(a[i][k]*b[k][j]) para todos os valores possíveis de k.
 */


long unsigned int currentThreads;

using namespace std;
mutex mtx;

int nThreads = 8;  // numero de threads consumidoras utilizadas
long unsigned int TAMANHOMATRIZ = 3; // numero de linhas das matrizes
/*
 * */

struct Dados
{
    long unsigned int **b;// b é referencia pra matriz inteira linha x coluna
    vector<long unsigned int*> a, c;//vetor de referencias para linhas da matriz a e c que essa thread consumirá
    long unsigned int threadId;

    void adicionarlinhas(long unsigned int *A, long unsigned int *B)//atualizar conteúdo da thread
    {
        a.push_back(A);
        c.push_back(B);
    }
    long unsigned int* geta()
    {
        long unsigned int* A;
        A = a.back();
        a.pop_back();
        return A;
    }
    long unsigned int* getc()
    {
        long unsigned int* C;
        C = c.back();
        c.pop_back();
        return C;
    }
    Dados(long unsigned int **B, long unsigned int t)
    {
        b = B;
        threadId = t;
    }
    Dados()// o compulador pediu então implementei
    {
        b = NULL;
        threadId = 0;
    }
};

void printaMat(long unsigned int **mat, long unsigned n, const char* titulo)
{
    printf("%s\n", titulo);
    for(long unsigned int i = 0; i < n; i++)
    {
        for(long unsigned int j = 0; j < n; j++)
            printf("%5ld ", mat[i][j]);
        printf("\n");
    }
}

void *multiplicalinha(void *dados)
{
    Dados *ptr = (Dados *) dados;
    long unsigned int *a, *c;//ponteiros temporarios

    while(!(ptr->a.empty()) && !(ptr->c.empty()))//enquanto tiver algo na fila dessa thread
    {
        a = ptr->geta();//pega uma linha de a
        c = ptr->getc();//pega uma linha correspondente em c
        for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)//executa o calculo normalmente
        {
            for(long unsigned int j = 0; j < TAMANHOMATRIZ; j++)
                c[i] += a[j] * ptr->b[i][j];
        }
    }
    printf("thread#%02lu: concluída\n", ptr->threadId);
    mtx.lock();
    currentThreads--;//decrementa 1 do número global de threads abertas, já que essa será fechada
    mtx.unlock();
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    try
    {
        if(argc > 1)//caso receba valor específico para tamanho de matriz
            TAMANHOMATRIZ = strtoul(argv[1], NULL, 10);
        if(argc > 2)//caso receba valor específico para quantidade de threads
            nThreads = atoi(argv[2]);
        printf("Execução para %lu linhas, utilizando %d threads:\n", TAMANHOMATRIZ, nThreads);
        pthread_t threads[nThreads];
        Dados data[nThreads];
        long unsigned int **a, **b, **c;

        // alocação e inicialização das variáveis a serem utilizadas
        a = new long unsigned int*[TAMANHOMATRIZ];
        b = new long unsigned int*[TAMANHOMATRIZ];
        c = new long unsigned int*[TAMANHOMATRIZ];

        //insere o vetor b no contexto de todas as threads
        for(long unsigned int i = 0; i < nThreads; i++)
            data[i] = Dados(b, i+1);

        // preenchimento das matrizes a serem multiplicadas
        for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
        {
            a[i] = new long unsigned int[TAMANHOMATRIZ];
            b[i] = new long unsigned int[TAMANHOMATRIZ];
            c[i] = new long unsigned int[TAMANHOMATRIZ]{0};//iniciados preenchidos com zeros

            for(long unsigned int j = 0; j < TAMANHOMATRIZ; j++)
                    a[i][j] += i+j;

            //distribui as linhas entre as threads utilizando o resto da divisão da linha atual pela quantidade de threads
            data[i % nThreads].adicionarlinhas(a[i], c[i]);
        }
        for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)//calculando a transposta de b
            for(long unsigned int j = 0; j < TAMANHOMATRIZ; j++)
                    b[j][i] += i+j;
        //
        // criando as threads
        for(long unsigned int i = 0; i < nThreads; i++)
        {
            if(pthread_create(&threads[i], NULL, multiplicalinha, (void *)&data[i]) != 0)
                throw("erro ao criar thread i");
            mtx.lock();
            currentThreads++;// incrementa o contador global de threads abertas
            mtx.unlock();
        }

        // imprimindo o andamento
        printf("esperando encerramento das threads abertas\n");
        while(currentThreads > 0)
        {
            usleep(0.1);//só liberará o fluxo do código uma vez que todas as threads tenham sido executadas até o fim
        }
        printf("threads concluidas\n");

        // imprimindo o resultado
        printaMat(c, TAMANHOMATRIZ, "\n\tresultado:");
        // desalocando memória
        for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
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
