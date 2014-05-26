
#include "resmapper.h"


RESMapper::RESMapper(UserPara* pUser)
{
	PSFlag = (*pUser).ProcessingStatusFlag;
	BufFlag= (*pUser).BufferSizeFlag;
	NIFFT = (*pUser).NIFFT;
	NumULSymbSF = (*pUser).NumULSymbSF;
	MDFT = (*pUser).MDFT;
	NumLayer = (*pUser).NumLayer;
	SCLoc = (*pUser).SCLoc;

	DMRSSymbPos[0]=(*pUser).DMRSSymbPos[0];
	DMRSSymbPos[1]=(*pUser).DMRSSymbPos[1];

	VpDMRS = (*pUser).pDMRS;

	/*
	pInpData=new complex<float>* [(NumULSymbSF-2)*NumLayer];
	for(int i=0;i<(NumULSymbSF-2)*NumLayer;i++)
	{
		*(pInpData+i)=new complex<float>[MDFT];
	}
	pOutData=new complex<float>* [NumULSymbSF*NumLayer];
	for(int i=0;i<NumULSymbSF*NumLayer;i++){*(pOutData+i)=new complex<float>[NIFFT];}
	for(int i=0;i<NumULSymbSF*NumLayer;i++)
	{
		for(int j=0;j<NIFFT;j++)
		{*(*(pOutData+i)+j)=complex<float>(1.0,0.0);}
	}
	*/

	//////////////////////// Calc In/Out buffer size//////////////////////////
//	InBufSz[0]=(NumULSymbSF-2)*NumLayer; InBufSz[1]=MDFT;
//	OutBufSz[0]=NumULSymbSF*NumLayer; OutBufSz[1]=NIFFT;
	InBufSz = NumLayer * (NumULSymbSF - 2) * MDFT;
	OutBufSz = NumLayer * NumULSymbSF * NIFFT;

	/*
	if(BufFlag)
	{
		cout<<"SubCarrierMap"<<endl;
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


RESMapper::RESMapper(BSPara* pBS)
{
	PSFlag = (*pBS).ProcessingStatusFlag;
	BufFlag=(*pBS).BufferSizeFlag;

	NumULSymbSF=(*pBS).NumULSymbSF;
	NIFFT=(*pBS).NIFFT;
	DMRSSymbPos[0]=(*pBS).DMRSSymbPos[0];
	DMRSSymbPos[1]=(*pBS).DMRSSymbPos[1];

	SCLocPattern=(*pBS).SCLocPattern;
	MDFTPerUser=(*pBS).MDFTPerUser;

	NumRxAntenna = (*pBS).NumRxAntenna;

	/*
	pInpData=new complex<float>*[NumULSymbSF*NumRxAntenna];
	for(int s=0;s<NumULSymbSF*NumRxAntenna;s++){*(pInpData+s)=new complex<float>[NIFFT];}

	pOutData = new complex<float>*[NumRxAntenna*NumULSymbSF];
	for(int s=0;s<NumRxAntenna*NumULSymbSF;s++){*(pOutData+s)=new complex<float>[MDFTPerUser];}
	*/
	//////////////////////// Calc In/Out buffer size//////////////////////////
//	InBufSz[0]=NumULSymbSF*NumRxAntenna; InBufSz[1]=NIFFT;
//	OutBufSz[0]=NumRxAntenna*NumULSymbSF; OutBufSz[1]=MDFTPerUser;
	InBufSz = NumRxAntenna * NumULSymbSF * NIFFT;
	OutBufSz = NumRxAntenna * NumULSymbSF * MDFTPerUser;
	
	/*
	if(BufFlag)
	{
		cout<<"SubCarrierDemap"<<endl;
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

//void RESMapper::SubCarrierMapping(FIFO<complex<float> > *pOutBuf)
void RESMapper::SubCarrierMapping(complex<float> *pInpData, complex<float> *pOutData)
{
 	if(PSFlag)
	{
		cout<<"SubCarrierMapping"<<endl;
	}
	
	for (int nlayer = 0; nlayer < NumLayer; nlayer++)
	{
		for (int nsym = 0; nsym < NumULSymbSF; nsym++)
		{
			int SymIdx = nlayer * NumULSymbSF + nsym;

			for (int n = 0; n < NIFFT; n++)
			{
				pOutData[SymIdx * NIFFT + n] = complex<float>(1.0, 0.0);
			}
		}
	}

//	bool ReadFlag = (*pInpBuf).Read(pInpData);
//	if(ReadFlag)
//	{
		//////////////// Start subcarrier mapper //////////////
		for (int nlayer = 0; nlayer < NumLayer; nlayer++)
		{
			int DMRSslot = 0;
			
			for (int nsym = 0; nsym < NumULSymbSF; nsym++)
			{
				int SymIdx = nlayer * NumULSymbSF + nsym;
				
				if (nsym == DMRSSymbPos[DMRSslot])
				{
					for (int n = SCLoc; n < SCLoc + MDFT; n++)
					{
						//	*(*(pOutData+SymIdx)+n)=*(*(*(VpDMRS+DMRSslot)+nlayer)+n-SCLoc);
						pOutData[SymIdx * NIFFT + n] = *(*(*(VpDMRS + DMRSslot) + nlayer) + n - SCLoc);
					}
					DMRSslot++;
				}
				else
				{

					for (int n=SCLoc;n<SCLoc+MDFT;n++)
					{
						//	*(*(pOutData+SymIdx)+n)=*(*(pInpData+nlayer*(NumULSymbSF-2)+nsym-DMRSslot)+n-SCLoc);
						pOutData[SymIdx * NIFFT + n] = pInpData[nlayer * (NumULSymbSF - 2) * MDFT + (nsym - DMRSslot) * MDFT + n - SCLoc];
					}
				}
			}
		}

//		bool WriteFlag = (*pOutBuf).Write(pOutData);
//		if(PSFlag)
//		{cout<<"SubCarrierMap completed"<<endl;}

		/////////////End subcarrier mapper ////////////////////
//	}
//	else
//	{cout<<"unable to read input buffer"<<endl;}

}


//void RESMapper::SubCarrierDemapping(FIFO<complex<float> >* pOutBuf)
void RESMapper::SubCarrierDemapping(complex<float> *pInpData, complex<float> *pOutData)
{
	if(PSFlag)
	{
		cout<<"SubCarrierDemapping"<<endl;
	}
	
//	bool ReadFlag = (*pInpBuf).Read(pInpData);
//	if(ReadFlag)
//	{

		int MDFT = MDFTPerUser;
		int SCLoc = SCLocPattern;

		//////////// Get DMRS /////////////
		for (int nrs = 0; nrs < 2; nrs++)
		{
			int DMRSPos = DMRSSymbPos[nrs];
			
			for(int nrx=0;nrx<NumRxAntenna;nrx++)
			{

				int SymIdxIn = nrx*NumULSymbSF+DMRSPos;
				int SymIdxOut= nrx*2+nrs;

				for(int n=0;n<MDFT;n++)
				{
					//	*(*(pOutData+SymIdxOut)+n)=*(*(pInpData+SymIdxIn)+(n+SCLoc));
					pOutData[SymIdxOut * MDFT + n] = pInpData[SymIdxIn * NIFFT + (n + SCLoc)];
				}
			}
		}
		///////////END Get DMRS////////////
		
		//////////// Get Data /////////////
		for(int nrx=0;nrx<NumRxAntenna;nrx++)
		{
			int SymOutIdx=0;
			for(int nsym=0;nsym<NumULSymbSF;nsym++)
			{
				int SymIdxIn = NumULSymbSF*nrx+nsym;
				if(((nsym==DMRSSymbPos[0])||(nsym==DMRSSymbPos[1])))
				{}
				else
				{
					int SymOutT=(NumULSymbSF-2)*nrx+SymOutIdx+NumRxAntenna*2;

					//	for(int n=0;n<MDFT;n++){*(*(pOutData+SymOutT)+n)=*(*(pInpData+SymIdxIn)+(n+SCLoc));}
					for (int n = 0; n < MDFT; n++)
					{
						pOutData[SymOutT * MDFT + n] = pInpData[SymIdxIn * NIFFT + (n + SCLoc)];
					}
					SymOutIdx++;
				}
			}
		}
		////////////END Get Data///////////
//		bool WriteFlag = (*(pOutBuf)).Write(pOutData);
		//  if(WriteFlag){cout<<"successfully written!"<<endl;}else{}
//		if(PSFlag)
//		{cout<<"SubCarrierDemap completed"<<endl;}
//	}
//	else
//	{cout<<"fail to read data from previous buffer"<<endl;}

}

RESMapper::~RESMapper()
{
//	delete[] pOutData;
//	delete[] pInpData;
//	delete[] pInpBuf;
}
