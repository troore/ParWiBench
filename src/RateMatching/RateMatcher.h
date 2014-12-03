
#ifndef __RATEMATCHER_H_
#define __RATEMATCHER_H_

#include <cmath>
#include <iostream>

#include "lte_phy.h"
#include "SubblockInterleaver_lte.h"

//SubblockInterleaver_lte<int,int> SbInterleaver;
//SubblockInterleaver_lte<int,float> SbDeInterleaver;

void TxRateMatching(LTE_PHY_PARAMS *lte_phy_params, int *pcMatrix, int *pcSeq);
void RxRateMatching(LTE_PHY_PARAMS *lte_phy_params, float *pLLRin, float *pLLRout, int *pHD);

#endif
