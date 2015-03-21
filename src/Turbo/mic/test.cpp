
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "Turbo.h"
#include "Turbo_pardec.h"
#include "lte_phy.h"
#include "GeneralFunc.h"
#include "timer/meas.h"
#include "check/check.h"
#include "micpower.h"
#include "util.h"

void test_turbo_encoding(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Turbo Encoder starts" << std::endl;
	
//	ReadInputFromFiles(lte_phy_params->te_in, lte_phy_params->te_in_buf_sz, "../TurboEncoderInput");
	GeneRandomInput(lte_phy_params->te_in, lte_phy_params->te_in_buf_sz, "/root/Turbo/testsuite/TurboEncoderInput");

	/*
	for (int i = 0; i < lte_phy_params->te_in_buf_sz; i++)
		lte_phy_params->te_in[i] = 1;
	*/

	turbo_encoding(lte_phy_params, lte_phy_params->te_in, lte_phy_params->te_out);

	WriteOutputToFiles(lte_phy_params->te_out, lte_phy_params->te_out_buf_sz, "/root/Turbo/testsuite/testTurboEncoderOutput");

	std::cout << "Turbo Encoder ends" << std::endl;
}

void test_turbo_decoding(LTE_PHY_PARAMS *lte_phy_params, int n_iters)
{
	std::cout << "Turbo Decoder starts" << std::endl;
	
	ReadInputFromFiles(lte_phy_params->td_in, lte_phy_params->td_in_buf_sz, "/root/Turbo/testsuite/testTurboEncoderOutput");
	
	for (int i = 0; i < lte_phy_params->td_in_buf_sz; i++)
	{
		lte_phy_params->td_in[i] = (1 - 2 * lte_phy_params->td_in[i]);
	}

	turbo_decoding(lte_phy_params, lte_phy_params->td_in, lte_phy_params->td_out, n_iters);
//	double tbegin,ttime,energy;
//	micpower_start();
//	tbegin = dtime();

//	for(int i=0;i<1;i++)
//		turbo_decoding(lte_phy_params, lte_phy_params->td_in, lte_phy_params->td_out, n_iters);
//	ttime = dtime() - tbegin;
//	energy = micpower_finalize();
//	printf("Energy used in %lf\n", energy);
//	printf("real time is %fms\n", ttime);
//	printf("%lf\t%f\t%f\n", energy, ttime, (energy * 1000.0) / ttime);

	for (int i = 0; i < lte_phy_params->td_out_buf_sz; i++)
	{
		lte_phy_params->td_out[i] = (1 - lte_phy_params->td_out[i]) / 2;
	}
	
	WriteOutputToFiles(lte_phy_params->td_out, lte_phy_params->td_out_buf_sz, "/root/Turbo/testsuite/testTurboDecoderOutput");
	
	std::cout << "Turbo Decoder ends" << std::endl;
}

//#define TurboEnc
void test(LTE_PHY_PARAMS *lte_phy_params)
{
	
//#ifdef TurboEnc

	test_turbo_encoding(lte_phy_params);
	
//#else

	int n_iters = 1;
	
	test_turbo_decoding(lte_phy_params, n_iters);

	char tx_in_fname[100];
	char rx_out_fname[100];

	strcpy(tx_in_fname, "/root/Turbo/testsuite/TurboEncoderInput");
	strcpy(rx_out_fname, "/root/Turbo/testsuite/testTurboDecoderOutput");
	check(tx_in_fname, rx_out_fname);
//#endif
}
