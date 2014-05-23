
#ifndef __TURBO_H_
#define __TURBO_H_

//#pragma once
#include <cmath>
#include <complex>
#include <stdexcept>
#include <iostream>
//#include "fsm.h"
#include "Interleaver_lte.h"
#include "BSPara.h"
#include "UserPara.h"
//#include "FIFO.h"


using namespace std;

class Turbo
{
 private:
	bool PSFlag;
	bool BufFlag;
	int InpBlockShift;
	int OutBlockShift;
	int NumBlock;
	int DataLength;
	int LastBlockLength;
	
	//	int *pLengthSet;

	int Rate;
	int iSeqLength;

	//	int* piSeq;
	//	int** pcSeq;

	//	float* pLLR;
	//	int* pData;

	/* Turbo specific parameters */
	int m_gens[2];
	int m_n_gens;
	int m_cst_len;
	int m_block_size;
	int m_n_iters;
	int m_n_tail;
	int m_n_states;
	int m_n_uncoded;
	int m_n_coded;

	int *m_gen_pols;
	int *m_rev_gen_pols;
	int *m_state_trans;
	int *m_rev_state_trans;
	int *m_output_parity;
	int *m_rev_output_parity;

	float Lc;
	/* Turbo specific parameters over... */
  
 public:
	//	int InBufSz[2];
	int InBufSz;
	//Turbo Encoder's FIFO
	//	FIFO<int> *pInpBuf;
	//	int OutBufSz[2];
	int OutBufSz;

	// Default constructor
	Turbo() {}
	Turbo(BSPara* pBS);
	Turbo(UserPara* pUser);
	~Turbo(void);

	//  void TurboEncoding(FIFO<int> *pInpBuf,FIFO<int> *pOutBuf);
	//	void TurboEncoding(FIFO<int> *pOutBuf);
	//	void TurboDecoding(FIFO<int> *pOutBuf);
	void TurboEncoding(int *piSeq, int *pcSeq);
	void TurboDecoding(float *pInpData, int *pOutBits);
	
	template <typename T> void internal_interleaver(T *in, T *out, int interleaver_size);
	template <typename T> void internal_deinterleaver(T *in, T *out, int deinterleaver_size);
	int reverse_int(int length, int in);
	void set_parameters(int *gens, int n_gens, int constraint_length, int block_size, int n_iters);
	void constituent_encoder(int *input, int *tail, int *parity);
	int calc_state_transition(int instate, int input, int *parity);
	void set_generator_polynomials(int gens[], int n_gens, int constraint_length);
	void decode_block(float *recv_syst1, float *recv_syst2, float *recv_parity1, float *recv_parity2, int *decoded_bits_i, int interleaver_size);
	void map_decoder(float *recv_syst, float *recv_parity, float *apriori, float *extrinsic, int interleaver_size);
	


};

#endif
