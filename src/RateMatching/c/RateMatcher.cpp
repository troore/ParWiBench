
#include "lte_phy.h"

static int InterColumnPattern[32] = {0,16,8,24,4,20,12,28,
									 2,18,10,26,6,22,14,30,
									 1,17,9,25,5,21,13,29,3,
									 19,11,27,7,23,15,31};

template <typename T>
#ifdef DEBUG_INTL
static void SubblockInterleaving(int SeqLen, T **pInpMtr, T **pOutMtr)
#else
static void SubblockInterleaving(int SeqLen, T pInpMtr[], T pOutMtr[])
#endif
{
	int D;
	int K_pi;
	int Rate;
	int R_sb, C_sb;
	int NumDummy;
	T DummyValue;
//	int InterColumnPattern[32];
		
	int OutIdx;

	D = SeqLen;
	Rate = 3;
	C_sb = 32;
	
#ifdef DEBUG_INTL
	R_sb = (int)(ceil((((float)D) / ((float)C_sb))));
#else
	R_sb = (D + (C_sb - 1)) / C_sb;
#endif
	
	K_pi = R_sb * C_sb;
	NumDummy = K_pi - D;
	DummyValue = (T)1000000;

#ifdef DEBUG_INTL
	T **pInterMatrix=new T*[R_sb];
	for(int r=0;r<R_sb;r++){*(pInterMatrix+r)=new T[C_sb];}
#else
	T pInterMatrix[((BLOCK_SIZE + 4 + 31) / 32) * 32];
#endif

#ifdef DEBUG_INTL
	T *VpInpSeq;
	T *VpOutSeq;
#endif
	
	for (int StrIdx = 0; StrIdx < (Rate - 1); StrIdx++)
	{
#ifdef DEBUG_INTL
		VpInpSeq=*(pInpMtr+StrIdx);
		VpOutSeq=*(pOutMtr+StrIdx);
#endif
		//////////////// write into matrix //////////////////
		for (int r = 0; r < R_sb; r++)
		{
			for (int c = 0; c < C_sb; c++)
			{
				int k = r * C_sb + c;
				
				if (k < NumDummy)
				{
#ifdef DEBUG_INTL
					*(*(pInterMatrix+r) + c) = DummyValue;
#else
					pInterMatrix[r * C_sb + c] = DummyValue;
#endif
				}
				else
				{
#ifdef DEBUG_INTL
					*(*(pInterMatrix+r)+c)=*(VpInpSeq+(k-NumDummy));
#else
					pInterMatrix[r * C_sb + c] = pInpMtr[StrIdx * D + k - NumDummy];
#endif
				}
			}
		}
		
		OutIdx=0;
		for (int c = 0; c < C_sb; c++)
		{
			int col = InterColumnPattern[c];
			for (int r = 0; r < R_sb; r++)
			{
				int k = col * R_sb + r;
				
#ifdef DEBUG_INTL
				T v = *(*(pInterMatrix+r)+col);
#else
				T v = pInterMatrix[r * C_sb + col];
#endif
				
				if (v == DummyValue)
				{}
				else
				{
#ifdef DEBUG_INTL
					*(VpOutSeq+OutIdx)=v;
#else
					pOutMtr[StrIdx * D + OutIdx] = v;
#endif
					OutIdx++;
				}  
			}
		}  
	}
	
//////////////////// Interleaving for i=2 ///////////////////////
#ifdef DEBUG_INTL
	int *Pi=new int[K_pi];
	T *pInterSeq=new T[K_pi];
#else
	int Pi[((BLOCK_SIZE + 4 + 31) / 32) * 32];
	T pInterSeq[((BLOCK_SIZE + 4 + 31) / 32) * 32];
#endif

#ifdef DEBUG_INTL
	VpInpSeq=*(pInpMtr+(Rate-1));
	VpOutSeq=*(pOutMtr+(Rate-1));
#endif
	
	for (int k = 0;k < NumDummy; k++)
	{
#ifdef DEBUG_INTL
		*(pInterSeq+k)=DummyValue;
#else
		pInterSeq[k] = DummyValue;
#endif
	}
	for (int k = NumDummy; k < K_pi; k++)
	{
#ifdef DEBUG_INTL
		*(pInterSeq+k)=*(VpInpSeq+(k-NumDummy));
#else
		pInterSeq[k] = pInpMtr[(Rate - 1) * D + (k - NumDummy)];
#endif
	}

	for (int k = 0; k < K_pi; k++)
	{
#ifdef DEBUG_INTL
		int idxP=(int)(floor((((double)(k))/((double)(R_sb)))));
#else
		int idxP = k / R_sb;
#endif
		int idx = (InterColumnPattern[idxP] + (C_sb * (k % R_sb)) + 1) % K_pi;

#ifdef DEBUG_INTL
		*(Pi+k)=idx;
#else
		Pi[k] = idx;
#endif
	}

	OutIdx=0;
	for (int k = 0; k < K_pi; k++)
	{
#ifdef DEBUG_INTL
		T v = *(pInterSeq+(*(Pi+k)));
#else
		T v = pInterSeq[Pi[k]];
#endif
		if (v == DummyValue)
		{}
		else
		{
#ifdef DEBUG_INTL
			*(VpOutSeq+OutIdx) = v;
#else
			pOutMtr[(Rate - 1) * D + OutIdx] = v;
#endif
			OutIdx++;
		}
	}

#ifdef DEBUG_INTL
	delete[] pInterSeq;
	delete[] Pi;
#endif
}


