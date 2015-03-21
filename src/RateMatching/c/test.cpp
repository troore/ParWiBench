
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lte_phy.h"
#include "RateMatcher.h"
#include "GeneralFunc.h"
#include "timer/meas.h"
#include "check/check.h"
#include "util.h"

#ifdef _RAPL
extern "C" {
#include "papi-rapl/rapl_power.h"
}
#endif

void tx_rate_matching(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Tx RateMatching starts" << std::endl;

//	ReadInputFromFiles(lte_phy_params->rm_in, lte_phy_params->rm_in_buf_sz, "../testsuite/TxRateMatchInput");
	GeneRandomInput(lte_phy_params->rm_in, lte_phy_params->rm_in_buf_sz, "../testsuite/RandomTxRateMatchInput");

#ifdef _RAPL
	rapl_power_start();
#else

	double tstart, tend, ttime;
	double n_gflops, gflops;

	tstart = dtime();
#endif

	int n_iters = 1;
	for (int i = 0; i < n_iters; i++) {
		TxRateMatching(lte_phy_params, lte_phy_params->rm_in, lte_phy_params->rm_out);
	}

#ifndef _RAPL
	tend = dtime();
	ttime = tend - tstart;
	n_gflops = n_iters * gflop_counter(lte_phy_params);
	gflops = (n_gflops * 10e3) / ttime;
	printf("%fms\n", ttime);
	printf("Number of gflops = %lf\n", n_gflops);
	printf("GFlops = %f\n", gflops);
#else
	rapl_power_stop();
#endif

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

//#define TxRateM
void test(LTE_PHY_PARAMS *lte_phy_params)
{
	
//	#ifdef TxRateM
	
	tx_rate_matching(lte_phy_params);
	
//	#else

	rx_rate_matching(lte_phy_params);
	
	char tx_in_fname[100];
	char rx_out_fname[100];

	strcpy(tx_in_fname, "../testsuite/RandomTxRateMatchInput");
	strcpy(rx_out_fname, "../testsuite/testRxRateMatchOutput");
	check(tx_in_fname, rx_out_fname);
	
//	#endif
}
