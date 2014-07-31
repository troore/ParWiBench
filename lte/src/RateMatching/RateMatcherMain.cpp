
#include "RateMatcherMain.h"

// #define TxRateM

LTE_PHY_PARAMS lte_phy_params;

void tx_rate_matching(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Tx RateMatching starts" << std::endl;

	ReadInputFromFiles(lte_phy_params->rm_in, lte_phy_params->rm_in_buf_sz, "TxRateMatchInput");

	TxRateMatching(lte_phy_params, lte_phy_params->rm_in, lte_phy_params->rm_out);
	
	WriteOutputToFiles(lte_phy_params->rm_out, lte_phy_params->rm_out_buf_sz, "testTxRateMatchOutput");

	std::cout << "Tx RateMatching ends" << std::endl;
}

void rx_rate_matching(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Rx RateMatching starts" << std::endl;

	ReadInputFromFiles(lte_phy_params->rdm_in, lte_phy_params->rdm_in_buf_sz, "testTxRateMatchOutput");
//	ReadInputFromFiles(rx_rm_in, in_buf_sz, "RxRateMatchInput");

	RxRateMatching(lte_phy_params, lte_phy_params->rdm_in, lte_phy_params->rdm_out, lte_phy_params->rdm_hard);

	/*
	int test_out_buffer[N_RM_IN_MAX];

	for (int i = 0; i < out_buf_sz; i++)
		test_out_buffer[i] = (int)rx_rm_out[i];
	*/

	WriteOutputToFiles(lte_phy_params->rdm_out, lte_phy_params->rdm_out_buf_sz, "testRxRateMatchOutput");
//	WriteOutputToFiles(test_out_buffer, out_buf_sz, "testRxRateMatchOutput");

	std::cout << "Rx RateMatching ends" << std::endl;
}

int main(int argc, char *argv[])
{
	int enum_fs;
	int n_tx_ant, n_rx_ant;
	int mod_type;
	
//	LTE_PHY_PARAMS lte_phy_params;

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

	#ifdef TxRateM
	
	tx_rate_matching(&lte_phy_params);
	
	#else

	rx_rate_matching(&lte_phy_params);
	
	#endif
	
	return 0;
}


