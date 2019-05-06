#include "initfun_matrix.h"
#include <sys/time.h>

void multiplyMatrix(int** A, int** B, int** Ans, int rA, int cArB, int cB){
    for (int i=0; i<rA; ++i)
        for (int j=0; j<cB; ++j)
            for (int p=0; p<cArB; ++p)
                Ans[i][j] += A[i][p] * B[p][j];
}

int main(int argc, char const *argv[])
{
    const int npruebas = 1;

    struct timeval start, end;
    long prom=0;
    double prom_sec;


    // rows x columns
    int rA, cA, rB, cB;
    do{
        printf("Requirement: matrix A columns = matrix B rows\n");
        printf("Dimensions of matrix A (row col)\n> ");
        scanf("%d %d", &rA, &cA);

        printf("Dimensions of matrix B (row col)\n> ");
        scanf("%d %d", &rB, &cB);
    } while (cA!=rB);


    /* Initialize */
    int** mA = createMatrix(rA,cA);
    int** mB = createMatrix(rB,cB);
    int** mC = createMatrix(rA, cB);

    fillMatrix(mA, rA, cA);
    fillMatrix(mB, rB, cB);

    for (int i=0; i<npruebas; ++i){
        gettimeofday(&start, 0);
        multiplyMatrix(mA, mB, mC, rA, cA, cB);
        gettimeofday(&end, 0);

        prom += (end.tv_sec - start.tv_sec) * 1000000;
        prom +=  end.tv_usec - start.tv_usec;
    }
    prom_sec = (double) prom/npruebas;
    prom_sec/= 1000000.0;

    printf("[%d medidas] Costo de multiplicación secuencial: %.5f segundos\n", npruebas, prom_sec);

    freeMatrix(mA, rA);
    freeMatrix(mB, rB);
    freeMatrix(mC, rA);


/*
    printf("\nMatrix Ans\n");
    printMatrix(mC, rA, cB);
    */



    return 0;
}