#include <omp.h>
#include "Modulation.h"
#include "util.h"

static const float INF = 1.0e9;

float BPSK_table[2][2]; float QPSK_table[4][2];
float QAM16_table[16][2];
float QAM64_table[64][2];

static void init_mod_tables()
{
	int i, j;
	
	// BPSK
	BPSK_table[0][0] = +1.0; BPSK_table[0][1] = +1.0;
	BPSK_table[1][0] = -1.0; BPSK_table[1][1] = -1.0;
	
	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < 2; j++)
		{
			BPSK_table[i][j] /= sqrt(2.0);
		}
	}

	// QPSK
	QPSK_table[0][0] = +1.0; QPSK_table[0][1] = +1.0;
	QPSK_table[1][0] = +1.0; QPSK_table[1][1] = -1.0;
	QPSK_table[2][0] = -1.0; QPSK_table[2][1] = +1.0;
	QPSK_table[3][0] = -1.0; QPSK_table[3][1] = -1.0;
	
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 2; j++)
		{
			QPSK_table[i][j] /= sqrt(2.0);
		}
	}

	// QAM16
	QAM16_table[0][0] = +1.0; QAM16_table[0][1] = +1.0;
	QAM16_table[1][0] = +1.0; QAM16_table[1][1] = +3.0;
	QAM16_table[2][0] = +3.0; QAM16_table[2][1] = +1.0;
	QAM16_table[3][0] = +3.0; QAM16_table[3][1] = +3.0;
	QAM16_table[4][0] = +1.0; QAM16_table[4][1] = -1.0;
	QAM16_table[5][0] = +1.0; QAM16_table[5][1] = -3.0;
	QAM16_table[6][0] = +3.0; QAM16_table[6][1] = -1.0;
	QAM16_table[7][0] = +3.0; QAM16_table[7][1] = -3.0;
	QAM16_table[8][0] = -1.0; QAM16_table[8][1] = +1.0;
	QAM16_table[9][0] = -1.0; QAM16_table[9][1] = +3.0;
	QAM16_table[10][0] = -3.0; QAM16_table[10][1] = +1.0;
	QAM16_table[11][0] = -3.0; QAM16_table[11][1] = +3.0;
	QAM16_table[12][0] = -1.0; QAM16_table[12][1] = -1.0;
	QAM16_table[13][0] = -1.0; QAM16_table[13][1] = -3.0;
	QAM16_table[14][0] = -3.0; QAM16_table[14][1] = -1.0;
	QAM16_table[15][0] = -3.0; QAM16_table[15][1] = -3.0;
	
	for (i = 0; i < 16; i++)
	{
		for (j = 0; j < 2; j++)
		{
			QAM16_table[i][j] /= sqrt(10.0);
		}
	}

	// QAM64
	QAM64_table[0][0] = +3.0; QAM64_table[0][1] = +3.0;
	QAM64_table[1][0] = +3.0; QAM64_table[1][1] = +1.0;
	QAM64_table[2][0] = +1.0; QAM64_table[2][1] = +3.0;
	QAM64_table[3][0] = +1.0; QAM64_table[3][1] = +1.0;
	QAM64_table[4][0] = +3.0; QAM64_table[4][1] = +5.0;
	QAM64_table[5][0] = +3.0; QAM64_table[5][1] = +7.0;
	QAM64_table[6][0] = +1.0; QAM64_table[6][1] = +5.0;
	QAM64_table[7][0] = +1.0; QAM64_table[7][1] = +7.0;

	QAM64_table[8][0] = +5.0; QAM64_table[8][1] = +3.0;
	QAM64_table[9][0] = +5.0; QAM64_table[9][1] = +1.0;
	QAM64_table[10][0] = +7.0; QAM64_table[10][1] = +3.0;
	QAM64_table[11][0] = +7.0; QAM64_table[11][1] = +1.0;
	QAM64_table[12][0] = +5.0; QAM64_table[12][1] = +5.0;
	QAM64_table[13][0] = +5.0; QAM64_table[13][1] = +7.0;
	QAM64_table[14][0] = +7.0; QAM64_table[14][1] = +5.0;
	QAM64_table[15][0] = +7.0; QAM64_table[15][1] = +7.0;

	QAM64_table[16][0] = +3.0; QAM64_table[16][1] = -3.0;
	QAM64_table[17][0] = +3.0; QAM64_table[17][1] = -1.0;
	QAM64_table[18][0] = +1.0; QAM64_table[18][1] = -3.0;
	QAM64_table[19][0] = +1.0; QAM64_table[19][1] = -1.0;
	QAM64_table[20][0] = +3.0; QAM64_table[20][1] = -5.0;
	QAM64_table[21][0] = +3.0; QAM64_table[21][1] = -7.0;
	QAM64_table[22][0] = +1.0; QAM64_table[22][1] = -5.0;
	QAM64_table[23][0] = +1.0; QAM64_table[23][1] = -7.0;

	QAM64_table[24][0] = +5.0; QAM64_table[24][1] = -3.0;
	QAM64_table[25][0] = +5.0; QAM64_table[25][1] = -1.0;
	QAM64_table[26][0] = +7.0; QAM64_table[26][1] = -3.0;
	QAM64_table[27][0] = +7.0; QAM64_table[27][1] = -1.0;
	QAM64_table[28][0] = +5.0; QAM64_table[28][1] = -5.0;
	QAM64_table[29][0] = +5.0; QAM64_table[29][1] = -7.0;
	QAM64_table[30][0] = +7.0; QAM64_table[30][1] = -5.0;
	QAM64_table[31][0] = +7.0; QAM64_table[31][1] = -7.0;

	QAM64_table[32][0] = -3.0; QAM64_table[32][1] = +3.0;
	QAM64_table[33][0] = -3.0; QAM64_table[33][1] = +1.0;
	QAM64_table[34][0] = -1.0; QAM64_table[34][1] = +3.0;
	QAM64_table[35][0] = -1.0; QAM64_table[35][1] = +1.0;
	QAM64_table[36][0] = -3.0; QAM64_table[36][1] = +5.0;
	QAM64_table[37][0] = -3.0; QAM64_table[37][1] = +7.0;
	QAM64_table[38][0] = -1.0; QAM64_table[38][1] = +5.0;
	QAM64_table[39][0] = -1.0; QAM64_table[39][1] = +7.0;

	QAM64_table[40][0] = -5.0; QAM64_table[40][1] = +3.0;
	QAM64_table[41][0] = -5.0; QAM64_table[41][1] = +1.0;
	QAM64_table[42][0] = -7.0; QAM64_table[42][1] = +3.0;
	QAM64_table[43][0] = -7.0; QAM64_table[43][1] = +1.0;
	QAM64_table[44][0] = -5.0; QAM64_table[44][1] = +5.0;
	QAM64_table[45][0] = -5.0; QAM64_table[45][1] = +7.0;
	QAM64_table[46][0] = -7.0; QAM64_table[46][1] = +5.0;
	QAM64_table[47][0] = -7.0; QAM64_table[47][1] = +7.0;

	QAM64_table[48][0] = -3.0; QAM64_table[48][1] = -3.0;
	QAM64_table[49][0] = -3.0; QAM64_table[49][1] = -1.0;
	QAM64_table[50][0] = -1.0; QAM64_table[50][1] = -3.0;
	QAM64_table[51][0] = -1.0; QAM64_table[51][1] = -1.0;
	QAM64_table[52][0] = -3.0; QAM64_table[52][1] = -5.0;
	QAM64_table[53][0] = -3.0; QAM64_table[53][1] = -7.0;
	QAM64_table[54][0] = -1.0; QAM64_table[54][1] = -5.0;
	QAM64_table[55][0] = -1.0; QAM64_table[55][1] = -7.0;

	QAM64_table[56][0] = -5.0; QAM64_table[56][1] = -3.0;
	QAM64_table[57][0] = -5.0; QAM64_table[57][1] = -1.0;
	QAM64_table[58][0] = -7.0; QAM64_table[58][1] = -3.0;
	QAM64_table[59][0] = -7.0; QAM64_table[59][1] = -1.0;
	QAM64_table[60][0] = -5.0; QAM64_table[60][1] = -5.0;
	QAM64_table[61][0] = -5.0; QAM64_table[61][1] = -7.0;
	QAM64_table[62][0] = -7.0; QAM64_table[62][1] = -5.0;
	QAM64_table[63][0] = -7.0; QAM64_table[63][1] = -7.0;
	
	for (i = 0; i < 64; i++)
	{
		for (j = 0; j < 2; j++)
		{
			QAM64_table[i][j] /= sqrt(42.0);
		}
	}
}

