
#include "TransformPrecoder.h"
#include "dft.h"
#include <cstring>

#ifdef DEBUG_FFT
void TransformPrecoding(LTE_PHY_PARAMS *lte_phy_params, /*float*/std::complex<float> *pInpSeq, /*float*/std::complex<float> *pDataMatrix)
#else
void TransformPrecoding(LTE_PHY_PARAMS *lte_phy_params, float *pInpSeq, float *pDataMatrix)
#endif
{
	int NumLayer = lte_phy_params->N_tx_ant;
	int MDFT = lte_phy_params->N_dft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;

#ifdef DEBUG_FFT
	fftwf_plan fftplan;
	std::complex<float> in[LTE_PHY_DFT_SIZE_MAX];
	std::complex<float> out[LTE_PHY_DFT_SIZE_MAX];

	fftplan = fftwf_plan_dft_1d(MDFT, reinterpret_cast<fftwf_complex*>(&in[0]), reinterpret_cast<fftwf_complex*>(&out[0]), FFTW_FORWARD, FFTW_ESTIMATE);
#endif

	for(int nlayer=0;nlayer<NumLayer;nlayer++)
	{
		for(int nsym=0;nsym<NumULSymbSF-2;nsym++)
		{

			int idx = nlayer*(MDFT*(NumULSymbSF-2))+nsym*MDFT;
			float norm = (float)sqrt((float)MDFT);
#ifdef DEBUG_FFT
			memcpy(in, &pInpSeq[idx], MDFT * sizeof(std::complex<float>));
			fftwf_execute_dft(fftplan, reinterpret_cast<fftwf_complex*>(&in[0]), reinterpret_cast<fftwf_complex*>(&out[0]));
#else
			dft(MDFT, pInpSeq + idx * 2, pDataMatrix + idx * 2, 1);
#endif
				
			for(int m = 0;m < MDFT; m++)
			{
				#ifdef DEBUG_FFT
				pDataMatrix[idx + m] = out[m] / norm;
				#else
				pDataMatrix[2 * (idx + m) + 0] = pDataMatrix[2 * (idx + m) + 0] / norm;
				pDataMatrix[2 * (idx + m) + 1] = pDataMatrix[2 * (idx + m) + 1] / norm;
				#endif
			}
				
		}
	}

#ifdef DEBUG_FFT
	fftwf_destroy_plan(fftplan);
#endif
}

#ifdef DEBUG_FFT
void TransformDecoding(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pDataMatrix, std::complex<float> *pDecSeq)
#else
void TransformDecoding(LTE_PHY_PARAMS *lte_phy_params, float *pDataMatrix, float *pDecSeq)
#endif
{
	int NumLayer = lte_phy_params->N_rx_ant;
	int MDFT = lte_phy_params->N_dft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;

#ifdef DEBUG_FFT
	fftwf_plan ifftplan;
	std::complex<float> in[LTE_PHY_DFT_SIZE_MAX];
	std::complex<float> out[LTE_PHY_DFT_SIZE_MAX];

	ifftplan=fftwf_plan_dft_1d(MDFT,reinterpret_cast<fftwf_complex*>(&in[0]),reinterpret_cast<fftwf_complex*>(&out[0]),FFTW_BACKWARD,FFTW_ESTIMATE);
#endif

	for(int nlayer=0;nlayer<NumLayer;nlayer++)
	{
		for(int nsym=0;nsym<NumULSymbSF-2;nsym++)
		{
			int idx = nlayer*(MDFT*(NumULSymbSF-2))+nsym*MDFT;
			float norm = (float)sqrt((float)MDFT);

#ifdef DEBUG_FFT
				memcpy(in, &pDataMatrix[idx], MDFT * sizeof(std::complex<float>));
				
				fftwf_execute_dft(ifftplan, reinterpret_cast<fftwf_complex*>(&in[0]), reinterpret_cast<fftwf_complex*>(&out[0]));
#else
				dft(MDFT, pDataMatrix + idx * 2, pDecSeq + idx * 2, -1);
#endif

			for(int m = 0; m < MDFT; m++)
			{
#ifdef DEBUG_FFT
				pDecSeq[idx + m] = out[m] / norm;
#else
				pDecSeq[2 * (idx + m) + 0] = pDecSeq[2 * (idx + m) + 0] / norm;
				pDecSeq[2 * (idx + m) + 1] = pDecSeq[2 * (idx + m) + 1] / norm;
#endif
			}                              
		}
	}

#ifdef DEBUG_FFT
	fftwf_destroy_plan(ifftplan);
#endif
}
