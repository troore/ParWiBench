
#ifndef __SCRAMBLER_H_
#define __SCRAMBLER_H_

#include "lte_phy.h"

#define N_SCRAMB_IN_MAX (LTE_PHY_N_ANT_MAX * LTE_PHY_DFT_SIZE_30_72MHZ * (LTE_PHY_N_SYMB_PER_SUBFR - 2) * QAM64_BITS_PER_SAMP)
#define N_SCRAMB_OUT_MAX (N_SCRAMB_IN_MAX)

void GenScrambInt(int *pScrambInt, int n);
void Scrambling(int *pInpSeq, int n_inp, int *pOutSeq, int n_out);

void Descrambling(float *pInpSeq, int n_inp, float *pOutBuf, int n_out);

#endif
