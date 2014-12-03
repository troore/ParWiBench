
#include "Equalizer.h"

#include <stdio.h>

//std::complex<float> ***VpCSI;
#ifdef DEBUG_EQ
std::complex<float> ***pEqW;
std::complex<float> ***pHdm;
#else
std::complex<float> pEqW[LTE_PHY_DFT_SIZE_MAX * LTE_PHY_N_ANT_MAX * LTE_PHY_N_ANT_MAX];
std::complex<float> pHdm[LTE_PHY_DFT_SIZE_MAX * LTE_PHY_N_ANT_MAX * LTE_PHY_N_ANT_MAX];
#endif

#ifdef DEBUG_EQ
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
#endif

#ifdef DEBUG_EQ
void FDLSEstimation(std::complex<float>** pXt, std::complex<float>** pXtDagger, std::complex<float>** pYt, std::complex<float>** pHTranspose, int NumLayer, int NumRxAntenna)
#else
void FDLSEstimation(std::complex<float> *pXt, std::complex<float> *pXtDagger, std::complex<float> *pYt, std::complex<float> *pHTranspose, int NumLayer, int NumRxAntenna)
#endif
{
#ifdef DEBUG_EQ
	std::complex<float>** pXDX=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pXDX+layer)=new std::complex<float>[NumLayer];}
#else
	std::complex<float> pXDX[LTE_PHY_N_ANT_MAX * LTE_PHY_N_ANT_MAX];
#endif

#ifdef DEBUG_EQ
	MatrixProd<int,std::complex<float> >(NumLayer, 2, NumLayer, pXtDagger, pXt, pXDX);
#else
	MatrixProd(NumLayer, 2, NumLayer, pXtDagger, pXt, pXDX);
#endif

#ifdef DEBUG_EQ
	std::complex<float>** pInvXDX=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pInvXDX+layer)=new std::complex<float>[NumLayer];}
#else
	std::complex<float> pInvXDX[LTE_PHY_N_ANT_MAX * LTE_PHY_N_ANT_MAX];
#endif

#ifdef DEBUG_EQ
	MatrixInv<int,std::complex<float> >(NumLayer,pXDX,pInvXDX);
#else
	MatrixInv(NumLayer, pXDX, pInvXDX);
#endif

#ifdef DEBUG_EQ
	std::complex<float>** pXDY=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pXDY+layer)=new std::complex<float>[NumRxAntenna];}
#else
	std::complex<float> pXDY[LTE_PHY_N_ANT_MAX * LTE_PHY_N_ANT_MAX];
#endif

#ifdef DEBUG_EQ
	MatrixProd<int,std::complex<float> >(NumLayer,2,NumRxAntenna,pXtDagger,pYt,pXDY);

	MatrixProd<int,std::complex<float> >(NumLayer,NumLayer,NumRxAntenna,pInvXDX,pXDY,pHTranspose);
#else
	MatrixProd(NumLayer, 2, NumRxAntenna, pXtDagger, pYt, pXDY);

	MatrixProd(NumLayer, NumLayer, NumRxAntenna, pInvXDX, pXDY, pHTranspose);
#endif

#ifdef DEBUG_EQ
	for(int layer=0;layer<NumLayer;layer++){delete[] *(pXDX+layer);delete[] *(pInvXDX+layer);delete[] *(pXDY+layer);}
	delete[] pXDX; delete[] pInvXDX; delete[] pXDY;  
#endif
}


