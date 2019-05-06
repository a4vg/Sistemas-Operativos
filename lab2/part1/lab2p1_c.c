#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h> // EXIT_...
#include <sys/wait.h>

#define R 0
#define W 1

int main(int argc, char const *argv[])
{
    const int npruebas = 5000;

    // time
    struct timeval start, end;
    long prom=0;
    double prom_sec;
    
    // read 0, write 1 
    int p2c[2]; // parent to child
    int c2p[2]; // child to parent
    char buffer;
    
    for (int i=0; i<npruebas; ++i){
        
        // Create pipes
        if (pipe(p2c)+pipe(c2p)<0) { perror("Can't create pipe"); return EXIT_FAILURE; }

        gettimeofday(&start, 0);
        switch(fork()){
            case -1: 
                perror("Can't create child");
                exit(EXIT_FAILURE);

            case 0: // child
                close(p2c[W]); // write parent->child
                close(c2p[R]); // read child->parent

                if (read(p2c[R], &buffer, 1)<1) { perror("Child failed to read"); return EXIT_FAILURE; }
                buffer = 'e';
                if (write(c2p[W], &buffer, 1)<1) { perror("Child failed to write"); return EXIT_FAILURE;};
                
                close(p2c[R]); // read parent->child
                close(c2p[W]); // write child->parent

                exit(EXIT_SUCCESS);

            default: // parent
                close(p2c[R]); // read parent->child
                close(c2p[W]); // write child->parent

                buffer = 'a';
                if (write(p2c[W], &buffer, 1)<1) { perror("Parent failed to write"); return EXIT_FAILURE; };
                if (read(c2p[R], &buffer, 1)<1) { perror("Parent failed to read"); return EXIT_FAILURE; };

                close(p2c[W]); // write parent->child
                close(c2p[R]); // read child->parent

                wait(NULL);
        }
        gettimeofday(&end, 0);

        prom += (end.tv_sec - start.tv_sec) * 1000000;
        prom +=  end.tv_usec - start.tv_usec;
    }
            
    prom_sec = (double) prom/npruebas;
    prom_sec/= 1000000.0; 
    printf("[%d medidas] Costo de context switch: %.10f segundos\n", npruebas, prom_sec);

    return EXIT_SUCCESS;
}