/*
 * decimal to bit sequence
 * i: decimal
 * n: width of bit sequence
 * bvec: vector to contain the sequence
 */
static void dec2bits(int i, int n, int *bvec)
{
	int j;
	
	for (j = 0; j < n; j++)
	{
		bvec[n - j - 1] = (i & 1);
		i = (i >> 1);
	}
}

static void set_mod_params(/*float (*mod_table)[2]*/ p_a *pp_table, int *bits_per_samp, int *mod_table_len, int mod_type)
{
	init_mod_tables();

//	int (*p)[2];

//	p = (int (*)[2])0x1;
	
	switch(mod_type)
	{
	case LTE_BPSK:
		*pp_table = &BPSK_table[0];
		*bits_per_samp = BPSK_BITS_PER_SAMP;
		*mod_table_len = BPSK_TABLE_LEN;
		break;
	case LTE_QPSK:
		*pp_table = &QPSK_table[0];
		*bits_per_samp = QPSK_BITS_PER_SAMP;
		*mod_table_len = QPSK_TABLE_LEN;
		break;
	case LTE_QAM16:
		*pp_table = &QAM16_table[0];
//		*pp_table = (float (*)[2])0x1;
		*bits_per_samp = QAM16_BITS_PER_SAMP;
		*mod_table_len = QAM16_TABLE_LEN;
		break;
	case LTE_QAM64:
		*pp_table = &QAM64_table[0];
		*bits_per_samp = QAM64_BITS_PER_SAMP;
		*mod_table_len = QAM64_TABLE_LEN;
		break;
	default:
		std::cout << "Invalid modulation type!" << std::endl;
		// TODO: Exception processing
		break;
	}
}

