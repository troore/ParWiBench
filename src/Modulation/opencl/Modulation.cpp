
#define PROGRAM_FILE "modulation.ocl"
#define MOD_KERNEL_FUNC "modulating"
#define DEMOD_KERNEL_FUNC "demodulating"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <CL/cl.h>

#include "Modulation.h"
#include "opencl/clutil.h"


typedef float (*p_a)[2];

static const float INF = 1.0e9;

float BPSK_table[2][2];
float QPSK_table[4][2];
float QAM16_table[16][2];
float QAM64_table[64][2];

static void init_mod_tables()
{
	int i, j;
	
	// BPSK
	BPSK_table[0][0] = +1.0; BPSK_table[0][1] = +1.0;
	BPSK_table[1][0] = -1.0; BPSK_table[1][1] = -1.0;
	
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
			BPSK_table[i][j] /= sqrt(2.0);
		}
	}

	// QPSK
	QPSK_table[0][0] = +1.0; QPSK_table[0][1] = +1.0;
	QPSK_table[1][0] = +1.0; QPSK_table[1][1] = -1.0;
	QPSK_table[2][0] = -1.0; QPSK_table[2][1] = +1.0;
	QPSK_table[3][0] = -1.0; QPSK_table[3][1] = -1.0;
	
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 2; j++)
		{
			QPSK_table[i][j] /= sqrt(2.0);
		}
	}

	// QAM16
	QAM16_table[0][0] = +1.0; QAM16_table[0][1] = +1.0;
	QAM16_table[1][0] = +1.0; QAM16_table[1][1] = +3.0;
	QAM16_table[2][0] = +3.0; QAM16_table[2][1] = +1.0;
	QAM16_table[3][0] = +3.0; QAM16_table[3][1] = +3.0;
	QAM16_table[4][0] = +1.0; QAM16_table[4][1] = -1.0;
	QAM16_table[5][0] = +1.0; QAM16_table[5][1] = -3.0;
	QAM16_table[6][0] = +3.0; QAM16_table[6][1] = -1.0;
	QAM16_table[7][0] = +3.0; QAM16_table[7][1] = -3.0;
	QAM16_table[8][0] = -1.0; QAM16_table[8][1] = +1.0;
	QAM16_table[9][0] = -1.0; QAM16_table[9][1] = +3.0;
	QAM16_table[10][0] = -3.0; QAM16_table[10][1] = +1.0;
	QAM16_table[11][0] = -3.0; QAM16_table[11][1] = +3.0;
	QAM16_table[12][0] = -1.0; QAM16_table[12][1] = -1.0;
	QAM16_table[13][0] = -1.0; QAM16_table[13][1] = -3.0;
	QAM16_table[14][0] = -3.0; QAM16_table[14][1] = -1.0;
	QAM16_table[15][0] = -3.0; QAM16_table[15][1] = -3.0;
	
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 2; j++)
		{
			QAM16_table[i][j] /= sqrt(10.0);
		}
	}

	// QAM64
	QAM64_table[0][0] = +3.0; QAM64_table[0][1] = +3.0;
	QAM64_table[1][0] = +3.0; QAM64_table[1][1] = +1.0;
	QAM64_table[2][0] = +1.0; QAM64_table[2][1] = +3.0;
	QAM64_table[3][0] = +1.0; QAM64_table[3][1] = +1.0;
	QAM64_table[4][0] = +3.0; QAM64_table[4][1] = +5.0;
	QAM64_table[5][0] = +3.0; QAM64_table[5][1] = +7.0;
	QAM64_table[6][0] = +1.0; QAM64_table[6][1] = +5.0;
	QAM64_table[7][0] = +1.0; QAM64_table[7][1] = +7.0;

	QAM64_table[8][0] = +5.0; QAM64_table[8][1] = +3.0;
	QAM64_table[9][0] = +5.0; QAM64_table[9][1] = +1.0;
	QAM64_table[10][0] = +7.0; QAM64_table[10][1] = +3.0;
	QAM64_table[11][0] = +7.0; QAM64_table[11][1] = +1.0;
	QAM64_table[12][0] = +5.0; QAM64_table[12][1] = +5.0;
	QAM64_table[13][0] = +5.0; QAM64_table[13][1] = +7.0;
	QAM64_table[14][0] = +7.0; QAM64_table[14][1] = +5.0;
	QAM64_table[15][0] = +7.0; QAM64_table[15][1] = +7.0;

	QAM64_table[16][0] = +3.0; QAM64_table[16][1] = -3.0;
	QAM64_table[17][0] = +3.0; QAM64_table[17][1] = -1.0;
	QAM64_table[18][0] = +1.0; QAM64_table[18][1] = -3.0;
	QAM64_table[19][0] = +1.0; QAM64_table[19][1] = -1.0;
	QAM64_table[20][0] = +3.0; QAM64_table[20][1] = -5.0;
	QAM64_table[21][0] = +3.0; QAM64_table[21][1] = -7.0;
	QAM64_table[22][0] = +1.0; QAM64_table[22][1] = -5.0;
	QAM64_table[23][0] = +1.0; QAM64_table[23][1] = -7.0;

	QAM64_table[24][0] = +5.0; QAM64_table[24][1] = -3.0;
	QAM64_table[25][0] = +5.0; QAM64_table[25][1] = -1.0;
	QAM64_table[26][0] = +7.0; QAM64_table[26][1] = -3.0;
	QAM64_table[27][0] = +7.0; QAM64_table[27][1] = -1.0;
	QAM64_table[28][0] = +5.0; QAM64_table[28][1] = -5.0;
	QAM64_table[29][0] = +5.0; QAM64_table[29][1] = -7.0;
	QAM64_table[30][0] = +7.0; QAM64_table[30][1] = -5.0;
	QAM64_table[31][0] = +7.0; QAM64_table[31][1] = -7.0;

	QAM64_table[32][0] = -3.0; QAM64_table[32][1] = +3.0;
	QAM64_table[33][0] = -3.0; QAM64_table[33][1] = +1.0;
	QAM64_table[34][0] = -1.0; QAM64_table[34][1] = +3.0;
	QAM64_table[35][0] = -1.0; QAM64_table[35][1] = +1.0;
	QAM64_table[36][0] = -3.0; QAM64_table[36][1] = +5.0;
	QAM64_table[37][0] = -3.0; QAM64_table[37][1] = +7.0;
	QAM64_table[38][0] = -1.0; QAM64_table[38][1] = +5.0;
	QAM64_table[39][0] = -1.0; QAM64_table[39][1] = +7.0;

	QAM64_table[40][0] = -5.0; QAM64_table[40][1] = +3.0;
	QAM64_table[41][0] = -5.0; QAM64_table[41][1] = +1.0;
	QAM64_table[42][0] = -7.0; QAM64_table[42][1] = +3.0;
	QAM64_table[43][0] = -7.0; QAM64_table[43][1] = +1.0;
	QAM64_table[44][0] = -5.0; QAM64_table[44][1] = +5.0;
	QAM64_table[45][0] = -5.0; QAM64_table[45][1] = +7.0;
	QAM64_table[46][0] = -7.0; QAM64_table[46][1] = +5.0;
	QAM64_table[47][0] = -7.0; QAM64_table[47][1] = +7.0;

	QAM64_table[48][0] = -3.0; QAM64_table[48][1] = -3.0;
	QAM64_table[49][0] = -3.0; QAM64_table[49][1] = -1.0;
	QAM64_table[50][0] = -1.0; QAM64_table[50][1] = -3.0;
	QAM64_table[51][0] = -1.0; QAM64_table[51][1] = -1.0;
	QAM64_table[52][0] = -3.0; QAM64_table[52][1] = -5.0;
	QAM64_table[53][0] = -3.0; QAM64_table[53][1] = -7.0;
	QAM64_table[54][0] = -1.0; QAM64_table[54][1] = -5.0;
	QAM64_table[55][0] = -1.0; QAM64_table[55][1] = -7.0;

	QAM64_table[56][0] = -5.0; QAM64_table[56][1] = -3.0;
	QAM64_table[57][0] = -5.0; QAM64_table[57][1] = -1.0;
	QAM64_table[58][0] = -7.0; QAM64_table[58][1] = -3.0;
	QAM64_table[59][0] = -7.0; QAM64_table[59][1] = -1.0;
	QAM64_table[60][0] = -5.0; QAM64_table[60][1] = -5.0;
	QAM64_table[61][0] = -5.0; QAM64_table[61][1] = -7.0;
	QAM64_table[62][0] = -7.0; QAM64_table[62][1] = -5.0;
	QAM64_table[63][0] = -7.0; QAM64_table[63][1] = -7.0;
	
	for (i = 0; i < 64; i++)
	{
		for (j = 0; j < 2; j++)
		{
			QAM64_table[i][j] /= sqrt(42.0);
		}
	}
}

