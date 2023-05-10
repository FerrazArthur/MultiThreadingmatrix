#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
using namespace std;

mutex mtx;
long unsigned int TAMANHOMATRIZ = 3; // Numero de linhas e numero de threads unificados em TAMANHOMATRIZ
#define MAXTHREADS 8

/*
 *Este código implementa uma solução multithread para multiplicação de matrizes. A matriz resultante da multiplicação é armazenada em uma matriz C que é inicializada com zeros. O número de threads que podem ser abertas simultaneamente é limitado e controlado pela variável global currentThreads.

A multiplicação de matrizes é realizada por meio de uma função multiplicaLinha, que é chamada por cada thread. Cada thread recebe uma entidade Dados que contém as referências para as matrizes A, B e C, além de um identificador de thread.

A função threadMatMul cria e inicia as threads para a multiplicação. Ela também aguarda a conclusão de todas as threads antes de imprimir a matriz resultante.

A função printaMat é utilizada para imprimir as matrizes A, B e C.

Na função principal (main), as matrizes A, B e C são alocadas e inicializadas. O número máximo de threads simultâneas é definido pela constante MAXTHREADS. Em seguida, a multiplicação é realizada e as matrizes são impressas. Por fim, a memória alocada é liberada.
 * Implementa um limite de quantas threads concorrentes podem existir, mas ainda não é uma boa solução.
 */
long unsigned int currentThreads = 0; // Variável global controle para quantidade de threads abertas

struct Dados
{
    long unsigned int *A, *C, **B;// A e C são Referências para linhas de matriz linha x coluna, B é referencia pra matriz inteira linha x coluna
    long unsigned int threadId;

    Dados(long unsigned int *a, long unsigned int **b, long unsigned int *c, long unsigned int t)
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
    for(long unsigned int i = 0; i < n; i++)
    {
        for(long unsigned int j = 0; j < n; j++)
            printf("%5ld ", mat[i][j]);
        printf("\n");
    }
}

void *multiplicaLinha(void *dados)
{
    Dados *ptr = (Dados *) dados;
    for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
    {
        for(long unsigned int j = 0; j < TAMANHOMATRIZ; j++)
            ptr->C[i] += ptr->A[j] * ptr->B[i][j];
    }
    printf("thread#%02lu: concluída\n", ptr->threadId);
    currentThreads--;//decrementa 1 do número global de threads abertas, já que essa será fechada
    pthread_exit(NULL);
}

void threadMatMul(long unsigned int **a, long unsigned **b, long unsigned **c)
{
    pthread_t threads[TAMANHOMATRIZ];
    Dados data[TAMANHOMATRIZ];
    // Criando as threads
    for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
    {
        data[i] = Dados(a[i], b, c[i], i);// Cada thread recebe uma entidade de Dados
        while(currentThreads > MAXTHREADS)//espera até que alguma thread finalize para abrir uma nova, caso seja necessário
            usleep(0.1);
        pthread_create(&threads[i], NULL, multiplicaLinha, (void *)&data[i]);
        mtx.lock();
        currentThreads++;// Incrementa o contador global de threads abertas
        mtx.unlock();
    }

    // Imprimindo o andamento

    printf("esperando encerramento das threads abertas\n");
    while(currentThreads > 0){
        sleep(0.1);//para não consumir muito processamento ao esperar
    }//só liberará o fluxo do código uma vez que todas as threads tenham sido executadas até o fim

    printf("threads concluidas\n");
}


using namespace std;
int main(int argc, char* argv[])
{
    if(argc > 1)//caso receba valor específico para tamanho de matriz
        TAMANHOMATRIZ = strtoul(argv[1], NULL, 10);

    long unsigned int **a, **b, **binv, **c;
    // Alocação e inicialização das variáveis a serem utilizadas
    a = new long unsigned int*[TAMANHOMATRIZ];
    b = new long unsigned int*[TAMANHOMATRIZ];
    c = new long unsigned int*[TAMANHOMATRIZ];

    // Preenchimento das matrizes a serem multiplicadas
    for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
    {
        a[i] = new long unsigned int[TAMANHOMATRIZ];
        b[i] = new long unsigned int[TAMANHOMATRIZ];
        c[i] = new long unsigned int[TAMANHOMATRIZ]{0};//iniciados preenchidos com zeros
        for(long unsigned int j = 0; j < TAMANHOMATRIZ; j++)
            {
                a[i][j] += i+j;
                b[i][j] += i+j;
            }
    }
    printf("Usando %d threads simultâneas\n", MAXTHREADS);
    //realizando a multiplicação
    threadMatMul(a, b, c);
    printaMat(a, TAMANHOMATRIZ, "matriz a:");
    printaMat(b, TAMANHOMATRIZ, "matriz b:");
    printaMat(c, TAMANHOMATRIZ, "matriz c:");
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
}
