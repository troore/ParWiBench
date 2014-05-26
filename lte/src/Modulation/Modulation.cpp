
#include "Modulation.h"

static const float INF = 1.0e9;

void Modulation::InitModTables()
{
	////////////////// Init all Modulation Table /////////////////////
	pBPSKtable=new float*[2];
	*(pBPSKtable+0)=new float[2];*(pBPSKtable+1)=new float[2];
	pBPSKtable[0][0]=+1.0;pBPSKtable[0][1]=+1.0;
	pBPSKtable[1][0]=-1.0;pBPSKtable[1][1]=-1.0;
	for(int r=0;r<2;r++)
	{
		for(int c=0;c<2;c++)
		{pBPSKtable[r][c]/=sqrt(2.0);}
	}

	pQPSKtable=new float*[4];
	for(int i=0;i<4;i++){*(pQPSKtable+i)=new float[2];}
	pQPSKtable[0][0]=+1.0;pQPSKtable[0][1]=+1.0;
	pQPSKtable[1][0]=+1.0;pQPSKtable[1][1]=-1.0;
	pQPSKtable[2][0]=-1.0;pQPSKtable[2][1]=+1.0;
	pQPSKtable[3][0]=-1.0;pQPSKtable[3][1]=-1.0;
	for(int r=0;r<4;r++)
	{
		for(int c=0;c<2;c++)
		{pQPSKtable[r][c]/=sqrt(2.0);}
	}

	pQAM16table=new float*[16];
	for(int i=0;i<16;i++){*(pQAM16table+i)=new float[2];}
	pQAM16table[0][0]= +1.0; pQAM16table[0][1]=+1.0;
	pQAM16table[1][0]= +1.0; pQAM16table[1][1]=+3.0;
	pQAM16table[2][0]= +3.0; pQAM16table[2][1]=+1.0;
	pQAM16table[3][0]= +3.0; pQAM16table[3][1]=+3.0;
	pQAM16table[4][0]= +1.0; pQAM16table[4][1]=-1.0;
	pQAM16table[5][0]= +1.0; pQAM16table[5][1]=-3.0;
	pQAM16table[6][0]= +3.0; pQAM16table[6][1]=-1.0;
	pQAM16table[7][0]= +3.0; pQAM16table[7][1]=-3.0;
	pQAM16table[8][0]= -1.0; pQAM16table[8][1]=+1.0;
	pQAM16table[9][0]= -1.0; pQAM16table[9][1]=+3.0;
	pQAM16table[10][0]=-3.0;pQAM16table[10][1]=+1.0;
	pQAM16table[11][0]=-3.0;pQAM16table[11][1]=+3.0;
	pQAM16table[12][0]=-1.0;pQAM16table[12][1]=-1.0;
	pQAM16table[13][0]=-1.0;pQAM16table[13][1]=-3.0;
	pQAM16table[14][0]=-3.0;pQAM16table[14][1]=-1.0;
	pQAM16table[15][0]=-3.0;pQAM16table[15][1]=-3.0;
	for(int r=0;r<16;r++)
	{
		for(int c=0;c<2;c++)
		{pQAM16table[r][c]/=sqrt(10.0);}
	}

	pQAM64table=new float*[64];
	for(int i=0;i<64;i++){*(pQAM64table+i)=new float[2];}
	pQAM64table[0][0]= +3.0; pQAM64table[0][1]= +3.0;
	pQAM64table[1][0]= +3.0; pQAM64table[1][1]= +1.0;
	pQAM64table[2][0]= +1.0; pQAM64table[2][1]= +3.0;
	pQAM64table[3][0]= +1.0; pQAM64table[3][1]= +1.0;
	pQAM64table[4][0]= +3.0; pQAM64table[4][1]= +5.0;
	pQAM64table[5][0]= +3.0; pQAM64table[5][1]= +7.0;
	pQAM64table[6][0]= +1.0; pQAM64table[6][1]= +5.0;
	pQAM64table[7][0]= +1.0; pQAM64table[7][1]= +7.0;

	pQAM64table[8][0]= +5.0; pQAM64table[8][1]= +3.0;
	pQAM64table[9][0]= +5.0; pQAM64table[9][1]= +1.0;
	pQAM64table[10][0]=+7.0; pQAM64table[10][1]=+3.0;
	pQAM64table[11][0]=+7.0; pQAM64table[11][1]=+1.0;
	pQAM64table[12][0]=+5.0; pQAM64table[12][1]=+5.0;
	pQAM64table[13][0]=+5.0; pQAM64table[13][1]=+7.0;
	pQAM64table[14][0]=+7.0; pQAM64table[14][1]=+5.0;
	pQAM64table[15][0]=+7.0; pQAM64table[15][1]=+7.0;

	pQAM64table[16][0]=+3.0; pQAM64table[16][1]=-3.0;
	pQAM64table[17][0]=+3.0; pQAM64table[17][1]=-1.0;
	pQAM64table[18][0]=+1.0; pQAM64table[18][1]=-3.0;
	pQAM64table[19][0]=+1.0; pQAM64table[19][1]=-1.0;
	pQAM64table[20][0]=+3.0; pQAM64table[20][1]=-5.0;
	pQAM64table[21][0]=+3.0; pQAM64table[21][1]=-7.0;
	pQAM64table[22][0]=+1.0; pQAM64table[22][1]=-5.0;
	pQAM64table[23][0]=+1.0; pQAM64table[23][1]=-7.0;

	pQAM64table[24][0]=+5.0; pQAM64table[24][1]=-3.0;
	pQAM64table[25][0]=+5.0; pQAM64table[25][1]=-1.0;
	pQAM64table[26][0]=+7.0; pQAM64table[26][1]=-3.0;
	pQAM64table[27][0]=+7.0; pQAM64table[27][1]=-1.0;
	pQAM64table[28][0]=+5.0; pQAM64table[28][1]=-5.0;
	pQAM64table[29][0]=+5.0; pQAM64table[29][1]=-7.0;
	pQAM64table[30][0]=+7.0; pQAM64table[30][1]=-5.0;
	pQAM64table[31][0]=+7.0; pQAM64table[31][1]=-7.0;

	pQAM64table[32][0]=-3.0; pQAM64table[32][1]=+3.0;
	pQAM64table[33][0]=-3.0; pQAM64table[33][1]=+1.0;
	pQAM64table[34][0]=-1.0; pQAM64table[34][1]=+3.0;
	pQAM64table[35][0]=-1.0; pQAM64table[35][1]=+1.0;
	pQAM64table[36][0]=-3.0; pQAM64table[36][1]=+5.0;
	pQAM64table[37][0]=-3.0; pQAM64table[37][1]=+7.0;
	pQAM64table[38][0]=-1.0; pQAM64table[38][1]=+5.0;
	pQAM64table[39][0]=-1.0; pQAM64table[39][1]=+7.0;

	pQAM64table[40][0]=-5.0; pQAM64table[40][1]=+3.0;
	pQAM64table[41][0]=-5.0; pQAM64table[41][1]=+1.0;
	pQAM64table[42][0]=-7.0; pQAM64table[42][1]=+3.0;
	pQAM64table[43][0]=-7.0; pQAM64table[43][1]=+1.0;
	pQAM64table[44][0]=-5.0; pQAM64table[44][1]=+5.0;
	pQAM64table[45][0]=-5.0; pQAM64table[45][1]=+7.0;
	pQAM64table[46][0]=-7.0; pQAM64table[46][1]=+5.0;
	pQAM64table[47][0]=-7.0; pQAM64table[47][1]=+7.0;

	pQAM64table[48][0]=-3.0; pQAM64table[48][1]=-3.0;
	pQAM64table[49][0]=-3.0; pQAM64table[49][1]=-1.0;
	pQAM64table[50][0]=-1.0; pQAM64table[50][1]=-3.0;
	pQAM64table[51][0]=-1.0; pQAM64table[51][1]=-1.0;
	pQAM64table[52][0]=-3.0; pQAM64table[52][1]=-5.0;
	pQAM64table[53][0]=-3.0; pQAM64table[53][1]=-7.0;
	pQAM64table[54][0]=-1.0; pQAM64table[54][1]=-5.0;
	pQAM64table[55][0]=-1.0; pQAM64table[55][1]=-7.0;

	pQAM64table[56][0]=-5.0; pQAM64table[56][1]=-3.0;
	pQAM64table[57][0]=-5.0; pQAM64table[57][1]=-1.0;
	pQAM64table[58][0]=-7.0; pQAM64table[58][1]=-3.0;
	pQAM64table[59][0]=-7.0; pQAM64table[59][1]=-1.0;
	pQAM64table[60][0]=-5.0; pQAM64table[60][1]=-5.0;
	pQAM64table[61][0]=-5.0; pQAM64table[61][1]=-7.0;
	pQAM64table[62][0]=-7.0; pQAM64table[62][1]=-5.0;
	pQAM64table[63][0]=-7.0; pQAM64table[63][1]=-7.0;
	for(int r=0;r<64;r++)
	{
		for(int c=0;c<2;c++)
		{pQAM64table[r][c]/=sqrt(42.0);}
	}
	////////////////END Init all Modulation Table ////////////////////
}