/*
 * decimal to bit sequence
 * i: decimal
 * n: width of bit sequence
 * bvec: vector to contain the sequence
 */
static void dec2bits(int i, int n, int *bvec)
{
	int j;
	
	for (j = 0; j < n; j++)
	{
		bvec[n - j - 1] = (i & 1);
		i = (i >> 1);
	}
}

static void set_mod_params(p_a *pp_table, int *bits_per_samp, int *mod_table_len, int mod_type)
{
	init_mod_tables();
	
	switch(mod_type)
	{
	case LTE_BPSK:
		*pp_table = &BPSK_table[0];
		*bits_per_samp = BPSK_BITS_PER_SAMP;
		*mod_table_len = BPSK_TABLE_LEN;
		break;
	case LTE_QPSK:
		*pp_table = &QPSK_table[0];
		*bits_per_samp = QPSK_BITS_PER_SAMP;
		*mod_table_len = QPSK_TABLE_LEN;
		break;
	case LTE_QAM16:
		*pp_table = &QAM16_table[0];
		*bits_per_samp = QAM16_BITS_PER_SAMP;
		*mod_table_len = QAM16_TABLE_LEN;
		break;
	case LTE_QAM64:
		*pp_table = &QAM64_table[0];
		*bits_per_samp = QAM64_BITS_PER_SAMP;
		*mod_table_len = QAM64_TABLE_LEN;
		break;
	default:
		std::cout << "Invalid modulation type!" << std::endl;
		// TODO: Exception processing
		break;
	}
}

