
#include "ScrambMain.h"

int RANDOMSEED;

//#define Scramb

int main()
{
#ifdef Scramb
	
	cout<<"Tx scrambling starts"<<endl;
	
	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);
	Scrambler<int> SCRB(&User);
//FIFO<int> SCRBIn(1,SCRB.InBufSz);
//	FIFO<int> SCRBOut(1,SCRB.OutBufSz);
	int *pInpSeq = new int[SCRB.InBufSz];
	int *pOutSeq = new int[SCRB.OutBufSz];
	
//	ReadInputFromFiles(SCRB.pInpBuf,(SCRB.InBufSz),"ScrambleInput");
	ReadInputFromFiles(pInpSeq, (SCRB.InBufSz), "ScrambleInput");
//GeneRandomInput(SCRB.pInpBuf,SCRB.InBufSz,"ScrambleRandomInput");
//GeneRandomInput(SCRB.pInpBuf,SCRB.InBufSz);
//	SCRB.Scrambling(&SCRBOut);
	SCRB.Scrambling(pInpSeq, pOutSeq);
	
//	WriteOutputToFiles(&SCRBOut,(SCRB.OutBufSz),"testScrambleOutput");
	WriteOutputToFiles(pOutSeq,(SCRB.OutBufSz),"testScrambleOutput");
//WriteOutputToFiles(&SCRBOut,(SCRB.OutBufSz),"ScrambleRandomOutput");
//ReadOutput(&SCRBOut,(SCRB.OutBufSz));

	delete[] pInpSeq;
	delete[] pOutSeq;

	cout << "Tx scrambling ends" << endl;

	#else

	cout<<"Rx descrambling starts"<<endl;
	
	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);
	Scrambler<float> DSCRB(&BS);
//FIFO<float> DSCRBIn(1,DSCRB.InBufSz);
//	FIFO<float> DSCRBOut(1,DSCRB.OutBufSz);
	
	float *pInpSeq = new float[DSCRB.InBufSz];
	float *pOutSeq = new float[DSCRB.OutBufSz];
	
//	ReadInputFromFiles(DSCRB.pInpBuf,DSCRB.InBufSz,"DescrambleInput");
//	ReadInputFromFiles(pInpSeq, DSCRB.InBufSz, "DescrambleInput");
	ReadInputFromFiles(pInpSeq, DSCRB.InBufSz, "testScrambleOutput");
	for (int i = 0; i < DSCRB.InBufSz; i++)
		pInpSeq[i] = (1 - 2 * pInpSeq[i]);
//GeneRandomInput(DSCRB.pInpBuf,DSCRB.InBufSz,"DescrambleRandomInput");
//GeneRandomInput(DSCRB.pInpBuf,DSCRB.InBufSz);
//	DSCRB.Descrambling(&DSCRBOut);
	DSCRB.Descrambling(pInpSeq, pOutSeq);
//	WriteOutputToFiles(&DSCRBOut,(DSCRB.OutBufSz),"testDescrambleOutput");
	WriteOutputToFiles(pOutSeq, (DSCRB.OutBufSz), "testDescrambleOutput");
	for (int i = 0; i < DSCRB.OutBufSz; i++)
		cout << (int)((1 - pOutSeq[i]) / 2) << "\t";
	cout << endl;
//WriteOutputToFiles(&DSCRBOut,(DSCRB.OutBufSz),"DescrambleRandomOutput");
//ReadOutput(&DSCRBOut,(DSCRB.OutBufSz));

	delete[] pInpSeq;
	delete[] pOutSeq;

	cout << "Rx descrambling ends" << endl;

	#endif
	
	return 0;
}



