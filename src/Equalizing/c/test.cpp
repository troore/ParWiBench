
#include <iostream>
#include <stdlib.h>

#include "lte_phy.h"
#include "refs/dmrs.h"
#include "GeneralFunc.h"
#include "Equalizer.h"
#include "timer/meas.h"
#include "check/check.h"

void test_equalizer(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Equalizing starts" << std::endl;
	
//	GeneRandomInput(lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
//	ReadInputFromFiles(lte_phy_params->eq_in, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
//	ReadInputFromFiles(lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
	GeneRandomInput(lte_phy_params->eq_in, lte_phy_params->eq_in_buf_sz, "LSCELSEqInputReal", "LSCELSEqInputImag");

	geneDMRS(lte_phy_params->DMRS, lte_phy_params->N_tx_ant, lte_phy_params->N_dft_sz);
	Equalizing(lte_phy_params, lte_phy_params->eq_in, lte_phy_params->eq_out);
//	geneDMRS(lte_phy_params->DMRSReal, lte_phy_params->DMRSImag, lte_phy_params->N_tx_ant, lte_phy_params->N_dft_sz);
//	Equalizing(lte_phy_params, lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_out_real, lte_phy_params->eq_out_imag);
	
	WriteOutputToFiles(lte_phy_params->eq_out, lte_phy_params->eq_out_buf_sz, "../testsuite/testLSCELSEqOutputReal", "../testsuite/testLSCELSEqOutputImag");
//	WriteOutputToFiles(lte_phy_params->eq_out_real, lte_phy_params->eq_out_imag, lte_phy_params->eq_out_buf_sz, "../testsuite/testLSCELSEqOutputReal", "../testsuite/testLSCELSEqOutputImag");

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

inline static int num_flops_matrixprod(int d1, int d2, int d3)
{
	return (6 * d1 * d2 * d3);
}

inline static int num_flops_matrixinv(int d)
{
	return (28 * d * d + 16 * d * d * (d - 1));
}

double gflop_counter(LTE_PHY_PARAMS *lte_phy_params)
{
	int n_dft = lte_phy_params->N_dft_sz;
	int n_tx_ant = lte_phy_params->N_tx_ant;
	int n_rx_ant = lte_phy_params->N_rx_ant;
	int n_symbs_sf = LTE_PHY_N_SYMB_PER_SUBFR;
	
	double cnter = 0.0;

	cnter = cnter + num_flops_matrixprod(n_tx_ant, 2, n_tx_ant) + num_flops_matrixprod(n_tx_ant, 2, n_rx_ant)
		+ num_flops_matrixprod(n_tx_ant, n_tx_ant, n_rx_ant) + num_flops_matrixinv(n_tx_ant);
	cnter = cnter + num_flops_matrixprod(n_tx_ant, n_rx_ant, n_tx_ant) + num_flops_matrixinv(n_tx_ant)
		+ (n_symbs_sf - 2) * (num_flops_matrixprod(n_tx_ant, n_rx_ant, 1) + num_flops_matrixprod(n_tx_ant, n_tx_ant, 1));

	return 1.0e-9 * cnter;
}
