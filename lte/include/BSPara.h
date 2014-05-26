#pragma once
#include <complex>
#include <iostream>
/////////// Single User Base Station Parameters/////////////////

#define MAX_BLOCK_SIZE 6144

using namespace std;

class BSPara
{
	public:
		int NumULSymbSF;

		int NIFFT;
		int CPLen;
		int RSU;
		int RSV;

		int DMRSSymbPos[2];
		int SCLocPattern;
		int MDFTPerUser;
		int MQAMPerUser;
		int NumLayerPerUser;
		int DataLengthPerUser;

		int EstimationMethod;
		int NumRxAntenna;

		bool ProcessingStatusFlag;
		bool BSParameterPrintFlag;
		bool BufferSizeFlag;

		/* Turbo parameters */
		int Rate;
		int gens[2];
		int n_gens;
		int constraint_length;
		int BlkSize;
		int n_iterations;
		/* Turbo parameters over... */
		
		BSPara(void);
		void initBSPara(void);
		~BSPara(void);
};
