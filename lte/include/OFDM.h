
#ifndef __OFDM_H_
#define __OFDM_H_

#include <complex>
#include <fftw3.h>

#include "lte_phy.h"

void ofmodulating(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pInpData, std::complex<float> *pOutData);
void ofdemodulating(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pInpData, std::complex<float> *pOutData);

#endif
