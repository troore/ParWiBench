
#include "ResMapper.h"

//#include <iostream>

void geneDMRS(float *pDMRS, int N_layer, int N_dft)
{
	int i;
	int table_len = 2 * N_layer * N_dft;
	
	int pPrimeTable[6][2];
	int ncs[2] = {3, 11};
	int Nzc;
	int zc_flag;
	int idx;
	int RSu, RSv;
	double qbar, q;
//	std::complex<double> px[1200];
	double px[2][1200];
	
	pPrimeTable[0][0] = 75;		pPrimeTable[0][1] = 73;
	pPrimeTable[1][0] = 150;	pPrimeTable[1][1] = 149;
	pPrimeTable[2][0] = 300;	pPrimeTable[2][1] = 293;
	pPrimeTable[3][0] = 600;	pPrimeTable[3][1] = 599;
	pPrimeTable[4][0] = 900;	pPrimeTable[4][1] = 887;
	pPrimeTable[5][0] = 1200;	pPrimeTable[5][1] = 1193;

	zc_flag = 1;
	idx = 0;
	while (zc_flag)
	{
		if (pPrimeTable[idx][0] == N_dft)
		{ 
			Nzc = pPrimeTable[idx][1];
			zc_flag = 0;
		}
		else
		{
			idx++;
		}
	}

	RSu = 0;
	RSv = 0;
	qbar = ((double)Nzc) * (RSu + 1.0) / 31.0;
	q = floor((qbar + 0.5)) + RSv * pow(-1.0, (floor((2.0 * qbar))));

#pragma simd
	for (int m = 0; m < Nzc; m++)
	{
		double theta = (-PI * q * m * (m + 1.0)) / ((double)Nzc);
		//	px[m] = std::complex<double>(cos(theta), sin(theta));
		px[0][m] = cos(theta);
		px[1][m] = sin(theta);
	}

	for (int slot = 0; slot < 2; slot++)
	{
		for (int layer = 0; layer < N_layer; layer++)
		{
			int cs = ncs[slot] + 6*layer;
			double alpha = (2.0 * PI * cs) / 12.0;

#pragma simd			
			for (int n = 0; n < N_dft; n++)
			{
				int idx = (n % Nzc);
				double a = cos(alpha * n);
				double b = sin(alpha * n);
				//	std::complex<double> c = std::complex<double>((cos(alpha*n)),(sin(alpha*n)));
				double c[2] = {a, b};

				pDMRS[(slot * N_layer + layer) * N_dft + n] = c[0] * px[0][idx] - c[1] * px[1][idx];
				pDMRS[table_len + (slot * N_layer + layer) * N_dft + n] = c[0] * px[1][idx] + c[1] * px[0][idx];
			}
		}
	}

	if (N_layer == 2)
	{
#pragma simd
		for (int n = 0; n < N_dft; n++)
		{
			//	(*(*(*(pDMRS+1)+1)+n))*=(-1.0);
			//	pDMRS[(1 * N_layer + 1) * N_dft + n] *= (-1.0);
			pDMRS[(1 * N_layer + 1) * N_dft + n] = -1.0 * pDMRS[(1 * N_layer + 1) * N_dft + n];
			pDMRS[table_len + (1 * N_layer + 1) * N_dft + n] = -1.0 * pDMRS[table_len + (1 * N_layer + 1) * N_dft + n];
		}
	}
	else
	{}
}

void SubCarrierMapping(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, float *pOutData)
{
	int NumLayer = lte_phy_params->N_tx_ant;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	int SCLoc = lte_phy_params->sc_loc;
	int NIFFT = lte_phy_params->N_fft_sz;
	int MDFT = lte_phy_params->N_dft_sz;

	int in_buf_sz = lte_phy_params->resm_in_buf_sz;
	int out_buf_sz = lte_phy_params->resm_out_buf_sz;

//	float DMRS[2 * (2 * LTE_PHY_N_ANT_MAX * LTE_PHY_DFT_SIZE_MAX)];
	float *pDMRS = (float *)malloc(2 * (2 * NumLayer * MDFT) * sizeof(float));

	geneDMRS(pDMRS, NumLayer, MDFT);

	/*
	for (int nlayer = 0; nlayer < NumLayer; nlayer++)
	{
		for (int nsym = 0; nsym < NumULSymbSF; nsym++)
		{
			int SymIdx = nlayer * NumULSymbSF + nsym;

			for (int n = 0; n < NIFFT; n++)
			{
				pOutData[SymIdx * NIFFT + n] = std::complex<float>(1.0, 0.0);
			}
		}
	}
	*/
	int nsig = NumLayer * NumULSymbSF * NIFFT;

#pragma
	for (int k = 0; k < nsig; k++)
	{
		pOutData[k] = 1.0;
		pOutData[out_buf_sz + k] = 0.0;
	}

	for (int nlayer = 0; nlayer < NumLayer; nlayer++)
	{
		int DMRSslot = 0;
//#pragma omp parallel for		
		for (int nsym = 0; nsym < NumULSymbSF; nsym++)
		{
			int SymIdx = nlayer * NumULSymbSF + nsym;
				
			if (nsym == lte_phy_params->dmrs_symb_pos[DMRSslot])
			{
#pragma simd
				for (int n = SCLoc; n < SCLoc + MDFT; n++)
				{
					//	pOutData[SymIdx * NIFFT + n] = DMRS[(DMRSslot * NumLayer + nlayer) * MDFT + (n - SCLoc)];
					pOutData[SymIdx * NIFFT + n] = pDMRS[(DMRSslot * NumLayer + nlayer) * MDFT + (n - SCLoc)];
					pOutData[out_buf_sz + SymIdx * NIFFT + n] = pDMRS[(2 * NumLayer * MDFT) + (DMRSslot * NumLayer + nlayer) * MDFT + (n - SCLoc)];
				}
				DMRSslot++;
			}
			else
			{
#pragma simd
				for (int n = SCLoc; n < SCLoc + MDFT; n++)
				{
					//	pOutData[SymIdx * NIFFT + n] = pInpData[nlayer * (NumULSymbSF - 2) * MDFT + (nsym - DMRSslot) * MDFT + n - SCLoc];
					pOutData[SymIdx * NIFFT + n] = pInpData[nlayer * (NumULSymbSF - 2) * MDFT + (nsym - DMRSslot) * MDFT + n - SCLoc];
					pOutData[out_buf_sz + SymIdx * NIFFT + n] = pInpData[in_buf_sz + nlayer * (NumULSymbSF - 2) * MDFT + (nsym - DMRSslot) * MDFT + n - SCLoc];
					
				}
			}
		}
	}

	free(pDMRS);
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
