
#ifndef __RESMAPPER_H_
#define __RESMAPPER_H_

#include <complex>

#include "lte_phy.h"

#define PI 3.14159265358979

void geneDMRS(std::complex<float> *pDMRS, int N_layer, int N_dft);
void SubCarrierMapping(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pInpData, std::complex<float> *pOutBuf);
void SubCarrierDemapping(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pInpData, std::complex<float> *pOutBuf);

#endif
