
#ifndef __EQUALIZER_H_
#define __EQUALIZER_H_

#include <complex>
#include <iostream>
#include <cmath>

#include "lte_phy.h"
#include "ResMapper.h"
#include "matrix.h"
#include "fftw3.h"

void Equalizer_init(LTE_PHY_PARAMS *lte_phy_params);

void Equalizer_cleanup(LTE_PHY_PARAMS *lte_phy_params);

void FDLSEstimation(std::complex<float>** pXt, std::complex<float>** pXtDagger, std::complex<float>** pYt, std::complex<float>** pHTranspose, int NumLayer, int NumRxAntenna);

//	void FDLSEqualization(complex<float>** pHTranspose,int m,int NumLayer);
void FDLSEqualization(std::complex<float> *pInpData, std::complex<float>** pHTranspose, int m, int NumLayer, int NumRxAntenna, int MDFTPerUser, std::complex<float> *pOutData);
//	void FDMSEEEqualization(complex<float>** pHTranspose,int m,int NumLayer, float No);
void FDMMSEEqualization(std::complex<float> *pInpData, std::complex<float>** pHTranspose, int m, int NumLayer, int NumRxAntenna, int MDFTPerUser, float No, std::complex<float> *pOutData);

void LSFreqDomain(std::complex<float> *pInpData, std::complex<float> *pOutData, int MDFT, int NumLayer, int NumRxAntenna);

void Equalizing(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pInpData, complex<float> *pOutData);

#endif
