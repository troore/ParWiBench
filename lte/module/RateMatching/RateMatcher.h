
#ifndef __RATEMATCHER_H_
#define __RATEMATCHER_H_

//#pragma once
#include <cmath>
#include "BSPara.h"
#include "UserPara.h"
#include "FIFO.h"
#include "SubblockInterleaver_lte.h"
#include <iostream>

using namespace std;

class RateMatcher
{
 private:
	bool PSFlag;
	bool BufFlag;
	int NumULSymbSF;

	int MDFT;
	int MQAM;
	int NumLayer;

	int DataLength;

	int Rate;

	//	int* pLengthSet;
	//	int LastBlockLen;
	int NumBlock;
	//	int EncDataLen;

	int NInfoBits;

	//	int InpBlockShift;
	//	int OutBlockShift;
	//	int iSeqLength;

	int NumExtraBits;
	//	int** pcMatrix;
	//	int* pcSeq;

	//	int EncLen;
	//	int *pEncS;

	//	float* pLLRin;
	//	float* pLLRout;
	//	int *pHD;
		
	SubblockInterleaver_lte<int,int> SbInterleaver;
	SubblockInterleaver_lte<int,float> SbDeInterleaver;

 public:
	//	int InBufSz[2];
	//	int OutBufSz[2];
	int InBufSz;
	int OutBufSz;
	//TxRateMatcher's FIFO
	//	FIFO<int> *pTxInpBuf;
	//RxRateMatcher's FIFO
	//	FIFO<float> *pRxInpBuf;


	//Default constructor
	RateMatcher() {};
	RateMatcher(BSPara* pBS);
	RateMatcher(UserPara* pUser);
	~RateMatcher();
	
	//  void RxRateMatching(FIFO<float>* pInpBuf,FIFO<float>* pOutBuf);
	//	void TxRateMatching(FIFO<int> *pOutBuf);
	//	void RxRateMatching(FIFO<float>* pOutBuf);

	void TxRateMatching(int *pcMatrix, int *pcSeq);
	void RxRateMatching(float *pLLRin, float *pLLRout, int *pHD);

	//	int* GetpcSeq(void) const{return pcSeq;}
	//	int* GetpEncS(void) const{return pEncS;}
	//	float* GetpLLRout(void) const {return pLLRout;}
	//	int* GetpHD(void) const {return pHD;}

};
#endif