template<typename T>
#ifdef DEBUG_INTL
static void SubblockDeInterleaving(int SeqLen, T **pInpMtr, T **pOutMtr)
#else
static void SubblockDeInterleaving(int SeqLen, T pInpMtr[], T pOutMtr[])
#endif
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
	
#ifdef DEBUG_INTL
	R_sb = (int)(ceil((((float)D) / ((float)C_sb))));
#else
	R_sb = (D + (C_sb - 1)) / C_sb;
#endif
	
	K_pi = R_sb * C_sb;
	NumDummy = K_pi - D;
	DummyValue = (T)1000000;
	
//////////////////// DeInterleaving for i=0,1 ///////////////////////
#ifdef DEBUG_INTL
	T **pInterMatrix=new T*[R_sb];
	for(int r=0;r<R_sb;r++){*(pInterMatrix+r)=new T[C_sb];}
#else
	T pInterMatrix[((BLOCK_SIZE + 4 + 31) / 32) * 32];
#endif

#ifdef DEBUG_INTL
	T *VpInpSeq;
	T *VpOutSeq;
#endif
	
	for (int StrIdx = 0; StrIdx < (Rate - 1); StrIdx++)
	{
#ifdef DEBUG_INTL
		VpInpSeq=*(pInpMtr+StrIdx);
		VpOutSeq=*(pOutMtr+StrIdx);
#endif

		InIdx=0;
		for (int r = 0;r < R_sb; r++)
		{
			for (int c = 0; c < C_sb; c++)
			{
				int k = r * C_sb + c;
				
				if (k < NumDummy)
				{
#ifdef DEBUG_INTL
					*(*(pInterMatrix+r)+c)=DummyValue;
#else
					pInterMatrix[r * C_sb + c] = DummyValue;
#endif
				}
				else
				{
#ifdef DEBUG_INTL
					*(*(pInterMatrix+r)+c)=(T)0;
#else
					pInterMatrix[r * C_sb + c] = (T)0;
#endif
				}
			}
		}

		for (int c = 0; c < C_sb; c++)
		{
			int col = InterColumnPattern[c];
			for (int r = 0; r < R_sb; r++)
			{
				int k = col * R_sb + r;
#ifdef DEBUG_INTL
				T v = *(*(pInterMatrix+r)+col);
#else
				T v = pInterMatrix[r * C_sb + col];
#endif
				if (v == DummyValue)
				{}
				else
				{
#ifdef DEBUG_INTL
					*(*(pInterMatrix+r)+col)=*(VpInpSeq+InIdx);
#else
					pInterMatrix[r * C_sb + col] = pInpMtr[StrIdx * D + InIdx];
#endif
					InIdx++;
				}  
			}
		}  

		OutIdx=0;
		for (int r = 0; r < R_sb; r++)
		{
			for(int c = 0; c < C_sb; c++)
			{
#ifdef DEBUG_INTL
				T v = *(*(pInterMatrix+r)+c);
#else
				T v = pInterMatrix[r * C_sb + c];
#endif
				if (v == DummyValue)
				{}
				else
				{
#ifdef DEBUG_INTL
					*(VpOutSeq+OutIdx)=*(*(pInterMatrix+r)+c);
#else
					pOutMtr[StrIdx * D + OutIdx] = pInterMatrix[r * C_sb + c];
#endif
					OutIdx++;
				}
			}
		}
	}
#ifdef DEBUG_INTL
	for(int r=0;r<R_sb;r++){delete[] *(pInterMatrix+r);}
	delete[] pInterMatrix;
#endif

//////////////////// DeInterleaving for i=2 ///////////////////////
#ifdef DEBUG_INTL	
	int *Pi=new int[K_pi];
	T *pInterSeq=new T[K_pi];
	VpInpSeq=*(pInpMtr+(Rate-1));
	VpOutSeq=*(pOutMtr+(Rate-1));
#else
	int Pi[((BLOCK_SIZE + 4 + 31) / 32) * 32];
	T pInterSeq[((BLOCK_SIZE + 4 + 31) / 32) * 32];
#endif
	
	for (int k = 0; k < NumDummy; k++)
		pInterSeq[k] = DummyValue;
//////////////// Pi & DePi//////////////////
	for(int k=0;k<K_pi;k++)
	{
#ifdef DEBUG_INTL
		int idxP=(int)(floor((((double)(k))/((double)(R_sb)))));
#else
		int idxP = k / R_sb;
#endif
		int idx = (InterColumnPattern[idxP] + (C_sb * (k % R_sb)) + 1) % K_pi;
		Pi[k]=idx;
	}
