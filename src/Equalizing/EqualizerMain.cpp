#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "lte_phy.h"
#include "GeneralFunc.h"
#include "Equalizer.h"
#include "timer/meas.h"
#include "check/check.h"
#include "micpower.h"
//int RANDOMSEED;

LTE_PHY_PARAMS lte_phy_params;

void test_equalizer(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Equalizing starts" << std::endl;
	GeneRandomInput(lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
//	ReadInputFromFiles(lte_phy_params->eq_in, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
//	ReadInputFromFiles(lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_in_buf_sz, "../testsuite/LSCELSEqInputReal", "../testsuite/LSCELSEqInputImag");
//	GeneRandomInput(lte_phy_params->eq_in, lte_phy_params->eq_in_buf_sz, "LSCELSEqInputReal", "LSCELSEqInputImag");
//<<<<<<< HEAD

//	Equalizing(lte_phy_params, lte_phy_params->eq_in, lte_phy_params->eq_out);
//	Equalizing(lte_phy_params, lte_phy_params->eq_in_real, lte_phy_params->eq_in_imag, lte_phy_params->eq_out_real, lte_phy_params->eq_out_imag);
	
//	WriteOutputToFiles(lte_phy_params->eq_out, lte_phy_params->eq_out_buf_sz, "../testsuite/testLSCELSEqOutputReal", "../testsuite/testLSCELSEqOutputImag");
//	WriteOutputToFiles(lte_phy_params->eq_out_real, lte_phy_params->eq_out_imag, lte_phy_params->eq_out_buf_sz, "../testsuite/testLSCELSEqOutputReal", "../testsuite/testLSCELSEqOutputImag");
//======= reserve for hibbert
	
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

int main(int argc, char *argv[])
{
	int fs_id;
	int n_tx_ant, n_rx_ant;
	int mod_type;

	char tx_in_fname[50];
	char rx_out_fname[50];
	int err_n;
	
	if (argc != 5 && argc != 2)
	{
		lte_phy_usage_info(argv[0]);
		
		return 1;
	}

	if (argc == 5)
	{
		fs_id = atoi(argv[1]);
		mod_type = atoi(argv[2]);
		n_tx_ant = atoi(argv[3]);
		n_rx_ant = atoi(argv[4]);

		lte_phy_init(&lte_phy_params, fs_id, mod_type, n_tx_ant, n_rx_ant);
	}
	else
	{
		fs_id = atoi(argv[1]);

		lte_phy_init(&lte_phy_params, fs_id);
	}

//	for (int i = 0; i < 100; i++)
		test_equalizer(&lte_phy_params);

		/*
		strcpy(tx_in_fname, "../testsuite/transformprecoderinputreal");
		strcpy(rx_out_fname, "../testsuite/testtransformdecoderoutputreal");
		err_n = check_float(tx_in_fname, rx_out_fname);
		printf("%d\n", err_n);
	
		strcpy(tx_in_fname, "../testsuite/TransformPrecoderInputImag");
		strcpy(rx_out_fname, "../testsuite/testTransformDecoderOutputImag");
		err_n = check_float(tx_in_fname, rx_out_fname);
		printf("%d\n", err_n);
		*/



	return 0;
}
