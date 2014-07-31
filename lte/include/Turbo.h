
#ifndef __TURBO_H_
#define __TURBO_H_

#include <cmath>
#include <complex>
#include <stdexcept>
#include <iostream>

#include "lte_phy.h"

#define TURBO_INT_K_TABLE_SIZE 188

//void turbo_init();

void turbo_encoding(LTE_PHY_PARAMS *lte_phy_params, int *piSeq, int *pcSeq);

void encode_block(int *input_bits,
		int *interleaved_input_bits,
		int *parity1,
		int *tail1,
		int *parity2,
		int *tail2,
		int blk_len);

void turbo_decoding(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, int *pOutBits);

int reverse_int(int length, int in);

void constituent_encoder(int *input, int input_len, int *tail, int *parity);
int calc_state_transition(int instate, int input, int *parity);
void set_generator_polynomials(int gens[], int n_gens, int constraint_length);
void decode_block(float *recv_syst1, float *recv_syst2, float *recv_parity1, float *recv_parity2, int *decoded_bits_i, int interleaver_size);
void map_decoder(float *recv_syst, float *recv_parity, float *apriori, float *extrinsic, int interleaver_size);
void log_decoder(float *recv_syst, float *recv_parity, float *apriori, float *extrinsic, int interleaver_size);

	
template <typename T> void internal_interleaver(T *in, T *out, int interleaver_size);
template <typename T> void internal_deinterleaver(T *in, T *out, int deinterleaver_size);
	
//	float max_log(float a, float b);
//	float add_log(float a, float b);
	
#endif