/*
 * decimal to bit sequence
 * i: decimal
 * n: width of bit sequence
 * bvec: vector to contain the sequence
 */
void Modulation::dec2bits(int i, int n, int *bvec)
{
	for (uint j = 0; j < n; j++)
	{
		bvec[n - j - 1] = (i & 1);
		i = (i >> 1);
	}
}




Modulation::Modulation(UserPara* pUser)
{
	PSFlag = (*pUser).ProcessingStatusFlag;
	BufFlag=(*pUser).BufferSizeFlag;

	MQAM=(*pUser).MQAM;
	NumLayer=(*pUser).NumLayer;
	NInfoBits=(*pUser).NInfoBits;
	QAMLen=(*pUser).QAMLen;

//	pBitsSeq=new int[NumLayer*NInfoBits];
	BitsPerSymb=NInfoBits/QAMLen;
//	pQAMSeq=new complex<float>[NumLayer*QAMLen];
	
	InitModTables();
	switch(MQAM)
	{
	case 2: pTable=pBPSKtable;break;
	case 4: pTable=pQPSKtable;break;
	case 16:pTable=pQAM16table;break;
	case 64:pTable=pQAM64table;break;
	default: cout<<"Invalid modulation scheme!"<<endl;break;
	}

	//////////////////////// Calc In/Out buffer size//////////////////////////
//	InBufSz[0]=1; InBufSz[1]=NumLayer*NInfoBits;
	InBufSz = NumLayer * NInfoBits;
//	OutBufSz[0]=1;OutBufSz[1]=NumLayer*QAMLen;
	OutBufSz = NumLayer * QAMLen;
	/*
	if(BufFlag)
	{
		cout<<"Modulation"<<endl;
		cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  int"<<endl;
		cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  complex<float>"<<endl; 
	}
	else
	{}
	*/
	//////////////////////End of clac in/out buffer size//////////////////////

	////////////////////// Initialize its own Input Buffer //////////////////////////
	//ModInBuf=FIFO<int>(1,InBufSz);
//	pInpBuf =new FIFO<int>[1];
//	(*pInpBuf).initFIFO(1,InBufSz);
	//////////////////End of initialization of its own input buffer//////////////////

}

