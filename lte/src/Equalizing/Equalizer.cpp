
#include "Equalizer.h"

//std::complex<float> ***VpCSI;
std::complex<float> ***pEqW;
std::complex<float> ***pHdm;

void Equalizer_init(LTE_PHY_PARAMS *lte_phy_params)
{
	int MDFTPerUser = lte_phy_params->N_dft_sz;
	int NumLayerPerUser = lte_phy_params->N_tx_ant;
	int NumRxAntenna = lte_phy_params->N_rx_ant;
	
	pEqW = new std::complex<float>**[MDFTPerUser];
	
	for(int m=0;m<MDFTPerUser;m++)
	{
		*(pEqW+m) = new std::complex<float>*[NumLayerPerUser];
		for(int l=0;l<NumLayerPerUser;l++)
		{
			*(*(pEqW+m)+l) = new std::complex<float>[NumRxAntenna];
		}
	}

	pHdm = new std::complex<float> **[MDFTPerUser];
	for(int m=0;m<MDFTPerUser;m++)
	{
		*(pHdm+m) = new std::complex<float> *[NumRxAntenna];
		for(int nrx=0;nrx<NumRxAntenna;nrx++)
		{
			*(*(pHdm+m)+nrx) = new std::complex<float>[NumLayerPerUser];
		}
	}
}

void FDLSEstimation(std::complex<float>** pXt, std::complex<float>** pXtDagger, std::complex<float>** pYt, std::complex<float>** pHTranspose, int NumLayer, int NumRxAntenna)
{
//////////////////// Freq Domain Estimate HTranspose ////////////////////
	std::complex<float>** pXDX=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pXDX+layer)=new std::complex<float>[NumLayer];}
	MatrixProd<int,std::complex<float> >(NumLayer,2,NumLayer,pXtDagger,pXt,pXDX);

	std::complex<float>** pInvXDX=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pInvXDX+layer)=new std::complex<float>[NumLayer];}
	MatrixInv<int,std::complex<float> >(NumLayer,pXDX,pInvXDX);

	std::complex<float>** pXDY=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pXDY+layer)=new std::complex<float>[NumRxAntenna];}
	MatrixProd<int,std::complex<float> >(NumLayer,2,NumRxAntenna,pXtDagger,pYt,pXDY);

	MatrixProd<int,std::complex<float> >(NumLayer,NumLayer,NumRxAntenna,pInvXDX,pXDY,pHTranspose);

	for(int layer=0;layer<NumLayer;layer++){delete[] *(pXDX+layer);delete[] *(pInvXDX+layer);delete[] *(pXDY+layer);}
	delete[] pXDX; delete[] pInvXDX; delete[] pXDY;  
//////////////////// END Freq Domain Estimate HTranspose ////////////////////
}