void Modulating(LTE_PHY_PARAMS *lte_phy_params, int *pBitsSeq, float *pModedSeq, int mod_type)
{

	float (*p_table)[2];
	int bits_per_samp;
	int mod_table_len;
	int n_samp, b, idx;
	int in_buf_sz, out_buf_sz;
	
//	float t[MAX_MOD_TABLE_LEN*2];
	float *p_mod_table;

	cl_platform_id platform;	
	cl_device_id device;
   	cl_context context;
  	cl_program program;
   	cl_kernel kernel;
   	cl_command_queue queue;
   	cl_int _err;

	cl_mem pBitsSeq_buffer, pModedSeq_buffer, p_mod_table_buffer;
	
  	size_t global_size, local_size;

	in_buf_sz = lte_phy_params->mod_in_buf_sz;
	out_buf_sz = lte_phy_params->mod_out_buf_sz;
	
	set_mod_params(&p_table, &bits_per_samp, &mod_table_len, mod_type);
	
	p_mod_table = (float *)malloc(2 * mod_table_len * sizeof(float));
	for (int i = 0; i < mod_table_len; i++)
	{
		p_mod_table[i] = p_table[i][0];
		p_mod_table[i + mod_table_len] = p_table[i][1];
	}
	//for(int i = 0; i < mod_table_len; i++)
	//	printf("%f ",t[2*i]);


//	initcl_context(&device,&context,&program,SCR_PROGRAM_FILE);

//	initcl_kernel(&device, &context, &queue, &kernel,&program,SCR_KERNEL_FUNC);

	platform = device_query();
	device = create_device(&platform);
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &_err);
	program = build_program(&context, &device, PROGRAM_FILE);
	kernel = clCreateKernel(program, MOD_KERNEL_FUNC, &_err);
	queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &_err);

	/* Create buffers*/

	pBitsSeq_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, in_buf_sz * sizeof(int), NULL, &_err);
	if(_err < 0) { perror("Couldn't create pBitsSeq_buffer");  exit(1);   };
	pModedSeq_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 2 * out_buf_sz * sizeof(float), NULL, &_err);
	if(_err < 0) { perror("Couldn't create pModedSeq_buffer");  exit(1);   };
	p_mod_table_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, mod_table_len * 2 * sizeof(float), NULL, &_err);	
	if(_err < 0) { perror("Couldn't create p_mod_table_buffer");  exit(1);   };

	//initcl_kernel(device, context, queue, kernel,program,SCR_KERNEL_FUNC);

	/* Set kernel arguments */
	_err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &pBitsSeq_buffer);
	_err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &pModedSeq_buffer);
	_err |= clSetKernelArg(kernel, 2, sizeof(int), &bits_per_samp);
	_err |= clSetKernelArg(kernel, 3, sizeof(int), &out_buf_sz);
	_err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &p_mod_table_buffer);
	_err |= clSetKernelArg(kernel, 5, sizeof(int),&mod_table_len);
	if(_err < 0) { perror("Couldn't create a kernel argument");  exit(1);   }
	
	global_size = out_buf_sz;

	_err = clEnqueueWriteBuffer(queue, pBitsSeq_buffer, CL_TRUE, 0, in_buf_sz * sizeof(int), pBitsSeq, 0, NULL, NULL);
	_err |= clEnqueueWriteBuffer(queue, p_mod_table_buffer, CL_TRUE, 0, mod_table_len * 2 * sizeof(float), p_mod_table, 0, NULL, NULL);

	_err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);

	_err = clEnqueueReadBuffer(queue, pModedSeq_buffer, CL_TRUE, 0, 2 * out_buf_sz * sizeof(float), pModedSeq, 0, NULL, NULL);

	clReleaseMemObject(pBitsSeq_buffer);
	clReleaseMemObject(pModedSeq_buffer);
	clReleaseMemObject(p_mod_table_buffer);
   	clReleaseKernel(kernel);
   	clReleaseCommandQueue(queue);
   	clReleaseProgram(program);
   	clReleaseContext(context);

	free(p_mod_table);
}

