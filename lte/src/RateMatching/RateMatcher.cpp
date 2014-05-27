
#include "RateMatcher.h"


RateMatcher::RateMatcher(UserPara* pUser)
{
	PSFlag = (*pUser).ProcessingStatusFlag;
	BufFlag=(*pUser).BufferSizeFlag;
	Rate=3;
	DataLength=(*pUser).DataLength;
	BlkSize = pUser->BlkSize;

//	NumBlock = (DataLength-(DataLength%6144))/6144+1;
	NumBlock = (DataLength + BlkSize - 1) / (BlkSize);
	LastBlockLen = (DataLength % BlkSize) ? (DataLength % BlkSize) : BlkSize;

	/*
	pLengthSet=new int[NumBlock];
	for(int nblock=0;nblock<NumBlock-1;nblock++)
	{
		*(pLengthSet+nblock)=6144;
	}
	*(pLengthSet+NumBlock-1)=DataLength%6144;
	*/

	NumLayer=(*pUser).NumLayer;
	NInfoBits=(*pUser).NInfoBits;

//	NumExtraBits=NumLayer*NInfoBits-((NumBlock-1)*(6144*Rate+12)+1*((*(pLengthSet+NumBlock-1))*Rate+12));
	NumExtraBits = NumLayer * NInfoBits - Rate * (((NumBlock - 1) * (BlkSize + 4) + 1 * (LastBlockLen + 4)));

	/*
	pcMatrix=new int*[Rate];
	for(int r=0;r<Rate;r++){*(pcMatrix+r)=new int[((NumBlock-1)*(6144+4)+1*((*(pLengthSet+NumBlock-1))+4))];}
	pcSeq = new int[NumLayer*NInfoBits];
	*/
	
//	EncLen=3*DataLength+12;
//	pEncS=new int[EncLen];
	
	
	//////////////////////// Calc In/Out buffer size//////////////////////////
//	InBufSz[0]=Rate;InBufSz[1]=((NumBlock-1)*(6144+4)+1*((*(pLengthSet+NumBlock-1))+4));
	InBufSz = Rate * ((NumBlock - 1) * (BlkSize + 4) + 1 * (LastBlockLen + 4));
//	OutBufSz[0]=1;  OutBufSz[1]=NumLayer*NInfoBits;
	OutBufSz = NumLayer * NInfoBits;

//	pcMatrix = new int[InBufSz];
//	pcSeq = new int[OutBufSz];
	
	/*
	if(BufFlag)
	{
		cout<<"TxRateMatch"<<endl;
		cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  int"<<endl;
		cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  int"<<endl; 
	}
	else
	{}
	*/
	//////////////////////End of clac in/out buffer size//////////////////////

	////////////////////// Initialize its own Input Buffer //////////////////////////
//	pTxInpBuf = new FIFO<int>[1];
//	(*pTxInpBuf).initFIFO(1,InBufSz);
	//////////////////End of initialization of its own input buffer//////////////////

}


