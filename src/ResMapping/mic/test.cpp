#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "GeneralFunc.h"
#include "timer/meas.h"
#include "check/check.h"
#include "Equalizer.h"
#include "ResMapper.h"
#include "OFDM.h"
#include "refs/dmrs.h"
#include "util.h"
#include "micpower.h"


void test_SCMapper(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Resource mapping starts" << std::endl;

//	ReadInputFromFiles(lte_phy_params->resm_in, lte_phy_params->resm_in_buf_sz, "../testsuite/SubCarrierMapInputReal", "../testsuite/SubCarrierMapInputImag");
//	ReadInputFromFiles(lte_phy_params->resm_in_real, lte_phy_params->resm_in_imag, lte_phy_params->resm_in_buf_sz, "../testsuite/SubCarrierMapInputReal", "../testsuite/SubCarrierMapInputImag");
//	GeneRandomInput(lte_phy_params->resm_in_real, lte_phy_params->resm_in_imag, lte_phy_params->resm_in_buf_sz, "../testsuite/SubCarrierMapInputReal", "../testsuite/SubCarrierMapInputImag");
//	GeneRandomInput(lte_phy_params->resm_in, lte_phy_params->resm_in_buf_sz, "../testsuite/SubCarrierMapInputReal", "../testsuite/SubCarrierMapInputImag");
	GeneRandomInput(lte_phy_params->resm_in, lte_phy_params->resm_in_buf_sz, "/root/ResMapping/testsuite/SubCarrierMapInputReal", "/root/ResMapping/testsuite/SubCarrierMapInputImag");

//	SubCarrierMapping(lte_phy_params, lte_phy_params->resm_in_real, lte_phy_params->resm_in_imag, lte_phy_params->resm_out_real, lte_phy_params->resm_out_imag);

//	SubCarrierMapping(lte_phy_params, lte_phy_params->resm_in, lte_phy_params->resm_out);
	double energy, ttime, tbegin;
	micpower_start();
	tbegin = dtime();
	for (int i = 0; i < 10000; i++)
		SubCarrierMapping(lte_phy_params, lte_phy_params->resm_in, lte_phy_params->resm_out);
	ttime = dtime() - tbegin;
	energy = micpower_finalize();
//	printf("Energy used is %lfJ\n", energy);
//	printf("whole time is %fms\n", ttime);
//	printf("Power is %fW\n", (energy * 1000.0) / ttime);

	printf("%lf\t%f\t%f\n", energy, ttime, (energy * 1000.0) / ttime);

	ofmodulating(lte_phy_params, lte_phy_params->resm_out, lte_phy_params->ofmod_out);

//	WriteOutputToFiles(lte_phy_params->ofmod_out, lte_phy_params->ofmod_out_buf_sz, "../testsuite/testModulationOutputReal", "../testsuite/testModulationOutputImag");
	WriteOutputToFiles(lte_phy_params->ofmod_out, lte_phy_params->ofmod_out_buf_sz, "/root/ResMapping/testsuite/testModulationOutputReal", "/root/ResMapping/testsuite/testModulationOutputImag");
	
//	WriteOutputToFiles(lte_phy_params->resm_out, lte_phy_params->resm_out_buf_sz, "../testsuite/testSubCarrierMapOutputReal", "../testsuite/testSubCarrierMapOutputImag");
//	WriteOutputToFiles(lte_phy_params->resm_out_real, lte_phy_params->resm_out_imag, lte_phy_params->resm_out_buf_sz, "../testsuite/testSubCarrierMapOutputReal", "../testsuite/testSubCarrierMapOutputImag");
	
	std::cout << "Resource mapping ends" << std::endl;
}

void test_SCDemapper(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Resource demapping starts" << std::endl;
	
	int in_buf_sz, out_buf_sz;

	double tstart, tstop, ttime;

	in_buf_sz = lte_phy_params->N_tx_ant * lte_phy_params->N_fft_sz * lte_phy_params->N_symb_per_subfr;

//	ReadInputFromFiles(lte_phy_params->ofdemod_in, lte_phy_params->ofdemod_in_buf_sz, "../testsuite/testModulationOutputReal", "../testsuite/testModulationOutputImag");
	ReadInputFromFiles(lte_phy_params->ofdemod_in, lte_phy_params->ofdemod_in_buf_sz, "/root/ResMapping/testsuite/testModulationOutputReal", "/root/ResMapping/testsuite/testModulationOutputImag");
//	ReadInputFromFiles(lte_phy_params->resdm_in, lte_phy_params->resdm_in_buf_sz, "../testsuite/testSubCarrierMapOutputReal", "../testsuite/testSubCarrierMapOutputImag");
//	GeneRandomInput(lte_phy_params->resdm_in, lte_phy_params->resdm_in_buf_sz, "../testsuite/SubCarrierDemapInputReal", "../testsuite/SubCarrierDemapInputImag");
	
//	tstart = dtime();

	ofdemodulating(lte_phy_params, lte_phy_params->ofdemod_in, lte_phy_params->ofdemod_out);
	SubCarrierDemapping(lte_phy_params, lte_phy_params->ofdemod_out, lte_phy_params->resdm_out);
	Equalizing(lte_phy_params, lte_phy_params->resdm_out, lte_phy_params->eq_out);

/*	tstop = dtime();

	ttime = (tstop - tstart);
	
	std::cout << ttime << "ms\n";
*/
	
//	WriteOutputToFiles(lte_phy_params->eq_out, lte_phy_params->eq_out_buf_sz, "../testsuite/testLSCELSEqOutputReal", "../testsuite/testLSCELSEqOutputImag");
	WriteOutputToFiles(lte_phy_params->eq_out, lte_phy_params->eq_out_buf_sz, "/root/ResMapping/testsuite/testLSCELSEqOutputReal", "/root/ResMapping/testsuite/testLSCELSEqOutputImag");

	std::cout << "Resource demapping ends" << std::endl;

}

// #define SCMapper
void test(LTE_PHY_PARAMS *lte_phy_params)
{
	geneDMRS(lte_phy_params->DMRS, lte_phy_params->N_tx_ant, lte_phy_params->N_dft_sz);
//#ifdef SCMapper
	
	test_SCMapper(lte_phy_params);

//#else

	test_SCDemapper(lte_phy_params);

	char tx_in_fname_real[100];
	char tx_in_fname_imag[100];
	char rx_out_fname_real[100];
	char rx_out_fname_imag[100];
	
	strcpy(tx_in_fname_real, "/root/ResMapping/testsuite/SubCarrierMapInputReal");
	strcpy(tx_in_fname_imag, "/root/ResMapping/testsuite/SubCarrierMapInputImag");
	strcpy(rx_out_fname_real, "/root/ResMapping/testsuite/testLSCELSEqOutputReal");
	strcpy(rx_out_fname_imag, "/root/ResMapping/testsuite/testLSCELSEqOutputImag");
	check(tx_in_fname_real, tx_in_fname_imag, rx_out_fname_real, rx_out_fname_imag);

//#endif
}
