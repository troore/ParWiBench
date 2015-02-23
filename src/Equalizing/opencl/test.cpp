
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "lte_phy.h"
#include "GeneralFunc.h"
#include "Equalizer.h"
#include "ResMapper.h"
#include "OFDM.h"
#include "refs/dmrs.h"
#include "timer/meas.h"
#include "check/check.h"
#include "util.h"

void test_equalizer(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Equalizing starts" << std::endl;

	double tstart, tend, ttime;
	double n_gflops, gflops;

	tstart = dtime();

//	GeneRandomInput(lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
//	ReadInputFromFiles(lte_phy_params->eq_in, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
//	ReadInputFromFiles(lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
//	GeneRandomInput(lte_phy_params->eq_in, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");

	GeneRandomInput(lte_phy_params->resm_in, lte_phy_params->resm_in_buf_sz, "../testsuite/SubCarrierMapInputReal", "../testsuite/SubCarrierMapInputImag");

	// for SubCarrierMapping
	geneDMRS(lte_phy_params->DMRS, lte_phy_params->N_tx_ant, lte_phy_params->N_dft_sz);
	// for Equalizing
	geneDMRS(lte_phy_params->DMRSReal, lte_phy_params->DMRSImag, lte_phy_params->N_tx_ant, lte_phy_params->N_dft_sz);
	SubCarrierMapping(lte_phy_params, lte_phy_params->resm_in, lte_phy_params->resm_out);
	ofmodulating(lte_phy_params, lte_phy_params->resm_out, lte_phy_params->ofmod_out);
	ofdemodulating(lte_phy_params, lte_phy_params->ofmod_out, lte_phy_params->ofdemod_out);
	SubCarrierDemapping(lte_phy_params, lte_phy_params->ofdemod_out, lte_phy_params->resdm_out);

	for (int i = 0; i < lte_phy_params->resdm_out_buf_sz; i++)
	{
		lte_phy_params->resdm_out_real[i] = lte_phy_params->resdm_out[i];
		lte_phy_params->resdm_out_imag[i] = lte_phy_params->resdm_out[i + lte_phy_params->resdm_out_buf_sz];
	}

	int n_iters = 1;
	for (int i = 0; i < n_iters; i++) {
		//	Equalizing(lte_phy_params, lte_phy_params->resdm_out, lte_phy_params->eq_out);
		Equalizing(lte_phy_params, lte_phy_params->resdm_out_real, lte_phy_params->resdm_out_imag, lte_phy_params->eq_out_real, lte_phy_params->eq_out_imag);
	}
//	geneDMRS(lte_phy_params->DMRSReal, lte_phy_params->DMRSImag, lte_phy_params->N_tx_ant, lte_phy_params->N_dft_sz);
//	Equalizing(lte_phy_params, lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_out_real, lte_phy_params->eq_out_imag);

	tend = dtime();
	ttime = tend - tstart;
	n_gflops = n_iters * gflop_counter(lte_phy_params);
	gflops = (n_gflops * 10e3) / ttime;
//	printf("%fms\n", ttime);
//	printf("Number of gflops = %lf\n", n_gflops);
//	printf("GFlops = %f\n", gflops);

//	WriteOutputToFiles(lte_phy_params->eq_out, lte_phy_params->eq_out_buf_sz, "../testsuite/testLSCELSEqOutputReal", "../testsuite/testLSCELSEqOutputImag");
	WriteOutputToFiles(lte_phy_params->eq_out_real, lte_phy_params->eq_out_imag, lte_phy_params->eq_out_buf_sz, "../testsuite/testLSCELSEqOutputReal", "../testsuite/testLSCELSEqOutputImag");

	std::cout << "Equalizing ends" << std::endl;
}


void test(LTE_PHY_PARAMS *lte_phy_params)
{
	test_equalizer(lte_phy_params);
	check();
}
