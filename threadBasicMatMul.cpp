#include <iostream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

#define NLINHAS 3 // Numero de linhas e numero de threads unificados em NLINHAS

/*Esse código é uma interpretação própria de como poderia ser um tratamento multithread para multiplicar matrizes e deve ser visto como uma abordagem inicial ao problema,
 * Mas não é uma boa solução.
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
    for(int i = 0; i < NLINHAS; i++)
    {
        for(int j = 0; j < NLINHAS; j++)
            ptr->C[i] += ptr->A[j] * ptr->B[i][j];
    }
    printf("thread#%02d: concluída\n", ptr->threadId);
    currentThreads--;//decrementa 1 do número global de threads abertas, já que essa será fechada
    pthread_exit(NULL);
}

void threadMatMul(long unsigned int **a, long unsigned **b, long unsigned **c)
{
    pthread_t threads[NLINHAS];
    Dados data[NLINHAS];
    // Criando as threads
    for(int i = 0; i < NLINHAS; i++)
    {
        data[i] = Dados(a[i], b, c[i], i);// Cada thread recebe uma entidade de Dados
        pthread_create(&threads[i], NULL, multiplicaLinha, (void *)&data[i]);
        currentThreads++;// Incrementa o contador global de threads abertas
    }

    // Imprimindo o andamento

    printf("esperando encerramento das threads abertas\n");
    while(currentThreads > 0){
        sleep(0.0000001);//para não consumir muito processamento ao esperar
    }//só liberará o fluxo do código uma vez que todas as threads tenham sido executadas até o fim

    printf("threads concluidas\n");
}


using namespace std;
int main()
{
    long unsigned int **a, **b, **binv, **c;
    // Alocação e inicialização das variáveis a serem utilizadas
    a = new long unsigned int*[NLINHAS];
    b = new long unsigned int*[NLINHAS];
    c = new long unsigned int*[NLINHAS];

    // Preenchimento das matrizes a serem multiplicadas
    for(int i = 0; i < NLINHAS; i++)
    {
        a[i] = new long unsigned int[NLINHAS];
        b[i] = new long unsigned int[NLINHAS];
        c[i] = new long unsigned int[NLINHAS]{0};//iniciados preenchidos com zeros
        for(int j = 0; j < NLINHAS; j++)
            {
                a[i][j] += i+j;
                b[i][j] += i+j;
            }
    }
    //realizando a multiplicação
    threadMatMul(a, b, c);
    printaMat(a, NLINHAS, "matriz a:");
    printaMat(b, NLINHAS, "matriz b:");
    printaMat(c, NLINHAS, "matriz c:");
    return 0;
}
