
#include "lte_phy.h"

#include "CL/opencl.h"
#include "opencl/clutil.h"

#define PROGRAM_FILE "ratematcher.ocl"
#define RM_KERNEL_FUNC "ratematcher"
#define RDM_KERNEL_FUNC "ratedematcher"

static int InterColumnPattern[32] = {0,16,8,24,4,20,12,28,
									 2,18,10,26,6,22,14,30,
									 1,17,9,25,5,21,13,29,3,
									 19,11,27,7,23,15,31};

template <typename T>
static void SubblockInterleaving(int SeqLen, T *pInpMtr, T *pOutMtr)
{
	int D;
	int K_pi;
	int Rate;
	int R_sb, C_sb;
	int NumDummy;
	T DummyValue;
		
	int OutIdx;

	D = SeqLen;
	Rate = 3;
	C_sb = 32;
	
	R_sb = (D + (C_sb - 1)) / C_sb;
	
	K_pi = R_sb * C_sb;
	NumDummy = K_pi - D;
	DummyValue = (T)1000000;

	T pInterMatrix[((BLOCK_SIZE + 31) / 32) * 32];

	for (int StrIdx = 0; StrIdx < (Rate - 1); StrIdx++)
	{
		//////////////// write into matrix //////////////////
		for (int r = 0; r < R_sb; r++)
		{
			for (int c = 0; c < C_sb; c++)
			{
				int k = r * C_sb + c;
				
				if (k < NumDummy)
				{
					pInterMatrix[r * C_sb + c] = DummyValue;
				}
				else
				{
					pInterMatrix[r * C_sb + c] = pInpMtr[StrIdx + (k - NumDummy) * Rate];
				}
			}
		}
		
		OutIdx = 0;
		for (int c = 0; c < C_sb; c++)
		{
			int col = InterColumnPattern[c];
			for (int r = 0; r < R_sb; r++)
			{
				int k = col * R_sb + r;
				
				T v = pInterMatrix[r * C_sb + col];
				if (v != DummyValue)
				{
					pOutMtr[StrIdx + OutIdx * Rate] = v;
					OutIdx++;
				}  
			}
		}  
	}
	
//////////////////// Interleaving for i=2 ///////////////////////

	int Pi[((BLOCK_SIZE + 31) / 32) * 32];
	T pInterSeq[((BLOCK_SIZE + 31) / 32) * 32];
	
	for (int k = 0;k < NumDummy; k++)
	{
		pInterSeq[k] = DummyValue;
	}
	for (int k = NumDummy; k < K_pi; k++)
	{
		pInterSeq[k] = pInpMtr[(Rate - 1) + (k - NumDummy) * Rate];
	}

	for (int k = 0; k < K_pi; k++)
	{
		int idxP = k / R_sb;
		int idx = (InterColumnPattern[idxP] + (C_sb * (k % R_sb)) + 1) % K_pi;

		Pi[k] = idx;
	}

	OutIdx=0;
	for (int k = 0; k < K_pi; k++)
	{
		T v = pInterSeq[Pi[k]];
		if (v != DummyValue)
		{
			pOutMtr[(Rate - 1) + OutIdx * Rate] = v;
			OutIdx++;
		}
	}
}


template<typename T>
static void SubblockDeInterleaving(int SeqLen, T pInpMtr[], T pOutMtr[])
{
	int D;
	int K_pi;
	int Rate;
	int R_sb, C_sb;
	int NumDummy;
	T DummyValue;
	
	int InIdx;
	int OutIdx;

	D = SeqLen;
	Rate = 3;
	C_sb = 32;
	
	R_sb = (D + (C_sb - 1)) / C_sb;
	
	K_pi = R_sb * C_sb;
	NumDummy = K_pi - D;
	DummyValue = (T)1000000;
	
//////////////////// DeInterleaving for i=0,1 ///////////////////////
	T pInterMatrix[((BLOCK_SIZE + 31) / 32) * 32];
	
	for (int StrIdx = 0; StrIdx < (Rate - 1); StrIdx++)
	{
		InIdx=0;
		for (int r = 0;r < R_sb; r++)
		{
			for (int c = 0; c < C_sb; c++)
			{
				int k = r * C_sb + c;
				
				if (k < NumDummy)
				{
					pInterMatrix[r * C_sb + c] = DummyValue;
				}
				else
				{
					pInterMatrix[r * C_sb + c] = (T)0;
				}
			}
		}

		for (int c = 0; c < C_sb; c++)
		{
			int col = InterColumnPattern[c];
			for (int r = 0; r < R_sb; r++)
			{
				int k = col * R_sb + r;
				T v = pInterMatrix[r * C_sb + col];
				if (v == DummyValue)
				{}
				else
				{
					pInterMatrix[r * C_sb + col] = pInpMtr[StrIdx * D + InIdx];
					InIdx++;
				}  
			}
		}  

		OutIdx=0;
		for (int r = 0; r < R_sb; r++)
		{
			for(int c = 0; c < C_sb; c++)
			{
				T v = pInterMatrix[r * C_sb + c];
				if (v == DummyValue)
				{}
				else
				{
					pOutMtr[StrIdx * D + OutIdx] = pInterMatrix[r * C_sb + c];
					OutIdx++;
				}
			}
		}
	}

//////////////////// DeInterleaving for i=2 ///////////////////////
	int Pi[((BLOCK_SIZE + 31) / 32) * 32];
	T pInterSeq[((BLOCK_SIZE + 31) / 32) * 32];
	
	for (int k = 0; k < NumDummy; k++)
		pInterSeq[k] = DummyValue;
//////////////// Pi & DePi//////////////////
	for(int k=0;k<K_pi;k++)
	{
		int idxP = k / R_sb;
		int idx = (InterColumnPattern[idxP] + (C_sb * (k % R_sb)) + 1) % K_pi;
		Pi[k]=idx;
	}
/////////////// DeInterleaving ////////////////////
	InIdx=0;
	for(int k=0;k<K_pi;k++)
	{
		T v = pInterSeq[Pi[k]];
		if (v == DummyValue)
		{}
		else
		{
			pInterSeq[Pi[k]] = pInpMtr[(Rate - 1) * D + InIdx];
			InIdx++;
		}
	}
	OutIdx=0;
	for (int k = NumDummy; k < K_pi; k++)
	{
		pOutMtr[(Rate - 1) * D + OutIdx] = pInterSeq[k];
		OutIdx++;
	}
}


