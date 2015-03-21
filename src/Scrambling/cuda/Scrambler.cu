
#include <stdio.h>
#include "Scrambler.h"


void GenScrambInt(int *pScrambInt, int n)
{
	int i;
	int N_c = 1600;
	
	int n_init[31] = { 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0};

	/////////////////////Generate ScrambSeq///////////////////////
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
	/////////////////////END Generate ScrambSeq///////////////////////
}

__global__ void scramb_kernel(int *pInpSeq, int *scramb_seq_int, int *pOutSeq, int n_inp)
{
	int i = blockDim.x * blockIdx.x + threadIdx.x;

	if (i < n_inp)
	{
		pOutSeq[i] = (pInpSeq[i] + scramb_seq_int[i]) % 2;
	}
}

void Scrambling(LTE_PHY_PARAMS *lte_phy_params, int *pInpSeq, int *pOutSeq)
{
	int n_inp;
//	int scramb_seq_int[N_SCRAMB_IN_MAX];
	int *scramb_seq_int;
	int *d_pInpSeq, *d_pOutSeq, *d_scramb_seq_int;

	int bdimx, gdimx;
//	int i;

	n_inp = lte_phy_params->scramb_in_buf_sz;
	bdimx = 32;
	gdimx = (n_inp + (bdimx - 1)) / bdimx;

	scramb_seq_int = (int *)malloc(n_inp * sizeof(int));

	cudaMalloc((void **)&d_pInpSeq, n_inp * sizeof(int));
	cudaMalloc((void **)&d_pOutSeq, n_inp * sizeof(int));
	cudaMalloc((void **)&d_scramb_seq_int, n_inp * sizeof(int));
	
	GenScrambInt(scramb_seq_int, n_inp);

	cudaMemcpy(d_pInpSeq, pInpSeq, n_inp * sizeof(int), cudaMemcpyHostToDevice);
	cudaMemcpy(d_scramb_seq_int, scramb_seq_int, n_inp * sizeof(int), cudaMemcpyHostToDevice);

	cudaEvent_t start, stop;
	cudaEventCreate(&start);
	cudaEventCreate(&stop);
	cudaEventRecord(start, 0);

	scramb_kernel<<<gdimx, bdimx>>>(d_pInpSeq, d_scramb_seq_int, d_pOutSeq, n_inp);

	cudaEventRecord(stop, 0);
	cudaEventSynchronize(stop);
	float time;
	cudaEventElapsedTime(&time, start, stop);
	cudaEventDestroy(start);
	cudaEventDestroy(stop);
	printf("Elapsed time: %.3fms", time);

	//////////////////////// scrambling kernel ////////////////////////////
	/*
	for (i = 0; i < n_inp; i++)
	{
		pOutSeq[i] = (pInpSeq[i] + scramb_seq_int[i]) % 2;
	}
	*/
	////////////////////////END scrambling kernel ////////////////////////////

	cudaMemcpy(pOutSeq, d_pOutSeq, n_inp * sizeof(int), cudaMemcpyDeviceToHost);

	free(scramb_seq_int);

	cudaFree(d_pInpSeq);
	cudaFree(d_pOutSeq);
	cudaFree(d_scramb_seq_int);
}


void Descrambling(LTE_PHY_PARAMS *lte_phy_params, float *pInpSeq, float *pOutSeq)
{
	int n_inp;
	float scramb_seq_float[N_SCRAMB_IN_MAX];
	int scramb_seq_int[N_SCRAMB_IN_MAX];

	int i;

	n_inp = lte_phy_params->scramb_in_buf_sz;
	// Generate integer scrambling sequence
	GenScrambInt(scramb_seq_int, n_inp);

	/*
	for (i = 0; i < n_inp; i++)
	{
		if (1 == scramb_seq_int[i])
		{
			scramb_seq_float[i] = -1.0;
		}
		else
		{
			scramb_seq_float[i] = +1.0;
		}
	}
	for (i = 0; i < n_inp; i++)
		pOutSeq[i] = pInpSeq[i] * scramb_seq_float[i];
	*/


	for (i = 0; i < n_inp; i++)
	{
		pOutSeq[i] = (pInpSeq[i] * (scramb_seq_int[i] * (-2.0) + 1.0));
	}
}
