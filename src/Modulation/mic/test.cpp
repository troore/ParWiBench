
#include <iostream>
#include <string.h>

#include "GeneralFunc.h"
#include "timer/meas.h"
#include "check/check.h"
#include "micpower.h"
#include "util.h"

#include "Modulation.h"

void test_mod(LTE_PHY_PARAMS *lte_phy_params, int mod_type)
{
	std::cout << "Modulation starts" << std::endl;

//	ReadInputFromFiles(lte_phy_params->mod_in, lte_phy_params->mod_in_buf_sz, "ModulationInput");
	GeneRandomInput(lte_phy_params->mod_in, lte_phy_params->mod_in_buf_sz, "/root/Modulation/testsuite/RandomModulationInput");

	Modulating(lte_phy_params, lte_phy_params->mod_in, lte_phy_params->mod_out, mod_type);
	
	WriteOutputToFiles(lte_phy_params->mod_out, lte_phy_params->mod_out_buf_sz, "/root/Modulation/testsuite/testModulationRandomOutputReal", "/root/Modulation/testsuite/testModulationRandomOutputImag");

	std::cout << "Modulation ends" << std::endl;

}

void test_demod(LTE_PHY_PARAMS *lte_phy_params, int mod_type)
{
		
	std::cout << "Demodulation starts" << std::endl;

	float awgn_sigma = 0.193649; //this value is for the standard input  see "AWGNSigma"

	ReadInputFromFiles(lte_phy_params->demod_in, lte_phy_params->demod_in_buf_sz, "/root/Modulation/testsuite/testModulationRandomOutputReal", "/root/Modulation/testsuite/testModulationRandomOutputImag");

	Demodulating(lte_phy_params, lte_phy_params->demod_in, lte_phy_params->demod_LLR, mod_type, awgn_sigma);
	
	double ttime,tbegin, tend, energy;
	micpower_start();
	tbegin = dtime();

	for(int i = 0;i < 10000; i++)	
		Demodulating(lte_phy_params, lte_phy_params->demod_in, lte_phy_params->demod_LLR, mod_type, awgn_sigma);

	tend = dtime();
	ttime = tend - tbegin;
	energy = micpower_finalize();
	printf("Energy used in %lf J\n", energy);
	printf("Whole time is %f ms\n", ttime);
	printf("Power: %f Watt\n", energy * 1e3 / ttime);

	for (int i = 0; i < lte_phy_params->demod_out_buf_sz; i++)
	{
		if (lte_phy_params->demod_LLR[i] > 0)
			lte_phy_params->demod_HD[i] = 1;
		else
			lte_phy_params->demod_HD[i] = 0;
	}
	
//	WriteOutputToFiles(lte_phy_params->demod_LLR, lte_phy_params->demod_out_buf_sz, "../testsuite/testDemodulationOutput");
	WriteOutputToFiles(lte_phy_params->demod_HD, lte_phy_params->demod_out_buf_sz, "/root/Modulation/testsuite/testDemodulationOutput");

	std::cout << "Demodulation ends" << std::endl;
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

	char tx_in_fname[100];
	char rx_out_fname[100];
		
	strcpy(tx_in_fname, "/root/Modulation/testsuite/RandomModulationInput");
	strcpy(rx_out_fname, "/root/Modulation/testsuite/testDemodulationOutput");
	check(tx_in_fname, rx_out_fname);

//#endif
}
