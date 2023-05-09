#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <mutex>
#include <vector>

#define debug(x, y) printf(x, y)

using namespace std;
/*
 * O código é uma implementação C++ de um algoritmo de multiplicação de matrizes usando threads. O programa cria uma thread para cada combinação de linhas e colunas das duas matrizes de entrada, sendo cada thread responsável por calcular um elemento da matriz de saída. O número de threads criadas é determinado pela constante "TAMANHOMATRIZ", que representa o número de linhas e colunas nas matrizes. O código usa um mutex para proteger os dados compartilhados e um vetor para acompanhar as threads e os dados
 * O programa inicializa as matrizes de entrada "a" e "b" com valores baseados na soma de seus índices de linha e coluna. O programa também inicializa a matriz de saída "c" com zeros. Depois de criar os threads, o programa espera até que todos os threads sejam concluídos antes de imprimir a matriz resultante e liberar a memória alocada.
 * */

#define TAMANHOMATRIZ 5 // Numero de linhas e numero de threads unificados em TAMANHOMATRIZ

int currentThreads = 0; // Variável global controle para quantidade de threads abertas
mutex mtx;

struct Dados
{
    long unsigned int *A, *C, *B;// A é referencia para linha de matriz linha x coluna, B é referencia pra linha da matriz transposta de B(seria a coluna de uma matriz b que multiplica a) e C é o endereço de um inteiro
    long unsigned int threadId, i, j;

    Dados(long unsigned int *a, long unsigned int *b, long unsigned int *c, long unsigned int t, long unsigned int I, long unsigned int J)
    {
        A = a;
        B = b;
        C = c;
        threadId = t;
        i = I;
        j = J;
    }
    Dados()// O compulador pediu então implementei
    {
        A = NULL;
        B = NULL;
        C = NULL;
        threadId = 0;
        i = 0;
        j = 0;
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

void *multiplicaLinha(void* dados)
{
    Dados *ptr = (Dados *) dados;
    for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
    {
        mtx.lock();
        *(ptr->C) += ptr->A[i] * ptr->B[i];
        mtx.unlock();
    }
    mtx.lock();
    currentThreads--;//Decrementa o numero de threads
    mtx.unlock();
    printf("thread#%02lu: concluída\n", ptr->threadId);
    pthread_exit(NULL);
}

int main()
{
    try
    {
        vector<pthread_t*> threads;
        vector<Dados*> data;
        pthread_t* ptr = NULL;
        Dados* dataptr = NULL;
        long unsigned int **a, **b, **c;

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
            for(int j = 0; j < TAMANHOMATRIZ; j++)
                    a[i][j] += i+j;
        }
        for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)//calculando a transposta de B
            for(int j = 0; j < TAMANHOMATRIZ; j++)
                {
                    b[j][i] += i+j;
                }
        // Criando as threads
        for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
            for(long unsigned int j = 0; j < TAMANHOMATRIZ; j++)
            {
                dataptr = new Dados(a[i], b[j], &c[i][j], i*TAMANHOMATRIZ + j, i, j);
                if(dataptr != NULL)
                    data.push_back(dataptr);
                else
                    throw("Erro de alocação de Data");
                ptr = (pthread_t*) malloc(sizeof(pthread_t));
                if(ptr != NULL)
                    threads.push_back(ptr);
                else
                    throw("Erro de alocação de thread");

                while(currentThreads >= MAXTHREADS)//não abra novas threads enquanto tiver mais do que >MAXTHREADS< rodando
                    usleep(0.1);

                if (pthread_create(threads.back(), NULL, multiplicaLinha, data.back()) != 0)
                    throw("Erro ao criar thread");
                mtx.lock();
                currentThreads++;// Incrementa o contador global de threads abertas
                mtx.unlock();
            }

        // Imprimindo o andamento
        printf("esperando encerramento das threads abertas\n");
        while(currentThreads > 0){
            usleep(0.1);
        }//só liberará o fluxo do código uma vez que todas as threads tenham sido executadas até o fim
        printf("threads concluidas\n");

        // Imprimindo o resultado
        printaMat(c, TAMANHOMATRIZ, "\n\tResultado:");
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
        while(threads.empty() == false)
        {
            ptr = threads.back();
            free(ptr);
            threads.pop_back();
        }
        while(data.empty() == false)
        {
            dataptr = data.back();
            free(dataptr);
            data.pop_back();
        }
        data.clear();
        threads.clear();
        return 0;
    }catch(const exception &e)
    {
        cerr<<e.what()<<endl;
    }
    return 0;
}
