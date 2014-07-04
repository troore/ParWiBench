
#include "LTEUplink.h"


using namespace std;

void SendToChannel(complex<float> *pTxSend, int n)
{
	WriteOutputToFiles(pTxSend, n, "TxImag.dat", "TxReal.dat");
}

int main(int argc, char *argv[])
{
//	int SNRArrayLen = 1;
	int numSFrames = 1/*MAX_SFRAMES*/;
	
	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);

	int DataK = BS.DataLengthPerUser;
	int LastK = DataK % (BS.BlkSize);
	int NumBlock = (DataK - LastK) / (BS.BlkSize) + 1;

	///////////////////// construct the kernels ////////////////////
	Turbo TxTurbo(&User);
	RateMatcher TxRM(&User);
	Scrambler<int> TxSCRB(&User);
	Modulation TxMod(&User);
	TransformPrecoder TxTransPre(&User);
	ResMapper TxResMap(&User);
	OFDM TxOFDM(&User);
	Sync TxSync(&User);
//	Channel TxCRx(&BS);
	/*
	  OFDM RxOFDM(&BS);
	  ResMapper RxResMap(&BS);
	  Equalizer RxEq(&BS, &User);
	  TransformPrecoder RxTransPre(&BS);
	  Modulation RxMod(&BS);
	  Scrambler<float> RxSCRB(&BS);
	  RateMatcher RxRM(&BS);
	  Turbo RxTurbo(&BS);
	*/
	//////////////////// Completed construction of the kernels/////////////////

	////////////////// allocate the FIFOs /////////////////////////
//	FIFO<int> RxFileSink(1,RxTbD.OutBufSz);
	//////////////////End of allocation of FIFOs /////////////////////////
	
	/** Allocate inputs and outputs **/
	// Turbo
	int *pTxTbInp = new int[TxTurbo.InBufSz];
	int *pTxTbOut = new int[TxTurbo.OutBufSz];
//	float *pRxTbInp = new float[RxTurbo.InBufSz];
//	int *pRxTbOut = new int[RxTurbo.OutBufSz];

	// Rate matching
	int *pTxRMInp = new int[TxRM.InBufSz];
	int *pTxRMOut = new int[TxRM.OutBufSz];
//	float *pRxRMInp = new float[RxRM.InBufSz];
//	float *pRxRMOut = new float[RxRM.OutBufSz];
//	int *pRxRMHard = new int[RxRM.OutBufSz];

	// Scrambling
	int *pTxSCRBInp = new int[TxSCRB.InBufSz];
	int *pTxSCRBOut = new int[TxSCRB.OutBufSz];
//	float *pRxSCRBInp = new float[RxSCRB.InBufSz];
//	float *pRxSCRBOut = new float[RxSCRB.OutBufSz];

	// Modulation
	int *pTxModInp = new int[TxMod.InBufSz];
	complex<float> *pTxModOut = new complex<float>[TxMod.OutBufSz];
//	complex<float> *pRxModInp = new complex<float>[RxMod.InBufSz];
//	float *pRxModOut = new float[RxMod.OutBufSz];
//	int *pRxModHD = new int[RxMod.OutBufSz];

	// Transform precoding
	complex<float> *pTxTransPreInp = new complex<float>[TxTransPre.InBufSz];
	complex<float> *pTxTransPreOut = new complex<float>[TxTransPre.OutBufSz];
//	complex<float> *pRxTransPreInp = new complex<float>[RxTransPre.InBufSz];
//	complex<float> *pRxTransPreOut = new complex<float>[RxTransPre.OutBufSz];
	
	// Resource mapping
	complex<float> *pTxResMapInp = new complex<float>[TxResMap.InBufSz];
	complex<float> *pTxResMapOut = new complex<float>[TxResMap.OutBufSz];
//	complex<float> *pRxResMapInp = new complex<float>[RxResMap.InBufSz];
//	complex<float> *pRxResMapOut = new complex<float>[RxResMap.OutBufSz];

	// Equalizing
//	complex<float> *pRxEqInp = new complex<float>[RxEq.InBufSz];
//	complex<float> *pRxEqOut = new complex<float>[RxEq.OutBufSz];

	// OFDM
	complex<float> *pTxOFDMInp = new complex<float>[TxOFDM.InBufSz];
	complex<float> *pTxOFDMOut = new complex<float>[TxOFDM.OutBufSz];
