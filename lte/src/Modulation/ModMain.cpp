
#include "ModMain.h"

//#define Mod

//int RANDOMSEED;

LTE_PHY_PARAMS lte_phy_params;


void test_mod(LTE_PHY_PARAMS *lte_phy_params, int mod_type)
{
	std::cout << "Modulation starts" << std::endl;

	ReadInputFromFiles(lte_phy_params->mod_in, lte_phy_params->mod_in_buf_sz, "ModulationInput");

	Modulating(lte_phy_params, lte_phy_params->mod_in, lte_phy_params->mod_out, mod_type);
	
	WriteOutputToFiles(lte_phy_params->mod_out, lte_phy_params->mod_out_buf_sz, "testModulationRandomOutputReal", "testModulationRandomOutputImag");

	std::cout << "Modulation ends" << std::endl;

}

void test_demod(LTE_PHY_PARAMS *lte_phy_params, int mod_type)
{
		
	std::cout << "Demodulation starts" << std::endl;

	float awgn_sigma = 0.193649; //this value is for the standard input  see "AWGNSigma"
	
//	ReadInputFromFiles(rx_demod_in, in_buf_sz, "testModulationRandomOutputReal", "testModulationRandomOutputImag");
	ReadInputFromFiles(lte_phy_params->demod_in, lte_phy_params->demod_in_buf_sz, "DemodulationInputReal", "DemodulationInputImag");

	Demodulating(lte_phy_params, lte_phy_params->demod_in, lte_phy_params->demod_LLR, mod_type, awgn_sigma);

	WriteOutputToFiles(lte_phy_params->demod_LLR, lte_phy_params->demod_out_buf_sz, "testDemodulationOutput");

	std::cout << "Demodulation ends" << std::endl;


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

#ifdef Mod

	test_mod(&lte_phy_params, mod_type);
	
	#else

	test_demod(&lte_phy_params, mod_type);

	#endif
	
	return 0;
}

