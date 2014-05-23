
#include "ofdm.h"
#include <cstring>

OFDM::OFDM(UserPara* pUser)
{
	PSFlag= (*pUser).ProcessingStatusFlag;
	BufFlag=(*pUser).BufferSizeFlag;
	NIFFT = (*pUser).NIFFT;
	CPLen = (*pUser).CPLen;
	NumULSymbSF=(*pUser).NumULSymbSF;
	NumLayer=(*pUser).NumLayer;

//	user_flag = 0;

//	pInpData = new complex<float>* [NumLayer*NumULSymbSF];
//	pOutData = new complex<float>* [NumLayer*NumULSymbSF];

	/*
	  for(int nsym = 0; nsym<NumLayer*NumULSymbSF;nsym++)
	  {
	  *(pInpData+nsym)=new complex<float>[NIFFT];
	  *(pOutData+nsym)=new complex<float>[(NIFFT+CPLen)];
	  }
	*/

//	complex<float> *in;
//	out = new complex<float>[NIFFT];
//	ifftplan=fftwf_plan_dft_1d(NIFFT,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out),FFTW_BACKWARD,FFTW_ESTIMATE);

	//////////////////////// Calc In/Out buffer size//////////////////////////
//	InBufSz[0]=NumULSymbSF*NumLayer; InBufSz[1]=NIFFT;
	InBufSz = NumLayer * NumULSymbSF * NIFFT;
//	OutBufSz[0]=NumLayer*NumULSymbSF; OutBufSz[1]=(NIFFT+CPLen);
	OutBufSz = NumLayer * NumULSymbSF * (NIFFT + CPLen);
	
	//////////////////////End of clac in/out buffer size//////////////////////
	////////////////////// Initialize its own Input Buffer //////////////////////////
//	pInpBuf =new FIFO<complex<float> >[1];
//	(*pInpBuf).initFIFO(1,InBufSz);
	//////////////////End of initialization of its own input buffer//////////////////
}


OFDM::OFDM(BSPara* pBS)
{
	PSFlag = (*pBS).ProcessingStatusFlag;
	BufFlag=(*pBS).BufferSizeFlag;
	
	NumULSymbSF=(*pBS).NumULSymbSF;
	NIFFT=(*pBS).NIFFT;
	CPLen=(*pBS).CPLen;
	NumRxAntenna=(*pBS).NumRxAntenna;

	SymNum = NumULSymbSF*NumRxAntenna;

//	user_flag = 1;

	/*
	  pInpData=new complex<float>*[SymNum];
	  pOutData=new complex<float>*[SymNum];
	  for(int s=0;s<SymNum;s++)
	  {
	  *(pInpData+s)=new complex<float>[(NIFFT+CPLen)];
	  *(pOutData+s)=new complex<float>[NIFFT];
	  }
	*/


//	complex<float> *in,*out;
//	complex<float> *in;
//	out = new complex<float>[NIFFT];
//	fftplan=fftwf_plan_dft_1d(NIFFT,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out),FFTW_FORWARD,FFTW_ESTIMATE);

//////////////////////// Calc In/Out buffer size//////////////////////////
//	InBufSz[0]=NumRxAntenna*NumULSymbSF;InBufSz[1]=(NIFFT+CPLen);
	InBufSz = NumRxAntenna * NumULSymbSF * (CPLen + NIFFT);
//	OutBufSz[0]=NumULSymbSF*NumRxAntenna; OutBufSz[1]=NIFFT;
	OutBufSz = NumRxAntenna * NumULSymbSF * NIFFT;

//////////////////////End of clac in/out buffer size//////////////////////
////////////////////// Initialize its own Input Buffer //////////////////////////
//	pInpBuf =new FIFO<complex<float> >[1];
//	(*pInpBuf).initFIFO(1,InBufSz);
//////////////////End of initialization of its own input buffer//////////////////

}

