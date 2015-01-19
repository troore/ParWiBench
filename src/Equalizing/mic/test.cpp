
#include <iostream>
#include <stdlib.h>

#include "lte_phy.h"
#include "GeneralFunc.h"
#include "Equalizer.h"
#include "timer/meas.h"
#include "check/check.h"
#include "micpower.h"

void test_equalizer(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Equalizing starts" << std::endl;
	
//	GeneRandomInput(lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
//	ReadInputFromFiles(lte_phy_params->eq_in, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
//	ReadInputFromFiles(lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
	GeneRandomInput(lte_phy_params->eq_in, lte_phy_params->eq_in_buf_sz, "LSCELSEqInputReal", "LSCELSEqInputImag");

#ifdef __MIC__
	Equalizing(lte_phy_params, lte_phy_params->eq_in, lte_phy_params->eq_out);
	double energy,ttime,tbegin;
	micpower_start();
	tbegin = dtime();
	for(int i=0;i<10000;i++)
#endif
	Equalizing(lte_phy_params, lte_phy_params->eq_in, lte_phy_params->eq_out);
//	Equalizing(lte_phy_params, lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_out_real, lte_phy_params->eq_out_imag);
#ifdef __MIC__
	ttime = dtime() - tbegin;
        energy = micpower_finalize();
        printf("Energy used in %lf\n", energy);
        printf("whole time is %fms\n", ttime);
#endif	
//	WriteOutputToFiles(lte_phy_params->eq_out_real, lte_phy_params->eq_out_imag, lte_phy_params->eq_out_buf_sz, "../testsuite/testLSCELSEqOutputReal", "../testsuite/testLSCELSEqOutputImag");
	WriteOutputToFiles(lte_phy_params->eq_out, lte_phy_params->eq_out_buf_sz, "../testsuite/testLSCELSEqOutputReal", "../testsuite/testLSCELSEqOutputImag");
	std::cout << "Equalizing ends" << std::endl;

}


void check()
{
	char tx_in_fname[50];
	char rx_out_fname[50];
	int err_n;

	// TODO: connect with OFDM
}

void test(LTE_PHY_PARAMS *lte_phy_params)
{
	test_equalizer(lte_phy_params);
	check();
}

