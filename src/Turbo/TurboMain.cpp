
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "Turbo.h"
#include "Turbo_pardec.h"
#include "GeneralFunc.h"
#include "gauss.h"
#include "meas.h"
#include "lte_phy.h"
#include "check.h"
#ifdef __MIC__
#include "micpower.h"
#endif
//#define TurboEnc

LTE_PHY_PARAMS lte_phy_params;

void test_turbo_encoding(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Turbo Encoder starts" << std::endl;
	
//	ReadInputFromFiles(lte_phy_params->te_in, lte_phy_params->te_in_buf_sz, "../TurboEncoderInput");
	GeneRandomInput(lte_phy_params->te_in, lte_phy_params->te_in_buf_sz, "../testsuite/TurboEncoderInput");

	/*
	for (int i = 0; i < lte_phy_params->te_in_buf_sz; i++)
		lte_phy_params->te_in[i] = 1;
	*/

	turbo_encoding(lte_phy_params, lte_phy_params->te_in, lte_phy_params->te_out);

	WriteOutputToFiles(lte_phy_params->te_out, lte_phy_params->te_out_buf_sz, "../testsuite/testTurboEncoderOutput");

	std::cout << "Turbo Encoder ends" << std::endl;
}

void test_turbo_decoding(LTE_PHY_PARAMS *lte_phy_params, int n_iters)
{
	std::cout << "Turbo Decoder starts" << std::endl;
	
	ReadInputFromFiles(lte_phy_params->td_in, lte_phy_params->td_in_buf_sz, "../testsuite/testTurboEncoderOutput");
	
	for (int i = 0; i < lte_phy_params->td_in_buf_sz; i++)
	{
		lte_phy_params->td_in[i] = (1 - 2 * lte_phy_params->td_in[i]);
	}
#ifdef __MIC__
	turbo_decoding(lte_phy_params, lte_phy_params->td_in, lte_phy_params->td_out, n_iters);
	double tbegin,ttime,energy;
	micpower_start();
	tbegin = dtime();

	for(int i=0;i<1000;i++)
	turbo_decoding(lte_phy_params, lte_phy_params->td_in, lte_phy_params->td_out, n_iters);
	ttime = dtime();
	energy = micpower_finalize();
	printf("Energy used in %lf\n", energy);
	printf("real time is %fms\n", ttime - tbegin);
#else
	turbo_decoding(lte_phy_params, lte_phy_params->td_in, lte_phy_params->td_out, n_iters);
#endif	
	for (int i = 0; i < lte_phy_params->td_out_buf_sz; i++)
	{
		lte_phy_params->td_out[i] = (1 - lte_phy_params->td_out[i]) / 2;
	}
	
	WriteOutputToFiles(lte_phy_params->td_out, lte_phy_params->td_out_buf_sz, "../testsuite/testTurboDecoderOutput");
	
	std::cout << "Turbo Decoder ends" << std::endl;
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

#ifdef TurboEnc

	test_turbo_encoding(&lte_phy_params);
	
#else

	int n_iters = 1;
	
	test_turbo_decoding(&lte_phy_params, n_iters);

	strcpy(tx_in_fname, "../testsuite/TurboEncoderInput");
	strcpy(rx_out_fname, "../testsuite/testTurboDecoderOutput");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);

#endif
	
	return 0;
}




