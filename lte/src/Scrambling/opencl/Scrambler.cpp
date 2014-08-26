
#include "Scrambler.h"
#include "CL/opencl.h"
#include "opencl/cldef.h"
#include "opencl/clutil.h"

#define SCR_PROGRAM_FILE "scrambler.ocl"
#define SCR_KERNEL_FUNC "scrambler"
#define DESCR_KERNEL_FUNC "descrambler"

void Scrambling(LTE_PHY_PARAMS *lte_phy_params, int *pInpSeq, int *pOutSeq)
{
	cl_ulong local_mem_size;
	cl_mem input_buffer, scramb_buffer, output_buffer;
	cl_int _err;

	size_t global_size, local_size;

	int n;
	int scramb_seq_int[N_SCRAMB_IN_MAX];

	int i;

	n = lte_phy_params->scramb_in_buf_sz;

	cl_params_init(SCR_PROGRAM_FILE, SCR_KERNEL_FUNC);

	/* Create buffers*/
	input_buffer = clCreateBuffer(g_context, CL_MEM_READ_ONLY, n * sizeof(int), NULL, &_err);
	scramb_buffer = clCreateBuffer(g_context, CL_MEM_READ_ONLY, n * sizeof(int), NULL, &_err);
	output_buffer = clCreateBuffer(g_context, CL_MEM_WRITE_ONLY, n * sizeof(int), NULL, &_err);

	/* Set kernel arguments */
	_err = clSetKernelArg(g_kernel, 0, sizeof(cl_mem), &input_buffer);
	_err |= clSetKernelArg(g_kernel, 1, sizeof(cl_mem), &scramb_buffer);
	_err |= clSetKernelArg(g_kernel, 2, sizeof(cl_mem), &output_buffer);
	_err |= clSetKernelArg(g_kernel, 3, sizeof(int), &n);

	// Generate integer scrambling sequence
	GenScrambInt(scramb_seq_int, n);

	_err = clEnqueueWriteBuffer(g_queue, input_buffer, CL_TRUE, 0, n * sizeof(int), pInpSeq, 0, NULL, NULL);
	_err = clEnqueueWriteBuffer(g_queue, scramb_buffer, CL_TRUE, 0, n * sizeof(int), scramb_seq_int, 0, NULL, NULL);

	global_size = n;
	_err = clEnqueueNDRangeKernel(g_queue, g_kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);

	_err = clEnqueueReadBuffer(g_queue, output_buffer, CL_TRUE, 0, n * sizeof(int), pOutSeq, 0, NULL, NULL);

	cl_params_release();
	clReleaseMemObject(input_buffer);
	clReleaseMemObject(scramb_buffer);
	clReleaseMemObject(output_buffer);
}

void Descrambling(LTE_PHY_PARAMS *lte_phy_params, float *pInpSeq, float *pOutSeq)
{
	cl_ulong local_mem_size;
	cl_mem input_buffer, scramb_buffer, output_buffer;
	cl_int _err;

	size_t global_size, local_size;


	int n;
	int scramb_seq_int[N_SCRAMB_IN_MAX];

	int i;

	n = lte_phy_params->scramb_in_buf_sz;

	cl_params_init(SCR_PROGRAM_FILE, DESCR_KERNEL_FUNC);

	/* Create buffers*/
	input_buffer = clCreateBuffer(g_context, CL_MEM_READ_ONLY, n * sizeof(float), NULL, &_err);
	scramb_buffer = clCreateBuffer(g_context, CL_MEM_READ_ONLY, n * sizeof(int), NULL, &_err);
	output_buffer = clCreateBuffer(g_context, CL_MEM_WRITE_ONLY, n * sizeof(float), NULL, &_err);

	/* Set kernel arguments */
	_err = clSetKernelArg(g_kernel, 0, sizeof(cl_mem), &input_buffer);
	_err |= clSetKernelArg(g_kernel, 1, sizeof(cl_mem), &scramb_buffer);
	_err |= clSetKernelArg(g_kernel, 2, sizeof(cl_mem), &output_buffer);
	_err |= clSetKernelArg(g_kernel, 3, sizeof(int), &n);

	// Generate integer scrambling sequence
	GenScrambInt(scramb_seq_int, n);

	/*
	 * kernel
	for (i = 0; i < n_inp; i++)
	{
		pOutSeq[i] = (pInpSeq[i] * (scramb_seq_int[i] * (-2.0) + 1.0));
	}
	*/
	_err = clEnqueueWriteBuffer(g_queue, input_buffer, CL_TRUE, 0, n * sizeof(float), pInpSeq, 0, NULL, NULL);
	_err = clEnqueueWriteBuffer(g_queue, scramb_buffer, CL_TRUE, 0, n * sizeof(int), scramb_seq_int, 0, NULL, NULL);

	global_size = n;
	_err = clEnqueueNDRangeKernel(g_queue, g_kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);

	_err = clEnqueueReadBuffer(g_queue, output_buffer, CL_TRUE, 0, n * sizeof(float), pOutSeq, 0, NULL, NULL);

	cl_params_release();
	clReleaseMemObject(input_buffer);
	clReleaseMemObject(scramb_buffer);
	clReleaseMemObject(output_buffer);
}

void GenScrambInt(int *pScrambInt, int n)
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
		pScrambInt[i] = 3; // What is this? Any use?
		pScrambInt[i] = (px1[i + N_c] + px2[i + N_c]) % 2;
	}
}

