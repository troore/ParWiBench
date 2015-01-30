
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "lte_phy.h"
#include "util.h"

static int InterColumnPattern[32] = {0,16,8,24,4,20,12,28,
									 2,18,10,26,6,22,14,30,
									 1,17,9,25,5,21,13,29,3,
									 19,11,27,7,23,15,31};

template <typename T>
static void SubblockInterleaving(int SeqLen, T *pInpMtr, T *pOutMtr)
{
	int D;
	int K_pi;
	int Rate;
	int R_sb, C_sb;
	int NumDummy;
	T DummyValue;
		
	int OutIdx;

	D = SeqLen;
	Rate = 3;
	C_sb = 32;
	
	R_sb = (D + (C_sb - 1)) / C_sb;
	
	K_pi = R_sb * C_sb;
	NumDummy = K_pi - D;
	DummyValue = (T)1000000;

//	T pInterMatrix[((BLOCK_SIZE + 31) / 32) * 32];
	T *pInterMatrix = (T *)malloc(K_pi * sizeof(T));

	omp_set_num_threads(num_threads);
	for (int StrIdx = 0; StrIdx < (Rate - 1); StrIdx++)
	{
		//////////////// write into matrix //////////////////
#pragma omp parallel
		{
			#pragma omp for
			for (int r = 0; r < R_sb; r++)
			{
				for (int c = 0; c < C_sb; c++)
				{
					int k = r * C_sb + c;
				
					if (k < NumDummy)
					{
						pInterMatrix[r * C_sb + c] = DummyValue;
					}
					else
					{
						pInterMatrix[r * C_sb + c] = pInpMtr[StrIdx + (k - NumDummy) * Rate];
					}
				}
			}
		}
		
		OutIdx = 0;
		for (int c = 0; c < C_sb; c++)
		{
			int col = InterColumnPattern[c];

			/*
			for (int r = 0; r < R_sb; r++)
			{
				int k = col * R_sb + r;
				
				T v = pInterMatrix[r * C_sb + col];
				if (v != DummyValue)
				{
					pOutMtr[StrIdx + OutIdx * Rate] = v;
					OutIdx++;
				}  
			}
			*/

			if (col < NumDummy)
			{
#pragma omp parallel shared (OutIdx)
				{
#pragma omp for 
					for (int r = 1; r < R_sb; r++)
					{
						T v = pInterMatrix[r * C_sb + col];
						pOutMtr[StrIdx + (OutIdx + (r - 1)) * Rate] = v;
					}
				}
				OutIdx += (R_sb - 1);
			}
			else
			{
#pragma omp parallel shared (OutIdx)
				{
#pragma omp for 
					for (int r = 0; r < R_sb; r++)
					{
						T v = pInterMatrix[r * C_sb + col];
						pOutMtr[StrIdx + (OutIdx + r) * Rate] = v;
					}
				}
				OutIdx += R_sb;
			}
		}  
	}
	
//////////////////// Interleaving for i=2 ///////////////////////

//	int Pi[((BLOCK_SIZE + 31) / 32) * 32];
//	T pInterSeq[((BLOCK_SIZE + 31) / 32) * 32];
	int *Pi = (int *)malloc(K_pi * sizeof(int));
	T *pInterSeq = (T *)malloc(K_pi * sizeof(T));
	
	for (int k = 0;k < NumDummy; k++)
	{
		pInterSeq[k] = DummyValue;
	}
	for (int k = NumDummy; k < K_pi; k++)
	{
		pInterSeq[k] = pInpMtr[(Rate - 1) + (k - NumDummy) * Rate];
	}

	for (int k = 0; k < K_pi; k++)
	{
		int idxP = k / R_sb;
		int idx = (InterColumnPattern[idxP] + (C_sb * (k % R_sb)) + 1) % K_pi;

		Pi[k] = idx;
	}

	OutIdx=0;
	for (int k = 0; k < K_pi; k++)
	{
		T v = pInterSeq[Pi[k]];
		if (v != DummyValue)
		{
			pOutMtr[(Rate - 1) + OutIdx * Rate] = v;
			OutIdx++;
		}
	}

	free(pInterMatrix);
	free(Pi);
	free(pInterSeq);
}


