
#include <iostream>
#include <string.h>

#include "GeneralFunc.h"
#include "timer/meas.h"
#include "check/check.h"
#include "micpower.h"

#include "Modulation.h"

void test_mod(LTE_PHY_PARAMS *lte_phy_params, int mod_type)
{
	std::cout << "Modulation starts" << std::endl;

//	ReadInputFromFiles(lte_phy_params->mod_in, lte_phy_params->mod_in_buf_sz, "ModulationInput");
	GeneRandomInput(lte_phy_params->mod_in, lte_phy_params->mod_in_buf_sz, "/home/xblee/ParWiBench/src/Modulation/testsuite/RandomModulationInput");

	Modulating_cplx(lte_phy_params, lte_phy_params->mod_in, lte_phy_params->mod_out_cplx, mod_type);
	
	WriteOutputToFiles(lte_phy_params->mod_out_cplx, lte_phy_params->mod_out_buf_sz, "/home/xblee/ParWiBench/src/Modulation/testsuite/testModulationRandomOutputReal", "/home/xblee/ParWiBench/src/Modulation/testsuite/testModulationRandomOutputImag");

	std::cout << "Modulation ends" << std::endl;

}

void test_demod(LTE_PHY_PARAMS *lte_phy_params, int mod_type)
{
		
	std::cout << "Demodulation starts" << std::endl;

	float awgn_sigma = 0.193649; //this value is for the standard input  see "AWGNSigma"

	ReadInputFromFiles(lte_phy_params->demod_in_cplx, lte_phy_params->demod_in_buf_sz, "/home/xblee/ParWiBench/src/Modulation/testsuite/testModulationRandomOutputReal", "/home/xblee/ParWiBench/src/Modulation/testsuite/testModulationRandomOutputImag");

	Demodulating_cplx(lte_phy_params, lte_phy_params->demod_in_cplx, lte_phy_params->demod_LLR, mod_type, awgn_sigma);
	double ttime,tbegin,energy;
	micpower_start();
	tbegin = dtime();

	for(int i = 0;i < 10000; i++)	
		Demodulating_cplx(lte_phy_params, lte_phy_params->demod_in_cplx, lte_phy_params->demod_LLR, mod_type, awgn_sigma);
	ttime = dtime();
	energy = micpower_finalize();
	printf("Energy used in %lf\n", energy);
	printf("whole time is %fms\n", ttime - tbegin);

	for (int i = 0; i < lte_phy_params->demod_out_buf_sz; i++)
	{
		if (lte_phy_params->demod_LLR[i] > 0)
			lte_phy_params->demod_HD[i] = 1;
		else
			lte_phy_params->demod_HD[i] = 0;
	}
	
//	WriteOutputToFiles(lte_phy_params->demod_LLR, lte_phy_params->demod_out_buf_sz, "../testsuite/testDemodulationOutput");
	WriteOutputToFiles(lte_phy_params->demod_HD, lte_phy_params->demod_out_buf_sz, "/home/xblee/ParWiBench/src/Modulation/testsuite/testDemodulationOutput");

	std::cout << "Demodulation ends" << std::endl;
}

void check()
{
	char tx_in_fname[100];
	char rx_out_fname[100];
	int err_n;
	
	strcpy(tx_in_fname, "/home/xblee/ParWiBench/src/Modulation/testsuite/RandomModulationInput");
	strcpy(rx_out_fname, "/home/xblee/ParWiBench/src/Modulation/testsuite/testDemodulationOutput");
	err_n = check_float(tx_in_fname, rx_out_fname);
	printf("%d\n", err_n);
}

//#define Mod
void test(LTE_PHY_PARAMS *lte_phy_params)
{
//#ifdef Mod

//	test_mod(lte_phy_params, lte_phy_params->mod_type);
	test_mod(lte_phy_params, 2);

//#else

//	test_demod(lte_phy_params, lte_phy_params->mod_type);
	test_demod(lte_phy_params, 2);
	check();

//#endif
}