//void Modulation::Modulating(FIFO<complex<float> > *pOutBuf)
void Modulation::Modulating(int *pBitsSeq, complex<float> *pQAMSeq)
{
	if(PSFlag)
	{cout<<"Modulating"<<endl;}

//	bool ReadFlag = (*pInpBuf).Read(pBitsSeq);
//	if(ReadFlag)
//	{
		///////////// Modulating ////////////
		float I,Q;
		for(int nsymb=0;nsymb<QAMLen*NumLayer;nsymb++)
		{
			int Idx=0;
			for(int b=0;b<BitsPerSymb;b++)
			{
				Idx+=((*(pBitsSeq+nsymb*BitsPerSymb+b))*(pow(2.0,(float)(BitsPerSymb-1-b))));
			}
			I = *(*(pTable+Idx)+0);
			Q = *(*(pTable+Idx)+1);
			*(pQAMSeq+nsymb)=complex<float>(I,Q);
		}

//		bool WriteFlag = (*pOutBuf).Write(pQAMSeq);

		if(PSFlag)
		{cout<<"Modulation completed"<<endl;}

		///////////End Modulating////////////
//	}
//	else
//	{cout<<"unable to read input buffer"<<endl;}

}

Modulation::Modulation(BSPara* pBS)
{
	PSFlag = (*pBS).ProcessingStatusFlag;
	BufFlag=(*pBS).BufferSizeFlag;

	SCFDMAFlag=true;
	
	NumULSymbSF=(*pBS).NumULSymbSF;
	NumRxAntenna=(*pBS).NumRxAntenna;
	MDFT=(*pBS).MDFTPerUser;
	MQAM=(*pBS).MQAMPerUser;
	NumLayer=(*pBS).NumLayerPerUser;

	QAMLen=MDFT*(NumULSymbSF-2);
	BitsPerSymb = ((int)((log((float)MQAM))/(log(2.0))));
	NInfoBits=QAMLen*BitsPerSymb;

	InitModTables();
	switch(MQAM)
	{
	case 2: pTable=pBPSKtable;break;
	case 4: pTable=pQPSKtable;break;
	case 16:pTable=pQAM16table;break;
	case 64:pTable=pQAM64table;break;
	default: cout<<"Invalid modulation scheme"<<endl;
	}  

//	InitBitMap(MQAM);
//	pDecQAMSeq = new complex<float>[QAMLen*NumLayer];
//	pLLR = new float[NInfoBits*NumLayer];

//////////////////////// Calc In/Out buffer size//////////////////////////
//	InBufSz[0]=1;InBufSz[1]=QAMLen*NumLayer;
	InBufSz = QAMLen * NumLayer;
//	OutBufSz[0]=1;OutBufSz[1]=NInfoBits*NumLayer;
	OutBufSz = NInfoBits * NumLayer;
	
	/*
	if(BufFlag)
	{
		cout<<"Demodulation"<<endl;
		cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  complex<float>"<<endl;
		cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  float"<<endl; 
	}
	else
	{}
	*/
//////////////////////End of clac in/out buffer size//////////////////////
////////////////////// Initialize its own Input Buffer //////////////////////////
//ModInBuf=FIFO<int>(1,InBufSz);
//	pInpBuf =new FIFO<complex<float> >[1];
//	(*pInpBuf).initFIFO(1,InBufSz);
//////////////////End of initialization of its own input buffer//////////////////
}