#ifdef DEBUG_EQ
void FDLSEqualization(std::complex<float> *pInpData, std::complex<float>** pHTranspose, int m, int NumLayer, int NumRxAntenna, int MDFTPerUser, std::complex<float> *pOutData)
#else
void FDLSEqualization(std::complex<float> *pInpData, std::complex<float> *pHTranspose, int m, int NumLayer, int NumRxAntenna, int MDFTPerUser, std::complex<float> *pOutData)
#endif
{
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
//////////////////// Freq Domain Equalize received Data /////////////////
#ifdef DEBUG_EQ
	std::complex<float>** pH=new std::complex<float>*[NumRxAntenna];
	for(int nrx=0;nrx<NumRxAntenna;nrx++){*(pH+nrx)=new std::complex<float>[NumLayer];}
	std::complex<float>** pHDagger=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pHDagger+layer)=new std::complex<float>[NumRxAntenna];}
	std::complex<float>** pHDH=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pHDH+layer)=new std::complex<float>[NumLayer];}
	std::complex<float>** pInvHDH=new std::complex<float>*[NumLayer];
	for(int layer=0;layer<NumLayer;layer++){*(pInvHDH+layer)=new std::complex<float>[NumLayer];}
#else
	std::complex<float> pH[LTE_PHY_N_ANT_MAX * LTE_PHY_N_ANT_MAX];
	std::complex<float> pHDagger[LTE_PHY_N_ANT_MAX * LTE_PHY_N_ANT_MAX];
	std::complex<float> pHDH[LTE_PHY_N_ANT_MAX * LTE_PHY_N_ANT_MAX];
	std::complex<float> pInvHDH[LTE_PHY_N_ANT_MAX * LTE_PHY_N_ANT_MAX];
#endif

#ifdef DEBUG_EQ
	std::complex<float>* pHDY=new std::complex<float>[NumLayer];
#else
	std::complex<float> pHDY[LTE_PHY_N_ANT_MAX];
#endif

	for (int nrx = 0; nrx < NumRxAntenna; nrx++)
	{
		for (int layer = 0; layer < NumLayer; layer++)
		{
#ifdef DEBUG_EQ
			*(*(pH+nrx)+layer)=*(*(pHTranspose+layer)+nrx);
			*(*(pHDagger+layer)+nrx)=conj((*(*(pHTranspose+layer)+nrx)));
#else
			pH[nrx * NumLayer + layer] = pHTranspose[layer * NumRxAntenna + nrx];
			pHDagger[layer * NumRxAntenna + nrx] = conj(pHTranspose[layer * NumRxAntenna + nrx]);
#endif
		}
	}
#ifdef DEBUG_EQ
	MatrixProd<int,std::complex<float> >(NumLayer,NumRxAntenna,NumLayer,pHDagger,pH,pHDH);
	MatrixInv<int,std::complex<float> >(NumLayer,pHDH,pInvHDH);
#else
	MatrixProd(NumLayer, NumRxAntenna, NumLayer, pHDagger, pH, pHDH);
	MatrixInv(NumLayer, pHDH, pInvHDH);
