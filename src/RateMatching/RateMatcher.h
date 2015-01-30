
#ifndef __RATEMATCHER_H_
#define __RATEMATCHER_H_

#include "lte_phy.h"

void TxRateMatching(LTE_PHY_PARAMS *lte_phy_params, int *pcMatrix, int *pcSeq);
void RxRateMatching(LTE_PHY_PARAMS *lte_phy_params, float *pLLRin, float *pLLRout, int *pHD);

#endif
