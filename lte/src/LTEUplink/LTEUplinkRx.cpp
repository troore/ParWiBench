
#include "LTEUplink.h"


using namespace std;

/*
void RecvFromChannel(complex<float> *pRxRecv, int n)
{
	ReadInputFromFiles(pRxRecv, n, "TxImag.dat", "TxReal.dat");
}
*/
void Synchronization(complex<float> *output_buffer, Sync *pRxSync)
{
	ifstream fin_real, fin_imag;

	float real, imag;
	complex<float> data;
	int rx_len;

	fin_real.open("TxImag.dat");
	fin_imag.open("TxReal.dat");

	int cnt = 0;
	
	if (fin_real.is_open() && fin_imag.is_open())
	{
		while (!fin_real.eof() && !fin_imag.eof()/* && !rx_len*/)
		{
//			std::cout << cnt++ << std::endl;
			fin_real >> real;
			fin_imag >> imag;
			data = complex<float>(real, imag);

			rx_len = pRxSync->TryDetectPreamble(&data, 1, output_buffer);

//			std::cout << rx_len << std::endl;
		}
	}
	else
	{
		std::cout << "Fail to open file to read. Exiting..." << std::endl;

		exit(1);
	} 

	fin_imag.close();
	fin_real.close();

//	WriteOutputToFiles(pOutData, sync.OutBufSz, "SynchronizationOutputReal", "SynchronizationOutputImag");
}

