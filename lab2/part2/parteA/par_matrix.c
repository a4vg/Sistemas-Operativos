#include "initfun_matrix.h"
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

#define N 2

struct funParameters{
    int** A;
    int** B;
    int** Ans;
    int cArB;
    int cB;
    int from;
    int to;
};

void* setMultipliedElement(void* par){
    struct funParameters *p = (struct funParameters*) par;
    for (int k=p->from; k<p->to; ++k)
        for (int i=0; i<p->cB; ++i)
            for (int j=0; j<p->cArB; ++j)
                p->Ans[k][i] += p->A[k][j] * p->B[j][i];

    pthread_exit(NULL);
}


int main(int argc, char const *argv[])
{
    const int npruebas = 20;
    struct timeval start, end;
    long prom=0;
    double prom_sec;

    pthread_t hilos[N];

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

    // Iterate through matrix ans
    struct funParameters* params =  malloc(sizeof( struct funParameters ));
    params->A = mA;
    params->B = mB;
    params->Ans = mC;
    params->cArB = cA;
    params->cB = cB;
    params->from=0;
    int each_thr = ceil( (float) rA/N);
    params->to=each_thr;

    int useful_thr = rA*(N>rA) + N*(N<=rA);
    
    for (int i=0; i<npruebas; ++i){
        for (int counter=0; counter<useful_thr; ++counter){

            gettimeofday(&start, 0);

            int rc = pthread_create(&hilos[counter], NULL, setMultipliedElement, (void*) params);
            if (rc) printf("ERROR in %i\n", counter);
            pthread_join(hilos[counter], NULL);

            gettimeofday(&end, 0);

            params->from = params->to;
            params->to = params->from + each_thr;
            if (params->to>rA) params->to = rA;

            prom += (end.tv_sec - start.tv_sec) * 1000000;
            prom +=  end.tv_usec - start.tv_usec;
        }
    }

    prom_sec = (double) prom/npruebas;
    prom_sec/= 1000000.0;
    printf("[%d medidas] Costo de multiplicación paralela (%d hilos): %.5f segundos\n", npruebas, N, prom_sec);

    freeMatrix(mA, rA);
    freeMatrix(mB, rB);
    freeMatrix(mC, rA);

    free (params);

    return 0;
}