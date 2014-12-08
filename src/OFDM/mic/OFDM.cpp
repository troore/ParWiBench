
#include <cstring>
#include "OFDM.h"

#include "meas.h"

void ofmodulating(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, float *pOutData)
{
//	fftwf_plan ifftplan;

	int NumLayer = lte_phy_params->N_tx_ant;
	int NIFFT = lte_phy_params->N_fft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	int CPLen = lte_phy_params->N_samps_cp_l_0;

	int nlayer, nsym, i;
	
//	float in[LTE_PHY_FFT_SIZE_MAX][2];
//	float out[LTE_PHY_FFT_SIZE_MAX][2];
	
//	ifftplan = fftwf_plan_dft_1d(NIFFT, reinterpret_cast<fftwf_complex*>(&in[0]), reinterpret_cast<fftwf_complex*>(&out[0]), FFTW_BACKWARD, FFTW_ESTIMATE);

	static double elapsed_time = 0.0;
	double start, end;
	start = clock();
	for (nlayer = 0; nlayer < NumLayer; nlayer++)
	{
		for (nsym = 0; nsym < NumULSymbSF; nsym++)
		{
			int symb_idx = nlayer * NumULSymbSF + nsym;
			float norm = (float)sqrt((float)NIFFT)/* (float)NIFFT*/;

			//	memcpy(in, &pInpData[symb_idx * NIFFT], NIFFT * sizeof(std::complex<float>));

			//	fftwf_execute_dft(ifftplan, reinterpret_cast<fftwf_complex*>(in), reinterpret_cast<fftwf_complex*>(out));
			//	ifft(NIFFT, pOutData + symb_idx * (NIFFT + CPLen) + CPLen, pInpData + symb_idx * NIFFT);

			/*
			for (i = 0; i < NIFFT; i++)
			{
				printf("%f\t", pInpData[symb_idx * NIFFT + i][0]);
				printf("%f\n", pInpData[symb_idx * NIFFT + i][1]);
			}
			*/
			
			iter_fft(NIFFT, pInpData + symb_idx * NIFFT * 2, pOutData + (symb_idx * (NIFFT + CPLen) + CPLen) * 2, -1);

			for (i = 0; i < NIFFT; i++)
			{
				int out_samp_idx = symb_idx * (NIFFT + CPLen) + CPLen + i;
				
				pOutData[out_samp_idx*2+0] /= norm;
				pOutData[out_samp_idx*2+1] /= norm;
			}

			
			for (i = 0; i < CPLen; i++)
			{
				pOutData[(symb_idx * (NIFFT + CPLen) + i) * 2 + 0] = pOutData[(symb_idx * (NIFFT + CPLen) + NIFFT + i)*2+0];
				pOutData[(symb_idx * (NIFFT + CPLen) + i) * 2 + 1] = pOutData[(symb_idx * (NIFFT + CPLen) + NIFFT + i)*2+1];
			}
		}
	}
	end = clock();
	elapsed_time = elapsed_time + (end - start);
	printf("%lfms\n", elapsed_time * 1000.0 / CLOCKS_PER_SEC);
//	fftwf_destroy_plan(ifftplan);
}

void ofdemodulating(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, float *pOutData)
{
//	fftwf_plan fftplan;

	int NumRxAntenna = lte_phy_params->N_rx_ant;
	int NIFFT = lte_phy_params->N_fft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	int CPLen = lte_phy_params->N_samps_cp_l_0;

	int nrx, nsym, i;
	
//	std::complex<float> in[LTE_PHY_FFT_SIZE_MAX];
//	std::complex<float> out[LTE_PHY_FFT_SIZE_MAX];

//	fftplan=fftwf_plan_dft_1d(NIFFT, reinterpret_cast<fftwf_complex*>(&in[0]), reinterpret_cast<fftwf_complex*>(&out[0]), FFTW_FORWARD, FFTW_ESTIMATE);
	
	static double elapsed_time = 0.0;
	double start, end;
	start = clock();
//	for (nrx=0;nrx<NumRxAntenna;nrx++)
//	{
//		for (nsym=0;nsym<NumULSymbSF;nsym++)
//		{

	int num_threads=236,sum=0,quotient=0,remain=0;
        int ii,j,k;
	sum = NumRxAntenna*NumULSymbSF;
	quotient = sum / num_threads;
	remain = sum % num_threads;
#pragma omp parallel for private(j,k)
	        for(i=0;i<num_threads;i++)
		{
			int i_begin,num_j;
			if(i>=remain)
			{
				i_begin = remain  + i * quotient;
				num_j = quotient;
			}
			else 
			{ 
				i_begin = i * (quotient + 1);
				num_j = quotient + 1;
			} 
			for(j=0;j<num_j;j++)
			{
				nrx = (i_begin + j) / NumULSymbSF;
				nsym = (i_begin + j) % NumULSymbSF;
				int symb_idx = nrx * NumULSymbSF + nsym;
				float norm = (float)sqrt((float)NIFFT) 
			/*(float)1*/;

			//	memcpy(in, &pInpData[SymIdx * (CPLen + NIFFT) + CPLen], NIFFT * sizeof(std::complex<float>));
			
			//	fftwf_execute_dft(fftplan, reinterpret_cast<fftwf_complex*>(&in[0]), reinterpret_cast<fftwf_complex*>(&out[0]));
			/*
			for (i = 0; i < CPLen + NIFFT; i++)
			{
				printf("%f\t", *(pInpData + symb_idx * (CPLen + NIFFT) + i)[0]);
				printf("%f\n", *(pInpData + symb_idx * (CPLen + NIFFT) + i)[1]);
			}
			*/
			
			iter_fft(NIFFT, pInpData + (symb_idx * (CPLen + NIFFT) + CPLen) * 2, pOutData + symb_idx * NIFFT * 2, 1);

			
			for(ii = 0; ii < NIFFT; ii++)
			{
				pOutData[(symb_idx * NIFFT + ii) * 2 + 0] /= norm;
				pOutData[(symb_idx * NIFFT + ii) * 2 + 1] /= norm;
			}
		}
	}
	end = clock();
	elapsed_time += (end - start);
	printf("%lfms\n", elapsed_time * 1000.0 / CLOCKS_PER_SEC);

//	fftwf_destroy_plan(fftplan);
}