template<typename T>
static void SubblockDeInterleaving(int SeqLen, T *pInpMtr, T *pOutMtr)
{
	int D;
	int K_pi;
	int Rate;
	int R_sb, C_sb;
	int NumDummy;
	T DummyValue;
	
	int InIdx;
	int OutIdx;

	D = SeqLen;
	Rate = 3;
	C_sb = 32;
	
	R_sb = (D + (C_sb - 1)) / C_sb;
	
	K_pi = R_sb * C_sb;
	NumDummy = K_pi - D;
	DummyValue = (T)1000000;
	
//////////////////// DeInterleaving for i=0,1 ///////////////////////
//	T pInterMatrix[((BLOCK_SIZE + 31) / 32) * 32];
	T *pInterMatrix = (T *)malloc(K_pi * sizeof(float));
	
	for (int StrIdx = 0; StrIdx < (Rate - 1); StrIdx++)
	{
		InIdx=0;
		for (int r = 0;r < R_sb; r++)
		{
			for (int c = 0; c < C_sb; c++)
			{
				int k = r * C_sb + c;
				
				if (k < NumDummy)
				{
					pInterMatrix[r * C_sb + c] = DummyValue;
				}
				else
				{
					pInterMatrix[r * C_sb + c] = (T)0;
				}
			}
		}

		for (int c = 0; c < C_sb; c++)
		{
			int col = InterColumnPattern[c];
			for (int r = 0; r < R_sb; r++)
			{
				int k = col * R_sb + r;
				T v = pInterMatrix[r * C_sb + col];
				if (v == DummyValue)
				{}
				else
				{
					//	pInterMatrix[r * C_sb + col] = pInpMtr[StrIdx * D + InIdx];
					pInterMatrix[r * C_sb + col] = pInpMtr[StrIdx + InIdx * Rate];
					InIdx++;
				}  
			}
		}  

		OutIdx=0;
		for (int r = 0; r < R_sb; r++)
		{
			for(int c = 0; c < C_sb; c++)
			{
				T v = pInterMatrix[r * C_sb + c];
				if (v == DummyValue)
				{}
				else
				{
					//	pOutMtr[StrIdx * D + OutIdx] = pInterMatrix[r * C_sb + c];
					pOutMtr[StrIdx + OutIdx * Rate] = pInterMatrix[r * C_sb + c];
					OutIdx++;
				}
			}
		}
	}

//////////////////// DeInterleaving for i=2 ///////////////////////
//	int Pi[((BLOCK_SIZE + 31) / 32) * 32];
//	T pInterSeq[((BLOCK_SIZE + 31) / 32) * 32];
	int *Pi = (int *)malloc(K_pi * sizeof(int));
	T *pInterSeq = (T *)malloc(K_pi * sizeof(T));
	
	for (int k = 0; k < NumDummy; k++)
		pInterSeq[k] = DummyValue;
//////////////// Pi & DePi//////////////////
	for (int k = 0; k < K_pi; k++)
	{
		int idxP = k / R_sb;
		int idx = (InterColumnPattern[idxP] + (C_sb * (k % R_sb)) + 1) % K_pi;
		Pi[k]=idx;
	}

/////////////// DeInterleaving ////////////////////
	InIdx=0;
	for (int k = 0; k < K_pi; k++)
	{
		T v = pInterSeq[Pi[k]];
		//	printf("%d %f\n", Pi[k], v);
		if (v == DummyValue)
		{}
		else
		{
			//	pInterSeq[Pi[k]] = pInpMtr[(Rate - 1) * D + InIdx];
			pInterSeq[Pi[k]] = pInpMtr[(Rate - 1) + InIdx * Rate];
			InIdx++;
		}
	}

	OutIdx=0;
	for (int k = NumDummy; k < K_pi; k++)
	{
		//	pOutMtr[(Rate - 1) * D + OutIdx] = pInterSeq[k];
		pOutMtr[(Rate - 1) + OutIdx * Rate] = pInterSeq[k];
		OutIdx++;
	}
	
	free(pInterMatrix);
	free(Pi);
	free(pInterSeq);
}


