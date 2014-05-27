
#include "OFDMMain.h"

//int RANDOMSEED;

//#define MOD

int main(int argc, char *argv[])
{
	#ifdef MOD
	
	std::cout << "OFDM modulation starts" << std::endl;
	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);
	OFDM SCFM(&User);
	//FIFO<complex<float> > SCFMIn(1,SCFM.InBufSz);
//	FIFO<complex<float> > SCFMOut(1,SCFM.OutBufSz);
	complex<float> *pInpData = new complex<float>[SCFM.InBufSz];
	complex<float> *pOutData = new complex<float>[SCFM.OutBufSz];

//	ReadInputFromFiles(SCFM.pInpBuf,(SCFM.InBufSz),"ModulationInputReal","ModulationInputImag");
	ReadInputFromFiles(pInpData, (SCFM.InBufSz), "ModulationInputReal", "ModulationInputImag");
	//GeneRandomInput(SCFM.pInpBuf,SCFM.InBufSz,"SCFDMAModulationRandomInputReal","SCFDMAModulationRandomInputImag");
	//GeneRandomInput(SCFM.pInpBuf,SCFM.InBufSz);
//	SCFM.modulating(&SCFMOut);
	SCFM.modulating(pInpData, pOutData);
	
//	WriteOutputToFiles(&SCFMOut,(SCFM.OutBufSz),"testModulationOutputReal","testModulationOutputImag");
	WriteOutputToFiles(pOutData, (SCFM.OutBufSz), "testModulationOutputReal", "testModulationOutputImag");
	//ReadOutput(&SCFMOut,(SCFM.OutBufSz));

	delete[] pInpData;
	delete[] pOutData;
	
	std::cout << "OFDM modulation ends" << std::endl;
	
	#else

	std::cout <<"OFDM demodulation starts"<< std::endl;
	BSPara BS;
	BS.initBSPara();
//	UserPara User(&BS);
	OFDM SCFD(&BS);
	//FIFO<complex<float> > SCFDIn(1,SCFD.InBufSz);
//	FIFO<complex<float> > SCFDOut(1,SCFD.OutBufSz);

	complex<float> *pInpData = new complex<float>[SCFD.InBufSz];
	complex<float> *pOutData = new complex<float>[SCFD.OutBufSz];
	
//	ReadInputFromFiles(SCFD.pInpBuf,(SCFD.InBufSz),"DemodulationInputReal","DemodulationInputImag");
	ReadInputFromFiles(pInpData, (SCFD.InBufSz), "DemodulationInputReal", "DemodulationInputImag");
	//GeneRandomInput(SCFD.pInpBuf,SCFD.InBufSz,"SCFDMADemodulationRandomInputReal","SCFDMADemodulationRandomInputImag");
	//GeneRandomInput(SCFD.pInpBuf,SCFD.InBufSz);
//	SCFD.demodulating(&SCFDOut);
	SCFD.demodulating(pInpData, pOutData);
//	WriteOutputToFiles(&SCFDOut,(SCFD.OutBufSz),"testDemodulationOutputReal","testDemodulationOutputImag");
	WriteOutputToFiles(pOutData, (SCFD.OutBufSz), "testDemodulationOutputReal", "testDemodulationOutputImag");
	//ReadOutput(&SCFDOut,(SCFD.OutBufSz));

	delete[] pInpData;
	delete[] pOutData;
	
	std::cout<<"OFDM demodulation ends"<<std::endl;
	
	#endif
	
	return 0;
}

