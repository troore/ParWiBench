
#include "Scrambler.h"
#include "CL/opencl.h"
#include "opencl/clutil.h"
#include "timer/meas.h"

#define PROGRAM_FILE "scrambler.ocl"
#define SCR_KERNEL_FUNC "scrambler"
#define DESCR_KERNEL_FUNC "descrambler"

inline static void GenScrambInt(int *pScrambInt, int n)
{
	int i;
	int N_c = 1600;
	
	int n_init[31] = { 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0};

	int px1[N_SCRAMB_IN_MAX + 1600];
	int px2[N_SCRAMB_IN_MAX + 1600];

	for (i = 0; i < 31; i++)
	{
		px1[i] = 0;
		px2[i] = n_init[i];
	}
	px1[0] = 1;
	
	for (i = 0; i < n + N_c - 31; i++)
	{
		px1[i + 31] =(px1[i + 3] + px1[i]) % 2;
		px2[i + 31] = (px2[i + 3] + px2[i + 2] + px2[i + 1] + px2[i]) % 2;
	}
	for (i = 0; i < n; i++)
	{
	//	pScrambInt[i] = 3; // What is this? Any use?
		pScrambInt[i] = (px1[i + N_c] + px2[i + N_c]) % 2;
	}
}

void Scrambling(LTE_PHY_PARAMS *lte_phy_params, int *pInpSeq, int *pOutSeq)
{
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_int _err;

	cl_kernel kernel;

	cl_ulong local_mem_size;
	cl_mem input_buffer, scramb_buffer, output_buffer;

	size_t global_size, local_size;

	int n;
	int scramb_seq_int[N_SCRAMB_IN_MAX];

	
//	double tstart, tstop, ttime;
//	tstart = dtime();
	// Generate integer scrambling sequence
	GenScrambInt(scramb_seq_int, n);
//	tstop = dtime();

//	ttime = tstop - tstart;

//	printf("Elapsed time of GenScrambInt is %lfms\n", ttime);

//	int i;

	platform = device_query();
	device = create_device(&platform);
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &_err);
	program = build_program(&context, &device, PROGRAM_FILE);
	kernel = clCreateKernel(program, SCR_KERNEL_FUNC, &_err);
	queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &_err);

	n = lte_phy_params->scramb_in_buf_sz;

	/* Create buffers*/
	input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, n * sizeof(int), NULL, &_err);
	scramb_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, n * sizeof(int), NULL, &_err);
	output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, n * sizeof(int), NULL, &_err);

	/* Set kernel arguments */
	_err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer);
	_err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &scramb_buffer);
	_err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &output_buffer);
	_err |= clSetKernelArg(kernel, 3, sizeof(int), &n);
	int n_iters = 1;
	_err |= clSetKernelArg(kernel, 4, sizeof(int), &n_iters);


	global_size = n;
	local_size = 32;

	double elapsed_time = 0.0;

	for (int k = 0; k < 100000; k++) {
	cl_event prof_event;

		
	_err = clEnqueueWriteBuffer(queue, input_buffer, CL_TRUE, 0, n * sizeof(int), pInpSeq, 0, NULL, NULL);
	_err = clEnqueueWriteBuffer(queue, scramb_buffer, CL_TRUE, 0, n * sizeof(int), scramb_seq_int, 0, NULL, NULL);

	_err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, /*NULL*/&local_size, 0, NULL, /*NULL*/&prof_event);

//		printf("%d\n", _err);
//		printf("%d\n", CL_INVALID_WORK_GROUP_SIZE);
//		printf("%d\n", CL_DEVICE_MAX_WORK_GROUP_SIZE);

	cl_ulong ev_start_time = (cl_ulong)0;
	cl_ulong ev_end_time = (cl_ulong)0;
	clFinish(queue);

	_err = clWaitForEvents(1, &prof_event);
	_err |= clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ev_start_time, NULL);
	_err |= clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ev_end_time, NULL);

//	printf("%d\n", ev_start_time);
//	printf("%d\n", ev_end_time);
	
	_err = clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, 0, n * sizeof(int), pOutSeq, 0, NULL, NULL);

	elapsed_time = elapsed_time + (double)(ev_end_time - ev_start_time) / 1000000.0;
	}

	printf("Elapsed time of kernel is: %lfms\n", elapsed_time);

	//	_err = clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, 0, n * sizeof(int), pOutSeq, 0, NULL, &prof_event);
	//	_err = clWaitForEvents(1, &prof_event);

	clReleaseMemObject(input_buffer);
	clReleaseMemObject(scramb_buffer);
	clReleaseMemObject(output_buffer);
}

void Descrambling(LTE_PHY_PARAMS *lte_phy_params, float *pInpSeq, float *pOutSeq)
{
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_int _err;

	cl_kernel kernel;

	cl_ulong local_mem_size;
	cl_mem input_buffer, scramb_buffer, output_buffer;

	size_t global_size, local_size;


	int n;
	int scramb_seq_int[N_SCRAMB_IN_MAX];

	int i;

	platform = device_query();
	device = create_device(&platform);
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &_err);
	program = build_program(&context, &device, PROGRAM_FILE);
	kernel = clCreateKernel(program, DESCR_KERNEL_FUNC, &_err);
	queue = clCreateCommandQueue(context, device, 0, &_err);

	n = lte_phy_params->scramb_in_buf_sz;

//	cl_params_init(SCR_PROGRAM_FILE, DESCR_KERNEL_FUNC);

	/* Create buffers*/
	input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, n * sizeof(float), NULL, &_err);
	scramb_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, n * sizeof(int), NULL, &_err);
	output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, n * sizeof(float), NULL, &_err);

	/* Set kernel arguments */
	_err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_buffer);
	_err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &scramb_buffer);
	_err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &output_buffer);
	_err |= clSetKernelArg(kernel, 3, sizeof(int), &n);

	// Generate integer scrambling sequence
	GenScrambInt(scramb_seq_int, n);

	/*
	 * kernel
	for (i = 0; i < n_inp; i++)
	{
		pOutSeq[i] = (pInpSeq[i] * (scramb_seq_int[i] * (-2.0) + 1.0));
	}
	*/
	_err = clEnqueueWriteBuffer(queue, input_buffer, CL_TRUE, 0, n * sizeof(float), pInpSeq, 0, NULL, NULL);
	_err = clEnqueueWriteBuffer(queue, scramb_buffer, CL_TRUE, 0, n * sizeof(int), scramb_seq_int, 0, NULL, NULL);

	global_size = n;
	_err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);

	_err = clEnqueueReadBuffer(queue, output_buffer, CL_TRUE, 0, n * sizeof(float), pOutSeq, 0, NULL, NULL);

//	cl_params_release();
	clReleaseMemObject(input_buffer);
	clReleaseMemObject(scramb_buffer);
	clReleaseMemObject(output_buffer);
}

