
#ifndef __USERPARA_H_
#define __USERPARA_H_

//#pragma once
#include <complex>
//#include "gauss.h"
#include <cmath>
#include <ctime>
#include <iostream>
#include "BSPara.h"

#define PI 3.141592653589793

using namespace std;

class UserPara
{
 private:
	int** pPrimeTable;
	int Nzc;
	void geneDMRS(void);
	BSPara *VpBS;
	void initUserPara(void);
	
 public:
	int NIFFT;
	int CPLen;
	int NumULSymbSF;

	int RSu;
	int RSv;

	int MQAM;
	int MDFT;
	int NumLayer;
	int ncs[2];
	int SCLoc;
	int DataLength;

	int DMRSSymbPos[2];

	int NInfoBits;
	int QAMLen;
	bool ProcessingStatusFlag;
	bool BufferSizeFlag;
	complex<float>*** pDMRS;

	/* Turbo parameters */
	int gens[2];
	int n_gens;
	int constraint_length;
	int BlkSize;
	int n_iterations;
	/* Turbo parameters over... */

    UserPara(BSPara *pBS);
    complex<float>*** GetpDMRS(void) const {return pDMRS;}
    ~UserPara(void);
};

#endif
