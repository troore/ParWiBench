
#ifndef __SCRAMBLER_H_
#define __SCRAMBLER_H_

//#pragma once
#include <cmath>
#include "BSPara.h"
#include "UserPara.h"
#include "FIFO.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

template <class T>
class Scrambler
{
 private:
	bool PSFlag;
	bool BufFlag;
	int NInfoBits;
	int NumLayer;

	int NumULSymbSF;
	int MDFT;
	int MQAM;

	//	int n_init[31];

	/*
	int* pInpSeq;
	int* pScrambSeq;
	int* pOutSeq;
	*/
	//	T *pInpSeq;
	//	T *pScrambSeq;
	//	T *pOutSeq;
	int *pScrambInt;

 public:
	//	int InBufSz[2];
	int InBufSz;
	//Scrambler's FIFO
	//	FIFO<int> *pInpBuf;
	//	FIFO<T> *pInpBuf;
	//	int OutBufSz[2];
	int OutBufSz;

	// Default constructor
	Scrambler() {}
	Scrambler(BSPara* pBS);
	Scrambler(UserPara* pUser);
	~Scrambler();

	void GenScrambInt();
	
	//  void Scrambling(FIFO<int> *pInpBuf, FIFO<int> *pOutBuf);
	//	void Scrambling(FIFO<T> *pOutBuf);
	void Scrambling(T *pInpSeq, T *pOutSeq);
	//	void Descrambling(FIFO<T> *pOutBuf);
	void Descrambling(T *pInpSeq, T *pOutBuf);
};

#endif
