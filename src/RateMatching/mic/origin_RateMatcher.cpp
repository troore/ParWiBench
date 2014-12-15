#include <sys/time.h>
#include "RateMatcher.h"

//SubblockInterleaver_lte<int,int> SbInterleaver;
//SubblockInterleaver_lte<int,float> SbDeInterleaver;
void _SubblockInterleaving(int SeqLen, int *InSeq, int *OutSeq, int offset);

double ddtime();

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
//	printf("%d\n",n_blocks);
	out_block_offset = 0;
	for (i = 0; i < n_blocks; i++)
	{
		cur_blk_len = (i != (n_blocks - 1)) ? rm_blk_sz : rm_last_blk_len;

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
//		_SubblockInterleaving(cur_blk_len, piSeq, pcSeq, out_block_offset);
//		double ttime,tbegin;
//		tbegin=ddtime();
		SubblockInterleaving(cur_blk_len, pInMatrix, pOutMatrix);
//		ttime = ddtime();
//		printf("Sub time is %f\n",ttime - tbegin);
		// Transposition again
		for (j = 0; j < cur_blk_len; j++)
		{
			//printf("old\n");
			for (r = 0; r < RATE; r++)
			{
				pcSeq[out_block_offset + RATE * j + r] = pOutMatrix[r * cur_blk_len + j];
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


double ddtime()
{
	double tseconds = 0.0;
	struct timeval mytime;
	gettimeofday(&mytime,(struct timezone*)0);
	tseconds = (double)(mytime.tv_sec +
			mytime.tv_usec*1.0e-6);
	return( tseconds );
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
	//printf("out_buf_sz = %d, n_blocks = %d, cur_blk_len = %d, RATE = %d\n",out_buf_sz, n_blocks,cur_blk_len,RATE);
	for (i = 0; i < n_blocks; i++)
	{
		cur_blk_len = (i != (n_blocks - 1)) ? rm_blk_sz : rm_last_blk_len;
		//printf("n_blocks = %d, cur_blk_len = %d, RATE = %d\n",n_blocks,cur_blk_len,RATE);
		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{

				pInMatrix[r * cur_blk_len + j] = pLLRin[out_block_offset + RATE * j + r];

			}
		}  
		double ttime,tbegin;
		tbegin = ddtime();
		SubblockDeInterleaving(cur_blk_len, pInMatrix, pOutMatrix);
		ttime = ddtime() - tbegin;
		//printf("Sub time is %f \n", ttime);
		for (j = 0; j < cur_blk_len; j++)
		{
			for (r = 0; r < RATE; r++)
			{
				
				pLLRin[out_block_offset + RATE * j + r] = pOutMatrix[r * cur_blk_len + j];
				
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

