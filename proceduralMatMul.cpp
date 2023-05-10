#include<iostream>

/*
 * Esse código é escrito em C++ e realiza a multiplicação de matrizes utilizando dois métodos diferentes: proceduralMatMul e proceduralMatMulTrans. O primeiro método realiza a multiplicação sequencial sem transposição da matriz b, enquanto o segundo realiza a multiplicação sequencial com a matriz b transposta.

O método printaMat é usado para imprimir as matrizes a, b e c na tela, para verificarmos o resultado da multiplicação.

Na função main, são inicializadas quatro matrizes dinamicamente, preenchidas com valores e multiplicadas utilizando o método proceduralMatMulTrans. Em seguida, as matrizes são impressas e a memória alocada é desalocada.
*/

long unsigned int TAMANHOMATRIZ = 3; 

void printaMat(long unsigned int **mat, long unsigned TAMANHOMATRIZ, const char* titulo)
{
    printf("%s\n", titulo);
    for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
    {
        for(long unsigned int j = 0; j < TAMANHOMATRIZ; j++)
            printf("%5ld ", mat[i][j]);
        printf("\n");
    }
}

void proceduralMatMul(long unsigned int** a, long unsigned int **b, long unsigned int **c)
{
    /*Realiza a multiplicação sequencial sem thread e sem transposição da matriz b
     */
    for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
    {
        for(long unsigned int j = 0; j< TAMANHOMATRIZ; j++)
        {
            for(long unsigned int x = 0; x < TAMANHOMATRIZ; x++)
                c[i][j] += a[i][x]*b[x][j];
        }
    }
}
void proceduralMatMulTrans(long unsigned int** a, long unsigned int **b, long unsigned int **c)
{
    /*Realiza a multiplicação sequencial sem thread com a matriz b transposta;
     * A vantagem disso é que o loop mais interno iterará apenas sobre elementos em uma mesma linha na memória, 
     * oque evita a possibilidade de o processar precisar buscar uma nova linha inteira para cada alteração no valor de x.
     */
    for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
    {
        for(long unsigned int j = 0; j< TAMANHOMATRIZ; j++)
        {
            for(long unsigned int x = 0; x < TAMANHOMATRIZ; x++)
                c[i][j] += a[i][x]*b[j][x];
        }
    }
}


using namespace std;
int main(int argc, char* argv[])
{
    if(argc > 1)//caso receba valor específico para tamanho de matriz
        TAMANHOMATRIZ = strtoul(argv[1], NULL, 10);

    long unsigned int **a, **b, **btrans, **c;
    //inicializando matrizes simultaneamente
    a = new long unsigned int*[TAMANHOMATRIZ];
    b = new long unsigned int*[TAMANHOMATRIZ];
    btrans = new long unsigned int*[TAMANHOMATRIZ];
    c = new long unsigned int*[TAMANHOMATRIZ];
    for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
    {
        a[i] = new long unsigned int[TAMANHOMATRIZ]{0};
        b[i] = new long unsigned int[TAMANHOMATRIZ]{0};
        btrans[i] = new long unsigned int[TAMANHOMATRIZ]{0};
        c[i] = new long unsigned int[TAMANHOMATRIZ]{0};
    }
    //preenchendo as matrizes
    for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
        for(long unsigned int j = 0; j < TAMANHOMATRIZ; j++)
            {
                a[i][j] += i+j;
                b[i][j] += i+j;
                btrans[j][i] += i+j;
            }
    //proceduralMatMul(a, b, c);
    proceduralMatMulTrans(a, btrans, c);
    //realizando a multiplicação
    printaMat(a, TAMANHOMATRIZ, "matriz a:");
    printaMat(b, TAMANHOMATRIZ, "matriz b:");
    printaMat(c, TAMANHOMATRIZ, "matriz c:");
    //
    //desalocando memória
    for(long unsigned int i = 0; i < TAMANHOMATRIZ; i++)
    {
        free(a[i]);
        free(b[i]);
        free(btrans[i]);
        free(c[i]);
    }
    free(a);
    free(b);
    free(btrans);
    free(c);
    return 0;
}
