#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    const int npruebas = 5000;

    struct timeval start, end;
    long prom=0;
    double prom_sec;

    for (int i=0; i<npruebas; ++i){
        gettimeofday(&start, 0);
        getpid();
        gettimeofday(&end, 0);

        prom += (end.tv_sec - start.tv_sec) * 1000000;
        prom +=  end.tv_usec - start.tv_usec;
    }

    prom_sec = (double) prom/npruebas;
    prom_sec/= 1000000.0;

    printf("[%d medidas] Costo de syscall (getpid()): %.10f segundos\n", npruebas, prom_sec);

    return 0;
}