//void OFDM::modulating(FIFO<complex<float> > *pOutBuf)
void OFDM::modulating(complex<float> *pInpData, complex<float> *pOutData)
{
	//cout<<"OFDM modulating"<<endl;
	if(PSFlag)
	{
		cout<<"OFDM modulating"<<endl;
	}

//	bool ReadFlag = (*pInpBuf).Read(pInpData);
	
//	if(ReadFlag)
//	{
	///////////////////// Start SCFDMA modulating /////////////////

	
//	ifftplan=fftwf_plan_dft_1d(NIFFT,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out), FFTW_BACKWARD, FFTW_ESTIMATE);

	fftwf_plan ifftplan;

	complex<float> *in, *out;

	in = new complex<float>[NIFFT];
	out = new complex<float>[NIFFT];
	
	ifftplan=fftwf_plan_dft_1d(NIFFT, reinterpret_cast<fftwf_complex*>(in), reinterpret_cast<fftwf_complex*>(out), FFTW_BACKWARD, FFTW_ESTIMATE);

	for(int nlayer=0;nlayer<NumLayer;nlayer++)
	{
		for(int nsym=0;nsym<NumULSymbSF;nsym++)
		{
			int idx=nlayer*NumULSymbSF+nsym;
			
			//		fftwf_plan ifftplan;

			//            fftwf_execute_dft(ifftplan,reinterpret_cast<fftwf_complex*>((*(pInpData+idx))),reinterpret_cast<fftwf_complex*>((*(pOutData+idx)+CPLen)));
			//	ifftplan=fftwf_plan_dft_1d(NIFFT,reinterpret_cast<fftwf_complex*>(&pInpData[idx * NIFFT]),reinterpret_cast<fftwf_complex*>(&pOutData[idx * (NIFFT + CPLen) + CPLen]), FFTW_BACKWARD, FFTW_ESTIMATE);
			memcpy(in, &pInpData[idx * NIFFT], NIFFT * sizeof(complex<float>));
			
			//	fftwf_execute_dft(ifftplan,reinterpret_cast<fftwf_complex*>(&pInpData[idx * NIFFT]),reinterpret_cast<fftwf_complex*>(&pOutData[idx * (NIFFT + CPLen) + CPLen]));
			fftwf_execute_dft(ifftplan, reinterpret_cast<fftwf_complex*>(in), reinterpret_cast<fftwf_complex*>(out));
			
			for(int n=0;n<NIFFT;n++)
			{
				//	*(*(pOutData+idx)+CPLen+n)=(*(out+n))/sqrt((float)NIFFT);
				//	pOutData[idx * (NIFFT + CPLen) + CPLen + n] /= sqrt((float)NIFFT);
				pOutData[idx * (NIFFT + CPLen) + CPLen + n] = out[n] / sqrt((float)NIFFT);
			}

			for(int n=0;n<CPLen;n++)
			{
				//	*(*(pOutData+idx)+n)=*(*(pOutData+idx)+n+NIFFT);
				pOutData[idx * (NIFFT + CPLen) + n] = pOutData[idx * (NIFFT + CPLen) + NIFFT + n];
			}
			
			//	fftwf_destroy_plan(ifftplan);
		}
	}

	delete[] in;
	delete[] out;
	
	fftwf_destroy_plan(ifftplan);

//		bool WriteFlag = (*pOutBuf).Write(pOutData);

	if(PSFlag)
	{
		cout<<"SCFDMAModulation completed"<<endl;
	}
	////////////////////End SCFDMA modulating /////////////////////
//	}
//	else
//	{
//		cout<<"unable to read input buffer"<<endl;
//	}
}

//void OFDM::demodulating(FIFO<complex<float> >* pOutBuf)
void OFDM::demodulating(complex<float> *pInpData, complex<float> *pOutData)
{
	if(PSFlag)
	{
		cout<<"OFDM demodulating"<<endl;
	}

//	bool ReadFlag = (*pInpBuf).Read(pInpData);
	
//	if(ReadFlag)
//	{
	fftwf_plan fftplan;

	complex<float> *in = new complex<float>[NIFFT];
	complex<float> *out = new complex<float>[NIFFT];

	fftplan=fftwf_plan_dft_1d(NIFFT, reinterpret_cast<fftwf_complex*>(in), reinterpret_cast<fftwf_complex*>(out), FFTW_FORWARD, FFTW_ESTIMATE);
	
	for(int nrx=0;nrx<NumRxAntenna;nrx++)
	{

		for(int nsym=0;nsym<NumULSymbSF;nsym++)
		{

			int SymIdx = nrx*NumULSymbSF+nsym;

//				fftwf_execute_dft(fftplan,reinterpret_cast<fftwf_complex*>((*(pInpData+SymIdx)+CPLen)),reinterpret_cast<fftwf_complex*>((*(pOutData+SymIdx))));
			//	fftplan=fftwf_plan_dft_1d(NIFFT,reinterpret_cast<fftwf_complex*>(&pInpData[SymIdx * (CPLen + NIFFT) + CPLen]),reinterpret_cast<fftwf_complex*>(&pOutData[SymIdx * NIFFT]),FFTW_FORWARD,FFTW_ESTIMATE);
			memcpy(in, &pInpData[SymIdx * (CPLen + NIFFT) + CPLen], NIFFT * sizeof(complex<float>));
			
			//	fftwf_execute_dft(fftplan, reinterpret_cast<fftwf_complex*>(in), reinterpret_cast<fftwf_complex*>(&pOutData[SymIdx * NIFFT]));
			fftwf_execute_dft(fftplan, reinterpret_cast<fftwf_complex*>(in), reinterpret_cast<fftwf_complex*>(out));
			for(int n = 0; n < NIFFT; n++)
			{
				pOutData[SymIdx * NIFFT + n] = out[n] / sqrt((float)NIFFT);
			}
			
//			fftwf_destroy_plan(fftplan);
		}
	}

	delete[] in;
	delete[] out;
	fftwf_destroy_plan(fftplan);

//		bool WriteFlag = (*pOutBuf).Write(pOutData);
   

	//      if(WriteFlag){cout<<"successfully written!"<<endl;}   
	if(PSFlag)
	{cout<<"SCFDMADemodulation completed"<<endl;} 
//	}
//	else
//	{cout<<"fail to read data from previous buffer"<<endl;}
}

OFDM::~OFDM()
{
	/*
	if (0 == user_flag)
		fftwf_destroy_plan(ifftplan);
	else
		fftwf_destroy_plan(fftplan);
	*/
//	delete[] pInpData;
//	delete[] pOutData;

//	if (out)
//		delete[] out;
//	delete[] pInpBuf;
}

