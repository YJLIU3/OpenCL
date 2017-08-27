__kernel void vecAdd(__global int* A,
        __global int* B,
        __global int* C)
{
    //获取当前工作项所在位置（线程索引号）
    int idx = get_global_id(0);
    C[idx] = A[idx] + B[idx];
}