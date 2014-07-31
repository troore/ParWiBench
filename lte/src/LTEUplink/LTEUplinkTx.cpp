
#include "LTEUplink.h"

#include <ctime>

LTE_PHY_PARAMS lte_phy_params;

void SendToChannel(std::complex<float> *pTxSend, int n)
{
	WriteOutputToFiles(pTxSend, n, "TxImag.dat", "TxReal.dat");
}

void lte_phy_tx_sim(LTE_PHY_PARAMS *lte_phy_params)
{
	//	int SNRArrayLen = 1;
	int numSFrames = 1/*MAX_SFRAMES*/;
	
	FILE *fptr = NULL;

	int* pTxDS = new int[lte_phy_params->data_len_per_subfr];
	int* pRxFS = new int[lte_phy_params->data_len_per_subfr];

	double start, end;
	double cost[10] = {0.0};
	
	for (int nsf = 0; nsf < numSFrames; nsf++)
	{
		////////////////////////// Run Subframe //////////////////////////////////
		int RANDOMSEED = (nsf + 1) * (/*nsnr*/1 + 2);

		//	GenerateLTEChainInput(TxTbE.pInpBuf,DataK,pTxDS);
		GenerateLTEChainInput(lte_phy_params->te_in, lte_phy_params->te_in_buf_sz, pTxDS, RANDOMSEED);

		WriteOutputToFiles(lte_phy_params->te_in, lte_phy_params->te_in_buf_sz, "TxBitStream.dat");

		/*
		  cout << "Turbo Tx Input" << endl;
		  for (int i = 0; i < DataK; i++)
		  cout << pTxTbInp[i] << "\t";
		  cout << endl;
		*/
			

		TIME_MEASURE_WRAPPER(turbo_encoding(lte_phy_params, lte_phy_params->te_in, lte_phy_params->te_out), 0);

		TIME_MEASURE_WRAPPER(TxRateMatching(lte_phy_params, lte_phy_params->te_out, lte_phy_params->rm_out), 1);

		TIME_MEASURE_WRAPPER(Scrambling(lte_phy_params, lte_phy_params->rm_out, lte_phy_params->scramb_out), 2);

		TIME_MEASURE_WRAPPER(Modulating(lte_phy_params, lte_phy_params->scramb_out, lte_phy_params->mod_out, lte_phy_params->mod_type), 3);

		TIME_MEASURE_WRAPPER(TransformPrecoding(lte_phy_params, lte_phy_params->mod_out, lte_phy_params->trans_encoder_out), 4);

		TIME_MEASURE_WRAPPER(SubCarrierMapping(lte_phy_params, lte_phy_params->trans_encoder_out, lte_phy_params->resm_out), 5);
			
		TIME_MEASURE_WRAPPER(ofmodulating(lte_phy_params, lte_phy_params->resm_out, lte_phy_params->ofmod_out), 6);
		
		// Send modulated data to air channel
		SendToChannel(lte_phy_params->ofmod_out, lte_phy_params->ofmod_out_buf_sz);


		////////////////////////// END Run Subframe/////////////////////////////////
	}

	for (int i = 0; i < 7; i++)
	{
		std::cout << (cost[i] * 1000) / CLOCKS_PER_SEC << "ms" << std::endl; 
	}
	
	delete[] pTxDS;
	delete[] pRxFS;
}

int main(int argc, char *argv[])
{
	int enum_fs;
	int n_tx_ant, n_rx_ant;
	int mod_type;
	
	if (argc != 5)
	{
		printf("Usage: ./a.out enum_fs mod_type n_tx_ant n_rx_ant\n");
		
		return -1;
	}
	
	enum_fs = atoi(argv[1]);
	mod_type = atoi(argv[2]);
	n_tx_ant = atoi(argv[3]);
	n_rx_ant = atoi(argv[4]);
	
	lte_phy_init(&lte_phy_params, enum_fs, mod_type, n_tx_ant, n_rx_ant);

	lte_phy_tx_sim(&lte_phy_params);

	return 0;
}
