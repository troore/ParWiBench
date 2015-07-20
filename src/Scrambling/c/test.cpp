
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "GeneralFunc.h"
#include "Scrambler.h"
#include "timer/meas.h"
#include "check/check.h"
#include "test.h"
#include "util.h"

#ifdef _RAPL
extern "C" {
#include "papi-rapl/rapl_power.h"
}
#endif


void test_scrambling(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Tx scrambling starts" << std::endl;

//	ReadInputFromFiles(lte_phy_params->scramb_in, lte_phy_params->scramb_in_buf_sz, "../testsuite/ScrambleInput");
	GeneRandomInput(lte_phy_params->scramb_in, lte_phy_params->scramb_in_buf_sz, "../testsuite/RandomScrambleInput");

	GenScrambInt(lte_phy_params->scramb_seq_int, lte_phy_params->scramb_in_buf_sz);
	
//	double tstart, tend, ttime;

//	tstart = dtime();

//	for (int i = 0; i < 1; i++) {
	Scrambling(lte_phy_params, lte_phy_params->scramb_in, lte_phy_params->scramb_out);
//	}

//	tend = dtime();
//	ttime = tend - tstart;
//	printf("%fms\n", ttime);
	
	WriteOutputToFiles(lte_phy_params->scramb_out, lte_phy_params->scramb_out_buf_sz, "../testsuite/testScrambleOutput");

	std::cout << "Tx scrambling ends" << std::endl;
}

void test_descrambling(LTE_PHY_PARAMS *lte_phy_params)
{
	
	std::cout << "Rx descrambling starts" << std::endl;

	float *rx_scramb_in = (float *)malloc(lte_phy_params->descramb_in_buf_sz * sizeof(float));
	float *rx_scramb_out = (float *)malloc(lte_phy_params->descramb_out_buf_sz * sizeof(float));
	int i;

	ReadInputFromFiles(lte_phy_params->descramb_in, lte_phy_params->descramb_in_buf_sz, "../testsuite/testScrambleOutput");
//	ReadInputFromFiles(rx_scramb_in, in_buf_sz, "testScrambleOutput");

	GenScrambInt(lte_phy_params->scramb_seq_int, lte_phy_params->descramb_in_buf_sz);
	
	for (i = 0; i < lte_phy_params->descramb_in_buf_sz; i++)
	{
		if (lte_phy_params->descramb_in[i] == 0)
			rx_scramb_in[i] = -1.0;
		else
			rx_scramb_in[i] = 1.0;
	}

#ifdef _RAPL
	rapl_power_start();
#else
	
	double tstart, tend, ttime;
	double n_gflops, gflops;

	tstart = dtime();
#endif

	int n_iters = 10000;
	for (i = 0; i < n_iters; i++) {
		Descrambling(lte_phy_params, rx_scramb_in, rx_scramb_out);
	}
	
#ifndef _RAPL
	tend = dtime();
	ttime = tend - tstart;
	n_gflops = n_iters * gflop_counter(lte_phy_params);
	gflops = (n_gflops * 10e3) / ttime;
	printf("%fms\n", ttime);
	printf("Number of gflops = %lf\n", n_gflops);
	printf("GFlops = %lf\n", gflops);
#else
	rapl_power_stop();
#endif

	for (i = 0; i < lte_phy_params->descramb_out_buf_sz; i++)
	{
		if (rx_scramb_out[i] > 0)
			lte_phy_params->descramb_out[i] = 1.0;
		else
			lte_phy_params->descramb_out[i] = 0.0;
	}

	WriteOutputToFiles(lte_phy_params->descramb_out, lte_phy_params->descramb_out_buf_sz, "../testsuite/testDescrambleOutput");
	
	cout << "Rx descrambling ends" << endl;
}

//#define Scramb
void test(LTE_PHY_PARAMS *lte_phy_params)
{
//#ifdef Scramb
	test_scrambling(lte_phy_params);
//#else
	test_descrambling(lte_phy_params);

	char tx_in_fname[50];
	char rx_out_fname[50];

	strcpy(tx_in_fname, "../testsuite/RandomScrambleInput");
	strcpy(rx_out_fname, "../testsuite/testDescrambleOutput");
	check(tx_in_fname, rx_out_fname);
//#endif
}
