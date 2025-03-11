#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define CHALLENGE "2-XOR"

int main(){

    printf("GPU OPTIMISATION BENCHMARK\n")
    unsigned int start_time = 0;

    unsigned int stop_time = 0;

    const char * no_optim_bin_path = (const char *) malloc(sizeof(char)*strlen("./"+CHALLENGE+"/main_cpu_no_optim")+1);
    strcpy(no_optim_bin_path,"./"+CHALLENGE+"/main_cpu_no_optim");
    const char * gpu_optim_bin_path = (const char *) malloc(sizeof(char)*strlen("./"+CHALLENGE+"/main")+1);
    strcpy("./"+CHALLENGE+"/main");
    
    printf("Without GPU optimisation : ");
    start_time = time(NULL);
    system(no_optim_bin_path);
    stop_time = time(NULL);
    printf("Elapsed time (millis) : %d",stop_time-start_time);

    printf("With GPU optimisation : ");
    start_time = time(NULL);
    system(gpu_optim_bin_path);
    stop_time = time(NULL);
    printf("Elapsed time (millis) : %d",stop_time-start_time);
    return 0;
}