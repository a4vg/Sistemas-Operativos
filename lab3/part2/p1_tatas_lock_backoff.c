#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <pthread.h>

#include "hrtimer_x86.h"
#include "atomic_ops.h"

#define N 4

long g_cuenta = 0;
volatile unsigned long lock=0;

void acquire_lock(){
    int delay = rand() % 4096;
    
    while (1){
        while (lock==1); // spin
        if (!tas(&lock)) return; // lock acquired
        backoff(&delay); // max=4096, delay won't be greatee than 4096*2
    }
}

void* increment(void*){
    for (int i=0; i<5000; ++i){
        acquire_lock();
        ++g_cuenta;
        lock = 0; // release lock
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    /* Get time */
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

    srand(time(NULL)); // init rand

    for (int i=0; i<npruebas; ++i){
        secs_hr = gethrtime_x86();
        for (int i=0; i<N; ++i) pthread_create(&hilos[i], NULL, increment, NULL);
        for (int i=0; i<N; ++i) pthread_join(hilos[i], NULL);

        secs_hr_prom += gethrtime_x86()-secs_hr;
    }
    secs_hr_prom/=npruebas;

    printf("Cuenta: %li\n", g_cuenta);
    printf("[%d medidas] Costo del programa (%d hilos): %.5f segundos\n", npruebas, nhilos, secs_hr_prom);

    return 0;
}