void FDLSEqualization(std::complex<float> *pInpData, std::complex<float>** pHTranspose, int m, int NumLayer, int NumRxAntenna, int MDFTPerUser, std::complex<float> *pOutData)
{
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
//////////////////// Freq Domain Equalize received Data /////////////////
	std::complex<float>** pH=new std::complex<float>*[NumRxAntenna];
	for(int nrx=0;nrx<NumRxAntenna;nrx++){*(pH+nrx)=new std::complex<float>[NumLayer];}
	std::complex<float>** pHDagger=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pHDagger+layer)=new std::complex<float>[NumRxAntenna];}
	std::complex<float>** pHDH=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pHDH+layer)=new std::complex<float>[NumLayer];}
	std::complex<float>** pInvHDH=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pInvHDH+layer)=new std::complex<float>[NumLayer];}

	std::complex<float>* pHDY=new std::complex<float>[NumLayer];

	for(int nrx=0;nrx<NumRxAntenna;nrx++)
	{
		for(int layer=0;layer<NumLayer;layer++)
		{
			*(*(pH+nrx)+layer)=*(*(pHTranspose+layer)+nrx);
			*(*(pHDagger+layer)+nrx)=conj((*(*(pHTranspose+layer)+nrx)));
		}
	}
	MatrixProd<int,std::complex<float> >(NumLayer,NumRxAntenna,NumLayer,pHDagger,pH,pHDH);
	MatrixInv<int,std::complex<float> >(NumLayer,pHDH,pInvHDH);

	////////////////// Equalizing Data /////////////////
	for(int nSymb=0;nSymb<NumULSymbSF-2;nSymb++)
	{
		std::complex<float>* pYData=new std::complex<float>[NumRxAntenna];
		for(int nrx=0;nrx<NumRxAntenna;nrx++)
		{
			int IDX=(NumULSymbSF-2)*nrx+nSymb+2*NumRxAntenna;
			//	*(pYData+nrx)=*(*(pInpData+IDX)+m);
			*(pYData + nrx) = pInpData[IDX * MDFTPerUser + m];
		}
		MatrixProd<int,std::complex<float> >(NumLayer,NumRxAntenna,1,pHDagger,pYData,pHDY);

		std::complex<float>* pXData=new std::complex<float>[NumLayer];
		MatrixProd<int,std::complex<float> >(NumLayer,NumLayer,1,pInvHDH,pHDY,pXData);

		/////////////////////// Get EqW ////////////////////////
		std::complex<float> **pW = new std::complex<float>*[NumLayer];
		for(int layer=0;layer<NumLayer;layer++){*(pW+layer)=new std::complex<float>[NumRxAntenna];}
		MatrixProd<int,std::complex<float> >(NumLayer,NumLayer,1,pInvHDH,pHDagger,pW);
		for(int layer=0;layer<NumLayer;layer++)
		{
			for(int nrx=0;nrx<NumRxAntenna;nrx++)
			{*(*(*(pEqW+m)+layer)+nrx)=*(*(pW+layer)+nrx);}
		}   
		for(int layer=0;layer<NumLayer;layer++){delete[] *(pW+layer);}
		delete[] pW;
		////////////////////////END Get EqW/////////////////////
		//////////////////////// Get pHdm ////////////////////////
		for(int nrx=0;nrx<NumRxAntenna;nrx++)
		{
			for(int layer=0;layer<NumLayer;layer++)
			{*(*(*(pHdm+m)+nrx)+layer)=*(*(pH+nrx)+layer);}
		}
		/////////////////////// END Get pHdm /////////////////////
		for(int layer=0;layer<NumLayer;layer++)
		{
			int IDX = (NumULSymbSF-2)*layer+nSymb;
			
			//	*(*(pOutData+IDX)+m)=*(pXData+layer);
			pOutData[IDX * MDFTPerUser + m] = *(pXData+layer);
		}

		delete[] pYData;
		delete[] pXData;
	}
	//////////////// END Equalizing Data ///////////////  

	for(int nrx=0;nrx<NumRxAntenna;nrx++){delete[] *(pH+nrx);}
	delete[] pH;
	for(int layer=0;layer<NumLayer;layer++)
	{delete[] *(pHDagger+layer);delete[] *(pHDH+layer);delete[] *(pInvHDH+layer);}
	delete[] pHDagger;
	delete[] pHDH;
	delete[] pInvHDH;

	delete[] pHDY;
//////////////////// END Freq Domain Equalize received Data//////////////////

}


