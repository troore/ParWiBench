
#include "LTEUplink.h"

////////Global Variable//////////
//int RANDOMSEED;

LTE_PHY_PARAMS lte_phy_params;

////////End Global Variable//////////

void lte_phy_sim(LTE_PHY_PARAMS *lte_phy_params)
{
	int SNRArrayLen = 1;
	int numSFrames = 1;
	int NumErrBit;
	int RANDOMSEED;
	
	int* pTxDS = new int[lte_phy_params->data_len_per_subfr];
	int* pRxFS = new int[lte_phy_params->data_len_per_subfr];
	
	float *AWGNSigmaArray = new float[SNRArrayLen];

	int i;
	
	for (i = 0; i < SNRArrayLen; i++)
	{
		float x = 10.5 + 0.5 * ((float)i);
		
		AWGNSigmaArray[i] = sqrt((1.50 / lte_phy_params->N_bits_per_samp) * (pow(10.0, (-x / 10.0))));
		std::cout << AWGNSigmaArray[i] << std::endl;
	}

	for (int nsnr = 0; nsnr < SNRArrayLen; nsnr++)
	{
		for (int nsf = 0; nsf < numSFrames; nsf++)
		{
			////////////////////////// Run Subframe //////////////////////////////////

			
			RANDOMSEED = (nsf + 1) * (nsnr + 2);

			GenerateLTEChainInput(lte_phy_params->te_in, lte_phy_params->te_in_buf_sz, pTxDS, RANDOMSEED);

			turbo_encoding(lte_phy_params, lte_phy_params->te_in, lte_phy_params->te_out);

			/*
			for (i = 0; i < lte_phy_params->te_out_buf_sz; i++)
			{
				std::cout << lte_phy_params->te_out[i] << std::endl;
			}
			*/

			TxRateMatching(lte_phy_params, lte_phy_params->te_out, lte_phy_params->rm_out);

			/*
			for (i = 0; i < lte_phy_params->rm_out_buf_sz; i++)
			{
				std::cout << lte_phy_params->rm_out[i] << std::endl;
			}
			*/

			Scrambling(lte_phy_params, lte_phy_params->rm_out, lte_phy_params->scramb_out);

			/*
			for (i = 0; i < lte_phy_params->scramb_out_buf_sz; i++)
			{
				std::cout << lte_phy_params->scramb_out[i] << std::endl;
			}
			*/

			Modulating(lte_phy_params, lte_phy_params->scramb_out, lte_phy_params->mod_out, lte_phy_params->mod_type);

			TransformPrecoding(lte_phy_params, lte_phy_params->mod_out, lte_phy_params->trans_encoder_out);

			SubCarrierMapping(lte_phy_params, lte_phy_params->trans_encoder_out, lte_phy_params->resm_out);

			ofmodulating(lte_phy_params, lte_phy_params->resm_out, lte_phy_params->ofmod_out);

			/*
			for (i = 0; i < lte_phy_params->mod_out_buf_sz; i++)
			{
				std::cout << lte_phy_params->mod_out[i] << std::endl;
			}
			*/
	
			/** TODO: Adding channel **/
//			TxCRx.ApplyChannel(RxSCFD.pInpBuf,(AWGNSigmaArray[nsnr]));

			ofdemodulating(lte_phy_params, lte_phy_params->ofmod_out, lte_phy_params->ofdemod_out);
			
			SubCarrierDemapping(lte_phy_params, lte_phy_params->ofdemod_out, lte_phy_params->resdm_out);

			Equalizer_init(lte_phy_params);
			Equalizing(lte_phy_params, lte_phy_params->resdm_out, lte_phy_params->eq_out);
			Equalizer_cleanup(lte_phy_params);
			
			TransformDecoding(lte_phy_params, lte_phy_params->eq_out, lte_phy_params->trans_decoder_out);
		   
			Demodulating(lte_phy_params, lte_phy_params->trans_decoder_out, lte_phy_params->demod_LLR, lte_phy_params->mod_type, AWGNSigmaArray[nsnr]);

			for (i = 0; i < lte_phy_params->demod_out_buf_sz; i++)
			{
				//	std::cout << lte_phy_params->descramb_out[i] << "\t";
				lte_phy_params->demod_LLR[i] *= (-1.0);
				//	std::cout << lte_phy_params->descramb_out[i] << std::endl;
			}

			Descrambling(lte_phy_params, lte_phy_params->demod_LLR, lte_phy_params->descramb_out);
			
			RxRateMatching(lte_phy_params, lte_phy_params->descramb_out, lte_phy_params->rdm_out, lte_phy_params->rdm_hard);
				
			turbo_decoding(lte_phy_params, lte_phy_params->rdm_out, lte_phy_params->td_out);
				
			for (int i = 0; i < lte_phy_params->td_out_buf_sz; i++)
				lte_phy_params->td_out[i] = (1 - lte_phy_params->td_out[i]) / 2;
	
			ReadLTEChainOutput(lte_phy_params->td_out, pRxFS, lte_phy_params->td_out_buf_sz);

			NumErrBit = 0;
			for (int i = 0; i < lte_phy_params->data_len_per_subfr; i++)
			{
				if (*(pTxDS + i) != *(pRxFS + i))
				{
					NumErrBit++;
					cout << i << endl;
				}
			}
			std:: cout << "Num of error bits=" << NumErrBit << std::endl;

			////////////////////////// END Run Subframe/////////////////////////////////
		}
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

	lte_phy_sim(&lte_phy_params);

	return 0;
}
