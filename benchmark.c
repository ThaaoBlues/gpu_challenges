#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define CHALLENGE "2-XOR"

int main(){

    printf("GPU OPTIMISATION BENCHMARK\n");
    time_t start_time = 0;

    time_t stop_time = 0;

    const char * no_optim_bin_path = (const char *) malloc(sizeof(char)*200);
    sprintf(no_optim_bin_path,"./%s/main_cpu_no_optim",CHALLENGE);


    const char * gpu_optim_bin_path = (const char *) malloc(sizeof(char)*200);
    sprintf(gpu_optim_bin_path,"./%s/main",CHALLENGE);
    
    printf("Without GPU optimisation : \n");
    time(&start_time);
    system(no_optim_bin_path);
    time(&stop_time);
    printf("Elapsed time (millis) : %g\n",difftime(stop_time,start_time));

    printf("With GPU optimisation : \n");
    start_time = time(NULL);
    system(gpu_optim_bin_path);
    stop_time = time(NULL);
    printf("Elapsed time (millis) : %g\n",difftime(stop_time,start_time));
    return 0;
}