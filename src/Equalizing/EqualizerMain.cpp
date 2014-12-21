#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "lte_phy.h"
#include "GeneralFunc.h"
#include "Equalizer.h"

//int RANDOMSEED;

LTE_PHY_PARAMS lte_phy_params;

void test_equalizer(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Equalizing starts" << std::endl;

#ifdef DEBUG
	ReadInputFromFiles(lte_phy_params->eq_in, lte_phy_params->eq_in_buf_sz, "../LSCELSEqInputReal", "../LSCELSEqInputImag");
#else
	ReadInputFromFiles(lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_in_buf_sz, "../LSCELSEqInputReal", "../LSCELSEqInputImag");
#endif
//	GeneRandomInput(lte_phy_params->eq_in, lte_phy_params->eq_in_buf_sz, "LSCELSEqInputReal", "LSCELSEqInputImag");

#ifdef DEBUG
	Equalizing(lte_phy_params, lte_phy_params->eq_in, lte_phy_params->eq_out);
#else
	Equalizing(lte_phy_params, lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_out_real, lte_phy_params->eq_out_imag);
#endif
	
#ifdef DEBUG
	WriteOutputToFiles(lte_phy_params->eq_out, lte_phy_params->eq_out_buf_sz, "../testLSCELSEqOutputReal", "../testLSCELSEqOutputImag");
#else
	WriteOutputToFiles(lte_phy_params->eq_out_real, lte_phy_params->eq_out_imag, lte_phy_params->eq_out_buf_sz, "../testLSCELSEqOutputReal", "../testLSCELSEqOutputImag");
#endif

	std::cout << "Equalizing ends" << std::endl;

}

int main(int argc, char *argv[])
{
    int enum_fs;
	int n_tx_ant, n_rx_ant;
	int mod_type;

	if (argc != 5)
	{
		printf("Usage: ./a.out enum_fs mod_type n_tx_ant n_rx_ant\n");
		
		return 1;
	}
	
	enum_fs = atoi(argv[1]);
	mod_type = atoi(argv[2]);
	n_tx_ant = atoi(argv[3]);
	n_rx_ant = atoi(argv[4]);
	
	lte_phy_init(&lte_phy_params, enum_fs, mod_type, n_tx_ant, n_rx_ant);

//	for (int i = 0; i < 100; i++)
		test_equalizer(&lte_phy_params);

	return 0;
}
