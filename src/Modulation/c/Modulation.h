
#ifndef __MODULATION_H_
#define __MODULATION_H_

#include <complex>
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>

#include "lte_phy.h"

typedef float (*p_a)[2];

float vecmin(float* pV,int len);
float eudist(float a, float b, float c, float d);
void dec2bits(int i, int n, int *bvec);

void set_mod_params(p_a *pp_table, int *bits_per_samp, int *mod_table_len, int mod_type);
void init_mod_tables();

void Modulating(LTE_PHY_PARAMS *lte_phy_params, int *pBitsSeq, std::complex<float> *pQAMSeq, int mod_type);

void Demodulating(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pDecQAMSeq, float *pLLR, int mod_type, float awgnSigma);

void Demodulating(LTE_PHY_PARAMS *lte_phy_params, float *pDecQAMSeq_0, float *pDecQAMSeq_1, float *pLLR, int mod_type, float awgnSigma);

void Demodulating(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pDecQAMSeq, int *pHD, int mod_type);

#endif
