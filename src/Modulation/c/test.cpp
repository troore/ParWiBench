
#include <complex>
#include <iostream>
#include <string.h>

#include "GeneralFunc.h"
#include "timer/meas.h"
#include "Modulation.h"
#include "util.h"

#ifdef _RAPL
extern "C" {
#include "papi-rapl/rapl_power.h"
}
#endif

static void test_mod(LTE_PHY_PARAMS *lte_phy_params, int mod_type)
{
	std::cout << "Modulation starts" << std::endl;

//	ReadInputFromFiles(lte_phy_params->mod_in, lte_phy_params->mod_in_buf_sz, "ModulationInput");
	GeneRandomInput(lte_phy_params->mod_in, lte_phy_params->mod_in_buf_sz, "../testsuite/RandomModulationInput");
	
//	Modulating_cplx(lte_phy_params, lte_phy_params->mod_in, lte_phy_params->mod_out_cplx, mod_type);
	Modulating(lte_phy_params, lte_phy_params->mod_in, lte_phy_params->mod_out, mod_type);
	
	WriteOutputToFiles(lte_phy_params->mod_out, lte_phy_params->mod_out_buf_sz, "../testsuite/testModulationRandomOutputReal", "../testsuite/testModulationRandomOutputImag");

	std::cout << "Modulation ends" << std::endl;
}

static void test_demod(LTE_PHY_PARAMS *lte_phy_params, int mod_type)
{
		
	std::cout << "Demodulation starts" << std::endl;

	float awgn_sigma = 0.193649; //this value is for the standard input  see "AWGNSigma"

	ReadInputFromFiles(lte_phy_params->demod_in, lte_phy_params->demod_in_buf_sz, "../testsuite/testModulationRandomOutputReal", "../testsuite/testModulationRandomOutputImag");

#ifdef _RAPL
	rapl_power_start();
#else

	double tstart, tend, ttime;
	double n_gflops, gflops;

	tstart = dtime();
#endif

	int n_iters = 1000;
	for (int i = 0; i < n_iters; i++) {
		Demodulating(lte_phy_params, lte_phy_params->demod_in, lte_phy_params->demod_LLR, mod_type, awgn_sigma);
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

	for (int i = 0; i < lte_phy_params->demod_out_buf_sz; i++)
	{
		if (lte_phy_params->demod_LLR[i] > 0)
			lte_phy_params->demod_HD[i] = 1;
		else
			lte_phy_params->demod_HD[i] = 0;
	}
	
//	WriteOutputToFiles(lte_phy_params->demod_LLR, lte_phy_params->demod_out_buf_sz, "../testsuite/testDemodulationOutput");
	WriteOutputToFiles(lte_phy_params->demod_HD, lte_phy_params->demod_out_buf_sz, "../testsuite/testDemodulationOutput");

	std::cout << "Demodulation ends" << std::endl;
}


//#define Mod
void test(LTE_PHY_PARAMS *lte_phy_params)
{
//#ifdef Mod

//	test_mod(lte_phy_params, lte_phy_params->mod_type);
	test_mod(lte_phy_params, 2);

//#else

//	test_demod(lte_phy_params, lte_phy_params->mod_type);
	test_demod(lte_phy_params, 2);

	char tx_in_fname[100];
	char rx_out_fname[100];
		
	strcpy(tx_in_fname, "../testsuite/RandomModulationInput");
	strcpy(rx_out_fname, "../testsuite/testDemodulationOutput");
	check(tx_in_fname, rx_out_fname);

//#endif
}
