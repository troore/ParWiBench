
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lte_phy.h"
#include "RateMatcher.h"
#include "GeneralFunc.h"
#include "meas.h"
#include "check.h"
#ifdef __MIC__
#include "micpower.h"
#endif
//#define TxRateM

LTE_PHY_PARAMS lte_phy_params;

void tx_rate_matching(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Tx RateMatching starts" << std::endl;

#ifdef __MIC__
	
	//ReadInputFromFiles(lte_phy_params->rm_in, lte_phy_params->rm_in_buf_sz, "../testsuite/RandomTxRateMatchInput");
	GeneRandomInput(lte_phy_params->rm_in, lte_phy_params->rm_in_buf_sz, "../testsuite/RandomTxRateMatchInput");
	TxRateMatching(lte_phy_params, lte_phy_params->rm_in, lte_phy_params->rm_out);
	double energy,ttime,tbegin;
	micpower_start();
	tbegin = dtime();
	for(int i=0;i<10000;i++)
		TxRateMatching(lte_phy_params, lte_phy_params->rm_in, lte_phy_params->rm_out);
	ttime = dtime() - tbegin;
	energy = micpower_finalize();
	printf("Energy used in %lf\n", energy);
	printf("whole time is %fms\n", ttime);
	WriteOutputToFiles(lte_phy_params->rm_out, lte_phy_params->rm_out_buf_sz, "../testsuite/testTxRateMatchOutput");
#else
//	ReadInputFromFiles(lte_phy_params->rm_in, lte_phy_params->rm_in_buf_sz, "../testsuite/TxRateMatchInput");
	GeneRandomInput(lte_phy_params->rm_in, lte_phy_params->rm_in_buf_sz, "../testsuite/RandomTxRateMatchInput");
	TxRateMatching(lte_phy_params, lte_phy_params->rm_in, lte_phy_params->rm_out);
	WriteOutputToFiles(lte_phy_params->rm_out, lte_phy_params->rm_out_buf_sz, "../testsuite/testTxRateMatchOutput");
#endif
	std::cout << "Tx RateMatching ends" << std::endl;
}

void rx_rate_matching(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Rx RateMatching starts" << std::endl;

	ReadInputFromFiles(lte_phy_params->rdm_in, lte_phy_params->rdm_in_buf_sz, "../testsuite/testTxRateMatchOutput");
//	ReadInputFromFiles(rx_rm_in, in_buf_sz, "RxRateMatchInput");

	RxRateMatching(lte_phy_params, lte_phy_params->rdm_in, lte_phy_params->rdm_out, lte_phy_params->rdm_hard);

	/*
	int test_out_buffer[N_RM_OUT_MAX];
	for (int i = 0; i < lte_phy_params->rdm_out_buf_sz; i++)
	{
		if (lte_phy_params->rdm_out[i] == 0.0)
			test_out_buffer[i] = 0;
		else
			test_out_buffer[i] = 1;
	}
	*/
//	WriteOutputToFiles(lte_phy_params->rdm_out, lte_phy_params->rdm_out_buf_sz, "testRxRateMatchOutput");
	WriteOutputToFiles(/*test_out_buffer*/lte_phy_params->rdm_out, lte_phy_params->rdm_out_buf_sz, "../testsuite/testRxRateMatchOutput");

	std::cout << "Rx RateMatching ends" << std::endl;
}

int main(int argc, char *argv[])
{
	int fs_id;
	int n_tx_ant, n_rx_ant;
	int mod_type;

	char tx_in_fname[50];
	char rx_out_fname[50];
	int err_n;
	
	if (argc != 5 && argc != 2)
	{
		lte_phy_usage_info(argv[0]);
		
		return 1;
	}

	if (argc == 5)
	{
		fs_id = atoi(argv[1]);
		mod_type = atoi(argv[2]);
		n_tx_ant = atoi(argv[3]);
		n_rx_ant = atoi(argv[4]);

		lte_phy_init(&lte_phy_params, fs_id, mod_type, n_tx_ant, n_rx_ant);
	}
	else
	{
		fs_id = atoi(argv[1]);

		lte_phy_init(&lte_phy_params, fs_id);
	}

	#ifdef TxRateM
	
	tx_rate_matching(&lte_phy_params);
	
	#else

	rx_rate_matching(&lte_phy_params);
	
	strcpy(tx_in_fname, "../testsuite/RandomTxRateMatchInput");
	strcpy(rx_out_fname, "../testsuite/testRxRateMatchOutput");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
	
	#endif
	
	return 0;
}


