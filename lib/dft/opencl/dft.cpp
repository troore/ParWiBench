#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "CL/opencl.h"
#include "opencl/clutil.h"
#include "timer/meas.h"

#define PROGRAM_FILE "/home/xuechao/ParWiBench/lib/dft/opencl/dft.ocl"
#define DFT_UNCOALESED_KERNEL "dft_uncoalesed_kernel"
#define DFT_COALESED_KERNEL "dft_coalesed_kernel"

extern int num_threads;

//#define PI	3.14159265358979323846264338327950288

/*
  Discrete Fourier Transform
*/
void dft(int n, float *a, float *y, int direction)
{
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_int _err;

	cl_kernel uncoalesed_kernel, coalesed_kernel;

	cl_ulong local_mem_size;
	cl_mem a_buffer, y_buffer;

	size_t global_size, local_size;
	
	platform = device_query();
	device = create_device(&platform);
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &_err);
	program = build_program(&context, &device, PROGRAM_FILE);
	uncoalesed_kernel = clCreateKernel(program, DFT_UNCOALESED_KERNEL, &_err);
	coalesed_kernel = clCreateKernel(program, DFT_COALESED_KERNEL, &_err);
	queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &_err);

	/* Create buffers*/
	a_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, 2 * n * sizeof(float), NULL, &_err);
	y_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 2 * n * sizeof(float), NULL, &_err);

	
	global_size = num_threads;
	local_size = 128;
	
	/* Set kernel arguments */
	_err = clSetKernelArg(uncoalesed_kernel, 0, sizeof(int), &n);
	_err |= clSetKernelArg(uncoalesed_kernel, 1, sizeof(cl_mem), &a_buffer);
	_err |= clSetKernelArg(uncoalesed_kernel, 2, sizeof(cl_mem), &y_buffer);
	_err |= clSetKernelArg(uncoalesed_kernel, 3, sizeof(int), &direction);
	int n_iters = 1000;
	_err |= clSetKernelArg(uncoalesed_kernel, 4, sizeof(int), &n_iters);
	
	_err = clSetKernelArg(coalesed_kernel, 0, sizeof(int), &n);
	_err |= clSetKernelArg(coalesed_kernel, 1, sizeof(cl_mem), &a_buffer);
	_err |= clSetKernelArg(coalesed_kernel, 2, sizeof(cl_mem), &y_buffer);
	_err |= clSetKernelArg(coalesed_kernel, 3, sizeof(int), &direction);
	_err |= clSetKernelArg(coalesed_kernel, 4, 2 * local_size * sizeof(float), NULL);
//	int n_iters = 1;
	_err |= clSetKernelArg(coalesed_kernel, 5, sizeof(int), &n_iters);

	_err = clEnqueueWriteBuffer(queue, a_buffer, CL_TRUE, 0, 2 * n * sizeof(float), a, 0, NULL, NULL);

	double elapsed_time = 0.0;
	cl_event prof_event;
	
	_err = clEnqueueNDRangeKernel(queue, uncoalesed_kernel, 1, NULL, &global_size, &local_size, 0, NULL, /*NULL*/&prof_event);
//	_err = clEnqueueNDRangeKernel(queue, coalesed_kernel, 1, NULL, &global_size, &local_size, 0, NULL, /*NULL*/&prof_event);

	cl_ulong ev_start_time = (cl_ulong)0;
	cl_ulong ev_end_time = (cl_ulong)0;
	clFinish(queue);

	_err = clWaitForEvents(1, &prof_event);
	_err |= clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ev_start_time, NULL);
	_err |= clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ev_end_time, NULL);

	elapsed_time = elapsed_time + (double)(ev_end_time - ev_start_time) / 1000000.0;

	printf("Elapsed time of dft kernel is: %lfms\n", elapsed_time);

	_err = clEnqueueReadBuffer(queue, y_buffer, CL_TRUE, 0, 2 * n * sizeof(float), y, 0, NULL, NULL);

	clReleaseMemObject(a_buffer);
	clReleaseMemObject(y_buffer);
}

