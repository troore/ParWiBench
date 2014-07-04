#ifndef __SYNC_H_
#define __SYNC_H_

#include <complex>
#include "UserPara.h"
#include "BSPara.h"

//#define TESTBUFSZ 10

class Sync
{
 private:
	int NIFFT;
	int CPLen;
	int NumULSymbSF;
	int NumLayer;
	int NumRxAntenna;
	
	complex<float> *output_time_synchro;
	int syn_state;
	int n_signal_output;
	int n_data_output;
	int PreambleLen;
	
 public:
	int InBufSz;
	int OutBufSz;

	// Default constructor
	Sync() {};
	Sync(BSPara *BS);
	Sync(UserPara *pUser);

	// Destructor
	~Sync();

	inline int GetPreambleLen() { return PreambleLen; }

	void AddPreamble(complex<float> *pInpData, complex<float> *pOutData);

	int TryDetectPreamble(complex<float> *, int, complex<float> *);
	int TimeSync(complex<float> input, complex<float> *Output, int num_th, int m_th, int mf_th);

	float GetFreqOffset(complex<float> *Input, complex<float> *Output);
	int FreqSync(complex<float> *Input, complex<float> *Output, int len_symbol, float freq_offset);
	
};

#endif