#endif

	////////////////// Equalizing Data /////////////////
	for(int nSymb=0;nSymb<NumULSymbSF-2;nSymb++)
	{
#ifdef DEBUG_EQ
		std::complex<float>* pYData=new std::complex<float>[NumRxAntenna];
#else
		std::complex<float> pYData[LTE_PHY_N_ANT_MAX];
#endif
		for(int nrx=0;nrx<NumRxAntenna;nrx++)
		{
			int IDX=(NumULSymbSF-2)*nrx+nSymb+2*NumRxAntenna;
			//	*(pYData+nrx)=*(*(pInpData+IDX)+m);
			pYData[nrx] = pInpData[IDX * MDFTPerUser + m];
		}
#ifdef DEBUG_EQ
		MatrixProd<int,std::complex<float> >(NumLayer,NumRxAntenna,1,pHDagger,pYData,pHDY);
	//	for (int i = 0; i < 2; i++)
	//		for (int j = 0; j < 2; j++)
	//		printf("%f %f\n", std::real(pHDagger[i][j]), std::imag(pHDagger[i][j]));
#else
		MatrixProd(NumLayer, NumRxAntenna, 1, pHDagger, pYData, pHDY);
	//	for (int j = 0; j < 4; j++)
	//		printf("%f %f\n", std::real(pHDagger[j]), std::imag(pHDagger[j]));
#endif

#ifdef DEBUG_EQ
		std::complex<float>* pXData=new std::complex<float>[NumLayer];
		MatrixProd<int,std::complex<float> >(NumLayer,NumLayer,1,pInvHDH,pHDY,pXData);
#else
		std::complex<float> pXData[LTE_PHY_N_ANT_MAX];
		MatrixProd(NumLayer, NumLayer, 1, pInvHDH, pHDY, pXData);
#endif

		/*
		/////////////////////// Get EqW ////////////////////////
#ifdef DEBUG_EQ
		std::complex<float> **pW = new std::complex<float>*[NumLayer];
		for(int layer=0;layer<NumLayer;layer++){*(pW+layer)=new std::complex<float>[NumRxAntenna];}
	//	MatrixProd<int,std::complex<float> >(NumLayer,NumLayer,1,pInvHDH,pHDagger,pW);
		MatrixProd<int,std::complex<float> >(NumLayer,NumLayer,NumLayer,pInvHDH,pHDagger,pW);
#else
		std::complex<float> pW[LTE_PHY_N_ANT_MAX * LTE_PHY_N_ANT_MAX];
		MatrixProd(NumLayer, NumLayer, NumLayer, pInvHDH, pHDagger, pW);
#endif
		for (int layer = 0; layer < NumLayer; layer++)
		{
			for (int nrx = 0; nrx < NumRxAntenna; nrx++)
			{
#ifdef DEBUG_EQ
				*(*(*(pEqW+m)+layer)+nrx)=*(*(pW+layer)+nrx);
#else
				pEqW[m * NumLayer * NumRxAntenna + layer * NumRxAntenna + nrx] = pW[layer * NumRxAntenna + nrx];
#endif
			}
		}   
#ifdef DEBUG_EQ
		for(int layer=0;layer<NumLayer;layer++){delete[] *(pW+layer);}
		delete[] pW;
#endif
		////////////////////////END Get EqW/////////////////////
		//////////////////////// Get pHdm ////////////////////////
		for(int nrx=0;nrx<NumRxAntenna;nrx++)
		{
			for(int layer=0;layer<NumLayer;layer++)
			{*(*(*(pHdm+m)+nrx)+layer)=*(*(pH+nrx)+layer);}
		}
		/////////////////////// END Get pHdm /////////////////////
		*/
		
		for (int layer = 0; layer < NumLayer; layer++)
		{
			int IDX = (NumULSymbSF-2)*layer+nSymb;
			
			//	*(*(pOutData+IDX)+m)=*(pXData+layer);
			pOutData[IDX * MDFTPerUser + m] = pXData[layer];
		}

#ifdef DEBUG_EQ
		delete[] pYData;
		delete[] pXData;
#endif
	}
	//////////////// END Equalizing Data ///////////////  

#ifdef DEBUG_EQ
	for(int nrx=0;nrx<NumRxAntenna;nrx++){delete[] *(pH+nrx);}
	delete[] pH;
	for(int layer=0;layer<NumLayer;layer++)
	{delete[] *(pHDagger+layer);delete[] *(pHDH+layer);delete[] *(pInvHDH+layer);}
	delete[] pHDagger;
	delete[] pHDH;
	delete[] pInvHDH;

	delete[] pHDY;
#endif
//////////////////// END Freq Domain Equalize received Data//////////////////

}

/*
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
*/