/////////////////////////Frequency Domain MMSE Equalization///////////////////////////////////
////////////////////////////////////////////////////////////
void FDMMSEEqualization(std::complex<float> *pInpData, std::complex<float>** pHTranspose, int m, int NumLayer, int NumRxAntenna, int MDFTPerUser, float No, std::complex<float> *pOutData)
{
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
//////////////////// Freq Domain Equalize received Data /////////////////
	std::complex<float>** pH=new std::complex<float>*[NumRxAntenna];
	for(int nrx=0;nrx<NumRxAntenna;nrx++){*(pH+nrx)=new std::complex<float>[NumLayer];}
	std::complex<float>** pHDagger=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pHDagger+layer)=new std::complex<float>[NumRxAntenna];}
	std::complex<float>** pHHD=new std::complex<float>*[NumRxAntenna];
	for(int nrx=0;nrx<NumRxAntenna;nrx++){*(pHHD+nrx)=new std::complex<float>[NumRxAntenna];}
	std::complex<float>** pInvHHDN=new std::complex<float>*[NumRxAntenna];
	for(int nrx=0;nrx<NumRxAntenna;nrx++){*(pInvHHDN+nrx)=new std::complex<float>[NumRxAntenna];}

	std::complex<float>** pEo=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pEo+layer)=new std::complex<float>[NumRxAntenna];}

	for(int nrx=0;nrx<NumRxAntenna;nrx++)
	{
		for(int layer=0;layer<NumLayer;layer++)
		{
			*(*(pH+nrx)+layer)=*(*(pHTranspose+layer)+nrx);
			*(*(pHDagger+layer)+nrx)=conj((*(*(pHTranspose+layer)+nrx)));
		}
	}

	MatrixProd<int,std::complex<float> >(NumRxAntenna,NumLayer,NumRxAntenna,pH,pHDagger,pHHD);

	for(int nrx=0;nrx<NumRxAntenna;nrx++)
	{(*(*(pHHD+nrx)+nrx))+=No;}   

	MatrixInv<int,std::complex<float> >(NumRxAntenna,pHHD,pInvHHDN);
	MatrixProd<int,std::complex<float> >(NumLayer,NumRxAntenna,NumRxAntenna,pHDagger,pInvHHDN,pEo);
    /////////////////////// Get EqW ////////////////////////
    for(int layer=0;layer<NumLayer;layer++)
    {
		for(int nrx=0;nrx<NumRxAntenna;nrx++)
		{*(*(*(pEqW+m)+layer)+nrx)=*(*(pEo+layer)+nrx);}
    }   
    ////////////////////////END Get EqW/////////////////////
    //////////////////////// Get pHdm ////////////////////////
    for(int nrx=0;nrx<NumRxAntenna;nrx++)
    {
		for(int layer=0;layer<NumLayer;layer++)
		{*(*(*(pHdm+m)+nrx)+layer)=*(*(pH+nrx)+layer);}
    }
    /////////////////////// END Get pHdm /////////////////////

	////////////////// Equalizing Data /////////////////
	for(int nSymb=0;nSymb<NumULSymbSF-2;nSymb++)
	{
		std::complex<float>* pYData=new std::complex<float>[NumRxAntenna];
		for(int nrx=0;nrx<NumRxAntenna;nrx++)
		{
			int IDX=(NumULSymbSF-2)*nrx+nSymb+2*NumRxAntenna;
			//	*(pYData+nrx)=*(*(pInpData+IDX)+m);
			*(pYData + nrx) = pInpData[IDX * MDFTPerUser + m];
		}

		std::complex<float>* pXData=new std::complex<float>[NumLayer];
		MatrixProd<int,std::complex<float> >(NumLayer,NumRxAntenna,1,pEo,pYData,pXData);


		for(int layer=0;layer<NumLayer;layer++)
		{
			int IDX = (NumULSymbSF-2)*layer+nSymb;
			//	*(*(pOutData+IDX)+m)=*(pXData+layer);
			pOutData[IDX * MDFTPerUser + m] = *(pXData + layer);
		}

		delete[] pYData;
		delete[] pXData;
	}
	//////////////// END Equalizing Data ///////////////  

	for(int nrx=0;nrx<NumRxAntenna;nrx++){delete[] *(pH+nrx); delete[] *(pHHD+nrx); delete[] *(pInvHHDN+nrx);}
	delete[] pH; delete[] pHHD; delete[] pInvHHDN;
	for(int layer=0;layer<NumLayer;layer++)
	{delete[] *(pHDagger+layer);delete[] *(pEo+layer);}
	delete[] pHDagger;delete[] pEo;
//////////////////// END Freq Domain Equalize received Data//////////////////

}

////////////////////////////////////////////////////////////
/////////////////////////END Frequency Domain MMSE Equalization///////////////////////////////////

void LSFreqDomain(std::complex<float> *pInpData, std::complex<float> *pOutData, int MDFT, int NumLayer, int NumRxAntenna)
{
	std::complex<float> pDMRS[2 * LTE_PHY_N_ANT_MAX * LTE_PHY_DFT_SIZE_MAX];
//	pDMRS = (*VpUser).GetpDMRS();
	geneDMRS(pDMRS, NumLayer, MDFT);
  
	for(int m=0;m<MDFT;m++)
	{
		std::complex<float>** pXt=new std::complex<float>*[2];
		for(int slot=0;slot<2;slot++){*(pXt+slot)=new std::complex<float>[NumLayer];}
		std::complex<float>** pXtDagger = new std::complex<float>* [NumLayer];
		for(int layer=0;layer<NumLayer;layer++){*(pXtDagger+layer)=new std::complex<float>[2];}
		for(int slot=0;slot<2;slot++)
		{
			for(int layer=0;layer<NumLayer;layer++)
			{
				//	*(*(pXt+slot)+layer)=*(*(*(pDMRS+slot)+layer)+m);
				*(*(pXt+slot)+layer)= pDMRS[(slot * NumLayer + layer) * MDFT + m];
				//	*(*(pXtDagger+layer)+slot)=conj((*(*(*(pDMRS+slot)+layer)+m)));
				*(*(pXtDagger+layer)+slot)=conj(pDMRS[(slot * NumLayer + layer) * MDFT + m]);
			}
		}

		std::complex<float>** pYt=new std::complex<float>*[2];
		for(int slot=0;slot<2;slot++){*(pYt+slot)=new std::complex<float>[NumRxAntenna];}
		for(int slot=0;slot<2;slot++)
		{
			for(int nrx=0;nrx<NumRxAntenna;nrx++)
			{
				*(*(pYt+slot)+nrx) = pInpData[(nrx * 2 + slot) * MDFT + m];
			}
		}

		std::complex<float>** pHTranspose=new std::complex<float>*[NumLayer];
		for(int layer=0;layer<NumLayer;layer++){*(pHTranspose+layer)=new std::complex<float>[NumRxAntenna];}

		FDLSEstimation(pXt, pXtDagger, pYt, pHTranspose, NumLayer, NumRxAntenna);

		FDLSEqualization(pInpData, pHTranspose, m, NumLayer, NumRxAntenna, MDFT, pOutData); 

		for(int slot=0;slot<2;slot++){delete[] *(pXt+slot);}
		delete[] pXt;
		for(int layer=0;layer<NumLayer;layer++){delete[] *(pXtDagger+layer);}
		delete[] pXtDagger;

		for(int slot=0;slot<2;slot++){delete[] *(pYt+slot);}
		delete[] pYt; 

		for(int layer=0;layer<NumLayer;layer++){delete[]  *(pHTranspose+layer);}
		delete[] pHTranspose;
	}
}

