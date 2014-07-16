
#include "ScrambMain.h"

int RANDOMSEED;

//#define Scramb

// Scrambler buffers
int tx_scramb_in[N_SCRAMB_IN_MAX];
int tx_scramb_out[N_SCRAMB_OUT_MAX];
float rx_scramb_in[N_SCRAMB_IN_MAX];
float rx_scramb_out[N_SCRAMB_OUT_MAX];

void test_scrambling(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Tx scrambling starts" << std::endl;

	int in_buf_sz;
	int out_buf_sz;
	
	in_buf_sz = lte_phy_params->N_tx_ant * lte_phy_params->N_dft_sz * (lte_phy_params->N_symb_per_subfr - 2) * lte_phy_params->N_bits_per_samp;
	out_buf_sz = in_buf_sz;
	
	ReadInputFromFiles(tx_scramb_in, in_buf_sz, "ScrambleInput");
	
	Scrambling(tx_scramb_in, in_buf_sz, tx_scramb_out, out_buf_sz);
	
	WriteOutputToFiles(tx_scramb_out, out_buf_sz, "testScrambleOutput");

	std::cout << "Tx scrambling ends" << std::endl;

}

void test_descrambling(LTE_PHY_PARAMS *lte_phy_params)
{
	
	std::cout << "Rx descrambling starts" << std::endl;

	int i;
	int in_buf_sz;
	int out_buf_sz;
	
	in_buf_sz = lte_phy_params->N_tx_ant * lte_phy_params->N_dft_sz * (lte_phy_params->N_symb_per_subfr - 2) * lte_phy_params->N_bits_per_samp;
	out_buf_sz = in_buf_sz;
	
	ReadInputFromFiles(rx_scramb_in, in_buf_sz, "DescrambleInput");
//	ReadInputFromFiles(rx_scramb_in, in_buf_sz, "testScrambleOutput");
	/*
	for (i = 0; i < in_buf_sz; i++)
	{
		if (rx_scramb_in[i] == 0)
			rx_scramb_in[i] = -1;
		else
			rx_scramb_in[i] = 1;
	 }
	*/
	
	Descrambling(rx_scramb_in, in_buf_sz, rx_scramb_out, out_buf_sz);

	/*
	for (i = 0; i < out_buf_sz; i++)
	{
		if (rx_scramb_out[i] > 0)
			rx_scramb_out[i] = 1;
		else
			rx_scramb_out[i] = 0;
	}
	*/

	WriteOutputToFiles(rx_scramb_out, out_buf_sz, "testDescrambleOutput");
	
	cout << "Rx descrambling ends" << endl;

}

int main(int argc, char *argv[])
{
	int enum_fs;
	int n_tx_ant, n_rx_ant;
	int mod_type;
	
	LTE_PHY_PARAMS lte_phy_params;

	if (argc != 5)
	{
		printf("Usage: ./a.out enum_fs mod_type n_tx_ant n_rx_ant\n");
		
		return 1;
	}
	
	enum_fs = atoi(argv[1]);
	mod_type = atoi(argv[2]);
	n_tx_ant = atoi(argv[3]);
	n_rx_ant = atoi(argv[4]);
	
	lte_phy_init(&lte_phy_params, enum_fs, mod_type, n_tx_ant, n_rx_ant);
	
#ifdef Scramb

	test_scrambling(&lte_phy_params);

	#else

	test_descrambling(&lte_phy_params);
	
	#endif
	
	return 0;
}