//void RateMatcher::TxRateMatching(FIFO<int> *pOutBuf)
//void RateMatcher::TxRateMatching(int *pcMatrix, int *pcSeq)
void RateMatcher::TxRateMatching(int *piSeq, int *pcSeq)
{
	if(PSFlag)
	{cout<<"TxRateMatching"<<endl;}

//	bool ReadFlag = (*pTxInpBuf).Read(pcMatrix);
//	if(ReadFlag)
//	{
	////////////// Start Rate Matching ///////////

		
	int *pLengthSet = new int[NumBlock];
	
	for(int nblock = 0; nblock < NumBlock - 1; nblock++)
	{
		//	*(pLengthSet+nblock)=6144;
		pLengthSet[nblock] = BlkSize;
	}
	pLengthSet[NumBlock - 1] = (DataLength % BlkSize) ? (DataLength % BlkSize) : BlkSize;

	/*
	int EncLen=3*DataLength+12;
	int *pEncS=new int[EncLen];
	
	int EncSK=(*(pLengthSet+0))+4;
		
	for(int i=0;i<EncSK;i++)
	{
		for(int r=0;r<Rate;r++)
		{
			*(pEncS+i*Rate+r)=*(*(pcMatrix+r)+i);
		}
	}
	*/

	int **pInMatrix = new int*[Rate];
	int **pOutMatrix = new int*[Rate];
	
	for (int r = 0; r < Rate; r++)
	{
		*(pInMatrix + r) = new int[BlkSize + 4];
		*(pOutMatrix + r) = new int[BlkSize + 4];
	}
	
	for(int nBlock = 0; nBlock < NumBlock; nBlock++)
	{
		//	iSeqLength=*(pLengthSet+nBlock);
		int iSeqLength = pLengthSet[nBlock];
		//	int InpBlockShift = nBlock * (BlkSize + 4);
		int OutBlockShift = nBlock* Rate *(BlkSize + 4);
		/////////////////// TxRateMatch this block /////////////////////

		////////////////////// Subblock Interleaver //////////////////////////

		//	int *pInMatrix = new int[Rate * (iSeqLength + 4)];
		//	int *pOutMatrix = new int[Rate * (iSeqLength + 4)];

		/*
		for(int r = 0; r < Rate; r++)
		{
			*(pInMatrix+r) = new int[iSeqLength+4];
			*(pOutMatrix+r) = new int[iSeqLength+4];
			
			for(int i=0;i<iSeqLength+4;i++)
			{
				//	*(*(pInMatrix+r)+i)=*(*(pcMatrix+r)+InpBlockShift+i);
				*(*(pInMatrix+r)+i)= pcMatrix[r * (InBufSz / Rate) + (InpBlockShift + i)];
				//	pInMatrix[r * (iSeqLength + 4) + i] = pcMatrix[r * (InBufSz / Rate) + InpBlockShift + i];
			}
		}
		*/

		// Transposition
		for (int i = 0; i < iSeqLength + 4; i++)
		{
			for (int r = 0; r < Rate; r++)
			{
				pInMatrix[r][i] = piSeq[OutBlockShift + Rate * i + r];
				//	cout << pInMatrix[r][i];
			}
		}
		//cout << endl;
			
		SbInterleaver.SubblockInterleaving((iSeqLength+4), pInMatrix, pOutMatrix);

		// Make a copy
		/*
		for(int r=0;r<Rate;r++)
		{
			for(int i=0;i<iSeqLength+4;i++)
			{
				//	*(*(pcMatrix+r)+InpBlockShift+i)=*(*(pOutMatrix+r)+i);
				pcMatrix[r * (InBufSz / Rate) + InpBlockShift + i] = *(*(pOutMatrix+r)+i);
				//	pcMatrix[r * (InBufSz / Rate) + InpBlockShift + i] = pOutMatrix[r * (iSeqLength + 4) + i];
			}
		}
		*/
		/*
		  for(int r=0;r<Rate;r++)
		  {
		  delete[] *(pInMatrix+r);
		  delete[] *(pOutMatrix+r);
		  }
		*/
//		delete[] pInMatrix;
//		delete[] pOutMatrix;
		//////////////////// END Subblock Interleaver ////////////////////////

		/*
		for(int i=0;i<iSeqLength+4;i++)
		{
			//	*(pcSeq+OutBlockShift+3*i+0)=*(*(pcMatrix+0)+InpBlockShift+i);
			pcSeq[OutBlockShift + 3 * i + 0] = pcMatrix[0 * (InBufSz / Rate) + InpBlockShift + i];
			//	*(pcSeq+OutBlockShift+3*i+1)=*(*(pcMatrix+1)+InpBlockShift+i);
			pcSeq[OutBlockShift + 3 * i + 1] = pcMatrix[1 * (InBufSz / Rate) + InpBlockShift + i];
			//	*(pcSeq+OutBlockShift+3*i+2)=*(*(pcMatrix+2)+InpBlockShift+i);
			pcSeq[OutBlockShift + 3 * i + 2] = pcMatrix[2 * (InBufSz / Rate) + InpBlockShift + i];
		}
		*/
		
		// Transposition again
		for (int i = 0; i < iSeqLength + 4; i++)
		{
			for (int r = 0; r < Rate; r++)
			{
				pcSeq[OutBlockShift + Rate * i + r] = pOutMatrix[r][i];
			}
		}

		///////////////////END TxRateMatch this block /////////////////////
	}
	
	for(int i = 0; i < NumExtraBits; i++)
	{
		//	*(pcSeq + ((NumBlock-1)*(6144*Rate+12)+1*((*(pLengthSet+NumBlock-1))*Rate + 12)) + i) = 0;
		pcSeq[InBufSz + i] = 0;
	}

//		bool WriteFlag = (*pOutBuf).Write(pcSeq);

//		if(PSFlag)
//		{cout<<"TxRateMatch completed"<<endl;}

	///////////End Rate Match ////////////////////
//	}
//	else
//	{cout<<"unable to read input buffer"<<endl;}

	delete[] pLengthSet;
//	delete[] pEncS;
	for (int r = 0; r < Rate; r++)
	{
		delete[] pInMatrix[r];
		delete[] pOutMatrix[r];
	}
	delete[] pInMatrix;
	delete[] pOutMatrix;
}


