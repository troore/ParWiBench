
#ifndef __MODULATION_H_
#define __MODULATION_H_
//#pragma once
#include <complex>
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>

#include "BSPara.h"
#include "UserPara.h"
#include "FIFO.h"

using namespace std;

class Modulation
{
 private:
	bool PSFlag;
	bool BufFlag;
	bool SCFDMAFlag;
	
	float** pBPSKtable;
	float** pQPSKtable;
	float** pQAM16table;
	float** pQAM64table;

	int NumLayer;
	int NInfoBits;
	int QAMLen;


	int BitsPerSymb;
	
	float** pTable;
	
	//	int* pBitsSeq;
	//	complex<float>* pQAMSeq;

	int NumULSymbSF;
	int NumRxAntenna;

	int MDFT;
	int MQAM;

	//	int *bitmap;

	//	complex<float>* pDecQAMSeq;
	//	float* pLLR;

	//	complex<float> ***VpEqW;
	//	complex<float> ***VpHdm;
	//	complex<float> ***pW;
	//	float *pSCFactor;
	//	complex<float> *pSC_Cntx;
	float vecmin(float* pV,int len);
	float eudist(float a, float b, float c, float d);
	void dec2bits(int i, int n, int *bvec);
	//	void InitBitMap(int m);
	void InitModTables();

 public:
	//	int InBufSz[2];
	int InBufSz;
	//Modulation's FIFO
	//	FIFO<int> *pInpBuf;
	// FIFO<float> *pInpBuf;
	//int OutBufSz[2];
	int OutBufSz;

	// Default constructor
	Modulation() {}
	Modulation(BSPara* pBS);
    Modulation(UserPara* pUser);
	// Destructor
	~Modulation(void);
	//    void Modulating(FIFO<int> *pInpBuf, FIFO<complex<float> > *pOutBuf);
	//    void Modulating(FIFO<complex<float> > *pOutBuf);
	void Modulating(int *pBitsSeq, complex<float> *pQAMSeq);
	//  void Demodulating(FIFO<complex<float> >* pInpBuf,FIFO<float>* pOutBuf,float awgnSigma);
	//	void Demodulating(FIFO<float>* pOutBuf,float awgnSigma);
	void Demodulating(complex<float> *pDecQAMSeq, float *pLLR, float awgnSigma);
	//  void Demodulating(FIFO<complex<float> >* pInpBuf,FIFO<float>* pOutBuf,complex<float>*** pEqW,complex<float>*** pHdm,float awgnSigma);
	//	void Demodulating(FIFO<float>* pOutBuf,complex<float>*** pEqW,complex<float>*** pHdm,float awgnSigma);
	void Demodulating(complex<float> *pDecQAMSeq, float *pLLR, complex<float>***pEqW, complex<float>***pHdm, float awgnSigma);

	void Demodulating(complex<float> *pDecQAMSeq, int *pHD);

};

#endif