void LSFreqDomain(std::complex<float> *pInpData, std::complex<float> *pOutData, int MDFT, int NumLayer, int NumRxAntenna)
{
	std::complex<float> pDMRS[2 * LTE_PHY_N_ANT_MAX * LTE_PHY_DFT_SIZE_MAX];
//	pDMRS = (*VpUser).GetpDMRS();
	geneDMRS(pDMRS, NumLayer, MDFT);
  
	for(int m=0;m<MDFT;m++)
	{
#ifdef DEBUG_EQ
		std::complex<float>** pXt=new std::complex<float>*[2];
		for(int slot=0;slot<2;slot++){*(pXt+slot)=new std::complex<float>[NumLayer];}
		std::complex<float>** pXtDagger = new std::complex<float>* [NumLayer];
		for(int layer=0;layer<NumLayer;layer++){*(pXtDagger+layer)=new std::complex<float>[2];}
#else
		std::complex<float> pXt[2 * LTE_PHY_N_ANT_MAX];
		std::complex<float> pXtDagger[LTE_PHY_N_ANT_MAX * 2];
#endif
		for(int slot=0;slot<2;slot++)
		{
			for(int layer=0;layer<NumLayer;layer++)
			{
#ifdef DEBUG_EQ
				//	*(*(pXt+slot)+layer)=*(*(*(pDMRS+slot)+layer)+m);
				*(*(pXt+slot)+layer)= pDMRS[(slot * NumLayer + layer) * MDFT + m];
				//	*(*(pXtDagger+layer)+slot)=conj((*(*(*(pDMRS+slot)+layer)+m)));
				*(*(pXtDagger+layer)+slot)=conj(pDMRS[(slot * NumLayer + layer) * MDFT + m]);
#else
				pXt[slot * NumLayer + layer] = pDMRS[(slot * NumLayer + layer) * MDFT + m];
				pXtDagger[layer * 2 + slot] = conj(pDMRS[(slot * NumLayer + layer) * MDFT + m]);
#endif
			}
		}

#ifdef DEBUG_EQ
		std::complex<float>** pYt=new std::complex<float>*[2];
		for(int slot=0;slot<2;slot++){*(pYt+slot)=new std::complex<float>[NumRxAntenna];}
#else
		std::complex<float> pYt[2 * LTE_PHY_N_ANT_MAX];
#endif
		for(int slot=0;slot<2;slot++)
		{
			for(int nrx=0;nrx<NumRxAntenna;nrx++)
			{
#ifdef DEBUG_EQ
				*(*(pYt+slot)+nrx) = pInpData[(nrx * 2 + slot) * MDFT + m];
#else
				pYt[slot * NumRxAntenna + nrx] = pInpData[(nrx * 2 + slot) * MDFT + m];
#endif
			}
		}

#ifdef DEBUG_EQ
		std::complex<float>** pHTranspose=new std::complex<float>*[NumLayer];
		for(int layer=0;layer<NumLayer;layer++){*(pHTranspose+layer)=new std::complex<float>[NumRxAntenna];}
#else
		std::complex<float> pHTranspose[LTE_PHY_N_ANT_MAX * LTE_PHY_N_ANT_MAX];
#endif
		FDLSEstimation(pXt, pXtDagger, pYt, pHTranspose, NumLayer, NumRxAntenna);

		FDLSEqualization(pInpData, pHTranspose, m, NumLayer, NumRxAntenna, MDFT, pOutData); 

#ifdef DEBUG_EQ
		for(int slot=0;slot<2;slot++){delete[] *(pXt+slot);}
		delete[] pXt;
		for(int layer=0;layer<NumLayer;layer++){delete[] *(pXtDagger+layer);}
		delete[] pXtDagger;

		for(int slot=0;slot<2;slot++){delete[] *(pYt+slot);}
		delete[] pYt; 

		for(int layer=0;layer<NumLayer;layer++){delete[]  *(pHTranspose+layer);}
		delete[] pHTranspose;
#endif
	}
}

void Equalizing(LTE_PHY_PARAMS *lte_phy_params, std::complex<float> *pInpData, std::complex<float> *pOutData)
{
	int MDFT = lte_phy_params->N_dft_sz;
	int NumLayer = lte_phy_params->N_tx_ant;
	int NumRxAntenna = lte_phy_params->N_rx_ant;
	
	LSFreqDomain(pInpData, pOutData, MDFT, NumLayer, NumRxAntenna);
}

#ifdef DEBUG_EQ
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
#endif
