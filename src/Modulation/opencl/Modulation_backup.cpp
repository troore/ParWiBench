#define _CRT_SECURE_NO_WARNINGS

#define SCR_PROGRAM_FILE "modulation.cl"
#define SCR_KERNEL_FUNC "modulation"
#define DESCR_KERNEL_FUNC "demodulation"

#include "Modulation.h"
//#include "api.h"
//#include "CL/opencl.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif



static const float INF = 1.0e9;

float BPSK_table[2][2]; float QPSK_table[4][2];
float QAM16_table[16][2];
float QAM64_table[64][2];

void init_mod_tables()
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
void dec2bits(int i, int n, int *bvec)
{
	int j;
	
	for (j = 0; j < n; j++)
	{
		bvec[n - j - 1] = (i & 1);
		i = (i >> 1);
	}
}

void set_mod_params(/*float (*mod_table)[2]*/ p_a *pp_table, int *bits_per_samp, int *mod_table_len, int mod_type)
{
	init_mod_tables();

//	int (*p)[2];

//	p = (int (*)[2])0x1;
	
	switch(mod_type)
	{
	case LTE_BPSK:
	//	tt = &BPSK_table[0][0];
		*pp_table = &BPSK_table[0];
		*bits_per_samp = BPSK_BITS_PER_SAMP;
		*mod_table_len = BPSK_TABLE_LEN;
		break;
	case LTE_QPSK:
	//	tt = &QPSK_table[0][0];
		*pp_table = &QPSK_table[0];
		*bits_per_samp = QPSK_BITS_PER_SAMP;
		*mod_table_len = QPSK_TABLE_LEN;
		break;
	case LTE_QAM16:
	//	tt = &QAM16_table[0][0];
		*pp_table = &QAM16_table[0];
//		*pp_table = (float (*)[2])0x1;
		*bits_per_samp = QAM16_BITS_PER_SAMP;
		*mod_table_len = QAM16_TABLE_LEN;
		break;
	case LTE_QAM64:
	//	tt = &QAM64_table[0][0];
		*pp_table = &QAM64_table[0];
		*bits_per_samp = QAM16_BITS_PER_SAMP;	//177,why 16,not 64
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
	int in_buf_sz;

	in_buf_sz = lte_phy_params->mod_in_buf_sz;
	set_mod_params(&p_table, &bits_per_samp, &mod_table_len, mod_type);

	int n = in_buf_sz / bits_per_samp;
	cl_device_id device;
   	cl_context context;
  	cl_program program;
   	cl_kernel kernel;
   	cl_command_queue queue;
   	cl_int i, err;
  	size_t local_size, global_size;

	initcl_context(&device,&context,&program,SCR_PROGRAM_FILE);

	initcl_kernel(&device, &context, &queue, &kernel,&program,SCR_KERNEL_FUNC);

	/* Create buffers*/
	cl_mem pBitsSeq_buffer,pModedSeq_buffer,ptable_buffer;
	pBitsSeq_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, in_buf_sz* sizeof(int), pBitsSeq, &err);	
	pModedSeq_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 2*n*sizeof(float), pModedSeq, &err);
	ptable_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(mod_table_len*2)*sizeof(float),p_table, &err);	
	if(err < 0) { perror("Couldn't create a buffer");  exit(1);   };

	//initcl_kernel(device, context, queue, kernel,program,SCR_KERNEL_FUNC);

	/* Set kernel arguments */
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &pBitsSeq_buffer);
	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &pModedSeq_buffer);
	err |= clSetKernelArg(kernel, 2, sizeof(int), &bits_per_samp);
	err |= clSetKernelArg(kernel, 3, sizeof(int), &n);
	err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &ptable_buffer);
	if(err < 0) { perror("Couldn't create a kernel argument");  exit(1);   }
	
	global_size = 1;

	err = clEnqueueWriteBuffer(queue, pBitsSeq_buffer, CL_TRUE, 0, in_buf_sz * sizeof(int), pBitsSeq, 0, NULL, NULL);
	err |= clEnqueueWriteBuffer(queue, ptable_buffer, CL_TRUE, 0, sizeof(mod_table_len*2)*sizeof(float), p_table, 0, NULL, NULL);

	err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);

	err = clEnqueueReadBuffer(queue, pModedSeq_buffer, CL_TRUE, 0, 2 * n * sizeof(float), pModedSeq, 0, NULL, NULL);

	clReleaseMemObject(pBitsSeq_buffer);
	clReleaseMemObject(pModedSeq_buffer);
	clReleaseMemObject(ptable_buffer);
   	clReleaseKernel(kernel);
   	clReleaseCommandQueue(queue);
   	clReleaseProgram(program);
   	clReleaseContext(context);
}


/*
 * Euclidean distance of two complex values: a+bi and c+di
 */
