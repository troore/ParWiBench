#define PROGRAM_FILE "/home/pacuser12/ParWiBench/lib/fft/opencl/fft.ocl"
#define KERNEL_FUNC "fft_radix2_kernel"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include "CL/cl.h"

#include "clutil.h"
#include "fft.h"
#include "meas.h"

float v[N][2], v1[N][2], vout[N][2], v1out[N][2];

/* Print a vector of complexes as ordered pairs. */
static void print_vector(
		const char *title,
		float (*x)[2],
		int n)
{
	int i;
	printf("%s (dim=%d):", title, n);
	for(i=0; i<n; i++ ) printf(" %f,%f ", x[i][0],x[i][1]);
	putchar('\n');
}


void fft(int n, float (*a)[2], float (*y)[2], int direction)
{
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;

	cl_kernel kernel;

	cl_int _err;

	cl_ulong local_mem_size;
	cl_mem input_buffer, output_buffer;

	size_t global_size, local_size;

	/* Data and buffer */
	int num_points, points_per_group, p;
	float *input, *output;

	int i;

	platform = device_query();
	device = create_device(&platform);
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &_err);
	program = build_program(&context, &device, PROGRAM_FILE);
	kernel = clCreateKernel(program, KERNEL_FUNC, &_err);
	queue = clCreateCommandQueue(context, device, 0, &_err);

	input = (float *)malloc(n * 2 * sizeof(float));
	output = (float *)malloc(n * 2 * sizeof(float));

	/* Initialize data */
	for (i = 0; i < n; i++)
	{
		input[2 * i] = a[i][0];
		input[2 * i + 1] = a[i][1];
	//	printf("(%f\t%f)\n", a[i][0], a[i][1]);
	}

//	cl_params_init(PROGRAM_FILE, FFT_FUNC);

	/* Create buffer */
	input_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE, 2 * n * sizeof(float), NULL, &_err);
	if (_err < 0)
	{
		perror("Couldn't create input buffer");
		exit(1);
	};
	output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 2 * n * sizeof(float), NULL, &_err);
	if (_err < 0)
	{
		perror("Couldn't create output buffer");
		exit(1);
	};

	/* Determine maximum work-group size */
	_err = clGetKernelWorkGroupInfo(kernel, device, 
			CL_KERNEL_WORK_GROUP_SIZE, sizeof(local_size), &local_size, NULL);
	if (_err < 0) 
	{
		perror("Couldn't find the maximum work-group size");
		exit(1);   
	};
//	printf("Maximum work-group size is: %d\n", local_size);
	local_size = (int)pow(2, trunc(log2(local_size)));

	/* Determine local memory size */
	_err = clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, 
			sizeof(local_mem_size), &local_mem_size, NULL);
	if (_err < 0) 
	{
		perror("Couldn't determine the local memory size");
		exit(1);   
	};

	/* Initialize kernel arguments */
	num_points = n;
	/*
	points_per_group = local_mem_size / ( 2 * sizeof(float));
	if (points_per_group > num_points)
		points_per_group = num_points;
	*/

	global_size = num_points / 2;
	local_size = global_size;

	/* Set kernel arguments */
	_err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer);
	_err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_buffer);
	_err |= clSetKernelArg(kernel, 3, sizeof(int), &direction);
	if (_err < 0)
	{
		printf("Couldn't set a kernel argument");
		exit(1);
	};

	double elapsed_time = 0.0;
	cl_event prof_event;
	/* Enqueue fft kernel */
	for (p = 1; p <= (num_points / 2); p <<= 1)
	{
		_err = clSetKernelArg(kernel, 2, sizeof(int), &p);
		_err = clEnqueueWriteBuffer(queue, input_buffer, CL_TRUE, 0, 2 * n * sizeof(float), input, 0, NULL, NULL);
		
	//	_err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL); 
		_err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, &prof_event); 
		if (_err < 0)
		{
			perror("Invalid kernel");
			exit(1);   
		};
		cl_ulong ev_start_time = (cl_ulong)0;
		cl_ulong ev_end_time = (cl_ulong)0;
		clFinish(queue);
		_err = clWaitForEvents(1, &prof_event);
		_err |= clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ev_start_time, NULL);
		_err |= clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ev_end_time, NULL);

		elapsed_time = elapsed_time + (double)(ev_end_time - ev_start_time) / 1000000.0;

		_err = clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, 0, 2 * n * sizeof(float), output, 0, NULL, NULL);
		for (i = 0; i < 2 * num_points; i++)
		{
			input[i] = output[i];
		}

		/*
		printf("p=%d\n", p);
		for (i = 0; i < num_points; i++)
		{
			printf("%d: (%f,%f)\n", i, input[2 * i], input[2 * i + 1]);
		}
		*/
	}

	printf("Elapsed time is: %lfms\n", elapsed_time);


	/* Write back results */
	for (i = 0; i < n; i++)
	{
		y[i][0] = output[2 * i];
		y[i][1] = output[2 * i + 1];
	}

	/* Deallocate resources */
//	cl_params_release();
	clReleaseMemObject(input_buffer);
	clReleaseMemObject(output_buffer);

	free(input);
	free(output);
}

/*
int main(void)
{
//	float v[N][2], v1[N][2], vout[N][2], v1out[N][2];
	int k;

//	FILE *fptr_real, *fptr_imag;

//	fptr_real = fopen("../lte/src/OFDM/testsuite/ModulationInputReal", "r");
//	fptr_imag = fopen("../lte/src/OFDM/testsuite/ModulationInputImag", "r");

	// Fill v[] with a function of known FFT:
	for(k=0; k<N; k++)
	{
		v[k][0] = 0.125*cos(2*PI*k/(float)N);
		v[k][1] = 0.125*sin(2*PI*k/(float)N);
//		v1[k][0] =  0.3*cos(2*PI*k/(float)N);
//		v1[k][1] = -0.3*sin(2*PI*k/(float)N);

//		fscanf(fptr_real, "%f", &(v[k][0]));
//		fscanf(fptr_imag, "%f", &(v[k][1]));
	}

	print_vector("Orig", v, N);
	TIME_MEASURE_WRAPPER_SCALAR(fft(N, v, vout, -1))
	for (k = 0; k < N; k++)
	{
		vout[k][0] /= N;
		vout[k][1] /= N;
	}
	print_vector("FFT", vout, N);
	fft(N, vout, v, 1);
	print_vector("iFFT", v, N);

//	print_vector("Orig", v1, N);
//	fft(N, v1, v1out, -1);
//	print_vector("FFT", v1out, N);
//	fft(N, v1out, v1, 1);
//	print_vector("iFFT", v1, N);

	return 0;
}
*/
