__kernel void XOR(__global const bool *A, __global const bool *B, __global bool *C) {
    int id = get_global_id(0);
    C[id] = A[id] ^ B[id];
}