void Demodulating(LTE_PHY_PARAMS *lte_phy_params, float *pDecSeq, float *pLLR, int mod_type, float awgnSigma)
{
	float No = 2.0 * (pow(awgnSigma, 2.0));
	float (*p_table)[2];
	int bits_per_samp;
	int mod_table_len;
//	float t[MAX_MOD_TABLE_LEN*2];
	float *p_mod_table;

	int i, j, k;

	int in_buf_sz, out_buf_sz;
	
	int *p_idx_table;

	cl_platform_id platform;
	cl_device_id device;
   	cl_context context;
  	cl_program program;
   	cl_kernel kernel;
   	cl_command_queue queue;
   	cl_int _err;

	cl_mem pDecSeq_buffer, pLLR_buffer, p_mod_table_buffer, p_idx_table_buffer;
	
  	size_t global_size, local_size;
	
	set_mod_params(&p_table, &bits_per_samp, &mod_table_len, mod_type);

	p_mod_table = (float *)malloc(2 * mod_table_len * sizeof(float));
	for (int i = 0; i < mod_table_len; i++)
	{
		p_mod_table[i] = p_table[i][0];
		p_mod_table[i + mod_table_len] = p_table[i][1];
	}

	in_buf_sz = lte_phy_params->demod_in_buf_sz;
	out_buf_sz = lte_phy_params->demod_out_buf_sz;

	p_idx_table = (int *)malloc(mod_table_len * bits_per_samp * sizeof(int));
	
	//for(i = 0; i < in_buf_sz; i++)
	//	printf("%f ",pDecSeq[i+in_buf_sz]);
	for (i = 0; i < mod_table_len; i++)
	{
		for (j = 0; j < bits_per_samp; j++)
		{
			p_idx_table[i * bits_per_samp + j] = 0;
		}
	}
	for (i = 0; i < mod_table_len; i++)
	{ 
 
		int idx_val = i;
		int b = bits_per_samp - 1;
		
		while (idx_val)
		{
			p_idx_table[i * bits_per_samp + b] = idx_val % 2;
			idx_val /= 2;
			b--;
		}
	}

//	initcl_context(&device,&context,&program,SCR_PROGRAM_FILE);
//	initcl_kernel(&device, &context, &queue, &kernel,&program,DESCR_KERNEL_FUNC);

	platform = device_query();
	device = create_device(&platform);
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &_err);
	program = build_program(&context, &device, PROGRAM_FILE);
	kernel = clCreateKernel(program, DEMOD_KERNEL_FUNC, &_err);
	queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &_err);
	
	/* Create buffers*/
	pDecSeq_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, in_buf_sz * 2 * sizeof(float), NULL, &_err);
