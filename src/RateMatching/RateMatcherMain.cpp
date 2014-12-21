
#include "lte_phy.h"
#include "RateMatcher.h"
#include "GeneralFunc.h"
#include "gauss.h"
#include "meas.h"

//#define TxRateM

LTE_PHY_PARAMS lte_phy_params;

void tx_rate_matching(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Tx RateMatching starts" << std::endl;

	ReadInputFromFiles(lte_phy_params->rm_in, lte_phy_params->rm_in_buf_sz, "../TxRateMatchInput");

//for(int i=0;i<100000;i++)
	TxRateMatching(lte_phy_params, lte_phy_params->rm_in, lte_phy_params->rm_out);
	
	WriteOutputToFiles(lte_phy_params->rm_out, lte_phy_params->rm_out_buf_sz, "../testTxRateMatchOutput");

	std::cout << "Tx RateMatching ends" << std::endl;
}

void rx_rate_matching(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Rx RateMatching starts" << std::endl;

	ReadInputFromFiles(lte_phy_params->rdm_in, lte_phy_params->rdm_in_buf_sz, "../testTxRateMatchOutput");
//	ReadInputFromFiles(rx_rm_in, in_buf_sz, "RxRateMatchInput");

	RxRateMatching(lte_phy_params, lte_phy_params->rdm_in, lte_phy_params->rdm_out, lte_phy_params->rdm_hard);

	int test_out_buffer[N_RM_OUT_MAX];
	for (int i = 0; i < lte_phy_params->rdm_out_buf_sz; i++)
	{
		if (lte_phy_params->rdm_out[i] == 0.0)
			test_out_buffer[i] = 0;
		else
			test_out_buffer[i] = 1;
	}
//	WriteOutputToFiles(lte_phy_params->rdm_out, lte_phy_params->rdm_out_buf_sz, "testRxRateMatchOutput");
	WriteOutputToFiles(test_out_buffer, lte_phy_params->rdm_out_buf_sz, "../testRxRateMatchOutput");

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
	double ttime,tbegin;
	tbegin = dtime();
	#ifdef TxRateM
	
	tx_rate_matching(&lte_phy_params);
	
	#else

	rx_rate_matching(&lte_phy_params);
	
	#endif
	ttime = dtime();
	printf("whole time is %f\n",ttime - tbegin);
	return 0;
}


