
#include "TransformPrecoder.h"
#include <cstring>

TransformPrecoder::TransformPrecoder(UserPara* pUser)
{
	PSFlag = (*pUser).ProcessingStatusFlag;
	BufFlag=(*pUser).BufferSizeFlag;
	QAMLen=(*pUser).QAMLen;
	NumULSymbSF=(*pUser).NumULSymbSF;
	MDFT=(*pUser).MDFT;
	NumLayer=(*pUser).NumLayer;

	
//	pQAMSeq = new complex<float>[NumLayer*QAMLen];
//	pDataMatrix=new complex<float>*[NumLayer*(NumULSymbSF-2)];
//	for(int nsym=0;nsym<(NumULSymbSF-2)*NumLayer;nsym++)
//	{*(pDataMatrix+nsym)=new complex<float>[MDFT];}
	/*
	complex<float> *in,*out;
	fftplan=fftwf_plan_dft_1d(MDFT,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out),FFTW_FORWARD,FFTW_ESTIMATE);
	*/
	
	//////////////////////// Calc In/Out buffer size//////////////////////////
	
//	InBufSz[0]=1; InBufSz[1]=NumLayer*QAMLen;
	InBufSz = NumLayer * QAMLen;
//	OutBufSz[0]=NumLayer*(NumULSymbSF-2); OutBufSz[1]=MDFT;
	OutBufSz = NumLayer * (NumULSymbSF - 2) * MDFT;
	/*
	if(BufFlag)
	{
		cout<<"Transform Precoder"<<endl;
		cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  complex<float>"<<endl;
		cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  complex<float>"<<endl; 
	}
	else
	{}
	*/
	
	//////////////////////End of clac in/out buffer size//////////////////////
	////////////////////// Initialize its own Input Buffer //////////////////////////
//	pInpBuf =new FIFO<complex<float> >[1];
//	(*pInpBuf).initFIFO(1,InBufSz);
	//////////////////End of initialization of its own input buffer//////////////////

}

//void TransformPrecoder::TransformPrecoding(FIFO<complex<float> > *pOutBuf)
void TransformPrecoder::TransformPrecoding(complex<float> *pQAMSeq, complex<float> *pDataMatrix)
{

//	if(PSFlag)
//	{cout<<"TransformPrecoding"<<endl;}
//	bool ReadFlag = (*pInpBuf).Read(pQAMSeq);
//	if(ReadFlag)
//	{
	fftwf_plan fftplan;
	complex<float> *in,*out;

	in = new complex<float>[MDFT];
	out = new complex<float>[MDFT];
	
	fftplan=fftwf_plan_dft_1d(MDFT,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out),FFTW_FORWARD,FFTW_ESTIMATE);
		///////////// Start transform precoding /////////////////
		for(int nlayer=0;nlayer<NumLayer;nlayer++)
		{
			for(int nsym=0;nsym<NumULSymbSF-2;nsym++)
			{

				int idx = nlayer*(MDFT*(NumULSymbSF-2))+nsym*MDFT;
				//	int symIdx=nlayer*(NumULSymbSF-2)+nsym;

				//	fftwf_execute_dft(fftplan,reinterpret_cast<fftwf_complex*>((pQAMSeq+idx)),reinterpret_cast<fftwf_complex*>((*(pDataMatrix+symIdx))));

				memcpy(in, &pQAMSeq[idx], MDFT * sizeof(complex<float>));
				//	fftwf_execute_dft(fftplan, reinterpret_cast<fftwf_complex*>(&pQAMSeq[idx]), reinterpret_cast<fftwf_complex*>(&pDataMatrix[idx]));
				fftwf_execute_dft(fftplan, reinterpret_cast<fftwf_complex*>(in), reinterpret_cast<fftwf_complex*>(out));
				
				for(int m = 0;m < MDFT; m++)
				{
					pDataMatrix[idx + m] = out[m] / sqrt((float)MDFT);
				}
				
			}
		}

		delete[] in;
		delete[] out;
		fftwf_destroy_plan(fftplan);

		//	bool WriteFlag = (*pOutBuf).Write(pDataMatrix);

		if(PSFlag)
		{cout<<"TransformPrecode completed"<<endl;}


		//////////////End transform precoding////////////////////
//	}
//	else
//	{cout<<"unable to read input buffer"<<endl;}

}


