
#ifndef __TURBO_H_
#define __TURBO_H_

#include "lte_phy.h"

#define LOG_INFINITY 1e30

//void turbo_init();

void turbo_encoding(LTE_PHY_PARAMS *lte_phy_params, int *piSeq, int *pcSeq);

void encode_block(int *input_bits,
		int *interleaved_input_bits,
		int *parity1,
		int *tail1,
		int *parity2,
		int *tail2,
		int blk_len);

void turbo_decoding(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, int *pOutBits, int n_iters);

int reverse_int(int length, int in);

void constituent_encoder(int *input, int input_len, int *tail, int *parity);
int calc_state_transition(int instate, int input, int *parity);
void set_generator_polynomials(int *gens, int n_gens, int constraint_length);
void decode_block(float *recv_syst1, float *recv_syst2, float *recv_parity1, float *recv_parity2, int *decoded_bits_i, int interleaver_size, int n_iters);
void map_decoder(float *recv_syst, float *recv_parity, float *apriori, float *extrinsic, int interleaver_size);
void log_decoder(float *recv_syst, float *recv_parity, float *apriori, float *extrinsic, int interleaver_size);
void log_decoder_two_kernels(float *recv_syst, float *recv_parity, float *apriori, float *extrinsic, int interleaver_size);

template <typename T> void internal_interleaver(T *in, T *out, int m);
template <typename T> void internal_deinterleaver(T *in, T *out, int m);

float max_log(float a, float b);
//float add_log(float a, float b);
	
//	float max_log(float a, float b);
//	float add_log(float a, float b);


extern int InpBlockShift;
extern int OutBlockShift;
extern int NumBlock;
extern int DataLength;
extern int LastBlockLength;
	
extern int g_gens[N_GENS];

extern int g_gen_pols[N_GENS];
extern int g_rev_gen_pols[N_GENS];
extern int g_state_trans[N_STATES * 2];
extern int g_rev_state_trans[N_STATES * 2];
extern int g_output_parity[N_STATES * (N_GENS - 1) * 2];
extern int g_rev_output_parity[N_STATES * (N_GENS - 1) * 2];

extern float Lc;

extern std::string metric;
extern float (*com_log)(float, float);

extern const double log_double_max;
extern const double log_double_min;

#endif