//	complex<float> *pRxOFDMInp = new complex<float>[RxOFDM.InBufSz];
//	complex<float> *pRxOFDMOut = new complex<float>[RxOFDM.OutBufSz];

	// Synchronization
	complex<float> *pTxSyncInp = new complex<float>[TxSync.InBufSz];
	complex<float> *pTxSyncOut = new complex<float>[TxSync.OutBufSz];
	
	/** End of allocations **/

	FILE *fptr = NULL;

	int *pTxDS = new int[DataK];
	int *pRxFS = new int[DataK];
	
	for (int nsf = 0; nsf < numSFrames; nsf++)
	{
		////////////////////////// Run Subframe //////////////////////////////////
		int RANDOMSEED = (nsf + 1) * (/*nsnr*/1 + 2);

		//	GenerateLTEChainInput(TxTbE.pInpBuf,DataK,pTxDS);
		GenerateLTEChainInput(pTxTbInp, DataK, pTxDS, RANDOMSEED);

		WriteOutputToFiles(pTxTbInp, DataK, "TxBitStream.dat");

		/*
		  cout << "Turbo Tx Input" << endl;
		  for (int i = 0; i < DataK; i++)
		  cout << pTxTbInp[i] << "\t";
		  cout << endl;
		*/
			

		//	TxTbE.TurboEncoding(TxRM.pInpBuf);
		TxTurbo.TurboEncoding(pTxTbInp, pTxTbOut);

		//	cout << "RateMatching Tx Input" << endl;
		for (int i = 0; i < TxRM.InBufSz; i++)
		{
			pTxRMInp[i] = pTxTbOut[i];
			//	cout << pTxRMInp[i] << "\t";
		}
		//	cout << endl;

		TxRM.TxRateMatching(pTxRMInp, pTxRMOut);

		//	cout << "Scrambling Tx Input" << endl;
		for (int i = 0; i < TxSCRB.InBufSz; i++)
		{
			pTxSCRBInp[i] = pTxRMOut[i];
			//		cout << pTxSCRBInp[i] << "\t";
		}
		//	cout << endl;
			
		TxSCRB.Scrambling(pTxSCRBInp, pTxSCRBOut);

		//	cout << "Modulating Tx Input" << endl;
		for (int i = 0; i < TxMod.InBufSz; i++)
		{
			pTxModInp[i] = pTxSCRBOut[i];
			//		cout << pTxModInp[i] << "\t";
		}
		//	cout << endl;

		TxMod.Modulating(pTxModInp, pTxModOut);

		for (int i = 0; i < TxTransPre.InBufSz; i++)
		{
			pTxTransPreInp[i] = pTxModOut[i];
		}

		TxTransPre.TransformPrecoding(pTxTransPreInp, pTxTransPreOut);

		for (int i = 0; i < TxResMap.InBufSz; i++)
		{
			pTxResMapInp[i] = pTxTransPreOut[i];
		}

		TxResMap.SubCarrierMapping(pTxResMapInp, pTxResMapOut);
			
		for (int i = 0; i < TxOFDM.InBufSz; i++)
		{
			pTxOFDMInp[i] = pTxResMapOut[i];
		}

		TxOFDM.modulating(pTxOFDMInp, pTxOFDMOut);

		for (int i = 0; i < TxSync.InBufSz; i++)
		{
			pTxSyncInp[i] = pTxOFDMOut[i];
		}

		TxSync.AddPreamble(pTxSyncInp, pTxSyncOut);

		// Send modulated data to air channel
		SendToChannel(pTxSyncOut, TxSync.OutBufSz);


		////////////////////////// END Run Subframe/////////////////////////////////
	}

	delete[] pTxDS;
	delete[] pRxFS;

	/** Deallocation **/
	// Turbo
	delete[] pTxTbInp;
	delete[] pTxTbOut;
//	delete[] pRxTbInp;
//	delete[] pRxTbOut;

	// Rate matching
	delete[] pTxRMInp;
	delete[] pTxRMOut;
//	delete[] pRxRMInp;
//	delete[] pRxRMOut;
//	delete[] pRxRMHard;

	// Scrambling
	delete[] pTxSCRBInp;
	delete[] pTxSCRBOut;
//	delete[] pRxSCRBInp;
//	delete[] pRxSCRBOut;

	// Modulation
	delete[] pTxModInp;
	delete[] pTxModOut;
//	delete[] pRxModInp;
//	delete[] pRxModOut;

	// Transform precoding
	delete[] pTxTransPreInp;
	delete[] pTxTransPreOut;
//	delete[] pRxTransPreInp;
//	delete[] pRxTransPreOut;
	
	// Resouce mapping
	delete[] pTxResMapInp;
	delete[] pTxResMapOut;
//	delete[] pRxResMapInp;
//	delete[] pRxResMapOut;

	// OFDM
	delete[] pTxOFDMInp;
	delete[] pTxOFDMOut;

	// Sycnronization
	delete[] pTxSyncInp;
	delete[] pTxSyncOut;
//	delete[] pRxSyncInp;
//	delete[] pRxSyncOut;

	/** End of deallocation**/

	return 0;
}
