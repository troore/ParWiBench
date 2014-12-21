

#include <stdio.h>
#include <stdlib.h>

#include "lte_phy.h"
#include "dmrs.h"
#include "CL/opencl.h"
#include "opencl/clutil.h"
#include "meas.h"

#define PROGRAM_FILE "equalizer.ocl"
#define KERNEL_FUNC "lsfreqdomain"

static void MatrixProd(int d1, int d2, int d3, float M1[], float M2[], float oM[])
{
	int r, c, i;
	int m1_len = d1 * d2;
	int m2_len = d2 * d3;
	int om_len = d1 * d3;

	for (r = 0; r < d1; r++)
	{
		for (c = 0; c < d3; c++)
		{
			float tmp[2] = {0.0, 0.0};
			
			for (i = 0; i < d2; i++)
			{
				//	tmp += M1[r * d2 + i] * M2[i * d3 + c];
				tmp[0] += (M1[r * d2 + i] * M2[i * d3 + c] - M1[r * d2 + i + m1_len] * M2[i * d3 + c + m2_len]);
				tmp[1] += (M1[r * d2 + i] * M2[i * d3 + c + m2_len] + M1[r * d2 + i + m1_len] * M2[i * d3 + c]);
			}
		
			oM[r * d3 + c] = tmp[0];
			oM[r * d3 + c + om_len] = tmp[1];
		}
	}
}

static void MatrixProd(int d1, int d2, int d3,
					   float M1Real[], float M1Imag[],
					   float M2Real[], float M2Imag[],
					   float oMReal[], float oMImag[])
{
	int r, c, i;
	
	for (r = 0; r < d1; r++)
	{
		for (c = 0; c < d3; c++)
		{
			float tmp_real = 0.0;
			float tmp_imag = 0.0;
			
			for (i = 0; i < d2; i++)
			{
				tmp_real += (M1Real[r * d2 + i] * M2Real[i * d3 + c] - M1Imag[r * d2 + i] * M2Imag[i * d3 + c]);
				tmp_imag += (M1Real[r * d2 + i] * M2Imag[i * d3 + c] + M1Imag[r * d2 + i] * M2Real[i * d3 + c]);
			}
		
			oMReal[r * d3 + c] = tmp_real;
			oMImag[r * d3 + c] = tmp_imag;
		}
	}
}

