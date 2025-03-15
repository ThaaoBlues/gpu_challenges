#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>


#define DATA_SIZE 1024


int main() {
    // Host (CPU) code
    bool A[DATA_SIZE], B[DATA_SIZE], C[DATA_SIZE];

    // Initialize data
    for (int i = 0; i < DATA_SIZE; i++) {
        A[i] = rand()%2;
        B[i] = rand()%2;
    }
    printf("Initialised random bit arrays.\n");

    for(int i = 0;i<DATA_SIZE;i++){
        C[i] = A[i] ^ B[i];
    }


    for (int i = 0; i < DATA_SIZE; i++){
        printf("A[%d] = %d",i,A[i]);
        printf("B[%d] = %d",i,B[i]);
        printf("A[%d] XOR B[%d] = %d\n",i,i,C[i]);
    }

}