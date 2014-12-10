
#include "LTEUplink.h"


LTE_PHY_PARAMS lte_phy_params;

void RecvFromChannel(std::complex<float> *pRxRecv, int n)
{
	ReadInputFromFiles(pRxRecv, n, "TxImag.dat", "TxReal.dat");
}

void lte_phy_rx_sim(LTE_PHY_PARAMS *lte_phy_params)
{
	int SNRArrayLen = 1;
	int numSFrames = 1/*MAX_SFRAMES*/;
	int NumErrBits;
	
	float *AWGNSigmaArray = new float[SNRArrayLen];

	
	FILE *fptr = NULL;
	
	int* pTxDS=new int[lte_phy_params->data_len_per_subfr];
	int* pRxFS=new int[lte_phy_params->data_len_per_subfr];

	double start, end;
	double cost[10] = {0.0};
	
	for(int i = 0; i < SNRArrayLen; i++)
	{
		float x = 10.5 + 0.5 * ((float)i);
		
		AWGNSigmaArray[i] = sqrt((1.50 / lte_phy_params->N_bits_per_samp) * (pow(10.0, (-x / 10.0))));
		cout << AWGNSigmaArray[i] << endl;
	}

	for (int nsnr = 0; nsnr < SNRArrayLen; nsnr++)
	{
		for (int nsf = 0; nsf < numSFrames; nsf++)
		{
			////////////////////////// Run Subframe //////////////////////////////////
			RecvFromChannel(lte_phy_params->ofdemod_in, lte_phy_params->ofdemod_in_buf_sz);

			TIME_MEASURE_WRAPPER(ofdemodulating(lte_phy_params, lte_phy_params->ofdemod_in, lte_phy_params->ofdemod_out), 0);

			TIME_MEASURE_WRAPPER(SubCarrierDemapping(lte_phy_params, lte_phy_params->ofdemod_out, lte_phy_params->resdm_out), 1);

			Equalizer_init(lte_phy_params);
			TIME_MEASURE_WRAPPER(Equalizing(lte_phy_params, lte_phy_params->resdm_out, lte_phy_params->eq_out), 2);
			Equalizer_cleanup(lte_phy_params);

			TIME_MEASURE_WRAPPER(TransformDecoding(lte_phy_params, lte_phy_params->eq_out, lte_phy_params->trans_decoder_out), 3);

			TIME_MEASURE_WRAPPER(Demodulating(lte_phy_params, lte_phy_params->trans_decoder_out, lte_phy_params->demod_LLR, lte_phy_params->mod_type, AWGNSigmaArray[nsnr]), 4);

			for (int i = 0; i < lte_phy_params->demod_out_buf_sz; i++)
			{
				//	std::cout << lte_phy_params->descramb_out[i] << "\t";
				lte_phy_params->demod_LLR[i] *= (-1.0);
				//	std::cout << lte_phy_params->descramb_out[i] << std::endl;
			}

			TIME_MEASURE_WRAPPER(Descrambling(lte_phy_params, lte_phy_params->demod_LLR, lte_phy_params->descramb_out), 5);

			TIME_MEASURE_WRAPPER(RxRateMatching(lte_phy_params, lte_phy_params->descramb_out, lte_phy_params->rdm_out, lte_phy_params->rdm_hard), 6);

			TIME_MEASURE_WRAPPER(turbo_decoding(lte_phy_params, lte_phy_params->rdm_out, lte_phy_params->td_out), 7);

			for (int i = 0; i < lte_phy_params->td_out_buf_sz; i++)
				lte_phy_params->td_out[i] = (1 - lte_phy_params->td_out[i]) / 2;

			ReadLTEChainOutput(lte_phy_params->td_out, pRxFS, lte_phy_params->td_out_buf_sz);

			WriteOutputToFiles(pRxFS, lte_phy_params->data_len_per_subfr, "RxBitStream.dat");
			
			ReadInputFromFiles(pTxDS, lte_phy_params->data_len_per_subfr, "TxBitStream.dat");

			NumErrBits = 0;
			for (int i = 0; i < lte_phy_params->data_len_per_subfr; i++)
			{
				if (*(pTxDS + i) != *(pRxFS + i))
				{
					NumErrBits++;
					cout << i << endl;
				}
			}
			std:: cout << "Num of error bits=" << NumErrBits << std::endl;

			////////////////////////// END Run Subframe/////////////////////////////////
		}
	}

	for (int i = 0; i < 8; i++)
	{
		std::cout << (cost[i] * 1000) / CLOCKS_PER_SEC << "ms" << std::endl; 
	}


	delete[] AWGNSigmaArray;

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

	lte_phy_rx_sim(&lte_phy_params);

	return 0;
}