static void MatrixInv(int sz, float pM[], float pInvM[])
{
	int m_len = sz * sz;
	int x_len = sz * (2 * sz);
//	float pX[LTE_PHY_N_ANT_MAX * 2 * LTE_PHY_N_ANT_MAX * 2];
	float *pX = (float *)malloc(2 * x_len * sizeof(float));


	for (int r = 0; r < sz; r++)
	{
		for (int c = 0; c < sz; c++)
		{
			//	pX[r * (2 * sz) + c] = pM[r * sz + c];
			pX[r * (2 * sz) + c] = pM[r * sz + c];
			pX[r * (2 * sz) + c + x_len] = pM[r * sz + c + m_len];
		}
		for (int c = sz; c < 2 * sz; c++)
		{
			if (c == (r + sz))
			{
				//	pX[r * (2 * sz) + c] = (std::complex<float>)1.0;
				pX[r * (2 * sz) + c] = 1.0;
				pX[r * (2 * sz) + c + x_len] = 0.0;
			}
			else
			{
				//	pX[r * (2 * sz) + c] = (std::complex<float>)0.0;
				pX[r * (2 * sz) + c] = 0.0;
				pX[r * (2 * sz) + c + x_len] = 0.0;
			}
		}
	}

//	float pCurRow[2  * LTE_PHY_N_ANT_MAX * 2];
	int ae_row_len = 2 * sz;
	float *pCurRow = (float *)malloc(2 * ae_row_len * sizeof(float));
	
	for (int r = 0; r < sz; r++)
	{
		for (int c = 0; c < (2 * sz); c++)
		{
			float A[2], B[2];
			//	pCurRow[c] = pX[r * (2 * sz) + c] / pX[r * (2 * sz) + r];
			A[0] = pX[r * (2 * sz) + c];
			A[1] = pX[r * (2 * sz) + c + x_len];
			B[0] = pX[r * (2 * sz) + r];
			B[1] = pX[r * (2 * sz) + r + x_len];
			pCurRow[c] = (A[0] * B[0] + A[1] * B[1]) / (B[0] * B[0] + B[1] * B[1]);
			pCurRow[c + ae_row_len] = (A[1] * B[0] - A[0] * B[1]) / (B[0] * B[0] + B[1] * B[1]);
		}

		for (int c = 0; c < (2 * sz); c++)
		{
			//	pX[r * (2 * sz) + c] = pCurRow[c];
			pX[r * (2 * sz) + c] = pCurRow[c];
			pX[r * (2 * sz) + c + x_len] = pCurRow[c + ae_row_len];
		}

		for (int er = r + 1; er < sz; er++)
		{
			float curC[2] = {pX[er * (2 * sz) + r], pX[er * (2 * sz) + r + x_len]};

			for (int c = 0; c < (2 * sz); c++)
			{
				//	pX[er * (2 * sz) + c] -= curC * pCurRow[c];
				pX[er * (2 * sz) + c] -= (curC[0] * pCurRow[c] - curC[1] * pCurRow[c + ae_row_len]);
				pX[er * (2 * sz) + c + x_len] -= (curC[0] * pCurRow[c + ae_row_len] + curC[1] * pCurRow[c]);
			}  
		}
	}

	for (int r = sz - 1; r >= 0; r--)
	{
		for (int c = 0; c < (2 * sz); c++)
		{
			//	pCurRow[c] = pX[r * (2 * sz) + c];
			pCurRow[c] = pX[r * (2 * sz) + c];
			pCurRow[c + ae_row_len] = pX[r * (2 * sz) + c + x_len];
		}

		for (int er = r - 1; er >= 0; er--)
		{
			float curC[2] = {pX[er * (2 * sz) + r], pX[er * (2 * sz) + r + x_len]};

			for (int c = 0; c < (2 * sz); c++)
			{
				//	pX[er * (2 * sz) + c] -= curC * pCurRow[c];
				pX[er * (2 * sz) + c] -= (curC[0] * pCurRow[c] - curC[1] * pCurRow[c + ae_row_len]);
				pX[er * (2 * sz) + c + x_len] -= (curC[0] * pCurRow[c + ae_row_len] + curC[1] * pCurRow[c]);
			}  
		}
	}

	for (int r = 0; r < sz; r++)
	{
		for (int c = 0; c < sz; c++)
		{
			int col = c + sz;

			//	pInvM[r * sz + c] = pX[r * (2 * sz) + col];
			pInvM[r * sz + c] = pX[r * (2 * sz) + col];
			pInvM[r * sz + c + m_len] = pX[r * (2 * sz) + col + x_len];
		}

	}

	free(pX);
	free(pCurRow);
}

