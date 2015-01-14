
#include <stdlib.h>

#include "TransformPrecoder.h"
#include "dft/dft.h"

void TransformPrecoding(LTE_PHY_PARAMS *lte_phy_params, float *pInpSeq, float *pDataMatrix)
{
	int NumLayer = lte_phy_params->N_tx_ant;
	int MDFT = lte_phy_params->N_dft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;

	int in_buf_sz = lte_phy_params->trans_encoder_in_buf_sz;
	int out_buf_sz = lte_phy_params->trans_encoder_out_buf_sz;

//	float *p_samp_in_buf = (float *)malloc(2 * MDFT * sizeof(float));
//	float *p_samp_out_buf = (float *)malloc(2 * MDFT * sizeof(float));

	for (int nlayer = 0; nlayer < NumLayer; nlayer++)
	{
		for (int nsym = 0; nsym < NumULSymbSF - 2; nsym++)
		{
			int idx = nlayer * (MDFT * (NumULSymbSF - 2)) + nsym * MDFT;
			float norm = (float)sqrt((float)MDFT);

			dft(MDFT, pInpSeq + idx * 2, pDataMatrix + idx * 2, 1);
				
			for (int m = 0; m < MDFT; m++)
			{
				//	pDataMatrix[2 * (idx + m) + 0] = pDataMatrix[2 * (idx + m) + 0] / norm;
				//	pDataMatrix[2 * (idx + m) + 1] = pDataMatrix[2 * (idx + m) + 1] / norm;
				pDataMatrix[2 * (idx + m) + 0] = pDataMatrix[2 * (idx + m) + 0] / norm;
				pDataMatrix[2 * (idx + m) + 1] = pDataMatrix[2 * (idx + m) + 1] / norm;
			}
		}
	}

//	free(p_samp_in_buf);
//	free(p_samp_out_buf);
}

void TransformDecoding(LTE_PHY_PARAMS *lte_phy_params, float *pDataMatrix, float *pDecSeq)
{
	int NumLayer = lte_phy_params->N_rx_ant;
	int MDFT = lte_phy_params->N_dft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;

	int in_buf_sz = lte_phy_params->trans_decoder_in_buf_sz;
	int out_buf_sz = lte_phy_params->trans_decoder_out_buf_sz;

	for (int nlayer = 0; nlayer < NumLayer; nlayer++)
	{
		for(int nsym = 0; nsym < NumULSymbSF - 2; nsym++)
		{
			int idx = nlayer * (MDFT * (NumULSymbSF - 2)) + nsym * MDFT;
			float norm = (float)sqrt((float)MDFT);

			dft(MDFT, pDataMatrix + idx * 2, pDecSeq + idx * 2, -1);

			for(int m = 0; m < MDFT; m++)
			{
				pDecSeq[2 * (idx + m) + 0] = pDecSeq[2 * (idx + m) + 0] / norm;
				pDecSeq[2 * (idx + m) + 1] = pDecSeq[2 * (idx + m) + 1] / norm;
				/*
				  pDecSeq[idx + m] = pDecSeq[idx + m] / norm;
				  pDecSeq[idx + m + out_buf_sz] = pDecSeq[idx + m + out_buf_sz] / norm;
				*/
			}                              
		}
	}
}
