
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lte_phy.h"
#include "RateMatcher.h"
#include "GeneralFunc.h"
#include "timer/meas.h"
#include "check/check.h"
#include "micpower.h"
#include "util.h"

void tx_rate_matching(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Tx RateMatching starts" << std::endl;

	//ReadInputFromFiles(lte_phy_params->rm_in, lte_phy_params->rm_in_buf_sz, "../testsuite/RandomTxRateMatchInput");
	GeneRandomInput(lte_phy_params->rm_in, lte_phy_params->rm_in_buf_sz, "/root/RateMatching/testsuite/RandomTxRateMatchInput");
	TxRateMatching(lte_phy_params, lte_phy_params->rm_in, lte_phy_params->rm_out);
	double energy,ttime,tbegin;
	micpower_start();
	tbegin = dtime();
	for(int i=0;i<10000;i++)
		TxRateMatching(lte_phy_params, lte_phy_params->rm_in, lte_phy_params->rm_out);
	ttime = dtime() - tbegin;
	energy = micpower_finalize();
//	printf("Energy used in %lf\n", energy);
//	printf("whole time is %fms\n", ttime);
	printf("%lf\t%f\t%f\n", energy, ttime, (energy * 1000.0) / ttime);
	
	WriteOutputToFiles(lte_phy_params->rm_out, lte_phy_params->rm_out_buf_sz, "/root/RateMatching/testsuite/testTxRateMatchOutput");

	std::cout << "Tx RateMatching ends" << std::endl;
}

void rx_rate_matching(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Rx RateMatching starts" << std::endl;

	ReadInputFromFiles(lte_phy_params->rdm_in, lte_phy_params->rdm_in_buf_sz, "/root/RateMatching/testsuite/testTxRateMatchOutput");
//	ReadInputFromFiles(rx_rm_in, in_buf_sz, "RxRateMatchInput");

	RxRateMatching(lte_phy_params, lte_phy_params->rdm_in, lte_phy_params->rdm_out, lte_phy_params->rdm_hard);


//	WriteOutputToFiles(lte_phy_params->rdm_out, lte_phy_params->rdm_out_buf_sz, "testRxRateMatchOutput");
	WriteOutputToFiles(lte_phy_params->rdm_out, lte_phy_params->rdm_out_buf_sz, "/root/RateMatching/testsuite/testRxRateMatchOutput");

	std::cout << "Rx RateMatching ends" << std::endl;
}

//#define TxRateM
void test(LTE_PHY_PARAMS *lte_phy_params)
{
	
//	#ifdef TxRateM
	
	tx_rate_matching(lte_phy_params);
	
//	#else

	rx_rate_matching(lte_phy_params);

	char tx_in_fname[100];
	char rx_out_fname[100];

	strcpy(tx_in_fname, "/root/RateMatching/testsuite/RandomTxRateMatchInput");
	strcpy(rx_out_fname, "/root/RateMatching/testsuite/testRxRateMatchOutput");
	check(tx_in_fname, rx_out_fname);
	
//	#endif
}

