
#include "lte_phy.h"
#include "refs/dmrs.h"

#include <stdlib.h>
#include <immintrin.h>
#include <zmmintrin.h>

#define LEN16 16

#ifdef __MIC__
typedef __attribute__((aligned(64))) union zmmf {
	__m512 reg;
	float elems[LEN16];
} zmmf_t;

typedef __attribute__((aligned(64))) union zmmi {
	__m512i reg;
	int elems[LEN16];
} zmmi_t;
#endif

void SubCarrierMapping(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, float *pOutData)
{
	int NumLayer = lte_phy_params->N_tx_ant;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	int SCLoc = lte_phy_params->sc_loc;
	int NIFFT = lte_phy_params->N_fft_sz;
	int MDFT = lte_phy_params->N_dft_sz;
	int NDMRSslot = 2;

	int in_buf_sz = lte_phy_params->resm_in_buf_sz;
	int out_buf_sz = lte_phy_params->resm_out_buf_sz;

//	float DMRS[2 * (2 * LTE_PHY_N_ANT_MAX * LTE_PHY_DFT_SIZE_MAX)];
	float *pDMRS = (float *)malloc(2 * (NDMRSslot * NumLayer * MDFT) * sizeof(float));

	int *map_table = (int *)malloc(NumULSymbSF * sizeof(int));

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

	geneDMRS(pDMRS, NumLayer, MDFT);

	for (int nlayer = 0; nlayer < NumLayer; nlayer++)
	{
		for (int nsym = 0; nsym < NumULSymbSF; nsym++)
		{
			int SymIdx = nlayer * NumULSymbSF + nsym;

			for (int n = 0; n < NIFFT; n++)
			{
				pOutData[SymIdx * NIFFT + n] = 1.0;
				pOutData[SymIdx * NIFFT + n + out_buf_sz] = 0.0;
			}
		}
	}

	zmmi_t index;
	for (int i = 0; i < LEN16; i++)
	{
		index.elems[i] = 4 * i;
	}
	for (int nlayer = 0; nlayer < NumLayer; nlayer++)
	{
		for (int nsym = 0; nsym < NumULSymbSF; nsym++)
		{
			zmmf_t vbuffer_real, vbuffer_imag;
			
			int SymIdx = nlayer * NumULSymbSF + nsym;

			if (map_table[nsym] < 0)
			{
				// DMRS
				int DMRSslot = (-1) * map_table[nsym] - 1;
				int n;
				for (n = SCLoc; n < SCLoc + MDFT - LEN16 + 1; n += LEN16)
				{
					//	pOutData[SymIdx * NIFFT + n] = pDMRS[(DMRSslot * NumLayer + nlayer) * MDFT + (n - SCLoc)];
					//	pOutData[out_buf_sz + SymIdx * NIFFT + n] = pDMRS[(2 * NumLayer * MDFT) + (DMRSslot * NumLayer + nlayer) * MDFT + (n - SCLoc)];
					// gather
					vbuffer_real.reg = _mm512_i32gather_ps(index.reg, &pDMRS[(DMRSslot * NumLayer + nlayer) * MDFT + (n - SCLoc)], 1);
					vbuffer_imag.reg = _mm512_i32gather_ps(index.reg, &pDMRS[(2 * NumLayer * MDFT) + (DMRSslot * NumLayer + nlayer) * MDFT + (n - SCLoc)], 1);
					// scatter
					_mm512_i32scatter_ps(&pOutData[SymIdx * NIFFT + n], index.reg, vbuffer_real.reg, 1);
					_mm512_i32scatter_ps(&pOutData[out_buf_sz + SymIdx * NIFFT + n], index.reg, vbuffer_imag.reg, 1);
				}
				for (; n < SCLoc + MDFT; n++)
				{
					pOutData[SymIdx * NIFFT + n] = pDMRS[(DMRSslot * NumLayer + nlayer) * MDFT + (n - SCLoc)];
					pOutData[out_buf_sz + SymIdx * NIFFT + n] = pDMRS[(2 * NumLayer * MDFT) + (DMRSslot * NumLayer + nlayer) * MDFT + (n - SCLoc)];
				}
			}
			else
			{
				// Data
				int sym_offset = map_table[nsym];
				int n;
				for (n = SCLoc; n < SCLoc + MDFT - LEN16 + 1; n += LEN16)
				{
					//	pOutData[SymIdx * NIFFT + n] = pInpData[nlayer * (NumULSymbSF - 2) * MDFT + sym_offset * MDFT + n - SCLoc];
					//	pOutData[out_buf_sz + SymIdx * NIFFT + n] = pInpData[in_buf_sz + nlayer * (NumULSymbSF - 2) * MDFT + sym_offset * MDFT + n - SCLoc];
					// gather
					vbuffer_real.reg = _mm512_i32gather_ps(index.reg, &pInpData[nlayer * (NumULSymbSF - 2) * MDFT + sym_offset * MDFT + n - SCLoc], 1);
					vbuffer_imag.reg = _mm512_i32gather_ps(index.reg, &pInpData[in_buf_sz + nlayer * (NumULSymbSF - 2) * MDFT + sym_offset * MDFT + n - SCLoc], 1);
					// scatter
					_mm512_i32scatter_ps(&pOutData[SymIdx * NIFFT + n], index.reg, vbuffer_real.reg, 1);
					_mm512_i32scatter_ps(&pOutData[out_buf_sz + SymIdx * NIFFT + n], index.reg, vbuffer_imag.reg, 1);
				}
				for (; n < SCLoc + MDFT; n++)
				{
					pOutData[SymIdx * NIFFT + n] = pInpData[nlayer * (NumULSymbSF - 2) * MDFT + sym_offset * MDFT + n - SCLoc];
					pOutData[out_buf_sz + SymIdx * NIFFT + n] = pInpData[in_buf_sz + nlayer * (NumULSymbSF - 2) * MDFT + sym_offset * MDFT + n - SCLoc];
				}
			}
		}
	}

	free(pDMRS);
	free(map_table);
}

