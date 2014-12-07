
#include "OFDM.h"
#include <string.h>

#include "meas.h"

void ofmodulating(LTE_PHY_PARAMS *lte_phy_params, float (*pInpData)[2], float (*pOutData)[2])
{
	int NumLayer = lte_phy_params->N_tx_ant;
	int NIFFT = lte_phy_params->N_fft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	int CPLen = lte_phy_params->N_samps_cp_l_0;

	int nlayer, nsym, i;
	
	double start, end;
	start = clock();
	for (nlayer = 0; nlayer < NumLayer; nlayer++)
	{
		for (nsym = 0; nsym < NumULSymbSF; nsym++)
		{
			int symb_idx = nlayer * NumULSymbSF + nsym;
			float norm = (float)sqrt((float)NIFFT) /*1.0*/;

			fft(NIFFT, pInpData + symb_idx * NIFFT, pOutData + symb_idx * (NIFFT + CPLen) + CPLen, 1);

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
	printf("%lfms\n", ((end - start) * 1000.0 / CLOCKS_PER_SEC));
}

void ofdemodulating(LTE_PHY_PARAMS *lte_phy_params, float (*pInpData)[2], float (*pOutData)[2])
{
	int NumRxAntenna = lte_phy_params->N_rx_ant;
	int NIFFT = lte_phy_params->N_fft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	int CPLen = lte_phy_params->N_samps_cp_l_0;

	int nrx, nsym, i;
	
	double start, end;
	start = clock();
	for (nrx = 0; nrx < NumRxAntenna; nrx++)
	{
		for (nsym = 0; nsym < NumULSymbSF; nsym++)
		{
			int symb_idx = nrx * NumULSymbSF + nsym;
			float norm = (float)sqrt((float)NIFFT) /*1.0*/;

			/*
			for (i = 0; i < CPLen + NIFFT; i++)
			{
				printf("%f\t", *(pInpData + symb_idx * (CPLen + NIFFT) + i)[0]);
				printf("%f\n", *(pInpData + symb_idx * (CPLen + NIFFT) + i)[1]);
			}
			*/
			
			fft(NIFFT, pInpData + symb_idx * (CPLen + NIFFT) + CPLen, pOutData + symb_idx * NIFFT, -1);
			
			for(i = 0; i < NIFFT; i++)
			{
				pOutData[symb_idx * NIFFT + i][0] /= norm;
				pOutData[symb_idx * NIFFT + i][1] /= norm;
			}
		}
	}
	end = clock();
	printf("%lfms\n", ((end - start) * 1000.0 / CLOCKS_PER_SEC));
}

