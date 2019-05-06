#include <stdio.h>
#include <string>
#include <pthread.h>

#include "hrtimer_x86.h"

#define N 4

long g_cuenta = 0;

void* increment(void*){
    for (int i=0; i<50000; ++i) ++g_cuenta;

    return NULL;
}

int main(int argc, char const *argv[])
{
    // ./a.out pruebas hilos

    /* Get time */
    int npruebas;
    int nhilos;
    if (argc>1){
        npruebas = atoi(argv[1]);
        nhilos = atoi(argv[2]);
    }
    else{
        npruebas = 20;
        nhilos = N;
    }
    double secs_hr;
    double secs_hr_prom=0;

    /* Program */
    pthread_t hilos[nhilos];

    for (int i=0; i<npruebas; ++i){
        secs_hr = gethrtime_x86();
        for (int i=0; i<nhilos; ++i) pthread_create(&hilos[i], NULL, increment, NULL);
        for (int i=0; i<nhilos; ++i) pthread_join(hilos[i], NULL);
        
        secs_hr_prom += gethrtime_x86()-secs_hr;
    }

    secs_hr_prom/=npruebas;

    printf("Cuenta: %li\n", g_cuenta);
    printf("[%d medidas] Costo del programa (%d hilos): %.5f segundos\n", npruebas, nhilos, secs_hr_prom);

    return 0;
}