int main(int argc, char *argv[])
{
	int SNRArrayLen = 1;
	int numSFrames = 1/*MAX_SFRAMES*/;
	
	BSPara BS;
	BS.initBSPara();
	UserPara User(&BS);

	int DataK = BS.DataLengthPerUser;
	int LastK = DataK % (BS.BlkSize);
	int NumBlock = (DataK - LastK) / (BS.BlkSize) + 1;
//	int HDLen = (NumBlock - 1) * BS.Rate * ((BS.BlkSize + 4) + 1 * (LastK + 4));

	float *AWGNSigmaArray = new float[SNRArrayLen];
	
	for(int i = 0; i < SNRArrayLen; i++)
	{
		float x = 10.5 + 0.5 * ((float)i);
		
		AWGNSigmaArray[i] = sqrt((1.50 / ((log((float)((User).MQAM))) / (log(2.0)))) * (pow(10.0, (-x / 10.0))));
		cout << AWGNSigmaArray[i] << endl;
	}

	///////////////////// construct the kernels ////////////////////
	/*
	  Turbo TxTurbo(&User);
	  RateMatcher TxRM(&User);
	  Scrambler<int> TxSCRB(&User);
	  Modulation TxMod(&User);
	  TransformPrecoder TxTransPre(&User);
	  ResMapper TxResMap(&User);
	  OFDM TxOFDM(&User);
	*/
//	Channel TxCRx(&BS);
	Sync RxSync(&BS);

	std::cout << "HAHA:" <<  RxSync.OutBufSz << std::endl;
	
	OFDM RxOFDM(&BS);
	ResMapper RxResMap(&BS);
	Equalizer RxEq(&BS, &User);
	TransformPrecoder RxTransPre(&BS);
	Modulation RxMod(&BS);
	Scrambler<float> RxSCRB(&BS);
	RateMatcher RxRM(&BS);
	Turbo RxTurbo(&BS);
	//////////////////// Completed construction of the kernels/////////////////

	////////////////// allocate the FIFOs /////////////////////////
//	FIFO<int> RxFileSink(1,RxTbD.OutBufSz);
	//////////////////End of allocation of FIFOs /////////////////////////
	
	/** Allocate inputs and outputs **/
	// Turbo
//	int *pTxTbInp = new int[TxTurbo.InBufSz];
//	int *pTxTbOut = new int[TxTurbo.OutBufSz];
	float *pRxTbInp = new float[RxTurbo.InBufSz];
	int *pRxTbOut = new int[RxTurbo.OutBufSz];

	// Rate matching
//	int *pTxRMInp = new int[TxRM.InBufSz];
//	int *pTxRMOut = new int[TxRM.OutBufSz];
	float *pRxRMInp = new float[RxRM.InBufSz];
	float *pRxRMOut = new float[RxRM.OutBufSz];
	int *pRxRMHard = new int[RxRM.OutBufSz];

	// Scrambling
//	int *pTxSCRBInp = new int[TxSCRB.InBufSz];
//	int *pTxSCRBOut = new int[TxSCRB.OutBufSz];
	float *pRxSCRBInp = new float[RxSCRB.InBufSz];
	float *pRxSCRBOut = new float[RxSCRB.OutBufSz];

	// Modulation
//	int *pTxModInp = new int[TxMod.InBufSz];
//	complex<float> *pTxModOut = new complex<float>[TxMod.OutBufSz];
	complex<float> *pRxModInp = new complex<float>[RxMod.InBufSz];
	float *pRxModOut = new float[RxMod.OutBufSz];
	int *pRxModHD = new int[RxMod.OutBufSz];

	// Transform precoding
//	complex<float> *pTxTransPreInp = new complex<float>[TxTransPre.InBufSz];
//	complex<float> *pTxTransPreOut = new complex<float>[TxTransPre.OutBufSz];
	complex<float> *pRxTransPreInp = new complex<float>[RxTransPre.InBufSz];
	complex<float> *pRxTransPreOut = new complex<float>[RxTransPre.OutBufSz];
	
	// Resource mapping
//	complex<float> *pTxResMapInp = new complex<float>[TxResMap.InBufSz];
//	complex<float> *pTxResMapOut = new complex<float>[TxResMap.OutBufSz];
	complex<float> *pRxResMapInp = new complex<float>[RxResMap.InBufSz];
	complex<float> *pRxResMapOut = new complex<float>[RxResMap.OutBufSz];

	// Equalizing
	complex<float> *pRxEqInp = new complex<float>[RxEq.InBufSz];
	complex<float> *pRxEqOut = new complex<float>[RxEq.OutBufSz];

	// OFDM
//	complex<float> *pTxOFDMInp = new complex<float>[TxOFDM.InBufSz];
//	complex<float> *pTxOFDMOut = new complex<float>[TxOFDM.OutBufSz];
	complex<float> *pRxOFDMInp = new complex<float>[RxOFDM.InBufSz];
	complex<float> *pRxOFDMOut = new complex<float>[RxOFDM.OutBufSz];

	// Synchronization
//	complex<float> *pRxSyncInp = new complex<float>[RxSync.InBufSz];
	complex<float> *pRxSyncOut = new complex<float>[RxSync.OutBufSz];
	
	/** End of allocations **/

	FILE *fptr = NULL;
	
	int* pTxDS=new int[DataK];
	int* pRxFS=new int[DataK];

	double start, end, cost;

	start = clock();

	for (int nsnr = 0; nsnr < SNRArrayLen; nsnr++)
	{
		int PacketErr=0;
		int BitErr=0;

		int HPacketErr=0;
		int HBitErr=0;

		for (int nsf = 0; nsf < numSFrames; nsf++)
		{
			////////////////////////// Run Subframe //////////////////////////////////
			//	int RANDOMSEED = (nsf + 1) * (nsnr + 2);

			//	GenerateLTEChainInput(TxTbE.pInpBuf,DataK,pTxDS);
			//	GenerateLTEChainInput(pTxTbInp, DataK, pTxDS, RANDOMSEED);
			//	RecvFromChannel(pRxOFDMInp, RxOFDM.InBufSz);

			//	start = clock();
			Synchronization(pRxSyncOut, &RxSync);
			//	end = clock();
			//	cost = ((end - start) / CLOCKS_PER_SEC) * 1000;

			//	std::cout <<"Synchronization:" << cost << "ms" << std::endl;

			for (int i = 0; i < RxOFDM.InBufSz; i++)
			{
				pRxOFDMInp[i] = pRxSyncOut[i];
			}

			//	start = clock();
			RxOFDM.demodulating(pRxOFDMInp, pRxOFDMOut);
			//	end = clock();
			//	cost = ((end - start) / CLOCKS_PER_SEC) * 1000;

			//	std::cout <<"OFDM Demod:" << cost << "ms" << std::endl;


			for (int i = 0; i < RxResMap.InBufSz; i++)
			{
				pRxResMapInp[i] = pRxOFDMOut[i];
			}

			//	start = clock();
			RxResMap.SubCarrierDemapping(pRxResMapInp, pRxResMapOut);
			//	end = clock();
			//	cost = ((end - start) / CLOCKS_PER_SEC) * 1000;

			//	std::cout <<"Res Demap:" << cost << "ms" << std::endl;

			// RxE.Equalizing(RxTD.pInpBuf,TxCRx.GetpPCSI(),TxCRx.GetAWGNNo());
			
			for (int i = 0; i < RxEq.InBufSz; i++)
			{
				pRxEqInp[i] = pRxResMapOut[i];
			}
			
			//	start = clock();
			RxEq.Equalizing(pRxEqInp, pRxEqOut);
			//	end = clock();
			//	cost = ((end - start) / CLOCKS_PER_SEC) * 1000;

			//	std::cout <<"Equalizing:" << cost << "ms" << std::endl;

			for (int i = 0; i < RxTransPre.InBufSz; i++)
			{
				pRxTransPreInp[i] = pRxEqOut[i];
			}
			
			//	start = clock();
			RxTransPre.TransformDecoding(pRxTransPreInp, pRxTransPreOut);
			//	end = clock();
			//	cost = ((end - start) / CLOCKS_PER_SEC) * 1000;

			//	std::cout <<"DFT:" << cost << "ms" << std::endl;

			//	cout << "Modulating Rx Input" << endl;
			for (int i = 0; i < RxMod.InBufSz; i++)
			{
				pRxModInp[i] = pRxTransPreOut[i];
				//		cout << pRxModInp[i] << "\t";
			}
			//	cout << endl;
			
			// RxD.Demodulating(RxDSCRB.pInpBuf,RxE.GetpEqW(),RxE.GetpHdm(),(AWGNSigmaArray[nsnr]));
			//	start = clock();
			RxMod.Demodulating(pRxModInp, pRxModOut, (AWGNSigmaArray[nsnr]));
			//	end = clock();
			//	cost = ((end - start) / CLOCKS_PER_SEC) * 1000;

			//	std::cout <<"Demodulation:" << cost << "ms" << std::endl;


			//	RxMod.Demodulating(pRxModInp, pRxModHD);

			//	cout << "Scrambling Rx Input" << endl;
			for (int i = 0; i < RxSCRB.InBufSz; i++)
			{
				//	pRxSCRBInp[i] = (1 - 2 * (pRxModOut[i] > 0));
				pRxSCRBInp[i] = -pRxModOut[i];
				//	pRxSCRBInp[i] = (float)(1 - 2 * pRxModHD[i]);
				//		cout << pRxSCRBInp[i] << "\t";
			}
			//	cout << endl;

			//	start = clock();
			RxSCRB.Descrambling(pRxSCRBInp, pRxSCRBOut);
			//	end = clock();
			//	cost = ((end - start) / CLOCKS_PER_SEC) * 1000;

			//	std::cout <<"Descrambling:" << cost << "ms" << std::endl;

			//	cout << "RateMatching Rx Input" << endl;
			for (int i = 0; i < RxRM.InBufSz; i++)
			{
				pRxRMInp[i] = pRxSCRBOut[i];
				//		cout << pRxRMInp[i] << "\t";
			}
			//	cout << endl;
		   
			//	start = clock();
			RxRM.RxRateMatching(pRxRMInp, pRxRMOut, pRxRMHard);
			//	end = clock();
			//	cost = ((end - start) / CLOCKS_PER_SEC) * 1000;

			//	std::cout <<"Rate Dematch:" << cost << "ms" << std::endl;

			//	cout << "Turbo Rx Input" << endl;
			for (int i = 0; i < RxTurbo.InBufSz; i++)
			{
				pRxTbInp[i] = pRxRMOut[i];
				//	cout << pRxTbInp[i] << "\t";
			}
			//	cout << endl;
			//	RxTbD.TurboDecoding(&RxFileSink);

			//	start = clock();
			RxTurbo.TurboDecoding(pRxTbInp, pRxTbOut);
			//	end = clock();
			//	cost = ((end - start) / CLOCKS_PER_SEC) * 1000;

			//	std::cout <<"Turbo:" << cost << "ms" << std::endl;
			
			for (int i = 0; i < RxTurbo.OutBufSz; i++)
			{
				pRxTbOut[i] = (1 - pRxTbOut[i]) / 2;
				//	cout << pRxTbOut[i] << "\t";
			}
			//	cout << endl;

			//	ReadLTEChainOutput(&RxFileSink,pRxFS);
			ReadLTEChainOutput(pRxTbOut, pRxFS, DataK);
			WriteOutputToFiles(pRxFS, DataK, "RxBitStream.dat");
			
			ReadInputFromFiles(pTxDS, DataK, "TxBitStream.dat");

			int NumErrBit=0;
			
			for(int i = 0; i < DataK/*TxRM.InBufSz*/; i++)
			{
				if(*(pTxDS+i)==*(pRxFS+i)){}
				//	if (pTxRMInp[i] == (pRxRMOut[i] > 0)) {}
				else
				{
					NumErrBit++;
					cout << i << endl;
				}
			}
			cout<<"Num of error bits="<<NumErrBit<<endl;

			////////////////////////// END Run Subframe/////////////////////////////////
		}
	}

	end = clock();

	std::cout << (end - start) * 1000 / CLOCKS_PER_SEC << std::endl;

	delete[] AWGNSigmaArray;

	delete[] pTxDS;
	delete[] pRxFS;

	/** Deallocation **/
	// Turbo
	delete[] pRxTbInp;
	delete[] pRxTbOut;

	// Rate matching
	delete[] pRxRMInp;
	delete[] pRxRMOut;
	delete[] pRxRMHard;

	// Scrambling
	delete[] pRxSCRBInp;
	delete[] pRxSCRBOut;

	// Modulation
	delete[] pRxModInp;
	delete[] pRxModOut;

	// Transform precoding
	delete[] pRxTransPreInp;
	delete[] pRxTransPreOut;
	
	// Resouce mapping
	delete[] pRxResMapInp;
	delete[] pRxResMapOut;

	// Equalizing
	delete[] pRxEqInp;
	delete[] pRxEqOut;

	// OFDM
	delete[] pRxOFDMInp;
	delete[] pRxOFDMOut;

	// Synchronization
//	delete[] pRxSyncInp;
	delete[] pRxSyncOut;
	
	/** End of deallocation**/

	return 0;
}
