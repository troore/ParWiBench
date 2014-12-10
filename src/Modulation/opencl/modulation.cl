
//#include "../../lte_phy.h"
#define MAX_MOD_BITS_PER_SAMP 6
#define MAX_MOD_TABLE_LEN  64

 vecmin(float* pV, int len)
{
	float minValue = *pV;
	int i;
	
	for (i = 1; i < len; i++)
	{
		if (*(pV + i) < minValue)
		{
			minValue=*(pV + i);
		}
	}
	
	return minValue;
}

//typedef float (*p_a)[2];

__kernel void modulation(__global int *pBitsSeq, __global float *pModedSeq, int bits_per_samp, int n,__global float *t, int mod_table_len)
{
	int task_id = get_global_id(0);
	if (task_id < n)
	{
		int idx = 0;
		for (int b = 0; b < bits_per_samp; b++)
		{
			idx += pBitsSeq[task_id * bits_per_samp + b] * pow(2.0, (float)(bits_per_samp - 1 - b));
		}
		//pModedSeq[task_id] = idx*1.0;
		//pModedSeq[task_id+n] = -idx*1.0;
		pModedSeq[task_id] = t[idx*2];
		pModedSeq[task_id + n ] = t[idx*2+1];
		//pModedSeq[task_id] = std::complex<float>(I, Q);
	}
}

__kernel void demodulation(__global float *pDecSeq,int bits_per_samp, int in_buf_sz,__global float *p_table,int mod_table_len, __global float *idx_table, __global float *pLLR, float No)
{
	int task_id = get_global_id(0);
	int i, j, k;
	float metric[MAX_MOD_TABLE_LEN];
	float metric_set[2][(MAX_MOD_TABLE_LEN / 2)];
	//float No = 2.0 * (pow(awgnSigma, 2.0));

	if (task_id < in_buf_sz){
		for(j = 0; j < mod_table_len; j++)
		{
			//metric[j] = pow(abs((pDecSeq[task_id] - (std::complex<float>(p_table[j][0], p_table[j][1])))), 2.0);
			metric[j] = pow(pDecSeq[task_id]-p_table[j * 2 ],2.0)  + pow(pDecSeq[task_id+in_buf_sz]-p_table[j * 2 + 1],2.0);
		}	
		for (j = 0; j < bits_per_samp; j++)
		{
			float min0, min1;
			int idx0 = 0, idx1 = 0;

			for (k = 0; k < mod_table_len; k++)
			{
				if(idx_table[k * MAX_MOD_BITS_PER_SAMP + j] == 0)
				{
					metric_set[0][idx0] = metric[k];
					idx0++;
				}
				else
				{
					metric_set[1][idx1] = metric[k];
					idx1++;
				}
			}

		//	min0 = vecmin(metric_set[0], mod_table_len / 2);
		//	min1  = vecmin(metric_set[1], mod_table_len / 2);
			min0 = metric_set[0][0];
			min1 = metric_set[1][0];
			for(k = 1; k < mod_table_len/2; k++)
			{
				min0 = fmin(metric_set[0][k],min0);
			}
			for(k = 1; k < mod_table_len/2;k++)
			{
				min1 = fmin(metric_set[1][k],min1);
			}
			if (No == (float)0)
			{
				pLLR[task_id * bits_per_samp + j] = (min0 - min1);
			}
			else
			{
				pLLR[task_id * bits_per_samp + j] = (min0 - min1) / No;
			//	pLLR[task_id * bits_per_samp+j] = 1.1;
			}
		}
	}
}
		
