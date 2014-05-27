
#include "EqualizerMain.h"

//int RANDOMSEED;

int main()
{
	cout<<"Equalizing starts"<<endl;
	
	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);
	Equalizer Eq(&BS,&User);

//FIFO<complex<float> > EqIn(1,Eq.InBufSz);
//	FIFO<complex<float> > EqOut(1,Eq.OutBufSz);
	complex<float> *pInpData = new complex<float>[Eq.InBufSz];
	complex<float> *pOutData = new complex<float>[Eq.OutBufSz];

//	ReadInputFromFiles(Eq.pInpBuf,(Eq.InBufSz),"LSCELSEqInputReal","LSCELSEqInputImag");
	ReadInputFromFiles(pInpData, (Eq.InBufSz), "LSCELSEqInputReal", "LSCELSEqInputImag");
//GeneRandomInput(Eq.pInpBuf,Eq.InBufSz,"LSCELSEqRandomInputReal","LSCELSEqRandomInputImag");
//GeneRandomInput(Eq.pInpBuf,Eq.InBufSz);
//	Eq.Equalizing(&EqOut);
	Eq.Equalizing(pInpData, pOutData);
	
//	WriteOutputToFiles(&EqOut,(Eq.OutBufSz),"testLSCELSEqOutputReal","testLSCELSEqOutputImag");
	WriteOutputToFiles(pOutData, (Eq.OutBufSz), "testLSCELSEqOutputReal", "testLSCELSEqOutputImag");
//WriteOutputToFiles(&EqOut,(Eq.OutBufSz),"LSCELSEqRandomOutputReal","LSCELSEqRandomOutputImag");
//ReadOutput(&EqOut,(Eq.OutBufSz));

	delete[] pInpData;
	delete[] pOutData;
	
	cout<<"Equalizing ends"<<endl;

	return 0;
}
