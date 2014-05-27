
#ifndef __EQUALIZER_H_
#define __EQUALIZER_H_

//#pragma once
#include <complex>
#include <iostream>
#include <cmath>

#include "UserPara.h"
#include "BSPara.h"
#include "FIFO.h"
#include "matrix.h"
#include "fftw3.h"


using namespace std;

class Equalizer
{
	private:
		bool PSFlag;
		bool BufFlag;

		int NumULSymbSF;
		int MDFTPerUser;
		int NumLayerPerUser;

		int NumRxAntenna;
		int EstimationMethod;

		UserPara* VpUser;
		complex<float>*** pDMRS;
		//FIFO<complex<float> >* VpInpBuf;
		FIFO<complex<float> >* VpOutBuf;

		complex<float>** pInpData;
		complex<float>** pOutData;


		complex<float> ***VpCSI;
		complex<float> ***pEqW;
		complex<float> ***pHdm;

		void FDLSEstimation(complex<float>** pXt,complex<float>** pXtDagger,complex<float>** pYt,complex<float>** pHTranspose,int NumLayer);

		//	void FDLSEqualization(complex<float>** pHTranspose,int m,int NumLayer);
		void FDLSEqualization(complex<float> *pInpData, complex<float>** pHTranspose, int m, int NumLayer, complex<float> *pOutData);
		void FDMMSEEqualization(complex<float>** pHTranspose,int m,int NumLayer,float No);

		//void LSFreqDomain(void);
		void LSFreqDomain(complex<float> *pInpData, complex<float> *pOutData);

	public:
		//		int InBufSz[2];
		int InBufSz;
		//Equalizer's FIFO
		//	FIFO<complex<float> >* pInpBuf;
		//	int OutBufSz[2];
		int OutBufSz;

		Equalizer(BSPara* pBS, UserPara* pUser);
		//  void Equalizing(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf);
		//  void Equalizing(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf,complex<float>***pPCSI,float AWGNNo);
		//		void Equalizing(FIFO<complex<float> >* pOutBuf);
		void Equalizing(complex<float> *pInpData, complex<float> *pOutData);
		//	void Equalizing(FIFO<complex<float> >* pOutBuf,complex<float>***pPCSI,float AWGNNo);
		complex<float> ***GetpEqW(void) const {return pEqW;}
		complex<float> ***GetpHdm(void) const {return pHdm;}
		~Equalizer(void);
};

#endif
