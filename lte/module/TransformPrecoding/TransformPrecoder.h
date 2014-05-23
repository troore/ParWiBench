#ifndef __TRANSFORM_PRECODER_H_
#define __TRANSFORM_PRECODER_H_

//#pragma once
#include <iostream>
#include <complex>
#include <fftw3.h>
#include "BSPara.h"
#include "UserPara.h"
#include "FIFO.h"

using namespace std;

class TransformPrecoder
{
 private:
	bool PSFlag;
	bool BufFlag;
	int QAMLen;
	int NumULSymbSF;
	int MDFT;
	int NumLayer;
	
	int QAM;
	int SymbNum;
	
	//	complex<float>* pQAMSeq;
	//	complex<float>* pDecQAMSeq;
	//	complex<float>** pDataMatrix;
	
	//	fftwf_plan fftplan;
	//	fftwf_plan ifftplan;
 public:
	//	int InBufSz[2];
	//Transform Precoder's FIFO
	//	FIFO<complex<float> > *pInpBuf;
	//	int OutBufSz[2];
	int InBufSz;
	int OutBufSz;

	// Default constructor
	TransformPrecoder() {}
	TransformPrecoder(BSPara* pBS);
    TransformPrecoder(UserPara* pUser);
    ~TransformPrecoder();

	//    void TransformPrecoding(FIFO<complex<float> > *pInpBuf, FIFO<complex<float> > *pOutBuf);
	//    void TransformPrecoding(FIFO<complex<float> > *pOutBuf);
	void TransformPrecoding(complex<float> *pQAMSeq, complex<float> *pDataMatrix);
	//    void TransformDecoding(FIFO<complex<float> >* pInpBuf,FIFO<complex<float> >* pOutBuf);
	//    void TransformDecoding(FIFO<complex<float> >* pOutBuf);
	void TransformDecoding(complex<float> *pDataMatrix, complex<float> *pDecQAMSeq);

};

#endif
