
#include <stdio.h>
#include <cstring>
#include <limits>

#include "ParTurbo.h"

#include <time.h>

#include "CL/cl.h"
#include "clutil.h"
#include "meas.h"

#define PROGRAM_FILE	"./parturbo.ocl"
#define KERNEL_FUNC	"par_log_decoder_kernel"

#define NUM_KERNELS	2

float recv_syst1[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
float recv_syst2[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
float recv_parity1[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
float recv_parity2[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
	
float Le12[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
float Le12_int[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
float Le21[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
float Le21_int[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
float L[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];

float old_alpha1[N_STATES * MAX_SUBBLOCKS_NUM];
float old_beta1[N_STATES * MAX_SUBBLOCKS_NUM];
float new_alpha1[N_STATES * MAX_SUBBLOCKS_NUM];
float new_beta1[N_STATES * MAX_SUBBLOCKS_NUM];
	
float old_alpha2[N_STATES * MAX_SUBBLOCKS_NUM];
float old_beta2[N_STATES * MAX_SUBBLOCKS_NUM];
float new_alpha2[N_STATES * MAX_SUBBLOCKS_NUM];
float new_beta2[N_STATES * MAX_SUBBLOCKS_NUM];

#define MAX_SUBBLOCK_ALPHA_LEN_NORMAL	((BLOCK_SIZE / NUM_SUBBLOCKS_PER_BLOCK) + 1)
#define MAX_SUBBLOCK_ALPHA_LEN_EXTRA	((BLOCK_SIZE / NUM_SUBBLOCKS_PER_BLOCK) + N_TAIL + 1)

// local memory for log_decoder_kernel
float l_alpha[N_STATES * MAX_SUBBLOCK_ALPHA_LEN_EXTRA];
float l_beta[N_STATES * MAX_SUBBLOCK_ALPHA_LEN_EXTRA];
float l_gamma[N_STATES * 2 * MAX_SUBBLOCK_ALPHA_LEN_EXTRA];
float l_denom[MAX_SUBBLOCK_ALPHA_LEN_EXTRA];
float l_apriori[MAX_SUBBLOCK_ALPHA_LEN_EXTRA - 1];

#define MAX_ALPHA_LEN	((BLOCK_SIZE / NUM_SUBBLOCKS_PER_BLOCK) + 1) * MAX_SUBBLOCKS_NUM + (MAX_SUBBLOCKS_NUM / NUM_SUBBLOCKS_PER_BLOCK) * N_TAIL

float alpha[];

void par_turbo_decoding(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, int *pOutBits, int n_sfr, int n_iters)
{
	int out_fr_len;
	int blk_sz;
	int n_blk;
	int subblk_sz;
	int n_subblk;
	int n_subblk_per_blk;
	int i, j, k;

	int cur_blk_len;
	int out_bit_offset;
	int syst_block_offset, parity_block_offset;

	/*
	  float recv_syst1[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
	  float recv_syst2[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
	  float recv_parity1[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
	  float recv_parity2[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
	*/

	/*
	 * FIXME: number of sub-blocks is an important parameter for this parallel version 
	 * */
	n_subblk_per_blk = NUM_SUBBLOCKS_PER_BLOCK;

	subblk_sz = BLOCK_SIZE / n_subblk_per_blk;	// tailed bits shouldn't be ignored
	out_fr_len = lte_phy_params->td_out_buf_sz;	// output length of a codeword after turbo decoding
	n_blk = n_sfr * ((out_fr_len + BLOCK_SIZE - 1) / BLOCK_SIZE);
	blk_sz = BLOCK_SIZE;
	n_subblk = n_sfr * ((out_fr_len + subblk_sz - 1) / subblk_sz);	// number of subblocks

	set_generator_polynomials(g_gens, N_GENS, CST_LEN);

	out_bit_offset = 0;
	syst_block_offset = 0;
	parity_block_offset = 0;
	for (i = 0; i < n_blk; i++)
	{
		/*
		 * FIXME: for simplicity, lengths of blocks are same with each other for now
		 * */
		cur_blk_len = BLOCK_SIZE;

		// data part
		for (j = 0; j < cur_blk_len; j++)
		{
			recv_syst1[syst_block_offset + j] = pInpData[out_bit_offset++];
			//	printf("%.0f", recv_syst1[syst_block_offset + j]);
			for (k = 0; k < N_GENS - 1; k++)
			{
				recv_parity1[parity_block_offset + j * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
				//	printf("%.0f", recv_parity1[parity_block_offset + j * (N_GENS - 1) + k]);
			}
			for (k = 0; k < N_GENS - 1; k++)
			{
				recv_parity2[parity_block_offset + j * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
				//	printf("%.0f", recv_parity2[parity_block_offset + j * (N_GENS - 1) + k]);
			}
		}
		//	printf("\n");
		// first tail
		for (j = 0; j < N_TAIL; j++)
		{
			recv_syst1[syst_block_offset + cur_blk_len + j] = pInpData[out_bit_offset++];
			//	printf("%.0f", recv_syst1[syst_block_offset + cur_blk_len + j]);
			for (k = 0; k < N_GENS - 1; k++)
			{
				recv_parity1[parity_block_offset + (cur_blk_len + j) * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
				//	printf("%.0f", recv_parity1[parity_block_offset + (cur_blk_len + j) * (N_GENS - 1) + k]);
			}
		}
		//	printf("\n");
		// second tail
		for (j = 0; j < N_TAIL; j++)
		{
			recv_syst2[syst_block_offset + cur_blk_len + j] = pInpData[out_bit_offset++];
			//	printf("%.0f", recv_syst2[syst_block_offset + cur_blk_len + j]);
			for (k = 0; k < N_GENS - 1; k++)
			{
				recv_parity2[parity_block_offset + (cur_blk_len + j) * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
				//	printf("%.0f", recv_parity2[parity_block_offset + (cur_blk_len + j) * (N_GENS - 1) + k]);
			}
		}
		//	printf("\n");
		
		syst_block_offset += (cur_blk_len + N_TAIL);
		parity_block_offset += (N_GENS - 1) * (cur_blk_len + N_TAIL);
	}

	#ifdef GOLD
	gold_decode_all_subblocks(recv_syst1, recv_syst2, recv_parity1, recv_parity2, 
							  subblk_sz, n_subblk, n_subblk_per_blk, pOutBits, n_iters);
	#else
	par_decode_all_subblocks(recv_syst1, recv_syst2, recv_parity1, recv_parity2, 
							 subblk_sz, n_subblk, n_subblk_per_blk, pOutBits, n_iters);
	#endif
}

template <typename T>
void par_interleaver(void (*pInterleaver)(T *, T *, int), T *in, T *out, 
					 int n_subblk, int subblk_sz, int n_subblk_per_blk)
{
	int stride;
	int subblk_offset = 0;
	int i;

	for (i = 0; i < n_subblk; i++)
	{
		pInterleaver(in + subblk_offset, out + subblk_offset, subblk_sz);

		if ((i % n_subblk_per_blk) == (n_subblk_per_blk - 1))
			stride = subblk_sz + N_TAIL;
		else
			stride = subblk_sz;

		subblk_offset += stride;
	}
}

template <typename T>
void par_interleaver(void (*pInterleaver)(T *, T *, int), T *in, T *out, int n_blk, int blk_sz)
{
	int subblk_offset = 0;
	int i;

	for (i = 0; i < n_blk; i++)
	{
		pInterleaver(in + i * (blk_sz + N_TAIL), out + i * (blk_sz + N_TAIL), blk_sz);
	}
}

void gold_decode_all_subblocks(float *recv_syst1, 
							  float *recv_syst2,
							  float *recv_parity1,
							  float *recv_parity2,
							  int subblk_sz,
							  int n_subblk,
							  int n_subblk_per_blk,
							   int *decoded_bits,
	int n_iters)
{
	int subblk_offset;
	int output_bit_offset;
	int n_blk;
	int blk_sz;
	int i, j, k, s;
	int stride;

	/*
	  float Le12[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
	  float Le12_int[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
	  float Le21[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
	  float Le21_int[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
	  float L[MAX_SUBFRAME_BUFFER_SIZE * N_TURBO_IN_MAX];
	*/

	/*
	  float old_alpha1[N_STATES * MAX_SUBBLOCKS_NUM];
	  float old_beta1[N_STATES * MAX_SUBBLOCKS_NUM];
	  float new_alpha1[N_STATES * MAX_SUBBLOCKS_NUM];
	  float new_beta1[N_STATES * MAX_SUBBLOCKS_NUM];
	
	  float old_alpha2[N_STATES * MAX_SUBBLOCKS_NUM];
	  float old_beta2[N_STATES * MAX_SUBBLOCKS_NUM];
	  float new_alpha2[N_STATES * MAX_SUBBLOCKS_NUM];
	  float new_beta2[N_STATES * MAX_SUBBLOCKS_NUM];
	*/
	
	memset(Le21, 0, (n_subblk * subblk_sz + (n_subblk / n_subblk_per_blk) * N_TAIL) * sizeof(float));
	memset(Le12_int, 0, (n_subblk * subblk_sz + (n_subblk / n_subblk_per_blk) * N_TAIL) * sizeof(float));

	n_blk = n_subblk / n_subblk_per_blk;
	blk_sz = subblk_sz * n_subblk_per_blk;
//	par_interleaver(internal_interleaver, recv_syst1, recv_syst2, n_subblk, subblk_sz, n_subblk_per_blk);
	par_interleaver(internal_interleaver, recv_syst1, recv_syst2, n_blk, blk_sz);

	// do the first alpha initialization
	for (i = 0; i < n_subblk; i++)
	{
		if (/*0 == i*/ 0 == (i % n_subblk_per_blk))
		{
			for (s = 0; s < N_STATES; s++)
			{
				if (0 == s)
				{
					old_alpha1[i * N_STATES + s] = 1.0;
					old_alpha2[i * N_STATES + s] = 1.0;
				}
				else
				{
					old_alpha1[i * N_STATES + s] = 0.0;
					old_alpha2[i * N_STATES + s] = 0.0;
				}
			}
		}
		else
		{
			for (s = 0; s < N_STATES; s++)
			{
				old_alpha1[i * N_STATES + s] = 1.0 / (N_STATES * 1.0);
				old_alpha2[i * N_STATES + s] = 1.0 / (N_STATES * 1.0);
			}
		}
	}

	// do the first beta initialization
	for (i = 0; i < n_subblk; i++)
	{
		if (/*(n_subblk - 1) == i*/ (n_subblk_per_blk - 1) == (i % n_subblk_per_blk))
		{
			for (s = 0; s < N_STATES; s++)
			{
				if (0 == s)
				{
					old_beta1[i * N_STATES + s] = 1.0;
					old_beta2[i * N_STATES + s] = 1.0;
				}
				else
				{
					old_beta1[i * N_STATES + s] = 0.0;
					old_beta2[i * N_STATES + s] = 0.0;
				}
			}
		}
		else
		{
			for (s = 0; s < N_STATES; s++)
			{
				old_beta1[i * N_STATES + s] = 1.0 / (N_STATES * 1.0);
				old_beta2[i * N_STATES + s] = 1.0 / (N_STATES * 1.0);
			}
		}
	}

	// do the iterative decoding, kernel part
	for (k = 0; k < /*MAX_ITERATIONS*/ n_iters; k++)
	{
//		printf("%d:\n", n_iters);

		gold_log_decoder_kernel(recv_syst1, recv_parity1, Le21, Le12, n_subblk, subblk_sz, n_subblk_per_blk, old_alpha1, old_beta1, new_alpha1, new_beta1);
#ifdef DEBUG		
		printf("Le12:\n");
		for (i = 0; i < subblk_sz; i++)
			printf("%.0f", Le12[i]);
		printf("\n");
		for (i = subblk_sz; i < 2 * subblk_sz + 3; i++)
			printf("%.0f", Le12[i]);
		printf("\n");
#endif
		for (i = 0; i < n_subblk; i++)
		{
			if (i % n_subblk_per_blk != 0)
			{
				for (s = 0; s < N_STATES; s++)
				{
					old_alpha1[i * N_STATES + s] = new_alpha1[i * N_STATES + s];
					//	old_beta1[i * N_STATES + s] = new_beta1[i * N_STATES + s];
				}
			}
		}
		for (i = 0; i < n_subblk; i++)
		{
			if (i % n_subblk_per_blk != (n_subblk_per_blk - 1))
			{
				for (s = 0; s < N_STATES; s++)
				{
					//	old_alpha1[i * N_STATES + s] = new_alpha1[i * N_STATES + s];
					old_beta1[i * N_STATES + s] = new_beta1[i * N_STATES + s];
				}
			}
		}

		//	par_interleaver(internal_interleaver, Le12, Le12_int, n_subblk, subblk_sz, n_subblk_per_blk);
		par_interleaver(internal_interleaver, Le12, Le12_int, n_blk, blk_sz);

		gold_log_decoder_kernel(recv_syst2, recv_parity2, Le12_int, Le21_int, n_subblk, subblk_sz, n_subblk_per_blk, old_alpha2, old_beta2, new_alpha2, new_beta2);
#ifdef DEBUG
		printf("Le21_int:\n");
		for (i = 0; i < subblk_sz; i++)
			printf("%.0f", Le21_int[i]);
		printf("\n");
		for (i = subblk_sz; i < 2 * subblk_sz + 3; i++)
			printf("%.0f", Le21_int[i]);
		printf("\n");
#endif
		for (i = 0; i < n_subblk; i++)
		{
			if (i % n_subblk_per_blk != 0)
			{
				for (s = 0; s < N_STATES; s++)
				{
					old_alpha2[i * N_STATES + s] = new_alpha2[i * N_STATES + s];
					//	old_beta2[i * N_STATES + s] = new_beta2[i * N_STATES + s];
				}
			}
		}
		for (i = 0; i < n_subblk; i++)
		{
			if (i % n_subblk_per_blk != (n_subblk_per_blk - 1))
			{
				for (s = 0; s < N_STATES; s++)
				{
					//	old_alpha2[i * N_STATES + s] = new_alpha2[i * N_STATES + s];
					old_beta2[i * N_STATES + s] = new_beta2[i * N_STATES + s];
				}
			}
		}

//		par_interleaver(internal_deinterleaver, Le21_int, Le21, n_subblk, subblk_sz, n_subblk_per_blk);
		par_interleaver(internal_deinterleaver, Le21_int, Le21, n_blk, blk_sz);
#ifdef DEBUG
		printf("Le21:\n");
		for (i = 0; i < subblk_sz; i++)
			printf("%.0f", Le21[i]);
		printf("\n");
		for (i = subblk_sz; i < 2 * subblk_sz + 3; i++)
			printf("%.0f", Le21[i]);
		printf("\n");
#endif
	}

	subblk_offset = 0;
	output_bit_offset = 0;
	for (i = 0; i < n_subblk; i++)
	{
		if ((i % n_subblk_per_blk) == (n_subblk_per_blk - 1))
			stride = subblk_sz + N_TAIL;
		else
			stride = subblk_sz;
		
		for (j = subblk_offset; j < subblk_offset + subblk_sz; j++)
		{
			L[output_bit_offset] = recv_syst1[j] + Le21[j] + Le12[j];
			decoded_bits[output_bit_offset] = (L[output_bit_offset] > 0.0) ? 1 : -1;
			output_bit_offset++;
		}

		subblk_offset += stride;
	}
}

void gold_log_decoder_kernel(float *recv_syst, 
					  float *recv_parity,
					  float *apriori, 
					  float *extrinsic,
					  int n_subblk,
					  int subblk_sz,
					  int n_subblk_per_blk,
					  float *old_alpha,
					  float *old_beta,
					  float *new_alpha,
					  float *new_beta)
{
	float nom, den, temp0, temp1, exp_temp0, exp_temp1, rp;
	int i, j, s0, s1, k, kk, l, s, s_prim, s_prim0, s_prim1;
	int alpha_len;
	int subblk_offset;
	int stride;

	Lc = 1.0;

//	gold_log_decoder_kernel(recv_syst, recv_parity, apriori, extrinsic, alpha, beta, n_subblk, subblk_sz, n_subblk_per_blk);

	com_log = max_log;

	/** 
	 * tid = get_thread_id();
	 * i = tid / N_STATES; 
	 * s_prim = tid % N_STATES;
	 */

	for (i = 0; i < n_subblk; i++)
	{
		subblk_offset = (i / n_subblk_per_blk) * (BLOCK_SIZE + N_TAIL) + (i % n_subblk_per_blk) * subblk_sz;

		if ((i % n_subblk_per_blk) == (n_subblk_per_blk - 1))
			stride = subblk_sz + N_TAIL;
		else
			stride = subblk_sz;

		// Initiate alpha and beta
		for (s = 0; s < N_STATES; s++)
		{
			l_alpha[s * (stride + 1) + 0] = old_alpha[i * N_STATES + s];
			l_beta[s * (stride + 1) + stride] = old_beta[i * N_STATES + s];
		}

		// Synchronize
		
		for (k = 0; k <= stride; k++)
		{
			l_denom[k] = -LOG_INFINITY;
		}
		for (k = 0; k < stride; k++)
		{
			l_apriori[k] = apriori[subblk_offset + k];
		}

		// Calculate gamma
		for (k = 1; k <= stride; k++)
		{
			kk = k - 1;

			for (s_prim = 0; s_prim < N_STATES; s_prim++)
			{
				exp_temp0 = 0.0;
				exp_temp1 = 0.0;

				for (j = 0; j < (N_GENS - 1); j++)
				{
					rp = recv_parity[subblk_offset * (N_GENS - 1) + kk * (N_GENS - 1) + j];
					if (0 == g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 0])
					{
						exp_temp0 += rp;
					}
					else
					{ 
						exp_temp0 -= rp;
					}
					if (0 == g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 1])
					{
						exp_temp1 += rp;
					}
					else
					{
						exp_temp1 -= rp; 
					}
				}

				l_gamma[(2 * s_prim + 0) * (stride + 1) + k] = 0.5 * ((l_apriori[kk] + recv_syst[subblk_offset + kk]) + exp_temp0);
				//	printf("%f\t", l_gamma[(2 * s_prim + 0) * (stride + 1) + k]);
				l_gamma[(2 * s_prim + 1) * (stride + 1) + k] = -0.5 * ((l_apriori[kk] + recv_syst[subblk_offset + kk]) - exp_temp1);
				//	printf("%f\n", l_gamma[(2 * s_prim + 1) * (stride + 1) + k]);
			}
		}

		// Calculate alpha, going forward through the trellis
		for (k = 1; k <= stride; k++)
		{
			for (s = 0; s < N_STATES; s++)
			{
				s_prim0 = g_rev_state_trans[s * 2 + 0];
				s_prim1 = g_rev_state_trans[s * 2 + 1];
				temp0 = l_alpha[s_prim0 * (stride + 1) + k - 1] + l_gamma[(2 * s_prim0 + 0) * (stride + 1) + k];
				temp1 = l_alpha[s_prim1 * (stride + 1) + k - 1] + l_gamma[(2 * s_prim1 + 1) * (stride + 1) + k];
				l_alpha[s * (stride + 1) + k] = com_log(temp0, temp1);
				l_denom[k] = com_log(l_alpha[s * (stride + 1) + k], l_denom[k]);
			}

			// Normalization of alpha
			for (s = 0; s < N_STATES; s++)
			{
				l_alpha[s * (stride + 1) + k] -= l_denom[k];
			}
		}

		// Calculate beta going backward in the trellis
		for (k = stride; k >= /*2*/ 1; k--)
		{
			for (s_prim = 0; s_prim < N_STATES; s_prim++)
			{
				s0 = g_state_trans[s_prim * 2 + 0];
				s1 = g_state_trans[s_prim * 2 + 1];
				temp0 = l_beta[s0 * (stride + 1) + k] +	l_gamma[(2 * s_prim + 0) * (stride + 1) + k];
				temp1 = l_beta[s1 * (stride + 1) + k] + l_gamma[(2 * s_prim + 1) * (stride + 1) + k];
				l_beta[s_prim * (stride + 1) + k - 1] = com_log(temp0, temp1);
			}
			// Normalization of beta
			for (s = 0; s < N_STATES; s++)
			{
				l_beta[s * (stride + 1) + k - 1] -= l_denom[k];
			}
		}

		// Calculate extrinsic output for each bit
		for (k = 1; k <= stride; k++)
		{
			kk = k - 1;
			nom = -LOG_INFINITY;
			den = -LOG_INFINITY;
			for (s_prim = 0; s_prim < N_STATES; s_prim++)
			{
				s0 = g_state_trans[s_prim * 2 + 0];
				s1 = g_state_trans[s_prim * 2 + 1];
				exp_temp0 = 0.0;
				exp_temp1 = 0.0;
				for (j = 0; j < (N_GENS - 1); j++)
				{
					rp = recv_parity[subblk_offset * (N_GENS - 1) + kk * (N_GENS - 1) + j];
					if (0 == g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 0])
					{
						exp_temp0 += rp;
					}
					else
					{
						exp_temp0 -= rp; 
					}
					if (0 == g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 1])
					{ 
						exp_temp1 += rp;
					}
					else
					{
						exp_temp1 -= rp;
					}
				}
				nom = com_log(nom, l_alpha[s_prim * (stride + 1) + kk] + 0.5 * exp_temp0 + l_beta[s0 * (stride + 1) + k]);
				den = com_log(den, l_alpha[s_prim * (stride + 1) + kk] + 0.5 * exp_temp1 + l_beta[s1 * (stride + 1) + k]);
			}
			
			extrinsic[subblk_offset + kk] = nom - den;

			/* Use new alpha and beta to avoid global synchronization */
			if (/*i != (n_subblk - 1)*/ (i % n_subblk_per_blk) != (n_subblk_per_blk - 1))
			{
				for (s = 0; s < N_STATES; s++)
				{
					new_alpha[(i + 1) * N_STATES + s] = l_alpha[s * (stride + 1) + stride];
				}
			}
			if (/*i != 0*/ (i % n_subblk_per_blk) != 0)
			{
				for (s = 0; s < N_STATES; s++)
				{
					new_beta[(i - 1) * N_STATES + s] = l_beta[s * (stride + 1) + /*1*/ 0];
				}
			}
		}
	}
}

void par_decode_all_subblocks(float *recv_syst1, 
							  float *recv_syst2,
							  float *recv_parity1,
							  float *recv_parity2,
							  int subblk_sz,
							  int n_subblk,
							  int n_subblk_per_blk,
							  int *decoded_bits,
							  int n_iters)
{
	cl_int _err;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel par_log_decoder_kernel;

	cl_mem recv_syst_buffer, recv_parity_buffer;
//	cl_mem recv_syst2_buffer, recv_parity2_buffer;
	cl_mem Le12_buffer, Le12_int_buffer, Le21_buffer, Le21_int_buffer, L_buffer;
//	cl_mem old_alpha1_buffer, old_beta1_buffer, new_alpha1_buffer, new_beta1_buffer;
//	cl_mem old_alpha2_buffer, old_beta2_buffer, new_alpha2_buffer, new_beta2_buffer;
	cl_mem old_alpha_buffer, old_beta_buffer, new_alpha_buffer, new_beta_buffer;
	cl_mem g_output_parity_buffer, g_state_trans_buffer, g_rev_state_trans_buffer;

	size_t global_size, local_size;
	
	int n_blk, blk_sz;
	int syst_len, alpha_len;
	int local_syst_len, local_alpha_len;
	int n_subblks_per_work_group;

	int i, s, k;

	device_query();
	
	syst_len = n_subblk * subblk_sz + (n_subblk / n_subblk_per_blk) * N_TAIL;
	alpha_len = syst_len + n_subblk * 1;

	/*
	 * FIXME: n_subblks_per_work_group should be a parameter to this function
	 */
	n_subblks_per_work_group = n_subblk_per_blk;
	global_size = N_STATES * n_subblk;
	local_size = N_STATES * n_subblks_per_work_group;
	local_syst_len = (BLOCK_SIZE / n_subblk_per_blk) * n_subblks_per_work_group + (n_subblks_per_work_group / n_subblk_per_blk) * N_TAIL;
	local_alpha_len = local_syst_len + 1 * n_subblks_per_work_group;
		
	memset(Le21, 0, (n_subblk * subblk_sz + (n_subblk / n_subblk_per_blk) * N_TAIL) * sizeof(float));
	memset(Le12_int, 0, (n_subblk * subblk_sz + (n_subblk / n_subblk_per_blk) * N_TAIL) * sizeof(float));

	n_blk = n_subblk / n_subblk_per_blk;
	blk_sz = subblk_sz * n_subblk_per_blk;
	
	/* Initiate kernel arguments */
//	par_interleaver(internal_interleaver, recv_syst1, recv_syst2, n_subblk, subblk_sz, n_subblk_per_blk);
	par_interleaver(internal_interleaver, recv_syst1, recv_syst2, n_blk, blk_sz);

	// do the first alpha initialization
	for (i = 0; i < n_subblk; i++)
	{
		if (/*0 == i*/ 0 == (i % n_subblk_per_blk))
		{
			for (s = 0; s < N_STATES; s++)
			{
				if (0 == s)
				{
					old_alpha1[i * N_STATES + s] = 1.0;
					old_alpha2[i * N_STATES + s] = 1.0;
				}
				else
				{
					old_alpha1[i * N_STATES + s] = 0.0;
					old_alpha2[i * N_STATES + s] = 0.0;
				}
			}
		}
		else
		{
			for (s = 0; s < N_STATES; s++)
			{
				old_alpha1[i * N_STATES + s] = 1.0 / (N_STATES * 1.0);
				old_alpha2[i * N_STATES + s] = 1.0 / (N_STATES * 1.0);
			}
		}
	}

	// do the first beta initialization
	for (i = 0; i < n_subblk; i++)
	{
		if ((n_subblk - 1) == i /*(n_subblk_per_blk - 1) == (i % n_subblk_per_blk)*/)
		{
			for (s = 0; s < N_STATES; s++)
			{
				if (0 == s)
				{
					old_beta1[i * N_STATES + s] = 1.0;
					old_beta2[i * N_STATES + s] = 1.0;
				}
				else
				{
					old_beta1[i * N_STATES + s] = 0.0;
					old_beta2[i * N_STATES + s] = 0.0;
				}
			}
		}
		else
		{
			for (s = 0; s < N_STATES; s++)
			{
				old_beta1[i * N_STATES + s] = 1.0 / (N_STATES * 1.0);
				old_beta2[i * N_STATES + s] = 1.0 / (N_STATES * 1.0);
			}
		}
	}

	// Create device, context, program, kernel, queue
//	cl_params_init(PROGRAM_FILE, KERNEL_FUNC);
	
	/* Create a device and context */
	device = create_device();
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &_err);

	/* Build the program */
	program = build_program(context, device, PROGRAM_FILE);

	/* Create kernel */
	for (i = 0; i < NUM_KERNELS; i++)
		par_log_decoder_kernel[i] = clCreateKernel(program, KERNEL_FUNC, &_err);

	/* Create a command queue */
	queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &_err);

	
	// Create buffer(s)
	recv_syst_buffer[0] = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, syst_len * sizeof(float), recv_syst1, &_err));
	recv_parity_buffer[0] = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, syst_len * sizeof(float), recv_parity1, &_err));
	recv_syst_buffer[1] = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, syst_len * sizeof(float), recv_syst2, &_err));
	recv_parity_buffer[1] = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, syst_len * sizeof(float), recv_parity2, &_err));

	Le12_buffer = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_WRITE, syst_len * sizeof(float), NULL, &_err));
	Le12_int_buffer = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_WRITE, syst_len * sizeof(float), NULL, &_err));
	Le21_buffer = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_WRITE, syst_len * sizeof(float), NULL, &_err));
	Le21_int_buffer = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_WRITE, syst_len * sizeof(float), NULL, &_err));
	L_buffer = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_WRITE, syst_len * sizeof(float), NULL, &_err));

	for (i = 0; i < NUM_KERNELS; i++)
	{
		old_alpha_buffer[i] = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_ONLY, N_STATES * n_subblk * sizeof(float), NULL, &_err));
		old_beta_buffer[i] = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_ONLY, N_STATES * n_subblk * sizeof(float), NULL, &_err));
		new_alpha_buffer[i] = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_ONLY, N_STATES * n_subblk * sizeof(float), NULL, &_err));
		new_beta_buffer[i] = CL_CHECK_ERR(clCreateBuffer(context, CL_MEM_READ_ONLY, N_STATES * n_subblk * sizeof(float), NULL, &_err));
	}

	g_output_parity_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, N_STATES * (N_GENS - 1) * 2 * sizeof(int), g_output_parity, &_err);
	g_state_trans_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, N_STATES * 2 * sizeof(int), g_state_trans, &_err);
	g_rev_state_trans_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, N_STATES * 2 * sizeof(int), g_rev_state_trans, &_err);

	/* Set kernel arguments */
	for (i = 0; i < NUM_KERNELS; i++)
	{
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 0, sizeof(cl_mem), &recv_syst_buffer[i]));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 1, sizeof(cl_mem), &recv_parity_buffer[i]));
		if (0 == i)
		{
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 2, sizeof(cl_mem), &Le21_buffer));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 3, sizeof(cl_mem), &Le12_buffer));
		}
		else
		{
			CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 2, sizeof(cl_mem), &Le12_int_buffer));
			CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 3, sizeof(cl_mem), &Le21_int_buffer));
		}
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 4, sizeof(int), &n_subblk));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 5, sizeof(int), &subblk_sz));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 6, sizeof(int), &n_subblk_per_blk));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 7, sizeof(cl_mem), &old_alpha_buffer[i]));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 8, sizeof(cl_mem), &old_beta_buffer[i]));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 9, sizeof(cl_mem), &new_alpha_buffer[i]));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 10, sizeof(cl_mem), &new_beta_buffer[i]));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 11, sizeof(cl_mem), &g_output_parity_buffer));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 12, sizeof(cl_mem), &g_state_trans_buffer));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 13, sizeof(cl_mem), &g_rev_state_trans_buffer));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 14, N_STATES * local_alpha_len * sizeof(float), NULL));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 15, N_STATES * local_alpha_len * sizeof(float), NULL));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 16, N_STATES * 2 * local_alpha_len * sizeof(float), NULL));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 17, N_STATES * local_alpha_len * sizeof(float), NULL));
		CL_CHECK(clSetKernelArg(par_log_decoder_kernel[i], 18, N_STATES * local_syst_len * sizeof(float), NULL));
	}
	
	double elapsed_time = 0.0;
	cl_event prof_event;

	for (k = 0; k < n_iters; k++)
	{
		// Input(s) for the first Log-MAP kernel
		CL_CHECK(clEnqueueWriteBuffer(queue, Le21_buffer, CL_TRUE, 0, syst_len * sizeof(float), Le21, 0, NULL, NULL));
		CL_CHECK(clEnqueueWriteBuffer(queue, old_alpha_buffer[0], CL_TRUE, 0, N_STATES * n_subblk * sizeof(float), old_alpha1, 0, NULL, NULL));
		CL_CHECK(clEnqueueWriteBuffer(queue, old_beta_buffer[0], CL_TRUE, 0, N_STATES * n_subblk * sizeof(float), old_beta1, 0, NULL, NULL));

		// Launch the first Log-MAP kernel
		printf("%d\n", CL_INVALID_PROGRAM_EXECUTABLE);
		printf("%d\n", CL_INVALID_COMMAND_QUEUE);
		printf("%d\n", CL_INVALID_KERNEL);
		printf("%d\n", CL_INVALID_CONTEXT);
		printf("%d\n", CL_INVALID_KERNEL_ARGS);
		printf("%d\n", CL_INVALID_WORK_DIMENSION);
		printf("%d\n", CL_OUT_OF_RESOURCES);
		printf("%d\n", CL_INVALID_EVENT_WAIT_LIST);
		printf("%d\n", CL_MEM_OBJECT_ALLOCATION_FAILURE);
		printf("%d\n", CL_OUT_OF_HOST_MEMORY);
		CL_CHECK(clEnqueueNDRangeKernel(queue, par_log_decoder_kernel[0], 1, NULL, &global_size, &local_size, 0, NULL, &prof_event));

		// Get output(s) of the first log-MAP kernel
		CL_CHECK(clEnqueueReadBuffer(queue, Le12_buffer, CL_TRUE, 0, syst_len * sizeof(float), Le12, 0, NULL, NULL));
		CL_CHECK(clEnqueueReadBuffer(queue, new_alpha_buffer[0], CL_TRUE, 0, N_STATES * n_subblk * sizeof(float), new_alpha1, 0, NULL, NULL));
		CL_CHECK(clEnqueueWriteBuffer(queue, new_beta_buffer[0], CL_TRUE, 0, N_STATES * n_subblk * sizeof(float), new_beta1, 0, NULL, NULL));

		// Update boundary alpha and beta for the next iteration
		for (i = 0; i < n_subblk; i++)
		{
			if (i % n_subblk_per_blk != 0)
			{
			for (s = 0; s < N_STATES; s++)
			{
				old_alpha1[i * N_STATES + s] = new_alpha1[i * N_STATES + s];
				//	old_beta1[i * N_STATES + s] = new_beta1[i * N_STATES + s];
			}
			}
		}
		for (i = 0; i < n_subblk - 1; i++)
		{
			for (s = 0; s < N_STATES; s++)
			{
				//	old_alpha1[i * N_STATES + s] = new_alpha1[i * N_STATES + s];
				old_beta1[i * N_STATES + s] = new_beta1[i * N_STATES + s];
			}
		}
		
		par_interleaver(internal_interleaver, Le12, Le12_int, n_blk, blk_sz);

		// Input(s) for the second Log-MAP kernel
		CL_CHECK(clEnqueueWriteBuffer(queue, Le12_int_buffer, CL_TRUE, 0, syst_len * sizeof(float), Le12_int, 0, NULL, NULL));
		CL_CHECK(clEnqueueWriteBuffer(queue, old_alpha_buffer[1], CL_TRUE, 0, N_STATES * n_subblk * sizeof(float), old_alpha2, 0, NULL, NULL));
		CL_CHECK(clEnqueueWriteBuffer(queue, old_beta_buffer[1], CL_TRUE, 0, N_STATES * n_subblk * sizeof(float), old_beta2, 0, NULL, NULL));

		// Launch the second Log-MAP kernel
		CL_CHECK(clEnqueueNDRangeKernel(queue, par_log_decoder_kernel[1], 1, NULL, &global_size, &local_size, 0, NULL, &prof_event));

		// Get output(s) of the second Log-MAP kernel
		CL_CHECK(clEnqueueReadBuffer(queue, Le12_int_buffer, CL_TRUE, 0, syst_len * sizeof(float), Le12_int, 0, NULL, NULL));
		CL_CHECK(clEnqueueReadBuffer(queue, new_alpha_buffer[1], CL_TRUE, 0, N_STATES * n_subblk * sizeof(float), new_alpha2, 0, NULL, NULL));
		CL_CHECK(clEnqueueWriteBuffer(queue, new_beta_buffer[1], CL_TRUE, 0, N_STATES * n_subblk * sizeof(float), new_beta2, 0, NULL, NULL));

		for (i = 1; i < n_subblk; i++)
		{
			for (s = 0; s < N_STATES; s++)
			{
				old_alpha2[i * N_STATES + s] = new_alpha2[i * N_STATES + s];
				//	old_beta2[i * N_STATES + s] = new_beta2[i * N_STATES + s];
			}
		}
		for (i = 0; i < n_subblk - 1; i++)
		{
			for (s = 0; s < N_STATES; s++)
			{
				//	old_alpha2[i * N_STATES + s] = new_alpha2[i * N_STATES + s];
				old_beta2[i * N_STATES + s] = new_beta2[i * N_STATES + s];
			}
		}
		
		par_interleaver(internal_deinterleaver, Le21_int, Le21, n_blk, blk_sz);
	}

//	cl_params_release();
	for (i = 0; i < NUM_KERNELS; i++)
	{
		clReleaseKernel(par_log_decoder_kernel[i]);
	}
	clReleaseProgram(program);
	clReleaseCommandQueue(queue);
	clReleaseContext(context);

	for (i = 0; i < NUM_KERNELS; i++)
	{
		clReleaseMemObject(recv_syst_buffer[i]);
		clReleaseMemObject(recv_parity_buffer[i]);
	}

	clReleaseMemObject(g_output_parity_buffer);
	clReleaseMemObject(g_state_trans_buffer);
	clReleaseMemObject(g_rev_state_trans_buffer);
	
	clReleaseMemObject(Le12_buffer);
	clReleaseMemObject(Le12_int_buffer);
	clReleaseMemObject(Le21_buffer);
	clReleaseMemObject(Le21_int_buffer);
	clReleaseMemObject(L_buffer);
}

