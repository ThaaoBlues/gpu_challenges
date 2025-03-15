__kernel void XOR(__global unsigned int *A, __global unsigned int *B, __global unsigned int *C) {
    int id = get_global_id(0);
    C[id] = A[id] ^ B[id];
}