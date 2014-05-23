
#include "main.h"

int RANDOMSEED;

//#define SCMapper

int main()
{
	
#ifdef SCMapper
	
	cout << "Resource mapping starts" << endl;
	
	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);
	RESMapper SCM(&User);
	//FIFO<complex<float> > SCMIn(1,SCM.InBufSz);
//	FIFO<complex<float> > SCMOut(1,SCM.OutBufSz);
	complex<float> *pInpData = new complex<float>[SCM.InBufSz];
	complex<float> *pOutData = new complex<float>[SCM.OutBufSz];
	
//	ReadInputFromFiles(SCM.pInpBuf,(SCM.InBufSz),"SubCarrierMapInputReal","SubCarrierMapInputImag");
	ReadInputFromFiles(pInpData, (SCM.InBufSz), "SubCarrierMapInputReal","SubCarrierMapInputImag");
	//GeneRandomInput(SCM.pInpBuf,SCM.InBufSz,"SubCarrierMapRandomInputReal","SubCarrierMapRandomInputImag");
	//GeneRandomInput(SCM.pInpBuf,SCM.InBufSz);
//	SCM.SubCarrierMapping(&SCMOut);
	SCM.SubCarrierMapping(pInpData, pOutData);
	
//	WriteOutputToFiles(&SCMOut,(SCM.OutBufSz),"testSubCarrierMapOutputReal","testSubCarrierMapOutputImag");
	WriteOutputToFiles(pOutData, (SCM.OutBufSz), "testSubCarrierMapOutputReal","testSubCarrierMapOutputImag");
	//ReadOutput(&SCMOut,(SCM.OutBufSz));
	
	delete[] pInpData;
	delete[] pOutData;

	cout << "Resource mapping ends" << endl;

	#else

	cout<<"Resource demapping starts"<<endl;

	BSPara BS;
	BS.initBSPara();
//	UserPara User(&BS);
	RESMapper SCD(&BS);
	//FIFO<complex<float> > SCDIn(1,SCD.InBufSz);
//	FIFO<complex<float> > SCDOut(1,SCD.OutBufSz);
	
	complex<float> *pInpData = new complex<float>[SCD.InBufSz];
	complex<float> *pOutData = new complex<float>[SCD.OutBufSz];
	
//	ReadInputFromFiles(SCD.pInpBuf,(SCD.InBufSz),"SubCarrierDemapInputReal","SubCarrierDemapInputImag");
	ReadInputFromFiles(pInpData, (SCD.InBufSz), "SubCarrierDemapInputReal","SubCarrierDemapInputImag");
	//GeneRandomInput(SCD.pInpBuf,SCD.InBufSz,"SubCarrierDemapRandomInputReal","SubCarrierDemapRandomInputImag");
	//GeneRandomInput(SCD.pInpBuf,SCD.InBufSz);
//	SCD.SubCarrierDemapping(&SCDOut);
	SCD.SubCarrierDemapping(pInpData, pOutData);
//	WriteOutputToFiles(&SCDOut,(SCD.OutBufSz),"testSubCarrierDemapOutputReal","testSubCarrierDemapOutputImag");
	WriteOutputToFiles(pOutData, (SCD.OutBufSz), "testSubCarrierDemapOutputReal", "testSubCarrierDemapOutputImag");
	//ReadOutput(&SCDOut,(SCD.OutBufSz));

	delete[] pInpData;
	delete[] pOutData;

	cout<<"Resource demapping ends"<<endl;

	#endif
	
	return 0;
}