void Equalizing(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pInpData, std::complex<float> *pOutData)
{
	int MDFT = lte_phy_params->N_dft_sz;
	int NumLayer = lte_phy_params->N_tx_ant;
	int NumRxAntenna = lte_phy_params->N_rx_ant;
	
	LSFreqDomain(pInpData, pOutData, MDFT, NumLayer, NumRxAntenna);
}

/*
  void Equalizer::Equalizing(FIFO<complex<float> >* pOutBuf,complex<float>***pPCSI,float AWGNNo)
  {
  if(PSFlag)
  {cout<<"Equalizing"<<endl;}
//VpInpBuf=pInpBuf;
VpOutBuf=pOutBuf;
VpCSI=pPCSI;
float No=AWGNNo;

int MDFT=MDFTPerUser;
int NumLayer=NumLayerPerUser;

int SCLoc = (*VpUser).SCLoc;

bool ReadFlag = (*pInpBuf).Read(pInpData);

if(ReadFlag)
{

for(int m=0;m<MDFT;m++)
{
////////////////////////// HTranspose////////////////////////
complex<float>** pHTranspose=new complex<float>*[NumLayer];
for(int layer=0;layer<NumLayer;layer++){*(pHTranspose+layer)=new complex<float>[NumRxAntenna];}
   
for(int layer=0;layer<NumLayer;layer++)
{
for(int nrx=0;nrx<NumRxAntenna;nrx++)
{*(*(pHTranspose+layer)+nrx)=*(*(*(VpCSI+layer)+nrx)+(m+SCLoc));}
}
//////////////////////////END HTranspose/////////////////////

//////////////////// Freq Domain Equalize received Data /////////////////
FDMMSEEqualization(pHTranspose,m,NumLayer,No);
//////////////////// END Freq Domain Equalize received Data//////////////////


/////////////////////////////////Delete///////////////////////////////////////    
////////////////////////// HTranspose////////////////////////
for(int layer=0;layer<NumLayer;layer++){delete[]  *(pHTranspose+layer);}
delete[] pHTranspose;
//////////////////////////END HTranspose/////////////////////   
}
bool WriteFlag=(*VpOutBuf).Write(pOutData);
//   if(WriteFlag){cout<<"successfully written!"<<endl;}
if(PSFlag)
{cout<<"Equalization completed"<<endl;}
}
else
{cout<<"fail to read data from previous buffer"<<endl;}

}
*/

void Equalizer_cleanup(LTE_PHY_PARAMS *lte_phy_params)
{
	int MDFTPerUser = lte_phy_params->N_dft_sz;
	int NumLayerPerUser = lte_phy_params->N_tx_ant;
	int NumRxAntenna = lte_phy_params->N_rx_ant;

	for(int m=0;m<MDFTPerUser;m++)
	{
		for(int l=0;l<NumLayerPerUser;l++)
		{
			delete[] *(*(pEqW+m)+l);
		}

		for(int nrx=0;nrx<NumRxAntenna;nrx++)
		{
			delete[] *(*(pHdm+m)+nrx);
		}
	}

	for(int m=0;m<MDFTPerUser;m++)
	{
		delete[] *(pEqW+m);
		delete[] *(pHdm+m);
	}

	delete[] pEqW;
	delete[] pHdm;
}
