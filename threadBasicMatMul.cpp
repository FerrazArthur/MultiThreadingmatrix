#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
using namespace std;
mutex mtx;
#define TAMANHOMATRIZ 3 // Numero de linhas e numero de threads unificados em TAMANHOMATRIZ

/*
 * Esse algoritmo implementa uma abordagem de multiplicação de matrizes utilizando threads em C++. O número de threads utilizado é definido pela constante TAMANHOMATRIZ, que também define o número de linhas e colunas das matrizes a serem multiplicadas.

As threads são criadas no método threadMatMul, que recebe como parâmetros as matrizes a e b e a matriz resultante c. Dentro desse método, as threads são criadas utilizando a função pthread_create, passando como argumentos um identificador da thread, um ponteiro para uma estrutura de dados Dados e o método multiplicaLinha, que é responsável pela multiplicação das linhas da matriz a pelas colunas da matriz b, preenchendo a matriz resultante c.

O método multiplicaLinha recebe como parâmetro um ponteiro para uma estrutura Dados, que contém as referências para as linhas da matriz a, a matriz b e a linha da matriz c que será preenchida. A multiplicação é feita utilizando um loop aninhado, onde cada elemento da linha de a é multiplicado pelos elementos da coluna correspondente em b, e o resultado é somado na posição correspondente na matriz c. Ao final, a thread é finalizada com pthread_exit.

Após a criação das threads, o método threadMatMul aguarda até que todas as threads tenham finalizado, utilizando um loop com a função sleep, verificando se o contador global de threads ainda é maior que zero. Quando todas as threads finalizam, a função printaMat é chamada para imprimir as matrizes a, b e c.

Por fim, são desalocados os ponteiros utilizados na memória com os operadores free.

É importante mencionar que essa abordagem não é a mais eficiente para a multiplicação de matrizes em termos de desempenho e que a utilização de mutexes pode ser necessária para garantir a integridade dos dados.
 *
 */
int currentThreads = 0; // Variável global controle para quantidade de threads abertas

struct Dados
{
    long unsigned int *A, *C, **B;// A e C são Referências para linhas de matriz linha x coluna, B é referencia pra matriz inteira linha x coluna
    int threadId;

    Dados(long unsigned int *a, long unsigned int **b, long unsigned int *c, int t)
    {
        A = a;
        B = b;
        C = c;
        threadId = t;
    }
    Dados()// O compulador pediu então implementei
    {
        A = NULL;
        B = NULL;
        C = NULL;
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
    for(int i = 0; i < TAMANHOMATRIZ; i++)
    {
        for(int j = 0; j < TAMANHOMATRIZ; j++)
            ptr->C[i] += ptr->A[j] * ptr->B[i][j];
    }
    printf("thread#%02d: concluída\n", ptr->threadId);
    currentThreads--;//decrementa 1 do número global de threads abertas, já que essa será fechada
    pthread_exit(NULL);
}

void threadMatMul(long unsigned int **a, long unsigned **b, long unsigned **c)
{
    pthread_t threads[TAMANHOMATRIZ];
    Dados data[TAMANHOMATRIZ];
    // Criando as threads
    for(int i = 0; i < TAMANHOMATRIZ; i++)
    {
        data[i] = Dados(a[i], b, c[i], i);// Cada thread recebe uma entidade de Dados
        pthread_create(&threads[i], NULL, multiplicaLinha, (void *)&data[i]);
        mtx.lock();
        currentThreads++;// Incrementa o contador global de threads abertas
        mtx.unlock();
    }

    // Imprimindo o andamento

    printf("esperando encerramento das threads abertas\n");
    while(currentThreads > 0){
        usleep(0.1);//para não consumir muito processamento ao esperar
    }//só liberará o fluxo do código uma vez que todas as threads tenham sido executadas até o fim

    printf("threads concluidas\n");
}


using namespace std;
int main()
{
    long unsigned int **a, **b, **binv, **c;
    // Alocação e inicialização das variáveis a serem utilizadas
    a = new long unsigned int*[TAMANHOMATRIZ];
    b = new long unsigned int*[TAMANHOMATRIZ];
    c = new long unsigned int*[TAMANHOMATRIZ];

    // Preenchimento das matrizes a serem multiplicadas
    for(int i = 0; i < TAMANHOMATRIZ; i++)
    {
        a[i] = new long unsigned int[TAMANHOMATRIZ];
        b[i] = new long unsigned int[TAMANHOMATRIZ];
        c[i] = new long unsigned int[TAMANHOMATRIZ]{0};//iniciados preenchidos com zeros
        for(int j = 0; j < TAMANHOMATRIZ; j++)
            {
                a[i][j] += i+j;
                b[i][j] += i+j;
            }
    }
    //realizando a multiplicação
    threadMatMul(a, b, c);
    printaMat(a, TAMANHOMATRIZ, "matriz a:");
    printaMat(b, TAMANHOMATRIZ, "matriz b:");
    printaMat(c, TAMANHOMATRIZ, "matriz c:");
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
}
