
#include "ScrambMain.h"

int RANDOMSEED;

//#define Scramb

LTE_PHY_PARAMS lte_phy_params;

void test_scrambling(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Tx scrambling starts" << std::endl;

	ReadInputFromFiles(lte_phy_params->scramb_in, lte_phy_params->scramb_in_buf_sz, "testsuite/ScrambleInput");
	
	Scrambling(lte_phy_params, lte_phy_params->scramb_in, lte_phy_params->scramb_out);
	
	WriteOutputToFiles(lte_phy_params->scramb_out, lte_phy_params->scramb_out_buf_sz, "testsuite/testScrambleOutput");

	std::cout << "Tx scrambling ends" << std::endl;

}

void test_descrambling(LTE_PHY_PARAMS *lte_phy_params)
{
	
	std::cout << "Rx descrambling starts" << std::endl;

	int i;

	ReadInputFromFiles(lte_phy_params->descramb_in, lte_phy_params->descramb_in_buf_sz, "testsuite/DescrambleInput");
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
	
	Descrambling(lte_phy_params, lte_phy_params->descramb_in, lte_phy_params->descramb_out);

	/*
	for (i = 0; i < out_buf_sz; i++)
	{
		if (rx_scramb_out[i] > 0)
			rx_scramb_out[i] = 1;
		else
			rx_scramb_out[i] = 0;
	}
	*/

	WriteOutputToFiles(lte_phy_params->descramb_out, lte_phy_params->descramb_out_buf_sz, "testsuite/testDescrambleOutput");
	
	cout << "Rx descrambling ends" << endl;

}

int main(int argc, char *argv[])
{
	int enum_fs;
	int n_tx_ant, n_rx_ant;
	int mod_type;
	

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