void TxRateMatching(LTE_PHY_PARAMS *lte_phy_params, int *piSeq, int *pcSeq)
{	
	int in_buf_sz;
	int out_buf_sz;
	int n_blocks;
	int rm_blk_sz;
	int rm_data_length;
	int rm_last_blk_len;
	int out_block_offset;
	int n_extra_bits;
	int cur_blk_len;

//	int pInMatrix[RATE * (BLOCK_SIZE + 4)];
//	int pOutMatrix[RATE * (BLOCK_SIZE + 4)];

	int i, j, r;

	int InverseColumnPattern[32];
	
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_int _err;

	cl_kernel kernel;
	cl_mem piSeq_buffer, pcSeq_buffer;
	cl_mem InterColumnPattern_buffer, InverseColumnPattern_buffer;

	size_t global_size, local_size;

	for (i = 0; i < 32; i++)
	{
		InverseColumnPattern[InterColumnPattern[i]] = i;
	}

	platform = device_query();
	device = create_device(&platform);
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &_err);
	program = build_program(&context, &device, PROGRAM_FILE);
	kernel = clCreateKernel(program, RM_KERNEL_FUNC, &_err);
	queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &_err);

	in_buf_sz = lte_phy_params->rm_in_buf_sz;
	out_buf_sz = lte_phy_params->rm_out_buf_sz;
	rm_blk_sz = BLOCK_SIZE + 4;

	//printf("%d\n",CL_DEVICE_MAX_WORK_GROUP_SIZE);
	rm_data_length = (in_buf_sz / RATE);

	n_blocks = (rm_data_length + (rm_blk_sz - 1)) / rm_blk_sz;
	printf("n_blocks:%d\n",n_blocks);
	if (rm_data_length % rm_blk_sz)
	{
		rm_last_blk_len = (rm_data_length % rm_blk_sz);
	}
	else
	{
		rm_last_blk_len = rm_blk_sz;
	}

	/*
	out_block_offset = 0;
	for (i = 0; i < n_blocks; i++)
	{
		cur_blk_len = (i != (n_blocks - 1)) ? rm_blk_sz : rm_last_blk_len;
			
		//	SubblockInterleaving(cur_blk_len, pInMatrix, pOutMatrix);
		SubblockInterleaving(cur_blk_len, piSeq + out_block_offset, pcSeq + out_block_offset);

		out_block_offset += RATE * cur_blk_len;
	}
	*/
	/*
	 * Use OpenCL kernel
	 */

	/* Create buffers*/
	piSeq_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, in_buf_sz * sizeof(int), NULL, &_err);
	pcSeq_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, in_buf_sz * sizeof(int), NULL, &_err);
	InterColumnPattern_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, 32 * sizeof(int), NULL, &_err);
	InverseColumnPattern_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, 32 * sizeof(int), NULL, &_err);

	/* Set kernel arguments */
	_err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &piSeq_buffer);
	_err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &pcSeq_buffer);
	_err |= clSetKernelArg(kernel, 2, sizeof(int), &in_buf_sz);
	_err |= clSetKernelArg(kernel, 3, sizeof(int), &rm_blk_sz);
	_err |= clSetKernelArg(kernel, 4, sizeof(int), &rm_last_blk_len);
	_err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &InterColumnPattern_buffer);
	_err |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &InverseColumnPattern_buffer);
	_err |= clSetKernelArg(kernel, 7, sizeof(int), &rm_data_length);
	_err |= clSetKernelArg(kernel, 8, sizeof(int), &n_blocks);
	_err |= clSetKernelArg(kernel, 9, sizeof(int) * ((rm_blk_sz+31)/32) * 32, NULL);
	if(_err < 0) {printf("err set args:%d\n",_err);exit(1);}

	/* Kernel */
	_err = clEnqueueWriteBuffer(queue, piSeq_buffer, CL_TRUE, 0, in_buf_sz * sizeof(int), piSeq, 0, NULL, NULL);
	_err = clEnqueueWriteBuffer(queue, InterColumnPattern_buffer, CL_TRUE, 0, 32 * sizeof(int), InterColumnPattern, 0, NULL, NULL);
	_err = clEnqueueWriteBuffer(queue, InverseColumnPattern_buffer, CL_TRUE, 0, 32 * sizeof(int), InverseColumnPattern, 0, NULL, NULL);
	if(_err < 0) {printf("err write buffer:%d\n",_err);exit(1);}

	local_size = 64;
	printf("local_size:%d\n",local_size);
	int groups = (rm_blk_sz + (local_size -1))/local_size;
	
	//global_size = ((rm_data_length + (local_size-1))/local_size)*local_size;
	global_size = n_blocks * groups * local_size;

	printf("global_size:%d\n",global_size);

	double elapsed_time = 0.0;
	cl_event prof_event;

	_err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, /*NULL*/&prof_event);
	if(_err < 0) {printf("err in kernel:%d\n",_err);exit(1);}

	cl_ulong ev_start_time = (cl_ulong)0;
	cl_ulong ev_end_time = (cl_ulong)0;
	clFinish(queue);

	_err = clWaitForEvents(1, &prof_event);
	_err |= clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ev_start_time, NULL);
	_err |= clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ev_end_time, NULL);

	elapsed_time = elapsed_time + (double)(ev_end_time - ev_start_time) / 1000000.0;

	printf("Elapsed time of kernel is: %lfms\n", elapsed_time);

	_err = clEnqueueReadBuffer(queue, pcSeq_buffer, CL_TRUE, 0, in_buf_sz * sizeof(int), pcSeq, 0, NULL, NULL);

	n_extra_bits = out_buf_sz - in_buf_sz;
	for (i = 0; i < n_extra_bits; i++)
	{
		pcSeq[in_buf_sz + i] = 0;
	}

	clReleaseMemObject(piSeq_buffer);
	clReleaseMemObject(pcSeq_buffer);
	clReleaseMemObject(InterColumnPattern_buffer);
	clReleaseMemObject(InverseColumnPattern_buffer);
	clReleaseKernel(kernel);
   	clReleaseCommandQueue(queue);
   	clReleaseProgram(program);
   	clReleaseContext(context);
}