//	printf("%d\n", _err);
//	printf("CL_SUCCESS=%d\n", CL_SUCCESS);
//	printf("CL_INVALID_CONTEXT=%d\n", CL_INVALID_CONTEXT);
//	printf("CL_INVALID_VALUE=%d\n", CL_INVALID_VALUE);
//	printf("CL_INVALID_BUFFER_SIZE=%d\n", CL_INVALID_BUFFER_SIZE);
//	printf("CL_INVALID_HOST_PTR=%d\n", CL_INVALID_HOST_PTR);
//	printf("CL_MEM_OBJECT_ALLOCATION_FAILURE=%d\n", CL_MEM_OBJECT_ALLOCATION_FAILURE);
//	printf("CL_OUT_OF_HOST_MEMORY=%d\n", CL_OUT_OF_HOST_MEMORY);
	
	if(_err < 0) { perror("Couldn't create pDecSeq_buffer");  exit(1);   };
	pLLR_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, out_buf_sz * sizeof(float), NULL, &_err);
	if(_err < 0) { perror("Couldn't create pLLR_buffer");  exit(1);   };	
	p_mod_table_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, mod_table_len * 2 * sizeof(float), NULL, &_err);
	if(_err < 0) { perror("Couldn't create ptable_buffer");  exit(1);   };	
	p_idx_table_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, mod_table_len * bits_per_samp * sizeof(int), NULL, &_err);	
	if(_err < 0) { perror("Couldn't create idx_table_buffer");  exit(1);   };	
	//initcl_kernel(device, context, queue, kernel,program,DESCR_KERNEL_FUNC);
	
	/* Set kernel arguments */
	_err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &pDecSeq_buffer);
	_err |= clSetKernelArg(kernel, 1, sizeof(int), &bits_per_samp);
	_err |= clSetKernelArg(kernel, 2, sizeof(int), &in_buf_sz);
	_err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &p_mod_table_buffer);
	_err |= clSetKernelArg(kernel, 4, sizeof(int), &mod_table_len);
	_err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &p_idx_table_buffer);
	_err |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &pLLR_buffer);
	_err |= clSetKernelArg(kernel, 7, sizeof(float), &No);
	if(_err < 0) { perror("Couldn't create a kernel argument");  exit(1);   }
	
	global_size = in_buf_sz;
	_err = clEnqueueWriteBuffer(queue, pDecSeq_buffer, CL_TRUE, 0, in_buf_sz * 2 * sizeof(float), pDecSeq, 0, NULL, NULL);
	_err |= clEnqueueWriteBuffer(queue, p_mod_table_buffer, CL_TRUE, 0, mod_table_len * 2 * sizeof(float), p_mod_table, 0, NULL, NULL);
	_err |= clEnqueueWriteBuffer(queue, p_idx_table_buffer, CL_TRUE, 0, mod_table_len * bits_per_samp * sizeof(int), p_idx_table, 0, NULL, NULL);

	_err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);

	_err = clEnqueueReadBuffer(queue, pLLR_buffer, CL_TRUE, 0, out_buf_sz * sizeof(float), pLLR, 0, NULL, NULL);

	//for(int i = 0; i < n; i++)
	//	printf("%f ",pLLR[i]);
	clReleaseMemObject(pDecSeq_buffer);
	clReleaseMemObject(p_mod_table_buffer);
	clReleaseMemObject(p_idx_table_buffer);
	clReleaseMemObject(pLLR_buffer);
   	clReleaseKernel(kernel);
   	clReleaseCommandQueue(queue);
   	clReleaseProgram(program);
   	clReleaseContext(context);

	free(p_mod_table);
	free(p_idx_table);
}

