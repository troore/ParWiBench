
#include <cstring>
#include "OFDM.h"

#include "meas.h"

void ofmodulating(LTE_PHY_PARAMS *lte_phy_params, float (*pInpData)[2], float (*pOutData)[2])
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
			
			recur_fft(NIFFT, pInpData + symb_idx * NIFFT, pOutData + symb_idx * (NIFFT + CPLen) + CPLen, -1);

			for (i = 0; i < NIFFT; i++)
			{
				int out_samp_idx = symb_idx * (NIFFT + CPLen) + CPLen + i;
				
				pOutData[out_samp_idx][0] /= norm;
				pOutData[out_samp_idx][1] /= norm;
			}

			
			for (i = 0; i < CPLen; i++)
			{
				pOutData[symb_idx * (NIFFT + CPLen) + i][0] = pOutData[symb_idx * (NIFFT + CPLen) + NIFFT + i][0];
				pOutData[symb_idx * (NIFFT + CPLen) + i][1] = pOutData[symb_idx * (NIFFT + CPLen) + NIFFT + i][1];
			}
		}
	}
	end = clock();
	elapsed_time = elapsed_time + (end - start);
	printf("%lfms\n", elapsed_time * 1000.0 / CLOCKS_PER_SEC);
//	fftwf_destroy_plan(ifftplan);
}

void ofdemodulating(LTE_PHY_PARAMS *lte_phy_params, float (*pInpData)[2], float (*pOutData)[2])
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
	for (nrx=0;nrx<NumRxAntenna;nrx++)
	{
		for (nsym=0;nsym<NumULSymbSF;nsym++)
		{
			int symb_idx = nrx * NumULSymbSF + nsym;
			float norm = (float)sqrt((float)NIFFT) /*(float)1*/;

			//	memcpy(in, &pInpData[SymIdx * (CPLen + NIFFT) + CPLen], NIFFT * sizeof(std::complex<float>));
			
			//	fftwf_execute_dft(fftplan, reinterpret_cast<fftwf_complex*>(&in[0]), reinterpret_cast<fftwf_complex*>(&out[0]));
			/*
			for (i = 0; i < CPLen + NIFFT; i++)
			{
				printf("%f\t", *(pInpData + symb_idx * (CPLen + NIFFT) + i)[0]);
				printf("%f\n", *(pInpData + symb_idx * (CPLen + NIFFT) + i)[1]);
			}
			*/
			
			recur_fft(NIFFT, pInpData + symb_idx * (CPLen + NIFFT) + CPLen, pOutData + symb_idx * NIFFT, 1);

			
			for(i = 0; i < NIFFT; i++)
			{
				pOutData[symb_idx * NIFFT + i][0] /= norm;
				pOutData[symb_idx * NIFFT + i][1] /= norm;
			}
		}
	}
	end = clock();
	elapsed_time += (end - start);
	printf("%lfms\n", elapsed_time * 1000.0 / CLOCKS_PER_SEC);

//	fftwf_destroy_plan(fftplan);
}

