#ifndef __PARTURBO_H_
#define __PARTURBO_H_

void par_turbo_decoding(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, int *pOutBits, int n_sfr, int n_iters);

void gold_decode_all_subblocks(float *recv_syst1, 
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
