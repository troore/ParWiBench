
#define PROGRAM_FILE "resmapper.ocl"
#define RESM_KERNEL_FUNC "mapping"
#define RESDM_KERNEL_FUNC "demapping"

#include "lte_phy.h"
#include "refs/dmrs.h"
#include "util.h"

#include <CL/cl.h>
#include "opencl/clutil.h"

#include <stdlib.h>

void SubCarrierMapping(LTE_PHY_PARAMS *lte_phy_params, float *pInpDataReal, float *pInpDataImag,
					   float *pOutDataReal, float *pOutDataImag)
{
	int NumLayer = lte_phy_params->N_tx_ant;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	int SCLoc = lte_phy_params->sc_loc;
	int NIFFT = lte_phy_params->N_fft_sz;
	int MDFT = lte_phy_params->N_dft_sz;
	
	int in_buf_sz = lte_phy_params->resm_in_buf_sz;
	int out_buf_sz = lte_phy_params->resm_out_buf_sz;

	float *pDMRSReal;
	float *pDMRSImag;
	int *map_table;

	cl_platform_id platform;	
	cl_device_id device;
   	cl_context context;
  	cl_program program;
   	cl_kernel kernel;
   	cl_command_queue queue;
   	cl_int _err;

	cl_mem pInpDataReal_buffer, pInpDataImag_buffer;
	cl_mem pOutDataReal_buffer, pOutDataImag_buffer;
	cl_mem pDMRSReal_buffer, pDMRSImag_buffer;
	cl_mem map_table_buffer;

	size_t global_size, local_size;

//	pDMRSReal = (float *)malloc((2 * NumLayer * MDFT) * sizeof(float));
//	pDMRSImag = (float *)malloc((2 * NumLayer * MDFT) * sizeof(float));

//	geneDMRS(pDMRSReal, pDMRSImag, NumLayer, MDFT);
	pDMRSReal = lte_phy_params->DMRSReal;
	pDMRSImag = lte_phy_params->DMRSImag;

	map_table = (int *)malloc(NumULSymbSF * sizeof(int));

	int t1 = 0, t2 = 0;
	for (int nsym = 0; nsym < NumULSymbSF; nsym++)
	{
		if (nsym == lte_phy_params->dmrs_symb_pos[t1])
		{
			t1++;
			map_table[nsym] = -t1;
		}
		else
		{
			map_table[nsym] = t2;
			t2++;
		}
	}

	for (int nlayer = 0; nlayer < NumLayer; nlayer++)
	{
		for (int nsym = 0; nsym < NumULSymbSF; nsym++)
		{
			int SymIdx = nlayer * NumULSymbSF + nsym;

			for (int n = 0; n < NIFFT; n++)
			{
				pOutDataReal[SymIdx * NIFFT + n] = 1.0;
				pOutDataImag[SymIdx * NIFFT + n] = 0.0;
			}
		}
	}

//	printf("%f\t%f\n", pOutDataReal[0], pOutDataImag[0]);

	platform = device_query();
	device = create_device(&platform);
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &_err);
	program = build_program(&context, &device, PROGRAM_FILE);
	kernel = clCreateKernel(program, RESM_KERNEL_FUNC, &_err);
	queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &_err);

	/* Create buffers */
	pInpDataReal_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, in_buf_sz * sizeof(float), NULL, &_err);
	if (_err < 0) { printf("Couldn't create pInpDataReal_buffer"); exit(1); }
	pInpDataImag_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, in_buf_sz * sizeof(float), NULL, &_err);
	if (_err < 0) { printf("Couldn't create pInpDataImag_buffer"); exit(1); }
	pOutDataReal_buffer = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, out_buf_sz * sizeof(float), pOutDataReal, &_err);
	if (_err < 0) { printf("Couldn't create pOutDataReal_buffer"); exit(1); }
	pOutDataImag_buffer = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, out_buf_sz * sizeof(float), pOutDataImag, &_err);
	if (_err < 0) { printf("Couldn't create pOutDataImag_buffer"); exit(1); }
	pDMRSReal_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, (2 * NumLayer * MDFT) * sizeof(float), NULL, &_err);
	if (_err < 0) { printf("Couldn't create pDMRSReal_buffer"); exit(1); }
	pDMRSImag_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, (2 * NumLayer * MDFT) * sizeof(float), NULL, &_err);
	if (_err < 0) { printf("Couldn't create pDMRSImag_buffer"); exit(1); }
	map_table_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, NumULSymbSF * sizeof(int), NULL, &_err);
	if (_err < 0) { printf("Couldn't create map_table_buffer"); exit(1); }

	/* Set kernel arguments */
	_err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &pInpDataReal_buffer);
	_err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &pInpDataImag_buffer);
	_err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &pOutDataReal_buffer);
	_err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &pOutDataImag_buffer);
	_err |= clSetKernelArg(kernel, 4, sizeof(cl_mem), &pDMRSReal_buffer);
	_err |= clSetKernelArg(kernel, 5, sizeof(cl_mem), &pDMRSImag_buffer);
	_err |= clSetKernelArg(kernel, 6, sizeof(cl_mem), &map_table_buffer);
	_err |= clSetKernelArg(kernel, 7, sizeof(int), &NumLayer);
	_err |= clSetKernelArg(kernel, 8, sizeof(int), &NumULSymbSF);
	_err |= clSetKernelArg(kernel, 9, sizeof(int), &MDFT);
	_err |= clSetKernelArg(kernel, 10, sizeof(int), &NIFFT);
	_err |= clSetKernelArg(kernel, 11, sizeof(int), &SCLoc);
	int n_iters = 10000;
	_err |= clSetKernelArg(kernel, 12, sizeof(int), &n_iters);
	if (_err < 0) { perror("Couldn't create one of the kernel arguments");  exit(1); }

