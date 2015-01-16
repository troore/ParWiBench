
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "GeneralFunc.h"
#include "meas.h"
#include "check.h"
#include "micpower.h"
#include "OFDM.h"
#include "lte_phy.h"

//#define MOD

LTE_PHY_PARAMS lte_phy_params;

void test_mod(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "OFDM modulation starts" << std::endl;

//	ReadInputFromFiles(lte_phy_params->ofmod_in, lte_phy_params->ofmod_in_buf_sz, "../testsuite/ModulationInputReal", "../testsuite/ModulationInputImag");
//	ReadInputFromFiles(lte_phy_params->ofmod_in_real, lte_phy_params->ofmod_in_imag, lte_phy_params->ofmod_in_buf_sz, "../testsuite/ModulationInputReal", "../testsuite/ModulationInputImag");
//	GeneRandomInput(lte_phy_params->ofmod_in, lte_phy_params->ofmod_in_buf_sz, "../testsuite/RandomModulationInputReal", "../testsuite/RandomModulationInputImag");
	GeneRandomInput(lte_phy_params->ofmod_in_real, lte_phy_params->ofmod_in_imag, lte_phy_params->ofmod_in_buf_sz, "../testsuite/RandomModulationInputReal", "../testsuite/RandomModulationInputImag");
#ifdef __MIC__
	ofmodulating(lte_phy_params, lte_phy_params->ofmod_in, lte_phy_params->ofmod_out);
#else
	ofmodulating(lte_phy_params, lte_phy_params->ofmod_in_real, lte_phy_params->ofmod_in_imag, lte_phy_params->ofmod_out_real, lte_phy_params->ofmod_out_imag);
#endif	
//	WriteOutputToFiles(lte_phy_params->ofmod_out, lte_phy_params->ofmod_out_buf_sz, "../testsuite/testModulationOutputReal", "../testsuite/testModulationOutputImag");
	WriteOutputToFiles(lte_phy_params->ofmod_out_real, lte_phy_params->ofmod_out_imag, lte_phy_params->ofmod_out_buf_sz, "../testsuite/testModulationOutputReal", "../testsuite/testModulationOutputImag");

	std::cout << "OFDM modulation ends" << std::endl;
}

void test_demod(LTE_PHY_PARAMS *lte_phy_params)
{
	
	std::cout <<"OFDM demodulation starts"<< std::endl;

//	ReadInputFromFiles(lte_phy_params->ofdemod_in, lte_phy_params->ofdemod_in_buf_sz, "../testsuite/testModulationOutputReal", "../testsuite/testModulationOutputImag");
	ReadInputFromFiles(lte_phy_params->ofdemod_in_real, lte_phy_params->ofdemod_in_imag, lte_phy_params->ofdemod_in_buf_sz, "../testsuite/testModulationOutputReal", "../testsuite/testModulationOutputImag");
//	ReadInputFromFiles(lte_phy_params->ofdemod_in_real, lte_phy_params->ofdemod_in_imag, lte_phy_params->ofdemod_in_buf_sz, "../testsuite/DemodulationInputReal", "../testsuite/DemodulationInputImag");
#ifdef __MIC__
	ofdemodulating(lte_phy_params, lte_phy_params->ofdemod_in, lte_phy_params->ofdemod_out);
	double energy,ttime,tbegin;
	micpower_start();
	tbegin = dtime();
	for(int i = 0;i < 10000; i++)
		ofdemodulating(lte_phy_params, lte_phy_params->ofdemod_in, lte_phy_params->ofdemod_out);
#else
	//	ofdemodulating(lte_phy_params, lte_phy_params->ofdemod_in, lte_phy_params->ofdemod_out);
	ofdemodulating(lte_phy_params, lte_phy_params->ofdemod_in_real, lte_phy_params->ofdemod_in_imag, lte_phy_params->ofdemod_out_real, lte_phy_params->ofdemod_out_imag);
#endif
#ifdef __MIC__
	ttime = dtime() - tbegin;
	energy = micpower_finalize();
	printf("Energy used in %lf\n", energy);
	printf("whole time is %fms\n", ttime);
#endif
//	WriteOutputToFiles(lte_phy_params->ofdemod_out, lte_phy_params->ofdemod_out_buf_sz, "../testsuite/testDemodulationOutputReal", "../testsuite/testDemodulationOutputImag");
	WriteOutputToFiles(lte_phy_params->ofdemod_out_real, lte_phy_params->ofdemod_out_imag, lte_phy_params->ofdemod_out_buf_sz, "../testsuite/testDemodulationOutputReal", "../testsuite/testDemodulationOutputImag");
	
	std::cout << "OFDM demodulation ends" << std::endl;
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

#ifdef MOD
	
	test_mod(&lte_phy_params);
	
#else

	test_demod(&lte_phy_params);
	
	strcpy(tx_in_fname, "../testsuite/RandomModulationInputReal");
	strcpy(rx_out_fname, "../testsuite/testDemodulationOutputReal");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
	
	strcpy(tx_in_fname, "../testsuite/RandomModulationInputImag");
	strcpy(rx_out_fname, "../testsuite/testDemodulationOutputImag");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);

#endif
	
	return 0;
}

