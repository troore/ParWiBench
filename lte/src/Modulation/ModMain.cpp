
#include "ModMain.h"

//#define Mod

int RANDOMSEED;

using namespace std;

int main()
{
#ifdef Mod
	
	cout<<"Modulation starts"<<endl;
	
	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);
	Modulation Md(&User);
//FIFO<int> MdIn(1,Md.InBufSz);
//	FIFO<complex<float> > MdOut(1,Md.OutBufSz);
	int *pBitsSeq = new int[Md.InBufSz];
	complex<float> *pQAMSeq = new complex<float>[Md.OutBufSz];
	
//	ReadInputFromFiles(Md.pInpBuf,(Md.InBufSz),"ModulationInput");
	ReadInputFromFiles(pBitsSeq, (Md.InBufSz), "ModulationInput");
//GeneRandomInput(Md.pInpBuf,Md.InBufSz,"ModulationRandomInput");
//GeneRandomInput(Md.pInpBuf,Md.InBufSz);
//	Md.Modulating(&MdOut);
	Md.Modulating(pBitsSeq, pQAMSeq);
	
//	WriteOutputToFiles(&MdOut,(Md.OutBufSz),"testModulationRandomOutputReal","testModulationRandomOutputImag");
	WriteOutputToFiles(pQAMSeq, (Md.OutBufSz), "testModulationRandomOutputReal","testModulationRandomOutputImag");
//ReadOutput(&MdOut,(Md.OutBufSz));

	delete[] pBitsSeq;
	delete[] pQAMSeq;

	cout<<"Modulation ends"<<endl;
	
	#else
	
	cout<<"Demodulation starts"<<endl;
	
	BSPara BS;
	BS.initBSPara();
//	UserPara User(&BS);
	Modulation Dmd(&BS);
//FIFO<complex<float> > DmdIn(1,Dmd.InBufSz);
//	FIFO<float> DmdOut(1,Dmd.OutBufSz);
	complex<float> *pDecQAMSeq = new complex<float>[Dmd.InBufSz];
	float *pLLR = new float[Dmd.OutBufSz];
	int *pHD = new int[Dmd.OutBufSz];
	float awgn_sigma = 0.193649;//this value is for the standard input  see "AWGNSigma"
//	ReadInputFromFiles(Dmd.pInpBuf,(Dmd.InBufSz),"DemodulationInputReal","DemodulationInputImag");
//	ReadInputFromFiles(pDecQAMSeq, (Dmd.InBufSz), "DemodulationInputReal", "DemodulationInputImag");
	ReadInputFromFiles(pDecQAMSeq, (Dmd.InBufSz), "testModulationRandomOutputReal", "testModulationRandomOutputImag");
//GeneRandomInput(Dmd.pInpBuf,Dmd.InBufSz,"DemodulationRandomInputReal","DemodulationRandomInputImag");
//GeneRandomInput(Dmd.pInpBuf,Dmd.InBufSz);
//	Dmd.Demodulating(&DmdOut,awgn_sigma);
	Dmd.Demodulating(pDecQAMSeq, pLLR, awgn_sigma);
//	Dmd.Demodulating(pDecQAMSeq, pHD);
	
//	WriteOutputToFiles(&DmdOut,(Dmd.OutBufSz),"testDemodulationOutput");
	WriteOutputToFiles(pLLR, (Dmd.OutBufSz), "testDemodulationOutput");
//	WriteOutputToFiles(pHD, (Dmd.OutBufSz), "testDemodulationOutput");
//ReadOutput(&DmdOut,(Dmd.OutBufSz));

	delete[] pDecQAMSeq;
	delete[] pLLR;
	
	cout<<"Demodulation ends"<<endl;


	#endif
	
	return 0;
}

