#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h> // exit
#include <string.h> // memset


#define NPHIL 5
#define NMAKIS 20

float tiempo = 0;
int g_philEating = 0;
int g_philsLeft = NPHIL;
int g_makis[NPHIL] = {0};
_Bool g_chopsticks[NPHIL];
pthread_barrier_t barrier;
pthread_mutex_t lock;

_Bool Eat(int n){
    printf("\x1b[38;5;196mFilósofo %i está comiendo\x1b[0m\n", n);
    ++g_makis[n];

    if (g_makis[n]==NMAKIS) {
        printf("Phil %i is full... Bye\n", n);

        return 1; 
    }
    return 0;
}

_Bool TakeChopstick(int n){
    pthread_mutex_lock(&lock);
    _Bool left=0, right=0;
    if (!g_chopsticks[n]){
        g_chopsticks[n] = 1;
        left = 1;
        // printf("Phil %i took a chopstick\n", n);
    }
    if (!g_chopsticks[(n+1)%NPHIL]){
        g_chopsticks[(n+1)%NPHIL] = 1;
        right = 1;
        // printf("Phil %i took a chopstick\n", n);
    }
    pthread_mutex_unlock(&lock);
    return right && left;

}
    
void *Compete(void *i){
    int n = *((int*) i);
    _Bool full = 0;
    _Bool won;
    while(1){
        if (full && won) --g_philsLeft; // just won
        pthread_barrier_wait(&barrier);

        if (!g_philsLeft) break;

        memset(g_chopsticks, 0, sizeof(g_chopsticks));
        g_philEating = 0;
        won=0;

        pthread_barrier_wait(&barrier);

        if (!full) won = TakeChopstick(n);
        
        if (won){
            pthread_mutex_lock(&lock);
            ++g_philEating;
            pthread_mutex_unlock(&lock);
        }
        
        pthread_barrier_wait(&barrier);
        if (g_philEating){
            if (won){
                full = Eat(n);
                tiempo+= 4/g_philEating;
            }
            else if (!full) printf("\x1b[38;5;111mFilósofo %i está hablando\x1b[0m\n", n);
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    const int npruebas = 20;

    for (int c=0; c<npruebas; ++c){
        g_philsLeft = NPHIL;
        memset(g_makis, 0, sizeof(g_makis));

        pthread_t philosophers[NPHIL];
        pthread_barrier_init(&barrier, NULL, NPHIL);
        
        int ids[NPHIL];
        for (int i=0; i<NPHIL; ++i) { ids[i]=i; pthread_create(&philosophers[i], NULL, Compete, (void*) &ids[i]); }
        for (int i=0; i<NPHIL; ++i) pthread_join(philosophers[i], NULL);
        printf("Acabado #%i\n\n\n", c+1);
        pthread_barrier_destroy(&barrier);
    }

    printf("[%i medidas] Tiempo ficticio: %f segundos\n", npruebas, tiempo/npruebas);

    
    return 0;
}