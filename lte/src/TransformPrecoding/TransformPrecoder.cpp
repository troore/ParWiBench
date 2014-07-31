
#include "TransformPrecoder.h"
#include <cstring>

void TransformPrecoding(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pInpSeq, std::complex<float> *pDataMatrix)
{
	int NumLayer = lte_phy_params->N_tx_ant;
	int MDFT = lte_phy_params->N_dft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	
	fftwf_plan fftplan;
	std::complex<float> in[LTE_PHY_DFT_SIZE_MAX];
	std::complex<float> out[LTE_PHY_DFT_SIZE_MAX];

	fftplan = fftwf_plan_dft_1d(MDFT, reinterpret_cast<fftwf_complex*>(&in[0]), reinterpret_cast<fftwf_complex*>(&out[0]), FFTW_FORWARD, FFTW_ESTIMATE);

	for(int nlayer=0;nlayer<NumLayer;nlayer++)
	{
		for(int nsym=0;nsym<NumULSymbSF-2;nsym++)
		{

			int idx = nlayer*(MDFT*(NumULSymbSF-2))+nsym*MDFT;

			memcpy(in, &pInpSeq[idx], MDFT * sizeof(std::complex<float>));

			fftwf_execute_dft(fftplan, reinterpret_cast<fftwf_complex*>(&in[0]), reinterpret_cast<fftwf_complex*>(&out[0]));
				
			for(int m = 0;m < MDFT; m++)
			{
				pDataMatrix[idx + m] = out[m] / (float)sqrt((float)MDFT);
			}
				
		}
	}

	fftwf_destroy_plan(fftplan);
}

void TransformDecoding(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pDataMatrix, std::complex<float> *pDecSeq)
{
	int NumLayer = lte_phy_params->N_rx_ant;
	int MDFT = lte_phy_params->N_dft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;

	fftwf_plan ifftplan;
	std::complex<float> in[LTE_PHY_DFT_SIZE_MAX];
	std::complex<float> out[LTE_PHY_DFT_SIZE_MAX];

	ifftplan=fftwf_plan_dft_1d(MDFT,reinterpret_cast<fftwf_complex*>(&in[0]),reinterpret_cast<fftwf_complex*>(&out[0]),FFTW_BACKWARD,FFTW_ESTIMATE);

	for(int nlayer=0;nlayer<NumLayer;nlayer++)
	{
		for(int nsym=0;nsym<NumULSymbSF-2;nsym++)
		{
			int idx = nlayer*(MDFT*(NumULSymbSF-2))+nsym*MDFT;

			memcpy(in, &pDataMatrix[idx], MDFT * sizeof(std::complex<float>));
				
			fftwf_execute_dft(ifftplan, reinterpret_cast<fftwf_complex*>(&in[0]), reinterpret_cast<fftwf_complex*>(&out[0]));

			for(int m = 0; m < MDFT; m++)
			{
				pDecSeq[idx + m] = out[m] / (float)sqrt((float)MDFT);
			}                              
		}
	}

	fftwf_destroy_plan(ifftplan);
}