void Modulating(LTE_PHY_PARAMS *lte_phy_params, int *pBitsSeq, std::complex<float> *pModedSeq, int mod_type)
{
	float I, Q;
	float (*p_table)[2];
	int bits_per_samp;
	int mod_table_len;
	int n_samp, b, idx;
	int in_buf_sz;

	in_buf_sz = lte_phy_params->mod_in_buf_sz;

	set_mod_params(&p_table, &bits_per_samp, &mod_table_len, mod_type);

	for (n_samp = 0; n_samp < (in_buf_sz / bits_per_samp); n_samp++)
	{
		idx = 0;
		for (b = 0; b < bits_per_samp; b++)
		{
			idx += pBitsSeq[n_samp * bits_per_samp + b] * pow(2.0, (float)(bits_per_samp - 1 - b));
		}
		I = p_table[idx][0];
		Q = p_table[idx][1];
		
		pModedSeq[n_samp] = std::complex<float>(I, Q);
	}
}

void Modulating(LTE_PHY_PARAMS *lte_phy_params, int *pBitsSeq, float *pModedSeq, int mod_type)
{
	float I, Q;
	float (*p_table)[2];
	int bits_per_samp;
	int mod_table_len;
	int n_samp, b, idx;
	int in_buf_sz;
	int out_buf_sz;

	in_buf_sz = lte_phy_params->mod_in_buf_sz;
	out_buf_sz = lte_phy_params->mod_out_buf_sz;

	set_mod_params(&p_table, &bits_per_samp, &mod_table_len, mod_type);

	for (n_samp = 0; n_samp < (in_buf_sz / bits_per_samp); n_samp++)
	{
		idx = 0;
		for (b = 0; b < bits_per_samp; b++)
		{
			idx += pBitsSeq[n_samp * bits_per_samp + b] * /* pow(2.0, (float)(bits_per_samp - 1 - b)) */ (1 << (bits_per_samp - 1 - b));
		}
		I = p_table[idx][0];
		Q = p_table[idx][1];
		
		pModedSeq[n_samp] = I;
		pModedSeq[n_samp + out_buf_sz] = Q;
	}
}

