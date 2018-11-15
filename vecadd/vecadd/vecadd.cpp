#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <cl.h>

#pragma warning(disable: 4996)
using namespace std;

#define elements 500

bool GetFileData(const char* fname, string& str)
{
	FILE* fp = fopen(fname, "r");
	if (fp == NULL)
	{
		printf("no found file\n");
		return false;
	}

	int n = 0;
	while (feof(fp) == 0)
	{
		str += fgetc(fp);
	}

	return true;
}

int main()
{
	
	string code_file;

	if (false == GetFileData("vecadd.cl", code_file))
		return 0;

	char* buf_code = new char[code_file.size()];
	strcpy(buf_code, code_file.c_str());
	buf_code[code_file.size() - 1] = NULL;

	
	cl_device_id device;
	cl_platform_id platform_id = NULL;
	cl_context context;
	cl_command_queue cmdQueue;
	cl_mem bufferA, bufferB, bufferC;
	cl_program program;
	cl_kernel kernel = NULL;

	size_t globalWorkSize[1];
	globalWorkSize[0] = elements;

	cl_int err;


	int* buf_A = new int[elements];
	int* buf_B = new int[elements];
	int* buf_C = new int[elements];

	size_t datasize = sizeof(int) * elements;

	for (int i = 0; i < elements; i++)
	{
		buf_A[i] = i;
	}
	for (int i = 0; i < elements; i++)
	{
		buf_B[i] = elements + i;
	}

	err = clGetPlatformIDs(1, &platform_id, NULL);

	if (err != CL_SUCCESS)
	{
		cout << "clGetPlatformIDs error" << endl;
		return 0;
	}


	clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device, NULL);

	context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);

	cmdQueue = clCreateCommandQueue(context, device, 0, NULL);

	bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, NULL);

	bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, NULL);

	bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasize, NULL, NULL);

	clEnqueueWriteBuffer(cmdQueue, bufferA, CL_FALSE, 0, datasize, buf_A, 0, NULL, NULL);

	clEnqueueWriteBuffer(cmdQueue, bufferB, CL_FALSE, 0, datasize, buf_B, 0, NULL, NULL);

	program = clCreateProgramWithSource(context, 1, (const char**)&buf_code, NULL, NULL);

	clBuildProgram(program, 1, &device, NULL, NULL, NULL);

	kernel = clCreateKernel(program, "vecAdd", NULL);

	clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
	clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
	clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);

	const size_t offset[1] = { 0 };
	clEnqueueNDRangeKernel(cmdQueue, kernel,1, offset,globalWorkSize,NULL, 0, NULL, NULL);


	clEnqueueReadBuffer(cmdQueue, bufferC, CL_TRUE, 0, datasize, buf_C, 0, NULL, NULL);

	cout << "[" << buf_A[0] << "," << buf_A[1] << "," << buf_A[2] << "," << buf_A[3] << "]+["
		<< buf_B[0] << "," << buf_B[1] << "," << buf_B[2] << "," << buf_B[3] << "]=["
		<< buf_C[0] << "," << buf_C[1] << "," << buf_C[2] << "," << buf_C[3] << "]" << endl;


	clReleaseKernel(kernel);
	clReleaseProgram(program);
	clReleaseCommandQueue(cmdQueue);
	clReleaseMemObject(bufferA);
	clReleaseMemObject(bufferB);
	clReleaseMemObject(bufferC);
	clReleaseContext(context);

	//delete buf_A;
	//delete buf_B;
	//delete buf_C;
	//delete buf_code;
	getchar();

	return 0;
}