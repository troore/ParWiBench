#ifndef __TRANSFORM_PRECODER_H_
#define __TRANSFORM_PRECODER_H_

#include <iostream>
#include <complex>
#include <fftw3.h>

#include "lte_phy.h"

void TransformPrecoding(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pInpSeq, std::complex<float> *pDataMatrix);
void TransformDecoding(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pDataMatrix, std::complex<float> *pDecSeq);

#endif
