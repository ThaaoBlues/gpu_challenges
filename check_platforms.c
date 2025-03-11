#include <stdio.h>
#include <CL/cl.h>

int main() {

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

    printf("Checking metadata of the first device found.\n");
    printf("device_id=%d\n",devices[0]);

    // Print device name
    char device_name[128];
    const cl_device_id * device = devices;
    clGetDeviceInfo(devices[0], CL_DEVICE_NAME, sizeof(device_name), device_name, NULL);
    printf("Device name: %s\n", device_name);

    return 0;
}
