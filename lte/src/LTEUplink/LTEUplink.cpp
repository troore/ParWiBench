
#include "LTEUplink.h"

////////Global Variable//////////
//int RANDOMSEED;

////////End Global Variable//////////

using namespace std;

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
	int HDLen = (NumBlock - 1) * BS.Rate * ((BS.BlkSize + 4) + 1 * (LastK + 4));

	float *AWGNSigmaArray = new float[SNRArrayLen];
	
	for(int i = 0; i < SNRArrayLen; i++)
	{
		float x = 10.5 + 0.5 * ((float)i);
		
		AWGNSigmaArray[i] = sqrt((1.50 / ((log((float)((User).MQAM))) / (log(2.0)))) * (pow(10.0, (-x / 10.0))));
		cout << AWGNSigmaArray[i] << endl;
	}

	float *PE = new float[SNRArrayLen];
	float *PB = new float[SNRArrayLen];
	float *HPE = new float[SNRArrayLen];
	float *HPB = new float[SNRArrayLen];

	///////////////////// construct the kernels ////////////////////
	Turbo TxTurbo(&User);
	RateMatcher TxRM(&User);
	Scrambler<int> TxSCRB(&User);
	Modulation TxMod(&User);
	/*
	TransformPrecoder TxTP(&User);
	SubCarrierMap TxSCM(&User);
	SCFDMAModulation TxSCFM(&User);
	*/
//	Channel TxCRx(&BS);
	/*
	SCFDMADemodulation RxSCFD(&BS);
	SubCarrierDemap RxSCD(&BS);
	Equalizer RxE(&BS,&User);
	TransformDecoder RxTD(&BS);
	*/
	Modulation RxMod(&BS);
	Scrambler<float> RxSCRB(&BS);
	RateMatcher RxRM(&BS);
	Turbo RxTurbo(&BS);
	//////////////////// Completed construction of the kernels/////////////////

	////////////////// allocate the FIFOs /////////////////////////
