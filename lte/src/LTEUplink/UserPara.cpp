#include "UserPara.h"

UserPara::UserPara(BSPara *pBS)
{
	pPrimeTable=new int*[6];
	for(int i=0;i<6;i++){*(pPrimeTable+i)=new int[2];}
	pPrimeTable[0][0]=75;  pPrimeTable[0][1]=73;
	pPrimeTable[1][0]=150; pPrimeTable[1][1]=149;
	pPrimeTable[2][0]=300; pPrimeTable[2][1]=293;
	pPrimeTable[3][0]=600; pPrimeTable[3][1]=599;
	pPrimeTable[4][0]=900; pPrimeTable[4][1]=887;
	pPrimeTable[5][0]=1200;pPrimeTable[5][1]=1193;

	///////////// initialize UserPara using BSPara ////////////////////
	VpBS = pBS;
	initUserPara();
	///////////////// completed initialization ////////////////////////

}

void UserPara::initUserPara()
{

	NIFFT=(*VpBS).NIFFT;
	CPLen=(*VpBS).CPLen;
	NumULSymbSF=14;

	RSu=(*VpBS).RSU;
	RSv=(*VpBS).RSV;

	MQAM=(*VpBS).MQAMPerUser;
	MDFT=(*VpBS).MDFTPerUser;
	NumLayer=(*VpBS).NumLayerPerUser;
	SCLoc=(*VpBS).SCLocPattern;
	DataLength=(*VpBS).DataLengthPerUser;
	DMRSSymbPos[0]=(*VpBS).DMRSSymbPos[0];
	DMRSSymbPos[1]=(*VpBS).DMRSSymbPos[1];

	ProcessingStatusFlag=(*VpBS).ProcessingStatusFlag;
	BufferSizeFlag = (*VpBS).BufferSizeFlag;
	NInfoBits = MDFT*(NumULSymbSF-2)*((int)((log((double)MQAM))/(log(2.0))));
	QAMLen = MDFT*(NumULSymbSF-2);

	/* Turbo parameters */
	n_gens = VpBS->n_gens;
	for (int i = 0; i < n_gens; i++)
	{
		gens[i] = VpBS->gens[i];
	}
	constraint_length = VpBS->constraint_length;
	n_iterations = VpBS->n_iterations;
	BlkSize = VpBS->BlkSize;
	/* Turbo parameters over... */

	ncs[0]=3;ncs[1]=11;

	pDMRS = new complex<float>**[2];
	for(int slot=0;slot<2;slot++){*(pDMRS+slot)=new complex<float>*[NumLayer];}
	for(int slot=0;slot<2;slot++)
	{
		for(int layer=0;layer<NumLayer;layer++)
		{*(*(pDMRS+slot)+layer)=new complex<float>[MDFT];}
	}

	bool zcFlag=true;
	int idx=0;
	while(zcFlag)
	{
		if(pPrimeTable[idx][0]==MDFT)
		{ 
			Nzc=pPrimeTable[idx][1];
			zcFlag=false;
		}
		else
		{idx++;}
	}

	geneDMRS();

}


void UserPara::geneDMRS(void)
{
	double qbar=(double)Nzc*(RSu+1.0)/31.0;
	double q=floor((qbar+0.5))+RSv*pow(-1.0,(floor((2.0*qbar))));


	complex<double>* px= new complex<double>[Nzc];
	for(int m=0;m<Nzc;m++)
	{
		double theta = -PI*q*m*(m+1.0)/((double)Nzc);
		*(px+m)=complex<double>(cos(theta),sin(theta));
	}



	for(int slot=0;slot<2;slot++)
	{
		for(int layer=0;layer<NumLayer;layer++)
		{
			int cs = ncs[slot]+6*layer;
			double alpha = 2.0*PI*cs/12.0;
			for(int n=0;n<MDFT;n++)
			{
				int idx=n%Nzc;
				complex<double> c = complex<double>((cos(alpha*n)),(sin(alpha*n)));
				*(*(*(pDMRS+slot)+layer)+n)=(complex<float>)(c*(*(px+idx)));
			}
		}
	}

	if(NumLayer==2)
	{
		for(int n=0;n<MDFT;n++)
		{(*(*(*(pDMRS+1)+1)+n))*=(-1.0);}
	}
	else{}

	delete[] px;
}

UserPara::~UserPara(void)
{
	for(int slot=0;slot<2;slot++)
	{
		for(int layer=0;layer<NumLayer;layer++)
		{delete[] *(*(pDMRS+slot)+layer);}
	}
	for(int slot=0;slot<2;slot++){delete[] *(pDMRS+slot);}
	delete[] pDMRS;

	for(int i=0;i<6;i++){delete[] *(pPrimeTable+i);}
	delete[] pPrimeTable;
}
