
#ifndef __TURBO_H_
#define __TURBO_H_

#include <cmath>
#include <complex>
#include <stdexcept>
#include <iostream>

#include "lte_phy.h"

#define TURBO_INT_K_TABLE_SIZE 188

#define RATE 3
#define N_GENS 2 // number of generators
#define CST_LEN 4 // constraint length
#define N_TAIL (CST_LEN - 1)
#define N_STATES (1 << N_TAIL)
#define N_UNCODED (BLOCK_SIZE)
#define N_CODED (1 + (N_GENS - 1) * 2) * N_UNCODED + 2 * N_GENS * N_TAIL
#define MAX_ITERATIONS 1 // turbo decoder maximum interation times

#define N_TURBO_OUT_MAX (LTE_PHY_N_ANT_MAX * LTE_PHY_DFT_SIZE_30_72MHZ * (LTE_PHY_N_SYMB_PER_SUBFR - 2) * QAM64_BITS_PER_SAMP)
#define N_TURBO_IN_MAX (N_TURBO_OUT_MAX / RATE)

void turbo_init(int data_length);

void turbo_encoding(int *piSeq, int input_data_length, int *pcSeq);

void encode_block();

void turbo_decoding(float *pInpData, int *pOutBits, int out_data_length);

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