static void MatrixInv(int sz,
					  float pMReal[], float pMImag[],
					  float pInvMReal[], float pInvMImag[])
{
//	int m_len = sz * sz;
	int x_len = sz * (2 * sz);
//	float pX[LTE_PHY_N_ANT_MAX * 2 * LTE_PHY_N_ANT_MAX * 2];
	float *pXReal = (float *)malloc(x_len * sizeof(float));
	float *pXImag = (float *)malloc(x_len * sizeof(float));


	for (int r = 0; r < sz; r++)
	{
		for (int c = 0; c < sz; c++)
		{
			//	pX[r * (2 * sz) + c] = pM[r * sz + c];
			pXReal[r * (2 * sz) + c] = pMReal[r * sz + c];
			pXImag[r * (2 * sz) + c] = pMImag[r * sz + c];
		}
		for (int c = sz; c < 2 * sz; c++)
		{
			if (c == (r + sz))
			{
				//	pX[r * (2 * sz) + c] = (std::complex<float>)1.0;
				pXReal[r * (2 * sz) + c] = 1.0;
				pXImag[r * (2 * sz) + c] = 0.0;
			}
			else
			{
				//	pX[r * (2 * sz) + c] = (std::complex<float>)0.0;
				pXReal[r * (2 * sz) + c] = 0.0;
				pXImag[r * (2 * sz) + c] = 0.0;
			}
		}
	}

//	float pCurRow[2  * LTE_PHY_N_ANT_MAX * 2];
	int ae_row_len = 2 * sz;
	float *pCurRowReal = (float *)malloc(ae_row_len * sizeof(float));
	float *pCurRowImag = (float *)malloc(ae_row_len * sizeof(float));
	
	for (int r = 0; r < sz; r++)
	{
		for (int c = 0; c < (2 * sz); c++)
		{
			float AReal, AImag, BReal, BImag;
			//	pCurRow[c] = pX[r * (2 * sz) + c] / pX[r * (2 * sz) + r];
			AReal = pXReal[r * (2 * sz) + c];
			AImag = pXImag[r * (2 * sz) + c];
			BReal = pXReal[r * (2 * sz) + r];
			BImag = pXImag[r * (2 * sz) + r];
			pCurRowReal[c] = (AReal * BReal + AImag * BImag) / (BReal * BReal + BImag * BImag);
			pCurRowImag[c] = (AImag * BReal - AReal * BImag) / (BReal * BReal + BImag * BImag);
		}

		for (int c = 0; c < (2 * sz); c++)
		{
			//	pX[r * (2 * sz) + c] = pCurRow[c];
			pXReal[r * (2 * sz) + c] = pCurRowReal[c];
			pXImag[r * (2 * sz) + c] = pCurRowImag[c];
		}

		for (int er = r + 1; er < sz; er++)
		{
			float curCReal = pXReal[er * (2 * sz) + r];
			float curCImag = pXImag[er * (2 * sz) + r];

			for (int c = 0; c < (2 * sz); c++)
			{
				//	pX[er * (2 * sz) + c] -= curC * pCurRow[c];
				pXReal[er * (2 * sz) + c] -= (curCReal * pCurRowReal[c] - curCImag * pCurRowImag[c]);
				pXImag[er * (2 * sz) + c] -= (curCReal * pCurRowImag[c] + curCImag * pCurRowReal[c]);
			}  
		}
	}

	for (int r = sz - 1; r >= 0; r--)
	{
		for (int c = 0; c < (2 * sz); c++)
		{
			//	pCurRow[c] = pX[r * (2 * sz) + c];
			pCurRowReal[c] = pXReal[r * (2 * sz) + c];
			pCurRowImag[c] = pXImag[r * (2 * sz) + c];
		}

		for (int er = r - 1; er >= 0; er--)
		{
			float curCReal = pXReal[er * (2 * sz) + r];
			float curCImag = pXImag[er * (2 * sz) + r];

			for (int c = 0; c < (2 * sz); c++)
			{
				//	pX[er * (2 * sz) + c] -= curC * pCurRow[c];
				pXReal[er * (2 * sz) + c] -= (curCReal * pCurRowReal[c] - curCImag * pCurRowImag[c]);
				pXImag[er * (2 * sz) + c] -= (curCReal * pCurRowImag[c] + curCImag * pCurRowReal[c]);
			}  
		}
	}

	for (int r = 0; r < sz; r++)
	{
		for (int c = 0; c < sz; c++)
		{
			int col = c + sz;

			//	pInvM[r * sz + c] = pX[r * (2 * sz) + col];
			pInvMReal[r * sz + c] = pXReal[r * (2 * sz) + col];
			pInvMImag[r * sz + c] = pXImag[r * (2 * sz) + col];
		}

	}

	free(pXReal);
	free(pXImag);
	free(pCurRowReal);
	free(pCurRowImag);
}