/////////////// DeInterleaving ////////////////////
	InIdx=0;
	for(int k=0;k<K_pi;k++)
	{
		T v = pInterSeq[Pi[k]];
		if (v == DummyValue)
		{}
		else
		{
#ifdef DEBUG_INTL	
			*(pInterSeq+(*(Pi+k)))=*(VpInpSeq+InIdx);
#else
			pInterSeq[Pi[k]] = pInpMtr[(Rate - 1) * D + InIdx];
#endif
			InIdx++;
		}
	}
	OutIdx=0;
	for (int k = NumDummy; k < K_pi; k++)
	{
#ifdef DEBUG_INTL
		*(VpOutSeq+OutIdx)=*(pInterSeq+k);
#else
		pOutMtr[(Rate - 1) * D + OutIdx] = pInterSeq[k];
#endif
		OutIdx++;
	}

#ifdef DEBUG_INTL
	delete[] pInterSeq;
	delete[] Pi;
#endif
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

	#ifdef DEBUG_INTL
	int **pInMatrix = new int*[RATE];
	int **pOutMatrix = new int*[RATE];
	#else
	int pInMatrix[RATE * (BLOCK_SIZE + 4)];
	int pOutMatrix[RATE * (BLOCK_SIZE + 4)];
	#endif

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
	
#ifdef DEBUG_INTL
	for (r = 0; r < RATE; r++)
	{
		*(pInMatrix + r) = new int[BLOCK_SIZE + 4];
		*(pOutMatrix + r) = new int[BLOCK_SIZE + 4];
	}
#endif

	out_block_offset = 0;
	for (i = 0; i < n_blocks; i++)
	{
		cur_blk_len = (i != (n_blocks - 1)) ? rm_blk_sz : rm_last_blk_len;

		// Transposition
		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{
#ifdef DEBUG_INTL
				pInMatrix[r][j] = piSeq[out_block_offset + RATE * j + r];
#else
				pInMatrix[r * cur_blk_len + j] = piSeq[out_block_offset + RATE * j + r];
#endif
				//	cout << pInMatrix[r][i];
			}
		}
		//cout << endl;
			
		SubblockInterleaving(cur_blk_len, pInMatrix, pOutMatrix);
		
		// Transposition again
		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{
#ifdef DEBUG_INTL
				pcSeq[out_block_offset + RATE * j + r] = pOutMatrix[r][j];
#else
				pcSeq[out_block_offset + RATE * j + r] = pOutMatrix[r * cur_blk_len + j];
#endif
			}
		}

		out_block_offset += RATE * cur_blk_len;
	}

	n_extra_bits = out_buf_sz - in_buf_sz;
	for (i = 0; i < n_extra_bits; i++)
	{
		pcSeq[in_buf_sz + i] = 0;
	}

#ifdef DEBUG_INTL
	for (r = 0; r < RATE; r++)
	{
		delete[] pInMatrix[r];
		delete[] pOutMatrix[r];
	}
	delete[] pInMatrix;
	delete[] pOutMatrix;
#endif
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
	
#ifdef DEBUG_INTL
	float **pInMatrix = new float*[RATE];
	float **pOutMatrix = new float*[RATE];
#else
	float pInMatrix[RATE * (BLOCK_SIZE + 4)];
	float pOutMatrix[RATE * (BLOCK_SIZE + 4)];
#endif

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

#ifdef DEBUG_INTL
	for(r = 0; r < RATE; r++)
	{
		*(pInMatrix + r) = new float[BLOCK_SIZE + 4];
		*(pOutMatrix + r) = new float[BLOCK_SIZE + 4];
	}
#endif

	out_block_offset = 0;
	for (i = 0; i < n_blocks; i++)
	{
		cur_blk_len = (i != (n_blocks - 1)) ? rm_blk_sz : rm_last_blk_len;
		
		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{
#ifdef DEBUG_INTL
				*(*(pInMatrix + r) + j) = *(pLLRin + out_block_offset + RATE * j + r);
#else
				pInMatrix[r * cur_blk_len + j] = pLLRin[out_block_offset + RATE * j + r];
#endif
			}
		}  

		SubblockDeInterleaving(cur_blk_len, pInMatrix, pOutMatrix);

		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{
#ifdef DEBUG_INTL
				*(pLLRin + out_block_offset + RATE * j + r) = *(*(pOutMatrix + r) + j);
				#else
				pLLRin[out_block_offset + RATE * j + r] = pOutMatrix[r * cur_blk_len + j];
				#endif
			}
		}

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

	for (i = 0; i < out_buf_sz; i++)
	{
		pLLRout[i] = pLLRin[i];
	}

#ifdef DEBUG_INTL
	for (r = 0; r < RATE; r++)
	{
		delete[] *(pInMatrix + r);
		delete[] *(pOutMatrix + r);
	}
	delete[] pInMatrix;
	delete[] pOutMatrix;
#endif
}

