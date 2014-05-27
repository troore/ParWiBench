
#ifndef __RESMAPPER_H_
#define __RESMAPPER_H_

#include <complex>
#include "UserPara.h"
#include "FIFO.h"

class ResMapper
{
 private:
	bool PSFlag;
	bool BufFlag;
	int MDFT;
	int NIFFT;
	int NumULSymbSF;
	int NumLayer;
	int SCLoc;
	int SCLocPattern;
	int MDFTPerUser;
	int NumRxAntenna;

	int DMRSSymbPos[2];
	
	complex<float>*** VpDMRS;

	//	complex<float>** pInpData;
	//	complex<float>** pOutData;

 public:
	//	int InBufSz[2];
	int InBufSz;
	//SubCarrierMapper's FIFO
	//	FIFO<complex<float> > *pInpBuf;
	//	int OutBufSz[2];
	int OutBufSz;

	// Default constructor
	ResMapper() {}
    ResMapper(UserPara* pUser);
	ResMapper(BSPara *pBS);
	~ResMapper();
	//    void SubCarrierMapping(FIFO<complex<float> > *pInpBuf, FIFO<complex<float> > *pOutBuf);
	//	void SubCarrierMapping(FIFO<complex<float> > *pOutBuf);
	//	void SubCarrierDemapping(FIFO<complex<float> >* pOutBuf);
	void SubCarrierMapping(complex<float> *pInpData, complex<float> *pOutBuf);
	void SubCarrierDemapping(complex<float> *pInpData, complex<float> *pOutBuf);
};

#endif