void RxRateMatching(LTE_PHY_PARAMS *lte_phy_params, float *pLLRin, float *pLLRout, int *pHD)
{
	int n_blocks;
	int rm_blk_sz;
	int rm_data_length;
	int rm_last_blk_len;
	int out_block_offset;
	int cur_blk_len;
	int out_buf_sz;
	
	float pInMatrix[RATE * (BLOCK_SIZE + 4)];
	float pOutMatrix[RATE * (BLOCK_SIZE + 4)];

	int i, j, r;

	out_buf_sz = lte_phy_params->rdm_out_buf_sz;
	rm_blk_sz = BLOCK_SIZE + 4;
	rm_data_length = (out_buf_sz / RATE);

	n_blocks = (rm_data_length + (rm_blk_sz - 1)) / rm_blk_sz;
	if (rm_data_length % rm_blk_sz)
	{
		rm_last_blk_len = (rm_data_length % rm_blk_sz);
	}
	else
	{
		rm_last_blk_len = rm_blk_sz;
	}

	out_block_offset = 0;
	for (i = 0; i < n_blocks; i++)
	{
		cur_blk_len = (i != (n_blocks - 1)) ? rm_blk_sz : rm_last_blk_len;
		
		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{
				pInMatrix[r * cur_blk_len + j] = pLLRin[out_block_offset + RATE * j + r];
			}
		}  

		SubblockDeInterleaving(cur_blk_len, pInMatrix, pOutMatrix);

		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{
				pLLRin[out_block_offset + RATE * j + r] = pOutMatrix[r * cur_blk_len + j];
			}
		}

		out_block_offset += RATE * cur_blk_len;
	}

	for (i = 0; i < out_buf_sz; i++)
	{
		if (pLLRin[i] < 0)
		{
			pHD[i] = 0;
		}
		else
		{
			pHD[i] = 1;
		}
	}

	for (i = 0; i < out_buf_sz; i++)
	{
		pLLRout[i] = pLLRin[i];
	}
}

