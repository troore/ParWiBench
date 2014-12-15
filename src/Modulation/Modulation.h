
#ifndef __MODULATION_H_
#define __MODULATION_H_

//#include <complex>
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>

#include "lte_phy.h"

void Modulating(LTE_PHY_PARAMS *lte_phy_params, int *pBitsSeq, float *pQAMSeq, int mod_type);

void Demodulating(LTE_PHY_PARAMS *lte_phy_params, float *pDecQAMSeq, float *pLLR, int mod_type, float awgnSigma);

/*
 * for Hibbert's MIC implementation
void Demodulating(LTE_PHY_PARAMS *lte_phy_params, float *pDecQAMSeq_0, float *pDecQAMSeq_1, float *pLLR, int mod_type, float awgnSigma);

void Demodulating(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pDecQAMSeq, int *pHD, int mod_type);
*/

void Demodulating(LTE_PHY_PARAMS *lte_phy_params, float *pDecQAMSeq, int *pHD, int mod_type);

#endif