void TxRateMatching(LTE_PHY_PARAMS *lte_phy_params, int *piSeq, int *pcSeq)
{
	int in_buf_sz;
	int out_buf_sz;
	int n_blocks;
	int rm_blk_sz;
	int rm_data_length;
	int rm_last_blk_len;
	int out_block_offset;
	int n_extra_bits;
	int cur_blk_len;

//	int pInMatrix[RATE * (BLOCK_SIZE + 4)];
//	int pOutMatrix[RATE * (BLOCK_SIZE + 4)];

	int i, j, r;

	in_buf_sz = lte_phy_params->rm_in_buf_sz;
	out_buf_sz = lte_phy_params->rm_out_buf_sz;
	rm_blk_sz = BLOCK_SIZE + 4;
	rm_data_length = (in_buf_sz / RATE);

	n_blocks = (rm_data_length + (rm_blk_sz - 1)) / rm_blk_sz;
	if (rm_data_length % rm_blk_sz)
	{
		rm_last_blk_len = (rm_data_length % rm_blk_sz);
	}
	else
	{
		rm_last_blk_len = rm_blk_sz;
	}

	out_block_offset = 0;
	for (i = 0; i < n_blocks; i++)
	{
		cur_blk_len = (i != (n_blocks - 1)) ? rm_blk_sz : rm_last_blk_len;

		/*
		// Transposition
		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{
				pInMatrix[r * cur_blk_len + j] = piSeq[out_block_offset + RATE * j + r];
				//	cout << pInMatrix[r][i];
			}
		}
		//cout << endl;
		*/
			
		//	SubblockInterleaving(cur_blk_len, pInMatrix, pOutMatrix);
		SubblockInterleaving(cur_blk_len, piSeq + out_block_offset, pcSeq + out_block_offset);

		/*
		// Transposition again
		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{
				pcSeq[out_block_offset + RATE * j + r] = pOutMatrix[r * cur_blk_len + j];
			}
		}
		*/

		out_block_offset += RATE * cur_blk_len;
	}

	n_extra_bits = out_buf_sz - in_buf_sz;
	for (i = 0; i < n_extra_bits; i++)
	{
		pcSeq[in_buf_sz + i] = 0;
	}
}

void RxRateMatching(LTE_PHY_PARAMS *lte_phy_params, float *pLLRin, float *pLLRout, int *pHD)
{
	int n_blocks;
	int rm_blk_sz;
	int rm_data_length;
	int rm_last_blk_len;
	int out_block_offset;
	int cur_blk_len;
	int out_buf_sz;
	
//	float pInMatrix[RATE * (BLOCK_SIZE + 4)];
//	float pOutMatrix[RATE * (BLOCK_SIZE + 4)];

//	float *pInMatrix = (float *)malloc(RATE * (BLOCK_SIZE + 4) * sizeof(float));
//	float *pOutMatrix = (float *)malloc(RATE * (BLOCK_SIZE + 4) * sizeof(float));

	int i, j, r;

	out_buf_sz = lte_phy_params->rdm_out_buf_sz;
	rm_blk_sz = BLOCK_SIZE + 4;
	rm_data_length = (out_buf_sz / RATE);

	n_blocks = (rm_data_length + (rm_blk_sz - 1)) / rm_blk_sz;
	if (rm_data_length % rm_blk_sz)
	{
		rm_last_blk_len = (rm_data_length % rm_blk_sz);
	}
	else
	{
		rm_last_blk_len = rm_blk_sz;
	}

	out_block_offset = 0;
	for (i = 0; i < n_blocks; i++)
	{
		cur_blk_len = (i != (n_blocks - 1)) ? rm_blk_sz : rm_last_blk_len;

		/*
		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{
				pInMatrix[r * cur_blk_len + j] = pLLRin[out_block_offset + RATE * j + r];
			}
		}
		*/

		//	SubblockDeInterleaving(cur_blk_len, pInMatrix, pOutMatrix);
		SubblockDeInterleaving(cur_blk_len, pLLRin + out_block_offset, pLLRout + out_block_offset);

		/*
		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{
				pLLRin[out_block_offset + RATE * j + r] = pOutMatrix[r * cur_blk_len + j];
			}
		}
		*/

		out_block_offset += RATE * cur_blk_len;
	}

	for (i = 0; i < out_buf_sz; i++)
	{
		if (pLLRin[i] < 0)
		{
			pHD[i] = 0;
		}
		else
		{
			pHD[i] = 1;
		}
	}

	/*
	for (i = 0; i < out_buf_sz; i++)
	{
		pLLRout[i] = pLLRin[i];
	}
	*/

//	free(pInMatrix);
//	free(pOutMatrix);
}

