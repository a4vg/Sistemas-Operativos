#include <stdio.h>
#include <stdlib.h> // calloc(), malloc(), srand()
#include <time.h> // time()

/* Matrix init functions */
int** createMatrix(int r, int c){
    int **m = (int **) malloc(r * sizeof(int*));
    for (int i=0; i<r; ++i) m[i] = (int *) calloc(c, sizeof(int));
    return m;
}

void fillMatrix(int** m, int r, int c){
    srand(time(NULL));
    for (int i = 0; i <  r; ++i)
      for (int j = 0; j < c; ++j)
         m[i][j] = rand() % 6; // rand 0-6
}

void printMatrix(int** m, int r, int c){
    for (int i = 0; i <  r; ++i){
          for (int j = 0; j < c; ++j)
            printf("%3d ", m[i][j]);
        printf("\n");
    }
}

void freeMatrix(int** m, int r){
    for (int i = 0; i <  r; ++i){
        int* tmp = m[i];
        free(tmp);
    }
}