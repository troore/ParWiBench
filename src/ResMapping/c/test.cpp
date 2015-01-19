#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "GeneralFunc.h"
#include "timer/meas.h"
#include "check/check.h"
#include "ResMapper.h"

void test_SCMapper(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Resource mapping starts" << std::endl;

//	ReadInputFromFiles(lte_phy_params->resm_in, lte_phy_params->resm_in_buf_sz, "../testsuite/SubCarrierMapInputReal", "../testsuite/SubCarrierMapInputImag");
//	ReadInputFromFiles(lte_phy_params->resm_in_real, lte_phy_params->resm_in_imag, lte_phy_params->resm_in_buf_sz, "../testsuite/SubCarrierMapInputReal", "../testsuite/SubCarrierMapInputImag");
//	GeneRandomInput(lte_phy_params->resm_in_real, lte_phy_params->resm_in_imag, lte_phy_params->resm_in_buf_sz, "../testsuite/SubCarrierMapInputReal", "../testsuite/SubCarrierMapInputImag");
	GeneRandomInput(lte_phy_params->resm_in, lte_phy_params->resm_in_buf_sz, "../testsuite/SubCarrierMapInputReal", "../testsuite/SubCarrierMapInputImag");

//	SubCarrierMapping(lte_phy_params, lte_phy_params->resm_in_real, lte_phy_params->resm_in_imag, lte_phy_params->resm_out_real, lte_phy_params->resm_out_imag);

	SubCarrierMapping(lte_phy_params, lte_phy_params->resm_in, lte_phy_params->resm_out);

	WriteOutputToFiles(lte_phy_params->resm_out, lte_phy_params->resm_out_buf_sz, "../testsuite/testSubCarrierMapOutputReal", "../testsuite/testSubCarrierMapOutputImag");
//	WriteOutputToFiles(lte_phy_params->resm_out_real, lte_phy_params->resm_out_imag, lte_phy_params->resm_out_buf_sz, "../testsuite/testSubCarrierMapOutputReal", "../testsuite/testSubCarrierMapOutputImag");
	
	std::cout << "Resource mapping ends" << std::endl;
}

void test_SCDemapper(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Resource demapping starts" << std::endl;
	
	int in_buf_sz, out_buf_sz;

	in_buf_sz = lte_phy_params->N_tx_ant * lte_phy_params->N_fft_sz * lte_phy_params->N_symb_per_subfr;
	
	ReadInputFromFiles(lte_phy_params->resdm_in, lte_phy_params->resdm_in_buf_sz, "../testsuite/testSubCarrierMapOutputReal", "../testsuite/testSubCarrierMapOutputImag");
//	GeneRandomInput(lte_phy_params->resdm_in, lte_phy_params->resdm_in_buf_sz, "../testsuite/SubCarrierDemapInputReal", "../testsuite/SubCarrierDemapInputImag");
	
	SubCarrierDemapping(lte_phy_params, lte_phy_params->resdm_in, lte_phy_params->resdm_out);

	WriteOutputToFiles(lte_phy_params->resdm_out, lte_phy_params->resdm_out_buf_sz, "../testsuite/testSubCarrierDemapOutputReal", "../testsuite/testSubCarrierDemapOutputImag");

	std::cout << "Resource demapping ends" << std::endl;

}

void check()
{
	char tx_in_fname[50];
	char rx_out_fname[50];
	int err_n;

	// TODO: connect with Equalizer
}

// #define SCMapper
void test(LTE_PHY_PARAMS *lte_phy_params)
{
//#ifdef SCMapper
	
	test_SCMapper(lte_phy_params);

//#else

	test_SCDemapper(lte_phy_params);

//#endif
}
