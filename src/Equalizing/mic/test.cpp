
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "lte_phy.h"
#include "GeneralFunc.h"
#include "Equalizer.h"
#include "ResMapper.h"
#include "OFDM.h"
#include "refs/dmrs.h"
#include "timer/meas.h"
#include "check/check.h"
#include "micpower.h"
#include "util.h"

void test_equalizer(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Equalizing starts" << std::endl;


//	GeneRandomInput(lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
//	ReadInputFromFiles(lte_phy_params->eq_in, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
//	ReadInputFromFiles(lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
//	GeneRandomInput(lte_phy_params->eq_in, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");

	GeneRandomInput(lte_phy_params->resm_in, lte_phy_params->resm_in_buf_sz, "/root/Equalizing/testsuite/SubCarrierMapInputReal", "/root/Equalizing/testsuite/SubCarrierMapInputImag");
	
	geneDMRS(lte_phy_params->DMRS, lte_phy_params->N_tx_ant, lte_phy_params->N_dft_sz);
	SubCarrierMapping(lte_phy_params, lte_phy_params->resm_in, lte_phy_params->resm_out);
	ofmodulating(lte_phy_params, lte_phy_params->resm_out, lte_phy_params->ofmod_out);
	ofdemodulating(lte_phy_params, lte_phy_params->ofmod_out, lte_phy_params->ofdemod_out);
	SubCarrierDemapping(lte_phy_params, lte_phy_params->ofdemod_out, lte_phy_params->resdm_out);

	double energy, ttime, tbegin;
	micpower_start();
	tbegin = dtime();
	
	int n_iters = 10000;
	for (int i = 0; i < n_iters; i++) {
		Equalizing(lte_phy_params, lte_phy_params->resdm_out, lte_phy_params->eq_out);
	}
//	geneDMRS(lte_phy_params->DMRSReal, lte_phy_params->DMRSImag, lte_phy_params->N_tx_ant, lte_phy_params->N_dft_sz);
//	Equalizing(lte_phy_params, lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_out_real, lte_phy_params->eq_out_imag);
	ttime = dtime() - tbegin;
	energy = micpower_finalize();
//	printf("Energy used is %lfJ\n", energy);
//	printf("whole time is %fms\n", ttime);
//	printf("Power is %fW\n", (energy * 1000.0) / ttime);

	printf("%lf\t%f\t%f\n", energy, ttime, (energy * 1000.0) / ttime);

	WriteOutputToFiles(lte_phy_params->eq_out, lte_phy_params->eq_out_buf_sz, "/root/Equalizing/testsuite/testLSCELSEqOutputReal", "/root/Equalizing/testsuite/testLSCELSEqOutputImag");
//	WriteOutputToFiles(lte_phy_params->eq_out_real, lte_phy_params->eq_out_imag, lte_phy_params->eq_out_buf_sz, "../testsuite/testLSCELSEqOutputReal", "../testsuite/testLSCELSEqOutputImag");

	std::cout << "Equalizing ends" << std::endl;
}


void test(LTE_PHY_PARAMS *lte_phy_params)
{
	test_equalizer(lte_phy_params);

	char tx_in_fname_real[100];
	char tx_in_fname_imag[100];
	char rx_out_fname_real[100];
	char rx_out_fname_imag[100];
	
	strcpy(tx_in_fname_real, "/root/Equalizing/testsuite/SubCarrierMapInputReal");
	strcpy(tx_in_fname_imag, "/root/Equalizing/testsuite/SubCarrierMapInputImag");
	strcpy(rx_out_fname_real, "/root/Equalizing/testsuite/testLSCELSEqOutputReal");
	strcpy(rx_out_fname_imag, "/root/Equalizing/testsuite/testLSCELSEqOutputImag");
	check(tx_in_fname_real, tx_in_fname_imag, rx_out_fname_real, rx_out_fname_imag);
}
