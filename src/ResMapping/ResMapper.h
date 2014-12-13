
#ifndef __RESMAPPER_H_
#define __RESMAPPER_H_

#include <complex>

#include "lte_phy.h"

#define PI 3.14159265358979

void SubCarrierMapping(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, float *pOutBuf);
void SubCarrierDemapping(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, float *pOutBuf);

#endif
