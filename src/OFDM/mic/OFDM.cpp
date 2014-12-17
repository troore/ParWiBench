#include "OFDM.h"
#include "meas.h"

void ofmodulating(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, float *pOutData)
{
    int NumLayer = lte_phy_params->N_tx_ant;
    int NIFFT = lte_phy_params->N_fft_sz;
    int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
    int CPLen = lte_phy_params->N_samps_cp_l_0;
    
    int in_buf_sz = lte_phy_params->ofmod_in_buf_sz;
    int out_buf_sz = lte_phy_params->ofmod_out_buf_sz;
    
    int nlayer, nsym, i;
    
    float *p_samp_in_buf = (float *)malloc(2 * NIFFT * sizeof(float));
    float *p_samp_out_buf = (float *)malloc(2 * NIFFT * sizeof(float));
    
    for (nlayer = 0; nlayer < NumLayer; nlayer++)
    {
        for (nsym = 0; nsym < NumULSymbSF; nsym++)
        {
            int symb_idx = nlayer * NumULSymbSF + nsym;
            float norm = (float)sqrt((float)NIFFT)/* (float)NIFFT*/;
            
            for (i = 0; i < NIFFT; i++)
            {
                p_samp_in_buf[i] = pInpData[symb_idx * NIFFT + i];
                p_samp_in_buf[i + NIFFT] = pInpData[symb_idx * NIFFT + i + in_buf_sz];
            }
            
            fft_iter(NIFFT, p_samp_in_buf, p_samp_out_buf, 1);
            
            for (i = 0; i < NIFFT; i++)
            {
                pOutData[symb_idx * (NIFFT + CPLen) + CPLen + i] = p_samp_out_buf[i] / norm;
                pOutData[symb_idx * (NIFFT + CPLen) + CPLen + i + out_buf_sz] = p_samp_out_buf[i + NIFFT] / norm;
            }
            
            for (i = 0; i < CPLen; i++)
            {
                pOutData[symb_idx * (NIFFT + CPLen) + i] = pOutData[symb_idx * (NIFFT + CPLen) + NIFFT + i];
                pOutData[symb_idx * (NIFFT + CPLen) + i + out_buf_sz] = pOutData[symb_idx * (NIFFT + CPLen) + NIFFT + i + out_buf_sz];
            }
        }
    }
    
    free(p_samp_in_buf);
    free(p_samp_out_buf);
}

void ofdemodulating(LTE_PHY_PARAMS *lte_phy_params, float *pInpData, float *pOutData)
{
	int NumRxAntenna = lte_phy_params->N_rx_ant;
	int NIFFT = lte_phy_params->N_fft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	int CPLen = lte_phy_params->N_samps_cp_l_0;

	int in_buf_sz = lte_phy_params->ofdemod_in_buf_sz;
	int out_buf_sz = lte_phy_params->ofdemod_out_buf_sz;

	int nrx, nsym, i;

	float *p_samp_in_buf = (float *)malloc(2 * NIFFT * sizeof(float));
	float *p_samp_out_buf = (float *)malloc(2 * NIFFT * sizeof(float));


//	for (nrx=0;nrx<NumRxAntenna;nrx++)
//	{
//		for (nsym=0;nsym<NumULSymbSF;nsym++)
//		{

	int num_threads=236,sum=0,quotient=0,remain=0;
	int ii,j,k;
	sum = NumRxAntenna*NumULSymbSF;
	quotient = sum / num_threads;
	remain = sum % num_threads;
	
#pragma omp parallel for private(j,k)
	for(i=0;i<num_threads;i++)
	{
		int i_begin,num_j;
		if(i>=remain)
		{
			i_begin = remain  + i * quotient;
			num_j = quotient;
		}
		else 
		{ 
			i_begin = i * (quotient + 1);
			num_j = quotient + 1;
		} 
		for(j=0;j<num_j;j++)
		{
			nrx = (i_begin + j) / NumULSymbSF;
			nsym = (i_begin + j) % NumULSymbSF;
			int symb_idx = nrx * NumULSymbSF + nsym;
			float norm = (float)sqrt((float)NIFFT)/*(float)1*/;

			for (ii = 0; ii < NIFFT; ii++)
			{
				p_samp_in_buf[ii] = pInpData[symb_idx * (CPLen + NIFFT) + CPLen + ii];
				p_samp_in_buf[ii + NIFFT] = pInpData[symb_idx * (CPLen + NIFFT) + ii + CPLen + in_buf_sz];
			}
			
			//	fft_nrvs(NIFFT, pInpData + (symb_idx * (CPLen + NIFFT) + CPLen) * 2, pOutData + symb_idx * NIFFT * 2, 1);
			fft_nrvs(NIFFT, p_samp_in_buf, p_samp_out_buf, -1);

			for(ii = 0; ii < NIFFT; ii++)
			{
				pOutData[symb_idx * NIFFT + ii] = p_samp_out_buf[ii] / norm;
				pOutData[symb_idx * NIFFT + ii + out_buf_sz] = p_samp_out_buf[ii + NIFFT] / norm;
			}
		}
	}

	free(p_samp_in_buf);
	free(p_samp_out_buf);
}

/*
void ofdemodulating(LTE_PHY_PARAMS *lte_phy_params, float *pInpDataReal, float *pInpDataImag,
					float *pOutDataReal, float *pOutDataImag)
{
	int NumRxAntenna = lte_phy_params->N_rx_ant;
	int NIFFT = lte_phy_params->N_fft_sz;
	int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
	int CPLen = lte_phy_params->N_samps_cp_l_0;

	int nrx, nsym, i;

//	for (nrx=0;nrx<NumRxAntenna;nrx++)
//	{
//		for (nsym=0;nsym<NumULSymbSF;nsym++)
//		{

	int num_threads=236,sum=0,quotient=0,remain=0;
	int ii,j,k;
	sum = NumRxAntenna*NumULSymbSF;
	quotient = sum / num_threads;
	remain = sum % num_threads;
	
#pragma omp parallel for private(j,k)
	for(i=0;i<num_threads;i++)
	{
		int i_begin,num_j;
		if(i>=remain)
		{
			i_begin = remain  + i * quotient;
			num_j = quotient;
		}
		else 
		{ 
			i_begin = i * (quotient + 1);
			num_j = quotient + 1;
		} 
		for(j=0;j<num_j;j++)
		{
			nrx = (i_begin + j) / NumULSymbSF;
			nsym = (i_begin + j) % NumULSymbSF;
			int symb_idx = nrx * NumULSymbSF + nsym;
			float norm = (float)sqrt((float)NIFFT)/*(float)1*/;
/*			
			fft_nrvs(NIFFT, pInpDataReal + symb_idx * (CPLen + NIFFT) + CPLen, pInpDataImag + symb_idx * (CPLen + NIFFT) + CPLen,
					 pOutDataReal + symb_idx * NIFFT, pOutDataReal + symb_idx * NIFFT,
					 -1);

			for(ii = 0; ii < NIFFT; ii++)
			{
				pOutDataReal[symb_idx * NIFFT + ii] /= norm;
				pOutDataImag[symb_idx * NIFFT + ii] /= norm;
			}
		}
	}
}
*/
