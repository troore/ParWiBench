#ifndef __PARTURBO_H_
#define __PARTURBO_H_

#include "Turbo.h"

#define MAX_SUBFRAME_BUFFER_SIZE	10
#define NUM_SUBBLOCKS_PER_BLOCK	4
#define MAX_BLOCKS_NUM	MAX_SUBFRAME_BUFFER_SIZE * (N_TURBO_IN_MAX / BLOCK_SIZE)	
#define MAX_SUBBLOCKS_NUM	(MAX_BLOCKS_NUM * NUM_SUBBLOCKS_PER_BLOCK)

void par_turbo_decoding(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, int *pOutBits, int n_sfr, int n_iters);

template <typename T>
void par_interleaver(void (*pInterleaver)(T *, T *, int), T *in, T *out, 
					 int n_subblk, int subblk_sz, int n_subblk_per_blk);

template <typename T>
void par_interleaver(void (*pInterleaver)(T *, T *, int), T *in, T *out, int n_blk, int blk_sz);

void gold_decode_all_subblocks(float *recv_syst1, 
							  float *recv_syst2,
							  float *recv_parity1,
							  float *recv_parity2,
							  int subblk_sz,
							  int n_subblk,
							  int n_subblk_per_blk,
							   int *decoded_bits,
	int n_iters);

void par_decode_all_subblocks(float *recv_syst1, 
							  float *recv_syst2,
							  float *recv_parity1,
							  float *recv_parity2,
							  int subblk_sz,
							  int n_subblk,
							  int n_subblk_per_blk,
							  int *decoded_bits,
	int n_iters);

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
					  float *new_beta);

#endif