void SubCarrierDemapping(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, float *pOutData)
{
	int SCLoc = lte_phy_params->sc_loc;
	int NumRxAntenna = lte_phy_params->N_rx_ant;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	int NIFFT = lte_phy_params->N_fft_sz;
	int MDFT = lte_phy_params->N_dft_sz;

	int NDMRSslot = 2;

	int in_buf_sz = lte_phy_params->resdm_in_buf_sz;
	int out_buf_sz = lte_phy_params->resdm_out_buf_sz;

	int *map_table = (int *)malloc(NumULSymbSF * sizeof(int));

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

	for (int nrx = 0; nrx < NumRxAntenna; nrx++)
	{	
		for (int nsym = 0; nsym < NumULSymbSF; nsym++)
		{	
			if (map_table[nsym] < 0)
			{
				// DMRS
				int sym_in_idx = nrx * NumULSymbSF + nsym;
				int sym_out_idx = nrx * NDMRSslot - map_table[nsym] - 1;
				
				for (int n = 0; n < MDFT; n++)
				{
					pOutData[sym_out_idx * MDFT + n] = pInpData[sym_in_idx * NIFFT + (n + SCLoc)];
					pOutData[sym_out_idx * MDFT + n + out_buf_sz] = pInpData[sym_in_idx * NIFFT + (n + SCLoc) + out_buf_sz];
				}
			}
			else
			{
				// Data
				int sym_in_idx = nrx * NumULSymbSF + nsym;
				int sym_out_idx = (NumULSymbSF - 2) * nrx + map_table[nsym] + NumRxAntenna * NDMRSslot;

				for (int n = 0; n < MDFT; n++)
				{
					pOutData[sym_out_idx * MDFT + n] = pInpData[sym_in_idx * NIFFT + (n + SCLoc)];
					pOutData[out_buf_sz + sym_out_idx * MDFT + n] = pInpData[in_buf_sz + sym_in_idx * NIFFT + (n + SCLoc)];
				}
			}
		}
	}

	free(map_table);
}