RateMatcher::RateMatcher(BSPara* pBS)
{
	PSFlag = (*pBS).ProcessingStatusFlag;
	BufFlag=(*pBS).BufferSizeFlag;

	NumULSymbSF=(*pBS).NumULSymbSF;

	MDFT=(*pBS).MDFTPerUser;
	MQAM=(*pBS).MQAMPerUser;
	NumLayer=(*pBS).NumLayerPerUser;
	DataLength=(*pBS).DataLengthPerUser;
	BlkSize = pBS->BlkSize;

	Rate=3;

//	pLLRin=new float[(NumLayer*(NumULSymbSF-2)*MDFT*((int)((log((double)MQAM))/(log(2.0)))))];
  
//	LastBlockLen=(DataLength%6144);
//	NumBlock=(DataLength-LastBlockLen)/6144+1;
//	NumBlock = (DataLength - DataLength % 6144) / 6144 + 1;
	NumBlock = (DataLength + BlkSize - 1) / BlkSize;
	LastBlockLen = (DataLength % BlkSize) ? (DataLength % BlkSize) : BlkSize;
//	EncDataLen=(NumBlock-1)*(6144*Rate+12)+1*(LastBlockLen*Rate+12);
//	pLLRout=new float[EncDataLen];

	/*
	pLengthSet=new int[NumBlock];
	for(int nblock=0;nblock<NumBlock-1;nblock++)
	{
		*(pLengthSet+nblock)=6144;
	}
	*(pLengthSet+NumBlock-1)=DataLength%6144;
	*/

//	pHD = new int[EncDataLen];

//////////////////////// Calc In/Out buffer size//////////////////////////
//	InBufSz[0]=1;InBufSz[1]=(NumLayer*(NumULSymbSF-2)*MDFT*((int)((log((double)MQAM))/(log(2.0)))));
//	OutBufSz[0]=1;OutBufSz[1]=EncDataLen;
	InBufSz = (NumLayer*(NumULSymbSF-2)*MDFT*((int)((log((double)MQAM))/(log(2.0)))));
//	OutBufSz = (NumBlock-1)*(6144*Rate+12)+1*((DataLength % 6144)*Rate+12);
	OutBufSz = Rate * ((NumBlock - 1) * (BlkSize + 4) + 1 * (LastBlockLen + 4));
	
//	pHD = new int[OutBufSz];

	/*
	  if(BufFlag)
	  {
	  cout<<"RxRateMatch"<<endl;
	  cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  float"<<endl;
	  cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  float"<<endl; 
	  }
	  else
	  {}
	*/
//////////////////////End of clac in/out buffer size//////////////////////
////////////////////// Initialize its own Input Buffer //////////////////////////
//	pRxInpBuf =new FIFO<float>[1];
//	(*pRxInpBuf).initFIFO(1,InBufSz);
//////////////////End of initialization of its own input buffer//////////////////

}

