
#include "OFDMMain.h"


//#define MOD

//int RANDOMSEED;

LTE_PHY_PARAMS lte_phy_params;

void test_mod(LTE_PHY_PARAMS *lte_phy_params)
{
	std::cout << "OFDM modulation starts" << std::endl;

//	ReadInputFromFiles(lte_phy_params->ofmod_in, lte_phy_params->ofmod_in_buf_sz, "../testsuite/ModulationInputReal", "../testsuite/ModulationInputImag");
	GeneRandomInput(lte_phy_params->ofmod_in, lte_phy_params->ofmod_in_buf_sz, "../testsuite/RandomModulationInputReal", "../testsuite/RandomModulationInputImag");

	ofmodulating(lte_phy_params, lte_phy_params->ofmod_in, lte_phy_params->ofmod_out);
	
	WriteOutputToFiles(lte_phy_params->ofmod_out, lte_phy_params->ofmod_out_buf_sz, "../testsuite/testModulationOutputReal", "../testsuite/testModulationOutputImag");

	std::cout << "OFDM modulation ends" << std::endl;
}

void test_demod(LTE_PHY_PARAMS *lte_phy_params)
{
	
	std::cout <<"OFDM demodulation starts"<< std::endl;

//	ReadInputFromFiles(rx_demod_in, in_buf_sz, "DemodulationInputReal", "DemodulationInputImag");
	ReadInputFromFiles(lte_phy_params->ofdemod_in, lte_phy_params->ofdemod_in_buf_sz, "../testsuite/testModulationOutputReal", "../testsuite/testModulationOutputImag");

	ofdemodulating(lte_phy_params, lte_phy_params->ofdemod_in, lte_phy_params->ofdemod_out);

	WriteOutputToFiles(lte_phy_params->ofdemod_out, lte_phy_params->ofdemod_out_buf_sz, "../testsuite/testDemodulationOutputReal", "../testsuite/testDemodulationOutputImag");
	
	std::cout << "OFDM demodulation ends" << std::endl;
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

//	#ifdef MOD
	
	test_mod(&lte_phy_params);
	
//	#else

	test_demod(&lte_phy_params);
	
//	#endif
	
	return 0;
}

