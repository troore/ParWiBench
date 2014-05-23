
#include "main.h"

//#define TxRateM

int RANDOMSEED;

int main()
{
	#ifdef TxRateM
	
	cout<<"Tx RateMatching starts"<<endl;
	
	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);
	RateMatcher TxRM(&User);
//FIFO<int> TxRMIn(1,TxRM.InBufSz);
//	FIFO<int> TxRMOut(1,TxRM.OutBufSz);
	int *pInpData = new int[TxRM.InBufSz];
	int *pOutData = new int[TxRM.OutBufSz];
//	ReadInputFromFiles(TxRM.pTxInpBuf,(TxRM.InBufSz),"TxRateMatchInput");
	ReadInputFromFiles(pInpData,(TxRMInBufSz),"TxRateMatchInput");
//GeneRandomInput(TxRM.pInpBuf,TxRM.InBufSz,"TxRateMatchRandomInput");
//GeneRandomInput(TxRM.pInpBuf,TxRM.InBufSz);
//	TxRM.TxRateMatching(&TxRMOut);
	TxRM.TxRateMatching(pInpData, pOutData);
//	WriteOutputToFiles(&TxRMOut,(TxRM.OutBufSz),"testTxRateMatchOutput");
	WriteOutputToFiles(pOutData,(TxRM.OutBufSz),"testTxRateMatchOutput");
//ReadOutput(&TxRMOut,(TxRM.OutBufSz));

	delete[] pInpData;
	delete[] pOutData;
	
	cout<<"Tx RateMatching ends"<<endl;

	#else

	cout<<"Rx RateMatching starts"<<endl;
	
	BSPara BS;
	BS.initBSPara();
//	UserPara User(&BS);
	RateMatcher RxRM(&BS);
//FIFO<float> RxRMIn(1,RxRM.InBufSz);
//	FIFO<float> RxRMOut(1,RxRM.OutBufSz);
	float *pInpData = new float[RxRM.InBufSz];
	float *pOutData = new float[RxRM.OutBufSz];
	int *pHard = new int[RxRM.OutBufSz];
//	ReadInputFromFiles(RxRM.pRxInpBuf,(RxRM.InBufSz),"RxRateMatchInput");
	ReadInputFromFiles(pInpData,(RxRM.InBufSz),"RxRateMatchInput");
//GeneRandomInput(RxRM.pInpBuf,RxRM.InBufSz,"RxRateMatchRandomInput");
//GeneRandomInput(RxRM.pInpBuf,RxRM.InBufSz);
//	RxRM.RxRateMatching(&RxRMOut);
	RxRM.RxRateMatching(pInpData, pOutData, pHard);
//	WriteOutputToFiles(&RxRMOut,(RxRM.OutBufSz),"testRxRateMatchOutput");
	WriteOutputToFiles(pOutData,(RxRM.OutBufSz),"testRxRateMatchOutput");
//ReadOutput(&RxRMOut,(RxRM.OutBufSz));
	
	cout<<"Rx RateMatching ends"<<endl;

	delete[] pInpData;
	delete[] pOutData;
	delete[] pHard;
	
	#endif
	
	return 0;
}


