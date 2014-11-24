
#include "OFDM.h"
#include <cstring>

void ofmodulating(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pInpData, std::complex<float> *pOutData)
{
	fftwf_plan ifftplan;

	int NumLayer = lte_phy_params->N_tx_ant;
	int NIFFT = lte_phy_params->N_fft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	int CPLen = lte_phy_params->N_samps_cp_l_0;
	
	std::complex<float> in[LTE_PHY_FFT_SIZE_MAX];
	std::complex<float> out[LTE_PHY_FFT_SIZE_MAX];
	
	ifftplan = fftwf_plan_dft_1d(NIFFT, reinterpret_cast<fftwf_complex*>(&in[0]), reinterpret_cast<fftwf_complex*>(&out[0]), FFTW_BACKWARD, FFTW_ESTIMATE);

	for(int nlayer=0;nlayer<NumLayer;nlayer++)
	{
		for(int nsym=0;nsym<NumULSymbSF;nsym++)
		{
			int idx=nlayer*NumULSymbSF+nsym;
			
			memcpy(in, &pInpData[idx * NIFFT], NIFFT * sizeof(std::complex<float>));
			
			fftwf_execute_dft(ifftplan, reinterpret_cast<fftwf_complex*>(in), reinterpret_cast<fftwf_complex*>(out));
			
			for(int n=0;n<NIFFT;n++)
			{
				pOutData[idx * (NIFFT + CPLen) + CPLen + n] = out[n] / (float)sqrt((float)NIFFT);
			}

			for(int n=0;n<CPLen;n++)
			{
				pOutData[idx * (NIFFT + CPLen) + n] = pOutData[idx * (NIFFT + CPLen) + NIFFT + n];
			}
		}
	}

	fftwf_destroy_plan(ifftplan);
}

void ofdemodulating(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pInpData, std::complex<float> *pOutData)
{
	fftwf_plan fftplan;

	int NumRxAntenna = lte_phy_params->N_rx_ant;
	int NIFFT = lte_phy_params->N_fft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	int CPLen = lte_phy_params->N_samps_cp_l_0;
	
	std::complex<float> in[LTE_PHY_FFT_SIZE_MAX];
	std::complex<float> out[LTE_PHY_FFT_SIZE_MAX];

	fftplan=fftwf_plan_dft_1d(NIFFT, reinterpret_cast<fftwf_complex*>(&in[0]), reinterpret_cast<fftwf_complex*>(&out[0]), FFTW_FORWARD, FFTW_ESTIMATE);
	
	for(int nrx=0;nrx<NumRxAntenna;nrx++)
	{

		for(int nsym=0;nsym<NumULSymbSF;nsym++)
		{
			int SymIdx = nrx*NumULSymbSF+nsym;

			memcpy(in, &pInpData[SymIdx * (CPLen + NIFFT) + CPLen], NIFFT * sizeof(std::complex<float>));
			
			fftwf_execute_dft(fftplan, reinterpret_cast<fftwf_complex*>(&in[0]), reinterpret_cast<fftwf_complex*>(&out[0]));
			
			for(int n = 0; n < NIFFT; n++)
			{
				pOutData[SymIdx * NIFFT + n] = out[n] / (float)sqrt((float)NIFFT);
			}
		}
	}

	fftwf_destroy_plan(fftplan);
}