float Modulation::vecmin(float* pV,int len)
{
	float minValue = INF;
	
	for(int i=0;i<len;i++)
	{
		if (*(pV+i)<minValue)
		{
			minValue=*(pV+i);
		}
		else
		{}
	}
	
	return minValue;
}

/*
 * Euclidean distance of two complex values: a+bi and c+di
 */
float Modulation::eudist(float a, float b, float c, float d)
{
	return sqrt((a - c) * (a - c) + (b - d) * (b - d));
}

void Modulation::Demodulating(complex<float> *pDecQAMSeq, int *pHD)
{
	float dist, mindist;
	int closest;

		
	int *bitmap = new int[MQAM * BitsPerSymb];
	int *bvec = new int[BitsPerSymb];

	/* set bitmap */
	for (int i = 0; i < MQAM; i++)
	{
		dec2bits(i, BitsPerSymb, bvec);
		for (int j = 0; j < BitsPerSymb; j++)
		{
			bitmap[i * BitsPerSymb + j] = bvec[j];
		}
	}
	/* set bitmap over... */


	/* Start demodulation */
	for (int i = 0; i < QAMLen * NumLayer; i++)
	{
		mindist = eudist(pTable[0][0], pTable[0][1], real(pDecQAMSeq[i]), imag(pDecQAMSeq[i]));
		
		closest = 0;
		for (int j = 1; j < MQAM; j++)
		{
			//	dist = std::abs(symbols(j) - signal(i));
			dist = eudist(pTable[j][0], pTable[j][1], real(pDecQAMSeq[i]), imag(pDecQAMSeq[i]));
			if (dist < mindist)
			{
				mindist = dist;
				closest = j;
			}
		}
		//	bits.replace_mid(i*k, bitmap.get_row(closest));
		for (int j = 0; j < BitsPerSymb; j++)
		{
			pHD[i * BitsPerSymb + j] = bitmap[closest * BitsPerSymb + j];
		}
	}
	/* End demodulation*/

	delete[] bitmap;
	delete[] bvec;
}

