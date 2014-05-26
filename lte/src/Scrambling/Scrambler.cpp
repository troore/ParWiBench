
#include "Scrambler.h"

template <class T>
Scrambler<T>::Scrambler(UserPara* pUser)
{
	PSFlag = (*pUser).ProcessingStatusFlag;
	BufFlag=(*pUser).BufferSizeFlag;
	NumLayer=(*pUser).NumLayer;
	NInfoBits=(*pUser).NInfoBits;


	//////////////////////// Calc In/Out buffer size//////////////////////////
//	InBufSz[0]=1; InBufSz[1]=NumLayer*NInfoBits;
//	OutBufSz[0]=1;OutBufSz[1]=NumLayer*NInfoBits;
	InBufSz = NumLayer * NInfoBits;
	OutBufSz = NumLayer * NInfoBits;

	pScrambInt = new int[InBufSz];
//	pScrambInt[0] = 0;
	
	/*
	if(BufFlag)
	{
		cout<<"Scramble"<<endl;
		cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  int"<<endl;
		cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  int"<<endl; 
	}
	else
	{}
	*/
	//////////////////////End of clac in/out buffer size//////////////////////
	////////////////////// Initialize its own Input Buffer //////////////////////////
//	pInpBuf =new FIFO<int>[1];
//	(*pInpBuf).initFIFO(1,InBufSz);
	//////////////////End of initialization of its own input buffer//////////////////
}

template <class T>
//void Scrambler<T>::Scrambling(FIFO<T> *pOutBuf)
void Scrambler<T>::Scrambling(T *pInpSeq, T *pOutSeq)
{
	if(PSFlag)
	{cout<<"Scrambling"<<endl;}

	int Mpn = NumLayer*NInfoBits;
	int *pScrambSeq = new int[Mpn];

	// Generate integer scrambling sequence
	GenScrambInt();

	for (int n = 0; n < Mpn; n++)
	{
		pScrambSeq[n] = pScrambInt[n];
	}
//	bool ReadFlag = (*pInpBuf).Read(pInpSeq);
//	if(ReadFlag)
//	{
	////////////////////////Scrambling////////////////////////////
	for(int n=0;n<Mpn;n++)
	{
		*(pOutSeq+n)=((*(pInpSeq+n))+(*(pScrambSeq+n)))%2;
	}
	////////////////////////END Scrambling////////////////////////////
//		bool WriteFlag = (*pOutBuf).Write(pOutSeq);
	if(PSFlag)
	{cout<<"Scramble completed"<<endl;}

//	}
//	else
//	{cout<<"unable to read input buffer"<<endl;}

	delete[] pScrambSeq;

}

template <class T>
Scrambler<T>::Scrambler(BSPara* pBS)
{
	PSFlag= (*pBS).ProcessingStatusFlag;
	BufFlag=(*pBS).BufferSizeFlag;
	NumULSymbSF=(*pBS).NumULSymbSF;


	MDFT=(*pBS).MDFTPerUser;
	MQAM=(*pBS).MQAMPerUser;
	NumLayer=(*pBS).NumLayerPerUser;
	NInfoBits = MDFT * (NumULSymbSF - 2) * ((int)(log((double)MQAM) / log(2.0)));
//////////////////////// Calc In/Out buffer size//////////////////////////
//	InBufSz[0]=1;InBufSz[1]=Mpn;
//	OutBufSz[0]=1;OutBufSz[1]=Mpn;
	InBufSz = NumLayer * NInfoBits;
	OutBufSz = InBufSz;

	pScrambInt = new int[InBufSz];
	/*
	if(BufFlag)
	{
		cout<<"Descramble"<<endl;
		cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  float"<<endl;
		cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  float"<<endl; 
	}
	else
	{}
	*/
//////////////////////End of clac in/out buffer size//////////////////////
////////////////////// Initialize its own Input Buffer //////////////////////////
//	pInpBuf =new FIFO<float>[1];
//	(*pInpBuf).initFIFO(1,InBufSz);
//////////////////End of initialization of its own input buffer//////////////////
}

template <class T>
//void Scrambler<T>::Descrambling(FIFO<T>* pOutBuf)
void Scrambler<T>::Descrambling(T *pInpSeq, T *pOutSeq)
{
	if(PSFlag)
	{cout<<"Descrambling"<<endl;  }
	
	int Mpn=NumLayer*NInfoBits;

	float *pScrambSeq=new float[Mpn];

	// Generate integer scrambling sequence
	GenScrambInt();

	for(int n=0;n<Mpn;n++)
	{
		if(*(pScrambInt+n)==1)
		{
			*(pScrambSeq+n)=-1.0;
		}
		else
		{
			*(pScrambSeq+n)=+1.0;
		}
	}


//	bool ReadFlag=(*pInpBuf).Read(pInpSeq);
//	if(ReadFlag)
//	{
		//////////////////////Descrambling////////////////////////////
		for(int n=0;n<Mpn;n++)
		{
			*(pOutSeq+n)=(*(pInpSeq+n))*(*(pScrambSeq+n));
		}
		///////////////////////END Descrambling/////////////////////////
//		bool WriteFlag = (*pOutBuf).Write(pOutSeq);
//  if(WriteFlag){cout<<"successfully written!"<<endl;}else{}
		if(PSFlag)
		{cout<<"Descramble completed"<<endl;  }
//	}
//	else
//	{cout<<"fail to read data from previous buffer"<<endl;}

			
		delete[] pScrambSeq;
}

template <class T>
void Scrambler<T>::GenScrambInt()
{

	int Mpn=NumLayer*NInfoBits;
	
	int n_init[31]={1,1,0,1,0,0,1,1,1,0,0,1,0,0,1,1,0,1,1,0,0,0,1,1,1,1,0,1,1,1,0};

	/////////////////////Generate ScrambSeq///////////////////////
	int Nc=1600;
	int* px1=new int[(Mpn+Nc)];
	int* px2=new int[(Mpn+Nc)];

	for(int n=0;n<31;n++){*(px1+n)=0;*(px2+n)=n_init[n];}
	*(px1+0)=1;
	
	for(int n=0;n<Mpn+Nc-31;n++)
	{
		*(px1+n+31)=((*(px1+n+3))+(*(px1+n)))%2;
		*(px2+n+31)=((*(px2+n+3))+(*(px2+n+2))+(*(px2+n+1))+(*(px2+n)))%2;
	}
	for(int n=0;n<Mpn;n++)
	{
		pScrambInt[n] = 3;
		*(pScrambInt+n)=((*(px1+n+Nc))+(*(px2+n+Nc)))%2;
	}
	/////////////////////END Generate ScrambSeq///////////////////////
	
}

template <class T>
Scrambler<T>::~Scrambler()
{
//	delete[] pInpSeq;
	delete[] pScrambInt;
//	delete[] pOutSeq;
//	delete[] pInpBuf;
}

template Scrambler<int>::Scrambler(UserPara* pUser);
template Scrambler<float>::Scrambler(BSPara* pBS);
//template void Scrambler<int>::Scrambling(FIFO<int> *pOutBuf);
template void Scrambler<int>::Scrambling(int *pInpSeq, int *pOutSeq);
//template void Scrambler<float>::Descrambling(FIFO<float>* pOutBuf);
template void Scrambler<float>::Descrambling(float *pInpSeq, float *pOutSeq);

template Scrambler<int>::~Scrambler();
template Scrambler<float>::~Scrambler();
