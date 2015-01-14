#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "CL/opencl.h"
#include "opencl/clutil.h"
#include "timer/meas.h"

#define PROGRAM_FILE "/home/xcwei/ParWiBench/lib/dft/opencl/dft.ocl"
#define KERNEL_FUNC "dft_kernel"


#define PI	3.14159265358979323846264338327950288

#define N	(1 << 11)
float v[N * 2], v1[N * 2], vout[N * 2], v1out[N * 2];


/* Print a vector of complexes as ordered pairs. */
/*
static void
print_vector(
	const char *title,
	float *x,
	int n)
{
	int i;
	printf("%s (dim=%d):", title, n);
	for(i=0; i<n; i++ ) printf(" %f,%f ", x[i], x[i + n]);
	putchar('\n');
}
*/

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

	cl_kernel kernel;

	cl_ulong local_mem_size;
	cl_mem a_buffer, y_buffer;

	size_t global_size, local_size;
	
	platform = device_query();
	device = create_device(&platform);
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &_err);
	program = build_program(&context, &device, PROGRAM_FILE);
	kernel = clCreateKernel(program, KERNEL_FUNC, &_err);
	queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &_err);

	/* Create buffers*/
	a_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, 2 * n * sizeof(float), NULL, &_err);
	y_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 2 * n * sizeof(float), NULL, &_err);

	/* Set kernel arguments */
	_err = clSetKernelArg(kernel, 0, sizeof(int), &n);
	_err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &a_buffer);
	_err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &y_buffer);
	_err |= clSetKernelArg(kernel, 3, sizeof(int), &direction);
	int n_iters = 1000;
	_err |= clSetKernelArg(kernel, 4, sizeof(int), &n_iters);

	_err = clEnqueueWriteBuffer(queue, a_buffer, CL_TRUE, 0, 2 * n * sizeof(float), a, 0, NULL, NULL);

	double elapsed_time = 0.0;
	cl_event prof_event;

	global_size = n;
//	local_size = 32;
	_err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, /*NULL*/&prof_event);

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


int main(int argc, char *argv[])
{
	int k;
	int n = atoi(argv[1]);
// Fill v[] with a function of known FFT:
	for (k = 0; k < n; k++)
	{
		v[k] = 0.125 * cos(2 * PI * k / (float)n);
		v[k + n] = 0.125 * sin(2 * PI * k / (float)n);
// v1[k][0] = 0.3*cos(2*PI*k/(float)N);
// v1[k][1] = -0.3*sin(2*PI*k/(float)N);
// fscanf(fptr_real, "%f", &(v[k][0]));
// fscanf(fptr_imag, "%f", &(v[k][1]));
	}
// v[0]=1;v[1]=0;
// v[2]=2;v[3]=0;
// v[4]=4;v[5]=0;
// v[6]=3;v[7]=0;
//	print_vector("Orig", v, N);

//	double tstart, tstop, ttime;
//	tstart = dtime();
	dft(n, v, vout, -1);
//	tstop = dtime();
//	ttime = tstop - tstart;
//	printf("Elapsed time of dft is %lfms\n", ttime);
	
//	for (k = 0; k < N; k++)
//	{
//		vout[k] /= N;
//		vout[k + N] /= N;
//	}
//	print_vector("iFFT", vout, N);
//	dft(N, vout, v, 1);
//	print_vector(" FFT", v, N);
	
	return 0;
}