//void Modulation::Demodulating(FIFO<float>* pOutBuf,complex<float>***pEqW,complex<float>***pHdm,float awgnSigma)
void Modulation::Demodulating(complex<float> *pDecQAMSeq, float *pLLR, complex<float>***pEqW, complex<float>***pHdm, float awgnSigma)
{
	if(PSFlag)
	{cout<<"Demodulating"<<endl;}
	
	complex<float> ***VpEqW = pEqW;
	complex<float> ***VpHdm = pHdm;
	float No=2.0*(pow(awgnSigma,2.0));

///////////// Calculate W ////////////////
	complex<float> *pSC_Cntx;
	float *pSCFactor;
	complex<float> ***pW;
	
	pSC_Cntx = new complex<float>[NumLayer];
	
	if(SCFDMAFlag)
	{
//////////////// SCFDMA = true //////////////////
		pSCFactor = new float[NumLayer];
		
		for(int layer=0;layer<NumLayer;layer++)
		{
			*(pSCFactor+layer)=0.0;
			for(int m=0;m<MDFT;m++)
			{
				float w2=0.0;
				for(int nrx=0;nrx<NumRxAntenna;nrx++)
				{
					complex<float> wnrx=*(*(*(VpEqW+m)+layer)+nrx);
					w2+=(pow((abs(wnrx)),2.0));
				}
				(*(pSCFactor+layer))+=w2;
			}
			(*(pSCFactor+layer))/=((float)MDFT);
		}

		for(int layer=0;layer<NumLayer;layer++)
		{
			*(pSC_Cntx+layer)=0.0;
			for(int m=0;m<MDFT;m++)
			{
				complex<float> Cmntx=0.0;
				for(int nrx=0;nrx<NumRxAntenna;nrx++)
				{
					Cmntx+=( ( *(*(*(VpHdm+m)+nrx)+layer) )*( *(*(*(VpEqW+m)+layer)+nrx) ) );
				}
				( *(pSC_Cntx+layer) )+=Cmntx;
			}
			(*(pSC_Cntx+layer))/= ((float)MDFT);
		}
		
////////////// END SCFDMA = true ////////////////
	}
	else
	{
//////////////// SCFDMA = false //////////////////
		pW = new complex<float>**[MDFT];
		
		for(int m=0;m<MDFT;m++)
		{
			*(pW+m)=new complex<float>*[NumLayer];
			for(int l=0;l<NumLayer;l++)
			{
				*(*(pW+m)+l)=new complex<float>[NumRxAntenna];
			}
		}

		for(int m=0;m<MDFT;m++)
		{
			for(int layer=0;layer<NumLayer;layer++)
			{
				for(int nrx=0;nrx<NumRxAntenna;nrx++)
				{*(*(*(pW+m)+layer)+nrx)=*(*(*(VpEqW+m)+layer)+nrx);}
			}
		}
////////////// END SCFDMA = false ////////////////
		for(int layer=0;layer<NumLayer;layer++)
		{*(pSC_Cntx+layer)=complex<float>(1.0,0.0);}
	}

//////////////END Calculate W /////////////////
//	bool ReadFlag = (*pInpBuf).Read(pDecQAMSeq);

//	if(ReadFlag)
//	{
		int** pIdxTable = new int*[MQAM];
		for(int i=0;i<MQAM;i++){*(pIdxTable+i)=new int[BitsPerSymb];}
		for(int i=0;i<MQAM;i++){for(int b=0;b<BitsPerSymb;b++){*(*(pIdxTable+i)+b)=0;}}
		for(int idx=0;idx<MQAM;idx++)
		{ 
 
			int IdxVal=idx;
			int b=BitsPerSymb-1;
			while(IdxVal)
			{

				*(*(pIdxTable+idx)+b)=(IdxVal%2);

				IdxVal/=2;
				b--;
			}
		}
        
		for(int nsymb=0;nsymb<QAMLen*NumLayer;nsymb++)
		{

			int numlayer = nsymb/QAMLen;

			//////////////////////Calc metric///////////////////////
			float* pMetric = new float[MQAM];
			for(int idx=0;idx<MQAM;idx++){*(pMetric+idx)=pow(abs(((*(pDecQAMSeq+nsymb))-((*(pSC_Cntx+numlayer))*(complex<float>((*(*(pTable+idx)+0)),(*(*(pTable+idx)+1))))))),2.0);}
     
			//////////////////END Calc metric///////////////////////
			/////////////////// Generate LLR for each bit ///////////////
			for(int nbit=0;nbit<BitsPerSymb;nbit++)
			{
				float** pMetricSet=new float*[2];
				*(pMetricSet+0)=new float[MQAM/2];*(pMetricSet+1)=new float[MQAM/2];
      
				int MSidx0=0; int MSidx1=0;
				for(int midx=0;midx<MQAM;midx++)
				{
					if((*(*(pIdxTable+midx)+nbit))==0)
					{*(*(pMetricSet+0)+MSidx0)=*(pMetric+midx);MSidx0++;}
					else
					{*(*(pMetricSet+1)+MSidx1)=*(pMetric+midx);MSidx1++;}
				}
     
				float minZero,minOne;
				minZero = vecmin(*(pMetricSet+0),MQAM/2);
				minOne  = vecmin(*(pMetricSet+1),MQAM/2);
      
				///////////////////////////////////////////
				float WNo;
				if(SCFDMAFlag)
				{  
					WNo=(*(pSCFactor+numlayer))*No;
				}
				else
				{ 
					int numsc = nsymb%MDFT;
					float W=0.0;
					for(int nrx=0;nrx<NumRxAntenna;nrx++)
					{
						W+=pow((abs((*(*(*(pW+numsc)+numlayer)+nrx)))),2.0);
					}
					WNo=W*No;
				} 
				////////////////////////////////////////
				if(WNo==(float)0){*(pLLR+nsymb*BitsPerSymb+nbit)=(minZero-minOne);}
				else
				{*(pLLR+nsymb*BitsPerSymb+nbit)=(minZero-minOne)/WNo;}
				delete[] *(pMetricSet+0); delete[] *(pMetricSet+1);
				delete[] pMetricSet;       
			}
			////////////////// END Generate LLR for each bit /////////////////
			delete[] pMetric;
		}  
		for(int i=0;i<MQAM;i++){delete[] *(pIdxTable+i);}
		delete[] pIdxTable;
    
//		bool WriteFlag = (*pOutBuf).Write(pLLR);
//    if(WriteFlag){cout<<"successfully written!"<<endl;}else{}
		if(PSFlag)
		{cout<<"Demodulation completed"<<endl;} 
//	}
//	else
//	{cout<<"fail to read data from previous buffer"<<endl;}
		
	if(SCFDMAFlag)
	{
		delete[] pSCFactor;
	}
	else
	{
		for(int m=0;m<MDFT;m++)
		{
			for(int layer=0;layer<NumLayer;layer++)
			{
				delete[] *(*(pW+m)+layer);
			}
			delete[] *(pW+m);
		}
		delete[] pW;
	}
  
	delete[] pSC_Cntx;
}


