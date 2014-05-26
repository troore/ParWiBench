
#include <complex>
#include <fftw3.h>
#include "UserPara.h"
#include "BSPara.h"
#include "FIFO.h"

class OFDM
{
 private:
	bool PSFlag;
	bool BufFlag;
	int NIFFT;
	int CPLen;
	int NumULSymbSF;
	int NumLayer;
	int NumRxAntenna;
	int SymNum;

	// "0" for user, "1" for basestation
	//	int user_flag;
	
	//	complex<float>** pInpData;
	//	complex<float>** pOutData;
	//	complex<float>* out;
	//	fftwf_plan ifftplan;
	//	fftwf_plan fftplan;

 public:
	//	int InBufSz[2];
	int InBufSz;
	//SCFDMAModulation's FIFO
	//	FIFO<complex<float> > *pInpBuf;
	//	int OutBufSz[2];
	int OutBufSz;

	// Default constructor
	OFDM() {};
    OFDM(UserPara* pUser);
	OFDM(BSPara *pPS);
	~OFDM();

	//    void modulating(FIFO<complex<float> > *pOutBuf);
	//	void demodulating(FIFO<complex<float> >* pOutBuf);
	void modulating(complex<float> *pInpData, complex<float> *pOutData);
	void demodulating(complex<float> *pInpData, complex<float> *pOutData);
};

