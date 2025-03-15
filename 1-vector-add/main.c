#include <stdio.h>
#include <stdlib.h>
#include <CL/cl.h>

#define DATA_SIZE 1024

// OpenCL kernel (device code)
const char *kernelSource = "__kernel void vector_add(__global const float *A, __global const float *B, __global float *C) {"
                           "    int id = get_global_id(0);"
                           "    C[id] = A[id] + B[id];"
                           "}";

// After a discussion with chagpt, 
// I undestood that each work-item run an instance of the kernel in parallel
// Here, as we have as many work-item as array elements
// we can get the id of the work-item while it executes the kernel
// so we can add the arrays elements corresponding to this id
// parallelism comes from here


// Each work item has 3 coordinates, (x,y,z)
// so we can base calculations on these coordinates
// to allow up to 3-dimensions parallel calculations


int main() {
    // Host (CPU) code
    float A[DATA_SIZE], B[DATA_SIZE], C[DATA_SIZE];

    // Initialize data
    for (int i = 0; i < DATA_SIZE; i++) {
        A[i] = i * 1.0f;
        B[i] = i * 2.0f;
    }

    // Get OpenCL platform
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);

    // Get GPU device
    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

    printf("%d\n",device);
    // Create OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);

    // Create command queue
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, NULL, NULL);

    // Create memory buffers
    // CL_MEM_READ_ONLY readonly mem
    // CL_MEM_COPY_HOST_PTR get data from CPU (here, A and B)

    // STACKOVERFLOW :
    /*
    CL_MEM_COPY_HOST_PTR simply copies the values at a time of creation of the buffer.

    CL_MEM_USE_HOST_PTR maintains a reference to that memory area and depending
    on the implementation it might access it directly while kernels are executing or 
    it might cache it. You must use mapbuffer to provide synchronization points 
    if you want to write cross platform code using this.

    CL_MEM_ALLOC_HOST_PTR is the only one that is often pinned memory. 
    As an example on AMD this one allocates a pinned memory area. 
    Often if you use CL_MEM_USE_HOST_PTR it will simply memcpy internally to a pinned memory 
    area and use that. 
    By using ALLOC_HOST_PTR you will avoid that. 
    But yet again this depends on the implementation and you must read the manufacturers 
    documentation on if this will provide you with pinned memory or not.
    */

    cl_mem d_A = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(A), A, NULL);
    cl_mem d_B = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(B), B, NULL);
    cl_mem d_C = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(C), NULL, NULL);

    // Create and build program
    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, NULL, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);

    // Create kernel
    cl_kernel kernel = clCreateKernel(program, "vector_add", NULL);

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
    clEnqueueReadBuffer(queue, d_C, CL_TRUE, 0, sizeof(C), C, 0, NULL, NULL);

    // Print results
    for (int i = 0; i < 10; i++)
        printf("A[%d]+B[%d] = C[%d] = %f+%f = %f\n", i,i,i,A[i],B[i],C[i]);

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