static float vecmin(float* pV, int len)
{
	float minValue = INF;
	int i;
	
	for (i = 0; i < len; i++)
	{
		if (*(pV + i) < minValue)
		{
			minValue=*(pV + i);
		}
		else
		{}
	}
	
	return minValue;
}

/*
 * Euclidean distance of two complex values: a+bi and c+di
 */
static float eudist(float a, float b, float c, float d)
{
	return sqrt((a - c) * (a - c) + (b - d) * (b - d));
}

void Demodulating(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pDecSeq, int *pHD, int mod_type)
{
	float dist, mindist;
	int closest;

	float (*p_table)[2];
	int bits_per_samp;
	int mod_table_len;
		
	int bitmap[MAX_MOD_TABLE_LEN * MAX_MOD_BITS_PER_SAMP];
	int bvec[MAX_MOD_BITS_PER_SAMP];
	
	int in_buf_sz;

	int i, j;

	in_buf_sz = lte_phy_params->demod_in_buf_sz;

	set_mod_params(&p_table, &bits_per_samp, &mod_table_len, mod_type);

	/* set bitmap */
	for (i = 0; i < mod_table_len; i++)
	{
		dec2bits(i, bits_per_samp, bvec);
		for (j = 0; j < bits_per_samp; j++)
		{
			bitmap[i * bits_per_samp + j] = bvec[j];
		}
	}
	/* set bitmap over... */


	/* Start demodulation */
	for (i = 0; i < in_buf_sz; i++)
	{
		mindist = eudist(p_table[0][0], p_table[0][1], std::real(pDecSeq[i]), std::imag(pDecSeq[i]));
		
		closest = 0;
		for (j = 1; j < mod_table_len; j++)
		{
			//	dist = std::abs(symbols(j) - signal(i));
			dist = eudist(p_table[j][0], p_table[j][1], std::real(pDecSeq[i]), std::imag(pDecSeq[i]));
			if (dist < mindist)
			{
				mindist = dist;
				closest = j;
			}
		}
		for (j = 0; j < bits_per_samp; j++)
		{
			pHD[i * bits_per_samp + j] = bitmap[closest * bits_per_samp + j];
		}
	}

	/* End demodulation*/
}

