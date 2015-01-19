
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "GeneralFunc.h"
#include "timer/meas.h"
#include "check/check.h"
//#include "micpower.h"
#include "OFDM.h"
#include "lte_phy.h"

void test_mod(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "OFDM modulation starts" << std::endl;

//	ReadInputFromFiles(lte_phy_params->ofmod_in, lte_phy_params->ofmod_in_buf_sz, "../testsuite/ModulationInputReal", "../testsuite/ModulationInputImag");
//	ReadInputFromFiles(lte_phy_params->ofmod_in_real, lte_phy_params->ofmod_in_imag, lte_phy_params->ofmod_in_buf_sz, "../testsuite/ModulationInputReal", "../testsuite/ModulationInputImag");

//	GeneRandomInput(lte_phy_params->ofmod_in_real, lte_phy_params->ofmod_in_imag, /*lte_phy_params->ofmod_in_buf_sz*/128, "../testsuite/RandomModulationInputReal", "../testsuite/RandomModulationInputImag");
	GeneRandomInput(lte_phy_params->ofmod_in, lte_phy_params->ofmod_in_buf_sz, "../testsuite/RandomModulationInputReal", "../testsuite/RandomModulationInputImag");
	
//	ofmodulating(lte_phy_params, lte_phy_params->ofmod_in_real, lte_phy_params->ofmod_in_imag, lte_phy_params->ofmod_out_real, lte_phy_params->ofmod_out_imag);
	ofmodulating(lte_phy_params, lte_phy_params->ofmod_in, lte_phy_params->ofmod_out);
	
//	WriteOutputToFiles(lte_phy_params->ofmod_out_real, lte_phy_params->ofmod_out_imag, /*lte_phy_params->ofmod_out_buf_sz*/137, "../testsuite/testModulationOutputReal", "../testsuite/testModulationOutputImag");
	WriteOutputToFiles(lte_phy_params->ofmod_out, lte_phy_params->ofmod_out_buf_sz, "../testsuite/testModulationOutputReal", "../testsuite/testModulationOutputImag");

	std::cout << "OFDM modulation ends" << std::endl;
}

void test_demod(LTE_PHY_PARAMS *lte_phy_params)
{
	
	std::cout <<"OFDM demodulation starts"<< std::endl;

//	ReadInputFromFiles(lte_phy_params->ofdemod_in_real, lte_phy_params->ofdemod_in_imag, lte_phy_params->ofdemod_in_buf_sz, "../testsuite/testModulationOutputReal", "../testsuite/testModulationOutputImag");
	ReadInputFromFiles(lte_phy_params->ofdemod_in, lte_phy_params->ofdemod_in_buf_sz, "../testsuite/testModulationOutputReal", "../testsuite/testModulationOutputImag");

	ofdemodulating(lte_phy_params, lte_phy_params->ofdemod_in, lte_phy_params->ofdemod_out);
//	ofdemodulating(lte_phy_params, lte_phy_params->ofdemod_in_real, lte_phy_params->ofdemod_in_imag, lte_phy_params->ofdemod_out_real, lte_phy_params->ofdemod_out_imag);
	WriteOutputToFiles(lte_phy_params->ofdemod_out, lte_phy_params->ofdemod_out_buf_sz, "../testsuite/testDemodulationOutputReal", "../testsuite/testDemodulationOutputImag");
//	WriteOutputToFiles(lte_phy_params->ofdemod_out_real, lte_phy_params->ofdemod_out_imag, lte_phy_params->ofdemod_out_buf_sz, "../testsuite/testDemodulationOutputReal", "../testsuite/testDemodulationOutputImag");
	
	std::cout << "OFDM demodulation ends" << std::endl;
}

void check()
{
	
	char tx_in_fname[50];
	char rx_out_fname[50];
	int err_n;
		
	strcpy(tx_in_fname, "../testsuite/RandomModulationInputReal");
	strcpy(rx_out_fname, "../testsuite/testDemodulationOutputReal");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
	
	strcpy(tx_in_fname, "../testsuite/RandomModulationInputImag");
	strcpy(rx_out_fname, "../testsuite/testDemodulationOutputImag");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
}

//#define MOD
void test(LTE_PHY_PARAMS *lte_phy_params)
{
//#ifdef MOD
	
	test_mod(lte_phy_params);
	
//#else

	test_demod(lte_phy_params);
	check();

//#endif
	
}