///////////////////////////////////////////////
//void Modulation::Demodulating(FIFO<float>* pOutBuf,float awgnSigma)
void Modulation::Demodulating(complex<float> *pDecQAMSeq, float *pLLR, float awgnSigma)
{
	if(PSFlag)
	{cout<<"Demodulating"<<endl;}
	float No=2.0*(pow(awgnSigma,2.0));
//	bool ReadFlag = (*pInpBuf).Read(pDecQAMSeq);

//	if(ReadFlag)
//	{
		int** pIdxTable = new int*[MQAM];
		for(int i=0;i<MQAM;i++){*(pIdxTable+i)=new int[BitsPerSymb];}
		for(int i=0;i<MQAM;i++){for(int b=0;b<BitsPerSymb;b++){*(*(pIdxTable+i)+b)=0;}}
		for(int idx=0;idx<MQAM;idx++)
		{ 
 
			int IdxVal=idx;
			int b=BitsPerSymb-1;
			while(IdxVal)
			{
				*(*(pIdxTable+idx)+b)=(IdxVal%2);
				IdxVal/=2;
				b--;
			}
		}
		for(int nsymb=0;nsymb<QAMLen*NumLayer;nsymb++)
		{
			//////////////////////Calc metric///////////////////////
			float* pMetric = new float[MQAM];
			for(int idx=0;idx<MQAM;idx++){*(pMetric+idx)=pow(abs(((*(pDecQAMSeq+nsymb))-((complex<float>((*(*(pTable+idx)+0)),(*(*(pTable+idx)+1))))))),2.0);}
			//////////////////END Calc metric///////////////////////
			/////////////////// Generate LLR for each bit ///////////////
			for(int nbit=0;nbit<BitsPerSymb;nbit++)
			{
				float** pMetricSet=new float*[2];
				*(pMetricSet+0)=new float[MQAM/2];*(pMetricSet+1)=new float[MQAM/2];
      
				int MSidx0=0; int MSidx1=0;
				for(int midx=0;midx<MQAM;midx++)
				{
					if((*(*(pIdxTable+midx)+nbit))==0)
					{*(*(pMetricSet+0)+MSidx0)=*(pMetric+midx);MSidx0++;}
					else
					{*(*(pMetricSet+1)+MSidx1)=*(pMetric+midx);MSidx1++;}
				}   
				float minZero,minOne;
				minZero = vecmin(*(pMetricSet+0),MQAM/2);
				minOne  = vecmin(*(pMetricSet+1),MQAM/2);

				if(No==(float)0){*(pLLR+nsymb*BitsPerSymb+nbit)=(minZero-minOne);}
				else
				{*(pLLR+nsymb*BitsPerSymb+nbit)=(minZero-minOne)/No;}
				delete[] *(pMetricSet+0); delete[] *(pMetricSet+1);
				delete[] pMetricSet;       
			}
			////////////////// END Generate LLR for each bit /////////////////
			delete[] pMetric;
		} 
		for(int i=0;i<MQAM;i++){delete[] *(pIdxTable+i);}
		delete[] pIdxTable;
//		bool WriteFlag = (*pOutBuf).Write(pLLR);   
		if(PSFlag)
		{cout<<"Demodulation completed"<<endl;}   
//	}
//	else
//	{cout<<"fail to read data from previous buffer"<<endl;}

}


///////////////////////////////////////////////

Modulation::~Modulation()
{
	delete[] pBPSKtable;
	delete[] pQPSKtable;
	delete[] pQAM16table;
	delete[] pQAM64table;

//	delete[] bitmap;
//	delete[] pBitsSeq;
//	delete[] pQAMSeq;

//	delete[] pInpBuf;
}
