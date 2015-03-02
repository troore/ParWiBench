
#ifndef __SCRAMBLER_H_
#define __SCRAMBLER_H_

//#include <stdio.h>
//#include <stdlib.h>

#include "lte_phy.h"

void GenScrambInt(int *pScrambInt, int n);
void Scrambling(LTE_PHY_PARAMS *lte_phy_params, int *pInpSeq, int *pOutSeq);

void Descrambling(LTE_PHY_PARAMS *lte_phy_params, float *pInpSeq, float *pOutBuf);

#endif
