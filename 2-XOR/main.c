#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#define DATA_SIZE 1024
#define KERNEL_FILE_READ_BUFFER_SIZE 1024

/*
Re-using vector adding code to make a parallel computed XOR :D

*/

struct KernelFile{
    unsigned int size;
    char * content;
};


void checkError(cl_int err, const char *operation) {
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error during operation '%s': %d\n", operation, err);
        exit(1);
    }
}


int main() {
    // loading kernel
    
    FILE* f = fopen("kernel.cl","rb");
    struct KernelFile* kernel_file = malloc(sizeof(struct KernelFile));

    fseek(f, 0, SEEK_END); 
    kernel_file->size = ftell(f)+1;
    fseek(f, 0, SEEK_SET);

    kernel_file->content = (char *) malloc(kernel_file->size+1);
    kernel_file->content[kernel_file->size] = '\0';

    fread(kernel_file->content,sizeof(char),kernel_file->size,f);

    fclose(f);


    // set seed for random bit generation
    srand(time(NULL));

    // Host (CPU) code
    bool A[DATA_SIZE], B[DATA_SIZE], C[DATA_SIZE];

    // Initialize data
    for (int i = 0; i < DATA_SIZE; i++) {
        A[i] = rand()%2;
        B[i] = rand()%2;
    }
    printf("Initialised random bit arrays.\n",A[0],B[0]);

    cl_int err = 0;
    
    // Get OpenCL platform
    cl_platform_id platforms[2];
    cl_uint num_platforms;
    err = clGetPlatformIDs(1, platforms, &num_platforms);

    // gpu is platform 0 on my computer
    printf("Number of platforms: %u\n", num_platforms);


    checkError(err,"clGetPlatformIDs");

    // Get GPU device
    cl_device_id devices[2];
    cl_uint num_devices = 0;
    err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 2, devices, &num_devices);
    checkError(err,"clGetDeviceIDs");

    printf("OpenCL found %d gpu devices\n",(unsigned int) num_devices);

    // we take the first one if multiple gpu are found
    //  ( on my laptop I only have intregrated intel GPU so no question asked xD )
    printf("device_id=%d\n",devices[0]);

    // Print device name
    char device_name[128];
    const cl_device_id * device = devices;
    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, sizeof(device_name), device_name, NULL);
    printf("Device name: %s\n", device_name);


    // Create OpenCL context
    cl_context context = clCreateContext(NULL, 1,devices, NULL, NULL, &err);
    checkError(err,"clCreateContext");


    // Create command queue
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, devices[0], NULL, NULL);


    // Create memory buffers
    // CL_MEM_READ_ONLY readonly mem
    // CL_MEM_COPY_HOST_PTR get data from CPU (here, A and B)
    // sizeof(A) renvoie la taille de A en octets, mais nous voulons le nombre de bits de A
    // d'où l'utilisation de sizeof(bool)*DATA_SIZE, car chaque élément est un bit
    cl_mem d_A = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(bool)*DATA_SIZE, A, NULL);
    cl_mem d_B = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(bool)*DATA_SIZE, B, NULL);
    cl_mem d_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY,sizeof(bool)*DATA_SIZE , NULL, NULL);

    // Create and build program
    const char ** kernel_code_pointer = (const char **) &(kernel_file->content);
    cl_program program = clCreateProgramWithSource(context, 1,kernel_code_pointer, NULL, NULL);
    clBuildProgram(program, 1, devices, NULL, NULL, NULL);

    // Create kernel
    cl_kernel kernel = clCreateKernel(program, "parallel_xor", NULL);

    // Set kernel arguments
    // here, we need to give kernel access to the GPU buffers
    // where we stored a copy of the orginial CPU-memory based vectors
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_A);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_B);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_C);


    // Execute kernel
    size_t globalSize = DATA_SIZE;
    // ND Range means we will specify how many threads we want to use
    // in that case, 
    // In my head the ideal solution would be as many threads as array elements
    // that we want to add
    // so I gave that
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);

    // Read back results
    // CL_TRUE => wait for GPU to return its results before contiuing
    // the classic CPU-based program
    clEnqueueReadBuffer(queue, d_C, CL_TRUE, 0, sizeof(bool)*DATA_SIZE, C, 0, NULL, NULL);

    // Print results
    
    for (int i = 0; i < DATA_SIZE; i++){
        printf("A[%d] = %d",i,A[i]);
        printf("B[%d] = %d",i,B[i]);
        printf("A[%d] XOR B[%d] = %d\n",i,i,C[i]);
    }


    // Cleanup
    clReleaseMemObject(d_A);
    clReleaseMemObject(d_B);
    clReleaseMemObject(d_C);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}