void Demodulating(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pDecSeq, float *pLLR, int mod_type, float awgnSigma)
{

	float No = 2.0 * (pow(awgnSigma, 2.0));
	float (*p_table)[2];
	int bits_per_samp;
	int mod_table_len;

	int idx_table[MAX_MOD_TABLE_LEN][MAX_MOD_BITS_PER_SAMP];
	float metric[MAX_MOD_TABLE_LEN];
	float metric_set[2][(MAX_MOD_TABLE_LEN / 2)];

	int in_buf_sz;

	int i, j, k;

	in_buf_sz = lte_phy_params->demod_in_buf_sz;

	set_mod_params(&p_table, &bits_per_samp, &mod_table_len, mod_type);
	
	for (i = 0; i < mod_table_len; i++)
	{
		for (j = 0; j < bits_per_samp; j++)
		{
			idx_table[i][j] = 0;
		}
	}
	for (i = 0; i < mod_table_len; i++)
	{ 
 
		int idx_val = i;
		int b = bits_per_samp - 1;
		
		while (idx_val)
		{
			idx_table[i][b] = idx_val % 2;
			idx_val /= 2;
			b--;
		}
	}
	
	for (i = 0; i < in_buf_sz; i++)
	{
		for(j = 0; j < mod_table_len; j++)
		{
			metric[j] = pow(abs((pDecSeq[i] - (std::complex<float>(p_table[j][0], p_table[j][1])))), 2.0);
		}

		for (j = 0; j < bits_per_samp; j++)
		{
			float min0, min1;
			int idx[2]={0,0};
		       

			for (k = 0; k < mod_table_len; k++)
			{
				
					metric_set[idx_table[k][j]][idx[idx_table[k][j]]] = metric[k];
					idx[idx_table[k][j]]++;
			}

			min0 = vecmin(metric_set[0], mod_table_len / 2);
			min1  = vecmin(metric_set[1], mod_table_len / 2);

			if (No == (float)0)
			{
				pLLR[i * bits_per_samp + j] = (min0 - min1);
			}
			else
			{
				pLLR[i * bits_per_samp + j] = (min0 - min1) / No;
			}
		}
	}
}


void Demodulating(LTE_PHY_PARAMS *lte_phy_params, float *pDecSeq, float *pLLR, int mod_type, float awgnSigma)
{

	float No = 2.0 * (pow(awgnSigma, 2.0));
	float (*p_table)[2];
	int bits_per_samp;
	int mod_table_len;

	int idx_table[MAX_MOD_TABLE_LEN][MAX_MOD_BITS_PER_SAMP];
	float metric[MAX_MOD_TABLE_LEN];
	float metric_set[2][(MAX_MOD_TABLE_LEN / 2)];

	int in_buf_sz;
//	int out_buf_sz;

	int i, j, k;

	in_buf_sz = lte_phy_params->demod_in_buf_sz;
//	out_buf_sz = lte_phy_params->demod_out_buf_sz;

	set_mod_params(&p_table, &bits_per_samp, &mod_table_len, mod_type);
	
	for (i = 0; i < mod_table_len; i++)
	{
		for (j = 0; j < bits_per_samp; j++)
		{
			idx_table[i][j] = 0;
		}
	}
	for (i = 0; i < mod_table_len; i++)
	{ 
 
		int idx_val = i;
		int b = bits_per_samp - 1;
		
		while (idx_val)
		{
			idx_table[i][b] = idx_val % 2;
			idx_val /= 2;
			b--;
		}
	}

	omp_set_num_threads(num_threads);
#pragma omp parallel
	{
		//	printf("tid=%d\n", omp_get_thread_num());
#pragma omp for private(i, j, k, metric, metric_set)
		for (i = 0; i < in_buf_sz; i++)
		{
//			#pragma omp barrier
			for(j = 0; j < mod_table_len; j++)
			{
				float tmp[2];
				//	metric[j] = pow(abs((pDecSeq[i] - (std::complex<float>(p_table[j][0], p_table[j][1])))), 2.0);
				tmp[0] = pDecSeq[i] - p_table[j][0];
				tmp[1] = pDecSeq[i + in_buf_sz] - p_table[j][1];
				metric[j] = tmp[0] * tmp[0] + tmp[1] * tmp[1];
			}

			for (j = 0; j < bits_per_samp; j++)
			{
				float min0, min1;
				int idx[2]={0,0};

				for (k = 0; k < mod_table_len; k++)
				{
				
					metric_set[idx_table[k][j]][idx[idx_table[k][j]]] = metric[k];
					idx[idx_table[k][j]]++;
				}

				min0 = vecmin(metric_set[0], mod_table_len / 2);
				min1 = vecmin(metric_set[1], mod_table_len / 2);

				if (No == (float)0)
				{
					pLLR[i * bits_per_samp + j] = (min0 - min1);
				}
				else
				{
					pLLR[i * bits_per_samp + j] = (min0 - min1) / No;
				}
			}
		}
	}
}