//void RateMatcher::RxRateMatching(FIFO<float>* pOutBuf)
void RateMatcher::RxRateMatching(float *pLLRin, float *pLLRout, int *pHD)
{
	if(PSFlag)
	{cout<<"RxRateMatching"<<endl;}
	
	int iSeqLength;
	int OutBlockShift;

//	bool ReadFlag = (*pRxInpBuf).Read(pLLRin);
//	if(ReadFlag)
//	{
	int *pLengthSet=new int[NumBlock];
	
	for(int nblock=0;nblock<NumBlock-1;nblock++)
	{
		//	*(pLengthSet+nblock)=6144;
		pLengthSet[nblock] = BlkSize;
	}
//	*(pLengthSet+NumBlock-1)=DataLength%6144;
	pLengthSet[NumBlock - 1] = (DataLength % BlkSize) ? (DataLength % BlkSize) : BlkSize;

	float **pInMatrix = new float*[Rate];
	float **pOutMatrix = new float*[Rate];
		
	for(int r = 0; r < Rate; r++)
	{
		*(pInMatrix + r) = new float[BlkSize + 4];
		*(pOutMatrix + r) = new float[BlkSize + 4];
	}

	for(int nBlock = 0; nBlock < NumBlock; nBlock++)
	{
		iSeqLength=pLengthSet[nBlock];
		//	OutBlockShift = nBlock*(6144*Rate+12);
		OutBlockShift = nBlock * Rate * (BlkSize + 4);
		
////////////////////// Subblock DeInterleaver //////////////////////////

		////////////////////// Read into pInMatrix //////////////////
		for(int i=0;i<iSeqLength+4;i++)
		{
			for(int r=0;r<Rate;r++)
			{
				*(*(pInMatrix+r)+i)=*(pLLRin+OutBlockShift+Rate*i+r);
			}
		}  

		//////////////////// END Read into pInMatrix ////////////////

		SbDeInterleaver.SubblockDeInterleaving((iSeqLength+4),pInMatrix,pOutMatrix);

		////////////////////// Write back to pLLRin //////////////////
		for(int i=0;i<iSeqLength+4;i++)
		{
			for(int r=0;r<Rate;r++)
			{
				*(pLLRin+OutBlockShift+Rate*i+r)=*(*(pOutMatrix+r)+i);
			}
		}  
		//////////////////// END Write back to pLLRin ////////////////


//////////////////// END Subblock DeInterleaver ////////////////////////
	}

//	for(int i=0;i<EncDataLen;i++)
	for (int i = 0; i < OutBufSz; i++)
	{
		if((*(pLLRin+i))<0){*(pHD+i)=0;}
		else{*(pHD+i)=1;}
	}

//	for(int i=0;i<EncDataLen;i++)
	for (int i = 0; i < OutBufSz; i++)
	{
		*(pLLRout+i)=*(pLLRin+i);
	}
//		bool WriteFlag = (*pOutBuf).Write(pLLRout);
	//   if(WriteFlag){cout<<"successfully written!"<<endl;}else{}
	if(PSFlag)
	{cout<<"RxRateMatch completed"<<endl;}

	delete[] pLengthSet;
	
	for(int r=0;r<Rate;r++)
	{
		delete[] *(pInMatrix+r);
		delete[] *(pOutMatrix+r);
	}
	delete[] pInMatrix;
	delete[] pOutMatrix;
//	}
//	else
//	{cout<<"fail to read data from previous buffer"<<endl;}

}


RateMatcher::~RateMatcher()
{
//	delete[] pLengthSet;

//	for(int r=0;r<Rate;r++){delete[] *(pcMatrix+r);}
//	delete[] pcMatrix;

//	delete[] pcSeq;

//	delete[] pEncS;
	
//	delete[] pTxInpBuf;
//	delete[] pRxInpBuf;
	
//	delete[] pLLRin;
//	delete[] pLLRout;
	
//	delete[] pHD;
}

