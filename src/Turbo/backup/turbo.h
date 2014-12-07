#ifndef __TURBO_H_
#define __TURBO_H_

#include "types.h"
#include <iostream>

class Turbo
{
 public:
	Turbo(uint32_T gens[], uint32_T n_gens, uint32_T constraint_length, uint32_T interleaver_size, uint32_T n_iters = 1);
	~Turbo();

	/*
	 * public utilities
	 */
	//	void set_parameters(uint32_T gen[], uint32_T n_gens, uint32_T constraint_length, uint32_T interleaver_size);
	void set_generator_polynomials(uint32_T gens[], uint32_T n_gens, uint32_T constraint_length);
	int reverse_int(uint32_T length, int in);
	int calc_state_transition(const uint32_T instate, const int input, uint8_T *parity);

	/*
	 * encoder
	 */
    void spec_encoder(const uint8_T *input, const uint32_T n_input,
							   uint8_T *output, uint32_T &n_output) const;
    void spec_constituent_encoder(const uint8_T *in_bits, uint8_T *out_bits, uint8_T *fb_bits) const;
	
	void encoder(const uint8_T *input, const uint32_T n_input,
					   uint8_T *output, uint32_T &n_output) const;
	void constituent_encoder(const uint8_T *input, uint8_T *tail, uint8_T *parity) const;
	
	template <typename T> void internal_interleaver(const T *in, T *out) const;
	template <typename T> void internal_deinterleaver(const T *in, T *out) const;	

	void gen_trellis();
	/*
	 * decoder
	 */
	void spec_decoder(const float_point *y, const uint32_T n_y, int8_T *d, uint32_T &n_d);
	void spec_map_decoder(const float_point *y_s, const float_point *y_p, const float_point *L_apriori,
						  float_point *L_extrinsic, float_point *LLR, const uint32_T n) const;
	
	void decoder(const float_point *recv_signals, const uint32_T n_recv_signals, int8_T *decoded_bits, uint32_T &n_decoded_bits);
	void decode_block(float_point *recv_syst1, float_point *recv_syst2, float_point *recv_parity1,
					  float_point *recv_parity2, int8_T *decoded_bits_i);
   
	void map_decoder(float_point *recv_syst, float_point *recv_parity, float_point *apriori, float_point *extrinsic);
	//	void log_map_decoder();
	
 private:
    uint32_T m_cst_len; // constraint length
	uint32_T m_n_states;
	/*
	 * m_n_uncoded: number of bits per codeblock to be coded
	 * m_n_coded: number of coded bits per codeblock
	 */
	uint32_T m_n_uncoded, m_n_coded;
	uint32_T m_n_tail;
	uint32_T m_n_gens; // number of polynomials
	uint32_T m_interleaver_size;

	/* one generator polynomial is enough because
	 * the two RSCs are same for 3GPP turbo encoder/decoder
	 */
	uint32_T *m_gen_pols;
	uint32_T *m_rev_gen_pols;

	// output_parity[m_n_states][(m_n_gens - 1) * 2]
	uint8_T *m_output_parity;
	uint8_T *m_rev_output_parity;
	// state_trans[m_n_states][2]
	uint8_T *m_state_trans;
	uint8_T *m_rev_state_trans;

	uint32_T m_n_iters;
	int8_T *m_trellis;

	float_point Lc;
};

#define TURBO_INT_K_TABLE_SIZE 188

extern uint32_T TURBO_INT_K_TABLE[];
extern uint32_T TURBO_INT_F1_TABLE[];
extern uint32_T TURBO_INT_F2_TABLE[];


#endif