/*float eudist(float a, float b, float c, float d)
{
	return sqrt((a - c) * (a - c) + (b - d) * (b - d));
}

void Demodulating(LTE_PHY_PARAMS *lte_phy_params,  std::complex<float>*pDecSeq, int *pHD, int mod_type)
{
	float dist, mindist;
	int closest;

	float (*p_table)[2];
	int bits_per_samp;
	int mod_table_len;
		
	int bitmap[MAX_MOD_TABLE_LEN * MAX_MOD_BITS_PER_SAMP];
	int bvec[MAX_MOD_BITS_PER_SAMP];
	
	int in_buf_sz;

	int i, j;

	in_buf_sz = lte_phy_params->demod_in_buf_sz;

	set_mod_params(&p_table, &bits_per_samp, &mod_table_len, mod_type);

	// set bitmap 
	for (i = 0; i < mod_table_len; i++)
	{
		dec2bits(i, bits_per_samp, bvec);
		for (j = 0; j < bits_per_samp; j++)
		{
			bitmap[i * bits_per_samp + j] = bvec[j];
		}
	}
	// set bitmap over... 


	// Start demodulation 
	for (i = 0; i < in_buf_sz; i++)
	{
		mindist = eudist(p_table[0][0], p_table[0][1], std::real(pDecSeq[i]), std::imag(pDecSeq[i]));
		
		closest = 0;
		for (j = 1; j < mod_table_len; j++)
		{
			//	dist = std::abs(symbols(j) - signal(i));
			dist = eudist(p_table[j][0], p_table[j][1], std::real(pDecSeq[i]), std::imag(pDecSeq[i]));
			if (dist < mindist)
			{
				mindist = dist;
				closest = j;
			}
		}
		for (j = 0; j < bits_per_samp; j++)
		{
			pHD[i * bits_per_samp + j] = bitmap[closest * bits_per_samp + j];
		}
	}

	//End demodulation
}*/

void Demodulating(LTE_PHY_PARAMS *lte_phy_params, float *pDecSeq, float *pLLR, int mod_type, float awgnSigma)
{

	float No = 2.0 * (pow(awgnSigma, 2.0));
	float (*p_table)[2];
	int bits_per_samp;
	int mod_table_len;

	int idx_table[MAX_MOD_TABLE_LEN][MAX_MOD_BITS_PER_SAMP];

	int i, j, k;

	int in_buf_sz = lte_phy_params->demod_in_buf_sz;
	int n = in_buf_sz  * bits_per_samp;
	
//	float *t;
	set_mod_params(&p_table, &bits_per_samp, &mod_table_len, mod_type);
	
	for (i = 0; i < mod_table_len; i++)
	{
		for (j = 0; j < bits_per_samp; j++)
		{
			idx_table[i][j] = 0;
		}
	}
	for (i = 0; i < mod_table_len; i++)
	{ 
 
		int idx_val = i;
		int b = bits_per_samp - 1;
		
		while (idx_val)
		{
			idx_table[i][b] = idx_val % 2;
			idx_val /= 2;
			b--;
		}
	}

	cl_device_id device;
   	cl_context context;
  	cl_program program;
   	cl_kernel kernel;
   	cl_command_queue queue;
   	cl_int err;
  	size_t local_size, global_size;
	
	initcl_context(&device,&context,&program,SCR_PROGRAM_FILE);
	initcl_kernel(&device, &context, &queue, &kernel,&program,DESCR_KERNEL_FUNC);

	/* Create buffers*/
	cl_mem pDecSeq_buffer,pLLR_buffer,ptable_buffer,idx_table_buffer;
	pDecSeq_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, in_buf_sz* sizeof(float), NULL, &err);	
	pLLR_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, 2*n*sizeof(float), NULL, &err);
	ptable_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(mod_table_len*2) *sizeof(float), NULL, &err);	
	idx_table_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(MAX_MOD_TABLE_LEN * MAX_MOD_BITS_PER_SAMP)*sizeof(int), NULL, &err);	
	if(err < 0) { perror("Couldn't create a buffer");  exit(1);   };
	
	//initcl_kernel(device, context, queue, kernel,program,DESCR_KERNEL_FUNC);
	
	/* Set kernel arguments */
	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &pDecSeq_buffer);
	err |= clSetKernelArg(kernel, 1, sizeof(int), &bits_per_samp);
	err |= clSetKernelArg(kernel, 2, sizeof(int), &n);
	err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &ptable_buffer);
	err |= clSetKernelArg(kernel, 4, sizeof(int), &mod_table_len);
	err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &idx_table_buffer);
	err |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &pLLR_buffer);
	err |= clSetKernelArg(kernel, 7, sizeof(float), &No);
	if(err < 0) { perror("Couldn't create a kernel argument");  exit(1);   }
	
	global_size = in_buf_sz;

	err = clEnqueueWriteBuffer(queue, pDecSeq_buffer, CL_TRUE, 0, in_buf_sz * sizeof(float), pDecSeq, 0, NULL, NULL);
	err |= clEnqueueWriteBuffer(queue, ptable_buffer, CL_TRUE, 0, sizeof(mod_table_len*2)*sizeof(float), p_table, 0, NULL, NULL);
	err |= clEnqueueWriteBuffer(queue, idx_table_buffer, CL_TRUE, 0, sizeof(MAX_MOD_TABLE_LEN * MAX_MOD_BITS_PER_SAMP)*sizeof(int), idx_table, 0, NULL, NULL);

	err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, NULL);

	err = clEnqueueReadBuffer(queue, pLLR_buffer, CL_TRUE, 0, 2 * n * sizeof(float), pLLR, 0, NULL, NULL);

	clReleaseMemObject(pDecSeq_buffer);
	clReleaseMemObject(ptable_buffer);
	clReleaseMemObject(idx_table_buffer);
	clReleaseMemObject(pLLR_buffer);
   	clReleaseKernel(kernel);
   	clReleaseCommandQueue(queue);
   	clReleaseProgram(program);
   	clReleaseContext(context);	
}