//	FIFO<int> RxFileSink(1,RxTbD.OutBufSz);
	//////////////////End of allocation of FIFOs /////////////////////////
	
	/** Allocate inputs and outputs **/
	int *pTxTbInp = new int[TxTurbo.InBufSz];
	int *pTxTbOut = new int[TxTurbo.OutBufSz];
	float *pRxTbInp = new float[RxTurbo.InBufSz];
	int *pRxTbOut = new int[RxTurbo.OutBufSz];

	int *pTxRMInp = new int[TxRM.InBufSz];
	int *pTxRMOut = new int[TxRM.OutBufSz];
	float *pRxRMInp = new float[RxRM.InBufSz];
	float *pRxRMOut = new float[RxRM.OutBufSz];
	int *pRxRMHard = new int[RxRM.OutBufSz];

	int *pTxSCRBInp = new int[TxSCRB.InBufSz];
	int *pTxSCRBOut = new int[TxSCRB.OutBufSz];
	float *pRxSCRBInp = new float[RxSCRB.InBufSz];
	float *pRxSCRBOut = new float[RxSCRB.OutBufSz];

	int *pTxModInp = new int[TxMod.InBufSz];
	complex<float> *pTxModOut = new complex<float>[TxMod.OutBufSz];
	complex<float> *pRxModInp = new complex<float>[RxMod.InBufSz];
	float *pRxModOut = new float[RxMod.OutBufSz];
	int *pRxModHD = new int[RxMod.OutBufSz];
	
	/** End of allocations **/

	FILE *fptr = NULL;
	
	int* pTxDS=new int[DataK];
	int *pHDDS=new int[HDLen];
	int *pHDFS=new int[HDLen];
	int* pRxFS=new int[DataK];

	for (int nsnr = 0; nsnr < SNRArrayLen; nsnr++)
	{
		int PacketErr=0;
		int BitErr=0;

		int HPacketErr=0;
		int HBitErr=0;

		for (int nsf = 0; nsf < numSFrames; nsf++)
		{
			////////////////////////// Run Subframe //////////////////////////////////
			int RANDOMSEED = (nsf + 1) * (nsnr + 2);

			//	GenerateLTEChainInput(TxTbE.pInpBuf,DataK,pTxDS);
			GenerateLTEChainInput(pTxTbInp, DataK, pTxDS, RANDOMSEED);

			
			cout << "Turbo Tx Input" << endl;
			for (int i = 0; i < DataK; i++)
				cout << pTxTbInp[i] << "\t";
			cout << endl;
			

			//	TxTbE.TurboEncoding(TxRM.pInpBuf);
			TxTurbo.TurboEncoding(pTxTbInp, pTxTbOut);

			cout << "RateMatching Tx Input" << endl;
			for (int i = 0; i < TxRM.InBufSz; i++)
			{
				pTxRMInp[i] = pTxTbOut[i];
				cout << pTxRMInp[i] << "\t";
			}
			cout << endl;

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
			

			/*
			TxTP.TransformPrecoding(TxSCM.pInpBuf);


			TxSCM.SubCarrierMapping(TxSCFM.pInpBuf);


			TxSCFM.SCFDMAModulating(TxCRx.pInpBuf);


			TxCRx.ApplyChannel(RxSCFD.pInpBuf,(AWGNSigmaArray[nsnr]));


			RxSCFD.SCFDMADemodulating(RxSCD.pInpBuf);


			RxSCD.SubCarrierDemapping(RxE.pInpBuf);


			// RxE.Equalizing(RxTD.pInpBuf,TxCRx.GetpPCSI(),TxCRx.GetAWGNNo());

			RxE.Equalizing(RxTD.pInpBuf);


			RxTD.TransformDecoding(RxD.pInpBuf);


			// RxD.Demodulating(RxDSCRB.pInpBuf,RxE.GetpEqW(),RxE.GetpHdm(),(AWGNSigmaArray[nsnr]));
			*/


		//	cout << "Modulating Rx Input" << endl;
			for (int i = 0; i < RxMod.InBufSz; i++)
			{
				pRxModInp[i] = pTxModOut[i];
		//		cout << pRxModInp[i] << "\t";
			}
		//	cout << endl;
			
			RxMod.Demodulating(pRxModInp, pRxModOut, (AWGNSigmaArray[nsnr]));
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
			
			RxSCRB.Descrambling(pRxSCRBInp, pRxSCRBOut);

		//	cout << "RateMatching Rx Input" << endl;
			for (int i = 0; i < RxRM.InBufSz; i++)
			{
				pRxRMInp[i] = pRxSCRBOut[i];
		//		cout << pRxRMInp[i] << "\t";
			}
		//	cout << endl;
		   
			RxRM.RxRateMatching(pRxRMInp, pRxRMOut, pRxRMHard);

			/*
			for (int i = 0; i < (RxRM.OutBufSz / 3); i++)
			{
				pRxTbInp[3 * i] = pRxRMOut[i];
				pRxTbInp[3 * i + 1] = pRxRMOut[i + 1];
				pRxTbInp[3 * i + 2] = pRxRMOut[i + 2];
			}
			*/

			cout << "Turbo Rx Input" << endl;
			for (int i = 0; i < RxTurbo.InBufSz; i++)
			{
				pRxTbInp[i] = pRxRMOut[i];
				cout << pRxTbInp[i] << "\t";
			}
			cout << endl;
			//	RxTbD.TurboDecoding(&RxFileSink);
			RxTurbo.TurboDecoding(pRxTbInp, pRxTbOut);

			cout << "Turbo Rx Output" << endl;
			for (int i = 0; i < RxTurbo.OutBufSz; i++)
			{
				pRxTbOut[i] = (1 - pRxTbOut[i]) / 2;
				cout << pRxTbOut[i] << "\t";
			}
			cout << endl;

			//	ReadLTEChainOutput(&RxFileSink,pRxFS);
			ReadLTEChainOutput(pRxTbOut, pRxFS, DataK);

			/*
			for(int i=0;i<HDLen;i++)
			{*(pHDDS+i)=*((TxRM.GetpEncS())+i);}


			for(int i=0;i<HDLen;i++)
			{
				*(pHDFS+i)=*(RxRM.GetpHD()+i);
			}
			*/

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

			/*
			int HDErrBit=0;
			
			for(int i=0;i<HDLen;i++)
			{
				if(*(pHDDS+i)==*(pHDFS+i)){}
				else{HDErrBit++;}
			} 
			cout<<"Num of HD error bits="<<HDErrBit<<endl;

			if(NumErrBit==0){}
			else{PacketErr++;BitErr+=NumErrBit;}

			if(HDErrBit==0){}
			else{HPacketErr++;HBitErr+=HDErrBit;}
			*/

			////////////////////////// END Run Subframe/////////////////////////////////
		}

		/*
		PE[nsnr] = ((float)(PacketErr))/((float)(nrun));
		PB[nsnr] = ((float)(BitErr))/((float)(nrun*DataK));

		HPE[nsnr] = ((float)(HPacketErr))/((float)(nrun));
		HPB[nsnr] = ((float)(HBitErr))/((float)(nrun*(DataK*3+12)));
		*/
		/*          
					fptr = NULL;
					fptr = fopen("sigma.txt","a+");
					if(fptr!=NULL)
					{
					fprintf(fptr,"%f ",AWGNSigmaArray[nsnr]);
					fprintf(fptr,",");
					fclose(fptr);
					}

					fptr = NULL;
					fptr = fopen("PacketErrorProb.txt","a+");
					if(fptr!=NULL)
					{
					fprintf(fptr,"%f ",PE[nsnr]);
					fprintf(fptr,",");
					fclose(fptr);
					}

					fptr = NULL;
					fptr = fopen("BitErrProb.txt","a+");
					if(fptr!=NULL)
					{
					fprintf(fptr,"%f ",PB[nsnr]);
					fprintf(fptr,",");
					fclose(fptr);
					}

					fptr = NULL;
					fptr = fopen("HDPacketErrorProb.txt","a+");
					if(fptr!=NULL)
					{
					fprintf(fptr,"%f ",HPE[nsnr]);
					fprintf(fptr,",");
					fclose(fptr);
					}

					fptr = NULL;
					fptr = fopen("HDBitErrProb.txt","a+");
					if(fptr!=NULL)
					{
					fprintf(fptr,"%f ",HPB[nsnr]);
					fprintf(fptr,",");
					fclose(fptr);
					}
					*/ 

	}

	delete[] AWGNSigmaArray;
	delete[] PE;
	delete[] PB;
	delete[] HPE;
	delete[] HPB;

	delete[] pTxDS;
	delete[] pHDDS;
	delete[] pHDFS;
	delete[] pRxFS;

	delete[] pTxTbInp;
	delete[] pTxTbOut;
	delete[] pRxTbInp;
	delete[] pRxTbOut;

	delete[] pTxRMInp;
	delete[] pTxRMOut;
	delete[] pRxRMInp;
	delete[] pRxRMOut;
	delete[] pRxRMHard;

	delete[] pTxSCRBInp;
	delete[] pTxSCRBOut;
	delete[] pRxSCRBInp;
	delete[] pRxSCRBOut;

	delete[] pTxModInp;
	delete[] pTxModOut;
	delete[] pRxModInp;
	delete[] pRxModOut;

	return 0;
}
