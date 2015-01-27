
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "lte_phy.h"
#include "GeneralFunc.h"
#include "Scrambler.h"
#include "timer/meas.h"
#include "check/check.h"
#include "micpower.h"

void test_scrambling(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "Tx scrambling starts" << std::endl;

//	ReadInputFromFiles(lte_phy_params->scramb_in, lte_phy_params->scramb_in_buf_sz, "../testsuite/ScrambleInput");
//	GeneRandomInput(lte_phy_params->scramb_in, lte_phy_params->scramb_in_buf_sz, "/home/xblee/ParWiBench/src/Scrambling/testsuite/RandomScrambleInput");
	GeneRandomInput(lte_phy_params->scramb_in, lte_phy_params->scramb_in_buf_sz, "../testsuite/RandomScrambleInput");

	GenScrambInt(lte_phy_params->scramb_seq_int, lte_phy_params->scramb_in_buf_sz);
	Scrambling(lte_phy_params, lte_phy_params->scramb_in, lte_phy_params->scramb_out);

//	WriteOutputToFiles(lte_phy_params->scramb_out, lte_phy_params->scramb_out_buf_sz, "/home/xblee/ParWiBench/src/Scrambling/testsuite/testScrambleOutput");
	WriteOutputToFiles(lte_phy_params->scramb_out, lte_phy_params->scramb_out_buf_sz, "../testsuite/testScrambleOutput");

	std::cout << "Tx scrambling ends" << std::endl;
}

void test_descrambling(LTE_PHY_PARAMS *lte_phy_params)
{
	
	std::cout << "Rx descrambling starts" << std::endl;

	float *rx_scramb_in = (float *)malloc(lte_phy_params->descramb_in_buf_sz * sizeof(float));
	float *rx_scramb_out = (float *)malloc(lte_phy_params->descramb_out_buf_sz * sizeof(float));
	int i;

//	ReadInputFromFiles(lte_phy_params->descramb_in, lte_phy_params->descramb_in_buf_sz, "/home/xblee/ParWiBench/src/Scrambling/testsuite/testScrambleOutput");
	ReadInputFromFiles(lte_phy_params->descramb_in, lte_phy_params->descramb_in_buf_sz, "../testsuite/testScrambleOutput");
//	ReadInputFromFiles(rx_scramb_in, in_buf_sz, "testScrambleOutput");
	
	for (i = 0; i < lte_phy_params->descramb_in_buf_sz; i++)
	{
		if (lte_phy_params->descramb_in[i] == 0)
			rx_scramb_in[i] = -1.0;
		else
			rx_scramb_in[i] = 1.0;
	 }
	GenScrambInt(lte_phy_params->scramb_seq_int, lte_phy_params->scramb_in_buf_sz);
	Descrambling(lte_phy_params, rx_scramb_in, rx_scramb_out);
	double energy,ttime,tbegin;
	micpower_start();
	tbegin = dtime();
	//	Descrambling(lte_phy_params, lte_phy_params->descramb_in, lte_phy_params->descramb_out);
	for(i = 0;i < 100000; i++)
		Descrambling(lte_phy_params, rx_scramb_in, rx_scramb_out);
	ttime = dtime() - tbegin;
	energy = micpower_finalize();
	printf("Energy used in %lf\n", energy);
	printf("whole time is %fms\n", ttime);
	for (i = 0; i < lte_phy_params->descramb_out_buf_sz; i++)
	{
		if (rx_scramb_out[i] > 0)
			lte_phy_params->descramb_out[i] = 1.0;
		else
			lte_phy_params->descramb_out[i] = 0.0;
	}
	
//	WriteOutputToFiles(lte_phy_params->descramb_out, lte_phy_params->descramb_out_buf_sz, "/home/xblee/ParWiBench/src/Scrambling/testsuite/testDescrambleOutput");
	WriteOutputToFiles(lte_phy_params->descramb_out, lte_phy_params->descramb_out_buf_sz, "../testsuite/testDescrambleOutput");
	
	cout << "Rx descrambling ends" << endl;
}

void check()
{	
	char tx_in_fname[100];
	char rx_out_fname[100];
	int err_n;
	
//	strcpy(tx_in_fname, "/home/xblee/ParWiBench/src/Scrambling/testsuite/RandomScrambleInput");
//	strcpy(rx_out_fname, "/home/xblee/ParWiBench/src/Scrambling/testsuite/testDescrambleOutput");
	strcpy(tx_in_fname, "../testsuite/RandomScrambleInput");
	strcpy(rx_out_fname, "../testsuite/testDescrambleOutput");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
}

//#define Scramb
void test(LTE_PHY_PARAMS *lte_phy_params)
{
//#ifdef Scramb
	test_scrambling(lte_phy_params);
//#else
	test_descrambling(lte_phy_params);
	check();
//#endif
}