//	global_size = NumLayer * NumULSymbSF * MDFT;
	global_size = num_threads;
	local_size = 128;

	_err = clEnqueueWriteBuffer(queue, pInpDataReal_buffer, CL_TRUE, 0, in_buf_sz * sizeof(float), pInpDataReal, 0, NULL, NULL);
	_err |= clEnqueueWriteBuffer(queue, pInpDataImag_buffer, CL_TRUE, 0, in_buf_sz * sizeof(float), pInpDataImag, 0, NULL, NULL);
	_err |= clEnqueueWriteBuffer(queue, pDMRSReal_buffer, CL_TRUE, 0, (2 * NumLayer * MDFT) * sizeof(float), pDMRSReal, 0, NULL, NULL);
	_err |= clEnqueueWriteBuffer(queue, pDMRSImag_buffer, CL_TRUE, 0, (2 * NumLayer * MDFT) * sizeof(float), pDMRSImag, 0, NULL, NULL);
	_err |= clEnqueueWriteBuffer(queue, map_table_buffer, CL_TRUE, 0, NumULSymbSF * sizeof(int), map_table, 0, NULL, NULL);
	if (_err < 0) { perror("Couldn't enqueue one of the write buffers");  exit(1); }


	double elapsed_time = 0.0;
	cl_event prof_event;
	
	_err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, &local_size /*NULL*/, 0, NULL, /*NULL*/ &prof_event);
	
	cl_ulong ev_start_time = (cl_ulong)0;
	cl_ulong ev_end_time = (cl_ulong)0;
	clFinish(queue);

	_err = clWaitForEvents(1, &prof_event);
	_err |= clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &ev_start_time, NULL);
	_err |= clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &ev_end_time, NULL);

	elapsed_time = elapsed_time + (double)(ev_end_time - ev_start_time) / 1000000.0;

	printf("Elapsed time of kernel is: %lfms\n", elapsed_time);

	_err = clEnqueueReadBuffer(queue, pOutDataReal_buffer, CL_TRUE, 0, out_buf_sz * sizeof(float), pOutDataReal, 0, NULL, NULL);
	_err |= clEnqueueReadBuffer(queue, pOutDataImag_buffer, CL_TRUE, 0, out_buf_sz * sizeof(float), pOutDataImag, 0, NULL, NULL);
	if (_err < 0) { perror("Couldn't enqueue one of the read buffers");  exit(1); }

//	free(pDMRSReal);
//	free(pDMRSImag);
	free(map_table);

	clReleaseMemObject(pInpDataReal_buffer);
	clReleaseMemObject(pInpDataImag_buffer);
	clReleaseMemObject(pOutDataReal_buffer);
	clReleaseMemObject(pOutDataImag_buffer);
	clReleaseMemObject(pDMRSReal_buffer);
	clReleaseMemObject(pDMRSImag_buffer);
	clReleaseMemObject(map_table_buffer);
}

void SubCarrierDemapping(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, float *pOutData)
{
	int SCLoc = lte_phy_params->sc_loc;
	int NumRxAntenna = lte_phy_params->N_rx_ant;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	int NIFFT = lte_phy_params->N_fft_sz;
	int MDFT = lte_phy_params->N_dft_sz;

	int in_buf_sz = lte_phy_params->resdm_in_buf_sz;
	int out_buf_sz = lte_phy_params->resdm_out_buf_sz;

	//////////// Get DMRS /////////////
	for (int nrs = 0; nrs < 2; nrs++)
	{
		int DMRSPos = lte_phy_params->dmrs_symb_pos[nrs];
			
		for (int nrx = 0; nrx < NumRxAntenna; nrx++)
		{

			int SymIdxIn = nrx * NumULSymbSF + DMRSPos;
			int SymIdxOut = nrx * 2 + nrs;

			for (int n = 0 ; n < MDFT; n++)
			{
				pOutData[SymIdxOut * MDFT + n] = pInpData[SymIdxIn * NIFFT + (n + SCLoc)];
				pOutData[out_buf_sz + SymIdxOut * MDFT + n] = pInpData[in_buf_sz + SymIdxIn * NIFFT + (n + SCLoc)];
			}
		}
	}
		
	//////////// Get Data /////////////
	for(int nrx = 0; nrx < NumRxAntenna; nrx++)
	{
		int SymOutIdx = 0;
		
		for (int nsym = 0; nsym < NumULSymbSF; nsym++)
		{
			int SymIdxIn = NumULSymbSF * nrx + nsym;
			
			if(((nsym == lte_phy_params->dmrs_symb_pos[0]) || (nsym == lte_phy_params->dmrs_symb_pos[1])))
			{}
			else
			{
				int SymOutT = (NumULSymbSF - 2) * nrx + SymOutIdx + NumRxAntenna * 2;

				for (int n = 0; n < MDFT; n++)
				{
					pOutData[SymOutT * MDFT + n] = pInpData[SymIdxIn * NIFFT + (n + SCLoc)];
					pOutData[out_buf_sz + SymOutT * MDFT + n] = pInpData[in_buf_sz + SymIdxIn * NIFFT + (n + SCLoc)];
				}
				SymOutIdx++;
			}
		}
	}
}
