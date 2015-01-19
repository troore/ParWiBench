
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lte_phy.h"
#include "RateMatcher.h"
#include "GeneralFunc.h"
#include "timer/meas.h"
#include "check/check.h"

void tx_rate_matching(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Tx RateMatching starts" << std::endl;

//	ReadInputFromFiles(lte_phy_params->rm_in, lte_phy_params->rm_in_buf_sz, "../testsuite/TxRateMatchInput");
	GeneRandomInput(lte_phy_params->rm_in, lte_phy_params->rm_in_buf_sz, "../testsuite/RandomTxRateMatchInput");
	TxRateMatching(lte_phy_params, lte_phy_params->rm_in, lte_phy_params->rm_out);
	WriteOutputToFiles(lte_phy_params->rm_out, lte_phy_params->rm_out_buf_sz, "../testsuite/testTxRateMatchOutput");
	
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

void check()
{
	char tx_in_fname[50];
	char rx_out_fname[50];
	int err_n;
		
	strcpy(tx_in_fname, "../testsuite/RandomTxRateMatchInput");
	strcpy(rx_out_fname, "../testsuite/testRxRateMatchOutput");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
}

//#define TxRateM
void test(LTE_PHY_PARAMS *lte_phy_params)
{
	
//	#ifdef TxRateM
	
	tx_rate_matching(lte_phy_params);
	
//	#else

	rx_rate_matching(lte_phy_params);
	check();
	
//	#endif
}
