
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
	TransformPrecoder TxTransPre(&User);
	ResMapper TxResMap(&User);
	OFDM TxOFDM(&User);
//	Channel TxCRx(&BS);
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
	int *pTxTbInp = new int[TxTurbo.InBufSz];
	int *pTxTbOut = new int[TxTurbo.OutBufSz];
	float *pRxTbInp = new float[RxTurbo.InBufSz];
	int *pRxTbOut = new int[RxTurbo.OutBufSz];

	// Rate matching
	int *pTxRMInp = new int[TxRM.InBufSz];
	int *pTxRMOut = new int[TxRM.OutBufSz];
	float *pRxRMInp = new float[RxRM.InBufSz];
	float *pRxRMOut = new float[RxRM.OutBufSz];
	int *pRxRMHard = new int[RxRM.OutBufSz];

	// Scrambling
	int *pTxSCRBInp = new int[TxSCRB.InBufSz];
	int *pTxSCRBOut = new int[TxSCRB.OutBufSz];
	float *pRxSCRBInp = new float[RxSCRB.InBufSz];
	float *pRxSCRBOut = new float[RxSCRB.OutBufSz];

	// Modulation
	int *pTxModInp = new int[TxMod.InBufSz];
	complex<float> *pTxModOut = new complex<float>[TxMod.OutBufSz];
	complex<float> *pRxModInp = new complex<float>[RxMod.InBufSz];
	float *pRxModOut = new float[RxMod.OutBufSz];
	int *pRxModHD = new int[RxMod.OutBufSz];

	// Transform precoding
	complex<float> *pTxTransPreInp = new complex<float>[TxTransPre.InBufSz];
	complex<float> *pTxTransPreOut = new complex<float>[TxTransPre.OutBufSz];
	complex<float> *pRxTransPreInp = new complex<float>[RxTransPre.InBufSz];
	complex<float> *pRxTransPreOut = new complex<float>[RxTransPre.OutBufSz];
	
	// Resource mapping
	complex<float> *pTxResMapInp = new complex<float>[TxResMap.InBufSz];
	complex<float> *pTxResMapOut = new complex<float>[TxResMap.OutBufSz];
	complex<float> *pRxResMapInp = new complex<float>[RxResMap.InBufSz];
	complex<float> *pRxResMapOut = new complex<float>[RxResMap.OutBufSz];

	// Equalizing
	complex<float> *pRxEqInp = new complex<float>[RxEq.InBufSz];
	complex<float> *pRxEqOut = new complex<float>[RxEq.OutBufSz];

	// OFDM
	complex<float> *pTxOFDMInp = new complex<float>[TxOFDM.InBufSz];
	complex<float> *pTxOFDMOut = new complex<float>[TxOFDM.OutBufSz];
	complex<float> *pRxOFDMInp = new complex<float>[RxOFDM.InBufSz];
	complex<float> *pRxOFDMOut = new complex<float>[RxOFDM.OutBufSz];
	
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


			/** TODO: Adding channel **/
//			TxCRx.ApplyChannel(RxSCFD.pInpBuf,(AWGNSigmaArray[nsnr]));

			for (int i = 0; i < RxOFDM.InBufSz; i++)
			{
				pRxOFDMInp[i] = pTxOFDMOut[i];
			}

			RxOFDM.demodulating(pRxOFDMInp, pRxOFDMOut);

			for (int i = 0; i < RxResMap.InBufSz; i++)
			{
				pRxResMapInp[i] = pRxOFDMOut[i];
			}

			RxResMap.SubCarrierDemapping(pRxResMapInp, pRxResMapOut);

			// RxE.Equalizing(RxTD.pInpBuf,TxCRx.GetpPCSI(),TxCRx.GetAWGNNo());
			
			for (int i = 0; i < RxEq.InBufSz; i++)
			{
				pRxEqInp[i] = pRxResMapOut[i];
			}
			
			RxEq.Equalizing(pRxEqInp, pRxEqOut);

			for (int i = 0; i < RxTransPre.InBufSz; i++)
			{
				pRxTransPreInp[i] = pRxEqOut[i];
			}
			
			RxTransPre.TransformDecoding(pRxTransPreInp, pRxTransPreOut);

		//	cout << "Modulating Rx Input" << endl;
			for (int i = 0; i < RxMod.InBufSz; i++)
			{
				pRxModInp[i] = pRxTransPreOut[i];
		//		cout << pRxModInp[i] << "\t";
			}
		//	cout << endl;
			
			// RxD.Demodulating(RxDSCRB.pInpBuf,RxE.GetpEqW(),RxE.GetpHdm(),(AWGNSigmaArray[nsnr]));			
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


			//	cout << "Turbo Rx Input" << endl;
			for (int i = 0; i < RxTurbo.InBufSz; i++)
			{
				pRxTbInp[i] = pRxRMOut[i];
				//	cout << pRxTbInp[i] << "\t";
			}
			//	cout << endl;
			//	RxTbD.TurboDecoding(&RxFileSink);
			RxTurbo.TurboDecoding(pRxTbInp, pRxTbOut);

			//	cout << "Turbo Rx Output" << endl;
			for (int i = 0; i < RxTurbo.OutBufSz; i++)
			{
				pRxTbOut[i] = (1 - pRxTbOut[i]) / 2;
				//	cout << pRxTbOut[i] << "\t";
			}
			//	cout << endl;

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

	/** Deallocation **/
	// Turbo
	delete[] pTxTbInp;
	delete[] pTxTbOut;
	delete[] pRxTbInp;
	delete[] pRxTbOut;

	// Rate matching
	delete[] pTxRMInp;
	delete[] pTxRMOut;
	delete[] pRxRMInp;
	delete[] pRxRMOut;
	delete[] pRxRMHard;

	// Scrambling
	delete[] pTxSCRBInp;
	delete[] pTxSCRBOut;
	delete[] pRxSCRBInp;
	delete[] pRxSCRBOut;

	// Modulation
	delete[] pTxModInp;
	delete[] pTxModOut;
	delete[] pRxModInp;
	delete[] pRxModOut;

	// Transform precoding
	delete[] pTxTransPreInp;
	delete[] pTxTransPreOut;
	delete[] pRxTransPreInp;
	delete[] pRxTransPreOut;
	
	// Resouce mapping
	delete[] pTxResMapInp;
	delete[] pTxResMapOut;
	delete[] pRxResMapInp;
	delete[] pRxResMapOut;

	// Equalizing
	delete[] pRxEqInp;
	delete[] pRxEqOut;

	// OFDM
	delete[] pTxOFDMInp;
	delete[] pTxOFDMOut;
	delete[] pRxOFDMInp;
	delete[] pRxOFDMOut;

	/** End of deallocation**/

	return 0;
}