void Equalizing(LTE_PHY_PARAMS *lte_phy_params,
				float *pInpDataReal, float *pInpDataImag,
				float *pOutDataReal, float *pOutDataImag)
{
	int MDFT = lte_phy_params->N_dft_sz;
	int NumLayer = lte_phy_params->N_tx_ant;
	int NumRxAntenna = lte_phy_params->N_rx_ant;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;

	int dmrs_len = NumLayer * MDFT * 2;
	
	float *pDMRSReal = (float *)malloc(dmrs_len * sizeof(float));
	float *pDMRSImag = (float *)malloc(dmrs_len * sizeof(float));
	
	geneDMRS(pDMRSReal, pDMRSImag, NumLayer, MDFT);

	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_int _err;

	cl_kernel kernel;

	cl_mem pInpDataReal_buffer, pInpDataImag_buffer;
	cl_mem pOutDataReal_buffer, pOutDataImag_buffer;
	cl_mem pDMRSReal_buffer, pDMRSImag_buffer;

	size_t global_size, local_size;

	platform = device_query();
	device = create_device(&platform);
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &_err);
	program = build_program(&context, &device, PROGRAM_FILE);
	kernel = clCreateKernel(program, KERNEL_FUNC, &_err);
	queue = clCreateCommandQueue(context, device, /*0*/CL_QUEUE_PROFILING_ENABLE, &_err);

	/* Create buffers*/
	pInpDataReal_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, (NumRxAntenna * NumULSymbSF * MDFT) * sizeof(float), NULL, &_err);
	pInpDataImag_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, (NumRxAntenna * NumULSymbSF * MDFT) * sizeof(float), NULL, &_err);
	pOutDataReal_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, (NumRxAntenna * (NumULSymbSF - 2) * MDFT) * sizeof(float), NULL, &_err);
	pOutDataImag_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, (NumRxAntenna * (NumULSymbSF - 2) * MDFT) * sizeof(float), NULL, &_err);
	pDMRSReal_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, (2 * NumLayer * MDFT) * sizeof(float), NULL, &_err);
	pDMRSImag_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, (2 * NumLayer * MDFT) * sizeof(float), NULL, &_err);

	/* Set kernel arguments */
	_err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &pInpDataReal_buffer);
	_err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &pInpDataImag_buffer);
	_err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &pOutDataReal_buffer);
	_err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &pOutDataImag_buffer);
	_err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &pDMRSReal_buffer);
	_err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &pDMRSImag_buffer);
	_err |= clSetKernelArg(kernel, 6, sizeof(int), &MDFT);
	_err |= clSetKernelArg(kernel, 7, sizeof(int), &NumLayer);
	_err |= clSetKernelArg(kernel, 8, sizeof(int), &NumRxAntenna);
	_err |= clSetKernelArg(kernel, 9, sizeof(int), &NumULSymbSF);

	_err = clEnqueueWriteBuffer(queue, pInpDataReal_buffer, CL_TRUE, 0, (NumRxAntenna * NumULSymbSF * MDFT) * sizeof(int), pInpDataReal, 0, NULL, NULL);
	_err |= clEnqueueWriteBuffer(queue, pInpDataImag_buffer, CL_TRUE, 0, (NumRxAntenna * NumULSymbSF * MDFT) * sizeof(int), pInpDataImag, 0, NULL, NULL);
	_err |= clEnqueueWriteBuffer(queue, pDMRSReal_buffer, CL_TRUE, 0, (2 * NumLayer * MDFT) * sizeof(int), pDMRSReal, 0, NULL, NULL);
	_err |= clEnqueueWriteBuffer(queue, pDMRSImag_buffer, CL_TRUE, 0, (2 * NumLayer * MDFT) * sizeof(int), pDMRSImag, 0, NULL, NULL);

	global_size = MDFT;

	double elapsed_time = 0.0;
	cl_event prof_event;

	_err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, NULL, 0, NULL, /*NULL*/&prof_event);

	cl_ulong ev_start_time = (cl_ulong)0;
	cl_ulong ev_end_time = (cl_ulong)0;
	clFinish(queue);

	_err = clWaitForEvents(1, &prof_event);
	_err = clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ev_start_time, NULL);
	_err = clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ev_end_time, NULL);

	elapsed_time = elapsed_time + (double)(ev_end_time - ev_start_time) / 1000000.0;

	printf("Elapsed time of kernel is: %lfms\n", elapsed_time);

	_err = clEnqueueReadBuffer(queue, pOutDataReal_buffer, CL_TRUE, 0, (NumRxAntenna * (NumULSymbSF - 2) * MDFT) * sizeof(int), pOutDataReal, 0, NULL, NULL);
	_err = clEnqueueReadBuffer(queue, pOutDataImag_buffer, CL_TRUE, 0, (NumRxAntenna * (NumULSymbSF - 2) * MDFT) * sizeof(int), pOutDataImag, 0, NULL, NULL);


	free(pDMRSReal);
	free(pDMRSImag);

	clReleaseMemObject(pInpDataReal_buffer);
	clReleaseMemObject(pInpDataImag_buffer);
	clReleaseMemObject(pOutDataReal_buffer);
	clReleaseMemObject(pOutDataImag_buffer);
	clReleaseMemObject(pDMRSReal_buffer);
	clReleaseMemObject(pDMRSImag_buffer);
}