TransformPrecoder::TransformPrecoder(BSPara* pBS)
{
	PSFlag = (*pBS).ProcessingStatusFlag;
	BufFlag = (*pBS).BufferSizeFlag;

	NumULSymbSF=(*pBS).NumULSymbSF;
	QAM=(*pBS).MQAMPerUser;
	MDFT=(*pBS).MDFTPerUser;
	NumLayer=(*pBS).NumLayerPerUser;

	SymbNum=(NumULSymbSF-2)*(NumLayer);
	QAMLen = MDFT*(NumULSymbSF-2);

	/*
	pDataMatrix=new complex<float>*[SymbNum];
	for(int s=0;s<SymbNum;s++){*(pDataMatrix+s)=new complex<float>[MDFT];}
	pDecQAMSeq = new complex<float>[QAMLen*NumLayer];
	*/

	/*
	complex<float> *in,*out;     
	ifftplan=fftwf_plan_dft_1d(MDFT,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out),FFTW_BACKWARD,FFTW_ESTIMATE);
	*/
	
	//////////////////////// Calc In/Out buffer size//////////////////////////
//	InBufSz[0]=SymbNum;InBufSz[1]=MDFT;
	InBufSz = SymbNum * MDFT;
//	OutBufSz[0]=1;OutBufSz[1]=QAMLen*NumLayer;
	OutBufSz = QAMLen * NumLayer;
/*
	if(BufFlag)
	{
		cout<<"Transform Decoder"<<endl;
		cout<<"Input buffer size is "<<"{"<<InBufSz[0]<<" , "<<InBufSz[1]<<"}"<<"  complex<float>"<<endl;
		cout<<"Output buffer size is "<<"{"<<OutBufSz[0]<<" , "<<OutBufSz[1]<<"}"<<"  complex<float>"<<endl; 
	}
	else
	{}
*/
	//////////////////////End of clac in/out buffer size//////////////////////
	////////////////////// Initialize its own Input Buffer //////////////////////////
//	pInpBuf =new FIFO<complex<float> >[1];
//	(*pInpBuf).initFIFO(1,InBufSz);
	//////////////////End of initialization of its own input buffer//////////////////
}

//void TransformPrecoder::TransformDecoding(FIFO<complex<float> >* pOutBuf)
void TransformPrecoder::TransformDecoding(complex<float> *pDataMatrix, complex<float> *pDecQAMSeq)
{

	if(PSFlag)
	{cout<<"TransformDecoding"<<endl;}

	fftwf_plan ifftplan;
	complex<float> *in,*out;

	in = new complex<float>[MDFT];
	out = new complex<float>[MDFT];
	
	ifftplan=fftwf_plan_dft_1d(MDFT,reinterpret_cast<fftwf_complex*>(in),reinterpret_cast<fftwf_complex*>(out),FFTW_BACKWARD,FFTW_ESTIMATE);
//	bool ReadFlag = (*pInpBuf).Read(pDataMatrix);
//	if(ReadFlag)
//	{

		for(int nlayer=0;nlayer<NumLayer;nlayer++)
		{
			for(int nsym=0;nsym<NumULSymbSF-2;nsym++)
			{
				int idx = nlayer*(MDFT*(NumULSymbSF-2))+nsym*MDFT;
				//	int symIdx=nlayer*(NumULSymbSF-2)+nsym;

				memcpy(in, &pDataMatrix[idx], MDFT * sizeof(complex<float>));
				//	fftwf_execute_dft(ifftplan,reinterpret_cast<fftwf_complex*>((*(pDataMatrix+symIdx))),reinterpret_cast<fftwf_complex*>((pDecQAMSeq+idx)));
				//	fftwf_execute_dft(ifftplan, reinterpret_cast<fftwf_complex*>(&pDataMatrix[idx]), reinterpret_cast<fftwf_complex*>(&pDecQAMSeq[idx]));
				fftwf_execute_dft(ifftplan, reinterpret_cast<fftwf_complex*>(in), reinterpret_cast<fftwf_complex*>(out));

				for(int m = 0; m < MDFT; m++)
				{
					pDecQAMSeq[idx + m] = out[m] / sqrt((float)MDFT);
				}                              
			}
		}
		//	bool WriteFlag = (*pOutBuf).Write(pDecQAMSeq);
		//       if(WriteFlag){cout<<"successfully written!"<<endl;}else{}
		if(PSFlag)
		{cout<<"TransformDecode completed"<<endl;}
//	}
//	else
//	{cout<<"fail to read data from previous buffer"<<endl;}
		delete[] in;
		delete[] out;
		fftwf_destroy_plan(ifftplan);
}


TransformPrecoder::~TransformPrecoder()
{
}
