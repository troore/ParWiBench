
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "Turbo.h"
#include "Turbo_pardec.h"
#include "lte_phy.h"
#include "GeneralFunc.h"
#include "timer/meas.h"
#include "check/check.h"
#include "util.h"

#ifdef _RAPL
extern "C" {
#include "papi-rapl/rapl_power.h"
}
#endif


void test_turbo_encoding(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Turbo Encoder starts" << std::endl;
	
//	ReadInputFromFiles(lte_phy_params->te_in, lte_phy_params->te_in_buf_sz, "../TurboEncoderInput");
	GeneRandomInput(lte_phy_params->te_in, lte_phy_params->te_in_buf_sz, "../testsuite/TurboEncoderInput");

	/*
	for (int i = 0; i < lte_phy_params->te_in_buf_sz; i++)
		lte_phy_params->te_in[i] = 1;
	*/

	turbo_encoding(lte_phy_params, lte_phy_params->te_in, lte_phy_params->te_out);

	WriteOutputToFiles(lte_phy_params->te_out, lte_phy_params->te_out_buf_sz, "../testsuite/testTurboEncoderOutput");

	std::cout << "Turbo Encoder ends" << std::endl;
}

void test_turbo_decoding(LTE_PHY_PARAMS *lte_phy_params, int n_log_decoder_iters)
{
	std::cout << "Turbo Decoder starts" << std::endl;
	
	ReadInputFromFiles(lte_phy_params->td_in, lte_phy_params->td_in_buf_sz, "../testsuite/testTurboEncoderOutput");
	
	for (int i = 0; i < lte_phy_params->td_in_buf_sz; i++)
	{
		lte_phy_params->td_in[i] = (1 - 2 * lte_phy_params->td_in[i]);
	}

#ifdef _RAPL
	rapl_power_start();
#else
	double tstart, tend, ttime;
	double n_gflops, gflops;

	tstart = dtime();
#endif

	int n_test_iters = 1000;
	for (int i = 0; i < n_test_iters; i++)
	{
	turbo_decoding(lte_phy_params, lte_phy_params->td_in, lte_phy_params->td_out, n_log_decoder_iters);
	}

#ifndef _RAPL
	tend = dtime();
	ttime = tend - tstart;
	n_gflops = n_test_iters * gflop_counter_turbo_decoder(lte_phy_params, n_log_decoder_iters);
	gflops = (n_gflops * 10e3) / ttime;
	printf("%fms\n", ttime);
	printf("Number of gflops = %lf\n", n_gflops);
	printf("GFlops = %f\n", gflops);
#else
	rapl_power_stop();
#endif

	for (int i = 0; i < lte_phy_params->td_out_buf_sz; i++)
	{
		lte_phy_params->td_out[i] = (1 - lte_phy_params->td_out[i]) / 2;
	}
	
	WriteOutputToFiles(lte_phy_params->td_out, lte_phy_params->td_out_buf_sz, "../testsuite/testTurboDecoderOutput");
	
	std::cout << "Turbo Decoder ends" << std::endl;
}

//#define TurboEnc
void test(LTE_PHY_PARAMS *lte_phy_params)
{
	
//#ifdef TurboEnc

	test_turbo_encoding(lte_phy_params);
	
//#else

	int n_log_decoder_iters = 1;
	
	test_turbo_decoding(lte_phy_params, n_log_decoder_iters);

	char tx_in_fname[50];
	char rx_out_fname[50];

	strcpy(tx_in_fname, "../testsuite/TurboEncoderInput");
	strcpy(rx_out_fname, "../testsuite/testTurboDecoderOutput");
	
	check(tx_in_fname, rx_out_fname);
//#endif
}
